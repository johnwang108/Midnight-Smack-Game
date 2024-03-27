#include "Enemy.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include "../PFGameScene.h"



using namespace cugl;

#pragma mark -
#pragma mark Physics Constants

// Constants for enemy's behavior (e.g., movement speed, jump height) are defined here

#pragma mark -
#pragma mark Constructors

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type) {
    return init(pos, size, scale, type, EnemyModel::defaultSeq(type), EnemyModel::defaultSeq(type));
}

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    Size scaledSize = size;
    scaledSize.width *= ENEMY_HSHRINK;
    scaledSize.height *= ENEMY_VSHRINK;
    _drawScale = scale;
    _type = type;

    if (CapsuleObstacle::init(pos, scaledSize)) {
        setDensity(ENEMY_DENSITY);
        setFriction(0.0f); // Prevent sticking to walls
        if (_type != EnemyType::shrimp){
            setFixedRotation(true); // Avoid tumbling
        }
        else {
            setFixedRotation(false);
            setFriction(1.0f);
        }
        _isChasing = false;
        _isGrounded = false;
        _direction = -1; 
        _lastDirection = _direction;
        _changeDirectionInterval = 3.0f;
        _nextChangeTime = _changeDirectionInterval + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * _changeDirectionInterval;
        _health = 100.0f;
        _healthCooldown = 0.2f;
        _lastDamageTime = 0;
        _attacktime = false;
        _preparetime= 0;
        _shooted = false;
        _vulnerable = false;
        _state = "patrolling";
        setGestureSeq1(seq1);
        setGestureSeq2(seq2);
        b2Filter filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);
        return true;
    }

    return false;
}

#pragma mark -
#pragma mark Static Constructors

std::shared_ptr<EnemyModel> EnemyModel::alloc(const Vec2& pos, const Size& size, float scale, EnemyType type) {
    std::shared_ptr<EnemyModel> result = std::make_shared<EnemyModel>();
    return (result->init(pos, size, scale, type) ? result : nullptr);
}

#pragma mark -
#pragma mark Physics Methods

void EnemyModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();


    b2FixtureDef sensorDef;
    sensorDef.density = 0;  
    sensorDef.isSensor = true;  

    b2PolygonShape sensorShape;
    b2Vec2 sensorVertices[4];
    sensorVertices[0].Set(-getWidth() * ENEMY_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[1].Set(getWidth() * ENEMY_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[2].Set(getWidth() * ENEMY_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorVertices[3].Set(-getWidth() * ENEMY_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorShape.Set(sensorVertices, 4);

    sensorDef.shape = &sensorShape;

    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


    _sensorFixture = _body->CreateFixture(&sensorDef);
    b2Filter filter = getFilterData();
    filter.groupIndex = -1;
    _sensorFixture->SetFilterData(filter);


}

void EnemyModel::takeDamage(float damage, const int attackDirection) {
    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime= 0;
        _health -= damage;
        if (_health < 0) {
            _health = 0;
        }
        else {
            if (_health == 1) {
                setVulnerable(true);
            }
            b2Vec2 impulse = b2Vec2(-attackDirection * 3, 10);
            _body->SetLinearVelocity(impulse);
            setState("stunned");
            _knockbackTime = 1;
        }
    }
}

void EnemyModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }

}

#pragma mark -
#pragma mark Gameplay Methods

