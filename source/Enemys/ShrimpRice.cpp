#include "ShrimpRice.h"
#include "../PFGameScene.h"
using namespace cugl;


bool ShrimpRice::init(const Vec2& pos, const Size& size, float scale) {
    Size scaledSize = size;
    scaledSize.width *= SHRIMPRICE_HSHRINK;
    scaledSize.height *= SHRIMPRICE_VSHRINK;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _direction = -1;
        _lastDirection = -1;
        _health = 100.0f;
        _SFR_attack_chance=0.003f;
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
        _canturn = true;
        _angry = false;
        _timetosummon = false;
        _movestate1 = 0;
        _angrytime = 0;
        _attackcount = 0;
        _W3att = 0;
        return true;
    }
    return false;
}


void ShrimpRice::update(float dt) {

    Entity::update(dt);

    if (_body == nullptr) return;

    b2Vec2 velocity = _body->GetLinearVelocity();
    if (_angry) {
        velocity.x = _direction * SHRIMPRICE_CHASE_SPEED;
    }
    else {
        velocity.x = _direction * SHRIMPRICE_CHASE_SPEED * 1.3;
    }


    if (_direction != _lastDirection && _W3att <= 0) {
        setact("SFRTurn", 0.61f);
    }

    if (_W3att <= 0) {
        _lastDirection = _direction;
        _lastDamageTime += dt;
        _nextChangeTime -= dt;
    }

    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
     //   setGravityScale(1.5);
        if (_node != nullptr) {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        return;
    }



    if (_acttime<=0 && static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < _SFR_attack_chance && _W3att<=0) {
        float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
       // pa=0.9;
        if(pa<=0.2){
            setact("SFRWheelofDoom", 0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }else if(pa <= 0.53 && pa > 0.2) {
            setact("SFRWave1", 1.125);
        }
        else if (pa <= 0.86 && pa > 0.53) {
            setact("SFRJoustState1", 2.25);
            _canturn = false;
        }
        else {
            _W3att = 3;
        }
        _SFR_attack_chance = 0.003f;
        _attackcount = 0;
    }

    if (_W3att>0 && _acttime<=0) {
        setact("SFRWave3", 1.125);
        _W3att--;
    }

    if (_movestate1 > 0) {
        _movestate1 -= dt;
    }

    if (_acttime > 0) {
        _acttime -= dt;
        if (_act == "SFRWave2") {
            velocity.x *= 6;
        }
        if (_act == "SFR_Idle") {
            velocity.x *= 0;
        }
        if (_act == "SFRWheelofDoom") {
            velocity.x *= 0;
        }
        if (_act == "SFRJoustState2") {
            velocity.x *= 12;
        }
        if (_act == "SFR_Attack") {
            velocity.x *= 3;
        }
        if (_act == "SFRJoustState3") {
            velocity.x *= 0;
        }
        if (_act == "SFRJoustState1") {
            velocity.x *= -2;
        }
        if (_act == "SFRWave3") {
            velocity.x = 0;
        }
        if (_acttime <= 0) {
            if (_act == "SFRTurn") {
                if (_W3att > 0) {
                    _direction *= -1;
                }
                scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
                if (image != nullptr) {
                    image->flipHorizontal(!image->isFlipHorizontal());
                }
            }
            if (_act == "SFRWave3") {
                _timetosummon = true;
                if (_W3att > 0) {
                    setact("SFRTurn", 0.61f);
                }
                else {
                    _canturn = true;
                }
            }
            if (_act == "SFRWave1") {
                setact("SFRWave2", 5);
            }
            if (_act == "SFRWheelofDoom") {
                setact("SFR_Attack", 3.375);
            }
            if (_act == "SFRJoustState3") {
                setact("SFR_Idle", 1.291);
            }
            if (_act == "SFRJoustState2") {
                setact("SFRJoustState3", 0.6);
            }
            if (_act == "SFRJoustState1") {
                setact("SFRJoustState2", 3.375);
            }
            if (_act == "SFR_Idle") {
                _canturn = true;
            }

            _movestate1 = 0.7;
		}
	}


    if (!_angry) {
        if (_health <= 40) {
            _angry = true;
            setact("SFR_Idle", 5.0f);
            _angrytime = 10;
        }
    }



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
        _SFR_attack_chance += 0.001f;
        _attackcount += 1;
        if (_health <= 0) {
            _health = 0;
        }
        else if (knockback || _attackcount>=3) {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*300 , 0);
           // _body->SetLinearVelocity(b2Vec2(0, 0));
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 1;
            _acttime = 0;
        }
        else {
        //    b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE * 10, BULL_KNOCKBACK_FORCE_UP*3);
        //    _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 0.5;
        }
    }
}
void ShrimpRice::Summon(GameScene& scene) {

    Vec2 enemyPos = getPosition()-Vec2(-_direction*2,2);
    scene.spawnRice(enemyPos);

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
