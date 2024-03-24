#include "PFGameScene.h"
#include <box2d/b2_contact.h>

#pragma mark -
#pragma mark Collision Handling
/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door.
 *
 * @param  contact  The two bodies that collided
 */

void GameScene::beginContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();


    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);


    if (bd1->getName() == BACKGROUND_NAME || bd2->getName() == BACKGROUND_NAME) {
        return;
    }

    // Check if the player hits a wall NOT PLATFORM (not implemented for that atm)
    if ((bd1 == _avatar.get() && bd2->getName() == WALL_NAME) ||
        (bd2 == _avatar.get() && bd1->getName() == WALL_NAME)) {
        _avatar->setContactingWall(true);
        _avatar->setVX(0);
    }



    // See if we have landed on the ground.
    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) {
        _avatar->setGrounded(true);
        // Could have more than one ground
        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }

    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1) ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2)) {
                    _enemy->setGrounded(true);
            }

        }
    }
    if (_Bull != nullptr && _Bull->isChasing() && bd1 == _Bull.get() && bd2->getName() == WALL_NAME) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd2)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        _Bull->setIsChasing(false);
        _Bull->takeDamage(5, direction, true);
        popup(std::to_string(5), bullPos * _scale);
    }
    else if (_Bull != nullptr && _Bull->isChasing() && bd1->getName() == WALL_NAME && bd2 == _Bull.get()) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd1)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        _Bull->setIsChasing(false);
        _Bull->takeDamage(5, direction, true);
        popup(std::to_string(5), bullPos * _scale);
    }
    if (_Bull != nullptr && bd1 == _Bull.get() && bd2 == _avatar.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }
    else if (_Bull != nullptr && bd1 == _avatar.get() && bd2 == _Bull.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }

    if (_Bull != nullptr && bd1->getName() == ATTACK_NAME && bd2->getName() == BULL_TEXTURE && _Bull->getknockbacktime() <= 0) {
        Vec2 enemyPos = _Bull->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        if (_Bull->getHealth() == 66.0f) {
            _Bull->takeDamage(_avatar->getAttack()/4, direction, true);
            _Bull->setangrytime(4);
        }else {
            _Bull->takeDamage(_avatar->getAttack()/4, direction, false);
        }
        popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
        CULog("Bull Health: %f", _Bull->getHealth());
    }else if (_Bull != nullptr && bd2->getName() == ATTACK_NAME && bd1->getName() == BULL_TEXTURE && _Bull->getknockbacktime()<=0) {
        Vec2 enemyPos = _Bull->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        if (_Bull->getHealth() == 66.0f) {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, true);
            _Bull->setangrytime(4);
        }else {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, false);
        }
        popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
        CULog("Bull Health: %f", _Bull->getHealth());
    }
    if (bd1->getName() == "shake" && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd1)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x < enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
    }
    else if (bd2->getName() == "shake" && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd2)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x < enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
    }
    if (bd1->getName() == "enemy_attack" && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd1)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
        removeAttack((Attack*)bd1);
    }
    else if (bd2->getName() == "enemy_attack" && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd2)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
        removeAttack((Attack*)bd2);
    }

    // If we hit the "win" door, we are done
    if (!_failed && ((bd1 == _avatar.get() && bd2 == _goalDoor.get()) ||
        (bd1 == _goalDoor.get() && bd2 == _avatar.get()))) {
        setComplete(true);
    }
}


//Basically the same as removeAttack, can refactor
void GameScene::removeEnemy(EnemyModel* enemy) {
    if (enemy->isRemoved()) {
        return;
    }
    _worldnode->removeChild(enemy->getSceneNode());
    enemy->setDebugScene(nullptr);
    enemy->markRemoved(true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(POP_EFFECT);
    AudioEngine::get()->play(POP_EFFECT, source, false, EFFECT_VOLUME, true);
}

/**
 * Callback method for the start of a collision
 *
 * This method is called when two objects cease to touch.  The main use of this method
 * is to determine when the characer is NOT on the ground.  This is how we prevent
 * double jumping.
 */

void GameScene::endContact(b2Contact* contact) {
    b2Fixture* fix1 = contact->GetFixtureA();
    b2Fixture* fix2 = contact->GetFixtureB();

    b2Body* body1 = fix1->GetBody();
    b2Body* body2 = fix2->GetBody();

    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);

    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) {
        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty()) {
            _avatar->setGrounded(false);
        }
    }
    // Check if the player is no longer in contact with any walls
    bool p1 = (_avatar->getSensorName() == fd2);
    bool p2 = (bd1->getName() != WALL_NAME);
    bool p3 = (_avatar->getSensorName() == fd1);
    bool p4 = (bd2->getName() != WALL_NAME);
    bool p5 = _avatar->contactingWall();
    if (!(p1 || p2 || p3) && p4 && p5) {
        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
        _avatar->setContactingWall(false);
    }
    // Test bullet collision with enemy
    if (bd1->getName() == ATTACK_NAME && bd2->getName() == ENEMY_NAME) {
        Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        int damage = ((EnemyModel*)bd2)->getHealth();
        ((EnemyModel*)bd2)->takeDamage(_avatar->getAttack(), direction);
        damage -= ((EnemyModel*)bd2)->getHealth();
        if (damage > 0) popup(std::to_string((int)damage), enemyPos * _scale);
        if (((EnemyModel*)bd2)->getHealth() <= 50){
            ((EnemyModel*)bd2)->setVulnerable(true);
        }
    }else if (bd2->getName() == ATTACK_NAME && bd1->getName() == ENEMY_NAME) {
        Vec2 enemyPos = ((EnemyModel*)bd1)->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        int damage = ((EnemyModel*)bd1)->getHealth();
        ((EnemyModel*)bd1)->takeDamage(_avatar->getAttack(), direction);
        damage -= ((EnemyModel*)bd1)->getHealth();
        if (damage > 0) popup(std::to_string((int)damage), enemyPos * _scale);
        if (((EnemyModel*)bd1)->getHealth() <= 50) {
            ((EnemyModel*)bd1)->setVulnerable(true);
        }
    }







    if (bd1->getName() == ENEMY_NAME && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((DudeModel*)bd2)->getPosition();
        Vec2 attackerPos = ((EnemyModel*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
    }
    else if (bd2->getName() == ENEMY_NAME && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((DudeModel*)bd1)->getPosition();
        Vec2 attackerPos = ((EnemyModel*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
        _avatar->takeDamage(34, direction);
    }

}


