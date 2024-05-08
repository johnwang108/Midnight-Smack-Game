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
#define SCENE_WIDTH 12800
#define SCENE_HEIGHT 960

/** This is the aspect ratio for physics */
#define SCENE_ASPECT 10.0/133.0
// #define SCENE_ASPECT 10.0/84.0

/** Width of the game world in Box2d units */
// #define DEFAULT_WIDTH   210.0f
#define DEFAULT_WIDTH 400.0f
/** Height of the game world in Box2d units */
// #define DEFAULT_HEIGHT  25.0f
#define DEFAULT_HEIGHT 30.0f

#define MINIMAP_ZOOM 0.1f

#define MINIMAP_WIDTH 400

#define MINIMAP_HEIGHT 400

#define CAMERA_FOLLOWS_PLAYER true

#define COOKTIME_MAX_DIST 3.5f

#define FEEDBACK_DURATION 1.2f

#define HEALTHBAR_X_OFFSET 15
#define DISCARD_HOLD_TIME 2.0f
#define MIN_DISCARD_START_TIME 0.25f
/**desired order width in pixels*/
#define ORDER_WIDTH 100.0f

struct IngredientProperties {
    std::string name;
    std::vector<std::string> gestures;
};


std::map<EnemyType, IngredientProperties> enemyToIngredientMap = {
    {EnemyType::beef, {"beef", EnemyModel::defaultSeq(EnemyType::beef)}},
    {EnemyType::carrot, {"carrot", EnemyModel::defaultSeq(EnemyType::carrot)}},
    {EnemyType::egg, {"egg", EnemyModel::defaultSeq(EnemyType::egg)}},
    {EnemyType::rice, {"rice", EnemyModel::defaultSeq(EnemyType::rice)}},
    {EnemyType::rice_soldier, {"rice", EnemyModel::defaultSeq(EnemyType::rice_soldier)}},
    {EnemyType::shrimp, {"shrimp", EnemyModel::defaultSeq(EnemyType::shrimp)}}
};

//TODO FIX
std::map<IngredientType, IngredientType> cookedTypeMap = {
	{IngredientType::beef, IngredientType::cookedBeef},
	{IngredientType::carrot, IngredientType::cutCarrot},
	{IngredientType::egg, IngredientType::boiledEgg},
	{IngredientType::rice, IngredientType::boiledRice},
	{IngredientType::shrimp, IngredientType::cookedShrimp}
};

std::map<IngredientType, IngredientProperties> typeToPropertiesMap = {
    {IngredientType::beef, {"beef", EnemyModel::defaultSeq(EnemyType::beef)}},
    {IngredientType::carrot, {"carrot", EnemyModel::defaultSeq(EnemyType::carrot)}},
    {IngredientType::egg,  {"egg", EnemyModel::defaultSeq(EnemyType::egg)}},
    {IngredientType::rice, {"rice", EnemyModel::defaultSeq(EnemyType::rice)}},
    {IngredientType::shrimp, {"shrimp", EnemyModel::defaultSeq(EnemyType::shrimp)}},
    {IngredientType::boiledRice, {"boiledRice", EnemyModel::defaultSeq(EnemyType::rice)}},
    {IngredientType::cutCarrot, {"cutCarrot", EnemyModel::defaultSeq(EnemyType::carrot)}},
    {IngredientType::cookedBeef, {"cookedBeef", EnemyModel::defaultSeq(EnemyType::beef)}},
    {IngredientType::boiledEgg, {"boiledEgg", EnemyModel::defaultSeq(EnemyType::egg)}},
    {IngredientType::cookedShrimp, {"cookedShrimp", EnemyModel::defaultSeq(EnemyType::shrimp)}},
    {IngredientType::scrambledEgg, {"scrambledEgg", EnemyModel::defaultSeq(EnemyType::egg)}},
};


std::map<std::string, buff> ingredientToBuff = {
    {"beef", buff::speed},
    {"carrot", buff::jump},
    {"egg", buff::health},
    {"rice", buff::defense},
    {"carrot", buff::jump},
};

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
    _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/empanada-platform-level-01.json");
    // _level_model->setFilePath("json/bull-boss-level.json");
    setSceneWidth(_level_model->loadLevelWidth());
    setSceneHeight(_level_model->loadLevelHeight());
    return init(assets, Rect(0, 0, getSceneWidth(), getSceneHeight()), Vec2(0, DEFAULT_GRAVITY), input);
}

bool GameScene::initWithSave(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<JsonValue> save) {
    /*setSceneWidth(400);
    setSceneHeight(30);*/
    _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/empanada-platform-level-01.json");
    // _level_model->setFilePath("json/bull-boss-level.json");
    setSceneWidth(_level_model->loadLevelWidth());
    setSceneHeight(_level_model->loadLevelHeight());
    bool res = init(assets, Rect(0, 0, getSceneWidth(), getSceneHeight()), Vec2(0, DEFAULT_GRAVITY), input);
    if (save->size() == 0) return res;
    float locationX = save->get("player")->getFloat("location_x");
    float locationY = save->get("player")->getFloat("location_y");
    // bool res = init(assets, Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY), input);
    loadSave(save);
    return res;
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
    /*_scene_width = 400;
    _scene_height = 30;
    setSceneWidth(400);
    setSceneHeight(30);*/
    _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/empanada-platform-level-01.json");
    // _level_model->setFilePath("json/bull-boss-level.json");
    setSceneWidth(_level_model->loadLevelWidth());
    setSceneHeight(_level_model->loadLevelHeight());

    Size dimen = computeActiveSize();
    // SDL_ShowCursor(SDL_DISABLE);

    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
    _input = input;
    _input->init(getBounds());

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

     _scale = dimen.width == (getSceneWidth() * 32.0f) ? dimen.width / rect.size.width : dimen.height / rect.size.height;
     Vec2 offset((dimen.width - (getSceneWidth() * 32.0f)) / 2.0f, (dimen.height - (getSceneHeight() * 32.0f)) / 2.0f);
    /*_scale = dimen.width == (SCENE_WIDTH) ? dimen.width / rect.size.width : dimen.height / rect.size.height;
    Vec2 offset((dimen.width - SCENE_WIDTH) / 2.0f, (dimen.height - SCENE_HEIGHT) / 2.0f);*/

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
    _winnode->setPosition(1280/2,800/2);
    _winnode->setForeground(WIN_COLOR);
    setComplete(false);

    _losenode = scene2::Label::allocWithText(LOSE_MESSAGE, _assets->get<Font>(MESSAGE_FONT));
    _losenode->setAnchor(Vec2::ANCHOR_CENTER);
    _losenode->setPosition(1280 / 2, 800 / 2);
    _losenode->setForeground(LOSE_COLOR);
    setFailure(false);




    _slowed = false;
    _attacks = std::vector<std::shared_ptr<Attack>>();


    _dollarnode = std::make_shared<DollarScene>();
    _dollarnode->init(_assets, _input, "cooktime");
    _dollarnode->SceneNode::setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _dollarnode->setVisible(false);
    _dollarnode->setNighttime(true);
    _dollarnode->setPosition(0,0);

    _inventoryNode = std::make_shared<Inventory>();
    std::shared_ptr<Texture> invTex = _assets->get<Texture>("inventorySlot");
    _inventoryNode->init(_assets, _input, Size(invTex->getWidth()*NUM_SLOTS, 180.0f));
    _inventoryNode->setName("inventoryNode");
    _inventoryNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    _inventoryNode->setPosition(Vec2(1280.0f / 2.0f, 0));

#pragma mark: UI

    // ui stuff
    _uiScene = cugl::Scene2::alloc(Size(1280, 800));
    _uiScene->init(Size(1280, 800));
    _uiScene->setActive(true);
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

    std::shared_ptr<cugl::scene2::SceneNode> _SFRBarNode;
    _SFRBarNode = _assets->get<scene2::SceneNode>("SFR");
    _SFRhealthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("shrimpbar"));
    _SFRhealthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("bosshealth"));
    _uiScene->addChild(_SFRBarNode);
   
    _paused = false;
    
    //_uiScene->addChild(_pauseButton);
    _uiScene->addChild(_meterUINode);
    _uiScene->addChild(_dollarnode);

    _uiScene->addChild(_winnode);
    _uiScene->addChild(_losenode);
    _uiScene->addChild(_inventoryNode);


