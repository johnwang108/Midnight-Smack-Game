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
		return true;
    }
    return false;
}

void Beef::update(float dt) {
	EnemyModel::update(dt);
}

void Beef::fixedUpdate(float step) {
	EnemyModel::fixedUpdate(step);
	b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        velocity.x = 0;
    }
    else if (_state == "burrowing") {
        b2Filter filter = getFilterData();
        filter.maskBits = 0x0000;
        setFilterData(filter);
        setGravityScale(0);
        velocity.x = 0;
        velocity.y = -4;
    }
    else if (_state == "tracking") {
        cugl::Vec2 targetPos = _limit.nearestPoint(cugl::Vec2(getPosition().x + _direction * ENEMY_FORCE * 5, getPosition().y));

        cugl::Vec2 v = targetPos - getPosition();
        velocity = b2Vec2(v.x, v.y);
    }
    else if (_state == "unburrowing") {
        velocity.x = 0;
        velocity.y = 0;
    }
    else if (_state == "attacking") {
        velocity.x = 0;
        velocity.y = 13;
    }
    else if (_state == "stunned") {
        b2Filter filter = getFilterData();
        filter.maskBits = 0xFFFF;
        setFilterData(filter);
        setGravityScale(1);
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = 0;
        velocity.y = 0;
        setGravityScale(1);
        b2Filter filter = getFilterData();
        filter.maskBits = 0xFFFF;
        setFilterData(filter);
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

void Beef::setState(std::string state) {
    EnemyModel::setState(state);
    if (state == "burrowing") {
        _behaviorCounter = BEEF_BURROW_TIME;
    }
    else if (state == "tracking") {
        _behaviorCounter = -1;
    }
    else if (state == "unburrowing") {
        _behaviorCounter = BEEF_UNBURROW_TIME;
    }
    else if (state == "attacking") {
        _behaviorCounter = BEEF_ATTACK_TIME;
    }
    else if (state == "stunned") {
        _behaviorCounter = 120;
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
        if (abs(_distanceToPlayer.x) < 1) return "unburrowing";
        else return "tracking";
    }
    else if (state == "unburrowing") {
        return "attacking";
    }
    else if (state == "attacking") {
        return "stunned";
    }
    else if (state == "stunned") {
        return "burrowing";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
}