//Todo: make deterministic
void EnemyModel::update(float dt) {
    CapsuleObstacle::update(dt);
    if (_body == nullptr) {
        return;
    }

    //updating counters
    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }
    else if (_preparetime > 0) {
        if (_preparetime < 1 && _shooted) {
            _attacktime = true;
        }
        _preparetime -= dt;
        _body->SetLinearVelocity(b2Vec2(0, 0));
        if (_node != nullptr) {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        return;
    }
    //set behaviors
    if (_behaviorCounter > 0) {
        _behaviorCounter -= 1;
    }
    else if (_behaviorCounter == 0 || (_behaviorCounter == -1 && getNextState(_state) != _state)) {
        setState(getNextState(_state));
    }

    b2Vec2 velocity = _body->GetLinearVelocity();
    //   if (_state == "chasing") {
    //       _node->setColor(Color4::BLACK);
    //   }
    //   else if (_state == "rolling"){
    //       _node->setColor(Color4::RED);
    //   }
    //   else if (_state == "patrolling") {
    //       _node->setColor(Color4::WHITE);
    //   }
    //   else if (_state == "stunned") {
       //	_node->setColor(Color4::GRAY);
       //}
    //   else if (_state == "spitting") {
       //	_node->setColor(Color4::YELLOW);
       //}
    //   else {

//}
    //handle type specific behavior
    switch (getType()) {
    case EnemyType::shrimp:
        if (_state == "chasing") {
            velocity.x = ENEMY_FORCE * _direction / 3;
        }
        else if (_state == "rolling") {
            float spd = CHASE_SPEED * _direction;
            if (SIGNUM(_direction) != SIGNUM(velocity.x)) {
                spd *= 2.5;
			}
            velocity.x = velocity.x + spd;
            _body->ApplyAngularImpulse((_direction * -0.05f), true);
        }
        else if (_state == "stunned") {
			velocity.x = 0;
        }
        else if (_state == "patrolling") {
			velocity.x = ENEMY_FORCE * _direction;

		}
        else {
            CULog("error: shrimp");
        }
        break;

    case EnemyType::rice:
        if (_state == "chasing") {
            velocity.x = ENEMY_FORCE * _direction * 2;
        }
        else if (_state == "stunned") {
            velocity.x = 0;
        }
        else if (_state == "patrolling") {
            velocity.x = ENEMY_FORCE * _direction;
        }
        else {
            CULog("error: rice");
        }
        break;
    case EnemyType::egg:
        if (_state == "chasing") {
            velocity.x = ENEMY_FORCE * _direction;
        }
        else if (_state == "stunned") {
            velocity.x = 0;
        }
        else if (_state == "windup") {
            velocity.x = 0;
        }
        else if (_state == "spitting") {
            velocity.x = 0;
            setattacktime(true);
        }
        else if (_state == "patrolling") {
            velocity.x = ENEMY_FORCE * _direction;
        }
        else if (_state == "short_windup") {
            velocity.x = 0;
        }
        else {
            CULog("error: egg");
            CULog(_state.c_str());
        }
        break;
    case EnemyType::carrot:
        break;
    case EnemyType::beef:
        break;
    }
    
    //_body->SetLinearVelocity(handleMovement(velocity));
    _body->SetLinearVelocity(handleMovement(velocity));

    //if (_isGrounded) {
    //    b2Vec2 velocity = _body->GetLinearVelocity();
    //    velocity.x = ENEMY_FORCE;

    //    // Reverse direction at edges or obstacles
    //    if (velocity.x > ENEMY_MAXSPEED) {
    //        velocity.x = ENEMY_MAXSPEED;
    //        _direction = -_direction;
    //    }
    //    else if (velocity.x < -ENEMY_MAXSPEED) {
    //        velocity.x = -ENEMY_MAXSPEED;
    //        _direction = -_direction;
    //    }
    //    _nextChangeTime -= dt;
    //    if (isChasing()) {
    //        if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < ENEMY_ATTACK_CHANCE) {
    //            _preparetime = 2.5;
    //            _shooted = true;
    //        }
    //        velocity.x *= CHASE_SPEED;
    //    }
    //    else if (!isChasing() && _nextChangeTime <= 0) {
    //        _direction = (rand() % 2) * 2 - 1;
    //        _nextChangeTime = _changeDirectionInterval + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * _changeDirectionInterval;
    //    }

    //    if (_direction != _lastDirection) {
    //        // If direction changed, flip the image
    //        scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
    //        if (image != nullptr) {
    //            image->flipHorizontal(!image->isFlipHorizontal());
    //        }

    //    }


    //    velocity.x *= _direction;
    //    _lastDirection = _direction; // Update last direction

    //    _body->SetLinearVelocity(velocity);
    //}
    _lastDamageTime+= dt;
    

    // Update scene node position and rotation to match physics body
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

}

/**This function handles movement and behavior that are generic across enemy types. These are independent of dt*/
b2Vec2 EnemyModel::handleMovement(b2Vec2 velocity) {
    //clamp velocity
    if (velocity.x > ENEMY_MAXSPEED) {
        velocity.x = ENEMY_MAXSPEED;
    }
    else if (velocity.x < -ENEMY_MAXSPEED) {
        velocity.x = -ENEMY_MAXSPEED;
    }
    if (_isGrounded) {
        if (_direction != _lastDirection) {
            // If direction changed, flip the image
            scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
            if (image != nullptr) {
                image->flipHorizontal(!image->isFlipHorizontal());
            }
        }
        _lastDirection = _direction; // Update last direction
    }
    return velocity;
}


