#include "Enemy.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include "../PFGameScene.h"



/** 


Behavior diagram and info:

https://docs.google.com/drawings/d/1TS-DFsWZOT4SpqrswI8zruaZdR78v6QIWfDNnVQA1Yo/edit?pli=1


*/

#define BEEF_BURROW_TIME 30.0f
#define BEEF_UNBURROW_TIME 30.0f
#define BEEF_ATTACK_TIME 10.0f

#define CLOSE_ENOUGH 1.0f + ((float)rand() / ((float)RAND_MAX))/2.0f


using namespace cugl;

#pragma mark -
#pragma mark Physics Constants

// Constants for enemy's behavior (e.g., movement speed, jump height) are defined here

#pragma mark -
#pragma mark Constructors

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type) {
    return init(pos, size, scale, type, EnemyModel::defaultSeq(type), EnemyModel::defaultSeq(type));
}

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type, cugl::Spline2 limit) {
    return init(pos, size, scale, type, EnemyModel::defaultSeq(type), EnemyModel::defaultSeq(type), limit);
}

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    	return init(pos, size, scale, type, seq1, seq2, cugl::Spline2());
}

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type, std::vector<std::string> seq1, std::vector<std::string> seq2, cugl::Spline2 limit) {
    Size scaledSize = size;
    scaledSize.width *= ENEMY_HSHRINK;
    scaledSize.height *= ENEMY_VSHRINK;
    _drawScale = scale;
    _type = type;

    if (Entity::init(pos, scaledSize)) {
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
        _preparetime = 0;
        _shooted = false;
        _vulnerable = false;
        _state = "patrolling";
        setGestureSeq1(seq1);
        setGestureSeq2(seq2);
        b2Filter filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);
        setName("enemy");

        //soldier rice
        _targetPosition = cugl::Vec2();
        _closeEnough = CLOSE_ENOUGH;

        //beef and egg
        _limit = limit;

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

    Entity::createFixtures();


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
        _lastDamageTime = 0;
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

    Entity::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }

}

#pragma mark -
#pragma mark Gameplay Methods

