#include "Bull.h"
#include "../PFGameScene.h"
using namespace cugl;


bool BullModel::init(const Vec2& pos, const Size& size, float scale) {
    scaledSize = size;
    scaledSize.width *= BULL_HSHRINK;
    scaledSize.height *= BULL_VSHRINK;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _isChasing = false;
        _direction = -1; 
        _lastDirection = _direction;
        _nextChangeTime = 0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.5;
        _health = 100.0f;
        _healthCooldown = 0.1f;
        _angrytime = 0;
        _sprintPrepareTime = 0;
        _shake = false;
        _bull_attack_chance= BULL_ATTACK_CHANCE;
        _P2start = false;
        _shoot = false;
        setDensity(BULL_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);

        return true;
    }
    return false;
}
void BullModel::sethealthbar(){
    auto healthBarBackground = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("heartsbroken"));
    auto healthBarForeground = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("heartsfull"));
    healthBarBackground->setPosition(Vec2(10, 10));
    healthBarForeground->setPosition(Vec2(10, 10));
    _node->addChild(healthBarBackground);
    _node->addChild(healthBarForeground);
    _healthBarForeground = healthBarForeground;
}
void BullModel::update(float dt) {

    CapsuleObstacle::update(dt);
    if (_body == nullptr) return;
    /*
    if (_healthBarForeground != nullptr) {
        healthPercentage = _health / 100;
        _healthBarForeground->setScale(healthPercentage);
    }
    */
    b2Vec2 velocity = _body->GetLinearVelocity();
    velocity.x = BULL_FORCE * _direction;

    if (_sprintPrepareTime > 0) {
        _sprintPrepareTime -= dt;
        velocity.x = 0;
        if (_sprintPrepareTime <= 0) {
            float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
         //   pa = 0.87;
            if (pa<0.33) {
                _isChasing = true;
            }
            else if(pa>=0.33 && pa<0.66){
                _shake = true;
                b2Vec2 impulse = b2Vec2(0, BULL_KNOCKBACK_FORCE_UP * 25);
                _body->ApplyLinearImpulseToCenter(impulse, true);
                _knockbackTime = 2;
            }
            else {
                _shoot = true;
			}
            
        }
    }else if (!_isChasing &&  static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < _bull_attack_chance) {
        _sprintPrepareTime = 2;
    }

    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }

    if (_angrytime > 0) {
        _angrytime -= dt;
        _body->SetLinearVelocity(b2Vec2(0, 0));
        if (_node != nullptr) {
            scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
            if (image->getTexture() != _assets->get<Texture>("P2bull")) {
                image->setTexture(_assets->get<Texture>("P2bull"));
            }

            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        _bull_attack_chance = BULL_ATTACK_CHANCE*2;
        return;
    }

    if (_isChasing) {
        velocity.x *= BULL_CHASE_SPEED;
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

void BullModel::takeDamage(float damage, int attackDirection,bool knockback) {
 
    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage;
        if (_health <= 0) {
            _health = 0;
        }else if (knockback) {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*10, BULL_KNOCKBACK_FORCE_UP * 25);
            _body->SetLinearVelocity(b2Vec2(0, 0));
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 4;
        }else {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*5, 0);
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 0.5;
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

void BullModel::createAttack(GameScene& scene) {

    float _scale = scene.getScale();

    std::shared_ptr<Texture> image = _assets->get<Texture>("unball");
    Vec2 pos = getPosition();

    std::shared_ptr<EnemyAttack> attack = EnemyAttack::alloc(pos,
        cugl::Size(0.8*image->getSize().width / _scale,
              0.8*image->getSize().height / _scale)*1.3);

    pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += ATTACK_OFFSET_Y;



    if (getDirection() > 0) {
        attack->setFaceRight(true);
    }
    attack->setName("enemy_attack");
    attack->setBullet(true);
    attack->setGravityScale(0.1);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(true);
    attack->setrand(true);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}

void BullModel::createAttack2(GameScene& scene) {
    float _scale = scene.getScale();

    std::shared_ptr<Texture> image = _assets->get<Texture>(SHAKE_TEXTURE);
    Vec2 pos = getPosition();
    pos.x += ATTACK_OFFSET_X*6;
    pos.y -= ATTACK_OFFSET_Y * 3;
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(0.9*image->getSize().width / _scale,
            ATTACK_H * image->getSize().height / _scale));

    Vec2 pos2 = pos;
    pos2.x -= ATTACK_OFFSET_X *12;
    std::shared_ptr<Attack> attack2 = Attack::alloc(pos2,
        cugl::Size(0.9*image->getSize().width / _scale,
            ATTACK_H * image->getSize().height / _scale));

    attack->setFaceRight(true);
    attack->setName("shake");
    attack->setBullet(true);
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(false);
    attack->setGo(true);

    attack2->setFaceRight(false);
    attack2->setName("shake");
    attack2->setBullet(true);
    attack2->setGravityScale(0);
    attack2->setDebugColor(DEBUG_COLOR);
    attack2->setDrawScale(_scale);
    attack2->setEnabled(false);
    attack2->setGo(true);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<scene2::PolygonNode> sprite2 = scene2::PolygonNode::allocWithTexture(image);
    attack2->setSceneNode(sprite2);
    sprite2->setPosition(pos2);
    sprite2->flipHorizontal(true);

    scene.addObstacle(attack2, sprite2, true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}

void BullModel::createAttack3(GameScene& scene) {

    std::shared_ptr<DudeModel> _Su = scene.getAvatar();
    float _scale = scene.getScale();

    std::shared_ptr<Texture> image = _assets->get<Texture>("dball");
    Vec2 pos = getPosition();

    std::shared_ptr<EnemyAttack> attack = EnemyAttack::alloc(pos,
        cugl::Size(0.8 * image->getSize().width / _scale,
            0.8 * image->getSize().height / _scale) * 1.3);

    //pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    //pos.y += ATTACK_OFFSET_Y;



    attack->setName("enemy_attack");
    attack->setBullet(true);
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(true);
    attack->setstraight(_Su->getPosition());
    attack->setrand(false);


    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}