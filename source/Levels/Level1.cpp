#include "Level1.h"
//#include "../PFGameScene.h"
//
///** The goal door position */
//static float GOAL_POS[] = { 4.0f,14.0f };
//// float GOAL_POS[] = { 6.0f, 5.0f };
///** The position of the spinning barrier */
//static float SPIN_POS[] = { 13.0f,12.5f };
///** The initial position of the dude */
//static float DUDE_POS[] = { 2.5f, 5.0f };
///** The position of the rope bridge */
//static float BRIDGE_POS[] = { 9.0f, 3.8f };
//
//static float SHRIMP_POS[] = { 22.0f, 16.0f };
//
//static float EGG_POS[] = { 14.0f, 18.0f };
//
//static float RICE_POS[] = { 25.0f, 14.0f };
//
//static float BACKGROUND_POS[] = { 16.0f, 10.0f };
//
//
//static float WALL[WALL_COUNT][WALL_VERTS] = {
//	{27.5f, 36.0f,  0.0f, 36.0f,  0.0f,  0.0f,
//	  1.0f,  0.0f,  1.0f, 35.5f, 27.5f, 35.5f },
//	{55.0f, 36.0f, 27.5f, 36.0f, 27.5f, 35.5f,
//	 54.0f, 35.5f, 54.0f,  0.0f, 55.0f,  0.0f }
//};
//
//
//
///** The outlines of all of the platforms */
//
// 
// 

void Level1::populate(GameScene& scene) {

}

void Level1::update(float step) {

}

#pragma mark : Platforms
	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
		std::shared_ptr<Wall> platObj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, Color4::BLUE,
			reinterpret_cast<Vec2*>(PLATFORMS[ii]), PLATFORM_VERTS, std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));
		std::vector<Vec3> path;
		path = { Vec3(0,100,120), Vec3(100,100,120), Vec3(100,0,120), Vec3(0,-30,120) };
		if (ii % 3 == 1) {
			platObj->initiatePath(path, 2.0f);
		}
		things.push_back(platObj);
		scene.addObstacle(platObj->getObj(), platObj->getSprite(), 1);
	}

