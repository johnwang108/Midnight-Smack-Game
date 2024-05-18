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
        setFriction(1.0f);
        return true;
    }
    return false;
}

void Carrot::update(float dt) {
    EnemyModel::update(dt);
    if (_state == "respawning") {

    }
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
        std::shared_ptr<Sound> source = _assets->get<Sound>("hand");
        AudioEngine::get()->play("hand", source, false, 0.8f, false);

    }
    else if (_state == "midair") {
        if (isGrounded() || velocity.x == 0) {
            velocity.x = 0;
            setState("windup");
        }
    }
    else if (_state == "stunned") {

    }
    else if (_state == "patrolling") {
        velocity.x = 0;
    }
    else if (_state == "respawning") {
        setTangible(false);
        velocity.x = 0;
    }
    else {
        CULog("error: carrot");
        CULog(_state.c_str());
    }

    _body->SetLinearVelocity(handleMovement(velocity));
}

b2Vec2 Carrot::handleMovement(b2Vec2 velocity) {
	b2Vec2 v = EnemyModel::handleMovement(velocity);

    return v;
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
        b2Vec2 velocity = _body->GetLinearVelocity();
        Vec2 vec = _distanceToPlayer.normalize() * 20;
        velocity.y = std::max(15.0f, vec.y);
        velocity.x = vec.x;
        _body->SetLinearVelocity(velocity);
        _behaviorCounter = 0;
    }
    else if (state == "midair") {
        _behaviorCounter = -1;
    }
    else if (state == "respawning") {
        _behaviorCounter = getActionDuration("carrotRespawn");
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
    else if (state == "respawning") {
        return "patrolling";
    }
    return 0;
}
