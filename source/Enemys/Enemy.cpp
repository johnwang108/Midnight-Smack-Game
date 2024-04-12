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
            b2Vec2 impulse = b2Vec2(-attackDirection * 5, 10);
            _body->ApplyLinearImpulseToCenter(impulse, true);
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

void EnemyModel::update(float dt) {
    CapsuleObstacle::update(dt);
    if (_body == nullptr) {
        return;
    }
    
    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }else if (_preparetime > 0) {
        if (_preparetime < 1 && _shooted) {
            _attacktime = true;
        }
		_preparetime-=dt;
        _body->SetLinearVelocity(b2Vec2(0,0));
        if (_node != nullptr) {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        return;
	}

    // Example movement logic for enemy
    if (_isGrounded) {
        b2Vec2 velocity = _body->GetLinearVelocity();
        velocity.x = ENEMY_FORCE;

        // Reverse direction at edges or obstacles
        if (velocity.x > ENEMY_MAXSPEED) {
            velocity.x = ENEMY_MAXSPEED;
            _direction = -_direction;
        }
        else if (velocity.x < -ENEMY_MAXSPEED) {
            velocity.x = -ENEMY_MAXSPEED;
            _direction = -_direction;
        }
        _nextChangeTime -= dt;
        if (isChasing()) {
            if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < ENEMY_ATTACK_CHANCE) {
                _preparetime = 2.5;
                _shooted = true;
            }
            velocity.x *= CHASE_SPEED;
        } else if (!isChasing() && _nextChangeTime <= 0) {
              _direction = (rand() % 2) * 2 - 1;
              _nextChangeTime = _changeDirectionInterval + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * _changeDirectionInterval;
            }

        if (_direction != _lastDirection) {
            // If direction changed, flip the image
            scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
            if (image != nullptr) {
                image->flipHorizontal(!image->isFlipHorizontal());
            }

     }


     velocity.x *= _direction;
     _lastDirection = _direction; // Update last direction

     _body->SetLinearVelocity(velocity);
 }
   
    _lastDamageTime+= dt;
    

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

void EnemyModel::createAttack(GameScene& scene) {
    std::shared_ptr<AssetManager> _assets = scene.getAssets();
    float _scale = scene.getScale();

    std::shared_ptr<Texture> image;
    image = _assets->get<Texture>(ATTACK_TEXTURE_L);
    Vec2 pos = getPosition();

    std::shared_ptr<EnemyAttack> attack = EnemyAttack::alloc(pos,
        cugl::Size(image->getSize().width / _scale,
            ATTACK_H * image->getSize().height / _scale));

    pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += ATTACK_OFFSET_Y;



    if (getDirection() > 0) {
		attack->setFaceRight(true);
	}
    attack->setName("enemy_attack");
    attack->setBullet(true);
    attack->setGravityScale(0.2);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(true);
    attack->setrand(false);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);


}