# pragma mark: Background

    //_bgScene = cugl::Scene2::alloc(dimen);
    //_bgScene->init(dimen);
    //_bgScene->setActive(true);
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
    //_bgScene->setColor(Color4::CLEAR);

    _interactables = std::vector<std::shared_ptr<GestureInteractable>>();
    _plates = std::vector<std::shared_ptr<Plate>>();
    _stations = std::vector<std::shared_ptr<Station>>();

    //minimap setup
    _r = RenderTarget::alloc(MINIMAP_WIDTH, MINIMAP_HEIGHT);

    _minimapCamera = OrthographicCamera::alloc(MINIMAP_WIDTH, MINIMAP_HEIGHT);
    _minimapCamera->setZoom(MINIMAP_ZOOM);
    _minimapNode = cugl::scene2::PolygonNode::alloc();
    _minimapNode->setContentSize(MINIMAP_WIDTH, MINIMAP_HEIGHT);
    _minimapNode->setAnchor(Vec2::ANCHOR_CENTER);
    _minimapNode->setPosition(1280/ 2, 800 / 2);
    _uiScene->addChild(_minimapNode);

   _chapter = 1;
   _level = 1;
   loadLevel(_chapter, _level);
   // 
   // _level_model->setFilePath("json/empanada-platform-level-01.json");
   // currentLevel = _level_model;
    addChild(_worldnode);
    addChild(_debugnode);
    _numOrders = 0;

    _orders = std::unordered_map<int, std::vector<std::shared_ptr<scene2::SceneNode>>>();
    _orderNode = scene2::SceneNode::alloc();
    generateOrders();
    _orderNode->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    _orderNode->setPosition(1280 / 2, 800 - 50);
    _orderNode->setVisible(true);
    _uiScene->addChild(_orderNode);

    _actionManager = cugl::scene2::ActionManager::alloc();
    _BullactionManager = cugl::scene2::ActionManager::alloc();
    _SHRactionManager = cugl::scene2::ActionManager::alloc();

    _afterimages = std::vector < std::shared_ptr < scene2::SpriteNode>>();

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

    _currentInteractableID = -1;

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
        _assets = nullptr;
        _input = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _losenode = nullptr;
        //_bgScene = nullptr;
        _uiScene = nullptr;
        _dollarnode = nullptr;
        _world = nullptr;
        _goalDoor = nullptr;
        _background = nullptr;
        _avatar = nullptr;
        _enemies.clear();
        _afterimages.clear();
        _vulnerables.clear();
        for (auto& attack : _attacks) {
            attack = nullptr;
        }
        _attacks.clear();
        currentLevel = nullptr;
        _Bull = nullptr;
        _ShrimpRice = nullptr;
        level3 = nullptr;
        _healthBarForeground = nullptr;
        _healthBarBackground = nullptr;
        _BullhealthBarBackground = nullptr;
        _BullhealthBarForeground = nullptr;
        _SFRhealthBarBackground = nullptr;
        _SFRhealthBarForeground = nullptr;
        _cookBarFill = nullptr;
        _cookBarOutline = nullptr;
        _cookBarIcons.clear();
        _cookBarGlows.clear();
        //_buffLabel = nullptr;
        _popups.clear();
        _actionManager = nullptr;
        _BullactionManager = nullptr;
        _SHRactionManager = nullptr;
        _pauseButton = nullptr;
        _debugAnimTarget = nullptr;
        _level_model = nullptr;
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
    _paused = false;
    _worldnode->removeAllChildren();
    _world->clear();
    _debugnode->removeAllChildren();
    _avatar = nullptr;
    _goalDoor = nullptr;
    _background = nullptr;
    _sensorFixtures.clear();
    _inventoryNode->reset();
    for (auto& enemy : _enemies) {
        enemy = nullptr;
    }
    _enemies.clear();
    for (auto& attack : _attacks) {
        attack = nullptr;
    }
    _attacks.clear();
    for (auto& v : _vulnerables) {
        v = nullptr;
    }
    _vulnerables.clear();
    for (auto& a : _afterimages) {
        a = nullptr;
    }
    _afterimages.clear();
    for (auto& p : _popups) {
		std::get<0>(p) = nullptr;
	}
    _popups.clear();

    for (auto& s : _stations) {
        s = nullptr;
    }
    _stations.clear();

    for (auto& p : _plates) {
        p = nullptr;
    }
    _plates.clear();

    for (auto& i : _interactables) {
        i = nullptr;
    }
    _interactables.clear();

    for (auto& i : _orders) {
        for (auto& j : i.second) {
			j = nullptr;
		}
    }
    _orders.clear();
    _Bull = nullptr;

    removeChild(_worldnode);
    removeChild(_debugnode);
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
    loadLevel(_chapter, _level);
    addChild(_worldnode);
    addChild(_debugnode);

    generateOrders();
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
        //transition(true);
        //setTarget("main_menu");
    }


    //this is now minimap
    if (_input->didMinimap()) {
        _minimapNode->setVisible(!_minimapNode->isVisible());
    }

    if (_input->didPause()) {
        _paused = !_paused;
    }

    if (_input->getInventoryLeftPressed()) {
        _inventoryNode->selectPreviousSlot();
    }
    else if (_input->getInventoryRightPressed()) {
        _inventoryNode->selectNextSlot();
    }


    if (_input->didInteract()) {
        CULog("IIIII");
        if (_currentInteractableID != -1) {

            CULog("%i", _currentInteractableID);
            for (auto& i : _plates) {
                if (i->getId() == _currentInteractableID) {
                    IngredientType t = _inventoryNode->getIngredientTypeFromSlot(_inventoryNode->getSelectedSlot());
                    bool b = i->interact(t);
                    if (b) {
                        removeOrder(i->getId(), t, true);
                        _inventoryNode->popIngredientFromSlot(_inventoryNode->getSelectedSlot());
                    }
                    break;
                }
                CULog("plate");
            }

            for (auto& i : _stations) {
                if (i->getId() == _currentInteractableID) {
                    IngredientType t = _inventoryNode->getIngredientTypeFromSlot(_inventoryNode->getSelectedSlot());
                    if (i->interact(t)) {
                        i->setIngredientPtr(_inventoryNode->popIngredientFromSlot(_inventoryNode->getSelectedSlot()));
                    }
                    else if (i->isFull()) {
                        ////let's cook baby
                        //std::shared_ptr<Ingredient> ing = i->getIngredientPtr();
                        //_dollarnode->addIngredient(ing);
                        //_slowed = true;
                        //_dollarnode->setTargetGesturesNighttime({ ing->getGestures(), ing->getGestures() });
                        //_dollarnode->setIngredientInStation(ing);
                        //_dollarnode->setIsStation(true);
                        //i->setIngredientPtr(nullptr);
                        //

                        //add cooked version of ingredient to inventory
                        IngredientType t = Ingredient::getIngredientTypeFromString(i->getIngredientPtr()->getName());
                        IngredientProperties props = typeToPropertiesMap[i->getCookedType(t)];

                        std::shared_ptr<Ingredient> ing = std::make_shared<Ingredient>("", props.gestures, 0.0f);
                        ing->setName(props.name);
                        std::shared_ptr<Texture> tex = _assets->get<Texture>(ing->getName());
                        ing->init(tex);

                        //hack
                        int id = _plates.back()->getId();

                        IngredientType cookedType = i->getCookedType(t);
                        removeOrder(id, cookedType, false);
                        createOrder(id, cookedType, true);

                        _inventoryNode->addIngredient(ing);

                        i->clearIngredients();
                        i->setIngredientPtr(nullptr);

                    }
                }
                else CULog("Station id: %i", i->getId());
                CULog("station");
            }
        }

        else CULog("failure");
    }

    checkForCooktime();
    

    //handle animations
    if (!_actionManager->isActive("air_attack")) {
        _avatar->getBody()->SetFixedRotation(false);
        _avatar->getBody()->SetTransform(_avatar->getBody()->GetPosition(), 0.0f);
        _avatar->getBody()->SetFixedRotation(true);
    }
    
    //start running if idle or recovering and moving
    if (!_paused) {
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

        if (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() < _avatar->getFloatyFrames() && !_actionManager->isActive("air_roll")) {
            _avatar->animate("air_roll");
			auto dashAction = _avatar->getAction("air_roll");
			_actionManager->clearAllActions(_avatar->getSceneNode());
			_actionManager->activate("air_roll", dashAction, _avatar->getSceneNode());
        }


        //animate jumps if not attacking or taking damage
        if (!_avatar->isGrounded() && !_actionManager->isActive("attack") && !_actionManager->isActive("air_attack") && !(_avatar->getActiveAction() == "air_attack")&&  !_actionManager->isActive("air_roll") && _avatar->getLinearVelocity().y > 0 && (_avatar->getLastDamageTime() > _avatar->getHealthCooldown())) {
            //CULog("animating jump_up 1");
            _avatar->animate("jump_up");
            auto jumpAction = _avatar->getAction("jump_up");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_up", jumpAction, _avatar->getSceneNode());
        }
        if (!_avatar->isGrounded() && !_actionManager->isActive("attack") && !_actionManager->isActive("air_attack") && !(_avatar->getActiveAction() == "air_attack") && !_actionManager->isActive("air_roll") && _avatar->getLinearVelocity().y < 0 && (_avatar->getLastDamageTime() > _avatar->getHealthCooldown())) {
           // CULog("animating jump_down");
            _avatar->animate("jump_down");
            auto jumpAction = _avatar->getAction("jump_down");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_down", jumpAction, _avatar->getSceneNode());
        }
        if (_avatar->isGrounded() && (_actionManager->isActive("jump_down") || _actionManager->isActive("jump_up"))) {
            //CULog("animating jump_land");
            _avatar->animate("jump_land");
            auto jumpAction = _avatar->getAction("jump_land");
            _actionManager->clearAllActions(_avatar->getSceneNode());
            _actionManager->activate("jump_land", jumpAction, _avatar->getSceneNode());
        }


        //handle expired actions
        if (!_actionManager->isActive(_avatar->getActiveAction())) {
            if (_avatar->getActiveAction() == "attack") {
                //CULog("animating attack");
                _avatar->animate("recover");
                auto recoverAction = _avatar->getAction("recover");
                _actionManager->activate("recover", recoverAction, _avatar->getSceneNode());
            }
            else if (_avatar->getActiveAction() == "run" && _input->getHorizontal() != 0) {
                //CULog("animating run");
                _avatar->animate("run");
                auto runAction = _avatar->getAction("run");
                _actionManager->clearAllActions(_avatar->getSceneNode());
                _actionManager->activate("run", runAction, _avatar->getSceneNode());
            }
            else  if (_avatar->getActiveAction() == "air_attack") {
                //CULog("animating air roll");
                _avatar->animate("air_roll");
                auto recoverAction = _avatar->getAction("air_roll");
                _actionManager->activate("air_roll", recoverAction, _avatar->getSceneNode());
            } 
            else {
                //Todo:: blink idle
                //CULog("animating idle");
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
                afterimage->flipHorizontal(_avatar->getSpriteNode()->isFlipHorizontal());
                Color4 b = Color4::CYAN;
                b.a = 255;
                afterimage->setColor(b);
                afterimage->setScale(_avatar->getSpriteNode()->getScale());
                _afterimages.push_back(afterimage);
                addChild(afterimage);
			}

        _avatar->setShooting(_input->didFire());
        if (_avatar->isShooting() && (!_actionManager->isActive("attack") && !_actionManager->isActive("air_attack"))) {
            if (_avatar->isGrounded()) {
                auto att = _avatar->createAttack(getAssets(), _scale);
                addObstacle(std::get<0>(att), std::get<1>(att), true);
                _attacks.push_back(std::get<0>(att));
                std::get<0>(att)->setListener([=](physics2::Obstacle* obs) {
                    obs->setPosition(_avatar->getPosition());
                    obs->setAngle(_avatar->getBody()->GetAngle());
					});

                auto attackAction = _avatar->getAction("attack");
                _avatar->animate("attack");
                _actionManager->clearAllActions(_avatar->getSceneNode());
                _actionManager->activate("attack", attackAction, _avatar->getSceneNode());
            }
            else {
                float horiz = _input->getHorizontal();
                float vert = _input->getVertical();
                cugl::Vec2 dir = Vec2(horiz, vert).normalize();
                float angle = dir.getAngle();
                if (horiz == 0 && vert == 0) angle = 3.14159265 / 2;

                auto att = _avatar->createAirAttack(getAssets(), _scale, angle);
                addObstacle(std::get<0>(att), std::get<1>(att), true);
                _attacks.push_back(std::get<0>(att));
                std::get<0>(att)->setListener([=](physics2::Obstacle* obs) {
                    float avAngle = _avatar->getBody()->GetAngle() + (3.14159265 / 2);
                    Vec2 angleVec = Vec2(cos(avAngle), sin(avAngle));
                    obs->setAngle(_avatar->getBody()->GetAngle());
                    obs->setPosition(_avatar->getPosition() + angleVec * 1.5);
                });
               
                _avatar->getBody()->SetFixedRotation(false);
                _avatar->getBody()->SetTransform(_avatar->getBody()->GetPosition(), angle- (3.14159265 / 2));
                _avatar->getBody()->SetFixedRotation(true);

                auto attackAction = _avatar->getAction("air_attack");
                _avatar->animate("air_attack");
                _actionManager->clearAllActions(_avatar->getSceneNode());
                _actionManager->activate("air_attack", attackAction, _avatar->getSceneNode());
            }
        }
    }

    if (_input->didLevel1()) {
        setLevel(1, 1);
    }
    else if (_input->didLevel2()) {
        setLevel(1, 2);
    }
    else if (_input->didLevel3()) {
        setLevel(1, 3);
    }


    //advance level for debug
    if (_input->didAnimate()) {
        setComplete(true);
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
    //if (_input->didMusic()) {

    //    auto reader = JsonReader::alloc("./json/constants.json");

    //    std::shared_ptr<JsonValue> js = reader->readJson();
    //    
    //    std::shared_ptr<Sound> source = _assets->get<Sound>(js->get("environment")->get("1")->getString("music"));
    //    AudioEngine::get()->getMusicQueue()->clear();
    //    AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
    //}

    _dollarnode->update(dt);
    if (_slowed){ 
        //transition in dollar node
        _dollarnode->setVisible(true);
        if (!(_dollarnode->isFocus())) {
            _dollarnode->setFocus(true);
            _dollarnode->setReadyToCook(true);
        }
        //cooktime handling. Assume that _target not null, if it is null then continue
        //if (!_dollarnode->isPending()) {
        if (!_dollarnode->isPending()) {
            _slowed = false;
            std::string message = "";
            if (_dollarnode->getLastResult() > 0) {
                CULog("NICE!!!!!!!!!!!!!!");

                if (_dollarnode->isStation()) {
                    
                    ////add cooked version of ingredient to inventory
                    //IngredientType t = Ingredient::getIngredientTypeFromString(_dollarnode->getIngredientInStation()->getName());
                    //IngredientProperties props = typeToPropertiesMap[cookedTypeMap[t]];

                    //std::shared_ptr<Ingredient> ing = std::make_shared<Ingredient>("", props.gestures, 0.0f);
                    //ing->setName(props.name);
                    //std::shared_ptr<Texture> tex = _assets->get<Texture>(ing->getName());
                    //ing->init(tex);

                    ////hack
                    //int id = _plates.back()->getId();

                    //removeOrder(id, t, false);
                    //createOrder(id, t, true);

                    //_inventoryNode->addIngredient(ing);
                }
                else {
                    modifier mod = _dollarnode->getIsDurationSequence() ? modifier::duration : modifier::effect;
                    //_target->takeDamage(_avatar->getAttack(), 0);

                    buff reward = ingredientToBuff[_dollarnode->getIngredientInStation()->getName()];
                    _avatar->applyBuff(reward, mod);
                }
            }
            else {
                CULog("BOOOOOOOOOOOOOOO!!!!!!!!!!");
            }
            CULog("%i", _dollarnode->getLastResult());
            message = _feedbackMessages[_dollarnode->getLastResult()];
            popup(message, cugl::Vec2(_avatar->getPosition().x * _scale, _avatar->getPosition().y * 1.1 * _scale));
                
            _dollarnode->setIngredientInStation(nullptr);
            _dollarnode->setPending(true);
            //}
            //else {

            //}
        }
    }

    if (_avatar->getDuration() == 0 && !_avatar->hasSuper()) {
        //_buffLabel->setVisible(false);
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
    std::vector<std::shared_ptr<Rice>> spawns = std::vector<std::shared_ptr<Rice>>();
    for (auto& enemy : _enemies) {
        if (enemy != nullptr && enemy->getBody() != nullptr && !enemy->isRemoved()) {
            enemy->update(dt);
            Vec2 enemyPos = enemy->getPosition();
            float distance = avatarPos.distance(enemyPos);

            enemy->updatePlayerDistance(_avatar->getPosition());
			if (enemy->getattacktime()) {
                //spawn rice if rice
                if (enemy->getType() == EnemyType::rice) {
                    std::shared_ptr<Texture> image = _assets->get<Texture>("riceSoldier");
                    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);
                    float imageWidth = image->getWidth() / 4;
                    float imageHeight = image->getHeight() / 4;
                    Size singularSpriteSize = Size(imageWidth, imageHeight);
                    std::shared_ptr<Rice> riceSpawn = Rice::allocWithConstants(enemyPos + Vec2((rand() % 2) * enemy->getDirection(), rand() % 2), singularSpriteSize / (5 * getScale()), getScale(), _assets, true);

                    riceSpawn->setState("acknowledging");
                    riceSpawn->setListener([=](physics2::Obstacle* obs) {
                        riceSpawn->setTargetPosition(_avatar->getPosition());
                        riceSpawn->setPosition(obs->getPosition()* _scale);
                        riceSpawn->setAngle(obs->getAngle());
                        });
                    riceSpawn->setSceneNode(spritenode);
                    spritenode->setScale(0.375 / 1.75);
                    addObstacle(riceSpawn, spritenode);
                    spawns.push_back(riceSpawn);
                }
                else {
                    auto res = enemy->createAttack(_assets, _scale);
                    addObstacle(std::get<0>(res), std::get<1>(res));
                    _attacks.push_back(std::get<0>(res));
                }
                enemy->setattacktime(false);
                enemy->setshooted(false);
			}

            std::string actionKey = enemy->getActiveAction() + enemy->getId();

            if ((enemy->getActiveAction() != "" && !_actionManager->isActive(actionKey)) || enemy->getPriority() > enemy->getActivePriority())
            {
                _actionManager->clearAllActions(enemy->getSceneNode());
                std::string actionName = enemy->getRequestedAction();
                enemy->animate(actionName);
                auto action = enemy->getAction(actionName);
                _actionManager->activate(actionName + enemy->getId(), action, enemy->getSceneNode());

                //CULog("animating %s", actionName.c_str());
            }
        }
    }

    for (auto& spawn : spawns) {
        _enemies.push_back(spawn);
    }
    spawns.clear();

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
                if (_Bull->getnextchangetime() < 0 && _Bull->getacttime()<=0){
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _Bull->setDirection(direction);
                    _Bull->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            if (_Bull->getCAcount()<=0 && _Bull->isChasing() && ((_Bull->getPosition().x > 20 && _Bull->getDirection() == -1) || (_Bull->getPosition().x < 30 && _Bull->getDirection() == 1))) {
                Vec2 BullPos = _Bull->getPosition();
                int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                if (direction != _Bull->getDirection()) {
                    _Bull->setbreaking(2.0f);
                }
			}
            _Bull->update(dt);
        }
    if (_ShrimpRice != nullptr && !_ShrimpRice->isRemoved()) {
            Vec2 BullPos = _ShrimpRice->getPosition();
            if (_ShrimpRice->getHealth() <= 0) {
                _worldnode->removeChild(_ShrimpRice->getSceneNode());
                _ShrimpRice->setDebugScene(nullptr);
                _ShrimpRice->markRemoved(true);
            }
            if (_ShrimpRice->getact()=="SFRWave2") {
                float distance = avatarPos.distance(BullPos);
                if (distance < 4) {
                    _ShrimpRice->setact("SFRWave3",1.125);
                }
            }
            if (_ShrimpRice->gettimetosummon()) {
                _ShrimpRice->Summon(*this);
                _ShrimpRice->settimetosummon(false);
            }
            if (_ShrimpRice->getcanturn() && _ShrimpRice->getacttime()<=0) {
                if (_ShrimpRice->getnextchangetime() < 0) {
                    int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                    _ShrimpRice->setDirection(direction);
                    _ShrimpRice->setnextchangetime(0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
                }
            }
            if (_ShrimpRice->getangrytime() > 0 && !_enemies.empty()) {
                for (auto& enemy : _enemies) {
                    enemy->setnocoll(true);
                    Vec2 EnyPos = enemy->getPosition();
                    int direction = (BullPos.x > EnyPos.x) ? 1 : -1;
                    enemy->setPosition(enemy->getPosition() + Vec2(direction*0.3, 0.0f));
				}
            }
            else if(_ShrimpRice->getHealth()<=35&&_enemies.size()>=4){
                _ShrimpRice->setact("SFRStunState2", 5.0f);
                _ShrimpRice->setangrytime(5);
            }
            if(_ShrimpRice->getparry()&&!_ShrimpRice->getparry2()){
                _ShrimpRice->setparry(false);
                _ShrimpRice->setparry2(false);
                _ShrimpRice->setact("SFRStunState1", 0.6);
                _ShrimpRice->parry(*this);
                _ShrimpRice->setdelay(0.5);
            }
        
            _ShrimpRice->update(dt);
        }





    if (_Bull != nullptr) {
        if (!_paused) {
            _BullactionManager->update(dt);
        }
        if (_Bull->getangrytime()>0) {
            if (!_BullactionManager->isActive("bullStunned")) {
                _BullactionManager->clearAllActions(_Bull->getSceneNode());
                auto bullStunned = _Bull->getAction("bullStunned");
                _BullactionManager->activate("bullStunned", bullStunned, _Bull->getSceneNode());
            }
            if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                _Bull->animate("bullStunned");
            }
        }
        else if (_Bull->getCAcount()<=0 && _Bull->getbreaking()<=0 &&_Bull->isChasing() && ((_Bull->getPosition().x < 13 && _Bull->getDirection() == -1) || (_Bull->getPosition().x > 37 && _Bull->getDirection() == 1))) {
            if (!_BullactionManager->isActive("bullAttack")){
                _BullactionManager->clearAllActions(_Bull->getSceneNode());
                auto bullAttack = _Bull->getAction("bullAttack");
                _BullactionManager->activate("bullAttack", bullAttack, _Bull->getSceneNode());
            }
            if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                _Bull->animate("bullAttack");
            }
        }
        else if (_Bull->getacttime() > 0) {
            if (!_BullactionManager->isActive(_Bull->getact())) {
                _BullactionManager->clearAllActions(_Bull->getSceneNode());
                auto bullTurn = _Bull->getAction(_Bull->getact());
                _BullactionManager->activate(_Bull->getact(), bullTurn, _Bull->getSceneNode());
            }
            if (!_BullactionManager->isActive(_Bull->getActiveAction())) {
                _Bull->animate(_Bull->getact());
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
        else if ( _Bull->getsprintpreparetime() > 0 && _Bull->getknockbacktime() <= 0 &&_Bull->getattacktype()!="none") {
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
        if (!_paused) {
            _SHRactionManager->update(dt);
        }
        
        if (_ShrimpRice->getacttime() > 0) {
            if (!_SHRactionManager->isActive(_ShrimpRice->getact())) {
                _SHRactionManager->clearAllActions(_ShrimpRice->getSceneNode());
                auto bullTurn = _ShrimpRice->getAction(_ShrimpRice->getact());
                _SHRactionManager->activate(_ShrimpRice->getact(), bullTurn, _ShrimpRice->getSceneNode());
            }
            if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                _ShrimpRice->animate(_ShrimpRice->getact());
            }
        }
        else if (_ShrimpRice->getknockbacktime() <= 0 && _ShrimpRice->getmovestate1()>0) {
            if (!_SHRactionManager->isActive("SFRMoveState1")) {
                _SHRactionManager->clearAllActions(_ShrimpRice->getSceneNode());
                auto SFR_Move = _ShrimpRice->getAction("SFRMoveState1");
                _SHRactionManager->activate("SFRMoveState1", SFR_Move, _ShrimpRice->getSceneNode());
            }
            if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                _ShrimpRice->animate("SFRMoveState1");
            }
		}
        else if (_ShrimpRice->getknockbacktime() <= 0) {
            if (!_SHRactionManager->isActive("SFRMoveState2")) {
                _SHRactionManager->clearAllActions(_ShrimpRice->getSceneNode());
                auto SFR_Move = _ShrimpRice->getAction("SFRMoveState2");
                _SHRactionManager->activate("SFRMoveState2", SFR_Move, _ShrimpRice->getSceneNode());
            }
            if (!_SHRactionManager->isActive(_ShrimpRice->getActiveAction())) {
                _ShrimpRice->animate("SFRMoveState2");
            }
        }
        
    }

    if ((_afterimages.size() > 4 || (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() > _avatar->getFloatyFrames())) && !_afterimages.empty()) {
        std::shared_ptr<scene2::SceneNode> afterimage = _afterimages.front();
        _afterimages.erase(_afterimages.begin());
        removeChild(afterimage);
    }
     
    if (!_paused) {
        _actionManager->update(dt);
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
    if (_paused) {
        //step = 0;
        return;
    }

    if (_slowed) {
        step = step / 15;
    }

    if (CAMERA_FOLLOWS_PLAYER) {

        if (_level_model->getFilePath() == "json/intermediate.json") {
            _camera->setZoom(155.0 / 40.0);
            // _camera->setZoom(1.0);
        }

        else if (_level_model->getFilePath() != "") {
            _camera->setZoom(155.0 / 40.0);
        }
        else {
            _camera->setZoom(400.0 / 40.0);
        }

        cugl::Vec3 target = _avatar->getPosition() * _scale + _cameraOffset;
        float invZoom = 1 / _camera->getZoom();
        float cameraWidth = invZoom * (_camera->getViewport().getMaxX() - _camera->getViewport().getMinX()) / 2;
        float cameraHeight = invZoom * (_camera->getViewport().getMaxY() - _camera->getViewport().getMinY()) / 2;

        if (_level == 1 || _level == 2 || _level == 3 || _level == 4) {
            cugl::Vec3 mapMin = Vec3(_background->getBoundingRect().getMinX() + cameraWidth, _background->getBoundingRect().getMinY() + cameraHeight, 0);
            cugl::Vec3 mapMax = Vec3(_background->getBoundingRect().getMaxX() - cameraWidth, _background->getBoundingRect().getMaxY() - cameraHeight, 0);
            target.clamp(mapMin, mapMax);
        }
       

        cugl::Vec3 pos = _camera->getPosition();

        Rect viewport = _camera->getViewport();

        Vec2 worldPosition = Vec2(pos.x - viewport.size.width / 2 + 140,
            pos.y + viewport.size.height / 2 - 50);


        //magic number 0.2 are for smoothness
        float smooth = 0.2;
        pos.smooth(target, step, smooth);
        _camera->setPosition(pos);
        _minimapCamera->setPosition(pos);
        _camera->update();
        _minimapCamera->update();
    }
    if (_avatar->getHealth() <= 0) {
        setFailure(true);
    }
    if (_Bull != nullptr && _Bull->getHealth() <= 0) {
        setComplete(true);
    }

    if (!_slowed) {
        _dollarnode->setVisible(false);
        if (_dollarnode->isFocus()) {
            _dollarnode->setFocus(false);
            _dollarnode->setReadyToCook(false);
        }

        //_avatar->setMovement(_input->getHorizontal() * _avatar->getForce());
        _avatar->setAllMovement(_input->getHorizontal(), _input->getVertical());
        _avatar->setJumping(_input->didJump());
        _avatar->setDash(_input->didDash());
        _avatar->setInputWalk(_input->getHorizontal() != 0);
        _avatar->applyForce(_input->getHorizontal(), _input->getVertical());
        if (_avatar->isJumping() && _avatar->isGrounded()) {
            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }
    }
    else {
        _avatar->setAllMovement(0, 0);
        _avatar->setJumping(false);
        _avatar->setDash(false);
    }

    //su
    _avatar->fixedUpdate(step);
    for (auto& enemy : _enemies) {
        if (enemy != nullptr && enemy->getBody() != nullptr && !enemy->isRemoved()) {
            enemy->fixedUpdate(step);
        }
        if (enemy->getHealth() <= 0) {
            enemy->markForDeletion();
        }
        if (enemy->shouldDelete()) {
            removeEnemy(enemy.get());
        }
    }

    //attacks
    for (auto it = _attacks.begin(); it != _attacks.end();) {
        if ((*it) == nullptr || (*it)->isRemoved()) {
            ++it;
            continue;
        }
        else {
            (*it)->fixedUpdate(step);
        }

        if ((*it)->killMe()) {
            removeAttack((*it).get());
            it = _attacks.erase(it);
        }
        else {
            ++it;
        }
    }

    //interactables
    /*for (auto it = _interactables.begin(); it != _interactables.end();) {
        if ((*it) == nullptr || (*it)->isRemoved()) {
			
		}
	}*/

    //plates
    bool allPlatesDone = !_plates.empty();
    for (auto it = _plates.begin(); it != _plates.end();) {
        if ((*it) == nullptr || (*it)->isRemoved()) {
			++it;
			continue;
		}
        else {
            if (!(*it)->isSuccess()) {
                allPlatesDone = false;
            } else{
                CULog("SUcceeded");
                (*it)->getSceneNode()->setColor(Color4::GREEN);
            }
			++it;
		}
	}
    if (allPlatesDone) {
        setComplete(true);
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

            //this is where we will change the scene width and heights for everything
            advanceLevel();
            reset();

            //if (_level_model->getFilePath() == "json/intermediate.json") {
            //    //_bgScene->setColor(Color4::BLACK);
            //    _level_model->removeBackgroundImages(*this);
            //    _level_model->setFilePath("json/empanada-platform-level-01.json");
            //    currentLevel = _level_model;

            //    CULog("We should switch to our first initial level");
            //    // currentLevel
            //    reset();
            //}
            //else if (_level_model->getFilePath() == "json/empanada-platform-level-01.json") {
            //    //_bgScene->setColor(Color4::BLACK);
            //    _level_model->removeBackgroundImages(*this);
            //    _level_model->setFilePath("json/test_level_v2_experiment.json");
            //    currentLevel = _level_model;
            //    
            //    CULog("We should switch to our first initial level");
            //    // currentLevel
            //    reset();
            //}
            //else if (_level_model->getFilePath() == "json/test_level_v2_experiment.json") {
            //    _bgScene->setColor(Color4::BLACK);
            //    _level_model->removeBackgroundImages(*this);
            //    _level_model->setFilePath("json/bull-boss-level.json");
            //    currentLevel = _level_model;

            //    CULog("We should switch to the bull boss level");
            //    // currentLevel
            //    reset();
            //}
            //else if (currentLevel == level3) {
            //    currentLevel = _level_model;
            //    reset();
            //}
            //else {
            //    currentLevel = _level_model;
            //    reset();
            //}
        }
            // advanceLevel();
            // reset();
        // }
        else if (_failed) {
            reset();
        }
    }
}

/* Checks input for cooktime or discard 
*  if discard, it removes ingredient from inventory
*  if cooktime, it initiates cooktime by setting _slowed = true, and the dollar node gestures
*/
void GameScene::checkForCooktime() {
    if (_input->getLastSlowHeldDuration() > 0.0f) {
        CULog("Slow Duration %f, %d", _input->getLastSlowHeldDuration(), _input->justReleasedSlow());
    }
    if (_input->getLastSlowHeldDuration() < MIN_DISCARD_START_TIME && _input->justReleasedSlow() && _avatar->getMeter() > METER_COST) {

        std::shared_ptr<Ingredient> ing = _inventoryNode->popIngredientFromSlot(_inventoryNode->getSelectedSlot());
        //
        if (ing != nullptr) {
            _slowed = true;
            _dollarnode->setTargetGesturesNighttime({ ing->getGestures(), ing->getGestures() });
            _dollarnode->setIngredientInStation(ing);
            _dollarnode->setIsStation(false);
            _avatar->useMeter();
        }

        //Old CookTime Style commented out
        //if (_slowed) {
        //    //_slowed = !_slowed;
        //    //_target = nullptr;
        //}
        ////activate cooktime
        //else {
        //    CULog("Activate!");
        //    float minDist = FLT_MAX;
        //    for (auto& e : _enemies) {
        //        if (e->isRemoved() || !(e->isVulnerable())) {
        //            continue;
        //        }
        //        if ((e->getPosition() - _avatar->getPosition()).length() < minDist) {
        //            _target = e;
        //            minDist = (e->getPosition() - _avatar->getPosition()).length();
        //        }
        //    }
        //    if (minDist < COOKTIME_MAX_DIST) {
        //        _slowed = true;
        //        //_dollarnode->setTargetGestures(_target->getGestureSeq1());
        //        _dollarnode->setTargetGesturesNighttime(std::vector({_target->getGestureSeq1(), _target->getGestureSeq2()}));
        //    }

        //}

    }
    else if (_input->getLastSlowHeldDuration() > DISCARD_HOLD_TIME && _input->justReleasedSlow()) {
        std::shared_ptr<Ingredient> ing = _inventoryNode->popIngredientFromSlot(_inventoryNode->getSelectedSlot());
        //i think thats all
    }
}

//void GameScene::renderBG(std::shared_ptr<cugl::SpriteBatch> batch) {
//    _bgScene->render(batch);
//}

void GameScene::renderUI(std::shared_ptr<cugl::SpriteBatch> batch) {
    //render minimap
    std::shared_ptr<Texture> t;
    if (_minimapNode->isVisible()) {
        _r->begin();
        batch->begin(_minimapCamera->getCombined());
        batch->setSrcBlendFunc(_srcFactor);
        batch->setDstBlendFunc(_dstFactor);
        batch->setBlendEquation(_blendEquation);

        for (auto it = _children.begin(); it != _children.end(); ++it) {
            (*it)->render(batch, Affine2::IDENTITY, _color);
        }

        batch->end();
        _r->end();

        _minimapNode->setTexture(_r->getTexture());
        _minimapNode->flipVertical(true);
    }

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
        if (!_failed && !_complete) {
            std::shared_ptr<Sound> source = _assets->get<Sound>(LOSE_MUSIC);
            AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
            _losenode->setVisible(true);
            _countdown = EXIT_COUNT;
        }
    }
    else {
        _losenode->setVisible(false);
        _countdown = -1;
    }
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
    //attack->dispose();

    std::shared_ptr<Sound> source = _assets->get<Sound>(POP_EFFECT);
    AudioEngine::get()->play(POP_EFFECT, source, false, EFFECT_VOLUME, true);
}