void EnemyModel::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    _node->setPosition(getPosition() * _drawScale);
}

void EnemyModel::dispose() {
    _core = nullptr;
    _node = nullptr;
}

//when called, it will return a tuple <attack, node> of the attack that the enemy will create, using the given texture
std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> EnemyModel::createAttack(std::shared_ptr<AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE_L);
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(image->getSize().width / scale,
            ATTACK_H * image->getSize().height / scale));

    pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += ATTACK_OFFSET_Y;


    if (getDirection() > 0) {
		attack->setFaceRight(true);
	}
    attack->setName("enemy_attack");
    attack->setBullet(true);
    attack->setGravityScale(0.2);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(scale);
    attack->setstraight(_distanceToPlayer + getPosition());
    attack->setEnabled(true);
    attack->setrand(false);
    attack->setSpeed(30.0f);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    return std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>>(attack, sprite);

    /*std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);*/
}

//begins the aggro behavior, maintains player location information
void EnemyModel::setIsChasing(bool isChasing) { 
    _isChasing = isChasing; 
    if (isChasing) {
        if (_state == "patrolling") {
            setState("chasing");
        }
	}
    else {
        setState("patrolling");
	}
}

void EnemyModel::updatePlayerDistance(cugl::Vec2 playerPosition) {
	_distanceToPlayer = playerPosition - getPosition();
    //CULog("Distance to player: %f", _distanceToPlayer.length());
}

/**Sets the nextstate and also sets how long the enemy stays in that state. Counters set to -1 if the state will transition away based off of something else.*/
void EnemyModel::setState(std::string state) {
    _state = state;
    switch (_type) {
        case EnemyType::shrimp:
            if (state == "chasing") {
                _behaviorCounter = 30;
            }
            else if (state == "rolling") {
                _behaviorCounter = -1;
            }
            else if (state == "stunned") {
                _behaviorCounter = 60;
            }
            else if (state == "patrollling") {
                _behaviorCounter = -1;
            }
			break;
        case EnemyType::egg:
            if (state == "chasing") {
                _behaviorCounter = -1;
            }
            else if (state == "windup") {
                _behaviorCounter = 300;
            }
            else if (state == "spitting") {
                _behaviorCounter = 1;
            }
            else if (state == "short_windup") {
                _behaviorCounter = 180;
            }
            else if (state == "stunned") {
                _behaviorCounter = 30;
            }
            else if (state == "patrollling") {
                _behaviorCounter = -1;
            }
            break;
        case EnemyType::rice:
            if (state == "yelling") {
                _behaviorCounter = 30;
            }
            else if (state == "stunned") {
                _behaviorCounter = 60;
            }
            else if (state == "patrollling") {
                _behaviorCounter = -1;
            }
            break;
        default:
            _behaviorCounter = -1;
            break;
    }
}

std::string EnemyModel::getNextState(std::string state) {
    switch (_type) {
		case EnemyType::shrimp:
            if (state == "chasing") {
				return "rolling";
			}
            else if (state == "rolling") {
                return "rolling";
			}
            else if (state == "stunned") {
                return "chasing";
            }
            else if (state == "patrolling") {
				return "patrolling";
			}
			break;
        case EnemyType::rice:
            if (state == "chasing") {
                return "chasing";
            }
            else if (state == "yelling") {
                return "chasing";
            }
            else if (state == "stunned") {
                return "chasing";
            }
            else if (state == "patrolling") {
                return "patrolling";
            }
            break;
        case EnemyType::egg:
            if (state == "chasing") {
                if (_distanceToPlayer.length() > 12) return "chasing";
				else return "windup";
            }
            if (state == "windup") {
				return "spitting";
			}
            if (state == "short_windup") {
                return "spitting";
            }
            else if (state == "stunned") {
                return "chasing";
            }
            else if (state == "patrolling") {
                return "patrolling";
            }
            else if (state == "spitting") {
                if (_distanceToPlayer.length() > 12) return "chasing";
                else return "short_windup";
            }
            break;

        default:
            return "patrolling";
	}
    return "none";
}