//Todo: make deterministic
void EnemyModel::update(float dt) {
    Entity::update(dt);
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
            CULog(_state.c_str());
        }
        break;

    case EnemyType::rice:
        if (_state == "chasing") {
            velocity.x = 0;
            setActiveAction("riceIdle");
        }
        else if (_state == "yelling") {
			velocity.x = 0;
            setActiveAction("riceYell");
		}
        else if (_state == "stunned") {
            velocity.x = 0;
            //setActionParams("riceStun", false, false, true);
        }
        else if (_state == "pursuing") {
            if (velocity.x == 0 && (getActiveAction() != "riceYell" && getActiveAction() != "riceAttack") || isFinished()) setActiveAction("riceStartWalk");
            else if (_distanceToPlayer.length() < 0.05) {
                setActiveAction("riceAttack");
                velocity.x = 0;
                break;
            }
            else if (getActiveAction() == "riceStartWalk" && isFinished() || getActiveAction() == "riceWalk") setActiveAction("riceWalk");
            velocity.x = ENEMY_FORCE * _direction * 2;

        }
        else if (_state == "patrolling") {

            if (velocity.x == 0) setActiveAction("riceStartWalk");
            else if (getActiveAction() == "riceWalk" || (getActiveAction() == "riceStartWalk" && isFinished())) setActiveAction("riceWalk");

            velocity.x = ENEMY_FORCE * _direction;
        }
        else {
            CULog("error: rice");
            CULog(_state.c_str());
        }
        break;

    case EnemyType::rice_soldier:
        if (_state == "chasing") {
            velocity.x = 0;
            setActiveAction("riceIdle");
        }
        else if (_state == "pursuing") {
            float dir = SIGNUM(_targetPosition.x - getPosition().x);

            if (velocity.x == 0 && dir != 0 && ((getActiveAction() != "riceYell" && getActiveAction() != "riceAttack") || isFinished())) {
                setActiveAction("riceStartWalk");
            }
            else if (_distanceToPlayer.length() < 0.02) {
                setActiveAction("riceAttack");
                velocity.x = 0;
                break;
            }
            else if (getActiveAction() != "riceStartWalk" && isFinished()) setActiveAction("riceWalk");

            velocity.x = ENEMY_FORCE * dir * 3;
        }
        else if (_state == "stunned") {
            velocity.x = 0;
            //setActionParams("riceIdle", false, false, true);
        }
        else if (_state == "patrolling") {
            float diff = _targetPosition.x - getPosition().x;
            if (std::abs(diff) > CLOSE_ENOUGH) {
                if (velocity.x = 0) setActiveAction("riceStartWalk");
                else setActiveAction("riceWalk");

                velocity.x = ENEMY_FORCE * SIGNUM(diff);
            }
            else {
                if (velocity.x = 0) setActiveAction("riceIdle");
                else if ((getActiveAction() != "riceStartWalk" || isFinished())) setActiveAction("riceWalk");
                velocity.x = 0;
            }
        }
        else {
            CULog("error: rice soldier");
            CULog(_state.c_str());
        }
        break;

    case EnemyType::egg:
        if (_state == "chasing") {
            velocity.x = ENEMY_FORCE * _direction * 0.5;
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
            velocity.x = ENEMY_FORCE * _direction * 0.25;
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
        if (_state == "chasing") {
			velocity.x = ENEMY_FORCE * _direction * 2;
		}
        else if (_state == "windup") {
			velocity.x = 0;
		}
        else if (_state == "jumping") {
            if (isGrounded()) {
                velocity.y = 10;
                velocity.x = ENEMY_FORCE * _direction * 10;
            }
        }
        else if (_state == "midair") {
            if (isGrounded()) {
				velocity.x = 0;
			}
		}
        else if (_state == "stunned") {
            velocity.x = 0;
        }
        else if (_state == "patrolling") {
			velocity.x = ENEMY_FORCE * _direction;
		}
        else {
			CULog("error: carrot");
			CULog(_state.c_str());
		}
        break;
    case EnemyType::beef:
        if (_state == "chasing") {
            velocity.x = 0;
        }
        else if (_state == "burrowing") {
            //Todo: make mole burrow :)
            b2Filter filter = getFilterData();
            velocity.x = 0;
            velocity.y = (_node->getPositionY() - (_node->getHeight() / BEEF_BURROW_TIME));
        }
        else if (_state == "tracking") {
            cugl::Vec2 targetPos = _limit.nearestPoint(cugl::Vec2(getPosition().x + _direction * ENEMY_FORCE * 5, getPosition().y));
            
            cugl::Vec2 v = targetPos - getPosition();
            velocity = b2Vec2(v.x, v.y);  
        }
        else if (_state == "unburrowing") {
            velocity.x = 0;
        }
        else if (_state == "attacking") {
            velocity.x = 0;
            _node->setPositionY(_node->getPositionY() + (_node->getHeight() / BEEF_ATTACK_TIME));
        }
        else if (_state == "stunned") {
            velocity.x = 0;
		}
        else if (_state == "patrolling") {
            velocity.x = 0;
		}
        break;
    }
    
    _body->SetLinearVelocity(handleMovement(velocity));
    _lastDamageTime += dt;

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

    if (_state != "patrolling") {
        int direction = SIGNUM(_distanceToPlayer.x);
        setDirection(direction);
    }

    if (_lastDirection != _direction && _node != nullptr) {
        _node->flipHorizontal(!_node->isFlipHorizontal());
    }
    _lastDirection = _direction; // Update last direction

    return velocity;
}


void EnemyModel::dispose() {
    _core = nullptr;
    _node = nullptr;
}

//when called, it will return a tuple <attack, node> of the attack that the enemy will create, using the given texture
std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> EnemyModel::createAttack(std::shared_ptr<AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE);
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
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(scale);
    attack->setstraight(_distanceToPlayer + getPosition());
    attack->setEnabled(true);
    attack->setrand(false);
    attack->setSpeed(10.0f);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    return std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>>(attack, sprite);

    /*std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);*/
}

/** begins the aggro behavior, maintains player location information*/
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
    if (_type == EnemyType::rice_soldier) {
        return;
	}
    if (_distanceToPlayer.length() <= typeToAggroRange(_type) && _state == "patrolling") {
        setIsChasing(true);
    }
    else if (_distanceToPlayer.length() >  2 * typeToAggroRange(_type) && _state != "patrolling") {
		setIsChasing(false);
	}
    //CULog("Distance to player: %f", _distanceToPlayer.length());
}

