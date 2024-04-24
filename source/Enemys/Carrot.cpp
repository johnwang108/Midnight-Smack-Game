#include "Carrot.h"

bool Carrot::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    return init(pos, size, scale, EnemyModel::defaultSeq(EnemyType::carrot), EnemyModel::defaultSeqAlt(EnemyType::carrot));
}

bool Carrot::init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2) {
    if (EnemyModel::init(pos, size, scale, seq1, seq2)) {
        _type = EnemyType::carrot;
        setName("carrot");
        _health = 100.0f;
        setFixedRotation(true);
        return true;
    }
    return false;
}

void Carrot::update(float dt) {
    EnemyModel::update(dt);
}

void Carrot::fixedUpdate(float step) {
    EnemyModel::fixedUpdate(step);
    b2Vec2 velocity = _body->GetLinearVelocity();

    if (_state == "chasing") {
        velocity.x = ENEMY_FORCE * _direction * 2;
    }
    else if (_state == "windup") {
        velocity.x = 0;
    }
    else if (_state == "jumping") {
        if (isGrounded()) {
            Vec2 vec = _distanceToPlayer.normalize() * 20;
            velocity.y = std::max(10.0f, vec.y);
            velocity.x = vec.x;
        }
    }
    else if (_state == "midair") {
        if (isGrounded()) {
            velocity.x = 0;
        }
    }
    else if (_state == "stunned") {
        velocity.x = 0;
    }
    else if (_state == "patrolling") {
        velocity.x = ENEMY_FORCE * _direction;
    }
    else {
        CULog("error: carrot");
        CULog(_state.c_str());
    }

    _body->SetLinearVelocity(EnemyModel::handleMovement(velocity));
}

void Carrot::setState(std::string state) {
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
        _behaviorCounter = 1.0f;
    }
    else if (state == "jumping") {
        _behaviorCounter = -1;
    }
    else if (state == "midair") {
        _behaviorCounter = -1;
    }
}

std::string Carrot::getNextState(std::string state) {
    if (state == "chasing") {
        return "windup";
    }
    else if (state == "windup") {
        return "jumping";
    }
    else if (state == "jumping") {
        if (!isGrounded()) return "midair";
        else return "jumping";
    }
    else if (state == "midair") {
        if (isGrounded()) return "windup";
        else return "midair";
    }
    else if (state == "stunned") {
        return "windup";
    }
    else if (state == "patrolling") {
        return "patrolling";
    }
}
