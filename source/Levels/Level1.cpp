#include "Level1.h"
#include "../PFGameScene.h"

/** The goal door position */
static float GOAL_POS[] = { 44.0f,30.0f };
// float GOAL_POS[] = { 6.0f, 5.0f };
/** The position of the spinning barrier */
static float SPIN_POS[] = { 13.0f,12.5f };
/** The initial position of the dude */
static float DUDE_POS[] = { 2.5f, 5.0f };
/** The position of the rope bridge */
static float BRIDGE_POS[] = { 9.0f, 3.8f };

static float SHRIMP_POS[] = { 22.0f, 6.0f };

static float EGG_POS[] = { 14.0f, 6.0f };

static float RICE_POS[] = { 25.0f, 6.0f };

static float BACKGROUND_POS[] = { 16.0f, 10.0f };

std::vector<std::shared_ptr<Wall>> things;


//static float WALL[WALL_COUNT][WALL_VERTS] = {
//	{16.0f, 20.0f,  0.0f, 20.0f,  0.0f,  0.0f,
//	  1.0f,  0.0f,  1.0f, 19.5f, 16.0f, 19.5f },
//	{32.0f, 20.0f, 16.0f, 20.0f, 16.0f, 19.5f,
//	 31.0f, 19.5f, 31.0f,  0.0f, 32.0f,  0.0f }
//};

static float WALL[WALL_COUNT][WALL_VERTS] = {
	{27.5f, 36.0f,  0.0f, 36.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  1.0f, 35.5f, 27.5f, 35.5f },
	{55.0f, 36.0f, 27.5f, 36.0f, 27.5f, 35.5f,
	 54.0f, 35.5f, 54.0f,  0.0f, 55.0f,  0.0f }
};


/** The outlines of all of the platforms */

//float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
//	{ 1.0f, 3.0f, 1.0f, 2.5f, 6.0f, 2.5f, 6.0f, 3.0f},
//	{ 6.0f, 4.0f, 6.0f, 2.5f, 9.0f, 2.5f, 9.0f, 4.0f},
//	{23.0f, 4.0f,23.0f, 2.5f,31.0f, 2.5f,31.0f, 4.0f},
//	{26.0f, 5.5f,26.0f, 5.0f,28.0f, 5.0f,28.0f, 5.5f},
//	{29.0f, 7.0f,29.0f, 6.5f,31.0f, 6.5f,31.0f, 7.0f},
//	{24.0f, 8.5f,24.0f, 8.0f,27.0f, 8.0f,27.0f, 8.5f},
//	{29.0f,10.0f,29.0f, 9.5f,31.0f, 9.5f,31.0f,10.0f},
//	{23.0f,11.5f,23.0f,11.0f,27.0f,11.0f,27.0f,11.5f},
//	{19.0f,12.5f,19.0f,12.0f,23.0f,12.0f,23.0f,12.5f},
//	{ 1.0f,12.5f, 1.0f,12.0f, 7.0f,12.0f, 7.0f,12.5f}
//};

//PLATFORM_COUNT
static float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
	// {1.0f, 4.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 4.0f},
	{1.0f, 4.0f, 1.0f, 0.0f, 55.0f, 0.0f, 55.0f, 4.0f},
	{42.5f, 8.0f, 42.5f, 7.0f, 47.5f, 7.0f, 47.5f, 8.0f},
	{42.5f, 15.0f, 42.5f, 14.0f, 47.5f, 14.0f, 47.5f, 15.0f},
	{30.0f, 11.0f, 30.0f, 10.5f, 40.0f, 10.5f, 40.0f, 11.0f},
	{15.0f, 15.0f, 15.0f, 14.5f, 29.0f, 14.5f, 29.0f, 15.0f},
	{13.0f, 20.0f, 13.0f, 19.0f, 20.0f, 19.0f, 20.0f, 20.0f},
	{22.0f, 25.0f, 22.0f, 24.0f, 25.0f, 24.0f, 25.0f, 25.0f},
	{25.0f, 27.0f, 25.0f, 26.0f, 30.0f, 26.0f, 30.0f, 27.0f},
	{30.0f, 28.0f, 30.0f, 27.5f, 40.0f, 27.5f, 40.0f, 28.0f},
	{42.0f, 28.0f, 42.0f, 27.5f, 46.0f, 27.5f, 46.0f, 28.0f}
};



/** The outlines of all of the platforms */
static float ALT_PLATFORMS[ALT_PLATFORM_COUNT][ALT_PLATFORM_VERTS] = {
	{ 1.0f, .5f, 1.0f, .0f, 33.0f, .0f, 33.0f, .50f}
};


