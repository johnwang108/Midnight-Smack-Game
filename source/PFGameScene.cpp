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
#// include "</../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/Levels/LevelModel.h"

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace cugl;

#pragma mark -
#pragma mark Level Geography

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 6720
#define SCENE_HEIGHT 800

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 10.0/84.0
// #define SCENE_ASPECT 10.0/16.0

/** Width of the game world in Box2d units */
#define DEFAULT_WIDTH   210.0f
/** Height of the game world in Box2d units */
#define DEFAULT_HEIGHT  25.0f

#define INCLUDE_ROPE_BRIDGE false

#define CAMERA_FOLLOWS_PLAYER true

#define COOKTIME_MAX_DIST 3.5f

#define FEEDBACK_DURATION 1.2f

#define HEALTHBAR_X_OFFSET 15



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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
    return init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), input);
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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect, std::shared_ptr<PlatformInput> input) {
    return init(assets, rect, Vec2(0, DEFAULT_GRAVITY), input);
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
    const Rect& rect, const Vec2& gravity, std::shared_ptr<PlatformInput> input) {
    // Initialize the scene to a locked height (iPhone X is narrow, but wide)
    Size dimen = computeActiveSize();
    // SDL_ShowCursor(SDL_DISABLE);

    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    // Start up the input handler
    _assets = assets;

    _input = input;
    _input->init(getBounds());
    /*_input = std::make_shared<PlatformInput>();
    _input->init(getBounds());*/

    // Create the world and attach the listeners.
    _world = physics2::ObstacleWorld::alloc(rect, gravity);
    //Here, we are going to set our level's world equal to world
    // _assets->load<LevelModel>("long-level", "long-level.json");
    // _level_model = _assets->get<LevelModel>("long-level");

    // _level_model->setFilePath("long-level.json");

    /*if (_level_model == nullptr) {
        CULog("failed to load level!");
        return false;
    }*/
    // _level_model->setWorld(_world);

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
    _scale = dimen.width == SCENE_WIDTH ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);

    // Create the scene graph
    std::shared_ptr<Texture> image;
    _worldnode = scene2::SceneNode::alloc();
    _worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _worldnode->setPosition(0, 0);

    _debugnode = scene2::SceneNode::alloc();
    _debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
    _debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _debugnode->setPosition(0, 0);

    _winnode = scene2::Label::allocWithText(WIN_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _winnode->setAnchor(Vec2::ANCHOR_CENTER);
    _winnode->setPosition(0,0);
    _winnode->setForeground(WIN_COLOR);
    setComplete(false);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(0,0);
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

    /*_gestureFeedback = scene2::Label::allocWithText("Perfect", _assets->get<Font>(MESSAGE_FONT));
    _gestureFeedback->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    _gestureFeedback->setPosition(0, 0);
    _gestureFeedback->setForeground(Color4::BLACK);
    _gestureFeedback->setVisible(false);*/



    _slowed = false;
    _attacks = std::vector<std::shared_ptr<Attack>>();


    _dollarnode = std::make_shared<DollarScene>();
    //_dollarnode->init(_assets, _input, cugl::Rect(Vec2::ZERO, computeActiveSize()/2), "cooktime");
    _dollarnode->init(_assets, _input, "cooktime");
    _dollarnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _dollarnode->setVisible(false);
    _dollarnode->setNighttime(true);
    _dollarnode->setPosition(0,0);


#pragma mark: UI

    // ui stuff
    _uiScene = cugl::Scene2::alloc(Size(1280, 800));
    _uiScene->init(Size(1280, 800));
    _uiScene->setActive(true);
    // Right now we are manually adding in the json here
    // loadLevel(level1);
    std::shared_ptr<cugl::scene2::SceneNode> _meterUINode;
    _meterUINode = _assets->get<scene2::SceneNode>("night_meters");


    //auto healthBarBackground = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("heartsbroken"));
    //auto healthBarForeground = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("heartsfull"));
    //_healthBarForeground = healthBarForeground;
    //_healthBarBackground = healthBarBackground;

    //_healthBarForeground->setAnchor(Vec2::ANCHOR_MIDDLE_LEFT);
    //_healthBarForeground->setPosition(HEALTHBAR_X_OFFSET, dimen.height - _healthBarBackground->getHeight());
    //_healthBarBackground->setAnchor(Vec2::ANCHOR_MIDDLE_LEFT);
    //_healthBarBackground->setPosition(HEALTHBAR_X_OFFSET, dimen.height - _healthBarForeground->getHeight());

    // _healthBarForeground->setContentSize(_healthBarForeground->getWidth() * 3, _healthBarForeground->getHeight() * 3);
    // _healthBarBackground->setContentSize(_healthBarBackground->getWidth() * 3, _healthBarBackground->getHeight() * 3);

    _healthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("healthbar")->getChildByName("heartsfull"));
    _healthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("healthbar")->getChildByName("heartsbroken"));

    _cookBarOutline = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifeoutline"));
    _cookBarFill = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifefill"));

    _cookBarOutline = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifeoutline"));
    _cookBarFill = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifefill"));
    //_cookBarGlow = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifeglow"));
    for (std::string s : {"attackfill", "shieldfill", "speedfill", "healthfill", "jumpfill"}) {
		_cookBarIcons[s] = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("boost")->getChildByName(s));
        _cookBarIcons[s]->setVisible(false);
	}
    for (std::string s : {"attackready", "shieldready", "speedready", "healthready", "jumpready"}) {
        _cookBarIcons[s] = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("boost")->getChildByName(s));
        _cookBarIcons[s]->setVisible(false);
    }


    std::shared_ptr<cugl::scene2::SceneNode> _bullBarNode;
    _bullBarNode = _assets->get<scene2::SceneNode>("bullbar");
    _BullhealthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_bullBarNode->getChildByName("fullbullbar")->getChildByName("bullbar"));
    _BullhealthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_bullBarNode->getChildByName("fullbullbar")->getChildByName("bosshealth"));
    _uiScene->addChild(_bullBarNode);

    //std::shared_ptr<cugl::scene2::SceneNode> _SFRBarNode;
    //_SFRBarNode = _assets->get<scene2::SceneNode>("shrimpbar");

    //_SFRhealthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("fullbullbar")->getChildByName("bullbar"));
    //_SFRhealthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("fullbullbar")->getChildByName("bosshealth"));
   // _uiScene->addChild(_SFRBarNode);


    _buffLabel = scene2::Label::allocWithText("NO BUFF", _assets->get<Font>(MESSAGE_FONT));
    _buffLabel->setAnchor(Vec2::ANCHOR_CENTER);
    _buffLabel->setPosition(dimen.width - _buffLabel->getWidth()/2, dimen.height - _buffLabel->getHeight());

    /*_pauseButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("mymenubutton"));*/

    _paused = false;
    
    //_uiScene->addChild(_pauseButton);
    _uiScene->addChild(_meterUINode);
    _uiScene->addChild(_dollarnode);

    _uiScene->addChild(_winnode);
    _uiScene->addChild(_losenode);
