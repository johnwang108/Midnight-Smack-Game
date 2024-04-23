#include "ShrimpRice.h"
#include "../PFGameScene.h"
using namespace cugl;


bool ShrimpRice::init(const Vec2& pos, const Size& size, float scale) {
    Size scaledSize = size;
    scaledSize.width *= SHRIMPRICE_HSHRINK;
    scaledSize.height *= SHRIMPRICE_VSHRINK;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _isChasing = false;
        _direction = -1;
        _lastDirection = -1;
        _health = 100.0f;
        _SFR_attack_chance=0.002f;
        _healthCooldown = 0.1f;
        _knockbackTime = 0;
        _attacktype = "none";
        _act = "none";
        _acttime = 0;
        b2Filter filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);
        setDensity(SHRIMPRICE_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);

        return true;
    }
    return false;
}


void ShrimpRice::update(float dt) {

    Entity::update(dt);
    if (_body == nullptr) return;



    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        setGravityScale(1.5);
        return;
    }

    b2Vec2 velocity = _body->GetLinearVelocity();
    velocity.x = _direction * SHRIMPRICE_CHASE_SPEED*1.5; 

    if (_acttime<=0 && static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < _SFR_attack_chance) {
        float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        pa=0.8;
        if (pa <= 0.25) {
            setact("SFR_Attack", 1.125);
		}
        else if(pa<=0.5 && pa>0.25){
            setact("SFRWheelofDoom", 2 + 2 * static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }else if(pa <= 0.75 && pa > 0.5) {
            setact("SFRWave1", 1.125);
        }
        else {
            setact("SFRJoustState1", 1.125);
        }

    }
    if (_acttime > 0) {
        _acttime -= dt;
        if (_act == "SFRWave2") {
            velocity.x *= 6;
        }
        if (_act == "SFRWheelofDoom") {
            velocity.x *= 4;
            velocity.y = 8;
        }
        if (_acttime <= 0) {
            if (_act == "SFRWave1") {
                setact("SFRWave2", 10);
            }
            if (_act == "SFRWheelofDoom") {
                _knockbackTime = 2;
            }
            if (_act == "SFRJoustState2") {
                setact("SFRJoustState3", 0.3);
            }
            if (_act == "SFRJoustState1") {
                setact("SFRJoustState2", 4.5);
            }

		}
	}

    

    if (_direction != _lastDirection) {
        // If direction changed, flip the image
        scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
        if (image != nullptr) {
            image->flipHorizontal(!image->isFlipHorizontal());
        }
    }

    _lastDirection = _direction;
    _lastDamageTime += dt;
    _nextChangeTime -= dt;
    _body->SetLinearVelocity(velocity);


    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

}

void ShrimpRice::takeDamage(float damage, int attackDirection, bool knockback) {

    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage;
        if (_health <= 0) {
            _health = 0;
        }
        else if (knockback) {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE , BULL_KNOCKBACK_FORCE_UP * 5);
            _body->SetLinearVelocity(b2Vec2(0, 0));
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 4;
        }
        else {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE * 10, BULL_KNOCKBACK_FORCE_UP*3);
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 0.5;
        }
    }
}


void ShrimpRice::dispose() {
    _core = nullptr;
    _node = nullptr;
}

void ShrimpRice::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }

}

void ShrimpRice::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();


    b2FixtureDef sensorDef;
    sensorDef.density = 0;
    sensorDef.isSensor = true;

    b2PolygonShape sensorShape;
    b2Vec2 sensorVertices[4];
    sensorVertices[0].Set(-getWidth() * SHRIMPRICE_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[1].Set(getWidth() * SHRIMPRICE_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[2].Set(getWidth() * SHRIMPRICE_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorVertices[3].Set(-getWidth() * SHRIMPRICE_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorShape.Set(sensorVertices, 4);

    sensorDef.shape = &sensorShape;

    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


    _sensorFixture = _body->CreateFixture(&sensorDef);


}
