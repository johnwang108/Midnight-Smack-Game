#include "Boss3.h"

using namespace cugl;


bool Boss3::init(const Vec2& pos, const Size& size, float scale) {
    Size scaledSize = size;
    scaledSize.width *= Boss3_HSHRINK;
    scaledSize.height *= Boss3_VSHRINK;
    _drawScale = scale;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _isChasing = true; // Always chasing
        _direction = -1; // Could start in any direction
        setDensity(Boss3_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);
        return true;
    }
    return false;
}

void Boss3::update(float dt) {

    CapsuleObstacle::update(dt);
    if (_body == nullptr) return;

    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }

    b2Vec2 velocity = _body->GetLinearVelocity();
    velocity.x = _direction * Boss3_CHASE_SPEED; 


    _body->SetLinearVelocity(velocity);


    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

    _lastDamageTime += dt;
}

void Boss3::takeDamage(float damage, int attackDirection) {

    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage;
        if (_health <= 0) {
            _health = 0;
        }
        else {
            b2Vec2 impulse = b2Vec2(-attackDirection * 5, 10);
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 1;
        }
    }
}

void Boss3::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
    }
}

void Boss3::dispose() {
    _core = nullptr;
    _node = nullptr;
}

void Boss3::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }

}

void Boss3::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();


    b2FixtureDef sensorDef;
    sensorDef.density = 0;
    sensorDef.isSensor = true;

    b2PolygonShape sensorShape;
    b2Vec2 sensorVertices[4];
    sensorVertices[0].Set(-getWidth() * Boss3_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[1].Set(getWidth() * Boss3_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[2].Set(getWidth() * Boss3_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorVertices[3].Set(-getWidth() * Boss3_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorShape.Set(sensorVertices, 4);

    sensorDef.shape = &sensorShape;

    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


    _sensorFixture = _body->CreateFixture(&sensorDef);


}