# pragma mark: Background

    _bgScene = cugl::Scene2::alloc(dimen);
    _bgScene->init(dimen);
    _bgScene->setActive(true);
    // _bgScene = cugl::Scene2::alloc(cugl::Size(210, 25));
    // _bgScene->init(cugl::Size(210, 25));
    // _bgScene->setActive(true);

    cugl::Rect rectB = cugl::Rect(Vec2::ZERO, computeActiveSize());
    // Q: Can we create a background that isn't the whole size of the scene?
    // _bgScene->addChild(_background);
    // _background = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>("cutting_station"), rectB);
    // _background = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>(_level_model->getLevelScenery("1")));
    // _bgScene->addChild(_background);
    // _bgScene->addChild(_background);


    // _bgScene->addChild(_background);
    _bgScene->setColor(Color4::CLEAR);

    _target = std::make_shared<EnemyModel>();


    currentLevel = _level_model;
    // loadLevel(currentLevel);
    _level_model->setFilePath("json/test_level_v2_experiment.json");
    loadLevel(currentLevel);
    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_leftnode);
    addChild(_rightnode);

    //save();

    _actionManager = cugl::scene2::ActionManager::alloc();
    _BullactionManager = cugl::scene2::ActionManager::alloc();
    _SHRactionManager = cugl::scene2::ActionManager::alloc();

    _afterimages = std::vector < std::shared_ptr < scene2::SpriteNode >> ();

    //15 frame attack animation


    // i just changed level2 to _level_model
    // this will change for boss battle levels and so forth
    // currentLevel = _level_model;
    // currentLevel = level1;
    // loadLevel(_level_model);
    //App class will set active true
    setActive(false);
    transition(false);
    _active = true;
    _complete = false;
    setDebug(false);
    // zoomCamera(8.4);
    // XNA nostalgia
    // Application::get()->setClearColor(Color4f::CORNFLOWER);


    auto reader = JsonReader::alloc("./json/constants.json");

    std::shared_ptr<JsonValue> js = reader->readJson();

    _debugAnimTargetName = js->getString("entity");
    if (_debugAnimTargetName == "su") {
        _debugAnimTarget = _avatar;
    }
    else if (_debugAnimTargetName == "bull") {
        _debugAnimTarget = _Bull;
    }
    else if (_debugAnimTargetName == "shrimp") {
        _debugAnimTarget = _ShrimpRice;
    }
    else {
        _debugAnimTarget = nullptr;
    }

    _debugAnimName = js->getString("animation");
    _overrideAnim = false;

    setName("night");
    Application::get()->setClearColor(Color4::CLEAR);
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
    _worldnode->removeAllChildren();
    _world->clear();
    _debugnode->removeAllChildren();
    _avatar = nullptr;
    _goalDoor = nullptr;
    _background = nullptr;
    _sensorFixtures.clear();

    _enemies.clear();
    _vulnerables.clear();
    _Bull = nullptr;

    removeChild(_worldnode);
    removeChild(_debugnode);
    removeChild(_leftnode);
    removeChild(_rightnode);
    // removeChild(_gestureFeedback);

    setFailure(false);
    setComplete(false);

    if (_debugAnimTargetName == "bull" && _Bull != nullptr) {
        _debugAnimTarget = _Bull;
    }
    else if (_debugAnimTargetName == "shrimp" && _ShrimpRice != nullptr) {
        _debugAnimTarget = _ShrimpRice;
    }
    // loadLevel(_level_model);

    loadLevel(currentLevel);
    addChild(_worldnode);
    addChild(_debugnode);
    addChild(_leftnode);
    addChild(_rightnode);
    // addChild(_gestureFeedback);
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
        node->setPosition(obj->getPosition() * _scale);
        // node->setPosition(Vec2(500.0, 50.0));
    }
    _worldnode->addChild(node);

    // Dynamic objects need constant updating
    if (obj->getBodyType() == b2_dynamicBody) {
        scene2::SceneNode* weak = node.get(); // No need for smart pointer in callback
        obj->setListener([=](physics2::Obstacle* obs) {
            weak->setPosition(obs->getPosition() * _scale);
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
    Timestamp now = Timestamp();
    _input->update(dt);


    //ADDED 03/26
    //has the level loaded yet

    // Process the toggled key commands
    if (_input->didDebug()) { setDebug(!isDebug()); }
    if (_input->didReset()) { reset(); }
    if (_input->didExit()) {
        transition(true);
        setTarget("main_menu");
    }

    if (_input->didTransition()) {
        transition(true);
        setTarget("day");
        CULog("TTTTTTTTTTT");
        return;
    }

    //TODO handle vulnerables smarter
    if (_input->didSlow()) {
        if (_slowed) {
            //_slowed = !_slowed;
            //_target = nullptr;
        }
        //activate cooktime
        else {
            CULog("Activate!");
            float minDist = FLT_MAX;
            for (auto& e : _enemies) {
                if (e->isRemoved() || !(e->isVulnerable())) {
                    continue;
                }
                if ((e->getPosition() - _avatar->getPosition()).length() < minDist) {
                    _target = e;
                    minDist = (e->getPosition() - _avatar->getPosition()).length();
                }
            }
            if (minDist < COOKTIME_MAX_DIST) {
                _slowed = true;
                _dollarnode->setTargetGestures(_target->getGestureSeq1());
            }

        }

    }

    //handle animations
    
    //start running if idle or recovering and moving
    if (!_overrideAnim) {
        if ((_actionManager->isActive("idle") || _actionManager->isActive("recover")) && (_input->getHorizontal() != 0)) {
            _avatar->animate("run");
            auto runAction = _avatar->getAction("run");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("run", runAction, _avatar->getSceneNode());
        }
        //cancel run animation if stopped running
        if (_actionManager->isActive("run") && _input->getHorizontal() == 0) {
            _avatar->animate("idle");
            auto idleAction = _avatar->getAction("idle");
            _actionManager->activate("idle", idleAction, _avatar->getSceneNode());
        }

        if (_avatar->isJumping() && _avatar->isGrounded()) {
            _avatar->animate("jump_ready");
            auto jumpAction = _avatar->getAction("jump_ready");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_ready", jumpAction, _avatar->getSceneNode());
        }


        //animate jumps if not attacking or taking damage
        if (!_avatar->isGrounded() && !_actionManager->isActive("attack") && !_actionManager->isActive("air_attack") && _avatar->getLinearVelocity().y > 0 && (_avatar->getLastDamageTime() > _avatar->getHealthCooldown())) {
            _avatar->animate("jump_up");
            auto jumpAction = _avatar->getAction("jump_up");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_up", jumpAction, _avatar->getSceneNode());
        }
        if (!_avatar->isGrounded() && !_actionManager->isActive("attack") && !_actionManager->isActive("air_attack") && _avatar->getLinearVelocity().y < 0 && (_avatar->getLastDamageTime() > _avatar->getHealthCooldown())) {
            _avatar->animate("jump_down");
            auto jumpAction = _avatar->getAction("jump_down");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_down", jumpAction, _avatar->getSceneNode());
        }
        if (_avatar->isGrounded() && (_actionManager->isActive("jump_down") || _actionManager->isActive("jump_up"))) {
            _avatar->animate("jump_land");
            auto jumpAction = _avatar->getAction("jump_land");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_land", jumpAction, _avatar->getSceneNode());
        }


        //handle expired actions
        if (!_actionManager->isActive(_avatar->getActiveAction())) {
            if (_avatar->getActiveAction() == "attack") {
                _avatar->animate("recover");
                auto recoverAction = _avatar->getAction("recover");
                _actionManager->activate("recover", recoverAction, _avatar->getSceneNode());
            }
            else if (_avatar->getActiveAction() == "run" && _input->getHorizontal() != 0) {
                _avatar->animate("run");
                auto runAction = _avatar->getAction("run");
                _actionManager->clearAllActions(_avatar->getSceneNode());
                _actionManager->activate("run", runAction, _avatar->getSceneNode());
            }
            else {
                //Todo:: blink idle
                if (((float)rand() / RAND_MAX) < 0.0f) {
                    _avatar->animate("idle_blink");
                    auto idleAction = _avatar->getAction("idle_blink");
                    _actionManager->activate("idle_blink", idleAction, _avatar->getSceneNode());
                }
                else {
                    _avatar->animate("idle");
                    auto idleAction = _avatar->getAction("idle");
                    _actionManager->activate("idle", idleAction, _avatar->getSceneNode());
                }
            }
        }

            if (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() < _avatar->getFloatyFrames() && _avatar->getDashCooldownMax() - _avatar->getDashCooldown() != 0 && _avatar->getDashCooldown() % 3  == 1) {
				std::shared_ptr<scene2::SpriteNode> afterimage = scene2::SpriteNode::allocWithSprite(_avatar->getSpriteNode());
                afterimage->setAnchor(Vec2(0.5, 0.35));
                afterimage->setPosition(_avatar->getSceneNode()->getPosition());
                afterimage->setPositionY(afterimage->getPositionY() + 0.0f);
                Color4 b = Color4::CYAN;
                b.a = 255;
                afterimage->setColor(b);
                afterimage->setScale(_avatar->getSpriteNode()->getScale());
                _afterimages.push_back(afterimage);
                addChild(afterimage);
			}

        _avatar->setShooting(_input->didFire());
        if (_avatar->isShooting() && (!_actionManager->isActive("attack") && !_actionManager->isActive("air_attack"))) {

            CULog("CREATED ATTACK");
            auto att = _avatar->createAttack(getAssets(), _scale);
            addObstacle(std::get<0>(att), std::get<1>(att), true);
            _attacks.push_back(std::get<0>(att));

            //if (_avatar->isGrounded()) {
            auto attackAction = _avatar->getAction("attack");
            _avatar->animate("attack");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("attack", attackAction, _avatar->getSceneNode());
            //}
            //else {
            //    float horiz = _input->getHorizontal();
            //    float vert = _input->getVertical();
            //    cugl::Vec2 dir = Vec2(horiz, vert).normalize();
            //    float angle = dir.getAngle();
            //   
            //    Affine2 aff = _avatar->getSpriteNode()->getTransform();
            //    aff.rotate(angle);
            //    _avatar->getSpriteNode()->setTransform(aff);

            //    auto attackAction = _avatar->getAction("air_attack");
            //    _avatar->animate("air_attack");
            //    _actionManager->clearAllActions(_avatar->getSceneNode());
            //    _actionManager->activate("air_attack", attackAction, _avatar->getSceneNode());
            //}
        }
    }
    if (_input->didAnimate()) {
        auto reader = JsonReader::alloc("./json/constants.json");

        std::shared_ptr<JsonValue> js = reader->readJson();

        _debugAnimTargetName = js->getString("entity");
        if (_debugAnimTargetName == "su") {
            _debugAnimTarget = _avatar;
        }
        else if (_debugAnimTargetName == "bull") {
            _debugAnimTarget = _Bull;
        }
        else if (_debugAnimTargetName == "shrimp") {
            _debugAnimTarget = _ShrimpRice;
        }
        else {
            _debugAnimTarget = nullptr;
        }

        _debugAnimName = js->getString("animation");
        if (_debugAnimTarget != nullptr) {
            CULog("Overriding");
            _overrideAnim = true;
            _debugAnimTarget->animate(_debugAnimName);
            auto action = _debugAnimTarget->getAction(_debugAnimName);
            _actionManager->clearAllActions(_debugAnimTarget->getSceneNode());
            _actionManager->activate(_debugAnimName, action, _debugAnimTarget->getSceneNode());
        }
    }
    if (_overrideAnim && !_actionManager->isActive(_debugAnimName)) {
        _overrideAnim = false;
    }

    //if (!_actionManager->isActive("air_attack")) {
    //    _avatar->getSceneNode()->setAngle(0.0);
    //}
    if (_input->didBackground()) {
        
        std::shared_ptr<scene2::SceneNode> bg = getChildByName("background");

        std::shared_ptr<scene2::PolygonNode> bgNode = std::dynamic_pointer_cast<scene2::PolygonNode>(bg);
        auto reader = JsonReader::alloc("./json/constants.json");

        std::shared_ptr<JsonValue> js = reader->readJson();

        std::shared_ptr<Texture> bgTexture = _assets->get<Texture>(js->get("environment")->get("1")->getString("background"));
        bgNode->setTexture(bgTexture);
        
    }
    if (_input->didMusic()) {

        auto reader = JsonReader::alloc("./json/constants.json");

        std::shared_ptr<JsonValue> js = reader->readJson();
        
        std::shared_ptr<Sound> source = _assets->get<Sound>(js->get("environment")->get("1")->getString("music"));
        AudioEngine::get()->getMusicQueue()->clear();
        AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
    }
   

    _dollarnode->update(dt);
    if (!_slowed) {
        _dollarnode->setVisible(false);
        if (_dollarnode->isFocus()) {
            _dollarnode->setFocus(false);
            _dollarnode->setReadyToCook(false);
        }

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
        if (!(_dollarnode->isFocus())) {
            _dollarnode->setFocus(true);
            _dollarnode->setReadyToCook(true);
        }

        _avatar->setMovement(0);
        //_avatar->setJumping(_input->didJump());
        _avatar->setJumping(false);
        //_avatar->setDash(_input->didDash());
        _avatar->setDash(false);
        _avatar->applyForce(0, 0);

        //cooktime handling. Assume that _target not null, if it is null then continue
        if (!_dollarnode->isPending()) {
            if (_target != nullptr) {
                _slowed = false;
                std::string message = "";
                if (_dollarnode->getLastResult() > 0) {
                    CULog("NICE!!!!!!!!!!!!!!");
                    _target->takeDamage(_avatar->getAttack(), 0);

                    //DEFAULT: APPLY DURATION BUFF 
                    _avatar->applyBuff(EnemyModel::typeToBuff(_target->getType()), modifier::effect);
                    //set buff label
                    _buffLabel->setText(DudeModel::getStrForBuff(EnemyModel::typeToBuff(_target->getType())));
                    _buffLabel->setVisible(true);
                }
                else {
                    CULog("BOOOOOOOOOOOOOOO!!!!!!!!!!");
                }
                message = _feedbackMessages[_dollarnode->getLastResult()];
                popup(message, cugl::Vec2(_target->getPosition().x * _scale, _target->getPosition().y * 1.1 * _scale));

                _target = nullptr;
            }
            else {

            }
        }
    }

    if (_avatar->getDuration() == 0 && !_avatar->hasSuper()) {
        _buffLabel->setVisible(false);
    }

    //iterate over popups to update
    for (auto& tpl : _popups) {
        std::shared_ptr<scene2::Label> popup = std::get<0>(tpl);
        Timestamp time = std::get<1>(tpl);

        //handle removes. TODO!!!!!!
        if (now.ellapsedMillis(time) / 1000.0f >= FEEDBACK_DURATION) {
            popup->setText("");
            popup->setVisible(false);
        }
        //handle movement
        else {
            popup->setPositionY(popup->getPositionY() - 1.0f);
        }

    }


    Vec2 avatarPos = _avatar->getPosition();
    for (auto& enemy : _enemies) {
        if (enemy != nullptr && !enemy->isRemoved()) {
            Vec2 enemyPos = enemy->getPosition();
            float distance = avatarPos.distance(enemyPos);

            enemy->updatePlayerDistance(_avatar->getPosition());
			if (enemy->getattacktime()) {
				auto res = enemy->createAttack(_assets, _scale);
				addObstacle(std::get<0>(res), std::get<1>(res));
				enemy->setattacktime(false);
				enemy->setshooted(false);
			}

            std::string actionKey = enemy->getActiveAction() + enemy->getId();

            //pausing shit
            //if (enemy->getPaused() && !_actionManager->isPaused(actionKey)) {
            //    CULog("Pausing");
            //    enemy->getSpriteNode()->setFrame(enemy->getPausedFrame());
            //    _actionManager->pauseAllActions(enemy->getSceneNode());
            //}
            //else if (!enemy->getPaused() && _actionManager->isPaused(actionKey)){
            //    CULog("Unpausing");
            //    enemy->getSpriteNode()->setFrame(enemy->getActiveFrame());
            //    _actionManager->unpauseAllActions(enemy->getSceneNode());
            //}

            if ((enemy->getActiveAction() != "" && !_actionManager->isActive(actionKey)) || enemy->getPriority() > enemy->getActivePriority())
            {
                _actionManager->clearAllActions(enemy->getSceneNode());
                std::string actionName = enemy->getRequestedAction();
                enemy->animate(actionName);
                auto action = enemy->getAction(actionName);
                _actionManager->activate(actionName + enemy->getId(), action, enemy->getSceneNode());
            }
            enemy->update(dt);
          
        }
    }


        if (_Bull != nullptr && !_Bull->isRemoved()) {
            if (_Bull->getHealth() <= 0) {
                _worldnode->removeChild(_Bull->getSceneNode());
                _Bull->setDebugScene(nullptr);
                _Bull->markRemoved(true);
            }
            if (_Bull->getangrytime() > 0 && _Bull->getknockbacktime() <= 0) {
                if (int(_Bull->getangrytime() * 10) % 2 < 1) {
                    _Bull->createAttack(*this);
                }
            }
            if (_Bull->getshake() && _Bull->getknockbacktime() <= 0) {
                _Bull->setshake(false);
                _Bull->createAttack2(*this);
            }
            if (_Bull->getshoot()) {
                _Bull->setshoot(false);
                _Bull->createAttack3(*this);
            }
            if (!_Bull->isChasing()) {
                Vec2 BullPos = _Bull->getPosition();
                float distance = avatarPos.distance(BullPos);
                if (_Bull->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _Bull->setDirection(direction);
                    _Bull->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            _Bull->update(dt);
        }
        if (_ShrimpRice != nullptr && !_ShrimpRice->isRemoved()) {
            if (_ShrimpRice->getHealth() <= 0) {
                _worldnode->removeChild(_ShrimpRice->getSceneNode());
                _ShrimpRice->setDebugScene(nullptr);
                _ShrimpRice->markRemoved(true);
            }

            if (!_ShrimpRice->isChasing()) {
                Vec2 BullPos = _ShrimpRice->getPosition();
                float distance = avatarPos.distance(BullPos);
                if (_ShrimpRice->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _ShrimpRice->setDirection(direction);
                    _ShrimpRice->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            _ShrimpRice->update(dt);
        }

    if (!_overrideAnim) {
        if (_Bull != nullptr) {
            _BullactionManager->update(dt);
            if (_Bull->getangrytime() > 0) {
                if (!_BullactionManager->isActive("bullStunned")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullStunned = _Bull->getAction("bullStunned");
                    _BullactionManager->activate("bullStunned", bullStunned, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullStunned");
                }
            }
            else if (_Bull->isChasing() && ((_Bull->getPosition().x < 15 && _Bull->getDirection() == -1) || _Bull->getPosition().x > 35 && _Bull->getDirection() == 1)) {
                if (!_BullactionManager->isActive("bullAttack")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullAttack = _Bull->getAction("bullAttack");
                    _BullactionManager->activate("bullAttack", bullAttack, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullAttack");
                }
            }
            else if (_Bull->getturing() > 0) {
                if (!_BullactionManager->isActive("bullTurn")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullTurn = _Bull->getAction("bullTurn");
                    _BullactionManager->activate("bullTurn", bullTurn, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullTurn");
                }
            }
            else if (_Bull->getsprintpreparetime() <= 0 && _Bull->getknockbacktime() <= 0) {
                if (!_BullactionManager->isActive("bullRun")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullRun = _Bull->getAction("bullRun");
                    _BullactionManager->activate("bullRun", bullRun, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullRun");
                }
            }
            else if (_Bull->getsprintpreparetime() > 0 && _Bull->getknockbacktime() <= 0) {
                if (!_BullactionManager->isActive(_Bull->getattacktype())) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullTelegraph = _Bull->getAction(_Bull->getattacktype());
                    _BullactionManager->activate(_Bull->getattacktype(), bullTelegraph, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate(_Bull->getattacktype());
                }
            }



        }

        if (_ShrimpRice != nullptr) {
            _SHRactionManager->update(dt);

            if (_ShrimpRice->getattackcombo() > 0) {
                if (!_SHRactionManager->isActive("SFR_Attack")) {
                    auto SFR_Attack = _ShrimpRice->getAction("SFR_Attack");
                    _SHRactionManager->activate("SFR_Attack", SFR_Attack, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFR_Attack");
                }
            }
            else if (_ShrimpRice->getWheelofDoom() > 0) {
                if (!_SHRactionManager->isActive("SFRWheelofDoom")) {
                    auto SFRWheelofDoom = _ShrimpRice->getAction("SFRWheelofDoom");
                    _SHRactionManager->activate("SFRWheelofDoom", SFRWheelofDoom, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFRWheelofDoom");
                }
            }
            else if (_ShrimpRice->getknockbacktime() <= 0) {
                if (!_SHRactionManager->isActive("SFR_Move")) {
                    auto SFR_Move = _ShrimpRice->getAction("SFR_Move");
                    _SHRactionManager->activate("SFR_Move", SFR_Move, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFR_Move");
                }
            }

        }if (_Bull != nullptr && !_Bull->isRemoved()) {
            if (_Bull->getHealth() <= 0) {
                _worldnode->removeChild(_Bull->getSceneNode());
                _Bull->setDebugScene(nullptr);
                _Bull->markRemoved(true);
            }
            if (_Bull->getangrytime() > 0 && _Bull->getknockbacktime() <= 0) {
                if (int(_Bull->getangrytime() * 10) % 2 < 1) {
                    _Bull->createAttack(*this);
                }
            }
            if (_Bull->getshake() && _Bull->getknockbacktime() <= 0) {
                _Bull->setshake(false);
                _Bull->createAttack2(*this);
            }
            if (_Bull->getshoot()) {
                _Bull->setshoot(false);
                _Bull->createAttack3(*this);
            }
            if (!_Bull->isChasing()) {
                Vec2 BullPos = _Bull->getPosition();
                float distance = avatarPos.distance(BullPos);
                if (_Bull->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _Bull->setDirection(direction);
                    _Bull->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            _Bull->update(dt);
        }
        if (_ShrimpRice != nullptr && !_ShrimpRice->isRemoved()) {
            if (_ShrimpRice->getHealth() <= 0) {
                _worldnode->removeChild(_ShrimpRice->getSceneNode());
                _ShrimpRice->setDebugScene(nullptr);
                _ShrimpRice->markRemoved(true);
            }

            if (!_ShrimpRice->isChasing()) {
                Vec2 BullPos = _ShrimpRice->getPosition();
                float distance = avatarPos.distance(BullPos);
                if (_ShrimpRice->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _ShrimpRice->setDirection(direction);
                    _ShrimpRice->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            _ShrimpRice->update(dt);
        }

        if (_Bull != nullptr) {
            _BullactionManager->update(dt);
            if (_Bull->getangrytime() > 0) {
                if (!_BullactionManager->isActive("bullStunned")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullStunned = _Bull->getAction("bullStunned");
                    _BullactionManager->activate("bullStunned", bullStunned, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullStunned");
                }
            }
            else if (_Bull->isChasing() && ((_Bull->getPosition().x < 15 && _Bull->getDirection() == -1) || _Bull->getPosition().x > 35 && _Bull->getDirection() == 1)) {
                if (!_BullactionManager->isActive("bullAttack")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullAttack = _Bull->getAction("bullAttack");
                    _BullactionManager->activate("bullAttack", bullAttack, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullAttack");
                }
            }
            else if (_Bull->getturing() > 0) {
                if (!_BullactionManager->isActive("bullTurn")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullTurn = _Bull->getAction("bullTurn");
                    _BullactionManager->activate("bullTurn", bullTurn, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullTurn");
                }
            }
            else if (_Bull->getsprintpreparetime() <= 0 && _Bull->getknockbacktime() <= 0) {
                if (!_BullactionManager->isActive("bullRun")) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullRun = _Bull->getAction("bullRun");
                    _BullactionManager->activate("bullRun", bullRun, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate("bullRun");
                }
            }
            else if (_Bull->getsprintpreparetime() > 0 && _Bull->getknockbacktime() <= 0) {
                if (!_BullactionManager->isActive(_Bull->getattacktype())) {
                    _BullactionManager->clearAllActions(_Bull->getSceneNode());
                    auto bullTelegraph = _Bull->getAction(_Bull->getattacktype());
                    _BullactionManager->activate(_Bull->getattacktype(), bullTelegraph, _Bull->getSceneNode());
                }
                if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                    _Bull->animate(_Bull->getattacktype());
                }
            }



        }

        if (_ShrimpRice != nullptr) {
            _SHRactionManager->update(dt);

            if (_ShrimpRice->getattackcombo() > 0) {
                if (!_SHRactionManager->isActive("SFR_Attack")) {
                    auto SFR_Attack = _ShrimpRice->getAction("SFR_Attack");
                    _SHRactionManager->activate("SFR_Attack", SFR_Attack, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFR_Attack");
                }
            }
            else if (_ShrimpRice->getWheelofDoom() > 0) {
                if (!_SHRactionManager->isActive("SFRWheelofDoom")) {
                    auto SFRWheelofDoom = _ShrimpRice->getAction("SFRWheelofDoom");
                    _SHRactionManager->activate("SFRWheelofDoom", SFRWheelofDoom, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFRWheelofDoom");
                }
            }
            else if (_ShrimpRice->getknockbacktime() <= 0) {
                if (!_SHRactionManager->isActive("SFR_Move")) {
                    auto SFR_Move = _ShrimpRice->getAction("SFR_Move");
                    _SHRactionManager->activate("SFR_Move", SFR_Move, _ShrimpRice->getSceneNode());
                }
                if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                    _ShrimpRice->animate("SFR_Move");
                }
            }

        }
    }
    

    if ((_afterimages.size() > 4 || (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() > _avatar->getFloatyFrames())) && !_afterimages.empty()) {
        std::shared_ptr<scene2::SceneNode> afterimage = _afterimages.front();
        _afterimages.erase(_afterimages.begin());
        removeChild(afterimage);
    }
     
    _actionManager->update(dt);
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
    if (_healthBarForeground != nullptr) {
        _healthPercentage = _avatar->getHealth() / 100;
        float totalWidth = _healthBarForeground->getWidth();
        float height = _healthBarForeground->getHeight();
        float clipWidth = totalWidth * _healthPercentage;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, clipWidth, height));
        _healthBarForeground->setScissor(scissor);
    }
    if (_cookBarFill != nullptr) {
        float meterPercentage = _avatar->getMeter() / 100.0f;
        float totalWidth = _cookBarFill->getWidth();
        float height = _cookBarFill->getHeight();
        float clipWidth = totalWidth * meterPercentage;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, clipWidth, height));
        _cookBarFill->setScissor(scissor);
    }
    if (_BullhealthBarForeground != nullptr && _Bull != nullptr) {
        _healthPercentage = _Bull->getHealth() / 100;
        float totalWidth = _BullhealthBarForeground->getWidth();
        float height = _BullhealthBarForeground->getHeight();
        float clipWidth = totalWidth * _healthPercentage;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, clipWidth, height));
        _BullhealthBarForeground->setScissor(scissor);
    }
    if (_SFRhealthBarForeground != nullptr && _ShrimpRice != nullptr) {
        _healthPercentage = _ShrimpRice->getHealth() / 100;
        float totalWidth = _SFRhealthBarForeground->getWidth();
        float height = _SFRhealthBarForeground->getHeight();
        float clipWidth = totalWidth * _healthPercentage;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, clipWidth, height));
        _SFRhealthBarForeground->setScissor(scissor);
    }

    if (_slowed) {
        step = step / 15;
    }
    //camera logic
    // 
    // Commented out 03/27 4:30AM to check level editor
    // 
    if (CAMERA_FOLLOWS_PLAYER) {

        if (currentLevel == _level_model) {
            // we will have to not hard code this in future: WIDTH_OF_LEVEL / 40.0
            _camera->setZoom(210.0/40.0);
            // _camera->setZoom(1.5);
        }
        else if (currentLevel == level2) {
            _camera->setZoom(210.0 / 40.0);
        }
        else  if (currentLevel == level3) {
            _camera->setZoom(210.0 / 40.0);
        }
        else {
            _camera->setZoom(2.0);
        }

        cugl::Vec3 target = _avatar->getPosition() * _scale + _cameraOffset;
        //cugl::Vec3 mapMin = Vec3(SCENE_WIDTH / 2, SCENE_HEIGHT / 2, 0);
        //cugl::Vec3 mapMax = Vec3(1400 - SCENE_WIDTH / 2, 900 - SCENE_HEIGHT / 2, 0); //replace magic numbers
        /*target.clamp(mapMin, mapMax);*/

        cugl::Vec3 pos = _camera->getPosition();

        Rect viewport = _camera->getViewport();

        Vec2 worldPosition = Vec2(pos.x - viewport.size.width / 2 + 140,
            pos.y + viewport.size.height / 2 - 50);


        //magic number 0.2 are for smoothness
        //float smooth = std::min(0.2f, (target - pos).length());
        float smooth = 0.2;
        pos.smooth(target, step, smooth);
        pos = _avatar->getPosition() * _scale;
        _camera->setPosition(pos);
        _camera->update();
        //_dollarnode->setPosition(pos);
    }
    if (_avatar->getHealth() <= 0) {
        setFailure(true);
    }
    if (_Bull != nullptr && _Bull->getHealth() <= 0) {
        setComplete(true);
    }

    for (auto& enemy : _enemies) {
        if (enemy != nullptr && !enemy->isRemoved()) {
            enemy->fixedUpdate(step);
		}
        if (enemy->getHealth() <= 0) {
            enemy->markForDeletion();
        }
        if (enemy->shouldDelete()) {
            removeEnemy(enemy.get());
        }
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

    //commented out avatar stuff, 03/27 4:30 AM



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

    // Record failure if necessary.

    //COMMENTED OUT 4:30AM 03/27

    if (!_failed && _avatar->getY() < 0) {
        setFailure(true);
    }

    //Reset the game if we win or lose.
    if (_countdown > 0) {
        _countdown--;
    }
    else if (_countdown == 0) {
        if (_failed == false) {

            if (currentLevel == _level_model) {
                currentLevel = level2;
                reset();
            }
            else if (currentLevel == level3) {
                currentLevel = _level_model;
                reset();
            }
            else {
                currentLevel = _level_model;
                reset();
            }
        }
        else if (_failed) {
            reset();
        }
    }
}

void GameScene::renderBG(std::shared_ptr<cugl::SpriteBatch> batch) {
    _bgScene->render(batch);
}

void GameScene::renderUI(std::shared_ptr<cugl::SpriteBatch> batch) {
    _uiScene->render(batch);
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
    }
    else if (!value) {
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
    }
    else {
        _losenode->setVisible(false);
        _countdown = -1;
    }
}


/**
 * Add a new bullet to the world and send it in the right direction.
 */
// void GameScene::createAttack() {
    /*Vec2 pos = _avatar->getPosition();
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
        cugl::Size(0.6 * ATTACK_W * image->getSize().width / _scale,
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
    AudioEngine::get()->play(PEW_EFFECT, source, false, EFFECT_VOLUME, true);

    _attacks.push_back(attack);*/
// }

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
    AudioEngine::get()->play(POP_EFFECT, source, false, EFFECT_VOLUME, true);
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

// void GameScene::beginContact(b2Contact* contact) {
    //b2Fixture* fix1 = contact->GetFixtureA();
    //b2Fixture* fix2 = contact->GetFixtureB();

    //b2Body* body1 = fix1->GetBody();
    //b2Body* body2 = fix2->GetBody();


    //std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
    //std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);

    //physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
    //physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);


    //if (bd1->getName() == BACKGROUND_NAME || bd2->getName() == BACKGROUND_NAME) {
    //    return;
    //}

    //// Check if the player hits a wall NOT PLATFORM (not implemented for that atm)
    //if ((bd1 == _avatar.get() && bd2->getName() == WALL_NAME) ||
    //    (bd2 == _avatar.get() && bd1->getName() == WALL_NAME)) {
    //    _avatar->setContactingWall(true);
    //    _avatar->setVX(0);
    //}



    //// See if we have landed on the ground.
    //if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
    //    (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) {
    //    _avatar->setGrounded(true);
    //    // Could have more than one ground
    //    _sensorFixtures.emplace(_avatar.get() == bd1 ? fix2 : fix1);
    //}

    //for (auto& _enemy : _enemies) {
    //    if (!_enemy->isRemoved()) {
    //        if ((_enemy->getSensorName() == fd2 && _enemy.get() != bd1) ||
    //            (_enemy->getSensorName() == fd1 && _enemy.get() != bd2)) {
    //            _enemy->setGrounded(true);
    //        }

    //    }
    //}
    //if (_Bull != nullptr && _Bull->isChasing() && bd1 == _Bull.get() && bd2->getName() == WALL_NAME) {
    //    Vec2 wallPos = ((physics2::PolygonObstacle*)bd2)->getPosition();
    //    Vec2 bullPos = _Bull->getPosition();
    //    int direction = (wallPos.x > bullPos.x) ? 1 : -1;
    //    _Bull->setIsChasing(false);
    //    _Bull->takeDamage(0, direction, true);
    //}
    //else if (_Bull != nullptr && _Bull->isChasing() && bd1->getName() == WALL_NAME && bd2 == _Bull.get()) {
    //    Vec2 wallPos = ((physics2::PolygonObstacle*)bd1)->getPosition();
    //    Vec2 bullPos = _Bull->getPosition();
    //    int direction = (wallPos.x > bullPos.x) ? 1 : -1;
    //    _Bull->setIsChasing(false);
    //    _Bull->takeDamage(0, direction, true);
    //}
    //if (_Bull != nullptr && bd1 == _Bull.get() && bd2 == _avatar.get()) {
    //    Vec2 avatarPos = _avatar->getPosition();
    //    Vec2 bullPos = _Bull->getPosition();
    //    int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
    //    _avatar->takeDamage(34, direction);
    //}
    //else if (_Bull != nullptr && bd1 == _avatar.get() && bd2 == _Bull.get()) {
    //    Vec2 avatarPos = _avatar->getPosition();
    //    Vec2 bullPos = _Bull->getPosition();
    //    int direction = (avatarPos.x > bullPos.x) ? 1 : -1;
    //    _avatar->takeDamage(34, direction);
    //}

    //if (_Bull != nullptr && bd1->getName() == ATTACK_NAME && bd2->getName() == BULL_TEXTURE && _Bull->getknockbacktime() <= 0) {
    //    Vec2 enemyPos = _Bull->getPosition();
    //    Vec2 attackerPos = ((Attack*)bd1)->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
    //    if (_Bull->getHealth() == 66.0f) {
    //        _Bull->takeDamage(17, direction, true);
    //        _Bull->setangrytime(4);
    //    }
    //    else {
    //        _Bull->takeDamage(17, direction, false);
    //    }
    //    CULog("Bull Health: %f", _Bull->getHealth());
    //}
    //else if (_Bull != nullptr && bd2->getName() == ATTACK_NAME && bd1->getName() == BULL_TEXTURE && _Bull->getknockbacktime() <= 0) {
    //    Vec2 enemyPos = _Bull->getPosition();
    //    Vec2 attackerPos = ((Attack*)bd2)->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
    //    if (_Bull->getHealth() == 66.0f) {
    //        _Bull->takeDamage(17, direction, true);
    //        _Bull->setangrytime(4);
    //    }
    //    else {
    //        _Bull->takeDamage(17, direction, false);
    //    }
    //    CULog("Bull Health: %f", _Bull->getHealth());
    //}
    //if (bd1->getName() == "shake" && bd2 == _avatar.get()) {
    //    Vec2 enemyPos = ((Attack*)bd1)->getPosition();
    //    Vec2 attackerPos = _avatar->getPosition();
    //    int direction = (attackerPos.x < enemyPos.x) ? -1 : 1;
    //    _avatar->takeDamage(34, direction);
    //}
    //else if (bd2->getName() == "shake" && bd1 == _avatar.get()) {
    //    Vec2 enemyPos = ((Attack*)bd2)->getPosition();
    //    Vec2 attackerPos = _avatar->getPosition();
    //    int direction = (attackerPos.x < enemyPos.x) ? -1 : 1;
    //    _avatar->takeDamage(34, direction);
    //}
    //if (bd1->getName() == "enemy_attack" && bd2 == _avatar.get()) {
    //    Vec2 enemyPos = ((EnemyAttack*)bd1)->getPosition();
    //    Vec2 attackerPos = _avatar->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
    //    _avatar->takeDamage(34, direction);
    //    removeAttack((EnemyAttack*)bd1);
    //}
    //else if (bd2->getName() == "enemy_attack" && bd1 == _avatar.get()) {
    //    Vec2 enemyPos = ((EnemyAttack*)bd2)->getPosition();
    //    Vec2 attackerPos = _avatar->getPosition();
    //    int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
    //    _avatar->takeDamage(34, direction);
    //    removeAttack((EnemyAttack*)bd2);
    //}

    //// If we hit the "win" door, we are done
    //if (!_failed && ((bd1 == _avatar.get() && bd2 == _goalDoor.get()) ||
    //    (bd1 == _goalDoor.get() && bd2 == _avatar.get()))) {
    //    setComplete(true);
    //}
// }


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

//void GameScene::endContact(b2Contact* contact) {
//    b2Fixture* fix1 = contact->GetFixtureA();
//    b2Fixture* fix2 = contact->GetFixtureB();
//
//    b2Body* body1 = fix1->GetBody();
//    b2Body* body2 = fix2->GetBody();
//
//    std::string* fd1 = reinterpret_cast<std::string*>(fix1->GetUserData().pointer);
//    std::string* fd2 = reinterpret_cast<std::string*>(fix2->GetUserData().pointer);
//
//    physics2::Obstacle* bd1 = reinterpret_cast<physics2::Obstacle*>(body1->GetUserData().pointer);
//    physics2::Obstacle* bd2 = reinterpret_cast<physics2::Obstacle*>(body2->GetUserData().pointer);
//
//    if ((_avatar->getSensorName() == fd2 && _avatar.get() != bd1) ||
//        (_avatar->getSensorName() == fd1 && _avatar.get() != bd2)) {
//        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
//        if (_sensorFixtures.empty()) {
//            _avatar->setGrounded(false);
//        }
//    }
//    // Check if the player is no longer in contact with any walls
//    bool p1 = (_avatar->getSensorName() == fd2);
//    bool p2 = (bd1->getName() != WALL_NAME);
//    bool p3 = (_avatar->getSensorName() == fd1);
//    bool p4 = (bd2->getName() != WALL_NAME);
//    bool p5 = _avatar->contactingWall();
//    if (!(p1 || p2 || p3) && p4 && p5) {
//        _sensorFixtures.erase(_avatar.get() == bd1 ? fix2 : fix1);
//        _avatar->setContactingWall(false);
//    }
//    // Test bullet collision with enemy
//    if (bd1->getName() == ATTACK_NAME && bd2->getName() == ENEMY_NAME) {
//        Vec2 enemyPos = ((EnemyModel*)bd2)->getPosition();
//        Vec2 attackerPos = ((Attack*)bd1)->getPosition();
//        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
//        ((EnemyModel*)bd2)->takeDamage(34, direction);
//        if (((EnemyModel*)bd2)->getHealth() <= 50) {
//            ((EnemyModel*)bd2)->setVulnerable(true);
//        }
//    }
//    else if (bd2->getName() == ATTACK_NAME && bd1->getName() == ENEMY_NAME) {
//        Vec2 enemyPos = ((EnemyModel*)bd1)->getPosition();
//        Vec2 attackerPos = ((Attack*)bd2)->getPosition();
//        int direction = (attackerPos.x > enemyPos.x) ? 1 : -1;
//        ((EnemyModel*)bd1)->takeDamage(34, direction);
//        if (((EnemyModel*)bd1)->getHealth() <= 50) {
//            ((EnemyModel*)bd1)->setVulnerable(true);
//        }
//    }
//
//
//
//
//
//
//
//    if (bd1->getName() == ENEMY_NAME && bd2 == _avatar.get()) {
//        Vec2 enemyPos = ((DudeModel*)bd2)->getPosition();
//        Vec2 attackerPos = ((EnemyModel*)bd1)->getPosition();
//        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
//        _avatar->takeDamage(34, direction);
//    }
//    else if (bd2->getName() == ENEMY_NAME && bd1 == _avatar.get()) {
//        Vec2 enemyPos = ((DudeModel*)bd1)->getPosition();
//        Vec2 attackerPos = ((EnemyModel*)bd2)->getPosition();
//        int direction = (attackerPos.x > enemyPos.x) ? -1 : 1;
//        _avatar->takeDamage(34, direction);
//    }
//
//}

/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)SCENE_WIDTH) / ((float)SCENE_HEIGHT);
    if (ratio1 < ratio2) {
        dimen *= SCENE_WIDTH / dimen.width;
    }
    else {
        dimen *= SCENE_HEIGHT / dimen.height;
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


//Marks transitioning between cooking and platforming. Call this method with t = true when you want to transition away from this scene
void GameScene::transition(bool t) {
    _transitionScenes = t;
}

void GameScene::popup(std::string s, cugl::Vec2 pos) {
    Timestamp now = Timestamp();
    now.mark();
    std::shared_ptr<cugl::scene2::Label> popup = cugl::scene2::Label::allocWithText(pos, s, _assets->get<Font>(MESSAGE_FONT));

    popup = scene2::Label::allocWithText(s, _assets->get<Font>(MESSAGE_FONT));
    popup->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    popup->setForeground(Color4::BLACK);
    popup->setVisible(true);
    popup->setPosition(pos);

    addChild(popup);

    _popups.push_back(std::make_tuple(popup, now));
}

/**Potentially saves and/or modifies the following information:
chapter - int
level - int
night: {
    location_x_player - float
	location_y_player - float
	health_player - float
    meter_player - float
	for each enemy type:
        location_x_type - float array
		location_y_type - float array 
		health_enemy_type - float array
}

Retains the following from previous save file: 
day { ... }
persistent { ... }
*/

void GameScene::save() {
    /*std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({ root,"save.json" });*/

    //Should only change nighttime save data unless level was completed, in which case change level/chapter accordingly.
    std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({ root,"save.json" });

    auto reader = JsonReader::alloc(path);

    std::shared_ptr<JsonValue> prev_json = reader->readJson();
    reader->close();

    //write basic info.
    //placeholders

    std::shared_ptr<JsonValue> json = JsonValue::allocObject();

    json->appendValue("chapter", 1.0f);
    json->appendValue("level", 1.0f);
    
    std::shared_ptr<JsonValue> night = JsonValue::allocObject();
    
    night->appendValue("location_x_player", _avatar->getPosition().x);
    night->appendValue("location_y_player", _avatar->getPosition().y);
    night->appendValue("health_player", _avatar->getHealth());

    std::vector<std::string> types = { "egg", "carrot", "shrimp", "rice", "beef" };
    for (auto t = types.begin(); t != types.end(); t++) {
        std::string type = *t;
        night->appendArray("location_x_" + type);
        night->appendArray("location_y_" + type);
        night->appendArray("health_" + type);
    }
    
    for (auto& e : _enemies) {
        if (e->isRemoved()) {
			continue;
		}
        std::string type = EnemyModel::typeToStr(e->getType());
		night->insertValue(0, "location_x_" + type, e->getPosition().x);
        night->insertValue(0, "location_y_" + type, e->getPosition().y);
        night->insertValue(0, "health_" + type, e->getHealth());
	}

    json->appendChild("night", night);

    std::shared_ptr<JsonValue> day = prev_json->get("day");
    std::shared_ptr<JsonValue> persistent = prev_json->get("persistent");
    if (persistent == nullptr || persistent->isNull()) {
        persistent = JsonValue::allocObject();
    }
    else {
        persistent->_parent = nullptr;
    }
    if (day == nullptr || day->isNull()) {
        day = JsonValue::allocObject();
    }
    else {
        day->_parent = nullptr;
    }

    json->appendChild("day", day);
    json->appendChild("persistent", persistent);

    json->appendValue("test", 0.0f);

    auto writer = JsonWriter::alloc(path);

    writer->writeJson(json);
    
    writer->close();
}

void GameScene::loadSave() {
	/*std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({ root,"save.json" });*/


    //CULog("PATH");
    //CULog(path.c_str());

    std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({ root,"save.json" });
    auto reader = JsonReader::alloc(path);

    std::shared_ptr<JsonValue> loaded_json = reader->readJson();

    //Todo:: load enemies separately from level.

    int chapter = loaded_json->getInt("chapter");
    int level = loaded_json->getInt("level");

    loadLevel(chapter, level);

    reader->close();
    
}

//load level with int specifiers
void GameScene::loadLevel(int chapter, int level) {
    std::shared_ptr<Levels> level_obj = nullptr;
    if (chapter == 1) {
        if (level == 1) {
            level_obj = level1;
        }
        else if (level == 2) {
            level_obj = level2;
        }
    }

    loadLevel(level_obj);

}

