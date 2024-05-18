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

#define MINIMAP_ZOOM 0.25f

#define MINIMAP_WIDTH 1280/2

#define MINIMAP_HEIGHT 800/2

#define TIMER_DIAMETER_SIZE 40.0f

#define CAMERA_FOLLOWS_PLAYER true

#define COOKTIME_MAX_DIST 3.5f

#define FEEDBACK_DURATION 1.2f

#define HEALTHBAR_X_OFFSET 15
#define DISCARD_HOLD_TIME 2.0f
#define MIN_DISCARD_START_TIME 0.25f
/**desired order width in pixels*/
#define INVENTORY_OFFSET 30.0f




std::map<EnemyType, IngredientProperties> enemyToIngredientMap = {
    {EnemyType::beef, {"beef", EnemyModel::defaultSeq(EnemyType::beef)}},
    {EnemyType::carrot, {"carrot", EnemyModel::defaultSeq(EnemyType::carrot)}},
    {EnemyType::egg, {"egg", EnemyModel::defaultSeq(EnemyType::egg)}},
    {EnemyType::rice, {"rice", EnemyModel::defaultSeq(EnemyType::rice)}},
    {EnemyType::rice_soldier, {"rice", EnemyModel::defaultSeq(EnemyType::rice_soldier)}},
    {EnemyType::shrimp, {"shrimp", EnemyModel::defaultSeq(EnemyType::shrimp)}}
};

std::map<std::string, IngredientProperties> ingredientNameToIngredientProperties = {
    {"beef", {"beef", EnemyModel::defaultSeq(EnemyType::beef)}},
    {"carrot", {"carrot", EnemyModel::defaultSeq(EnemyType::carrot)}},
    {"egg", {"egg", EnemyModel::defaultSeq(EnemyType::egg)}},
    {"rice", {"rice", EnemyModel::defaultSeq(EnemyType::rice)}},
    {"rice_soldier", {"rice", EnemyModel::defaultSeq(EnemyType::rice_soldier)}},
    {"shrimp", {"shrimp", EnemyModel::defaultSeq(EnemyType::shrimp)}}
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

std::map<std::string, buff> gestureToBuff = {
    {"triangle", buff::speed},
    {"caret", buff::jump},
    {"pigtail", buff::health},
    {"v", buff::defense},
    {"circle", buff::attack},
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
    // _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/sfrBoss.json");
    // _level_model->setFilePath("json/empanada_level_3_final.json");
    _level_model->setFilePath("json/johntutorial_colored.tmj");
    // _level_model->setFilePath("json/TestLevel1.json");
    // _level_model->setFilePath("json/SFRLevel3.tmj");
    // _level_model->setFilePath("json/empanada level 4.json");
    // _level_model->setFilePath("json/empanada-platform-level-01.json");
    // _level_model->setFilePath("json/bull-boss-level.json");
    setSceneWidth(_level_model->loadLevelWidth());
    setSceneHeight(_level_model->loadLevelHeight());
    return init(assets, Rect(0, 0, getSceneWidth(), getSceneHeight()), Vec2(0, DEFAULT_GRAVITY), input);
}

bool GameScene::initWithSave(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::shared_ptr<JsonValue> save) {
    /*setSceneWidth(400);
    setSceneHeight(30);*/
    // _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/sfrBoss.json");
    // _level_model->setFilePath("json/empanada_level_3_final.json");
    _level_model->setFilePath("json/johntutorial_colored.tmj");
    // _level_model->setFilePath("json/TestLevel1.json");
    // _level_model->setFilePath("json/SFRLevel3.tmj");
    // _level_model->setFilePath("json/empanada level 4.json");
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
    // _level_model->setFilePath("json/intermediate.json");
    // _level_model->setFilePath("json/sfrBoss.json");
    // _level_model->setFilePath("json/empanada_level_3_final.json");
    _level_model->setFilePath("json/johntutorial_colored.tmj");
    // _level_model->setFilePath("json/TestLevel1.json");
    // _level_model->setFilePath("json/SFRLevel3.tmj");
    // _level_model->setFilePath("json/empanada level 4.json");
    _timer = 0.0f;
    _timeLimit = 200.0f;
    _respawnTimes = std::deque<float>({10.0f, 100.0f, 150.0f, 200.0f});
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
    _worldnode->setName("worldnode");

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
    _inventoryNode->init(_assets, _input, Size(invTex->getWidth()*NUM_SLOTS, invTex->getHeight()));
    _inventoryNode->setName("inventoryNode");
    _inventoryNode->setAnchor(Vec2::ANCHOR_TOP_LEFT);
    // hardcode :)
    _inventoryNode->setPosition(Vec2(INVENTORY_OFFSET, 900 - INVENTORY_OFFSET));

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

    float off_x = 950.0f;
    _healthBarForeground->setPositionX(_healthBarForeground->getPositionX() + off_x);
    _healthBarBackground->setPositionX(_healthBarBackground->getPositionX() + off_x);

    _cookBarOutline = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifeoutline"));
    _cookBarFill = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifefill"));

    _cookBarOutline->setPositionX(_cookBarOutline->getPositionX() + off_x);
    _cookBarFill->setPositionX(_cookBarFill->getPositionX() + off_x);

    //_cookBarGlow = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("gainingboost")->getChildByName("knifeglow"));
    for (std::string s : {"attackfill", "shieldfill", "speedfill", "healthfill", "jumpfill"}) {
        auto x = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("boost")->getChildByName(s));
        x->setVisible(false);
        x->setPositionX(x->getPositionX() + off_x);
	}
    for (std::string s : {"attackready", "shieldready", "speedready", "healthready", "jumpready"}) {
        _cookBarIcons[s] = std::dynamic_pointer_cast<scene2::PolygonNode>(_meterUINode->getChildByName("boost")->getChildByName(s));
        _cookBarIcons[s]->setVisible(false);
        _cookBarIcons[s]->setPositionX(_cookBarIcons[s]->getPositionX() + off_x);
    }


    std::shared_ptr<cugl::scene2::SceneNode> _bullBarNode;
    _bullBarNode = _assets->get<scene2::SceneNode>("bullbar");
    _BullhealthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_bullBarNode->getChildByName("fullbullbar")->getChildByName("bullbar"));
    _BullhealthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_bullBarNode->getChildByName("fullbullbar")->getChildByName("bosshealth"));
    _BullhealthBarBackground->setVisible(false);
    _BullhealthBarForeground->setVisible(false);
    _uiScene->addChild(_bullBarNode);

    std::shared_ptr<cugl::scene2::SceneNode> _SFRBarNode;
    _SFRBarNode = _assets->get<scene2::SceneNode>("SFR");
    _SFRhealthBarBackground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("shrimpbar"));
    _SFRhealthBarForeground = std::dynamic_pointer_cast<scene2::PolygonNode>(_SFRBarNode->getChildByName("bosshealth"));
    _SFRhealthBarBackground->setVisible(false);
    _SFRhealthBarForeground->setVisible(false);
    _uiScene->addChild(_SFRBarNode);
   
    _paused = false;
    
    //_uiScene->addChild(_pauseButton);
    _uiScene->addChild(_meterUINode);
    _uiScene->addChild(_dollarnode);

    _uiScene->addChild(_winnode);
    _uiScene->addChild(_losenode);
    _uiScene->addChild(_inventoryNode);


    _pauseMenu = std::make_shared<MenuScene>();
    _pauseMenu->init(_assets, "pause");

    _loseScreen = std::make_shared<MenuScene>();
    _loseScreen->init(_assets, "loseScreen");
    
    _winScreen = std::make_shared<MenuScene>();
    _winScreen->init(_assets, "winScreen");
    


