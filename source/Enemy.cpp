#include "Enemy.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>



using namespace cugl;

#pragma mark -
#pragma mark Physics Constants

// Constants for enemy's behavior (e.g., movement speed, jump height) are defined here

#pragma mark -
#pragma mark Constructors

bool EnemyModel::init(const Vec2& pos, const Size& size, float scale, EnemyType type) {
    Size scaledSize = size;
    scaledSize.width *= ENEMY_HSHRINK;
    scaledSize.height *= ENEMY_VSHRINK;
    _drawScale = scale;
    _type = type;

    if (CapsuleObstacle::init(pos, scaledSize)) {
        setDensity(ENEMY_DENSITY);
        setFriction(0.0f); // Prevent sticking to walls
        setFixedRotation(true); // Avoid tumbling

        _isGrounded = false;
        _direction = -1; // Start moving right by default
        _lastDirection = _direction;
        _changeDirectionInterval = 3.0f;
        _nextChangeTime = _changeDirectionInterval + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * _changeDirectionInterval;

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

void EnemyModel::update(float dt) {
    CapsuleObstacle::update(dt);
    if (_body == nullptr) {
        return;
    }

    // Example movement logic for enemy
    if (_isGrounded) {
        b2Vec2 velocity = _body->GetLinearVelocity();
        velocity.x = ENEMY_FORCE * _direction;

        // Reverse direction at edges or obstacles
        if (velocity.x > ENEMY_MAXSPEED) {
            velocity.x = ENEMY_MAXSPEED;
            _direction = -_direction;
        }
        else if (velocity.x < -ENEMY_MAXSPEED) {
            velocity.x = -ENEMY_MAXSPEED;
            _direction = -_direction;
        }

        _body->SetLinearVelocity(velocity);
    }

    _nextChangeTime -= dt;
    if (_nextChangeTime <= 0) {
        _direction = (rand() % 2) * 2 - 1;
        _nextChangeTime = _changeDirectionInterval + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * _changeDirectionInterval;
    }

    if (_direction != _lastDirection) {
        // If direction changed, flip the image
        scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
        if (image != nullptr) {
            image->flipHorizontal(!image->isFlipHorizontal());
        }
        _lastDirection = _direction; // Update last direction
    }


    // Update scene node position and rotation to match physics body
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }

}


void EnemyModel::setSceneNode(const std::shared_ptr<scene2::SceneNode>& node) {
    _node = node;
    _node->setPosition(getPosition() * _drawScale);
}

void EnemyModel::dispose() {
    _core = nullptr;
    _node = nullptr;
}


