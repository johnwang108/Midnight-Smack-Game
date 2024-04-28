#include "ShrimpRice.h"
#include "../PFGameScene.h"
using namespace cugl;


bool ShrimpRice::init(const Vec2& pos, const Size& size, float scale) {
    Size scaledSize = size;
    scaledSize.width *= SHRIMPRICE_HSHRINK;
    scaledSize.height *= SHRIMPRICE_VSHRINK;
    if (CapsuleObstacle::init(pos, scaledSize)) {
        _drawScale = scale;
        _isChasing = false;
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

        return true;
    }
    return false;
}


void ShrimpRice::update(float dt) {

    Entity::update(dt);
    if (_body == nullptr) return;



    if (_knockbackTime > 0) {
        _knockbackTime -= dt;
     //   setGravityScale(1.5);
        return;
    }

    b2Vec2 velocity = _body->GetLinearVelocity();
    if (_angry) {
        velocity.x = _direction * SHRIMPRICE_CHASE_SPEED;
    }
    else {
        velocity.x = _direction * SHRIMPRICE_CHASE_SPEED * 1.3;
	}
    

    if (_acttime<=0 && static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < _SFR_attack_chance) {
        float pa = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
       // pa=0.2;
        if(pa<=0.33){
            setact("SFRWheelofDoom", 0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        }else if(pa <= 0.66 && pa > 0.33) {
            setact("SFRWave1", 1.125);
        }
        else {
            setact("SFRJoustState1", 1.125);
            _canturn = false;
        }
        _SFR_attack_chance = 0.003f;
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
            velocity.x *= -1;
        }
        if (_acttime <= 0) {
            if (_act == "SFRWave3") {
                _timetosummon = true;
            }
            if (_act == "SFRWave1") {
                setact("SFRWave2", 10);
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
            if (_act == "SFRTurn") {
                scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
                if (image != nullptr) {
                    image->flipHorizontal(!image->isFlipHorizontal());
                }
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

    if (_direction != _lastDirection) {
        setact("SFRTurn", 0.61f);
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

void ShrimpRice::takeDamage(float damage, int attackDirection, bool knockback) {

    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage;
        _SFR_attack_chance += 0.001f;
        if (_health <= 0) {
            _health = 0;
        }
        else if (knockback) {
            b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE , BULL_KNOCKBACK_FORCE_UP * 5);
            _body->SetLinearVelocity(b2Vec2(0, 0));
            _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 4;
        }
        else {
        //    b2Vec2 impulse = b2Vec2(-attackDirection * BULL_KNOCKBACK_FORCE * 10, BULL_KNOCKBACK_FORCE_UP*3);
        //    _body->ApplyLinearImpulseToCenter(impulse, true);
            _knockbackTime = 0.5;
        }
    }
}
void ShrimpRice::Summon(GameScene& scene) {

    Vec2 enemyPos = getPosition()-Vec2(0,2);
    std::shared_ptr<EnemyModel> new_enemy;
    std::shared_ptr<EntitySpriteNode> spritenode;
    std::vector<std::shared_ptr<EnemyModel>> Enemies=scene.getEnemies();

    std::shared_ptr<Texture> image = _assets->get<Texture>("riceSoldier");
    spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
    float imageWidth = image->getWidth() / 4;
    float imageHeight = image->getHeight() / 4;
    Size singularSpriteSize = Size(imageWidth, imageHeight);
    new_enemy = Rice::allocWithConstants(enemyPos, singularSpriteSize / (5 * scene.getScale()), scene.getScale(), _assets, true);
    
    spritenode->setScale(0.2f);
    spritenode->setAnchor(Vec2(0.5, 0.35));
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    scene.addObstacle(new_enemy, spritenode);
    Enemies.push_back(new_enemy);

    scene.setEnemies(Enemies);
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
