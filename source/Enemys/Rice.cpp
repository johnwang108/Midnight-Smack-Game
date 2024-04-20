#include "Rice.h"


bool Rice::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, bool isSoldier) {
    EnemyType t = isSoldier ? EnemyType::rice_soldier : EnemyType::rice;
    return init(pos, size, scale, EnemyModel::defaultSeq(t), EnemyModel::defaultSeq(t), isSoldier);
}


bool Rice::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, bool isSoldier) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _isSoldier = isSoldier;
        _type = isSoldier ? EnemyType::rice_soldier : EnemyType::rice;
        _targetPosition = cugl::Vec2();
        _closeEnough = CLOSE_ENOUGH;
        std::string name = "rice";
        if (_isSoldier) name += "_soldier";
        CULog(name.c_str());
        setName(name);
        _health = 100.0f;
        setFixedRotation(true);
        return true;
    }
    return false;
}



//Todo:: move animation requests into update
void Rice::update(float dt) {
    //replace with enemy update
    EnemyModel::update(dt);
}

void Rice::fixedUpdate(float step) {
    EnemyModel::fixedUpdate(step);
    b2Vec2 velocity = _body->GetLinearVelocity();

    if (_killMeCountdown != 0.0f) {
        setRequestedActionAndPrio("riceDeath", 1000);
        velocity = b2Vec2(0, 0);
    }

    else if (_state == "chasing") {
        setRequestedActionAndPrio("riceIdle", 1);
    }
	else if (_state == "yelling") {
        velocity.x = 0;
        setRequestedActionAndPrio("riceYell", 50);
	}
	else if (_state == "acknowledging") {
        velocity.x = 0;
        setRequestedActionAndPrio("riceAcknowledge", 50);
	}
	else if (_state == "pursuing") {
        if (getActiveAction() == "riceStartWalk" || getActiveAction() == "riceWalk") setRequestedActionAndPrio("riceWalk", 20);
        else setRequestedActionAndPrio("riceStartWalk", 30);

        if (_distanceToPlayer.length() < 0.05) {
            setState("attacking");
            velocity.x = 0;
        }
        if (_type == EnemyType::rice) {
            velocity.x = ENEMY_FORCE * _direction * 2;
        }
        else {
            float dir = SIGNUM(_targetPosition.x - getPosition().x);
            velocity.x = ENEMY_FORCE * dir * 5;
        }
	}
	else if (_state == "attacking") {
        velocity.x = 0;
        setRequestedActionAndPrio("riceAttack", 80);
	}
	else if (_state == "stunned") {
        if (!isGrounded()) {
            setPausedAndFrame(true, 2);
        }
        else {
            setPausedAndFrame(false, -1);
        }
        setRequestedActionAndPrio("riceHurt", 100);
	}
	else if (_state == "patrolling") {

        if (_isSoldier) {
            float diff = _targetPosition.x - getPosition().x;
            //too far, start walking
            if (std::abs(diff) > CLOSE_ENOUGH * 2 && velocity.x == 0) {
                setRequestedActionAndPrio("riceStartWalk", 30);
                velocity.x = ENEMY_FORCE * SIGNUM(diff);
            }
            //chilling
            else if ((std::abs(diff) > CLOSE_ENOUGH || std::abs(diff) < CLOSE_ENOUGH) && velocity.x == 0) {
                setRequestedActionAndPrio("riceIdle", 1);
                velocity.x = 0;
            }
            //can stop walking
            else if (std::abs(diff) < CLOSE_ENOUGH && velocity.x != 0) {
                setRequestedActionAndPrio("riceEndWalk", 30);
                velocity.x = 0;
            }
            else {
                setRequestedActionAndPrio("riceWalk", 20);
                velocity.x = ENEMY_FORCE * SIGNUM(diff);
            }
        }
        else {
            if (getActiveAction() == "riceStartWalk" || getActiveAction() == "riceWalk") setRequestedActionAndPrio("riceWalk", 20);
                else setRequestedActionAndPrio("riceStartWalk", 30);
                velocity.x = ENEMY_FORCE * _direction;
        }
	}

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

void Rice::setState(std::string state) {
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
    else if (state == "yelling") {
        _behaviorCounter = getActionDuration("riceYell");
    }
    else if (state == "acknowledging") {
        _behaviorCounter = getActionDuration("riceAcknowledge");
    }
    else if (state == "pursuing") {
        _behaviorCounter = -1;
    }
    else if (state == "attacking") {
        _behaviorCounter = getActionDuration("riceAttack");
    }
}

std::string Rice::getNextState(std::string state) {
    if (state == "chasing") {
        return "yelling";
    }
    else if (state == "yelling" || state == "acknowledging") {
        return "pursuing";
    }
    else if (state == "stunned") {
        if (_body->GetLinearVelocity().x == 0) return _type == EnemyType::rice ? "yelling" : "pursuing";
        return "stunned";
    }
    else if (state == "pursuing") {
        return "pursuing";
    }
    else if (_state == "attacking") {
        if (getActiveAction() == "riceAttack") return "attacking";
        return "pursuing";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
}