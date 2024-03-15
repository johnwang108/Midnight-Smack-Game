//
//  PFGameScene.cpp
//  PlatformDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our ObstacleWorld class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design. This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together. However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden (2007).
//  This file has been refactored to support the physics changes in CUGL 2.5.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/24
//
#include "PFGameScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;

#pragma mark -
#pragma mark Level Geography

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1280
#define SCENE_HEIGHT 800

// #define SCENE_WIDTH 1024
// #define SCENE_HEIGHT 576

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 10.0/16.0
// #define SCENE_ASPECT 10.0/16.0

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   50.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  40.0f

#define INCLUDE_ROPE_BRIDGE false

#define CAMERA_FOLLOWS_PLAYER true



bool isLevel1 = true;



#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : Scene2(),
	_worldnode(nullptr),
	_debugnode(nullptr),
	_world(nullptr),
	_avatar(nullptr),
	_complete(false),
	_debug(false)
{    
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  This initializer uses the default scale.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets) {
    return init(assets,Rect(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT),Vec2(0,DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect) {
    return init(assets,rect,Vec2(0,DEFAULT_GRAVITY));
}

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * The game world is scaled so that the screen coordinates do not agree
 * with the Box2d coordinates.  The bounds are in terms of the Box2d
 * world, not the screen.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param rect      The game bounds in Box2d coordinates
 * @param gravity   The gravitational force on this Box2d world
 *
 * @return  true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, 
                     const Rect& rect, const Vec2& gravity) {
    // Initialize the scene to a locked height (iPhone X is narrow, but wide)
    Size dimen = computeActiveSize();
    SDL_ShowCursor(SDL_DISABLE);

    if (assets == nullptr) {
        return false;
    } else if (!Scene2::init(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    _input = std::make_shared<PlatformInput>();
    _input->init(getBounds());
    
    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect,gravity);
    _world->activateCollisionCallbacks(true);
    _world->onBeginContact = [this](b2Contact* contact) {
      beginContact(contact);
    };
    _world->onEndContact = [this](b2Contact* contact) {
      endContact(contact);
    };
  
    // IMPORTANT: SCALING MUST BE UNIFORM
    // This means that we cannot change the aspect ratio of the physics world
    // Shift to center if a bad fit
    _scale = dimen.width == SCENE_WIDTH ? dimen.width/rect.size.width : dimen.height/rect.size.height;
    Vec2 offset((dimen.width-SCENE_WIDTH)/2.0f,(dimen.height-SCENE_HEIGHT)/2.0f);

    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(offset);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(offset);

    _winnode = scene2::Label::allocWithText(WIN_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(dimen.width/2.0f,dimen.height/2.0f);
    _winnode->setForeground(WIN_COLOR);
    setComplete(false);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(dimen.width/2.0f,dimen.height/2.0f);
    _losenode->setForeground(LOSE_COLOR);
    setFailure(false);

    
    _leftnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(LEFT_IMAGE));
    _leftnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_MIDDLE_RIGHT);
    _leftnode->setScale(0.35f);
    _leftnode->setVisible(false);

    _rightnode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>(RIGHT_IMAGE));
    _rightnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_MIDDLE_LEFT);
    _rightnode->setScale(0.35f);
    _rightnode->setVisible(false);

    _gesturehud = scene2::Label::allocWithText("Gestures, Similarity: t tosdgodfho figjgoj ghkohko ", _assets->get<Font>(SMALL_MSG));
    _gesturehud->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _gesturehud->setScale(0.7f);
    CULog("%f", _gesturehud->getContentWidth());
    CULog("%f", _gesturehud->getWidth());
    _gesturehud->setPosition(0,50);
    _gesturehud->setForeground(LOSE_COLOR);
    _gesturehud->setVisible(true);


    _slowed = false;
    _attacks = std::vector<std::shared_ptr<Attack>>();

    _dollarnode = std::make_shared<DollarScene>();
    
    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_winnode);
    addChild(_losenode);
    addChild(_leftnode);
    addChild(_rightnode);
    addChild(_gesturehud);
    addChild(_dollarnode);
    _dollarnode->init(_assets, _input);
    _dollarnode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    //_dollarnode->setPosition(getSize().getIWidth() / 2.0f, getSize().getIHeight() / 2.0f);
    _dollarnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _dollarnode->setVisible(false);

    loadLevel(level1);

    _active = true;
    _complete = false;
    setDebug(false);
    
    // XNA nostalgia
    // Application::get()->setClearColor(Color4f::CORNFLOWER);
    Application::get()->setClearColor(Color4::YELLOW);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        _input->dispose();
        _world = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _losenode = nullptr;
        _leftnode = nullptr;
        _rightnode = nullptr;
        //_dollarnode->dispose();
        _dollarnode = nullptr;
        _complete = false;
        _debug = false;
        Scene2::dispose();
    }
}