#pragma mark : Dude
	Vec2 dudePos = DUDE_POS;
	// node = scene2::SceneNode::alloc();

	image = _assets->get<Texture>("su_idle");
	//hardcoded size
	cugl::Size s = PLAYER_SIZE_DEFAULT;
	_avatar = DudeModel::allocWithConstants(dudePos, s, _scale, _assets);
	std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4,16);

	//CALCULATE sue sprite size from sue obstacle size. Goal: su's feet line up with foot sensor, and head (not hat) with top of obstacle. Todo still
	//float scalar = (s.width *_scale) / spritenode->getSize().width;
	spritenode->setAnchor(Vec2(0.5, 0.35));
	spritenode->setPosition(dudePos);
	_avatar->setSceneNode(spritenode);
	_avatar->setDebugColor(DEBUG_COLOR);

	scene.addObstacle(_avatar, spritenode); // Put this at the very front



	//CULog("Dude position: %f %f", _avatar->getPosition().x, _avatar->getPosition().y);
	//CULog("Dude anchor: %f %f", _avatar->getAnchor().x, _avatar->getAnchor().y);

	// Play the background music on a loop.
	//std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
	//AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);


	//cugl::Size shrimpSize = cugl::Size(2.0f, 2.0f);

	Vec2 pos = SHRIMP_POS;
	Size size = cugl::Size(4.0f, 4.0f);
	image = _assets->get<Texture>("beefIdle");

	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
	std::shared_ptr<EnemyModel> _enemy = EnemyModel::allocWithConstants({ 30.0f, 6.0f }, size, _scale, _assets, EnemyType::beef);
	spritenode->setScale(0.1f);
	spritenode->setAnchor(Vec2(0.5, 0.5));
	_enemy->setSceneNode(spritenode);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	_enemy->setLimit(cugl::Spline2(Vec2(0.0f, 0.3f), Vec2(100.0f, 0.3f)));
	scene.addObstacle(_enemy, spritenode);
	_enemies.push_back(_enemy);


	cugl::Size riceSize = cugl::Size(1.0f, 1.5f);
	cugl::Vec2 riceAnchor = cugl::Vec2(0.5, 0.30);

	Vec2 rice_pos = RICE_POS;
	image = _assets->get<Texture>("riceLeader");
	_enemy = EnemyModel::allocWithConstants(rice_pos, riceSize, _scale, _assets, EnemyType::rice);
	spritenode = EntitySpriteNode::allocWithSheet(image, 3,4,12);
	spritenode->setScale(0.12f);
	spritenode->setAnchor(riceAnchor);
	_enemy->setSceneNode(spritenode);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, spritenode);
	_enemies.push_back(_enemy);

	rice_pos = RICE_POS;
	image = _assets->get<Texture>("riceLeader");
	_enemy = EnemyModel::allocWithConstants({20.0f, 6.0f}, riceSize, _scale, _assets, EnemyType::rice);
	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 4, 12);
	spritenode->setScale(0.12f);
	spritenode->setAnchor(riceAnchor);
	_enemy->setSceneNode(spritenode);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, spritenode);
	_enemies.push_back(_enemy);

	image = _assets->get<Texture>("riceSoldier");
	std::shared_ptr<EnemyModel> _enemy1 = EnemyModel::allocWithConstants({27.0f, 6.0f}, riceSize, _scale, _assets, EnemyType::rice_soldier);
	spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
	spritenode->setScale(0.2f);
	spritenode->setAnchor(riceAnchor);
	_enemy1->setSceneNode(spritenode);
	_enemy1->setName(ENEMY_NAME);
	_enemy1->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy1, spritenode);
	_enemies.push_back(_enemy1);

	image = _assets->get<Texture>("riceSoldier");
	std::shared_ptr<EnemyModel> _enemy2 = EnemyModel::allocWithConstants({ 29.0f, 6.0f }, riceSize, _scale, _assets, EnemyType::rice_soldier);
	spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
	spritenode->setScale(0.2f);
	spritenode->setAnchor(riceAnchor);
	_enemy2->setSceneNode(spritenode);
	_enemy2->setName(ENEMY_NAME);
	_enemy2->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy2, spritenode);
	_enemies.push_back(_enemy2);

	EnemyModel* _enemy1Weak = _enemy1.get();
	EnemyModel* _enemy2Weak = _enemy2.get();

	scene2::SceneNode* weak = _enemy->getSceneNode().get();
	_enemy->setListener([=](physics2::Obstacle* obs) {

		weak->setPosition(obs->getPosition()* _scale);
		weak->setAngle(obs->getAngle());
		if (_enemy->getState() == "pursuing") {

			if(_enemy1Weak->getState() == "patrolling") _enemy1Weak->setState("acknowledging");
			if (_enemy2Weak->getState() == "patrolling") _enemy2Weak->setState("acknowledging");
			_enemy1Weak->setTargetPosition(_avatar->getPosition());
			_enemy2Weak->setTargetPosition(_avatar->getPosition());
		}
		else {
			_enemy1Weak->setState("patrolling");
			_enemy2Weak->setState("patrolling");
			_enemy1Weak->setTargetPosition(_enemy->getPosition());
			_enemy2Weak->setTargetPosition(_enemy->getPosition());
		}
		});




	////shrimp 2
	//image = _assets->get<Texture>("shrimp_rolling");
	//_enemy = EnemyModel::alloc({ 30.0f, 6.0f }, shrimpSize, _scale, EnemyType::shrimp);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.1f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);



	//////shrimp 3
	//image = _assets->get<Texture>("shrimp_rolling");
	//_enemy = EnemyModel::alloc({ 25.0f, 18.0f }, shrimpSize, _scale, EnemyType::shrimp);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.1f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);

	//cugl::Size riceSize = cugl::Size(1.0f, 2.0f);

	//Vec2 rice_pos = RICE_POS;
	//image = _assets->get<Texture>(RICE_TEXTURE);
	//_enemy = EnemyModel::alloc(rice_pos, riceSize, _scale, EnemyType::rice);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.12f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);

	//////rice 2
	//image = _assets->get<Texture>(RICE_TEXTURE);
	//_enemy = EnemyModel::alloc({27.0f, 28.0f}, riceSize, _scale, EnemyType::rice);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.12f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);

	//////rice 3
	//image = _assets->get<Texture>(RICE_TEXTURE);
	//_enemy = EnemyModel::alloc({ 35.0f, 37.0f }, riceSize, _scale, EnemyType::rice);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.12f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);



	//cugl::Size eggSize = cugl::Size(1.5f, 4.5f);
	//Vec2 egg_pos = EGG_POS;
	//image = _assets->get<Texture>(EGG_TEXTURE);
	//_enemy = EnemyModel::alloc(egg_pos, eggSize, _scale, EnemyType::egg);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.1f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);

	//////egg 2
	////image = _assets->get<Texture>(EGG_TEXTURE);
	////_enemy = EnemyModel::alloc({25.0f, 28.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
	////sprite = scene2::PolygonNode::allocWithTexture(image);
	////_enemy->setSceneNode(sprite);
	////_enemy->setName(ENEMY_NAME);
	////_enemy->setDebugColor(DEBUG_COLOR);
	////scene.addObstacle(_enemy, sprite);
	////_enemies.push_back(_enemy);

	//////egg 3

	//image = _assets->get<Texture>(EGG_TEXTURE);
	//_enemy = EnemyModel::alloc({30.0f, 30.0f}, eggSize, _scale, EnemyType::egg);
	//sprite = scene2::PolygonNode::allocWithTexture(image);
	//sprite->setScale(0.1f);
	//_enemy->setSceneNode(sprite);
	//_enemy->setName(ENEMY_NAME);
	//_enemy->setDebugColor(DEBUG_COLOR);
	//scene.addObstacle(_enemy, sprite);
	//_enemies.push_back(_enemy);

	scene.setAssets(_assets);
	scene.setScale(_scale);
	scene.setBackground(_background);
	scene.setAvatar(_avatar);
	scene.setEnemies(_enemies);
	scene.setGoalDoor(_goalDoor);

}
void Level1::update(float step) {
	for (const auto& obj : things) {
		if (obj->queryPath(0).z > -1) {
			obj->update(step);
		}
	}
}