void Level1::populate(GameScene& scene) {
	_assets = scene.getAssets();
	_scale = scene.getScale();
	_background = scene.getBackground();
	_avatar = scene.getAvatar();
	_enemies = scene.getEnemies();
	_goalDoor = scene.getGoalDoor();


#pragma mark : Goal door
	std::shared_ptr<Texture> image = _assets->get<Texture>("background-1");
	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
	image = _assets->get<Texture>(GOAL_TEXTURE);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	std::shared_ptr<scene2::WireNode> draw;

	// Create obstacle
	Vec2 goalPos = GOAL_POS;
	Size goalSize(image->getSize().width / _scale,
		image->getSize().height / _scale);
	_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);

	// Set the physics attributes
	_goalDoor->setBodyType(b2_staticBody);
	_goalDoor->setDensity(0.0f);
	_goalDoor->setFriction(0.0f);
	_goalDoor->setRestitution(0.0f);
	_goalDoor->setSensor(true);
	// _goalDoor->setEnabledf

	// Add the scene graph nodes to this object
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_goalDoor->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_goalDoor, sprite);

#pragma mark : Walls
	// All walls and platforms share the same texture
	image = _assets->get<Texture>(EARTH_TEXTURE);
	std::string wname = "wall";
	for (int ii = 0; ii < WALL_COUNT; ii++) {
		std::shared_ptr<Wall> wallobj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, DEBUG_COLOR,
			reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS, wname);
		things.push_back(wallobj);
		scene.addObstacle(wallobj->getObj(), wallobj->getSprite(), 1);  // All walls share the same texture
	}

#pragma mark : Platforms
	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
		std::shared_ptr<Wall> platObj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, Color4::BLUE,
			reinterpret_cast<Vec2*>(PLATFORMS[ii]), PLATFORM_VERTS, std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));
		std::vector<Vec3> path;
		path = { Vec3(100,100,0), Vec3(200,200,0), Vec3(300,300,0), Vec3(400,400,0) };
		if (ii % 2 == 0) {
			platObj->initiatePath(path,2);
		}
		things.push_back(platObj);
		scene.addObstacle(platObj->getObj(), platObj->getSprite(), 1);
	}

#pragma mark : Dude
	Vec2 dudePos = DUDE_POS;
	// node = scene2::SceneNode::alloc();
	image = _assets->get<Texture>(DUDE_TEXTURE);
	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_avatar->setSceneNode(sprite);
	_avatar->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_avatar, sprite); // Put this at the very front

	// Play the background music on a loop.
	std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
	AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);


	Vec2 shrimp_pos = SHRIMP_POS;
	image = _assets->get<Texture>(SHRIMP_TEXTURE);
	std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc(shrimp_pos, image->getSize() / _scale, _scale, EnemyType::shrimp);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("pigtail");

	//shrimp 2
	image = _assets->get<Texture>(SHRIMP_TEXTURE);
	_enemy = EnemyModel::alloc({30.0f, 6.0f}, image->getSize() / _scale, _scale, EnemyType::shrimp);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("pigtail");



	//shrimp 3
	image = _assets->get<Texture>(SHRIMP_TEXTURE);
	_enemy = EnemyModel::alloc({25.0f, 18.0f}, image->getSize() / _scale, _scale, EnemyType::shrimp);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("pigtail");

	Vec2 rice_pos = RICE_POS;
	image = _assets->get<Texture>(RICE_TEXTURE);
	_enemy = EnemyModel::alloc(rice_pos, image->getSize() / _scale, _scale, EnemyType::rice);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("circle");

	//rice 2
	image = _assets->get<Texture>(RICE_TEXTURE);
	_enemy = EnemyModel::alloc({ 27.0f, 28.0f }, image->getSize() / _scale, _scale, EnemyType::rice);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("circle");

	//rice 3
	image = _assets->get<Texture>(RICE_TEXTURE);
	_enemy = EnemyModel::alloc({ 35.0f, 30.0f }, image->getSize() / _scale, _scale, EnemyType::rice);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("circle");


	Vec2 egg_pos = EGG_POS;
	image = _assets->get<Texture>(EGG_TEXTURE);
	_enemy = EnemyModel::alloc(egg_pos, image->getSize() / (_scale), _scale, EnemyType::egg);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("v");

	//egg 2
	image = _assets->get<Texture>(EGG_TEXTURE);
	_enemy = EnemyModel::alloc({25.0f, 28.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("v");

	//egg 3

	image = _assets->get<Texture>(EGG_TEXTURE);
	_enemy = EnemyModel::alloc({30.0f, 30.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	_enemy->setGestureString("v");


	scene.setAssets(_assets);
	scene.setScale(_scale);
	scene.setBackground(_background);
	scene.setAvatar(_avatar);
	scene.setEnemies(_enemies);
	scene.setGoalDoor(_goalDoor);

}
void Level1::update(float step) {
	//CULog("cry");
	for (const auto& obj : things) {
		if (obj->queryActivePath()) {
			obj->update(step);
		}
	}
}