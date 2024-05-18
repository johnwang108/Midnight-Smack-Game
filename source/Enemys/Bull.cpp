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
        _running = 0;
        _CA=0;
        _CAcount = 0;
        _breaking = 0;
        _attacktype = "none";
        _turing = 0;
        filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);
        setDensity(BULL_DENSITY);
        setFriction(0.0f);
        setFixedRotation(true);
        _actionM=scene2::ActionManager::alloc();
        n=0;
        _act="none";
        _acttime=0;

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

    if (_angrytime > 0) {
        _angrytime -= dt;
        _body->SetLinearVelocity(b2Vec2(0, 0));
        return;
    }

    if (_sprintPrepareTime > 0) {
        _sprintPrepareTime -= dt;
        velocity.x = 0;
        if (_sprintPrepareTime <= 0) {
            if (_attacktype == "bullTelegraph") {
                _isChasing = true;
            }
            else if (_attacktype == "bullStomp") {
                _shake = true;
                b2Vec2 impulse = b2Vec2(0, BULL_KNOCKBACK_FORCE_UP * 25);
                _body->ApplyLinearImpulseToCenter(impulse, true);
                _knockbackTime = 2;
            }
            else if (_attacktype == "bullDazedtoIdle") {
                _shoot = true;
            }

        }
    }
    else if (!_isChasing && static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < _bull_attack_chance && _acttime<=0) {
        _sprintPrepareTime = 2;
        float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
     //   pa = 0.2;
        if (pa < 0.33) {
            _attacktype = "bullTelegraph";
        }
        else if (pa >= 0.33 && pa < 0.66) {
            _attacktype = "bullStomp";
        }
        else {
            _attacktype = "bullDazedtoIdle";
        }

    }
    
    if(_acttime>0){
        _acttime-=dt;
        if(_acttime<=0){
            if(_act=="bullTurn"){
                scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
                if (image != nullptr) {
                    image->flipHorizontal(!image->isFlipHorizontal());
                }
            }
            if(_act=="bullStunned"){
                setact("bullDazedtoIdle", 1.0f);
            }
            if(_act=="bullCrash"){
                setact("bullStunned", 3.0f);
            }

        }
    }
    
    
    if (_running>0) {
        _running -= dt;
        setPosition(getPosition()+Vec2(_direction*0.2,0));
        if(_running<=0){
            _body->SetEnabled(true);
            if(_CAcount<=0){
                _breaking=4;
            }
        }
    }
    
    if (_breaking > 0) {
        _breaking -= dt;
        velocity.x *= BULL_CHASE_SPEED*_breaking / 2;
        if (_breaking <= 0) {
            _isChasing = false;
        }
    }

    if (_isChasing && _breaking<=0) {
        velocity.x *= BULL_CHASE_SPEED;
        if (_CAcount > 0) {
            velocity.x *= _CAcount / 1.2;
        }
        if (_CAcount == 6 && _CA <= 0 && _turing<=0) {
            _CAcount = 0;
            _bull_attack_chance = BULL_ATTACK_CHANCE * 1.5;
        }
        
        if(_turing>0){
            _turing-=dt;
            _body->SetEnabled(false);
           // _body->SetLinearVelocity(velocity);
            setPosition(getPosition()+Vec2(_direction*0.2,0));
            
            if(_turing<=0){
                setact("bullTurn", 0.75f);
                _CA=10-_CAcount;
            }
            return;
        }

        if (_CA > 0) {
            _CA -= dt;
            float t = 10-_CAcount;
            float yyy;
            if (_CA > t/2) {
                yyy = 25 * (dt / t);
                _node->setScale((0.5 + 0.5 * ((_CA-(t/2))/(t/2)))*0.5/4);
            }
            else {
                yyy = -25 * (dt / t);
                _node->setScale((1 - 0.5 * (_CA/(t/2)))*0.5/4);
            }

            setPosition(getPosition() + Vec2(-_direction * (90) * (dt / t), yyy));

            if (_CA <= 0) {
                _running = 1.3;
                setGravityScale(1);
                setact("bullTurn", 0.75f);
            }
            return;
        }
    }

    if (_direction != _lastDirection) {
        setact("bullTurn", 0.75f);
    }

    _lastDirection = _direction;
    _lastDamageTime += dt;
    _nextChangeTime -= dt;


    _body->SetLinearVelocity(velocity);
    _actionM->update(dt);
    if (_node != nullptr) {
        _node->setPosition(getPosition() * _drawScale);
        _node->setAngle(getAngle());
    }
}