//Basically the same as removeAttack, can refactor
void GameScene::removeEnemy(EnemyModel* enemy) {
    if (enemy->isRemoved()) {
        return;
    }
    CULog("removing");

    addEnemyToInventory(enemy->getType());

    _worldnode->removeChild(enemy->getSceneNode());
    enemy->setDebugScene(nullptr);
    enemy->markRemoved(true);
    //enemy->dispose();

    std::shared_ptr<Sound> source = _assets->get<Sound>(POP_EFFECT);
    AudioEngine::get()->play(POP_EFFECT, source, false, EFFECT_VOLUME, true);
}

void GameScene::addEnemyToInventory(EnemyType enemyType) {
    IngredientProperties props = enemyToIngredientMap[enemyType];

    std::shared_ptr<Ingredient> ing = std::make_shared<Ingredient>("", props.gestures, 0.0f);
    ing->setName(props.name);
    std::shared_ptr<Texture> tex = _assets->get<Texture>(ing->getName());
    ing->init(tex);

    _inventoryNode->addIngredient(ing);
}


/**
 * Returns the active screen size of this scene.
 *
 * This method is for graceful handling of different aspect
 * ratios
 */
Size GameScene::computeActiveSize() const {
    Size dimen = Application::get()->getDisplaySize();
    float ratio1 = dimen.width / dimen.height;
    float ratio2 = ((float)_scene_width) / ((float)_scene_height);
    if (ratio1 < ratio2) {
        dimen *= (_scene_width * 32.0f) / dimen.width;
    }
    else {
        dimen *= (_scene_height * 32.0f) / dimen.height;
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

    //auto reader = JsonReader::alloc(path);
    //std::shared_ptr<JsonValue> prevSave = reader->readJson();
    //reader->close();

    std::shared_ptr<JsonValue> json = JsonValue::allocObject();

    //std::shared_ptr<JsonValue> persistent = prevSave->get("persistent");
    std::shared_ptr<JsonValue> night = JsonValue::allocObject();
    //CHAPTER COMPLETION LOGIC
    if (isComplete()) {
        //if completed, don't need to save state. Just increment chapter and level accordingly
        json->appendValue("chapter", 1.0f);
        json->appendValue("level", 2.0f);
        json->appendValue("startFromNight", false);
        //json->appendChild("persistent", persistent);
        json->appendChild("night", night);
        return;
    }
    //else {
    //    if (persistent == nullptr || persistent->isNull()) {
    //        persistent = JsonValue::allocObject();
    //    }
    //    else {
    //        persistent->_parent = nullptr;
    //    }
    //}
    std::shared_ptr<JsonValue> player = JsonValue::allocObject();
    player->appendValue("location_x", (double) _avatar->getPosition().x);
    player->appendValue("location_y", (double) _avatar->getPosition().y);
    player->appendValue("health", (double) _avatar->getHealth());
    night->appendChild("player", player);

    std::vector<std::string> types = { "egg", "carrot", "shrimp", "rice", "beef", "rice_soldier"};
    for (auto t = types.begin(); t != types.end(); t++) {
        std::string type = *t;
        night->appendChild(type, JsonValue::allocObject());
    }

    for (auto& e : _enemies) {
        std::string type = EnemyModel::typeToStr(e->getType());
        std::shared_ptr<JsonValue> x = JsonValue::allocObject();
		x->appendValue("location_x", (double) e->getPosition().x);
        x->appendValue("location_y", (double) e->getPosition().y);
        x->appendValue("health", (double) e->getHealth());
        x->appendValue("isDead", e->isRemoved());
        night->get(type)->appendChild(e->getId(), x);
	}

    //placeholder  values for chapter and level
    json->appendValue("chapter", 1.0f);
    json->appendValue("level", 1.0f);
    json->appendValue("startFromNight", true);
    //json->appendChild("persistent", persistent);
    json->appendChild("night", night);

    auto writer = JsonWriter::alloc(path);
    writer->writeJson(json);
    writer->close();
}

bool GameScene::loadSave(std::shared_ptr<JsonValue> save) {
    if (save->size() == 0) {
        reset();
        return true;
    }
    
    int chap = save->getInt("chapter");
    int level = save->getInt("level");
    changeCurrentLevel(chap, level);
    reset();

    // max health, dash cooldown, attack damage, speed, metergain
    float locationX = save->get("player")->getFloat("location_x");
    float locationY = save->get("player")->getFloat("location_y");
    float health = save->get("player")->getFloat("health");
    _avatar->setPosition(locationX, locationY);
    _avatar->setHealth(health);
    for (auto& e : _enemies) {
        std::shared_ptr<JsonValue> enemy = save->get(EnemyModel::typeToStr(e->getType()))->get(e->getId());
        if (enemy->getBool("isDead", false)) {
            removeEnemy(e.get());
            continue;
        }
        e->setPosition(enemy->getFloat("location_x"), enemy->getFloat("location_y"));
        e->setHealth(enemy->getFloat("health"));
    }

    //todo: persistent


    return true;
}

//load level with int specifiers
void GameScene::loadLevel(int chapter, int level) {
    changeCurrentLevel(chapter, level);
    loadLevel(currentLevel);
}

void GameScene::advanceLevel() {
    _level += 1;
    _level = _level % 6;
    if (_level == 0) _level = 1;
    changeCurrentLevel(_chapter, _level);
}

void GameScene::setLevel(int chapter, int level) {
    _level = level;
    _chapter = chapter;
    reset();
}

void GameScene::changeCurrentLevel(int chapter, int level) {
    currentLevel = _level_model;
    if (chapter == 1) {
        if (level == 1) {
            _level_model->setFilePath("json/intermediate.json");
        }
        else if (level == 2) {
            _level_model->setFilePath("json/test_level_v2_experiment.json");
        }
        else if (level == 3) {
            _level_model->setFilePath("json/empanada-platform-level-01.json");
        }
        else if (level == 4) {
            //currentLevel = level2;
            _level_model->setFilePath("json/bull-boss-level.json"); 
        }
        else if (level == 5) {
            currentLevel = level3;
        }
    }
}

void GameScene::spawnShrimp(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("shrimpIdle");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 1, 1, 1);
    Size s = Size(2.0f, 2.0f);
    std::shared_ptr<EnemyModel> new_enemy = Shrimp::allocWithConstants(pos,s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}
void GameScene::spawnBeef(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("beefIdle");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
    Size s = cugl::Size(8.0f, 8.0f);
    std::shared_ptr<EnemyModel> new_enemy = Beef::allocWithConstants(pos, s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}
void GameScene::spawnEgg(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("eggIdle");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
    Size s = Size(2.25f, 6.0f);
    std::shared_ptr<EnemyModel> new_enemy = Egg::allocWithConstants(pos, s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    spritenode->setAnchor(0.5, 0.35);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}
void GameScene::spawnRice(Vec2 pos, bool isSoldier) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("riceLeader");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);
    float imageWidth = image->getWidth() / 4;
    float imageHeight = image->getHeight() / 4;
    Size singularSpriteSize = Size(imageWidth, imageHeight);
    std::shared_ptr<EnemyModel> new_enemy = Rice::allocWithConstants(pos, singularSpriteSize / (5 * getScale()), getScale(), _assets, isSoldier);
    spritenode->setAnchor(Vec2(0.5, 0.35));
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}

void GameScene::spawnCarrot(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("eggIdle");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
    Size s = Size(2.25f, 6.0f);
    std::shared_ptr<EnemyModel> new_enemy = Egg::allocWithConstants(pos, s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    spritenode->setAnchor(0.5, 0.35);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}

void GameScene::spawnStation(Vec2 pos, StationType type) {
    //obstacle has small size, not reflective of intended size
    Size s = Size(5.0f, 5.0f);
    std::shared_ptr<Texture> image;
    switch (type) {
    case StationType::CUT: {
        image = _assets->get<Texture>("knife");
        break;
    }
    case StationType::BOIL: {
        image = _assets->get<Texture>("pot");
        break;
    }
    case StationType::FRY: {
		image = _assets->get<Texture>("pan");
		break;
    } default: {
        image = _assets->get<Texture>("knife");
		break;
    }

    }
    std::shared_ptr<Station> station = Station::alloc(image, pos, s, type);

    addObstacle(station, station->getSceneNode());
    _interactables.push_back(station);
    _stations.push_back(station);
}

void GameScene::spawnPlate(Vec2 pos, std::unordered_map<IngredientType, int> map) {
    //obstacle has small size, not reflective of intended size
    Size s = Size(5.0f, 5.0f);
    std::shared_ptr<Texture> image = _assets->get<Texture>("sink");
    std::shared_ptr<Plate> plate = Plate::alloc(image, pos, s, map);

    addObstacle(plate, plate->getSceneNode());
    _interactables.push_back(plate);
    _plates.push_back(plate);
}

void GameScene::pogo() {
    float pi = 3.1416f;
    float angle = _avatar->getAngle();
    CULog("ANGLE: %f", angle);
    //pogo
    if (_avatar->getActiveAction() == "air_attack" && std::abs(angle) > pi / 2.0f && std::abs(angle) < 3 * pi / 2.0f) {
        //_avatar->setLinearVelocity(_avatar->getLinearVelocity().x, 0.0f);
        _avatar->jump(Vec2(std::sin((angle - pi) / 2.0f), 1 - std::cos((angle - pi) / 2.0f)));
    }
}

void GameScene::createOrder(int plateId, IngredientType ing, bool isPlate) {
    std::shared_ptr<scene2::SceneNode> order = scene2::SceneNode::alloc();
    std::shared_ptr<scene2::PolygonNode> background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("orderBackground"));
    order->addChild(background);
    std::shared_ptr<Texture> texture;
    switch (ing) {
    case IngredientType::cutCarrot: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateCarrotOrder");
        }
        else {
            texture = _assets->get<Texture>("cutCarrotOrder");
        }
        break;
    }

    case IngredientType::boiledEgg: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateBoiledEggOrder");
        }
        else {
            texture = _assets->get<Texture>("boilEggOrder");
        }
        break;
    }
    case IngredientType::scrambledEgg: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateScrambledEggOrder");
        }
        else {
            texture = _assets->get<Texture>("scrambledEggOrder");
        }
        break;
    }
    case IngredientType::cookedShrimp: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateShrimpOrder");
        }
        else {
            texture = _assets->get<Texture>("fryShrimpOrder");
        }
        break;
    }
    case IngredientType::boiledRice: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateRiceOrder");
        }
        else {
            texture = _assets->get<Texture>("boilRiceOrder");
        }
        break;
    }
    case IngredientType::cookedBeef: {
        if (isPlate) {
            texture = _assets->get<Texture>("plateBeefOrder");
        }
        else {
            texture = _assets->get<Texture>("fryBeefOrder");
        }
        break;
    }
    default: {
        texture = nullptr;
        CULog("Order error");
    }
    }
    std::shared_ptr<scene2::PolygonNode> text = scene2::PolygonNode::allocWithTexture(texture);
    float scale = ORDER_WIDTH / texture->getWidth();
    text->setPosition(0, 0);
    text->setScale(scale);
    background->setPosition(0,0);
    background->setScale(scale);
    order->addChild(text);
    order->setScale(scale);
    order->setContentWidth(ORDER_WIDTH);
    order->setName(Ingredient::getIngredientStringFromType(ing) + "Order");

    _orders[plateId].push_back(order);
    _orderNode->addChild(_orders[plateId].back());
    _numOrders += 1;
    positionOrders();
}

