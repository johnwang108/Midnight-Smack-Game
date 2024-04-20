#include "Beef.h"

bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
	return init(pos, size, scale, cugl::Spline2());
}

bool Beef::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, cugl::Spline2 limit) {
	return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::beef), EnemyModel::defaultSeq(EnemyType::beef), limit);
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
        //todo
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
        //cugl::Vec2 targetPos = _limit.nearestPoint(_distanceToPlayer.normalize() * std::min(_distanceToPlayer.length(), BEEF_SPEED));
        /*cugl::Vec2 targetPos = _distanceToPlayer.normalize() * std::min(_distanceToPlayer.length(), BEEF_SPEED);*/
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
        velocity.x = 0;
    }
    else if (_state == "burrowing") {
        velocity.x = 0;
    }
    else if (_state == "tracking") {
        velocity.x = _direction * BEEF_SPEED;
    }
    else if (_state == "unburrowing") {
        velocity.x = 0;
        velocity.y = 0;
    }
    else if (_state == "attacking") {
        //todo: make attack hitbox way bigger
        velocity.x = 0;
    }
    else if (_state == "stunned") {
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = 0;
        velocity.y = 0;
    }
    resetDebug();
    _body->SetLinearVelocity(handleMovement(velocity));
}

b2Vec2 Beef::handleMovement(b2Vec2 velocity) {
    velocity = EnemyModel::handleMovement(velocity);
    Vec2 newVelocity = Vec2(velocity.x, velocity.y);
    return velocity;
}

void Beef::setState(std::string state) {
    EnemyModel::setState(state);
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
        if (abs(_distanceToPlayer.x) < 1.0f) return "unburrowing";
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
}