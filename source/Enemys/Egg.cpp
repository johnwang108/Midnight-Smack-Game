#include "Egg.h"

bool Egg::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
	return init(pos, size, scale, cugl::Spline2());
}

bool Egg::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, cugl::Spline2 limit) {
	return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::egg), EnemyModel::defaultSeqAlt(EnemyType::egg), limit);
}
/**init with gesture sequences*/
bool Egg::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
	return init(pos, size, scale, seq1, seq2, cugl::Spline2());
}

bool Egg::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, cugl::Spline2 limit) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _type = EnemyType::egg;
        _limit = limit;
        setName("egg");
        _health = 100.0f;
        setFixedRotation(true);
        return true;
    }
    return false;
}

void Egg::update(float dt) {
	EnemyModel::update(dt);
}

void Egg::fixedUpdate(float step) {
	EnemyModel::fixedUpdate(step);
	b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        velocity.x = ENEMY_FORCE * _direction * 0.5;
    }
    else if (_state == "stunned") {
        velocity.x = 0;
    }
    else if (_state == "windup") {
        velocity.x = 0;
    }
    else if (_state == "spitting") {
        velocity.x = 0;
        setattacktime(true);
    }
    else if (_state == "patrolling") {
        velocity.x = ENEMY_FORCE * _direction * 0.25;
    }
    else if (_state == "short_windup") {
        velocity.x = 0;
    }
    else {
        CULog("error: egg");
        CULog(_state.c_str());
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> Egg::createAttack(std::shared_ptr<AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE);
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(image->getSize().width / scale,
            ATTACK_H * image->getSize().height / scale));

    pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += ATTACK_OFFSET_Y;


    if (getDirection() > 0) {
        attack->setFaceRight(true);
    }
    attack->setName("enemy_attack");
    attack->setBullet(true);
    attack->setGravityScale(0);
    attack->setDebugColor(Color4::RED);
    attack->setDrawScale(scale);
    attack->setstraight(_distanceToPlayer + getPosition());
    attack->setEnabled(true);
    attack->setrand(false);
    attack->setSpeed(10.0f);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    return std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>>(attack, sprite);

    /*std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);*/
}

void Egg::setState(std::string state) {
    EnemyModel::setState(state);
    if (state == "chasing") {
        _behaviorCounter = 0;
    }
    else if (state == "stunned") {
        _behaviorCounter = -1;
    }
    else if (state == "patrollling") {
        _behaviorCounter = -1;
    }
    else if (state == "windup") {
        _behaviorCounter = 2.0f;
    }
    else if (state == "spitting") {
        _behaviorCounter = 1;
    }
    else if (state == "short_windup") {
        _behaviorCounter = 1.0f;
    }
}

std::string Egg::getNextState(std::string state) {
    if (state == "chasing") {
        if (_distanceToPlayer.length() > 12) return "chasing";
        else return "windup";
    }
    else if (state == "windup") {
        return "spitting";
    }
    else if (state == "short_windup") {
        return "spitting";
    }
    else if (state == "stunned") {
        return "chasing";
    }
    else if (state == "spitting") {
        if (_distanceToPlayer.length() > 12) return "chasing";
        else return "short_windup";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
    return 0;
}
