#include "Egg.h"

bool Egg::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
	return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::egg), EnemyModel::defaultSeqAlt(EnemyType::egg));
}

bool Egg::init(cugl::Vec2 pos, cugl::Size size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _type = EnemyType::egg;
        setName("egg");
        _health = 100.0f;
        _attacktime = false;
        setFixedRotation(true);
        return true;
    }
    return false;
}

void Egg::update(float dt) {
	EnemyModel::update(dt);
    b2Vec2 velocity = _body->GetLinearVelocity();
    if (_killMeCountdown != 0.0f) {
        setRequestedActionAndPrio("eggDeath", 1000);
    }
    else if (_state == "chasing") {
        if (velocity.x = 0) setRequestedActionAndPrio("eggIdle", 1);
        else setRequestedActionAndPrio("eggWalk", 2);
    }
    else if (_state == "stunned") {
        setRequestedActionAndPrio("eggHurt", 100);
    }
    else if (_state == "windup") {
        setRequestedActionAndPrio("eggWindup", 50);
    }
    else if (_state == "spitting") {
        setRequestedActionAndPrio("eggAttack", 51);
    }
    else if (_state == "patrolling") {
        if (velocity.x == 0) setRequestedActionAndPrio("eggIdle", 1);
	    setRequestedActionAndPrio("eggWalk", 2);
    }
    else if (_state == "short_windup") {
        int prio = 50;
        if (getActiveAction() == "eggAttack") prio = getActivePriority() + 1;
        setRequestedActionAndPrio("eggWindupQuick", prio);
    }
    else {
        CULog("error: egg");
        CULog(_state.c_str());
    }
}

void Egg::fixedUpdate(float step) {
	EnemyModel::fixedUpdate(step);
	b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        velocity.x = ENEMY_FORCE * _direction * 1.2;
    }
    else if (_state == "stunned") {
        velocity.x = 0;
    }
    else if (_state == "windup") {
        velocity.x = 0;
    }
    else if (_state == "spitting") {
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = ENEMY_FORCE * _direction * 0.75;
    }
    else if (_state == "short_windup") {
        velocity.x = 0;
    }
    else {
        CULog("error: egg");
        CULog(_state.c_str());
    }

    if (_state != "patrolling") {
        setDirection(SIGNUM(_distanceToPlayer.x));
    }
    _body->SetLinearVelocity(handleMovement(velocity));
}

b2Vec2 Egg::handleMovement(b2Vec2 vel) {
    b2Vec2 v = EnemyModel::handleMovement(vel);
    
    int frame = getSpriteNode()->getFrame();
    if (getActiveAction() == "eggWalk" && !(frame > 9 && frame < 16)) v.x = 0;
    return v;
}

std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> Egg::createAttack(std::shared_ptr<AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();
    float attackOffsetY = getHeight() * 0.4;
    pos.x += (getDirection() > 0 ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += attackOffsetY;

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE);

    Size s = Size(1.0f, 0.5f);
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        s);
        //cugl::Size(image->getSize().width / scale, ATTACK_H * image->getSize().height / scale));

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
    attack->setLifetime(attack->getLifetime() * 10);
    attack->setSpeed((attack->getSpeed()) * 0.5/ getMass());



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
        _behaviorCounter = getActionDuration("eggHurt");
    }
    else if (state == "patrollling") {
        _behaviorCounter = -1;
    }
    else if (state == "windup") {
        _behaviorCounter = getActionDuration("eggWindup");
    }
    else if (state == "spitting") {
        setattacktime(true);
        _behaviorCounter = getActionDuration("eggAttack");
    }
    else if (state == "short_windup") {
        _behaviorCounter = getActionDuration("eggWindupQuick");
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


