#include "Shrimp.h"

bool Shrimp::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::shrimp), EnemyModel::defaultSeqAlt(EnemyType::shrimp));
}

bool Shrimp::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _type = EnemyType::shrimp;
        setFixedRotation(true);
        setFriction(1.0f);
        setName("shrimp");
        _health = 100.0f;
        return true;
    }
    return false;
}

void Shrimp::update(float dt) {
    EnemyModel::update(dt);
    b2Vec2 velocity = _body->GetLinearVelocity();
    if (_killMeCountdown != 0.0f) {
        if (_state == "standing" || _state == "standingRunning") setRequestedActionAndPrio("shrimpStandDeath", 1000);
		else if (_state == "curling" || _state == "rolling" || _state == "uncurling" || _state == "stunned") setRequestedActionAndPrio("shrimpRollDeath", 1000);
        else setRequestedActionAndPrio("shrimpIdleDeath", 1000);
    }
    else if (_state == "chasing") {
        setRequestedActionAndPrio("shrimpIdle", 1);
    }
    else if (_state == "standing") {
        if (getActiveAction() == "shrimpAlarm") setRequestedActionAndPrio("shrimpStand", 40);
        else setRequestedActionAndPrio("shrimpAlarm", 40);
    }
    else if (_state == "standingRunning") {
        setRequestedActionAndPrio("shrimpRun", 41);
    }
    else if (_state == "curling") {
        //jump forward is todo
        setRequestedActionAndPrio("shrimpCurl", 50);
    }
    else if (_state == "rolling") {
        setRequestedActionAndPrio("shrimpRoll", getActiveAction() == "shrimpRoll" ? 10 : getPriority() + 1);
    }
    else if (_state == "uncurling") {
        setRequestedActionAndPrio("shrimpUncurl", 30);
    }
    else if (_state == "stunned") {
        setRequestedActionAndPrio("shrimpRoll", getPriority() + 1);
    }
    else if (_state == "patrolling") {
        if (getActiveAction() == "shrimpIdle" && velocity.x != 0) setRequestedActionAndPrio("shrimpWalkStart", getPriority() + 1);
        else if (getActiveAction() == "shrimpWalkStart" && velocity.x != 0) setRequestedActionAndPrio("shrimpWalk", 30);
        else if (velocity.x == 0)setRequestedActionAndPrio("shrimpIdle", getPriority() + 1);
    }
    else {
        CULog("error: shrimp");
        CULog(_state.c_str());
    }
}

void Shrimp::fixedUpdate(float step) {
    EnemyModel::fixedUpdate(step);
    b2Vec2 velocity = _body->GetLinearVelocity();
    if (_state != "rolling" && _state != "stunned" || (_killMeCountdown !=0)) {
        setAngle(0.0f);
        setFixedRotation(true);
    }

    /*if (_state == "chasing") {
        velocity.x = ENEMY_FORCE * _direction / 3;
    }
    else if (_state == "rolling") {
        float spd = CHASE_SPEED * _direction;
        if (SIGNUM(_direction) != SIGNUM(velocity.x)) {
            spd *= 2.5;
        }
        velocity.x = velocity.x + spd;
        _body->ApplyAngularImpulse((_direction * -0.05f), true);
    }
    else if (_state == "stunned") {
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = ENEMY_FORCE * _direction;

    }*/

    //if (_state != "patrolling") CULog(_state.c_str());
    if (_state == "chasing") {
        velocity.x = ENEMY_FORCE * _direction / 3;
    }
    else if (_state == "standing") {
        velocity.x = 0;
    }
    else if (_state == "standingRunning") {
        velocity.x = ENEMY_FORCE * _direction * 2;
    }
    else if (_state == "curling") {
        //jump forward is todo

    }
    else if (_state == "rolling") {
        setFixedRotation(false);
        //float spd = SHRIMP_SPEED * _direction;
        //if (SIGNUM(_direction) != SIGNUM(velocity.x)) {
        //    spd *= 2.5;
        //}
        //_body->ApplyAngularImpulse((_direction * SHRIMP_SPEED * _body->GetMass() * - 1), true);
        //velocity.x = velocity.x + spd;

        float spd = SHRIMP_SPEED * _direction;
        if (SIGNUM(_direction) != SIGNUM(velocity.x)) {
            spd *= 2.5;
        }
        velocity.x = velocity.x + spd;
        _body->ApplyAngularImpulse((_direction * -0.05f), true);
    }
    else if (_state == "uncurling") {
        velocity.x = 0;
    }
    else if (_state == "stunned") {
        
    }
    else if (_state == "patrolling") {
        velocity.x = ENEMY_FORCE * _direction;
    }
    else {
        CULog("error: shrimp");
        CULog(_state.c_str());
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

std::string Shrimp::getNextState(std::string state) {
    if (state == "chasing") {
        return "standing";
    }
    else if (state == "standing") {
        return "standingRunning";
    }
    else if (state == "standingRunning") {
        if (_distanceToPlayer.length() < 10) return "curling";
		return "standingRunning";
    }
    else if (state == "curling") {
        return "rolling";
    }
    else if (state == "rolling") {
        return "rolling";
    }
    else if (state == "uncurling") {
        return "patrolling";
    }
    else if (state == "stunned") {
        return "rolling";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
    else return "patrolling";
}

void Shrimp::setState(std::string state) {
    EnemyModel::setState(state);
    if (state == "curling") {
        _body->ApplyLinearImpulse(b2Vec2(0, 30.0f), _body->GetWorldCenter(), true);
        _behaviorCounter = getActionDuration("shrimpCurl");
    }
    else if (state == "standing") {
        _behaviorCounter = getActionDuration("shrimpAlarm") + getActionDuration("shrimpStand");
        CULog(std::to_string(_behaviorCounter).c_str());
    }
    else if (state == "standingRunning") {
        _behaviorCounter = -1;
    }
    else if (state == "uncurling") {
        _behaviorCounter = getActionDuration("shrimpRollToIdle");
    }
    else if (state == "rolling") {
        _behaviorCounter = -1;
    }
    else if (state == "chasing") {
        _behaviorCounter = 0;
    }
    else if (state == "stunned") {
        _behaviorCounter = 2;
    }
    else if (state == "patrollling") {
        _behaviorCounter = -1;
    }
}