#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
    _world->clear();
    _worldnode->removeAllChildren();
    _debugnode->removeAllChildren();
    _avatar = nullptr;
    _goalDoor = nullptr;
    _background = nullptr;

    _enemies.clear();
    _Bull= nullptr;

    setFailure(false);
    setComplete(false);

    loadLevel(level1);
}

/**
 * Lays out the game geography.
 *
 * Pay close attention to how we attach physics objects to a scene graph.
 * The simplest way is to make a subclass, like we do for the Dude.  However,
 * for simple objects you can just use a callback function to lightly couple
 * them.  This is what we do with the crates.
 *
 * This method is really, really long.  In practice, you would replace this
 * with your serialization loader, which would process a level file.
 */

//void GameScene::populate() {


//}

/**
 * Adds the physics object to the physics world and loosely couples it to the scene graph
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is to make a subclass of a physics object, like we did 
 * with dude.  The other is to use callback functions to loosely couple 
 * the two.  This function is an example of the latter.
 *
 * @param obj             The physics object to add
 * @param node            The scene graph node to attach it to
 * @param zOrder          The drawing order
 * @param useObjPosition  Whether to update the node's position to be at the object's position
 */
void GameScene::addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
                            const std::shared_ptr<cugl::scene2::SceneNode>& node,
                            bool useObjPosition) {
    // Don't add out of bounds obstacles
    if (!(_world->inBounds(obj.get()))) {
        return;
    }

    _world->addObstacle(obj);
    obj->setDebugScene(_debugnode);
    
    // Position the scene graph node (enough for static objects)
  	if (useObjPosition) {
	  	node->setPosition(obj->getPosition()*_scale);
	  }
	  _worldnode->addChild(node);
    
    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs){
            weak->setPosition(obs->getPosition()*_scale);
            weak->setAngle(obs->getAngle());
        });
    }
}


