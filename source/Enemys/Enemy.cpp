#include "Enemy.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include "../PFGameScene.h"



/** 


Behavior diagram and info:

https://docs.google.com/drawings/d/1TS-DFsWZOT4SpqrswI8zruaZdR78v6QIWfDNnVQA1Yo/edit?pli=1


*/


using namespace cugl;

#pragma mark -
#pragma mark Physics Constants

// Constants for enemy's behavior (e.g., movement speed, jump height) are defined here

#pragma mark -
#pragma mark Constructors
/** do not use*/
bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return false;
}

bool EnemyModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    Size scaledSize = size;
    scaledSize.width *= ENEMY_HSHRINK;
    scaledSize.height *= ENEMY_VSHRINK;
    _drawScale = scale;

    if (Entity::init(pos, scaledSize)) {
        setDensity(ENEMY_DENSITY);
        setFriction(0.0f); // Prevent sticking to walls
        setFixedRotation(false);
        setFriction(1.0f);
        _isChasing = false;
        _isGrounded = false;
        _direction = -1;
        _lastDirection = _direction;
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
        _priority = 0;
        _activePriority = 0;
        _behaviorCounter = -1;
        _killMe = false;
        _killMeCountdown = 0.0f;
        _useID = true;
        _isTangible = true;

        return true;
    }

    return false;
}

#pragma mark -
#pragma mark Static Constructors

std::shared_ptr<EnemyModel> EnemyModel::alloc(const Vec2& pos, const Size& size, float scale, EnemyType type) {
    std::shared_ptr<EnemyModel> result = std::make_shared<EnemyModel>();
    return (result->init(pos, size, scale) ? result : nullptr);
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
            if (_type != EnemyType::beef) {
                b2Vec2 impulse = b2Vec2(-attackDirection * 10, 14);
                _body->SetLinearVelocity(impulse);
                setState("stunned");
                _knockbackTime = 1;
            }
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
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }
}

void EnemyModel::fixedUpdate(float step) {
    //updating counters
    if (_killMeCountdown > 0) {
        _killMeCountdown -= step;
    }
    else if (_killMeCountdown < 0) {
		_killMe = true;
    }

    if (_knockbackTime > 0) {
        _knockbackTime -= step;
    }
    else if (_preparetime > 0) {
        if (_preparetime < 1 && _shooted) {
            _attacktime = true;
        }
        _preparetime -= step;
        _body->SetLinearVelocity(b2Vec2(0, 0));
        if (_node != nullptr) {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
    }

    //set behaviors
    b2Vec2 velocity = _body->GetLinearVelocity();
    if (std::abs(velocity.x) < 1.0f) velocity.x = 0;
    _body->SetLinearVelocity(velocity);

    if (_behaviorCounter > 0) {
        _behaviorCounter -= step;
    }
    else if (_behaviorCounter <= 0 || (_behaviorCounter < (-1.0 * step) && getNextState(_state) != _state)) {
        setState(getNextState(_state));
    }
    _lastDamageTime += step;
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

    if (std::abs(velocity.x) < 0.03) velocity.x = 0;
    if (velocity.x != 0) {
        if (_state != "patrolling") {
            setDirection(SIGNUM(_distanceToPlayer.x));
        }
        else {
            setDirection(SIGNUM(velocity.x));
        }
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

//when called, it will return a tuple <attack, node> of the attack 
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
    //attack->setstraight(_distanceToPlayer + getPosition());
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
    else if (_distanceToPlayer.length() > 1.5f * typeToAggroRange(_type) && _state != "patrolling") {
		setIsChasing(false);
	}
    //CULog("Distance to player: %f", _distanceToPlayer.length());
}

/**Sets the nextstate and also sets how long the enemy stays in that state. 
Counters set to -1 if the state will transition away based off of requirements
other than time.*/
void EnemyModel::setState(std::string state) {
    _state = state;   
}