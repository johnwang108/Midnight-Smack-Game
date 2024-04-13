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
        _summoned = false;
        _running = false;
        _CA=0;
        _CAcount = 0;
        _breaking = 0;
        b2Filter filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);
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

    b2Vec2 velocity = _body->GetLinearVelocity();
    velocity.x = BULL_FORCE * _direction;


    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
        return;
    }

    if (_sprintPrepareTime > 0) {
        _sprintPrepareTime -= dt;
        velocity.x = 0;
        if (_sprintPrepareTime <= 0) {
            float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            pa = 0.2;
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

    if (_angrytime > 0) {
        _angrytime -= dt;
        _body->SetLinearVelocity(b2Vec2(0, 0));
        if (_node != nullptr) {
            scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
            if (image->getTexture() != _assets->get<Texture>("P2bull")) {
                image->setTexture(_assets->get<Texture>("P2bull"));
            }
            image->setScale(0.75);

            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }

        return;
    }

    if (_running) {
        _running = false;
        _node->setVisible(true);
        _body->SetEnabled(true);
      //  setPosition(getPosition() + -_direction * Vec2(40, 0));
    }
    if (_breaking>0) {
        _breaking -= dt;
        velocity.x *= 5/1.2*_breaking/10;
        if (_breaking<0.1) {
            _isChasing= false;
        }
    }

    if (_isChasing && !_breaking) {
        velocity.x *= BULL_CHASE_SPEED;
        if (_CAcount > 0) {
            velocity.x *= _CAcount/1.2;
            _bull_attack_chance = BULL_ATTACK_CHANCE*1.5;
        }
        if (_CAcount == 6 && _CA <= 0) {
            _CAcount = 0;
            _breaking = 10;
        }
        if (_CA > 0) {
            _CA -= dt;
           // _node->setVisible(false);
            _body->SetEnabled(false);
            
            float yyy;
            if(_CA>5){
                yyy=25*(dt/10);
            }else{
                yyy=-25*(dt/10);
            }
 
            setPosition(getPosition()+Vec2(-_direction*(38)*(dt/10),yyy));

            if (_CA <= 0) {
                _running = true;
                setGravityScale(1);
            }
            return;
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
  //  _nextChangeTime -= dt;
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
          //  b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*10, BULL_KNOCKBACK_FORCE_UP * 25);
          //  _body->SetLinearVelocity(b2Vec2(0, 0));
          //  _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 3.5;
        }else {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*5, 0);
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 0.5;
        }
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

    std::shared_ptr<Attack> attack = Attack::alloc(pos,
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
    pos.y -= ATTACK_OFFSET_Y * 4.5;
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
    attack->setnorotate(true);

    attack2->setFaceRight(false);
    attack2->setName("shake");
    attack2->setBullet(true);
    attack2->setGravityScale(0);
    attack2->setDebugColor(DEBUG_COLOR);
    attack2->setDrawScale(_scale);
    attack2->setEnabled(false);
    attack2->setGo(true);
    attack2->setnorotate(true);
    



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<scene2::PolygonNode> sprite2 = scene2::PolygonNode::allocWithTexture(image);
    attack2->setSceneNode(sprite2);
    sprite2->setPosition(pos2);

    scene.addObstacle(attack2, sprite2, true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}

void BullModel::createAttack3(GameScene& scene) {

    std::shared_ptr<DudeModel> _Su = scene.getAvatar();
    float _scale = scene.getScale();

    std::shared_ptr<Texture> image = _assets->get<Texture>("dball");
    Vec2 pos = getPosition();

    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(0.8 * image->getSize().width / _scale,
            0.8 * image->getSize().height / _scale) * 1.3);


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
void BullModel::Summon(GameScene& scene) {

    cugl::Size shrimpSize = cugl::Size(2.0f, 2.0f);

    std::vector<std::shared_ptr<EnemyModel>> Enemies=scene.getEnemies();
    std::shared_ptr<Texture> image = _assets->get<Texture>("shrimp_rolling");
    std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc({ getPosition() + Vec2(5.0f, 10.0f) }, shrimpSize, scene.getScale(), EnemyType::shrimp);
    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setScale(0.1f);
    _enemy->setSceneNode(sprite);
    _enemy->setName(ENEMY_NAME);
    _enemy->setDebugColor(DEBUG_COLOR);
    scene.addObstacle(_enemy, sprite);
    Enemies.push_back(_enemy);

    image = _assets->get<Texture>("shrimp_rolling");
    _enemy = EnemyModel::alloc({ getPosition() + Vec2(-5.0f, 10.0f) }, shrimpSize, scene.getScale(), EnemyType::shrimp);
    sprite = scene2::PolygonNode::allocWithTexture(image);
    sprite->setScale(0.1f);
    _enemy->setSceneNode(sprite);
    _enemy->setName(ENEMY_NAME);
    _enemy->setDebugColor(DEBUG_COLOR);
    scene.addObstacle(_enemy, sprite);
    Enemies.push_back(_enemy);

    scene.setEnemies(Enemies);
}

void BullModel::circleattack(GameScene& scene) {
    _CA = 10;
    setGravityScale(0);
}
