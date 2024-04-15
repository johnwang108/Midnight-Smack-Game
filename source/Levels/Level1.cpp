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

////float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
////	{ 1.0f, 3.0f, 1.0f, 2.5f, 6.0f, 2.5f, 6.0f, 3.0f},
////	{ 6.0f, 4.0f, 6.0f, 2.5f, 9.0f, 2.5f, 9.0f, 4.0f},
////	{23.0f, 4.0f,23.0f, 2.5f,31.0f, 2.5f,31.0f, 4.0f},
////	{26.0f, 5.5f,26.0f, 5.0f,28.0f, 5.0f,28.0f, 5.5f},
////	{29.0f, 7.0f,29.0f, 6.5f,31.0f, 6.5f,31.0f, 7.0f},
////	{24.0f, 8.5f,24.0f, 8.0f,27.0f, 8.0f,27.0f, 8.5f},
////	{29.0f,10.0f,29.0f, 9.5f,31.0f, 9.5f,31.0f,10.0f},
////	{23.0f,11.5f,23.0f,11.0f,27.0f,11.0f,27.0f,11.5f},
////	{19.0f,12.5f,19.0f,12.0f,23.0f,12.0f,23.0f,12.5f},
////	{ 1.0f,12.5f, 1.0f,12.0f, 7.0f,12.0f, 7.0f,12.5f}
////};
//
////static float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
////	{1.0f, 4.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 4.0f},
////	{ 6.0f, 4.0f, 6.0f, 2.5f, 9.0f, 2.5f, 9.0f, 4.0f},
////	{9.5f, 6.0f, 9.5f, 5.0f, 12.5f, 5.0f, 12.5f, 6.0f},
////	{15.0f, 8.5f, 15.0f, 7.0f, 20.0f, 4.5f, 20.0f, 6.0f},
////	{23.0f, 4.0f, 23.0f, 3.0f, 27.0f, 3.0f, 27.0f, 4.0f},
////	{28.0f, 5.0f, 28.0f, 4.0f, 30.0f, 8.0f, 30.0f, 9.0f},
////	{23.0f, 10.0f, 23.0f, 9.f, 27.0f, 9.f, 27.0f, 10.f},
////	{16.0f, 12.f, 16.0f, 10.0f, 22.0f, 12.0f, 22.0f, 10.f},
////	{6.0f, 15.0f, 6.0f, 14.5f, 14.0f, 12.5f, 14.0f, 13.0f},
////	{ 1.0f,12.5f, 1.0f,12.0f, 7.0f,12.0f, 7.0f,12.5f}
////};
//
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
//
///** The outlines of all of the platforms */
//static float ALT_PLATFORMS[ALT_PLATFORM_COUNT][ALT_PLATFORM_VERTS] = {
//	{ 1.0f, .5f, 1.0f, .0f, 33.0f, .0f, 33.0f, .50f}
//};
//
//
//void Level1::populate(GameScene& scene) {
//	_assets = scene.getAssets();
//	_scale = scene.getScale();
//	_background = scene.getBackground();
//	_avatar = scene.getAvatar();
//	_enemies = scene.getEnemies();
//	_goalDoor = scene.getGoalDoor();
//	//
//	//
//	//#pragma mark : Goal door
//	//	std::shared_ptr<Texture> image = _assets->get<Texture>("background-1");
//	//	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
//	//	image = _assets->get<Texture>(GOAL_TEXTURE);
//	//	std::shared_ptr<cugl::JsonValue> jsonValue = cugl::JsonValue::allocWithJson(json);
//	//	
//	//	std::map<int, std::string> idToImage = {};
//	//
//	//	std::shared_ptr<cugl::JsonReader> jsonReader;
//	//
//	//	// this is to fetch the layers object -- jsonValue->get("layers");
//	//	
//	//	// screen height and width will always be 1280 x 800
//	//	// this is equivalent to 40 by 25 in terms of 32 square
//	//	// pixel tiles
//	//	// the width 
//	//
//	//	window_height = jsonValue->getInt("height");
//	//	std::string window_height_string = jsonValue->getString("height");
//	//	int width = jsonValue->getInt("width");
//	//
//	//	CULog(window_height_string.c_str());
//	//
//	//	std::shared_ptr<cugl::JsonValue> tilesets = jsonValue->get("tilesets");
//	//	
//	//	if (tilesets != nullptr && tilesets->isArray()) {
//	//		for (int i = 0; i < tilesets->size(); i++) {
//	//			std::shared_ptr<cugl::JsonValue> tile  = tilesets->get(i);
//	//			int tileId = tile->getInt("firstgid");
//	//			// this gets out egg_64.tsj for example, but its a relative path!!!!! In reality, egg_64.tsj exists in my local documents folder.
//	//			// So, we got to move all the created json tilesets into our assets file
//	//			// STILL HAVE TO DO
//	//
//	//			// this grid is going to be important, as we will need to tell what pixel
//	//			// is associated with what image (STILL HAVE TO DO)
//	//
//	//
//	//			// it should be harder than just this
//	//			std::string tileSource = tile->getString("source");
//	//			// CULog(tileSource.c_str());
//	//			tileSource.insert(0, "\\json\\");
//	//			
//	//			// tileSource.insert(0, _getcwd(NULL, 0));
//	//			
//	//			// creates a reader that reads the tileset json in a level,
//	//			// where we will get the image 
//	//			// now, our tileSource string will be /../../assets/json/egg_64_enemy.tsj or something
//	//			// So, every tileSet will needed to be added into the assets/json folder as well :(
//	//
//	//			CULog(_getcwd(NULL, 0));
//	//
//	//			jsonReader = cugl::JsonReader::allocWithAsset(tileSource);
//	//			// _assets->attach<JsonValue>(jsonReader);
//	//			
//	//			std::string absPath = "";
//	//			CULog(tileSource.c_str());
//	//			CULog("WE GOT HERE WITHOUT AN ERROR!");
//	//			std::shared_ptr<JsonValue> nestedJson = jsonReader->readJson();
//	//			// jsonReader->readJsonString() != "image"
//	//
//	//			// THIS IS CURRENTLY NOT WORKING, RIGHT NOW EXTERNAL JSON THING NOT BEING READ
//	//			absPath = nestedJson->getString("image");
//	//			/*while (jsonReader->readJsonString() != "image") {
//	//				CULog("current item we have:");
//	//				CULog(jsonReader->readJsonString().c_str());
//	//				continue;
//	//			}*/
//	//			CULog("Did we ever get here?");
//	//			// absPath will now be the string RIGHT AFTER our image property
//	//			// in our tileSet json
//	//			// absPath = jsonReader->readJsonString();
//	//
//	//			// std::string absPath = tileSource->getString("image");
//	//			size_t startPos = absPath.find("textures");
//	//
//	//			if (startPos == std::string::npos) {
//	//				startPos = 0;
//	//			}
//	//			std::string imgPath = absPath.substr(startPos);
//	//			// imgPath = '\\' + imgPath;
//	//			std::replace(imgPath.begin(), imgPath.end(), '\/', '\\');
//	//			CULog(imgPath.c_str());
//	//			idToImage.emplace(tileId, imgPath);
//	//
//	//			CULog("OMG we got here!!");
//	//
//	//			// for our example, idToImage will look like:
//	//			// {1:'background-image-1', 705: 'blocky-landscape', 713: 'goaldoor', etc.}
//	//
//	//			// now, we have the image and can initialize as done
//	//			// in the populate method
//	//		}
//	//	}
//	//
//	//	/*for (auto const& [key, value] : idToImage) {
//	//		std::string str = k + ":" + v;
//	//		CULog(str.c_str());
//	//	}*/
//	//
//	//	std::shared_ptr<cugl::JsonValue> layers = jsonValue->get("layers");
//	//	int rowNum = 0;
//	//	int colNum = 0;
//	//	int platformNumber = 0;
//	//	int borderNumber = 0;
//	//	if (layers != nullptr && layers->isArray()) {
//	//		for (int i = 0; i < layers->size(); i++) {
//	//			std::shared_ptr<cugl::JsonValue> layer = layers->get(i);
//	//			// for each layer in layers, we must get its data array
//	//			std::shared_ptr<cugl::JsonValue> data = layer->get("data");
//	//			std::shared_ptr<scene2::PolygonNode> sprite;
//	//
//	//			if (i == 0) { // this will always be the background level
//	//				// background id will always be 1
//	//				// in theory, this should set up the background
//	//				CULog("Now we should be drawing the background");
//	//				std::string pathWeWant = idToImage.at(1);
//	//				CULog(pathWeWant.c_str());
//	//				CULog(_getcwd(NULL, 0));
//	//				CULog("----------- ");
//	//				assets->load<Texture>(pathWeWant, pathWeWant);
//	//				image = assets->get<Texture>(pathWeWant);
//	//				if (image == nullptr) {
//	//					CULog("POINTS TO NOTHING!!!");
//	//				}
//	//				else {
//	//					CULog("POINTS TO SOMETHING!!!!");
//	//				}
//	//				sprite = scene2::PolygonNode::allocWithTexture(image);
//	//				Size background_size(image->getSize().width / _scale, image->getSize().height / _scale);
//	//				// _background = physics2::BoxObstacle::alloc(Vec2(0,window_height), background_size);
//	//				_background = physics2::BoxObstacle::alloc(BACKGROUND_POS, background_size);
//	//				_background->setName(BACKGROUND_NAME);
//	//				_background->setBodyType(b2_staticBody);
//	//				_background->setDensity(0.0f);
//	//				_background->setFriction(0.0f);
//	//				_background->setRestitution(0.0f);
//	//				_background->setEnabled(false);
//	//				_background->setSensor(true);
//	//				scene.addObstacle(_background, sprite);
//	//				// breaks as we don't want to do anything else in this case
//	//				continue;
//	//			}
//				// in this example, our display size is 1280 by 800, where the width is 40 tiles and the height is 25
//				// so, there will 40 columns and 25 rows
//
//				// rowNum, colNum now will represent gridMapping of tiles
//				// now, we have to iterate through data array and see what our 
//				// values are there
//
//				//in our level_designer, there will be enums of possible things to add in
//				// WALL, PLATFORM, BACKGROUND, DUDE, ENEMY, AND OTHERS THAT ARISE.
//				// Each of our images in assets/textures will thus end in _BACKGROUND, or _ENEMY, or etc.
//
//		// Set the physics attributes
//
//
//#pragma mark : Walls
//	std::shared_ptr<Texture> image;
//	// All walls and platforms share the same texture
//
//	image = _assets->get<Texture>("earth");
//	std::string wname = "wall";
//	for (int ii = 0; ii < WALL_COUNT; ii++) {
//		std::shared_ptr<Wall> wallobj = Wall::alloc(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, DEBUG_COLOR,
//			reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS, wname);
//		things.push_back(wallobj);
//		scene.addObstacle(wallobj->getObj(), wallobj->getSprite(), 1);  // All walls share the same texture
//	}
//
//	//			//checking what type exists here
//	//			if (value.find("_ENEMY.tsj") != std::string::npos) {
//	//				createEnemy(value, assets, scene, rowNum, colNum);
//	//			}
//	//			else if (value.find("_GOAL_DOOR.tsj") != std::string::npos) {
//	//				createGoalDoor(value, assets, scene, rowNum, colNum);
//	//			}
//	//			else if (value.find("_DUDE.tsj") != std::string::npos) {
//	//				CULog("CHECKING IN ON THE DUDE");
//	//				createDude(value, assets, scene, rowNum, colNum);
//	//			}
//	//			else if (value.find("_BORDER.tsj") != std::string::npos) {
//	//				borderNumber = borderNumber + 1;
//	//				createPlatform(value, assets, scene, rowNum, colNum, borderNumber);
//	//				
//	//			}
//	//			else if (value.find("_PLATFORM.tsj") != std::string::npos) {
//	//				platformNumber = platformNumber + 1;
//	//				createPlatform(value, assets, scene, rowNum, colNum, platformNumber);
//	//			}
//	//			// WE WILL NEED TO ADD MORE?
//	//			CULog("UH-OH");
//
//	//		}
//
//	image = _assets->get<Texture>("su_idle");
//	//hardcoded size
//	cugl::Size s = PLAYER_SIZE_DEFAULT;
//	_avatar = DudeModel::allocWithConstants(dudePos, s, _scale, _assets);
//	std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);
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
//	Vec2 goalPos = GOAL_POS;
//	image = _assets->get<Texture>("goal");
//	Size goalSize(image->getSize().width / _scale, image->getSize().height / _scale);
//	_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);
//
//	_goalDoor->setBodyType(b2_staticBody);
//	_goalDoor->setDensity(0.0f);
//	_goalDoor->setFriction(0.0f);
//	_goalDoor->setRestitution(0.0f);
//	_goalDoor->setSensor(true);
//
//	std::shared_ptr<cugl::scene2::PolygonNode> sprite = cugl::scene2::PolygonNode::allocWithTexture(image);
//	_goalDoor->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_goalDoor, sprite);
//
//	Vec2 pos = SHRIMP_POS;
//	Size size = cugl::Size(2.0f, 2.0f);
//	image = _assets->get<Texture>("beefIdle");
//
//	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
//	std::shared_ptr<EnemyModel> _enemy = EnemyModel::allocWithConstants({ 30.0f, 6.0f }, size, _scale, _assets, EnemyType::beef);
//	spritenode->setScale(0.1f);
//	spritenode->setAnchor(Vec2(0.5, 0.35));
//	_enemy->setSceneNode(spritenode);
//	_enemy->setName(ENEMY_NAME);
//	_enemy->setDebugColor(DEBUG_COLOR);
//	_enemy->setLimit(cugl::Spline2(Vec2(1.0f, 1.0f), Vec2(50.0f, 1.0f)));
//	//scene.addObstacle(_enemy, spritenode);
//	//_enemies.push_back(_enemy);
//
//
//	cugl::Size riceSize = cugl::Size(1.0f, 2.0f);
//
//	Vec2 rice_pos = RICE_POS;
//	image = _assets->get<Texture>("riceLeader");
//	_enemy = EnemyModel::allocWithConstants(rice_pos, riceSize, _scale, _assets, EnemyType::rice);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 3, 4, 12);
//	spritenode->setScale(0.12f);
//	_enemy->setSceneNode(spritenode);
//	_enemy->setName(ENEMY_NAME);
//	_enemy->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy, spritenode);
//	_enemies.push_back(_enemy);
//
//	image = _assets->get<Texture>("riceSoldier");
//	std::shared_ptr<EnemyModel> _enemy1 = EnemyModel::allocWithConstants({ 27.0f, 6.0f }, riceSize, _scale, _assets, EnemyType::rice_soldier);
//	spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
//	spritenode->setScale(0.12f);
//	_enemy1->setSceneNode(spritenode);
//	_enemy1->setName(ENEMY_NAME);
//	_enemy1->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_enemy1, spritenode);
//	_enemies.push_back(_enemy1);
//
//	//image = _assets->get<Texture>("riceSoldier");
//	//std::shared_ptr<EnemyModel> _enemy2 = EnemyModel::allocWithConstants({ 29.0f, 6.0f }, riceSize, _scale, _assets, EnemyType::rice_soldier);
//	//spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 15);
//	//spritenode->setScale(0.12f);
//	//_enemy2->setSceneNode(spritenode);
//	//_enemy2->setName(ENEMY_NAME);
//	//_enemy2->setDebugColor(DEBUG_COLOR);
//	////scene.addObstacle(_enemy2, spritenode);
//	////_enemies.push_back(_enemy2);
//
//	//EnemyModel* _enemy1Weak = _enemy1.get();
//	//EnemyModel* _enemy2Weak = _enemy2.get();
//	//_enemy->setListener([=](physics2::Obstacle* obs) {
//	//	if (_enemy->getState() == "pursuing") {
//	//		_enemy1Weak->setState("pursuing");
//	//		_enemy2Weak->setState("pursuing");
//	//		_enemy1Weak->setTargetPosition(_avatar->getPosition());
//	//		_enemy2Weak->setTargetPosition(_avatar->getPosition());
//	//	}
//	//	else {
//	//		_enemy1Weak->setState("patrolling");
//	//		_enemy2Weak->setState("patrolling");
//	//		_enemy1Weak->setTargetPosition(_enemy->getPosition());
//	//		_enemy2Weak->setTargetPosition(_enemy->getPosition());
//	//	}
//	//	});
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
//		// }
//	// }
//
//
//}
//
////COME BACK TO THIS ONE
////void Level1::createBorder(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int borderNumber) {
////
////	assets->load<Texture>(path, path);
////	image = assets->get<Texture>(path);
////
////	std::string wname = "border";
////	//for (int ii = 0; ii < WALL_COUNT; ii++) {
////	//	std::shared_ptr<physics2::PolygonObstacle> wallobj;
////
////	//	Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
////	//	// Call this on a polygon to get a solid shape
////	//	EarclipTriangulator triangulator;
////	//	triangulator.set(wall.vertices);
////	//	triangulator.calculate();
////	//	wall.setIndices(triangulator.getTriangulation());
////	//	triangulator.clear();
////
////	//	wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
////	//	// You cannot add constant "".  Must stringify
////	//	wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
////	//	wallobj->setName(wname);
////
////	//	// Set the physics attributes
////	//	wallobj->setBodyType(b2_staticBody);
////	//	wallobj->setDensity(BASIC_DENSITY);
////	//	wallobj->setFriction(BASIC_FRICTION);
////	//	wallobj->setRestitution(BASIC_RESTITUTION);
////	//	wallobj->setDebugColor(DEBUG_COLOR);
////
////	//	wall *= _scale;
////	//	sprite = scene2::PolygonNode::allocWithTexture(image, wall);
////	//	scene.addObstacle(wallobj, sprite, 1);  // All walls share the same texture
////	//}
////}
//
//
////void Level1::createPlatform(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int platformNumber) {
////
////
////	float x = float(col * tile_width);
////	float y = float(window_height - (row * tile_height));
////
////	float platform_arr[8] = {x, y, x, y - 32.0f, x + 32.0f, y - 32.0f, x + 32.0f, y};
////
////	Poly2 platform(reinterpret_cast<Vec2*>(platform_arr), sizeof(platform_arr) / sizeof(float) / 2);
////
////	EarclipTriangulator triangulator;
////	triangulator.set(platform.vertices);
////	triangulator.calculate();
////	platform.setIndices(triangulator.getTriangulation());
////	triangulator.clear();
////
////	platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_TOP_LEFT);
////	platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(platformNumber));
////
////	platobj->setBodyType(b2_staticBody);
////	platobj->setDensity(BASIC_DENSITY);
////	platobj->setFriction(BASIC_FRICTION);
////	platobj->setRestitution(BASIC_RESTITUTION);
////	platobj->setDebugColor(DEBUG_COLOR);
////
////	platform *= _scale;
////	sprite = scene2::PolygonNode::allocWithTexture(image, platform);
////	scene.addObstacle(platobj, sprite, 1);
////
////}
////
////void Level1::createDude(std::string path, std::shared_ptr<AssetManager>& assets,
////	GameScene& scene, int row, int col) {
////
////	assets->load<Texture>(path, path);
////	image = assets->get<Texture>(path);
////
////	Vec2 dudePos = Vec2(col * tile_width, window_height - (row * tile_height));;
////	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	std::shared_ptr<scene2::WireNode> draw;
////
////	// Create obstacle
////	Vec2 goalPos = GOAL_POS;
////	Size goalSize(image->getSize().width / _scale,
////		image->getSize().height / _scale);
////	_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);
////
////	// Set the physics attributes
////	_goalDoor->setBodyType(b2_staticBody);
////	_goalDoor->setDensity(0.0f);
////	_goalDoor->setFriction(0.0f);
////	_goalDoor->setRestitution(0.0f);
////	_goalDoor->setSensor(true);
////	// _goalDoor->setEnabled
////
////	// Add the scene graph nodes to this object
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_goalDoor->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_goalDoor, sprite);
////
////
////		Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
////		// Call this on a polygon to get a solid shape
////		EarclipTriangulator triangulator;
////		triangulator.set(wall.vertices);
////		triangulator.calculate();
////		wall.setIndices(triangulator.getTriangulation());
////		triangulator.clear();
////
////		wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
////		// You cannot add constant "".  Must stringify
////		wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
////		wallobj->setName(wname);
////
////		// Set the physics attributes
////		wallobj->setBodyType(b2_staticBody);
////		wallobj->setDensity(BASIC_DENSITY);
////		wallobj->setFriction(BASIC_FRICTION);
////		wallobj->setRestitution(BASIC_RESTITUTION);
////		wallobj->setDebugColor(DEBUG_COLOR);
////
////		wall *= _scale;
////		sprite = scene2::PolygonNode::allocWithTexture(image, wall);
////		scene.addObstacle(wallobj, sprite, 1);  // All walls share the same texture
////	}
////
////#pragma mark : Platforms
////	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
////		std::shared_ptr<physics2::PolygonObstacle> platobj;
////		Poly2 platform(reinterpret_cast<Vec2*>(PLATFORMS[ii]), sizeof(PLATFORMS[ii]) / sizeof(float) / 2);
////
////		EarclipTriangulator triangulator;
////		triangulator.set(platform.vertices);
////		triangulator.calculate();
////		platform.setIndices(triangulator.getTriangulation());
////		triangulator.clear();
////
////		platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_CENTER);
////		// You cannot add constant "".  Must stringify
////		platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));
////
////		// Set the physics attributes
////		platobj->setBodyType(b2_staticBody);
////		platobj->setDensity(BASIC_DENSITY);
////		platobj->setFriction(BASIC_FRICTION);
////		platobj->setRestitution(BASIC_RESTITUTION);
////		platobj->setDebugColor(DEBUG_COLOR);
////
////		platform *= _scale;
////		sprite = scene2::PolygonNode::allocWithTexture(image, platform);
////		scene.addObstacle(platobj, sprite, 1);
////	}
////
////#pragma mark : Dude
////	Vec2 dudePos = DUDE_POS;
////	// node = scene2::SceneNode::alloc();
////	image = _assets->get<Texture>(DUDE_TEXTURE);
////	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_avatar->setSceneNode(sprite);
////	_avatar->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_avatar, sprite); // Put this at the very front
////
////	// Play the background music on a loop.
////	std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
////	AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);
////
////
////	Vec2 shrimp_pos = SHRIMP_POS;
////	image = _assets->get<Texture>(SHRIMP_TEXTURE);
////	std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc(shrimp_pos, image->getSize() / _scale, _scale, EnemyType::shrimp);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////
////	Vec2 rice_pos = RICE_POS;
////	image = _assets->get<Texture>(RICE_TEXTURE);
////	_enemy = EnemyModel::alloc(rice_pos, image->getSize() / _scale, _scale, EnemyType::rice);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////	_enemy->setGestureString("circle");
////
////	//rice 2
////	image = _assets->get<Texture>(RICE_TEXTURE);
////	_enemy = EnemyModel::alloc({ 27.0f, 28.0f }, image->getSize() / _scale, _scale, EnemyType::rice);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////
////	Vec2 egg_pos = EGG_POS;
////	image = _assets->get<Texture>(EGG_TEXTURE);
////	_enemy = EnemyModel::alloc(egg_pos, image->getSize() / (_scale), _scale, EnemyType::egg);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////	_enemy->setGestureString("pigtail");
////
////	//egg 2
////	image = _assets->get<Texture>(EGG_TEXTURE);
////	_enemy = EnemyModel::alloc({25.0f, 28.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////
////	//egg 3
////
////	image = _assets->get<Texture>(EGG_TEXTURE);
////	_enemy = EnemyModel::alloc({30.0f, 30.0f}, image->getSize() / (_scale), _scale, EnemyType::egg);
////	sprite = scene2::PolygonNode::allocWithTexture(image);
////	_enemy->setSceneNode(sprite);
////	_enemy->setName(ENEMY_NAME);
////	_enemy->setDebugColor(DEBUG_COLOR);
////	scene.addObstacle(_enemy, sprite);
////	_enemies.push_back(_enemy);
////
//scene.setAssets(_assets);
//scene.setScale(_scale);
//scene.setBackground(_background);
//scene.setAvatar(_avatar);
//scene.setEnemies(_enemies);
//scene.setGoalDoor(_goalDoor);
//
//// }
//
