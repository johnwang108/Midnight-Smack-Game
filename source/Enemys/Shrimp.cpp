#include "Shrimp.h"

bool Shrimp::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::shrimp), EnemyModel::defaultSeqAlt(EnemyType::shrimp));
}

bool Shrimp::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _type = EnemyType::shrimp;
        setFixedRotation(false);
        setFriction(1.0f);
        setName("shrimp");
        _health = 100.0f;
        return true;
    }
    return false;
}

void Shrimp::update(float dt) {
    EnemyModel::update(dt);
}

void Shrimp::fixedUpdate(float step) {
    EnemyModel::fixedUpdate(step);
    b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
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

    }
    else {
        CULog("error: shrimp");
        CULog(_state.c_str());
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

std::string Shrimp::getNextState(std::string state) {
    if (state == "chasing") {
        return "curling";
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
        _behaviorCounter = 60;
    }
    else if (state == "uncurling") {
        _behaviorCounter = 20;
    }
    else if (state == "rolling") {
        _behaviorCounter = -1;
    }
    else if (state == "chasing") {
        _behaviorCounter = 0;
    }
    else if (state == "stunned") {
        _behaviorCounter = -1;
    }
    else if (state == "patrollling") {
        _behaviorCounter = -1;
    }
}