/**Sets the nextstate and also sets how long the enemy stays in that state. 
Counters set to -1 if the state will transition away based off of requirements
other than time.*/
void EnemyModel::setState(std::string state) {
    _state = state;
    if (state == "chasing") {
        _behaviorCounter = -1;
    }
    else if (state == "stunned") {
        _behaviorCounter = 60;
    }
    else if (state == "patrollling") {
        _behaviorCounter = -1;
    }
    
    switch (_type) {
        case EnemyType::shrimp:
            if (state == "curling") {
                _behaviorCounter = 60;
            }
            else if (state == "uncurling") {
                _behaviorCounter = 20;
            }
            else if (state == "rolling") {
                _behaviorCounter = -1;
            }
			break;
        case EnemyType::egg:
            if (state == "windup") {
                _behaviorCounter = 400;
            }
            else if (state == "spitting") {
                _behaviorCounter = 1;
            }
            else if (state == "short_windup") {
                _behaviorCounter = 200;
            }
            break;
        case EnemyType::rice:
            if (state == "yelling") {
                _behaviorCounter = -1;
            }
            else if (state == "pursuing") {
                _behaviorCounter = -1;
            }
            else if (state == "attacking") {
                _behaviorCounter = -1;
            }
            break;
        case EnemyType::rice_soldier:
            if (state == "pursuing") {
                _behaviorCounter = -1;
            }
            else if (state == "attacking") {
                _behaviorCounter = -1;
            }
            break;
        case EnemyType::carrot:
            if (state == "windup") {
				_behaviorCounter = 60;
			}
            else if (state == "jumping") {
				_behaviorCounter = -1;
            }
            else if (state == "midair") {
                _behaviorCounter = -1;
            }
			break;
        case EnemyType::beef:
            if (state == "burrowing") {
                _behaviorCounter = BEEF_BURROW_TIME;
            }
            else if (state == "tracking") {
                _behaviorCounter = -1;
            }
            else if (state == "unburrowing") {
                _behaviorCounter = BEEF_UNBURROW_TIME;
            }
            else if (state == "attacking") {
                _behaviorCounter = BEEF_ATTACK_TIME;
            }
            else if (state == "stunned") {
                _behaviorCounter = 120;
            }
			break;
        default:
            _behaviorCounter = -1;
            break;
    }
}