void BullModel::takeDamage(float damage, int attackDirection,bool knockback) {
 
    if (_lastDamageTime >= _healthCooldown && _angrytime<=0) {
        _lastDamageTime = 0;
        _health -= damage;
        if (_health <= 0) {
            _health = 0;
        }else if (knockback) {
          //  b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE*10, BULL_KNOCKBACK_FORCE_UP * 25);
            _body->SetLinearVelocity(b2Vec2(0, 0));
          //  _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 2;
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
    std::vector<std::shared_ptr<Attack>> _attacks = scene.getattacks();

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
    attack->setDie(false);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);
    _attacks.push_back(attack);
    scene.addObstacle(attack, sprite, true);
    scene.setattacks(_attacks);
    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}

void BullModel::createAttack2(GameScene& scene) {
    float _scale = scene.getScale();
    std::vector<std::shared_ptr<Attack>> _attacks = scene.getattacks();
    std::shared_ptr<Texture> image = _assets->get<Texture>("bullStompEffect");
    std::vector<int> forward;
    for (int ii = 0; ii < 14; ii++) {
        forward.push_back(ii);
    }
    auto act = scene2::Animate::alloc(forward, 7.0f);
    
    Vec2 pos = getPosition();
    pos.x += ATTACK_OFFSET_X*6;
    pos.y -= ATTACK_OFFSET_Y * 4.5;
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(0.01*image->getSize().width / _scale,
            0.01 * image->getSize().height / _scale));

    Vec2 pos2 = pos;
    pos2.x -= ATTACK_OFFSET_X *12;
    std::shared_ptr<Attack> attack2 = Attack::alloc(pos2,
        cugl::Size(0.01*image->getSize().width / _scale,
            0.01 * image->getSize().height / _scale));

    attack->setFaceRight(true);
    attack->setName("shake");
    attack->setBullet(true);
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(false);
    attack->setGo(true);
    attack->setnorotate(true);
    attack->setDie(false);

    attack2->setFaceRight(false);
    attack2->setName("shake");
    attack2->setBullet(true);
    attack2->setGravityScale(0);
    attack2->setDebugColor(DEBUG_COLOR);
    attack2->setDrawScale(_scale);
    attack2->setEnabled(false);
    attack2->setGo(true);
    attack2->setnorotate(true);
    attack2->setDie(false);
    



    std::shared_ptr<scene2::SpriteNode> sprite = scene2::SpriteNode::allocWithSheet(image,4,4,14);
    sprite->setPosition(pos);
    sprite->setScale(0.3/4);
    attack->setSceneNode(sprite);
    _actionM->activate("s"+std::to_string(n), act, sprite);
    n+=1;
    scene.addObstacle(attack, sprite, true);

    std::shared_ptr<scene2::SpriteNode> sprite2 = scene2::SpriteNode::allocWithSheet(image,4,4,14);
    sprite2->setPosition(pos2);
    sprite2->setScale(0.3/4);
    attack2->setSceneNode(sprite2);
    _actionM->activate("s"+std::to_string(n), act, sprite2);
    n+=1;
    scene.addObstacle(attack2, sprite2, true);
    _attacks.push_back(attack);
    _attacks.push_back(attack2);
    scene.setattacks(_attacks);
    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}

void BullModel::createAttack3(GameScene& scene) {

    std::shared_ptr<DudeModel> _Su = scene.getAvatar();
    float _scale = scene.getScale();
    std::vector<std::shared_ptr<Attack>> _attacks = scene.getattacks();
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
    attack->setDie(false);


    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    scene.addObstacle(attack, sprite, true);
    _attacks.push_back(attack);
    scene.setattacks(_attacks);
    std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

}
void BullModel::Summon(GameScene& scene) {

    scene.spawnCarrot(getPosition() + Vec2(5.0f, 10.0f));
    scene.spawnCarrot(getPosition() + Vec2(-5.0f, 10.0f));

}

void BullModel::circleattack(GameScene& scene) {
    _turing=1.3;
    setGravityScale(0);
}
