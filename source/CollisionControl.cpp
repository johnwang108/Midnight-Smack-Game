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


 /**
 COLLISION MASKING TYPES

 Player				    = 00000001
 Wall				    = 00000010
 Enemy				    = 00000100
 Projectile			    = 00001000
 Intangibles			= 00010000
 (dying enemies, etc)
 Jump-thru platforms    = 00100000

 */

std::set<std::string> enemies({ "rice", "rice_soldier", "beef", "egg", "carrot", "shrimp" });   

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
    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1 && bd1->getName() != "attack") ||
        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2 && bd2->getName() != "attack")) {
        _avatar->setGrounded(true);

        // Could have more than one ground
        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }

    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1 && bd1->getName() != "attack") ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2) && bd2->getName() != "attack") {
                    _enemy->setGrounded(true);
            }
        }
    }

    if (_Bull != nullptr && _Bull->isChasing() && bd1 == _Bull.get() && bd2->getName() == WALL_NAME) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd2)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        if (_Bull->getCAcount() > 0 && _Bull->getCA() <= 0) {
            _Bull->setCAcount(_Bull->getCAcount() + 1);
            _Bull->circleattack(*this);
        }
        else {
            _Bull->setIsChasing(false);
            _Bull->takeDamage(0, direction, true);
            _BullactionManager->clearAllActions(_Bull->getSceneNode());
            auto bullCrash = _Bull->getAction("bullCrash");
            _Bull->animate("bullCrash");
            _BullactionManager->activate("bullCrash", bullCrash, _Bull->getSceneNode());
        }


       // popup(std::to_string(5), bullPos * _scale);
    }
    else if (_Bull != nullptr && _Bull->isChasing() && bd1->getName() == WALL_NAME && bd2 == _Bull.get()) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd1)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
       if (_Bull->getCAcount() > 0 && _Bull->getCA() <= 0) {
            _Bull->setCAcount(_Bull->getCAcount() + 1);
            _Bull->circleattack(*this);
        }
        else {
            _Bull->setIsChasing(false);
            _Bull->takeDamage(0, direction, true);
            _BullactionManager->clearAllActions(_Bull->getSceneNode());
            auto bullCrash = _Bull->getAction("bullCrash");
            _Bull->animate("bullCrash");
            _BullactionManager->activate("bullCrash", bullCrash, _Bull->getSceneNode());
        }
      //  popup(std::to_string(5), bullPos * _scale);
    }
    if (_Bull != nullptr && bd1 == _Bull.get() && bd2 == _avatar.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->addTouching();
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
        _avatar->addMeter(5.0f);

        float damage = _Bull->getHealth();

        if (_Bull->getHealth() == 74.5f) {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, true);
            _Bull->setsummoned(true);
            _Bull->setangrytime(4);
        }
        else if (_Bull->getHealth() == 40.5f) {
            _Bull->setsprintpreparetime(2);
            _Bull->setIsChasing(true);
            _Bull->setCAcount(2);
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, false);
        }
        else
        {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, false);
        }

        damage -= _Bull->getHealth();
        popup(std::to_string((int)damage), enemyPos * _scale);
        CULog("Bull Health: %f", _Bull->getHealth());
    }else if (_Bull != nullptr && bd2->getName() == ATTACK_NAME && bd1->getName() == BULL_TEXTURE && _Bull->getknockbacktime()<=0) {
        Vec2 enemyPos = _Bull->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        if (_Bull->getHealth() == 74.5f) {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, true);
            _Bull->setsummoned(true);
            _Bull->setangrytime(4);
        }
        else if (_Bull->getHealth() == 40.5f) {
            _Bull->setsprintpreparetime(2);
            _Bull->setIsChasing(true);
            _Bull->setCAcount(2);
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, false);
        }
        else
        {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, false);
        }
   
        popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
        CULog("Bull Health: %f", _Bull->getHealth());
    }
    if (bd1->getName() == "shake" && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd1)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x < enemyPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }
    else if (bd2->getName() == "shake" && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd2)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x < enemyPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }
    if (bd1->getName() == "enemy_attack" && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd1)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
        removeAttack((Attack*)bd1);
    }
    else if (bd2->getName() == "enemy_attack" && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((Attack*)bd2)->getPosition();
        Vec2 attackerPos = _avatar->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
        removeAttack((Attack*)bd2);
    }

    // Test bullet collision with enemy
    if (bd1->getName() == ATTACK_NAME && enemies.find(bd2->getName()) != enemies.end()) {
        Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        int damage = ((EnemyModel*)bd2)->getHealth();
        ((EnemyModel*)bd2)->takeDamage(_avatar->getAttack(), direction);
        damage -= ((EnemyModel*)bd2)->getHealth();

        _avatar->addMeter(5.0f);

        if (damage > 0) popup(std::to_string((int)damage), enemyPos * _scale);
        if (((EnemyModel*)bd2)->getHealth() <= 50) {
            ((EnemyModel*)bd2)->setVulnerable(true);
        }
    }

    //else if (bd2->getName() == ATTACK_NAME && bd1->getName() == ENEMY_NAME) {
    //    Vec2 enemyPos = ((EnemyModel*)bd1)->getPosition();
    //    Vec2 attackerPos = ((Attack*)bd2)->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
    //    int damage = ((EnemyModel*)bd1)->getHealth();
    //    CULog("DAMAGE");
    //    ((EnemyModel*)bd1)->takeDamage(_avatar->getAttack(), direction);
    //    damage -= ((EnemyModel*)bd1)->getHealth();

    //    _avatar->addMeter(5.0f);

    //    if (damage > 0) popup(std::to_string((int)damage), enemyPos * _scale);
    //    if (((EnemyModel*)bd1)->getHealth() <= 50) {
    //        ((EnemyModel*)bd1)->setVulnerable(true);
    //    }
    //}

    // If we hit the "win" door, we are done
    if (!_failed && ((bd1 == _avatar.get() && bd2 == _goalDoor.get()) ||
        (bd1 == _goalDoor.get() && bd2 == _avatar.get()))) {
        setComplete(true);
    }

    if (_avatar->getBodySensorName() == fd1 && enemies.find(bd2->getName()) != enemies.end()) {
        if (!((EnemyModel*)bd2)->isDying()) {
            Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
            Vec2 attackerPos = _avatar->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;

            _avatar->addTouching();
            _avatar->takeDamage(34, direction);
        }
    }
    //else if (_avatar->getBodySensorName() == fd2 && enemies.find(bd1->getName()) != enemies.end()) {
    //    Vec2 enemyPos = _avatar->getPosition();
    //    EnemyModel* enemy = (EnemyModel*)bd1;
    //    Vec2 attackerPos = enemy->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
    //    _avatar->addTouching();
    //    if (enemy->getType() == EnemyType::rice || enemy->getType() == EnemyType::rice_soldier) {
    //        enemy->setActiveAction("riceAttack");
    //    }
    //    _avatar->takeDamage(34, direction);
    //}
    
    if (_ShrimpRice != nullptr && bd1->getName() == ATTACK_NAME && bd2->getName() == "shrimpBoss" && _ShrimpRice->getknockbacktime() <= 0) {
        Vec2 enemyPos = _ShrimpRice->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        _avatar->addMeter(5.0f);

        _ShrimpRice->takeDamage(_avatar->getAttack() / 4, direction, false);

        popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
        CULog("shrimpBoss: %f", _ShrimpRice->getHealth());
    }
    else if (_ShrimpRice != nullptr && bd2->getName() == ATTACK_NAME && bd1->getName() == "shrimpBoss" && _ShrimpRice->getknockbacktime() <= 0) {
        Vec2 enemyPos = _ShrimpRice->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;

        _ShrimpRice->takeDamage(_avatar->getAttack() / 4, direction, false);

        popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
        CULog("shrimpBoss: %f", _ShrimpRice->getHealth());
    }
    if (_ShrimpRice != nullptr && bd1 == _ShrimpRice.get() && bd2 == _ShrimpRice.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _ShrimpRice->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->addTouching();
        _avatar->takeDamage(34, direction);
    }
    else if (_ShrimpRice != nullptr && bd1 == _avatar.get() && bd2 == _ShrimpRice.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _ShrimpRice->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }
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


    //Todo:: MAKE THIS BETTER ONCE BEN DONE WITH LEVEL EDITOR. FOR NOW JUST LOOP OVER ALL ENEMIES.
    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1 && bd1->getName() != "attack") ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2) && bd2->getName() != "attack") {
                _enemy->setGrounded(false);
            }
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

    if (_avatar->getBodySensorName() == fd1 && enemies.find(bd2->getName()) != enemies.end()) {
        if (!((EnemyModel*)bd2)->isDying()) {
            Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
            Vec2 attackerPos = _avatar->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
            _avatar->takeDamage(34, direction);
        }
        _avatar->removeTouching();
    }
}