#pragma mark -
#pragma mark Physics Handling
/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void GameScene::preUpdate(float dt) {
    _input->update(dt);

    // Process the toggled key commands
    if (_input->didDebug()) { setDebug(!isDebug()); }
    if (_input->didReset()) { reset(); }
    if (_input->didExit()) {
        CULog("Shutting down");
        Application::get()->quit();
    }

    //_slowed = _input->didSlow();
    if (_input->didSlow()) {
        _slowed = !_slowed;
    }
    if (!_slowed) {
        _dollarnode->setVisible(false);

        _avatar->setMovement(_input->getHorizontal() * _avatar->getForce());
        _avatar->setJumping(_input->didJump());
        _avatar->setDash(_input->didDash());
        _avatar->applyForce(_input->getHorizontal(), _input->getVertical());
        if (_avatar->isJumping() && _avatar->isGrounded()) {
            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }
    }
    else {
        _dollarnode->setVisible(true);

        _avatar->setMovement(0);
        _avatar->setJumping(_input->didJump());
        _avatar->setDash(_input->didDash());
        _avatar->applyForce(0, 0);
    }
    Vec2 avatarPos = _avatar->getPosition();
    for (auto& enemy : _enemies) {
        if (enemy != nullptr && !enemy->isRemoved()) {
            Vec2 enemyPos = enemy->getPosition();
            float distance = avatarPos.distance(enemyPos);

            if (distance < CHASE_THRESHOLD) {
                enemy->setIsChasing(true);
                if (enemy->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > enemyPos.x) ? 1 : -1;
                    enemy->setDirection(direction);
                    enemy->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
                if (enemy->getattacktime()) {
                    enemy->createAttack(*this);
                    enemy->setattacktime(false);
                    enemy->setshooted(false);
                }
            }
            else if (distance >= CHASE_THRESHOLD && enemy->isChasing()) {
                enemy->setIsChasing(false);
            }
            if (enemy->getHealth() <= 0) {
                removeEnemy(enemy.get());
            }
            enemy->update(dt);
        }
    }
    if (_Bull != nullptr && !_Bull->isRemoved()) {
        if (!_Bull->isChasing()) {
            Vec2 BullPos = _Bull->getPosition();
            float distance = avatarPos.distance(BullPos);
            if (_Bull->getnextchangetime() < 0) {
                int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                _Bull->setDirection(direction);
                _Bull->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
            }
            if (_Bull->getHealth() <= 0) {
                _Bull->markRemoved(true);
            }
            _Bull->update(dt);
        }
    }
}

/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step  The number of fixed seconds for this step
 */
void GameScene::fixedUpdate(float step) {
    // Turn the physics engine crank.
    if (_slowed) { 
        step = step / 5;
    }
    //camera logic
    if (CAMERA_FOLLOWS_PLAYER) {
        cugl::Vec3 target = _avatar->getPosition() * _scale + _cameraOffset;
        cugl::Vec3 pos = _camera->getPosition();

        //magic number 0.2 are for smoothness
        //float smooth = std::min(0.2f, (target - pos).length());
        float smooth = 0.2;
        pos.smooth(target, step, smooth);
        //cugl::Vec3 pos = _avatar->getPosition() * _scale;
        _camera->setPosition(pos);
		_camera->update();
    }

    _world->update(step);
}
    
