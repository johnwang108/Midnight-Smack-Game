#include "Enemy.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#define ENEMY_FORCE      1.0f
#define ENEMY_DAMPING    5.0f
#define ENEMY_MAXSPEED   10.0f
#define ENEMY_JUMP       2.5f
#define ENEMY_VSHRINK    0.8f
#define ENEMY_HSHRINK    0.7f
#define ENEMY_DENSITY    1.0f

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

    if (BoxObstacle::init(pos, scaledSize)) {
        setDensity(ENEMY_DENSITY);
        setFriction(0.0f); // Prevent sticking to walls
        setFixedRotation(true); // Avoid tumbling

        _isGrounded = false;
        _direction = 1; // Start moving right by default

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

    BoxObstacle::createFixtures();
    // Additional fixture setup for different enemy types can be added here
}

void EnemyModel::releaseFixtures() {
    if (_body != nullptr) {
        BoxObstacle::releaseFixtures();
        // Additional cleanup for custom fixtures can be added here
    }
}

#pragma mark -
#pragma mark Gameplay Methods

void EnemyModel::update(float dt) {
    BoxObstacle::update(dt);

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