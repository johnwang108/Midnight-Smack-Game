#include "Rice.h"

bool Rice::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, bool isSoldier) {
    EnemyType t = isSoldier ? EnemyType::rice_soldier : EnemyType::rice;
    return init(pos, size, scale, EnemyModel::defaultSeq(t), EnemyModel::defaultSeqAlt(t), isSoldier);
}

bool Rice::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, bool isSoldier) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _isSoldier = isSoldier;
        _type = isSoldier ? EnemyType::rice_soldier : EnemyType::rice;
        _targetPosition = cugl::Vec2();
        _closeEnough = CLOSE_ENOUGH;
        std::string name = "rice";
        if (_isSoldier) name += "_soldier";
        _soldiers = std::vector<std::shared_ptr<Rice>>();
        //vary speed 
        if (_type == EnemyType::rice_soldier) {
            float randFactor = (float)(rand()) / (float)(RAND_MAX);
            _force = ENEMY_FORCE - 2.0f + randFactor * 3.0f;
            _closeEnough = CLOSE_ENOUGH + (float)(rand()) / (float)(RAND_MAX);
        }
        else {
            _force = ENEMY_FORCE - 2.0f;
        }
        setName(name);
        _health = 100.0f;
        setFixedRotation(true);
        setFriction(0.0f);
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
    else {
        if (!_isSoldier) {
            if (getState() == "pursuing") {
                for (auto& soldier : _soldiers) {
                    if (soldier->getState() == "patrolling") {
                        soldier->setState("acknowledging");
                    }
                    soldier->setTargetPosition(getPosition() + _distanceToPlayer);
                }
            }
            else if (getState() == "patrolling") {
                for (auto& soldier : _soldiers) {
                    soldier->setState("patrolling");
                    soldier->setTargetPosition(getPosition());
                }
            }
        }
        if (_state == "chasing") {
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
            if ((getActiveAction() == "riceStartWalk" || getActiveAction() == "riceWalk") && velocity.x != 0) setRequestedActionAndPrio("riceWalk", 20);
            else if ((getActiveAction() == "riceStartWalk" || getActiveAction() == "riceWalk") && velocity.x == 0) setRequestedActionAndPrio("riceEndWalk", 30);
            else if (velocity.x != 0) setRequestedActionAndPrio("riceStartWalk", 60);
            else setRequestedActionAndPrio("riceIdle", 1);

            if (_type == EnemyType::rice) {
                velocity.x = _force * SIGNUM(_distanceToPlayer.x) * 2;
            }
            else {
                velocity.x = _force * SIGNUM(_distanceToPlayer.x) * 2;
                CULog("Resulting v: %f", velocity.x);
            }
/*            if (_distanceToPlayer.length() < 0.05) {
                setState("attacking");
                velocity.x = 0;
            }
            else */if (abs(_distanceToPlayer.x) < 0.001) {
                velocity.x = 0;
            }
            std::shared_ptr<Sound> source = _assets->get<Sound>("riceRun");
            AudioEngine::get()->play("riceRun", source, false, 0.8f, false);

        
        }
        else if (_state == "attacking") {
            velocity.x = 0;
            std::shared_ptr<Sound> source = _assets->get<Sound>("staffSpin");
            AudioEngine::get()->play("staffSpin", source, false, 0.8f, false);
            setRequestedActionAndPrio("riceAttack", 80);
        }
        else if (_state == "stunned") {
            //if (!isGrounded()) {
            //    setPausedAndFrame(true, 2);
            //}
            //else {
            //    setPausedAndFrame(false, -1);
            //}
            setRequestedActionAndPrio("riceHurt", 100);
        }
        else if (_state == "patrolling") {
            if (_isSoldier) {
                float diff = _targetPosition.x - getPosition().x;
                //too far, start walking
                if (std::abs(diff) > _closeEnough * 4 && velocity.x == 0) {
                    setRequestedActionAndPrio("riceStartWalk", 30);
                    velocity.x = _force * SIGNUM(diff) * 0.75;
                }
                //chilling
                else if ((std::abs(diff) > _closeEnough || std::abs(diff) < _closeEnough) && velocity.x == 0) {
                    setRequestedActionAndPrio("riceIdle", 1);
                    velocity.x = 0;
                }
                //can stop walking
                else if (std::abs(diff) < _closeEnough && velocity.x != 0) {
                    setRequestedActionAndPrio("riceEndWalk", 30);
                    velocity.x = 0;
                }
                else {
                    setRequestedActionAndPrio("riceWalk", 20);
                    velocity.x = _force * SIGNUM(diff);
                }
            }
            else {
                velocity.x = _force * _moveDirection;
                if (getActiveAction() == "riceStartWalk" || getActiveAction() == "riceWalk" && velocity.x != 0) setRequestedActionAndPrio("riceWalk", 20);
                else if (velocity.x != 0) setRequestedActionAndPrio("riceStartWalk", 30);
                else if (getActiveAction() == "riceWalk" || getActiveAction() == "riceStartWalk") setRequestedActionAndPrio("riceEndWalk", 40);
                else setRequestedActionAndPrio("riceIdle", 1);
            }
        }
        else if (_state == "respawning") {
            setRequestedActionAndPrio("riceRespawn", 1000);
            velocity.x = 0;
        }
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));

    if (_state == "pursuing" && _type == EnemyType::rice_soldier) {
        CULog("Actual velocity: %f", _body->GetLinearVelocity().x);
	}
}

void Rice::setState(std::string state) {
    if (_state == "yelling" && state != "stunned" && !shouldDelete()) {
        setattacktime(true);
    }
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
    else if (state == "respawning") {
        _behaviorCounter = getActionDuration("riceRespawn");
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
    else if (state == "respawning") {
        return "patrolling";
    }
    return "0";
}
