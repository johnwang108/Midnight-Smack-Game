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
}

void Beef::fixedUpdate(float step) {
	EnemyModel::fixedUpdate(step);
	b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        velocity.x = 0;
    }
    else if (_state == "burrowing") {
        //b2Filter filter = getFilterData();
        //filter.maskBits = 0x0000;
        //setFilterData(filter);
        //setGravityScale(0);
        //velocity.x = 0;
        ////if ((_limit.nearestPoint(getPosition()) - getPosition()).length() > BEEF_SPEED) {
        ////    velocity.y = BEEF_SPEED * SIGNUM((_limit.nearestPoint(getPosition()) - getPosition()).y);
        ////}
        //if ((projectOntoPath(getPosition()) - getPosition()).length() > BEEF_SPEED) {
        //    velocity.y = BEEF_SPEED * SIGNUM((projectOntoPath(getPosition()) - getPosition()).y);
        //}
        //else velocity.y = (projectOntoPath(getPosition()) - getPosition()).y;

        if (getHeight() > 0.2f) setHeight(getHeight() - 0.2f);
    }
    else if (_state == "tracking") {
        //cugl::Vec2 targetPos = _limit.nearestPoint(_distanceToPlayer.normalize() * std::min(_distanceToPlayer.length(), BEEF_SPEED));
        cugl::Vec2 targetPos = _distanceToPlayer.normalize() * std::min(_distanceToPlayer.length(), BEEF_SPEED);
        velocity.x = targetPos.x;
        velocity.y = targetPos.y;
    }
    else if (_state == "unburrowing") {
        velocity.x = 0;
        velocity.y = 0;
    }
    else if (_state == "attacking") {
        velocity.x = 0;
        setHeight(getHeight() + 0.2f);
    }
    else if (_state == "stunned") {
        //b2Filter filter = getFilterData();
        //filter.maskBits = 0xFFFF;
        //setFilterData(filter);
        //setGravityScale(1);
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = 0;
        velocity.y = 0;
        //setGravityScale(1);
        //b2Filter filter = getFilterData();
        //filter.maskBits = 0xFFFF;
        //setFilterData(filter);
    }
    resetDebug();
    _body->SetLinearVelocity(handleMovement(velocity));
}

b2Vec2 Beef::handleMovement(b2Vec2 velocity) {
    velocity = EnemyModel::handleMovement(velocity);
    Vec2 newVelocity = Vec2(velocity.x, velocity.y);

    //have to clamp velocity if going to leave the boundary.
    //if we're tracking, and we're on the edge of the boundary, and we try to move in that direction. Have to clamp it
    //if (_state == "tracking") {
    //    get bounds of the capsule
    //    cugl::Rect box = Rect(getPosition().x, getPosition().y, getWidth(), getHeight());
    //    cugl::Rect newBox = box + newVelocity;

    //        //detect if this movement will move us out
    //    bool upperRight = false;
    //    bool upperLeft = false;
    //    bool lowerLeft = false;
    //    bool lowerRight = false;
    //    if (!_boundaries.contains(Vec2(newBox.getMaxX(), newBox.getMaxY()))) upperRight = true;
    //    if (!_boundaries.contains(Vec2(newBox.getMinX(), newBox.getMaxY()))) upperLeft = true;
    //    if (!_boundaries.contains(Vec2(newBox.getMinX(), newBox.getMinY()))) lowerLeft = true;
    //    if (!_boundaries.contains(Vec2(newBox.getMaxX(), newBox.getMinY()))) lowerRight = true;

    //    //TODO: THIS DOESN"T WORK WELL
    //    if ((upperRight && upperLeft) || (lowerRight && lowerLeft)) newVelocity.y = 0;
    //    if ((upperLeft && lowerLeft) || (upperRight && lowerRight)) newVelocity.x = 0;

    //   
    //    
    //}

    //velocity.x = newVelocity.x;
    //velocity.y = newVelocity.y;
    return velocity;
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
        _behaviorCounter = BEEF_BURROW_TIME;
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
        if (abs(_distanceToPlayer.x) < 1.0f && _distanceToPlayer.length() < 4.0f) return "unburrowing";
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