# pragma mark: Background

    /*_bgScene = cugl::Scene2::alloc(dimen);
    _bgScene->init(dimen);
    _bgScene->setActive(true);
     _bgScene = cugl::Scene2::alloc(cugl::Size(210, 25));
     _bgScene->init(cugl::Size(210, 25));
     _bgScene->setActive(true);*/

    cugl::Rect rectB = cugl::Rect(Vec2::ZERO, computeActiveSize());
    // Q: Can we create a background that isn't the whole size of the scene?
    // _bgScene->addChild(_background);
    // _background = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>("cutting_station"), rectB);
    // _background = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>(_level_model->getLevelScenery("1")));
    // _bgScene->addChild(_background);
    // _bgScene->addChild(_background);

    _background = cugl::scene2::PolygonNode::allocWithTexture(nullptr, rectB);
    _background->setVisible(true);
    _background->setColor(Color4::YELLOW);
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
    _minimapNode->setVisible(false);
    _minimapNode->setAnchor(Vec2::ANCHOR_CENTER);
    _minimapNode->setPosition(1280/ 2, 800 / 2);

    _minimapIconNode = scene2::PolygonNode::alloc();
    _minimapIconNode->setVisible(false);
    _worldnode->addChild(_minimapIconNode);
    _uiScene->addChild(_minimapNode);

    _minimapIcons = std::unordered_map<std::string, std::shared_ptr<Texture>>();
    _minimapIcons["su"] = _assets->get<Texture>("suIcon");
    _minimapIcons["pot"] = _assets->get<Texture>("potIcon");
    _minimapIcons["pan"] = _assets->get<Texture>("panIcon");
    _minimapIcons["cut"] = _assets->get<Texture>("knifeIcon");
    _minimapIcons["plate"] = _assets->get<Texture>("plateIcon");

    _timerIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("timer"));
    _timerIcon->setScale(TIMER_DIAMETER_SIZE / _timerIcon->getContentWidth());
    _timerIcon->setPosition(1280/2.0f, 800 - TIMER_DIAMETER_SIZE - 10);
    _uiScene->addChild(_timerIcon);
    
    _timerFillIcon = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("timerFill"));
    _timerFillIcon->setScale(TIMER_DIAMETER_SIZE / _timerFillIcon->getContentWidth());
    _timerFillIcon->setPosition(1280/2.0f, 800 - TIMER_DIAMETER_SIZE - 10);
    _uiScene->addChild(_timerFillIcon);

    _actionManager = cugl::scene2::ActionManager::alloc();
    _BullactionManager = cugl::scene2::ActionManager::alloc();
    _SHRactionManager = cugl::scene2::ActionManager::alloc();

    _pendingAcrossAllPlates = std::unordered_map<IngredientType, int>();

    _interactivePopups = std::vector<std::shared_ptr<Popup>>();

   _chapter = 1;
   _level = 1;
   loadLevel(_chapter, _level);
   // currentLevel = _level_model;
   // loadLevel(currentLevel);
    addChild(_worldnode);
    addChild(_debugnode);
    _numOrders = 0;

    _ordersObj = std::make_shared<Orders>();
    _ordersObj->init(_assets);
    _ordersObj->setVisible(true);
    _ordersObj->setPosition(115, 600);
    _ordersObj->setActive(true);
    _uiScene->addChild(_ordersObj);

    _orders = std::unordered_map<int, std::vector<std::shared_ptr<scene2::SceneNode>>>();
    _orderNode = scene2::SceneNode::alloc();
    generateOrders();
    _orderNode->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    _orderNode->setPosition(1280 / 2, 800 - 50);
    _orderNode->setVisible(true);
    _uiScene->addChild(_orderNode);

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

    std::vector<std::string> paths = { 
        "tutorialPopupWASD",
        "tutorialPopupDash",
        "tutorialPopupAttack",
        "tutorialPopupInventory",
        "tutorialPopupGestures",
        "tutorialPopupStations",
        "tutorialPopupPlates",
        "tutorialPopupTimerDying",
        "tutorialPopupMinimap"
    };

    std::shared_ptr<Scene2Loader> loader = Scene2Loader::alloc();
    for (std::string path : paths) {
		auto reader = JsonReader::alloc("./json/Tutorials/" + path + ".json");
        std::shared_ptr<JsonValue> popupData = reader->readJson()->get(path);
        std::shared_ptr<Popup> p = Popup::allocWithData(_assets, _actionManager, loader.get(), popupData);
        p->setActive(false);
        p->setVisible(false);
        _interactivePopups.push_back(p);
        _uiScene->addChild(p);
        reader->close();
        reader = nullptr;
    }
    _popupIndex = 0;
    loader->dispose();
    loader = nullptr;

    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        _assets = nullptr;
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
        _input->dispose();
        _input = nullptr;
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
    _pauseMenu->setActive(false);
    _pauseMenu->reset();
    _loseScreen->setActive(false);
    _loseScreen->reset();
    _worldnode->removeAllChildren();
    _world->clear();
    _debugnode->removeAllChildren();
    _avatar->reset();
    _goalDoor = nullptr;
    _background = nullptr;
    _sensorFixtures.clear();
    _inventoryNode->reset();
    _winScreen->setActive(false);
    _winScreen->reset();
    _complete = false;
    _failed = false;
    for (auto& enemy : _enemies) {
        enemy = nullptr;
    }
    _enemies.clear();
    for (auto& attack : _attacks) {
        attack = nullptr;
    }
    _attacks.clear();
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

    _ordersObj->reset();
    for (auto& i : _interactivePopups) {
        i->dispose();
        i = nullptr;
    }
    _interactivePopups.clear();
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


    _level_model->removeBackgroundImages(*this);
    loadLevel(_chapter, _level);
    addChild(_worldnode);
    addChild(_debugnode);

    generateOrders();
    _timer = 0.0f;
    _timeLimit = 200.0f;
    _respawnTimes = std::deque<float>({ 50.0f, 100.0f, 150.0f, 200.0f });
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
    if (node == nullptr || obj == nullptr) return;
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
    //if (_input->didReset()) { reset(); }
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
    bool flag = false;
    if (_input->didReset() && _paused) {
        flag = true;
        _paused = false;
	}

    _pauseMenu->setActive(_paused);
    _ordersObj->setActive(_paused);

    if (_paused) {
        if (_pauseMenu->getReset()) {
            reset();
        }
        else if (_pauseMenu->didTransition()){
            transition(true);
            setTarget(_pauseMenu->getTarget());
            _pauseMenu->setTarget("");
            _pauseMenu->setTransition(false);
            _paused = false;
        }   
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
                        removeOrder(i->getId(), t);
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
                        removeingredient(i->getPosition(), Ingredient::getIngredientStringFromType(t));
                    }
                    else if (i->isFull()) {

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


    for (auto& i : _interactivePopups) {
        if (i->isActive()) i->update(dt);
    }
    
    //handle animations
    if (!_actionManager->isActive("air_attack")) {
        _avatar->getBody()->SetFixedRotation(false);
        _avatar->getBody()->SetTransform(_avatar->getBody()->GetPosition(), 0.0f);
        _avatar->getBody()->SetFixedRotation(true);
    }
    //if (isFailure() && _avatar->getActiveAction() != "death") {
    if (_avatar->isDead()){
        animate(_avatar, "death", true);
    }
    else if (!_paused) {
        if ((_actionManager->isActive("idle") || _actionManager->isActive("recover") || _actionManager->isActive("skid")) && (_input->getHorizontal() != 0)) {
            animate(_avatar, "run", true);
        }
        //cancel run animation if stopped running
        else if (_actionManager->isActive("run") && _input->getHorizontal() == 0) {
            animate(_avatar, "skid", true);
        }

        else if (_avatar->isJumping() && _avatar->isGrounded()) {
            animate(_avatar, "jump_ready", true);
        }

        else if (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() < _avatar->getFloatyFrames() && !_actionManager->isActive("air_roll")) {
            animate(_avatar, "air_roll", true);
        }


        //animate jumps if not attacking or taking damage
        else if (!_avatar->isGrounded() && !_actionManager->isActive("attack") && !_actionManager->isActive("jump_up") && !_actionManager->isActive("air_attack") && 
            !(_avatar->getActiveAction() == "air_attack") && !_actionManager->isActive("air_recover") && !_actionManager->isActive("air_roll")
            && (_avatar->getLastDamageTime() > _avatar->getHealthCooldown())) {
            if (_avatar->getLinearVelocity().y > 0) animate(_avatar, "jump_up", true);
            else animate(_avatar, "jump_down", true);
        }
        else if (_avatar->isGrounded() && (_actionManager->isActive("jump_down") || _actionManager->isActive("jump_up"))) {
            //CULog("animating jump_land");
            animate(_avatar, "jump_land", true);
        }
        else if (_avatar->getLastDamageTime() < _avatar->getHealthCooldown() && !_actionManager->isActive("hurt") && !_avatar->didAnimateHurt()) {
            animate(_avatar, "hurt", true);
            _avatar->setDidAnimateHurt(true);
        }

        //handle expired actions
        else if (!_actionManager->isActive(_avatar->getActiveAction())) {
            if (_avatar->getActiveAction() == "attack") {
                //CULog("animating attack");
                animate(_avatar, "recover", false);
            }
            else if (_avatar->getActiveAction() == "run" && _input->getHorizontal() != 0) {
                animate(_avatar, "run", true);
            }
            else  if (_avatar->getActiveAction() == "air_attack") {
                animate(_avatar, "air_recover", true);
            }
            else if (_avatar->getActiveAction() == "death") {
                //do nothing
            }
            else {
                //Todo:: blink idle
                //CULog("animating idle");
                //if (((float)rand() / RAND_MAX) < 0.0f) {
                //    animate(_avatar, "idle_blink", true);
                //}
                //else {
                animate(_avatar, "idle");
                //}
            }
        }
        else {
            //CULog("ANIMATION ERROR!!!!");
            //CULog(_avatar->getActiveAction().c_str());
            //CULog(_actionManager->isActive(_avatar->getActiveAction()) ? "true" : "false");
        }

        if (_avatar->getDashCooldownMax() - _avatar->getDashCooldown() < _avatar->getFloatyFrames() && _avatar->getDashCooldownMax() - _avatar->getDashCooldown() != 0 && _avatar->getDashCooldown() % 3 == 1) {
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
        if (_avatar->isShooting() && (!_actionManager->isActive("attack") && !_actionManager->isActive("air_attack") && !_actionManager->isActive("air_recover"))) {
            if (_avatar->isGrounded()) {
                auto att = _avatar->createAttack(getAssets(), _scale);
                addObstacle(std::get<0>(att), std::get<1>(att), true);
                _attacks.push_back(std::get<0>(att));
                std::get<0>(att)->setListener([=](physics2::Obstacle* obs) {
                    obs->setPosition(_avatar->getPosition());
                    obs->setAngle(_avatar->getBody()->GetAngle());
                    });
                animate(_avatar, "attack", true);
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
                _avatar->getBody()->SetTransform(_avatar->getBody()->GetPosition(), angle - (3.14159265 / 2));
                _avatar->getBody()->SetFixedRotation(true);

                animate(_avatar, "air_attack", true);
            }
        }
    }


    if (_input->didLevel1()) {
        ////setLevel(1, 1);
        //_interactivePopups.back()->toggle();
        _interactivePopups.at(_popupIndex)->toggle();
    }
    else if (_input->didLevel2()) {
        /*setLevel(1, 2);*/
        bool b = _interactivePopups.at(_popupIndex)->isActive();
        if (b) {
            _interactivePopups.at(_popupIndex)->toggle();
            _popupIndex--;
            if (_popupIndex < 0) {
                _popupIndex = _interactivePopups.size() - 1;
            }
            _interactivePopups.at(_popupIndex)->toggle();
        }
    }
    else if (_input->didLevel3()) {
        /*setLevel(1, 3);*/
        bool b = _interactivePopups.at(_popupIndex)->isActive();
        if (b) {
            _interactivePopups.at(_popupIndex)->toggle();
            _popupIndex++;
            if (_popupIndex == _interactivePopups.size()) {
                _popupIndex = 0;
            }
            _interactivePopups.at(_popupIndex)->toggle();
        }
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
        handleCooktime();
    }

    //CULog("sprite width/height: %f %f", _avatar->getSceneNode()->getSize().width, _avatar->getSceneNode()->getSize().height);
    //CULog("obstacle width/height: %f %f", _avatar->getHeight() * _scale, _avatar->getWidth() * _scale);

    //CULog("camera sprite width/height: %f %f", _avatar->getSceneNode()->getSize().width * _camera->getZoom(), _avatar->getSceneNode()->getSize().height * _camera->getZoom());




    if (!_slowed) {
        _dollarnode->setVisible(false);
        if (_dollarnode->isFocus()) {
            _dollarnode->setFocus(false);
            _dollarnode->setReadyToCook(false);
        }

        //_avatar->setMovement(_input->getHorizontal() * _avatar->getForce());
        if (!_avatar->isDead()) {
            _avatar->setAllMovement(_input->getHorizontal(), _input->getVertical());
            _avatar->setJumping(_input->didJump());
            _avatar->setDash(_input->didDash());
            _avatar->setInputWalk(_input->getHorizontal() != 0);
        }
        else {
            _avatar->setAllMovement(0.0f, 0.0f);
            _avatar->setJumping(false);
            _avatar->setDash(false);
            _avatar->setInputWalk(false);
        }
        if (_avatar->isJumping() && _avatar->isGrounded()) {
            std::shared_ptr<Sound> source = _assets->get<Sound>(JUMP_EFFECT);
            AudioEngine::get()->play(JUMP_EFFECT, source, false, EFFECT_VOLUME);
        }
    }
    else {
        _avatar->setAllMovement(0, 0);
        _avatar->setJumping(false);
        _avatar->setDash(false);
        _avatar->setInputWalk(false);
    }

    //if (_avatar->getDuration() == 0 && !_avatar->hasSuper()) {
    //    //_buffLabel->setVisible(false);
    //}

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
    std::vector<std::shared_ptr<EnemyModel>> spawns = std::vector<std::shared_ptr<EnemyModel>>();
    for (auto& enemy : _enemies) {
        if (enemy != nullptr && enemy->getBody() != nullptr && !enemy->isRemoved()) {
            enemy->setAwake(true);
            enemy->update(dt);
            Vec2 enemyPos = enemy->getPosition();
            float distance = avatarPos.distance(enemyPos);

            enemy->updatePlayerDistance(_avatar->getPosition());
			if (enemy->getattacktime()) {
                //spawn rice if rice
                if (enemy->getType() == EnemyType::rice) {
                    spawns.push_back(spawnRiceSoldier(enemyPos + Vec2((rand() % 2) * enemy->getDirection(), rand() % 2), std::static_pointer_cast<Rice>(enemy)));
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
            if (_Bull->getCAcount() <= 0 && _Bull->getbreaking() <= 0 && _Bull->isChasing() && ((_Bull->getPosition().x < 18 && _Bull->getDirection() == -1) || (_Bull->getPosition().x > 58 && _Bull->getDirection() == 1))) {
                _Bull->setact("bullAttack",2.0);
            }
            if (_Bull->getCAcount()<=0 && _Bull->isChasing() && ((_Bull->getPosition().x > 20 && _Bull->getDirection() == -1) || (_Bull->getPosition().x < 56 && _Bull->getDirection() == 1))) {
                Vec2 BullPos = _Bull->getPosition();
                int direction = (avatarPos.x > BullPos.x) ? 1 : -1;
                if (direction != _Bull->getDirection()) {
                    _Bull->setbreaking(2.0f);
                }
			}
            _Bull->update(dt);
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

    if (_ShrimpRice != nullptr && !_ShrimpRice->isRemoved()) {
        Vec2 BullPos = _ShrimpRice->getPosition();
        if (_ShrimpRice->getHealth() <= 0) {
            _worldnode->removeChild(_ShrimpRice->getSceneNode());
            _ShrimpRice->setDebugScene(nullptr);
            _ShrimpRice->markRemoved(true);
        }
        if (_ShrimpRice->getact() == "SFRWave2") {
            float distance = avatarPos.distance(BullPos);
            if (distance < 4) {
                _ShrimpRice->setact("SFRWave3", 1.125);
            }
        }
        if (_ShrimpRice->gettimetosummon()) {
            _ShrimpRice->Summon(*this);
            _ShrimpRice->settimetosummon(false);
        }
        if (_ShrimpRice->getcanturn() && _ShrimpRice->getacttime() <= 0) {
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
                enemy->setPosition(enemy->getPosition() + Vec2(direction * 0.3, 0.0f));
            }
        }
        else if (_ShrimpRice->getHealth() <= 35 && _enemies.size() >= 4) {
            _ShrimpRice->setact("SFRStunState2", 5.0f);
            _ShrimpRice->setangrytime(5);
        }
        if (_ShrimpRice->getparry() && !_ShrimpRice->getparry2()) {
            CULog("parry");
            _ShrimpRice->setparry(false);
            _ShrimpRice->setparry2(false);
            _ShrimpRice->setact("SFRStunState1", 0.6);
            _ShrimpRice->parry(*this);
            _ShrimpRice->setdelay(0.5);
        }

        _ShrimpRice->update(dt);
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

    if (flag && !_paused) {
        _paused = true;
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
    if (_timerFillIcon != nullptr) {
        float timePercentage = 1.0f - (_timer / _timeLimit);
        float width = _timerFillIcon->getWidth() * 2;
        float height = _timerFillIcon->getHeight()* 2;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, width, height * timePercentage));
        _timerFillIcon->setScissor(scissor);
    }
    if (_avatar != nullptr) {
        float meterPercentage = _avatar->getMeter() / 100.0f;
        float totalWidth = _cookBarFill->getWidth();
        float height = _cookBarFill->getHeight();
        float clipWidth = totalWidth * meterPercentage;
        std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, clipWidth, height));

        _cookBarIcons["attackready"]->setVisible(false);
        _cookBarIcons["speedready"]->setVisible(false);
        _cookBarIcons["jumpready"]->setVisible(false);
        _cookBarIcons["shieldready"]->setVisible(false);
        _cookBarIcons["healthready"]->setVisible(false);
        _cookBarFill->setVisible(false);
        
        switch (_avatar->getBuffType()) {
            case (buff::attack): {
                _cookBarIcons["attackready"]->setScissor(scissor);
                _cookBarIcons["attackready"]->setVisible(true);
                break;
            }
            case (buff::defense): {
                _cookBarIcons["shieldready"]->setScissor(scissor);
                _cookBarIcons["shieldready"]->setVisible(true);
                break;
			}
            case (buff::speed): {
                _cookBarIcons["speedready"]->setScissor(scissor);
                _cookBarIcons["speedready"]->setVisible(true);
                break;
            }
            case (buff::health): {
                _cookBarIcons["healthready"]->setScissor(scissor);
                _cookBarIcons["healthready"]->setVisible(true);
                break;
            }
            case (buff::jump): {
                _cookBarIcons["jumpready"]->setScissor(scissor);
                _cookBarIcons["jumpready"]->setVisible(true);
                break;
            }
            case (buff::none): {
                _cookBarFill->setScissor(scissor);
                _cookBarFill->setVisible(true);
                break;
            }
            default: {
                _cookBarFill->setScissor(scissor);
                _cookBarFill->setVisible(true);
                break;
            }
        }
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
    else {
        _timer += step;
    }

    if (!_respawnTimes.empty() &&_timer >= _respawnTimes.front()) {
		//setFailure(true);
        respawnEnemies();
        _respawnTimes.pop_front();
	}

	if (_level_model->getFilePath() == "json/intermediate.json") {
		_camera->setZoom(155.0 / 40.0);
	}
    else if (_level_model->getFilePath() == "json/sfrBoss.json") {
        _camera->setZoom(100.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/empanada_level_3_final.json" || _level_model->getFilePath() == "json/empanada level 4.json" || _level_model->getFilePath() == "json/empanada level 5.json") {
        _camera->setZoom(120.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/empanada level 7.json" || _level_model->getFilePath() == "json/empanada level 8.json" || _level_model->getFilePath() == "json/empanada level 6.json") {
        _camera->setZoom(120.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/SFRLevel3.tmj") {
        _camera->setZoom(120.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/test_level_v2_experiment.json") {
        _camera->setZoom(140.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/empanada-platform-level-01.json") {
        _camera->setZoom(200.0 / 40.0);
    }
    else if (_level_model->getFilePath() == "json/bull-boss-level.json" || _level_model->getFilePath() == "json/sfrBoss.json") {
        _camera->setZoom(100.0 / 40.0);
    }
	else if (_level_model->getFilePath() != "") {
		_camera->setZoom(120.0 / 40.0);
	}
	else {
		_camera->setZoom(120.0 / 40.0);
	}

	cugl::Vec3 target = _avatar->getPosition() * _scale + _cameraOffset;
	float invZoom = 1 / _camera->getZoom();
	float cameraWidth = invZoom * (_camera->getViewport().getMaxX() - _camera->getViewport().getMinX()) / 2;
	float cameraHeight = invZoom * (_camera->getViewport().getMaxY() - _camera->getViewport().getMinY()) / 2;

	if ((_level == 1 || _level == 2 || _level == 3 || _level == 4 || _level == 5 || _level == 6 || _level == 7 || _level == 8 || _level == 9 || _level == 10 || _level == 11 || _level == 12) && _background != nullptr) {

		float backgroundWidth = _background->getBoundingRect().getMaxX() - _background->getBoundingRect().getMinX();
		float backgroundHeight = _background->getBoundingRect().getMaxY() - _background->getBoundingRect().getMinY();

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
	_camera->update();

	if (_level == 1 || _level == 2 || _level == 3 || _level == 4 || _level == 5) {

		invZoom = 1 / _minimapCamera->getZoom();
		cameraWidth = invZoom * (_minimapCamera->getViewport().getMaxX() - _minimapCamera->getViewport().getMinX()) / 2;
		cameraHeight = invZoom * (_minimapCamera->getViewport().getMaxY() - _minimapCamera->getViewport().getMinY()) / 2;

		float backgroundWidth = _background->getBoundingRect().getMaxX() - _background->getBoundingRect().getMinX();
		float backgroundHeight = _background->getBoundingRect().getMaxY() - _background->getBoundingRect().getMinY();

		if (backgroundWidth < cameraWidth * 2) {
			_minimapCamera->setZoom(std::min(_minimapCamera->getZoom(), cameraWidth * 2 / backgroundWidth));
			invZoom = 1 / _minimapCamera->getZoom();
			cameraWidth = invZoom * (_minimapCamera->getViewport().getMaxX() - _minimapCamera->getViewport().getMinX()) / 2;
			cameraHeight = invZoom * (_minimapCamera->getViewport().getMaxY() - _minimapCamera->getViewport().getMinY()) / 2;
		}
		if (backgroundHeight < cameraHeight * 2) {
			_minimapCamera->setZoom(std::min(_minimapCamera->getZoom(), cameraHeight * 2 / backgroundHeight));
			invZoom = 1 / _minimapCamera->getZoom();
			cameraWidth = invZoom * (_minimapCamera->getViewport().getMaxX() - _minimapCamera->getViewport().getMinX()) / 2;
			cameraHeight = invZoom * (_minimapCamera->getViewport().getMaxY() - _minimapCamera->getViewport().getMinY()) / 2;
		}

		cugl::Vec3 mapMin = Vec3(_background->getBoundingRect().getMinX() + cameraWidth, _background->getBoundingRect().getMinY() + cameraHeight, 0);
		cugl::Vec3 mapMax = Vec3(_background->getBoundingRect().getMaxX() - cameraWidth, _background->getBoundingRect().getMaxY() - cameraHeight, 0);
		pos.clamp(mapMin, mapMax);
		_minimapCamera->setPosition(pos);
		_minimapCamera->update();
    }
    if (_Bull != nullptr && _Bull->getHealth() <= 0) {
        setComplete(true);
    }

    if (_timer > _timeLimit) {
        setFailure(true);
    }
 
    _inventoryNode->fixedUpdate(step);
    //su
    _avatar->fixedUpdate(step);
    if (_avatar->getDeathTimer() < 0) {
        respawnAvatar();
        _timer += _timeLimit / 10.0f;
    }
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
            if ((*it)->getName() == "ingredient2") {
                (*it)->setstraight(_avatar->getPosition());
            }
            (*it)->fixedUpdate(step);
        }
        if ((*it)->killMe()) {
            if ((*it)->getName() == "ingredient2" || (*it)->getName() == "ingredient2") {
                addIngredientToInventory(ingredientNameToIngredientProperties[(*it)->getSceneNode()->getName()]);
            }
            
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



    int timer = 0;
    int altTimer = 0;
    for (std::shared_ptr<Wall> bPlatform : _level_model->getBreakablePlatforms()) {
        // conditional where we know that our avatar has collided with the wall
        if (bPlatform->isFlagged()) {

            bPlatform->applyBreaking();

            /*while (timer < 10000000) {
                timer++;
            }

            CULog("we are about to change bPlatform's physical properties");
            bPlatform->setSensor(true);
            bPlatform->setReadyToBeReset(true);
            timer = 0;*/
        }
        else {
            /*if (bPlatform->isSensor()) {
                while (altTimer < 1000000) {
                    altTimer++;
                }
                bPlatform->setSensor(false);
                bPlatform->setReadyToBeReset(false);
                altTimer = 0;
            }*/
        }
    }

    CULog("Size of moving platforms array");
    CULog(std::to_string(_level_model->getMovingPlatforms().size()).c_str());
    CULog("------------------");
    CULog("updating path movements of moving platform");
    for (std::shared_ptr<Wall> mPlatform : _level_model->getMovingPlatforms()) {
        mPlatform->applyPathMovement(step);
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
    if (_complete) {
        if (_winScreen->getAdvance()) {
            CULog("advanced??? from complete");
            save();
            advanceLevel();
            reset();
        }
        else if (_winScreen->getReset()) {
            reset();
        }
        else if (_winScreen->didTransition()) {
            transition(true);
            setTarget(_winScreen->getTarget());
            _winScreen->setTarget("");
            _winScreen->setTransition(false);
            _complete = false;
        }
    }
    else if (_failed) {
        //reset();
        if (_loseScreen->getReset()) {
            reset();
        }
        else if (_loseScreen->didTransition()) {
            transition(true);
            setTarget(_loseScreen->getTarget());
            _loseScreen->setTarget("");
            _loseScreen->setTransition(false);
            _failed = false;
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
 
        if (ing != nullptr) {
            _slowed = true;
            //_dollarnode->setTargetGestures({ ing->getGestures(), ing->getGestures() });
            _dollarnode->addIngredientToStation(ing);
            _avatar->useMeter();
            _dollarnode->setVisible(true);
            if (!(_dollarnode->isFocus())) {
                _dollarnode->setFocus(true);
            }
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
        /*std::shared_ptr<Ingredient> ing =*/ 
        _inventoryNode->popIngredientFromSlot(_inventoryNode->getSelectedSlot());
        //i think thats all
    }
}

void GameScene::handleCooktime() {
    //transition in dollar node
    //cooktime handling. 
    if (_dollarnode->isCompleted()) {
        _slowed = false;
        std::string message = "";
        if (_dollarnode->getLastResult() > 0) {
            CULog("Succeeded gesture, awarding buff");
            modifier mod = modifier::duration;
            if (_dollarnode->getClosestGesture() != "") {
                buff reward = gestureToBuff[_dollarnode->getClosestGesture()];
                
                _avatar->applyBuff(reward, mod);
            }
            else {
                CULog("Closest gesture was empty, so no buff");
            }
        }
        else {
            CULog("No gestures matched with passing accuracy");
        }

        CULog("%i", _dollarnode->getLastResult());
        message = _feedbackMessages[_dollarnode->getLastResult()];
        popup(message, cugl::Vec2(_avatar->getPosition().x * _scale, _avatar->getPosition().y * 1.1 * _scale));
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
            if ((*it)->getName() == "background") {
				(*it)->render(batch, Affine2::IDENTITY, _color);
			}
            else (*it)->render(batch, Affine2::IDENTITY, _color);
    //        for (auto& i : _interactables) {
    //            if (i->isRemoved()) {
				//	continue;
				//}
    //            if (i->getName() == "interactable_plate") {
				//	_minimapIconNode->setTexture(_minimapIcons["plate"]);
				//	_minimapIconNode->setPosition(i->getPosition());
				//	_minimapIconNode->render(batch, Affine2::IDENTITY, _color);
    //                CULog("rending");
				//}
    //            else if (i->getName() == "interactable_cut") {
				//	_minimapIconNode->setTexture(_minimapIcons["cut"]);
				//	_minimapIconNode->setPosition(i->getPosition());
				//	_minimapIconNode->render(batch, Affine2::IDENTITY, _color);
				//}
    //            else if (i->getName() == "interactable_pan") {
				//	_minimapIconNode->setTexture(_minimapIcons["pan"]);
				//	_minimapIconNode->setPosition(i->getPosition());
				//	_minimapIconNode->render(batch, Affine2::IDENTITY, _color);
				//}
    //            else if (i->getName() == "interactable_pot") {
				//	_minimapIconNode->setTexture(_minimapIcons["pot"]);
				//	_minimapIconNode->setPosition(i->getPosition());
				//	_minimapIconNode->render(batch, Affine2::IDENTITY, _color);
				//}
    //        }

    //        _minimapIconNode->setTexture(_minimapIcons["su"]);
    //        _minimapIconNode->setPosition(_avatar->getPosition());
    //        _minimapIconNode->render(batch, Affine2::IDENTITY, _color);
    //        
    //        _minimapIconNode->setVisible(false);
            //(*it)->render(batch, Affine2::IDENTITY, _color);
        }

        batch->end();
        _r->end();

        _minimapNode->setTexture(_r->getTexture());
        _minimapNode->flipVertical(true);
    }

    _uiScene->render(batch);
    _pauseMenu->render(batch);
    _loseScreen->render(batch);
    _winScreen->render(batch);
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
        //_winnode->setVisible(true);
        _winScreen->setActive(true);
        //_countdown = EXIT_COUNT;
    }
    else if (!value) {
        _winnode->setVisible(false);
        //_countdown = -1;
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
    if (value) {
        if (!_failed && !_complete) {
            std::shared_ptr<Sound> source = _assets->get<Sound>(LOSE_MUSIC);
            AudioEngine::get()->getMusicQueue()->play(source, false, MUSIC_VOLUME);
            _losenode->setVisible(true);
            //_countdown = EXIT_COUNT;
            _loseScreen->setActive(true);
        }
    }
    else {
        _losenode->setVisible(false);
        //_countdown = -1;
    }
    _failed = value;
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

void GameScene::respawnAvatar() {
    _avatar->reset();
    _avatar->setPosition(_spawnPoint);
    _avatar->setAwake(true);
}



//Basically the same as removeAttack, can refactor
void GameScene::removeEnemy(EnemyModel* enemy) {
    if (enemy->isRemoved()) {
        return;
    }

    //addEnemyToInventory(enemy->getType());
    addingredient(enemy->getPosition(), enemyToIngredientMap[enemy->getType()].name);

    _worldnode->removeChild(enemy->getSceneNode());
    enemy->setDebugScene(nullptr);
    enemy->markRemoved(true);

    std::shared_ptr<Sound> source = _assets->get<Sound>(POP_EFFECT);
    AudioEngine::get()->play(POP_EFFECT, source, false, EFFECT_VOLUME, true);
}

void GameScene::respawnEnemy(std::shared_ptr<EnemyModel> enemy) {
    if (enemy->getType() == EnemyType::rice_soldier) {
        return;
    }
    else {
        CULog(("Respawned " + enemy->getName()).c_str());
        addObstacle(enemy, enemy->getSceneNode());
        enemy->respawn();
    }
}

void GameScene::addEnemyToInventory(EnemyType enemyType) {
    IngredientProperties props = enemyToIngredientMap[enemyType];

    std::shared_ptr<Ingredient> ing = std::make_shared<Ingredient>("", props.gestures, 0.0f);
    ing->setName(props.name);
    std::shared_ptr<Texture> tex = _assets->get<Texture>(ing->getName());
    ing->init(tex);

    _inventoryNode->addIngredient(ing);
}
void GameScene::addIngredientToInventory(IngredientProperties ingProp) {
    std::shared_ptr<Ingredient> ing = std::make_shared<Ingredient>("", ingProp.gestures, 0.0f);
    ing->setName(ingProp.name);
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
    //std::shared_ptr<cugl::scene2::Label> popup = cugl::scene2::Label::allocWithText(pos, s, _assets->get<Font>(MESSAGE_FONT));

    std::shared_ptr<cugl::scene2::Label> popup = scene2::Label::allocWithText(s, _assets->get<Font>(MESSAGE_FONT));
    popup->setAnchor(Vec2::ANCHOR_TOP_CENTER);
    popup->setForeground(Color4::RED);
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
    std::string root = cugl::Application::get()->getSaveDirectory();
    std::string path = cugl::filetool::join_path({ root,"save.json" });
    std::shared_ptr<JsonValue> json = JsonValue::allocObject();
    if (isComplete()) {
        //increment most recently beaten chapter and level
        json->appendValue("chapter",(float)_chapter);
        json->appendValue("level", (float)_level);
        auto writer = JsonWriter::alloc(path);
        writer->writeJson(json);
        writer->close();
    }
 //   std::shared_ptr<JsonValue> player = JsonValue::allocObject();
 //   player->appendValue("location_x", (double) _avatar->getPosition().x);
 //   player->appendValue("location_y", (double) _avatar->getPosition().y);
 //   player->appendValue("health", (double) _avatar->getHealth());

 //   std::vector<std::string> types = { "egg", "carrot", "shrimp", "rice", "beef", "rice_soldier"};
 //   for (auto t = types.begin(); t != types.end(); t++) {
 //       std::string type = *t;
 //       night->appendChild(type, JsonValue::allocObject());
 //   }

 //   for (auto& e : _enemies) {
 //       std::string type = EnemyModel::typeToStr(e->getType());
 //       std::shared_ptr<JsonValue> x = JsonValue::allocObject();
	//	x->appendValue("location_x", (double) e->getPosition().x);
 //       x->appendValue("location_y", (double) e->getPosition().y);
 //       x->appendValue("health", (double) e->getHealth());
 //       x->appendValue("isDead", e->isRemoved());
 //       night->get(type)->appendChild(e->getId(), x);
	//}

 //   //placeholder  values for chapter and level
 //   json->appendValue("chapter", 1.0f);
 //   json->appendValue("level", 1.0f);
 //   json->appendValue("startFromNight", true);
 //   //json->appendChild("persistent", persistent);
 //   json->appendChild("night", night);
}

bool GameScene::loadSave(std::shared_ptr<JsonValue> save) {
    if (save == nullptr || save->size() == 0) {
        int chap = 1;
        int level = 1;
        reset();
        return true;
    }
    
    int chap = save->getInt("chapter");
    int level = save->getInt("level");
    changeCurrentLevel(chap, level);
    reset();

    //float locationX = save->get("player")->getFloat("location_x");
    //float locationY = save->get("player")->getFloat("location_y");
    //float health = save->get("player")->getFloat("health");
    //_avatar->setPosition(locationX, locationY);
    //_avatar->setHealth(health);
    //for (auto& e : _enemies) {
    //    std::shared_ptr<JsonValue> enemy = save->get(EnemyModel::typeToStr(e->getType()))->get(e->getId());
    //    if (enemy->getBool("isDead", false)) {
    //        removeEnemy(e.get());
    //        continue;
    //    }
    //    e->setPosition(enemy->getFloat("location_x"), enemy->getFloat("location_y"));
    //    e->setHealth(enemy->getFloat("health"));
    //}

    return true;
}

//load level with int specifiers
void GameScene::loadLevel(int chapter, int level) {
    changeCurrentLevel(chapter, level);
    AudioEngine::get()->clear();
    if (level == 4) {
        std::shared_ptr<Sound> source = _assets->get<Sound>("theBull");
        AudioEngine::get()->play("theBull", source, true, EFFECT_VOLUME, true);
    }
    if (level == 5) {
        std::shared_ptr<Sound> source = _assets->get<Sound>("theShrimp");
        AudioEngine::get()->play("theShrimp", source, true, EFFECT_VOLUME, true);
    }
    loadLevel(currentLevel);
}

void GameScene::advanceLevel() {
    _level += 1;
    _level = _level % 18;
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
        //if (level == 1) {
        //    //_level_model->setFilePath("json/intermediate.json");
        //    _level_model->setFilePath("json/TestLevel3.tmj");
        //}
        //else if (level == 2) {
        //    _level_model->setFilePath("json/test_level_v2_experiment.json");
        //}
        //else if (level == 3) {
        //    _level_model->setFilePath("json/empanada-platform-level-01.json");
        //}
        //else if (level == 4) {
        //    //currentLevel = level2;
        //    _level_model->setFilePath("json/bull-boss-level.json"); 
        //}
        //else if (level == 5) {
        //    currentLevel = level3;
        //}

        if (level == 1) {
            _level_model->setFilePath("json/johntutorial_colored.tmj");
        }
        else if (level == 2) {
            _level_model->setFilePath("json/johntutorial_raw2.tmj");
        }
        else if (level == 3) {
            _level_model->setFilePath("json/empanada_level_3_final.json");
        }
        else if (level == 4) {
            _level_model->setFilePath("json/empanada level 4.json");
        }
        else if (level == 5) {
            _level_model->setFilePath("json/empanada level 5.json");
        }
        else if (level == 6) {
            _level_model->setFilePath("json/empanada level 6.json");
        }
        else if (level == 7) {
            // _level_model->setFilePath("json/intermediate.json");
            // _level_model->setFilePath("json/sfrBoss.json");
            // _level_model->setFilePath("json/empanada_level_3_final.json");
            _level_model->setFilePath("json/empanada level 7.json");
            // _level_model->setFilePath("json/TestLevel1.json");
        }
        else if (level == 8) {
            _level_model->setFilePath("json/empanada level 8.json");
        }
        else if (level == 9) {
            _level_model->setFilePath("json/empanada level 9.tmj");
        }
        else if (level == 10) {
            _level_model->setFilePath("json/empanada level 11.tmj");
        }
        else if (level == 11) {
            _level_model->setFilePath("json/empanada level 12.tmj");
        }
        else if (level == 12) {
            _level_model->setFilePath("json/bull-boss-level.json");
        }
        else if (level == 13) {
            _level_model->setFilePath("json/SFRLevel3.tmj");
        }
        else if (level == 14) {
            _level_model->setFilePath("json/intermediate.json");
        }
        else if (level == 15) {
            _level_model->setFilePath("json/test_level_v2_experiment.json");
		}
        else if (level == 16) {
            _level_model->setFilePath("json/empanada-platform-level-01.json");
        }
        else if (level == 17) {
            _level_model->setFilePath("json/sfrBoss.json");
        }
        else if (level == 18) {
            // currentLevel = level3;
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
    Size s = cugl::Size(6.5f, 6.0f);
    std::shared_ptr<EnemyModel> new_enemy = Beef::allocWithConstants(pos, s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}
void GameScene::spawnEgg(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("eggIdle");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
    Size s = Size(1.75f, 5.0f);
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
    Size s = Size(2.0f, 2.5f);
    std::shared_ptr<EnemyModel> new_enemy = Rice::allocWithConstants(pos, s, getScale(), _assets, isSoldier);
    spritenode->setAnchor(Vec2(0.5, 0.35));
    spritenode->setScale(0.35 / 1.75);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    _enemies.push_back(new_enemy);
}
std::shared_ptr<EnemyModel> GameScene::spawnRiceSoldier(Vec2 pos, std::shared_ptr<Rice> leader) {
    CULog("SPAWNING SOLDIER");
    std::shared_ptr<Texture> image = _assets->get<Texture>("riceLeader");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);

    Size s = Size(2.0f, 2.5f);
    std::shared_ptr<EnemyModel> new_enemy = Rice::allocWithConstants(pos, s, getScale(), _assets, true);
    spritenode->setAnchor(Vec2(0.5, 0.35));
    spritenode->setScale(0.35 / 1.75);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    addObstacle(new_enemy, spritenode);
    //DONT PUSH BACK INTO _enemies, just add to leader
    //_enemies.push_back(new_enemy);

    leader->addSoldier(std::static_pointer_cast<Rice>(new_enemy));
    return new_enemy;
}

void GameScene::spawnCarrot(Vec2 pos) {
    std::shared_ptr<Texture> image = _assets->get<Texture>("carrotEnemy");
    std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 1, 1, 1);
    Size s = Size(2.25f, 3.0f);
    std::shared_ptr<EnemyModel> new_enemy = Carrot::allocWithConstants(pos, s, getScale(), _assets);
    new_enemy->setSceneNode(spritenode);
    new_enemy->setDebugColor(DEBUG_COLOR);
    spritenode->setScale(0.2);
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
    CULog("%f", station->getSceneNode()->getScale());
}

void GameScene::spawnTutorialSign(Vec2 pos, std::string name) {
    //obstacle has small size, not reflective of intended size
    Size s = Size(4.0f, 2.0f);
    std::shared_ptr<Texture> image;

    image = _assets->get<Texture>("sign");

    CULog(name.c_str());


    std::shared_ptr<Scene2Loader> loader = Scene2Loader::alloc();
    auto reader = JsonReader::alloc("./json/Tutorials/" + name + ".json");
    std::shared_ptr<JsonValue> popupData = reader->readJson()->get(name);
    std::shared_ptr<Popup> p = Popup::allocWithData(_assets, _actionManager, loader.get(), popupData);
    p->setActive(false);
    p->setVisible(false);
    _interactivePopups.push_back(p);
    _uiScene->addChild(p);
    reader->close();
    reader = nullptr;
    loader->dispose();
    loader = nullptr;

    std::shared_ptr<TutorialSign> station = TutorialSign::alloc(image, pos, s, name);
    station->setPopup(p);

    CULog("position: %f %f", pos.x, pos.y);

    addObstacle(station, station->getSceneNode());
    _TutorialSigns.push_back(station);
}

void GameScene::spawnPlate(Vec2 pos, std::unordered_map<IngredientType, int> map) {
    //obstacle has small size, not reflective of intended size
    Size s = Size(5.0f, 5.0f);
    CULog("map details");
    for (auto& [key, value] : map) {
        CULog("%s %i", Ingredient::getIngredientStringFromType(key), value);
    }
    std::shared_ptr<Texture> image = _assets->get<Texture>("plate");

    std::shared_ptr<Plate> plate = Plate::alloc(image, pos, s, map);


    for (const auto& [key, value] : map) {
        _pendingAcrossAllPlates[key] += value;
    }

    Color4 c;

    if (_plates.size() == 0) {
        c = Color4::WHITE;
    }
    else if (_plates.size() == 1) {
        c = Color4::BLUE;
    }
    else if (_plates.size() == 2) {
        c = Color4::RED;
    }
    else if (_plates.size() == 3) {
        c = Color4::BLACK;
    }
    else if (_plates.size() == 4) {
        c = Color4::CYAN;
    }
    plate->getSceneNode()->setColor(c);

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

void GameScene::createOrder(int plateId, IngredientType ing) {
    std::shared_ptr<scene2::SceneNode> order = scene2::SceneNode::alloc();
    std::shared_ptr<scene2::PolygonNode> background = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("orderBackground"));
    order->addChild(background);
    std::shared_ptr<Texture> texture;
    CULog("type: %s", Ingredient::getIngredientStringFromType(ing));
    switch (ing) {
    case IngredientType::cutCarrot: {
        texture = _assets->get<Texture>("cutCarrotOrder");
        break;
    }

    case IngredientType::boiledEgg: {
        texture = _assets->get<Texture>("boilEggOrder");
        break;
    }
    case IngredientType::scrambledEgg: {
        texture = _assets->get<Texture>("fryEggOrder");
        break;
    }
    case IngredientType::cookedShrimp: {
        texture = _assets->get<Texture>("fryShrimpOrder");
        break;
    }
    case IngredientType::boiledRice: {
        texture = _assets->get<Texture>("boilRiceOrder");
        break;
    }
    case IngredientType::cookedBeef: {
        texture = _assets->get<Texture>("fryBeefOrder");
        break;
    }
    default: {
        texture = nullptr;
        CULog("Order error");
    }
    }
    std::shared_ptr<scene2::PolygonNode> text = scene2::PolygonNode::allocWithTexture(texture);
    float scale = ORDER_HEIGHT / texture->getHeight();
    text->setPosition(0, 0);
    text->setScale(scale);
    background->setPosition(0,0);
    background->setScale(scale);
    order->addChild(text);
    order->setContentHeight(ORDER_HEIGHT);
    order->setName(Ingredient::getIngredientStringFromType(ing) + "Order");

    _ordersObj->addOrder(plateId, order);
    //_orders[plateId].push_back(order);
    //_orderNode->addChild(_orders[plateId].back());
    _numOrders += 1;
    positionOrders();
}

void GameScene::removeOrder(int plateId, IngredientType ing) {
    /*for (auto it = _orders[plateId].begin(); it != _orders[plateId].end(); it++) {
        if ((*it) != nullptr && (*it)->getName() == (Ingredient::getIngredientStringFromType(ing) + "Order")) {
            std::shared_ptr<scene2::SceneNode> order = *it;
            _orderNode->removeChild(order);
            _orders[plateId].erase(it);
            order->dispose();
            _numOrders -= 1;
            positionOrders();
            
            return;
		}
	}*/
    _ordersObj->removeOrder(plateId, ing);
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
 //   CULog("positioning");
 //   float totalHeight = ORDER_HEIGHT * _numOrders;
 //   float start = 0;
 //   for (auto& t : _orders) {
	//	int i = 0;
 //       for (auto& b : t.second) {
	//		b->setPositionY(start);
 //           start += ORDER_HEIGHT;
	//	}
	//}
 //   //_orderNode->setContentHeight(totalHeight);
 //   _orderNode->setAnchor(Vec2::ANCHOR_TOP_CENTER);
 //   _orderNode->setPosition(1280 / 2, 800 - 50);

    //std::shared_ptr<scene2::Button> lButton = scene2::Button::alloc();
    //std::shared_ptr<scene2::Button> rButton = scene2::Button::alloc();

}

void GameScene::generateOrders() {
    _numOrders = 0;
    _orderNode->removeAllChildren();
    for (auto& p : _plates) {
        std::vector<IngredientType> x = p->getTargetIngredients();
        int id = p->getId();
        _orders[id] = std::vector<std::shared_ptr<scene2::SceneNode>>();
        for (IngredientType t : x) {
            createOrder(id, t);
        }
    }
}

void GameScene::respawnEnemies(float p) {
    CULog("RESPAWNING!");
    for (auto& e : _enemies) {
        if (e->isRemoved()) {
            respawnEnemy(e);
        }
    }
}

void GameScene::removeingredient(Vec2 pos, std::string textureName) {

    std::shared_ptr<Texture> image = _assets->get<Texture>(textureName);

    std::shared_ptr<Attack> attack = Attack::alloc(_avatar->getPosition(),
        cugl::Size(image->getSize().width*0.15/ _scale, image->getSize().height*0.15/ _scale));


    attack->setName("ingredient");
    attack->setBullet(true);
    attack->setGravityScale(0.5);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(true);
    attack->setstraight(pos);
    attack->setrand(false);
    attack->setDie(false);
    attack->setUp(1.5);


    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(_avatar->getPosition());
    sprite->setScale(0.15);

    addObstacle(attack, sprite, true);
    _attacks.push_back(attack);
}

void GameScene::addingredient(Vec2 pos, std::string textureName) {

    std::shared_ptr<Texture> image = _assets->get<Texture>(textureName);

    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(image->getSize().width * 0.15 / _scale, image->getSize().height * 0.15 / _scale));


    attack->setName("ingredient2");
    attack->setBullet(true);
    attack->setGravityScale(0.5);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(_scale);
    attack->setEnabled(true);
    attack->setstraight(_avatar->getPosition());
    attack->setrand(false);
    attack->setDie(false);
    attack->setUp(1.5);
    attack->setFollow(true);


    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setPosition(pos);
    sprite->setScale(0.15);
    sprite->setName(textureName);

    addObstacle(attack, sprite, true);
    _attacks.push_back(attack);
}