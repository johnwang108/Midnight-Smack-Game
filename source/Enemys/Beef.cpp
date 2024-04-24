#include "Beef.h"

bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
	return init(pos, size, scale, cugl::Spline2());
}

bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, cugl::Spline2 limit) {
	return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::beef), EnemyModel::defaultSeqAlt(EnemyType::beef), limit);
}
/**init with gesture sequences*/
bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
	return init(pos, size, scale, seq1, seq2, cugl::Spline2());
}

bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, cugl::Spline2 limit) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
		_type = EnemyType::beef;
		_limit = limit;
		setName("beef");
		_health = 100.0f;
        setFixedRotation(true);
        setFriction(1.0f);
        _defaultSize = Size(size);
        _attacked = false;
        //todo
        _isTangible = true;
        _dirtPile = nullptr;
		return true;
    }
    return false;
}

void Beef::update(float dt) {
	EnemyModel::update(dt);
    if (_state == "chasing") {
        setRequestedActionAndPrio("beefIdle", 1);
    }
    else if (_state == "burrowing") {
        int prio = 29;
        if (getActiveAction() == "beefAttack") prio = 39;
        setRequestedActionAndPrio("beefDig", prio);
    }
    else if (_state == "tracking") {
        if (getActiveAction() == "beefMove" && getActiveAction() == "beefStartMove") setRequestedActionAndPrio("beefMove", 30);
        else setRequestedActionAndPrio("beefStartMove", 31);
    }
    else if (_state == "unburrowing") {
        setRequestedActionAndPrio("beefRise", 32);
    }
    else if (_state == "attacking") {
        //todo: make attack hitbox way bigger
        setRequestedActionAndPrio("beefAttack", 33);
    }
    else if (_state == "stunned") {
        setRequestedActionAndPrio("beefHurt", 40);
    }
    else if (_state == "patrolling") {
        setRequestedActionAndPrio("beefIdle", 1);
    }
}

void Beef::fixedUpdate(float step) {
	EnemyModel::fixedUpdate(step);
	b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        setTangible(true);
        velocity.x = 0;
    }
    else if (_state == "burrowing") {
        setTangible(false);
        velocity.x = 0;
    }
    else if (_state == "tracking") {
        setTangible(false);
        velocity.x = _direction * BEEF_SPEED;
    }
    else if (_state == "unburrowing") {
        setTangible(false);
        velocity.x = 0;
    }
    else if (_state == "attacking") {
        setTangible(true);
        //todo: make attack hitbox way bigger
        velocity.x = 0;
        if (_node->getFrame() == 5 && !_attacked) {
            _attacked = true;
            setattacktime(true);
            CULog("frame 5!");
        }
    }
    else if (_state == "stunned") {
        setTangible(true);
        velocity.x = 0;
        _attacked = false;
    }
    else if (_state == "patrolling") {
        setTangible(true);
        velocity.x = 0;
        _attacked = false;
    }
    resetDebug();
    _body->SetLinearVelocity(handleMovement(velocity));
}

void Beef::setTangible(bool b) {
    _isTangible = b;
}

b2Vec2 Beef::handleMovement(b2Vec2 velocity) {
    velocity = EnemyModel::handleMovement(velocity);
    Vec2 newVelocity = Vec2(velocity.x, velocity.y);
    return velocity;
}

std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> Beef::createAttack(std::shared_ptr<AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE);
    Size size = getDimension();
    size.width *= 1.8;
    size.height *= 1.2;
    std::shared_ptr<Attack> attack = Attack::alloc(pos, size);

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
    attack->setShoot(false);
    attack->setEnabled(true);
    attack->setrand(false);
    attack->setSpeed(0.0f);
    attack->setnorotate(true);
    attack->setGo(false);
    //attack->setLifetime(attack->getLifetime() * 1.2);



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);

    return std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>>(attack, sprite);

    /*std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);*/
}

void Beef::setState(std::string state) {
    EnemyModel::setState(state);
    if (_state == "attacking") _attacked = false;
    if (state == "burrowing") {
        _behaviorCounter = getActionDuration("beefDig");
    }
    else if (state == "tracking") {
        _behaviorCounter = -1;
    }
    else if (state == "unburrowing") {
        _behaviorCounter = getActionDuration("beefRise");
    }
    else if (state == "attacking") {
        _behaviorCounter = getActionDuration("beefAttack");
    }
    else if (state == "stunned") {
        _behaviorCounter = getActionDuration("beefHurt");
    }
}

std::string Beef::getNextState(std::string state) {
    if (state == "chasing") {
        return "burrowing";
    }
    else if (state == "burrowing") {
        return "tracking";
    }
    else if (state == "tracking") {
        if (abs(_distanceToPlayer.x) < 5.0f) return "unburrowing";
        else return "tracking";
    }
    else if (state == "unburrowing") {
        return "attacking";
    }
    else if (state == "attacking") {
        return "burrowing";
    }
    else if (state == "stunned") {
        return "burrowing";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
    return 0;
}
