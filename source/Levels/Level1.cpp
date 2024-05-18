#include "Level1.h"
#include "../PFGameScene.h"

///** The goal door position */
//static float GOAL_POS[] = { 44.0f,30.0f };
//// float GOAL_POS[] = { 6.0f, 5.0f };
///** The position of the spinning barrier */
//static float SPIN_POS[] = { 13.0f,12.5f };
///** The initial position of the dude */
//static float DUDE_POS[] = { 2.5f, 5.0f };
///** The position of the rope bridge */
//static float BRIDGE_POS[] = { 9.0f, 3.8f };
//
//static float SHRIMP_POS[] = { 22.0f, 6.0f };
//
//static float EGG_POS[] = { 14.0f, 6.0f };
//
//static float RICE_POS[] = { 25.0f, 6.0f };
//
//static float BACKGROUND_POS[] = { 16.0f, 10.0f };
//
//std::vector<std::shared_ptr<Wall>> things;


//static float WALL[WALL_COUNT][WALL_VERTS] = {
//	{16.0f, 20.0f,  0.0f, 20.0f,  0.0f,  0.0f,
//	  1.0f,  0.0f,  1.0f, 19.5f, 16.0f, 19.5f },
//	{32.0f, 20.0f, 16.0f, 20.0f, 16.0f, 19.5f,
//	 31.0f, 19.5f, 31.0f,  0.0f, 32.0f,  0.0f }
//};

//static float WALL[WALL_COUNT][WALL_VERTS] = {
//	{27.5f, 36.0f,  0.0f, 36.0f,  0.0f,  0.0f,
//	  1.0f,  0.0f,  1.0f, 35.5f, 27.5f, 35.5f },
//	{55.0f, 36.0f, 27.5f, 36.0f, 27.5f, 35.5f,
//	 54.0f, 35.5f, 54.0f,  0.0f, 55.0f,  0.0f }
//};


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
//static float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
//	// {1.0f, 4.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 4.0f},
//	{1.0f, 4.0f, 1.0f, 0.0f, 55.0f, 0.0f, 55.0f, 4.0f},
//	{42.5f, 8.0f, 42.5f, 7.0f, 47.5f, 7.0f, 47.5f, 8.0f},
//	{42.5f, 15.0f, 42.5f, 14.0f, 47.5f, 14.0f, 47.5f, 15.0f},
//	{30.0f, 11.0f, 30.0f, 10.5f, 40.0f, 10.5f, 40.0f, 11.0f},
//	{15.0f, 15.0f, 15.0f, 14.5f, 29.0f, 14.5f, 29.0f, 15.0f},
//	{13.0f, 20.0f, 13.0f, 19.0f, 20.0f, 19.0f, 20.0f, 20.0f},
//	{22.0f, 25.0f, 22.0f, 24.0f, 25.0f, 24.0f, 25.0f, 25.0f},
//	{25.0f, 27.0f, 25.0f, 26.0f, 30.0f, 26.0f, 30.0f, 27.0f},
//	{30.0f, 28.0f, 30.0f, 27.5f, 40.0f, 27.5f, 40.0f, 28.0f},
//	{42.0f, 28.0f, 42.0f, 27.5f, 46.0f, 27.5f, 46.0f, 28.0f}
//};



/** The outlines of all of the platforms */
//static float ALT_PLATFORMS[ALT_PLATFORM_COUNT][ALT_PLATFORM_VERTS] = {
//	{ 1.0f, .5f, 1.0f, .0f, 33.0f, .0f, 33.0f, .50f}
//};


