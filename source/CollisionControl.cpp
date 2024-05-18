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

    // points to the wall class specifically
    Wall* bdWall1 = reinterpret_cast<Wall*>(body1->GetUserData().pointer);
    Wall* bdWall2 = reinterpret_cast<Wall*>(body2->GetUserData().pointer);

    if (bd1->getName() == BACKGROUND_NAME || bd2->getName() == BACKGROUND_NAME) {
        return;
    }

    if ((enemies.find(bd1->getName()) != enemies.end() && enemies.find(bd2->getName()) != enemies.end())) return;

    if ((enemies.find(bd1->getName()) != enemies.end() && bd2->getName().find("interactable") != std::string::npos)) return;
    else if ((enemies.find(bd2->getName()) != enemies.end() && bd1->getName().find("interactable") != std::string::npos)) return;

    if ((enemies.find(bd1->getName()) != enemies.end() && bd2->getName().find("TutorialSign") != std::string::npos)) return;
    else if ((enemies.find(bd2->getName()) != enemies.end() && bd1->getName().find("TutorialSign") != std::string::npos)) return;

    if (fd1 == _avatar->getLeftSensorName() && (bdWall2->getName() == BREAKABLE_PLATFORM_NAME)) {
        // in this condition, body1 is avatar and body2 is wall type
        _avatar->setContactingLeftWall(true);
        _avatar->setVX(0);
        CULog("collision on left side");
        CULog("our body2, which is a breakable platform object, should break in theory here");
        bdWall2->setFlag(true);
        // bdWall2->applyBreaking();
    }

    if (fd2 == _avatar->getLeftSensorName() && (bdWall1->getName() == BREAKABLE_PLATFORM_NAME)) {
        // in this condition, body1 is avatar and body2 is wall type
        _avatar->setContactingLeftWall(true);
        _avatar->setVX(0);
        CULog("collision on left side");
        CULog("our body1, which is a breakable platform object, should break in theory here");
        bdWall1->setFlag(true);
        // bdWall1->applyBreaking();
    }

    if (fd1 == _avatar->getRightSensorName() && (bdWall2->getName() == BREAKABLE_PLATFORM_NAME)) {
        // in this condition, body1 is avatar and body2 is wall type
        _avatar->setContactingRightWall(true);
        _avatar->setVX(0);
        CULog("collision on right side");
        CULog("our body2, which is a breakable platform object, should break in theory here");
        bdWall2->setFlag(true);
        // bdWall2->applyBreaking();
    }

    if (fd2 == _avatar->getRightSensorName() && (bdWall1->getName() == BREAKABLE_PLATFORM_NAME)) {
        // in this condition, body1 is avatar and body2 is wall type
        _avatar->setContactingRightWall(true);
        _avatar->setVX(0);
        CULog("collision on right side");
        CULog("our body1, which is a breakable platform object, should break in theory here");
        bdWall1->setFlag(true);
        // bdWall1->applyBreaking();
    }

    // we need to see if a wall type block can intersect with the avatar and have it actually work
    /*if (fd1 == _avatar->getLeftSensorName() && (bdWall2->getName() == WALL_NAME || bdWall2->getName() == PLATFORM_NAME) ||
        (fd2 == _avatar->getLeftSensorName() && (bdWall1->getName() == WALL_NAME || bdWall1->getName() == PLATFORM_NAME))) {
        CULog("collision detected with breakable platform 1");
        _avatar->setContactingLeftWall(true);
        _avatar->setVX(0);
        bdWall1->applyBreaking();
        bdWall2->applyBreaking();
        CULog("did the wall actually break on conditional 1?");
    }*/

    //if (fd1 == _avatar->getRightSensorName() && (bdWall2->getName() == WALL_NAME || bdWall2->getName() == PLATFORM_NAME) ||
    //    (fd2 == _avatar->getRightSensorName() && (bdWall1->getName() == WALL_NAME || bdWall1->getName() == PLATFORM_NAME))) {
    //    CULog("collision detected with breakable platform 2");
    //    _avatar->setContactingRightWall(true);
    //    _avatar->setVX(0);
    //    // bdWall1->applyBreaking();
    //    // bdWall2->applyBreaking();
    //    // CULog("did the wall actually break conditional 2?");
    //}


    // Check if the player hits a wall NOT PLATFORM (not implemented for that atm)
    if (fd1 == _avatar->getLeftSensorName() && (bd2->getName() == PLATFORM_NAME || bd2->getName() == DAMAGING_PLATFORM_NAME) ||
        (fd2 == _avatar->getLeftSensorName() && (bd1->getName() == PLATFORM_NAME || bd1->getName() == DAMAGING_PLATFORM_NAME))) {
        _avatar->setContactingLeftWall(true);
        _avatar->setVX(0);
    } 
        
    if (fd1 == _avatar->getRightSensorName() && (bd2->getName() == PLATFORM_NAME || bd2->getName() == DAMAGING_PLATFORM_NAME) ||
        (fd2 == _avatar->getRightSensorName() && (bd1->getName() == PLATFORM_NAME || bd1->getName() == DAMAGING_PLATFORM_NAME))) {
        _avatar->setContactingRightWall(true);
        _avatar->setVX(0);
    }

    // See if we have landed on a breakable platform.
    if ((_avatar->getSensorName() == fd2 && bdWall1->getName() == BREAKABLE_PLATFORM_NAME) ||
        (_avatar->getSensorName() == fd1 && bdWall2->getName() == BREAKABLE_PLATFORM_NAME)) {
        _avatar->setGrounded(true);
        CULog("collision with the ground of breakable platform");
        // Could have more than one ground

        if (_avatar.get() == bd1) {
            bdWall2->setFlag(true);
        }
        else {
            bdWall1->setFlag(true);
        }

        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }
    // See if we landed on a damageable platform
    if ((_avatar->getSensorName() == fd2 && bdWall1->getName() == DAMAGING_PLATFORM_NAME) ||
        (_avatar->getSensorName() == fd1 && bdWall2->getName() == DAMAGING_PLATFORM_NAME)) {
        CULog("collision with damaging platform");
        _avatar->takePlatformDamage(20, 1);
    }

    // See if we have landed on the ground.
    if ((_avatar->getSensorName() == fd2 && bd1->getName() == PLATFORM_NAME) ||
        (_avatar->getSensorName() == fd1 && bd2->getName() == PLATFORM_NAME)) {
        _avatar->setGrounded(true);

        // Could have more than one ground
        _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    }

    if ((_avatar.get() == bd2 && bd1->getName() == "TutorialSign")) {
        ((TutorialSign*)bd1)->setPopupActive(true);
    }
    else if ((_avatar.get() == bd1 && bd2->getName() == "TutorialSign")) {
        //_interactivePopups.at(_popupIndex)->toggle();
        ((TutorialSign*)bd2)->setPopupActive(true);
    }

    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1 && bd1->getName().find("attack") == std::string::npos) ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2 && bd2->getName().find("attack") == std::string::npos)) {
                    _enemy->setGrounded(true);
            }
            for (auto& p : _platforms) {
                if((_enemy.get() == bd1 && fd2 == p->getSensorName()) ||
                    (_enemy.get() == bd2 && fd1 == p->getSensorName())) {
                    _enemy->setState("patrolling");
                    _enemy->setDirection(-_enemy->getDirection());
                }
            }
        }
    }

    for (auto& s : _stations) {
        if (s.get() == bd1 && bd2->getName() == "ingredient" ) {
            if (((Attack*)bd2)->getUp() <= 0) {
                ((Attack*)bd2)->setLifetime(20);
                ((Attack*)bd2)->setDie(true);
            }
        }
        else if (s.get() == bd2 && bd1->getName() == "ingredient") {
            if (((Attack*)bd1)->getUp() <= 0) {
                ((Attack*)bd1)->setLifetime(20);
                ((Attack*)bd1)->setDie(true);
            }
        }
    }
    if (bd1->getName() == "ingredient2" && bd2 == _avatar.get()) {
        if (((Attack*)bd1)->getUp() <= 0) {
            ((Attack*)bd1)->setLifetime(5);
            ((Attack*)bd1)->setDie(true);
        }
    }
    else if (bd2->getName() == "ingredient2" && bd1 == _avatar.get()) {
        if (((Attack*)bd2)->getUp() <= 0) {
            ((Attack*)bd2)->setLifetime(5);
            ((Attack*)bd2)->setDie(true);
        }
    }
    

    if (_Bull != nullptr && _Bull->getrunning()<=0 && _Bull->isChasing() && bd1 == _Bull.get() && bd2->getName() == PLATFORM_NAME) {
        CULog("sing");
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd2)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        if (_Bull->getCAcount() > 0 && _Bull->getCA() <= 0 && _Bull->getturing() <= 0) {
            _Bull->setCAcount(_Bull->getCAcount() + 1);
            _Bull->circleattack(*this);
        }
        else {
            _Bull->setIsChasing(false);

            pogo();
            _Bull->takeDamage(0, direction, true);
            _Bull->setact("bullCrash", 3.0f);

        }
       // popup(std::to_string(5), bullPos * _scale);
    }
    else if (_Bull != nullptr && _Bull->getrunning()<=0 && _Bull->isChasing() && bd1->getName() == PLATFORM_NAME && bd2 == _Bull.get()) {
        CULog("sing");
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd1)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
       if (_Bull->getCAcount() > 0 && _Bull->getCA() <= 0 && _Bull->getturing()<=0) {
            _Bull->setCAcount(_Bull->getCAcount() + 1);
            _Bull->circleattack(*this);
        }
        else {
            _Bull->setIsChasing(false);
            pogo();
            _Bull->takeDamage(0, direction, true);
            _Bull->setact("bullCrash", 2.0f);
        }
      //  popup(std::to_string(5), bullPos * _scale);
    }
    if (_Bull != nullptr && bd1 == _Bull.get() && bd2 == _avatar.get()) {
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        //_avatar->addTouching();
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
        pogo();

        if (_Bull->getHealth() == 74.5f) {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, true);
            _Bull->setsummoned(true);
            _Bull->setangrytime(4);
        }
        else if (_Bull->getHealth() == 40.5f) {
            _Bull->setsprintpreparetime(2);
            _Bull->setact("bullTelegraph", 2.0f);
            _Bull->setattacktype("none");
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
        pogo();
        if (_Bull->getHealth() == 74.5f) {
            _Bull->takeDamage(_avatar->getAttack() / 4, direction, true);
            _Bull->setsummoned(true);
            _Bull->setangrytime(4);
        }
        else if (_Bull->getHealth() == 40.5f) {
            _Bull->setsprintpreparetime(2);
            _Bull->setact("bullTelegraph", 2.0f);
            _Bull->setattacktype("none");
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
    //else if (bd2->getName() == "enemy_attack" && bd1 == _avatar.get()) {
    //    Vec2 enemyPos = ((Attack*)bd2)->getPosition();
    //    Vec2 attackerPos = _avatar->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
    //    _avatar->takeDamage(34, direction);
    //    removeAttack((Attack*)bd2);
    //}

    //interactable 
    if (bd1->getName().find("interactable") != std::string::npos && bd2 == _avatar.get()) {
        //((GestureInteractable*)bd1)->getSceneNode()->setColor(Color4::BLUE);
        setInteractable(((GestureInteractable*)bd1)->getId());
	}
    if (bd2->getName().find("interactable") != std::string::npos && bd1 == _avatar.get()) {
        //((GestureInteractable*)bd2)->getSceneNode()->setColor(Color4::BLUE);
        setInteractable(((GestureInteractable*)bd2)->getId());
    }

    //station hit it to remove 
    if (bd1->getName().find("interactable") != std::string::npos && bd2->getName() == ATTACK_NAME) {
        ((GestureInteractable*)bd1)->hit();
    }

    // Test bullet collision with enemy
    if (bd1->getName() == ATTACK_NAME && enemies.find(bd2->getName()) != enemies.end()) {
        if (((EnemyModel*)bd2)->isTangible()) {
            Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
            Vec2 attackerPos = ((Attack*)bd1)->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
            int damage = ((EnemyModel*)bd2)->getHealth();
            ((EnemyModel*)bd2)->takeDamage(_avatar->getAttack(), direction);
            damage -= ((EnemyModel*)bd2)->getHealth();

            pogo();

            _avatar->addMeter(5.0f);
            if (damage > 0) popup(std::to_string((int)damage), enemyPos * _scale);
            if (((EnemyModel*)bd2)->getHealth() <= 50) {
                ((EnemyModel*)bd2)->setVulnerable(true);
            }
        }
        else CULog("Intangible!");
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
        if (((EnemyModel*)bd2)->isTangible() && !((EnemyModel*)bd2)->getnocoll()) {
            Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
            Vec2 attackerPos = _avatar->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;

            _avatar->addTouching();
            _avatar->takeDamage(34, direction);
            
        }
        else CULog("Intangible!");
    }
    else if (_avatar->getBodySensorName() == fd2 && enemies.find(bd1->getName()) != enemies.end()) {
		Vec2 enemyPos = _avatar->getPosition();
		EnemyModel* enemy = (EnemyModel*)bd1;
		Vec2 attackerPos = enemy->getPosition();
		int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
		_avatar->addTouching();
		_avatar->takeDamage(34, direction);
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
    

    if (_ShrimpRice != nullptr && bd1 == _ShrimpRice.get() && bd2 == _avatar.get()) {
        if (_ShrimpRice->getact() == "SFRJoustState2" ) {
            _ShrimpRice->setact("SFRJoustState2", 0.3);
            _ShrimpRice->setparry2(true);
        }
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _ShrimpRice->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
      //  _avatar->addTouching();
        _avatar->takeDamage(34, direction);
    }
    else if (_ShrimpRice != nullptr && bd1 == _avatar.get() && bd2 == _ShrimpRice.get()) {
        if (_ShrimpRice->getact() == "SFRJoustState2" ) {
            _ShrimpRice->setact("SFRJoustState2", 0.3);
            _ShrimpRice->setparry2(true);
        }
        Vec2 avatarPos = _avatar->getPosition();
        Vec2 bullPos = _ShrimpRice->getPosition();
        int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
        _avatar->takeDamage(34, direction);
    }
    if (_ShrimpRice != nullptr && bd1->getName() == ATTACK_NAME && bd2->getName() == "shrimpBoss" && _ShrimpRice->getknockbacktime() <= 0) {
        if (_ShrimpRice->getact() == "SFRJoustState2") {
            _ShrimpRice->setparry(true);
			_ShrimpRice->setparry2(false);
        }
        else {
            Vec2 enemyPos = _ShrimpRice->getPosition();
            Vec2 attackerPos = ((Attack*)bd1)->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
            _avatar->addMeter(5.0f);
            pogo();
            _ShrimpRice->takeDamage(_avatar->getAttack() / 4, direction, false);

            popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
            CULog("shrimpBoss: %f", _ShrimpRice->getHealth());
        }

    }

    else if (_ShrimpRice != nullptr && bd2->getName() == ATTACK_NAME && bd1->getName() == "shrimpBoss" && _ShrimpRice->getknockbacktime() <= 0) {
        if (_ShrimpRice->getact() == "SFRJoustState2") {
            _ShrimpRice->setparry(true);
            _ShrimpRice->setparry2(false);
        }
        else {
            Vec2 enemyPos = _ShrimpRice->getPosition();
            Vec2 attackerPos = ((Attack*)bd2)->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
            _avatar->addMeter(5.0f);
            pogo();
            _ShrimpRice->takeDamage(_avatar->getAttack() / 4, direction, false);

            popup(std::to_string((int)_avatar->getAttack() / 4), enemyPos * _scale);
            CULog("shrimpBoss: %f", _ShrimpRice->getHealth());
        }

    }
    
    if (_ShrimpRice != nullptr&& !_enemies.empty()&& _ShrimpRice->getangrytime()>0) {
        for (auto& _enemy : _enemies) {
            if (!_enemy->isRemoved()) {
                if (( _enemy.get() == bd1 && _ShrimpRice.get() == bd2) ||
                    (_enemy.get() == bd2 && _ShrimpRice.get() == bd1)) {
                    _enemy->markForDeletion();
                    pogo();
                    _ShrimpRice->takeDamage(-10, 0, false);
                }
            }
        }
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

    if ((_avatar->getSensorName() == fd2 && (bd1->getName() == BREAKABLE_PLATFORM_NAME || bd1->getName() == PLATFORM_NAME)) ||
        (_avatar->getSensorName() == fd1 && (bd2->getName() == BREAKABLE_PLATFORM_NAME || bd2->getName() == PLATFORM_NAME))) {
        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
        if (_sensorFixtures.empty()) {
            _avatar->setGrounded(false);
        }
    }


    //Todo:: MAKE THIS BETTER ONCE BEN DONE WITH LEVEL EDITOR. FOR NOW JUST LOOP OVER ALL ENEMIES.
    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1 && bd1->getName().find("attack") == std::string::npos) ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2) && bd2->getName().find("attack") == std::string::npos) {
                _enemy->setGrounded(false);
            }
        }
    }

    //interactable 
    if (bd1->getName().find("interactable") != std::string::npos && bd2 == _avatar.get()) {
        //((GestureInteractable*)bd1)->getSceneNode()->setColor(Color4::WHITE);
        setInteractable(-1);
    }
    if (bd2->getName().find("interactable") != std::string::npos && bd1 == _avatar.get()) {
        //((GestureInteractable*)bd2)->getSceneNode()->setColor(Color4::WHITE);
        setInteractable(-1);
    }

    if ((_avatar.get() == bd2 && bd1->getName() == "TutorialSign")) {
        ((TutorialSign*)bd1)->setPopupActive(false);
    }
    else if ((_avatar.get() == bd1 && bd2->getName() == "TutorialSign")) {
        //_interactivePopups.at(_popupIndex)->toggle();
        ((TutorialSign*)bd2)->setPopupActive(false);
    }
    //// Check if the player is no longer in contact with any walls
    //bool p1 = (_avatar->getLeftSensorName() == fd1);
    //bool p2 = (_avatar->getRightSensorName() == fd1);
    //bool p3 = (bd2->getName() != WALL_NAME);
    //bool p4 = _avatar->contactingWall();
    //if (!(p1 || p2) && p3 && p4) {
    //    //_sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
    //    _avatar->setContactingWall(false);
    //}

    if (fd1 == _avatar->getLeftSensorName() && (bd2->getName() == BREAKABLE_PLATFORM_NAME || bd2->getName() == PLATFORM_NAME || bd2->getName() == DAMAGING_PLATFORM_NAME) ||
        (fd2 == _avatar->getLeftSensorName() && (bd1->getName() == BREAKABLE_PLATFORM_NAME || bd1->getName() == PLATFORM_NAME || bd1->getName() == DAMAGING_PLATFORM_NAME))) {
        _avatar->setContactingLeftWall(false);
    }

    if (fd1 == _avatar->getRightSensorName() && (bd2->getName() == BREAKABLE_PLATFORM_NAME || bd2->getName() == PLATFORM_NAME || bd2->getName() == DAMAGING_PLATFORM_NAME) ||
        (fd2 == _avatar->getRightSensorName() && (bd1->getName() == BREAKABLE_PLATFORM_NAME || bd1->getName() == PLATFORM_NAME || bd1->getName() == DAMAGING_PLATFORM_NAME))) {
        _avatar->setContactingRightWall(false);
    }

    if (_avatar->getBodySensorName() == fd1 && enemies.find(bd2->getName()) != enemies.end()) {
        if (((EnemyModel*)bd2)->isTangible() && !((EnemyModel*)bd2)->getnocoll()) {
            Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
            Vec2 attackerPos = _avatar->getPosition();
            int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
            _avatar->takeDamage(34, direction);
        }
        else CULog("Intangible!");
        _avatar->removeTouching();
    }
}