/**Chasing is an intermediate state between passive and aggro behavior.
For example the only time beef is chasing is when it first spots the player or after it is stunned.*/
std::string EnemyModel::getNextState(std::string state) {
    switch (_type) {
		case EnemyType::shrimp:
            if (state == "chasing") {
				return "curling";
			}
            else if (state == "curling") {
                return "rolling";
            }
            else if (state == "rolling") {
                return "rolling";
			}
            else if (state == "uncurling") {
				return "patrolling";
			}
            else if (state == "stunned") {
                return "rolling";
            }
            else if (state == "patrolling") {
				return "patrolling";
			}
			break;
        case EnemyType::rice:
            if (state == "chasing") {
                return "yelling";
            }
            else if (state == "yelling") {
                if (getActiveAction() == "riceYell" && isFinished()) return "pursuing";
                return "yelling";
            }
            else if (state == "stunned") {
                return "chasing";
            }
            else if (state == "pursuing") {
                return "pursuing";
            }
            else if (_state == "attacking") {
                if (_activeAction == "riceAttack") return "attacking";
                return "pursuing";
            }
            else if (state == "patrolling") {
                return "patrolling";
            }

            break;
        case EnemyType::rice_soldier:
            if (state == "chasing") {
                return "pursuing";
            }
            else if (_state == "pursuing") {
                return "pursuing";
            }
            else if (_state == "attacking") {
                if (_activeAction == "riceAttack") return "attacking";
                return "pursuing";
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
            else if (state == "windup") {
				return "spitting";
			}
            else if (state == "short_windup") {
                return "spitting";
            }
            else if (state == "stunned") {
                return "chasing";
            }
            else if (state == "spitting") {
                if (_distanceToPlayer.length() > 12) return "chasing";
                else return "short_windup";
            }
            else if (state == "patrolling") {
                return "patrolling";
            }
            break;
        case EnemyType::beef:
            if (state == "chasing") {
                return "burrowing";
            }
            else if (state == "burrowing") {
                return "tracking";
            }
            else if (state == "tracking") {
                if (abs(_distanceToPlayer.x) < 1) return "unburrowing";
                else return "tracking";
            }
            else if (state == "unburrowing") {
                return "attacking";
            }
            else if (state == "attacking") {
                return "stunned";
            }
            else if (state == "stunned") {
                return "burrowing";
            }
            else if (state == "patrolling") {
                return "patrolling";
            }
            break;

        case EnemyType::carrot:
            if (state == "chasing") {
                return "windup";
            }
            else if (state == "windup") {
                return "jumping";
            }
            else if (state == "jumping") {
                if (!isGrounded()) return "midair";
				else return "jumping";
            }
            else if (state == "midair") {
                if (isGrounded()) return "windup";
                else return "midair";
            }
            else if (state == "stunned") {
                return "windup";
            }
            else if (state == "patrolling") {
				return "patrolling";
			}
			break;
        default:
            return "patrolling";
    }
}














//only called when there is no active action.
std::string EnemyModel::updateAnimation() {
    switch (_type) {
    case EnemyType::shrimp:
        if (_state == "chasing") {
            return "";
        }
        else if (_state == "curling") {
            return "";
        }
        else if (_state == "rolling") {
            return "";
        }
        else if (_state == "uncurling") {
            return "";
        }
        else if (_state == "stunned") {
            return "";
        }
        else if (_state == "patrolling") {
            return "";
        }
        break;
    case EnemyType::rice:
        if (_state == "chasing") {
            return "";
        }
        else if (_state == "yelling") {
            return "";
        }
        else if (_state == "stunned") {
            return "";
        }
        else if (_state == "attacking") {
            return "";
        }
        else if (_state == "pursuing") {
            return "";
        }
        else if (_state == "patrolling") {
            return "";
        }

        break;
    case EnemyType::rice_soldier:
        b2Vec2 velocity = _body->GetLinearVelocity();
        if (_state == "chasing") {

        } 
        else if (_state == "attacking") {

        }
        else if (_state == "pursuing") {

        }
        else if (_state == "stunned") {

        }
        else if (_state == "patrolling") {
            if (velocity.x == 0 && velocity.y == 0) {
                return "riceIdle";
			}
            else if (velocity.x != 0 && velocity.y == 0) {
				return "riceWalk";
            }
		}
        break;
    case EnemyType::egg:
        if (_state == "chasing") {
            if (_distanceToPlayer.length() > 12) return "chasing";
            else return "";
        }
        else if (_state == "windup") {
            return "";
        }
        else if (_state == "short_windup") {
            return "";
        }
        else if (_state == "stunned") {
            return "";
        }
        else if (_state == "spitting") {
            if (_distanceToPlayer.length() > 12) return "chasing";
            else return "";
        }
        else if (_state == "patrolling") {
            return "";
        }
        break;
    case EnemyType::beef:
        if (_state == "chasing") {
            return "";
        }
        else if (_state == "burrowing") {
            return "";
        }
        else if (_state == "tracking") {
            if (abs(_distanceToPlayer.x) < 1) return "unburrowing";
            else return "";
        }
        else if (_state == "unburrowing") {
            return "";
        }
        else if (_state == "attacking") {
            return "";
        }
        else if (_state == "stunned") {
            return "";
        }
        else if (_state == "patrolling") {
            return "";
        }
        break;

    case EnemyType::carrot:
        if (_state == "chasing") {
            return "";
        }
        else if (_state == "windup") {
            return "";
        }
        else if (_state == "jumping") {
            if (!isGrounded()) return "midair";
            else return "";
        }
        else if (_state == "midair") {
            if (isGrounded()) return "windup";
            else return "";
        }
        else if (_state == "stunned") {
            return "";
        }
        else if (_state == "patrolling") {
            return "";
        }
        break;
    default:
        return "patrolling";
    }
}