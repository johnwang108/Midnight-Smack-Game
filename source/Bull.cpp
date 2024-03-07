#include "Bull.h"

using namespace cugl;


bool BullModel::init(const Vec2& pos, const Size& size, float scale) {

    if (CapsuleObstacle::init(pos, size)) {
        _drawScale = scale;
        _isChasing = true; // Always chasing
        _direction = -1; // Could start in any direction
        setDensity(BULL_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);
        return true;
    }
    return false;
}

void BullModel::update(float dt) {

    CapsuleObstacle::update(dt);
    if (_body == nullptr) return;

    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }

    b2Vec2 velocity = _body->GetLinearVelocity();
    velocity.x = _direction * BULL_CHASE_SPEED; 


    _body->SetLinearVelocity(velocity);


    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

    _lastDamageTime += dt;
}

void BullModel::takeDamage(float damage, int attackDirection) {

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

void BullModel::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
    }
}