void Level1::populate(GameScene& scene) {
//	_assets = scene.getAssets();
//	_scale = scene.getScale();
//	_background = scene.getBackground();
//	_avatar = scene.getAvatar();
//	_enemies = scene.getEnemies();
//	_goalDoor = scene.getGoalDoor();
//
//
//#pragma mark : Goal door
//	std::shared_ptr<Texture> image = _assets->get<Texture>("background-1");
//	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
//	image = _assets->get<Texture>(GOAL_TEXTURE);
//	sprite = scene2::PolygonNode::allocWithTexture(image);
//	std::shared_ptr<scene2::WireNode> draw;
//
//	// Create obstacle
//	Vec2 goalPos = GOAL_POS;
//	Size goalSize(image->getSize().width / _scale,
//		image->getSize().height / _scale);
//	_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);
//
//	// Set the physics attributes
//	_goalDoor->setBodyType(b2_staticBody);
//	_goalDoor->setDensity(0.0f);
//	_goalDoor->setFriction(0.0f);
//	_goalDoor->setRestitution(0.0f);
//	_goalDoor->setSensor(true);
//	// _goalDoor->setEnabledf
//
//	// Add the scene graph nodes to this object
//	sprite = scene2::PolygonNode::allocWithTexture(image);
//	_goalDoor->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_goalDoor, sprite);
//
//#pragma mark : Walls
//	// All walls and platforms share the same texture
//	image = _assets->get<Texture>(EARTH_TEXTURE);
//	std::string wname = "wall";
//	for (int ii = 0; ii < WALL_COUNT; ii++) {
//		std::shared_ptr<Wall> wallobj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, DEBUG_COLOR,
//			reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS, wname);
//		things.push_back(wallobj);
//		scene.addObstacle(wallobj->getObj(), wallobj->getSprite(), 1);  // All walls share the same texture
//	}
//
//#pragma mark : Platforms
//	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
//		std::shared_ptr<Wall> platObj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, Color4::BLUE,
//			reinterpret_cast<Vec2*>(PLATFORMS[ii]), PLATFORM_VERTS, std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));
//		std::vector<Vec3> path;
//		path = { Vec3(0,100,120), Vec3(100,100,120), Vec3(100,0,120), Vec3(0,-30,120) };
//		if (ii % 3 == 1) {
//			//platObj->initiatePath(path, 2.0f);
//			
//		}
//		things.push_back(platObj);
//		scene.addObstacle(platObj->getObj(), platObj->getSprite(), 1);
//	}
//
//#pragma mark : Dude
//	Vec2 dudePos = DUDE_POS;
//	// node = scene2::SceneNode::alloc();
//
//	image = _assets->get<Texture>("su_idle");
//	//hardcoded size
//	cugl::Size s = PLAYER_SIZE_DEFAULT;
//	_avatar = DudeModel::allocWithConstants(dudePos, s, _scale, _assets);
//	std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4,16);
//
//	//CALCULATE sue sprite size from sue obstacle size. Goal: su's feet line up with foot sensor, and head (not hat) with top of obstacle. Todo still
//	//float scalar = (s.width *_scale) / spritenode->getSize().width;
//	spritenode->setAnchor(Vec2(0.5, 0.35));
//	spritenode->setPosition(dudePos);
//	_avatar->setSceneNode(spritenode);
//	_avatar->setDebugColor(DEBUG_COLOR);
//
//	scene.addObstacle(_avatar, spritenode); // Put this at the very front
//
//
//
//	//CULog("Dude position: %f %f", _avatar->getPosition().x, _avatar->getPosition().y);
//	//CULog("Dude anchor: %f %f", _avatar->getAnchor().x, _avatar->getAnchor().y);
//
//	// Play the background music on a loop.
//	//std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
//	//AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
//
//
//	//cugl::Size shrimpSize = cugl::Size(2.0f, 2.0f);
//
//	Vec2 pos = SHRIMP_POS;
//	Size size = cugl::Size(4.0f, 4.0f);
//	image = _assets->get<Texture>("beefIdle");
//
//	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
//	std::shared_ptr<EnemyModel> _enemy = Beef::allocWithConstants({ 30.0f, 6.0f }, size, _scale, _assets);
//	spritenode->setScale(0.1f);
//	spritenode->setAnchor(Vec2(0.5, 0.5));
//	_enemy->setSceneNode(spritenode);
//	_enemy->setName(ENEMY_NAME);
//	_enemy->setDebugColor(DEBUG_COLOR);
//	_enemy->setLimit(cugl::Spline2(Vec2(0.0f, 0.3f), Vec2(100.0f, 0.3f)));
//	//scene.addObstacle(_enemy, spritenode);
//	//_enemies.push_back(_enemy);
//
//
//	cugl::Size riceSize = cugl::Size(1.0f, 1.5f);
//	cugl::Vec2 riceAnchor = cugl::Vec2(0.5, 0.30);
//
//	Vec2 rice_pos = RICE_POS;
//	image = _assets->get<Texture>("riceLeader");
//	_enemy = Rice::allocWithConstants(rice_pos, riceSize, _scale, _assets, false);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 3,4,12);
//	spritenode->setScale(0.12f);
//	spritenode->setAnchor(riceAnchor);
//	_enemy->setSceneNode(spritenode);
//	_enemy->setName(ENEMY_NAME);
//	_enemy->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy, spritenode);
//	_enemies.push_back(_enemy);
//
//	rice_pos = RICE_POS;
//	image = _assets->get<Texture>("riceLeader");
//	_enemy = Rice::allocWithConstants({20.0f, 6.0f}, riceSize, _scale, _assets, false);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 4, 12);
//	spritenode->setScale(0.12f);
//	spritenode->setAnchor(riceAnchor);
//	_enemy->setSceneNode(spritenode);
//	_enemy->setName(ENEMY_NAME);
//	_enemy->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy, spritenode);
//	_enemies.push_back(_enemy);
//
//	image = _assets->get<Texture>("riceSoldier");
//	std::shared_ptr<EnemyModel> _enemy1 = Rice::allocWithConstants({27.0f, 6.0f}, riceSize, _scale, _assets, true);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
//	spritenode->setScale(0.2f);
//	spritenode->setAnchor(riceAnchor);
//	_enemy1->setSceneNode(spritenode);
//	_enemy1->setName(ENEMY_NAME);
//	_enemy1->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy1, spritenode);
//	_enemies.push_back(_enemy1);
//
//	image = _assets->get<Texture>("riceSoldier");
//	std::shared_ptr<EnemyModel> _enemy2 = Rice::allocWithConstants({ 29.0f, 6.0f }, riceSize, _scale, _assets, true);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
//	spritenode->setScale(0.2f);
//	spritenode->setAnchor(riceAnchor);
//	_enemy2->setSceneNode(spritenode);
//	_enemy2->setName(ENEMY_NAME);
//	_enemy2->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy2, spritenode);
//	_enemies.push_back(_enemy2);
//
//	Rice* _enemy1Weak = (Rice*)_enemy1.get();
//	Rice* _enemy2Weak = (Rice*)_enemy2.get();
//
//	scene2::SceneNode* weak = _enemy->getSceneNode().get();
//	_enemy->setListener([=](physics2::Obstacle* obs) {
//
//		weak->setPosition(obs->getPosition()* _scale);
//		weak->setAngle(obs->getAngle());
//		if (_enemy->getState() == "pursuing") {
//
//			if(_enemy1Weak->getState() == "patrolling") _enemy1Weak->setState("acknowledging");
//			if (_enemy2Weak->getState() == "patrolling") _enemy2Weak->setState("acknowledging");
//			_enemy1Weak->setTargetPosition(_avatar->getPosition());
//			_enemy2Weak->setTargetPosition(_avatar->getPosition());
//		}
//		else {
//			_enemy1Weak->setState("patrolling");
//			_enemy2Weak->setState("patrolling");
//			_enemy1Weak->setTargetPosition(_enemy->getPosition());
//			_enemy2Weak->setTargetPosition(_enemy->getPosition());
//		}
//		});
//
//
//
//
//	////shrimp 2
//	//image = _assets->get<Texture>("shrimp_rolling");
//	//_enemy = EnemyModel::alloc({ 30.0f, 6.0f }, shrimpSize, _scale, EnemyType::shrimp);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.1f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//
//
//	//////shrimp 3
//	//image = _assets->get<Texture>("shrimp_rolling");
//	//_enemy = EnemyModel::alloc({ 25.0f, 18.0f }, shrimpSize, _scale, EnemyType::shrimp);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.1f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//	//cugl::Size riceSize = cugl::Size(1.0f, 2.0f);
//
//	//Vec2 rice_pos = RICE_POS;
//	//image = _assets->get<Texture>(RICE_TEXTURE);
//	//_enemy = EnemyModel::alloc(rice_pos, riceSize, _scale, EnemyType::rice);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.12f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//	//////rice 2
//	//image = _assets->get<Texture>(RICE_TEXTURE);
//	//_enemy = EnemyModel::alloc({27.0f, 28.0f}, riceSize, _scale, EnemyType::rice);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.12f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//	//////rice 3
//	//image = _assets->get<Texture>(RICE_TEXTURE);
//	//_enemy = EnemyModel::alloc({ 35.0f, 37.0f }, riceSize, _scale, EnemyType::rice);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.12f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//
//
//	//cugl::Size eggSize = cugl::Size(1.5f, 4.5f);
//	//Vec2 egg_pos = EGG_POS;
//	//image = _assets->get<Texture>(EGG_TEXTURE);
//	//_enemy = EnemyModel::alloc(egg_pos, eggSize, _scale, EnemyType::egg);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.1f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//	//////egg 2
//	////image = _assets->get<Texture>(EGG_TEXTURE);
//	////_enemy = EnemyModel::alloc({25.0f, 28.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
//	////sprite = scene2::PolygonNode::allocWithTexture(image);
//	////_enemy->setSceneNode(sprite);
//	////_enemy->setName(ENEMY_NAME);
//	////_enemy->setDebugColor(DEBUG_COLOR);
//	////scene.addObstacle(_enemy, sprite);
//	////_enemies.push_back(_enemy);
//
//	//////egg 3
//
//	//image = _assets->get<Texture>(EGG_TEXTURE);
//	//_enemy = EnemyModel::alloc({30.0f, 30.0f}, eggSize, _scale, EnemyType::egg);
//	//sprite = scene2::PolygonNode::allocWithTexture(image);
//	//sprite->setScale(0.1f);
//	//_enemy->setSceneNode(sprite);
//	//_enemy->setName(ENEMY_NAME);
//	//_enemy->setDebugColor(DEBUG_COLOR);
//	//scene.addObstacle(_enemy, sprite);
//	//_enemies.push_back(_enemy);
//
//	scene.setAssets(_assets);
//	scene.setScale(_scale);
//	scene.setBackground(_background);
//	scene.setAvatar(_avatar);
//	scene.setEnemies(_enemies);
//	scene.setGoalDoor(_goalDoor);

}
void Level1::update(float step) {
	/*for (const auto& obj : things) {
		if (obj->queryPath(0).z > -1) {
			obj->update(step);
		}
	}*/
}