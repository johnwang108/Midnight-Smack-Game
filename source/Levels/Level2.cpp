#include "Level2.h"
#include "../PFGameScene.h"

/** The goal door position */
static float GOAL_POS[] = { 4.0f,14.0f };
// float GOAL_POS[] = { 6.0f, 5.0f };

/** The initial position of the dude */
static float DUDE_POS[] = { 2.5f, 5.0f };

static float SHRIMP_POS[] = { 22.0f, 16.0f };

static float EGG_POS[] = { 14.0f, 18.0f };

static float RICE_POS[] = { 25.0f, 14.0f };

static float BACKGROUND_POS[] = { 16.0f, 10.0f };

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */

static float WALL[WALL_COUNT][WALL_VERTS] = {
	{16.0f, 20.0f,  0.0f, 20.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  1.0f, 19.5f, 16.0f, 19.5f },
	{32.0f, 20.0f, 16.0f, 20.0f, 16.0f, 19.5f,
	 31.0f, 19.5f, 31.0f,  0.0f, 32.0f,  0.0f }
};



/** The outlines of all of the platforms */

float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
	{ 1.0f, 3.0f, 1.0f, 2.5f, 6.0f, 2.5f, 6.0f, 3.0f},
	{ 6.0f, 4.0f, 6.0f, 2.5f, 9.0f, 2.5f, 9.0f, 4.0f},
	{23.0f, 4.0f,23.0f, 2.5f,31.0f, 2.5f,31.0f, 4.0f},
	{26.0f, 5.5f,26.0f, 5.0f,28.0f, 5.0f,28.0f, 5.5f},
	{29.0f, 7.0f,29.0f, 6.5f,31.0f, 6.5f,31.0f, 7.0f},
	{24.0f, 8.5f,24.0f, 8.0f,27.0f, 8.0f,27.0f, 8.5f},
	{29.0f,10.0f,29.0f, 9.5f,31.0f, 9.5f,31.0f,10.0f},
	{23.0f,11.5f,23.0f,11.0f,27.0f,11.0f,27.0f,11.5f},
	{19.0f,12.5f,19.0f,12.0f,23.0f,12.0f,23.0f,12.5f},
	{ 1.0f,12.5f, 1.0f,12.0f, 7.0f,12.0f, 7.0f,12.5f}
};




/** The outlines of all of the platforms */
static float ALT_PLATFORMS[ALT_PLATFORM_COUNT][ALT_PLATFORM_VERTS] = {
	{ 1.0f, .5f, 1.0f, .0f, 33.0f, .0f, 33.0f, .50f}
};

void Level2::populate(GameScene& scene) {
	_assets = scene.getAssets();
	_scale = scene.getScale();
	_background = scene.getBackground();
	_avatar = scene.getAvatar();
	_enemies = scene.getEnemies();
	_goalDoor = scene.getGoalDoor();

# pragma mark: Background
	Vec2 background_pos = BACKGROUND_POS;
	std::shared_ptr<Texture> image = _assets->get<Texture>("background-1");
	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
	Size background_size(image->getSize().width / _scale, image->getSize().height / _scale);
	_background = physics2::BoxObstacle::alloc(background_pos, background_size);
	_background->setName(BACKGROUND_NAME);
	_background->setBodyType(b2_staticBody);
	_background->setDensity(0.0f);
	_background->setFriction(0.0f);
	_background->setRestitution(0.0f);
	_background->setEnabled(false);
	_background->setSensor(true);
	scene.addObstacle(_background, sprite);


#pragma mark : Goal door
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
	// _goalDoor->setEnabled

	// Add the scene graph nodes to this object
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_goalDoor->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_goalDoor, sprite);

#pragma mark : Walls
	// All walls and platforms share the same texture
	image = _assets->get<Texture>(EARTH_TEXTURE);
	for (int ii = 0; ii < WALL_COUNT; ii++) {
		std::shared_ptr<physics2::PolygonObstacle> wallobj;

		Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
		// Call this on a polygon to get a solid shape
		EarclipTriangulator triangulator;
		triangulator.set(wall.vertices);
		triangulator.calculate();
		wall.setIndices(triangulator.getTriangulation());
		triangulator.clear();

		wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
		// You cannot add constant "".  Must stringify
		wallobj->setName(WALL_NAME);

		// Set the physics attributes
		wallobj->setBodyType(b2_staticBody);
		wallobj->setDensity(BASIC_DENSITY);
		wallobj->setFriction(BASIC_FRICTION);
		wallobj->setRestitution(BASIC_RESTITUTION);
		wallobj->setDebugColor(DEBUG_COLOR);

		wall *= _scale;
		sprite = scene2::PolygonNode::allocWithTexture(image, wall);
		scene.addObstacle(wallobj, sprite, 1);  // All walls share the same texture
	}

#pragma mark : Platforms
	for (int ii = 0; ii < ALT_PLATFORM_COUNT; ii++) {
		std::shared_ptr<physics2::PolygonObstacle> platobj;
		Poly2 platform(reinterpret_cast<Vec2*>(ALT_PLATFORMS[ii]), sizeof(ALT_PLATFORMS[ii]) / sizeof(float) / 2);

		EarclipTriangulator triangulator;
		triangulator.set(platform.vertices);
		triangulator.calculate();
		platform.setIndices(triangulator.getTriangulation());
		triangulator.clear();

		platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_CENTER);
		// You cannot add constant "".  Must stringify
		platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));

		// Set the physics attributes
		platobj->setBodyType(b2_staticBody);
		platobj->setDensity(BASIC_DENSITY);
		platobj->setFriction(BASIC_FRICTION);
		platobj->setRestitution(BASIC_RESTITUTION);
		platobj->setDebugColor(DEBUG_COLOR);

		platform *= _scale;
		sprite = scene2::PolygonNode::allocWithTexture(image, platform);
		scene.addObstacle(platobj, sprite, 1);
	}

#pragma mark : Dude
	Vec2 dudePos = DUDE_POS;
	// node = scene2::SceneNode::alloc();
	image = _assets->get<Texture>(DUDE_TEXTURE);
	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_avatar->setSceneNode(sprite);
	_avatar->setDebugColor(DEBUG_COLOR);
	_avatar->setName(DUDE_TEXTURE);
	scene.addObstacle(_avatar, sprite); // Put this at the very front

	// Play the background music on a loop.
	std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
	AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);


	Vec2 shrimp_pos = SHRIMP_POS;
	image = _assets->get<Texture>(BULL_TEXTURE);
	std::shared_ptr<BullModel> _bull = BullModel::alloc(shrimp_pos, image->getSize() / _scale, _scale);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_bull->setSceneNode(sprite);
	_bull->setName(BULL_TEXTURE);
	_bull->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_bull, sprite);
	/*
	Vec2 egg_pos = EGG_POS;
	image = _assets->get<Texture>(EGG_TEXTURE);
	std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc(egg_pos, image->getSize() / (_scale), _scale, EnemyType::egg);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);
	*/

	scene.setAssets(_assets);
	scene.setScale(_scale);
	scene.setBackground(_background);
	scene.setAvatar(_avatar);
	scene.setEnemies(_enemies);
	scene.setGoalDoor(_goalDoor);
	scene.setBull(_bull);

}