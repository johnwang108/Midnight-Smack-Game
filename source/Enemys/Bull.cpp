#include "Bull.h"

using namespace cugl;


bool BullModel::init(const Vec2& pos, const Size& size, float scale) {
    Size scaledSize = size;
    scaledSize.width *= BULL_HSHRINK;
    scaledSize.height *= BULL_VSHRINK;
    _drawScale = scale;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _isChasing = false;
        _direction = -1; 
        _lastDirection = _direction;
        _nextChangeTime = 0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.5;
        _health = 100.0f;
        _healthCooldown = 0.2f;
        _isPreparingSprint = false;
        _sprintPrepareTime = 0;
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
    velocity.x = BULL_FORCE * _direction;

    if (_isPreparingSprint && _sprintPrepareTime > 0) {
        _sprintPrepareTime -= dt;
        velocity.x = 0;
        if (_sprintPrepareTime <= 0) {
            _isChasing = true;
            _isPreparingSprint = false;
        }
    }
    else if (!_isChasing && !_isPreparingSprint && static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < BULL_ATTACK_CHANCE) {
        _isPreparingSprint = true;
        _sprintPrepareTime = 3;
    }
    if (_isChasing) {
        velocity.x *= BULL_CHASE_SPEED;
    }


    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
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
}

void BullModel::takeDamage(float damage, int attackDirection) {
 
    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage;
        if (_health <= 0) {
            _health = 0;
        }
        else {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE, BULL_KNOCKBACK_FORCE_UP*25);
            _body->ApplyLinearImpulseToCenter(impulse, true);
            b2Vec2 tee=_body->GetLinearVelocity();
            _knockbackTime = 6;
        }
    }
}
 
void BullModel::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
    }
}

void BullModel::dispose() {
    _core = nullptr;
    _node = nullptr;
}

void BullModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }

    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;
    }

}

void BullModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }

    CapsuleObstacle::createFixtures();


    b2FixtureDef sensorDef;
    sensorDef.density = 0;
    sensorDef.isSensor = true;

    b2PolygonShape sensorShape;
    b2Vec2 sensorVertices[4];
    sensorVertices[0].Set(-getWidth() * BULL_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[1].Set(getWidth() * BULL_HSHRINK / 2.0f, -getHeight() / 2.0f);
    sensorVertices[2].Set(getWidth() * BULL_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorVertices[3].Set(-getWidth() * BULL_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
    sensorShape.Set(sensorVertices, 4);

    sensorDef.shape = &sensorShape;

    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


    _sensorFixture = _body->CreateFixture(&sensorDef);


}