void GameScene::removeOrder(int plateId, IngredientType ing, bool isPlate) {
    for (auto it = _orders[plateId].begin(); it != _orders[plateId].end(); it++) {
        if ((*it) != nullptr && (*it)->getName() == (Ingredient::getIngredientStringFromType(ing) + "Order")) {
            std::shared_ptr<scene2::SceneNode> order = *it;
            _orderNode->removeChild(order);
            _orders[plateId].erase(it);
            order->dispose();
            _numOrders -= 1;
            positionOrders();
            return;
		}
	}

}

void GameScene::toggleOrders(bool v) {
    //todo polish, moving them instead of popping them in and out
    for (auto const& t : _orders) {
        for (auto& b : t.second) {
            b->setVisible(v);
        }
	}
}

void GameScene::positionOrders() {
    CULog("positioning");
    float totalWidth = _numOrders * ORDER_WIDTH;
    float start = 0;
    for (auto& t : _orders) {
		int i = 0;
        for (auto& b : t.second) {
			b->setPositionX(start);
            start += ORDER_WIDTH;
		}
	}
    _orderNode->setContentWidth(totalWidth);
    _orderNode->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    _orderNode->setPosition(1280 / 2, 800 - 50);
}

void GameScene::generateOrders() {
    _numOrders = 0;
    for (auto& p : _plates) {
        std::vector<IngredientType> x = p->getTargetIngredients();
        int id = p->getId();
        _orders[id] = std::vector<std::shared_ptr<scene2::SceneNode>>();
        for (IngredientType t : x) {
            createOrder(id, t, false);
        }
    }
}