/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void GameScene::postUpdate(float remain) {
    // Since items may be deleted, garbage collect
    _world->garbageCollect();

    // TODO: Update this demo to support interpolation
    // We can interpolate the rope bridge and spinner as we have the data structures
    
    // I CHANGED THIS
    // _spinner->update(remain);
    // _ropebridge->update(remain);


    // Add a bullet AFTER physics allows it to hang in front
    // Otherwise, it looks like bullet appears far away
    _avatar->setShooting(_input->didFire());
    if (_avatar->isShooting()) {
        createAttack();
    }

    _gesturehud->setText(getGestureText(_input->getGestureString(), _input->getGestureSim()));


    //iterate through physics objects and delete any timed-out attacks
    //BAD CODE ALEART
    for (auto it = _attacks.begin(); it != _attacks.end();) {
        if ((*it)->killMe()) {
            removeAttack((*it).get());
            it = _attacks.erase(it);
        }
        else {
            ++it;
        }
    }
    for (auto& enemy : _enemies) {
        std::vector<std::shared_ptr<EnemyAttack>> attacks=enemy->getAttacks();
        for (auto it = attacks.begin(); it != attacks.end();) {
            if ((*it)->killMe()) {
                removeAttack((*it).get());
                it = attacks.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    // Record failure if necessary.
    if (!_failed && _avatar->getY() < 0) {
        setFailure(true);
    }

    // Reset the game if we win or lose.
   /* if (_countdown > 0) {
        _countdown--;
    } else if (_countdown == 0) {
        if (_failed == false && isLevel1) {
            isLevel1 = false;
            float x = 0.0f;
            float y = 0.0f;
            for (int i = 0; i < 8; ++i) {
                PLATFORMS[i][0] = 1.0f + x;
                PLATFORMS[i][1] = 4.0f + y;
                PLATFORMS[i][2] = 1.0f + x;
                PLATFORMS[i][3] = 2.5f + y;
                PLATFORMS[i][4] = 4.5f + x;
                PLATFORMS[i][5] = 2.5f + y;
                PLATFORMS[i][6] = 4.5f + x;
                PLATFORMS[i][7] = 4.0f + y;
                x = x + 3.5f;
                y = y + 1.5f;
            }
            PLATFORMS[8][0] = 29.0f;
            PLATFORMS[8][1] = 14.5f;
            PLATFORMS[8][2] = 29.0f;
            PLATFORMS[8][3] = 13.0f;
            PLATFORMS[8][4] = 31.0f;
            PLATFORMS[8][5] = 13.0f;
            PLATFORMS[8][6] = 31.0f;
            PLATFORMS[8][7] = 14.5f;
            PLATFORMS[9][0] = 0.0f;
            PLATFORMS[9][1] = 1.0f;
            PLATFORMS[9][2] = 0.0f;
            PLATFORMS[9][3] = 0.0f;
            PLATFORMS[9][4] = 1.0f;
            PLATFORMS[9][5] = 0.0f;
            PLATFORMS[9][6] = 1.0f;
            PLATFORMS[9][7] = 1.0f;
            GOAL_POS[0] = 29.0f;
            GOAL_POS[1] = 16.0f;
            SHRIMP_POS[0] = 14.0f,
            SHRIMP_POS[1] = 16.0f;
            RICE_POS[0] = 21.0f;
            RICE_POS[1] = 14.0f;
            EGG_POS[0] = 10.0f;
            EGG_POS[1] = 12.0f;
        }
        else if(_failed == false && !isLevel1) {
            //close game somehow
        }
        reset();
    }*/
}


/**
* Sets whether the level is completed.
*
* If true, the level will advance after a countdown
*
* @param value whether the level is completed.
*/
void GameScene::setComplete(bool value) {
    bool change = _complete != value;
	_complete = value;
	if (value && change) {
		std::shared_ptr<Sound> source = _assets->get<Sound>(WIN_MUSIC);
		AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
		_winnode->setVisible(true);
		_countdown = EXIT_COUNT;
	} else if (!value) {
		_winnode->setVisible(false);
		_countdown = -1;
	}
}

/**
 * Sets whether the level is failed.
 *
 * If true, the level will reset after a countdown
 *
 * @param value whether the level is failed.
 */
void GameScene::setFailure(bool value) {
	_failed = value;
	if (value) {
		std::shared_ptr<Sound> source = _assets->get<Sound>(LOSE_MUSIC);
        AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
		_losenode->setVisible(true);
		_countdown = EXIT_COUNT;
	} else {
		_losenode->setVisible(false);
		_countdown = -1;
	}
}


/**
 * Add a new bullet to the world and send it in the right direction.
 */
void GameScene::createAttack() {
	Vec2 pos = _avatar->getPosition();
	pos.x += (_avatar->isFacingRight() ? ATTACK_OFFSET_H : -ATTACK_OFFSET_H);
    pos.y += ATTACK_OFFSET_V;
    std::shared_ptr<Texture> image;
    if (_avatar->isFacingRight()) {
        image = _assets->get<Texture>(ATTACK_TEXTURE_R);
    }
    else {
        image = _assets->get<Texture>(ATTACK_TEXTURE_L);
    }


	std::shared_ptr<Attack> attack = Attack::alloc(pos, 
        cugl::Size(ATTACK_W * image->getSize().width / _scale, 
        ATTACK_H * image->getSize().height / _scale));
	attack->setName(ATTACK_NAME);
    attack->setDensity(HEAVY_DENSITY);
    attack->setBullet(true);
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);



	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setVisible(true);
    sprite->setPosition(pos);

	addObstacle(attack, sprite, true);

	std::shared_ptr<Sound> source = _assets->get<Sound>(PEW_EFFECT);
	AudioEngine::get()->play(PEW_EFFECT,source, false, EFFECT_VOLUME, true);

    _attacks.push_back(attack);
}

/**
 * Removes a new bullet from the world.
 *
 * @param  bullet   the bullet to remove
 */
template<typename T>
void GameScene::removeAttack(T* attack) {
  // do not attempt to remove a bullet that has already been removed
	if (attack->isRemoved()) {
		return;
	}
	_worldnode->removeChild(attack->getSceneNode());
	attack->setDebugScene(nullptr);
    attack->markRemoved(true);

	std::shared_ptr<Sound> source = _assets->get<Sound>(POP_EFFECT);
	AudioEngine::get()->play(POP_EFFECT,source,false,EFFECT_VOLUME, true);
}


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

    //See if the player collided with an enemy.

    if ((!_failed && !_complete) && ((_avatar.get() == bd1 && bd2->getName() == ENEMY_NAME) ||
        (_avatar.get() == bd2 && bd1->getName() == ENEMY_NAME))) {

        //if complete, don't fail
        setFailure(true);
    }

    for (auto& _enemy : _enemies) {
        if (!_enemy->isRemoved()) {
            if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1) ||
                (_enemy->getSensorName() == fd1 && _enemy.get() != bd2)) {
                _enemy->setGrounded(true);
            }

        }
    }
    if (_Bull!=nullptr && _Bull ->isChasing() && bd1 == _Bull.get() && bd2->getName() == WALL_NAME) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd2)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        _Bull->setIsChasing(false);
        _Bull->takeDamage(0, direction);
    }else if(_Bull != nullptr && _Bull->isChasing() && bd1->getName() == WALL_NAME && bd2 == _Bull.get()) {
        Vec2 wallPos = ((physics2::PolygonObstacle*)bd1)->getPosition();
        Vec2 bullPos = _Bull->getPosition();
        int direction = (wallPos.x > bullPos.x) ? 1 : -1;
        _Bull->setIsChasing(false);
        _Bull->takeDamage(0, direction);
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
  //  CULog("5 %d", p5);
    if (!(p1 || p2 || p3) && p4 && p5) {
        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
        _avatar->setContactingWall(false);
    }
    // Test bullet collision with enemy
    if (bd1->getName() == ATTACK_NAME && bd2->getName() == ENEMY_NAME) {
        Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
      //  removeAttack((Attack*)bd1);
        ((EnemyModel*)bd2)->takeDamage(34, direction);
    }
    else if (bd2->getName() == ATTACK_NAME && bd1->getName() == ENEMY_NAME) {
        Vec2 enemyPos = ((EnemyModel*)bd1)->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
     //   removeAttack((Attack*)bd2);
        ((EnemyModel*)bd1)->takeDamage(34, direction);
    }

    if (bd1->getName() == "enemy_attack" && bd2 == _avatar.get()) {
        Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        removeAttack((Attack*)bd1);
    }
    else if (bd2->getName() == "enemy_attack" && bd1 == _avatar.get()) {
        Vec2 enemyPos = ((EnemyModel*)bd1)->getPosition();
        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
        removeAttack((Attack*)bd2);
    }

}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width/dimen.height;
    float ratio2 = ((float)SCENE_WIDTH)/((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH/dimen.width;
    } else {
        dimen *= SCENE_HEIGHT/dimen.height;
    }
    return dimen;
}


std::string GameScene::getGestureText(std::string gest, float sim) {
    std::stringstream ss;
    ss << "Gesture: " << gest << ", " << "Similarity: " << sim;
    return ss.str();
}

void GameScene::zoomCamera(float scale) {
    _camera->setZoom(scale);
	_camera->update();

}

void GameScene::unzoomCamera() {
    _camera->setZoom(1);
    _camera->update();
}