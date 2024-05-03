
#include <cugl/assets/CUJsonLoader.h>
#include "LevelModel.h"
#include "../PFDudeModel.h"
#include "../PFGameScene.h"
//#include <direct.h>
#include "Levels.h"

#pragma mark -
#pragma mark Static Constructors


// HEADS UP:
// -----------------------------------
// in PFGameScene.cpp, we are going to load in our level by calling
// _assets->load<LevelModel>(level_one_key, level_one_file),
// where level_one_key = 'level1' and level_one_file = 'json/level1.json',
// we define std::shared_ptr<LevelModel> _level, and
// _level = assets->get<LevelModel>(level_one_key);

/**
* Creates a new, empty level
*/
LevelModel::LevelModel(void)
{
	//_bounds.size.set(1.0f, 1.0f);
}

LevelModel::LevelModel(std::string path)
{
	//this should set the file path in here
	setFilePath(path);
}

int LevelModel::loadLevelWidth() {
	// setFilePath(path);
	std::shared_ptr<cugl::JsonReader> levelReader;
	levelReader = levelReader = cugl::JsonReader::allocWithAsset(getFilePath());
	std::shared_ptr<JsonValue> level_json_file = levelReader->readJson();

	if (level_json_file == nullptr) {
		CUAssertLog(false, "Failed to load the level width BEFORE DONE in populate()");
	}

	level_width = level_json_file->getInt("width");
	return level_width;
}

int LevelModel::loadLevelHeight() {
	// setFilePath(path);
	std::shared_ptr<cugl::JsonReader> levelReader;
	levelReader = levelReader = cugl::JsonReader::allocWithAsset(getFilePath());
	std::shared_ptr<JsonValue> level_json_file = levelReader->readJson();

	if (level_json_file == nullptr) {
		CUAssertLog(false, "Failed to load the level width BEFORE DONE in populate()");
	}

	level_height = level_json_file->getInt("height");
	return level_height;
}

void LevelModel::populate(GameScene& scene) {

	_assets = scene.getAssets();
	//_scale = scene.getScale();
	_background = scene.getBackground();
	_avatar = scene.getAvatar();
	//_enemies = scene.getEnemies();
	_goalDoor = scene.getGoalDoor();


	std::shared_ptr<cugl::JsonReader> levelReader;
	//have to set the filepath before this

	levelReader = cugl::JsonReader::allocWithAsset(getFilePath());

	std::shared_ptr<JsonValue> level_json_file = levelReader->readJson();

	if (level_json_file == nullptr) {
		CUAssertLog(false, "Failed to load level file");
	}

	int window_height = level_json_file->getInt("height");
	// std::string window_height_string = json->getString("height");
	int window_width = level_json_file->getInt("width");

	level_width = window_width;
	level_height = window_height;

	CULog("Level Width:");
	CULog(std::to_string(level_width).c_str());
	CULog("Level Height:");
	CULog(std::to_string(level_height).c_str());


	// _bounds.size.set(window_width * 32, window_height * 32);

	idToImage = {};
	// do I need to set gravity? If so, add here

	// rather than creating a physics world, we will load in the one from game scene
	//setWorld(GameScene::getWorld());
	//world is only needed for creating and destroying obstacles in unload, we will do this in gamescene probably

	//in order for this to work, we will have to loop through the objects array in the json and find one with a certain name
	std::shared_ptr<cugl::JsonValue> tilesets = level_json_file->get("tilesets");
	std::shared_ptr<cugl::JsonValue> layers = level_json_file->get("layers");

	std::shared_ptr<scene2::PolygonNode> sprite;

	std::shared_ptr<cugl::JsonReader> jsonReader;

	// std::shared_ptr<cugl::JsonValue> tilesets = jsonValue->get("tilesets");

	if (tilesets != nullptr && tilesets->isArray()) {
		for (int i = 0; i < tilesets->size(); i++) {
			std::shared_ptr<cugl::JsonValue> tile = tilesets->get(i);
			int tileId = tile->getInt("firstgid");
			// this gets out egg_64.tsj for example, but its a relative path!!!!! In reality, egg_64.tsj exists in my local documents folder.
			// So, we got to move all the created json tilesets into our assets file
			// STILL HAVE TO DO

			// this grid is going to be important, as we will need to tell what pixel
			// is associated with what image (STILL HAVE TO DO)


			// it should be harder than just this
			std::string tileSource = tile->getString("source");
			// CULog(tileSource.c_str());
			tileSource.insert(0, "json\\");

			// tileSource.insert(0, _getcwd(NULL, 0));

			// creates a reader that reads the tileset json in a level,
			// where we will get the image 
			// now, our tileSource string will be /../../assets/json/egg_64_enemy.tsj or something
			// So, every tileSet will needed to be added into the assets/json folder as well :(

	//		CULog(_getcwd(NULL, 0));
			// CULog(level_file_path.c_str());
			jsonReader = cugl::JsonReader::allocWithAsset(tileSource);
			// _assets->attach<JsonValue>(jsonReader);
			if (jsonReader == nullptr) {
				CULog("WE GOT PROBLEMS");
			}
			std::string absPath = "";
			CULog(tileSource.c_str());
			CULog("WE GOT HERE WITHOUT AN ERROR!");
			std::shared_ptr<JsonValue> nestedJson = jsonReader->readJson();
			// jsonReader->readJsonString() != "image"

			absPath = nestedJson->getString("image");

			//we need to create dream-background.json and main platform.json

			/*while (jsonReader->readJsonString() != "image") {
				CULog("current item we have:");
				CULog(jsonReader->readJsonString().c_str());
				continue;
			}*/
			CULog("Did we ever get here?");
			// absPath will now be the string RIGHT AFTER our image property
			// in our tileSet json
			// absPath = jsonReader->readJsonString();

			// std::string absPath = tileSource->getString("image");
			size_t startPos = absPath.find("textures");

			if (startPos == std::string::npos) {
				startPos = 0;
			}
			std::string imgPath = absPath.substr(startPos);
			// imgPath = '\\' + imgPath;
			std::replace(imgPath.begin(), imgPath.end(), '\/', '\\');
			CULog(imgPath.c_str());
			idToImage.emplace(tileId, imgPath);

			CULog("OMG we got here!!");

			// for our example, idToImage will look like:
			// {1:'background-image-1', 705: 'blocky-landscape', 713: 'goaldoor', etc.}

			// now, we have the image and can initialize as done
			// in the populate method
		}
	}
	//Now we will initalize the image layers, in our case a dream background and main platform section
	int rowNum = 0;
	int colNum = 0;
	std::shared_ptr<Texture> image;
	std::shared_ptr<cugl::JsonValue> data;
	std::shared_ptr<cugl::JsonValue> objects;
	std::string enemy_name;
	std::shared_ptr<cugl::JsonValue> properties;
	std::shared_ptr<cugl::physics2::BoxObstacle> background_item;


	/*image = _assets->get<Texture>("textures\\dream-background.png");
	std::shared_ptr<cugl::scene2::PolygonNode> back_back_ground = cugl::scene2::PolygonNode::allocWithTexture(image);
	scene.addChild(back_back_ground);*/
	CULog("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ");
	// image = _assets->get<Texture>("textures\\main_platform.png");
	// _background = cugl::scene2::PolygonNode::allocWithTexture(image);
	// scene.addChild(_background);

	//initialize our backgrounds variable
	// _drawings = {};

	Vec2 p;

	std::shared_ptr<physics2::PolygonObstacle> mainPlatform;
	if (layers != nullptr && layers->isArray()) {
		for (int i = 0; i < layers->size(); i++) {
			std::shared_ptr<cugl::JsonValue> layer = layers->get(i);

			data = layer->get("data");
			objects = layer->get("objects");
			enemy_name = layer->getString("name");
			//type will be stored in class property of tile layer
			properties = layer->get("properties");

			if (data != nullptr && data->isArray()) {
				//we are looking at image layer, only adding backgrounds
				for (int j = 0; j < data->size(); j++) {
					colNum = j % static_cast<int>(window_width);
					if (colNum == 0 && j != 0) {
						rowNum += 1;
					}
					int tileId = data->get(j)->asInt();
					// CULog(("data value " + std::to_string(j)).c_str());
					if (idToImage.find(tileId) != idToImage.end()) {

						// we will have to change this, eventually we will also be drawing avatar, goaldoor, and
						// other things

						//means that we have found a key in our unordered map
						std::string pathWeWant = idToImage.at(tileId);
						CULog(pathWeWant.c_str());
//						CULog(_getcwd(NULL, 0));
						CULog("----------- ");
						if (_assets == nullptr) {
							CULog("you gotta define assets bro");
						}
						_assets->load<Texture>(pathWeWant, pathWeWant);
						CULog("//////////////");
						image = _assets->get<Texture>(pathWeWant);
						if (image == nullptr) {
							CULog("POINTS TO NOTHING!!!");
						}
						else {
							CULog("POINTS TO SOMETHING!!!!");
						}

						std::shared_ptr<cugl::JsonValue> property = properties->get(0);

						CULog("does it stop working here? 12:47 AM");
						std::string type = property->getString("value");
						CULog("we got passed an error!!!!!");
						CULog(type.c_str());

						int numOfRice = 0;

						if (type == "background") {
							CULog("identified class field as background");
							std::string debugMsg = "Current image we are loading into background: " + pathWeWant;
							CULog(debugMsg.c_str());
							// for some reason, despite appearing first, dream-background.png
							// writes over everything (over the avatar, main_platform, and object layers
							if (pathWeWant != "textures\\dream-background-1.png") {
								_background = cugl::scene2::PolygonNode::allocWithTexture(image);
								_background->getSize();
								_background->setName("background");
								// _background->setPosition(Vec2(0,0));
								CULog(std::to_string(_background->getPositionX()).c_str());
								CULog(std::to_string(_background->getPositionY()).c_str());
								// _background->setPositionY(400.0);
								scene.addChild(_background);
							}
						}
						else if (type == "golden_door") {
							CULog("identified class field as golden door");
							sprite = scene2::PolygonNode::allocWithTexture(image);
							// std::shared_ptr<scene2::WireNode> draw;
							// Create obstacle
							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;
							Vec2 goalPos = Vec2(colNum, rowPos);
							Size goalSize(image->getSize().width / scene.getScale(),
								image->getSize().height / scene.getScale());
							_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);
							// _goalDoor->setPosition(0, 0);
							// Set the physics attributes
							_goalDoor->setBodyType(b2_staticBody);
							_goalDoor->setDensity(0.0f);
							_goalDoor->setFriction(0.0f);
							_goalDoor->setRestitution(0.0f);
							_goalDoor->setSensor(true);
							// _goalDoor->setEnabled

							// Add the scene graph nodes to this object
							_goalDoor->setDebugColor(DEBUG_COLOR);
							scene.addObstacle(_goalDoor, sprite);
						}
						else if (type == "avatar") {
							CULog("identified class field as avatar");
							CULog("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
							sprite = scene2::PolygonNode::allocWithTexture(image);
							// harcoded window_height to 25

							// float w = ( (window_height - rowNum) % window_height + window_height) % window_height;

							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;

							// float scale_refactor = (210.0f / 40.0f);
							//WE WILL HAVE TO CHANGE THIS

							// Vec2 dudePos = Vec2(colNum + (3 * scale_refactor), rowPos + (3 * scale_refactor));
							Vec2 dudePos = Vec2(colNum, rowPos);

							p = dudePos;

							//Vec2 dudePos = Vec2(colNum, (window_height - (rowNum * 32)) / 32);
							// this is what we actually want, but code being annoying
							// _avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + scene.getScale()), scene.getScale());
							CULog("AVATAR: ");
							CULog("column number: ");
							CULog(std::to_string(colNum).c_str());
							CULog("row number: ");
							// % 25
							CULog(std::to_string(rowPos).c_str());
							Size size = Size(1.7, 3.3);
							CULog(std::to_string(scene.getScale()).c_str());
							_avatar = DudeModel::allocWithConstants(dudePos, size, scene.getScale(), _assets);
							// _avatar = DudeModel::alloc(dudePos, image->getSize(), scene.getScale());
							CULog(std::to_string(_avatar->getWidth()).c_str());
							CULog(std::to_string(_avatar->getHeight()).c_str());
							//we manually set this to combat an error in gameScene, does it work tho?
							// _avatar->setHeight(170.0f);
							// _avatar->setWidth(100.0f);
							//----------------
							// sprite = scene2::PolygonNode::allocWithTexture(image);
							std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);
							spritenode->setAnchor(Vec2(0.5, 0.35));
							_avatar->setSceneNode(spritenode);
							_avatar->setDebugColor(DEBUG_COLOR);
							// scene.addChild(sprite);
							scene.addObstacle(_avatar, spritenode);
						}
						else if (type == "boss") {
							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;
							Vec2 enemyPos = Vec2(colNum, rowPos);
							if (enemy_name == "bull") {
								CULog("We have correctly started loading the bull!");
								image = _assets->get<Texture>("bullIdle");
								std::shared_ptr<BullModel> _bull = BullModel::alloc(enemyPos, BULL_SIZE_DEFAULT, scene.getScale());
								std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 3, 4, 12);
								// spritenode->setAnchor(Vec2(0.5f, 0.43f));
								spritenode->setPosition(enemyPos);
								_bull->setSceneNode(spritenode);
								_bull->setName(BULL_TEXTURE);
								_bull->setDebugColor(DEBUG_COLOR);
								_bull->setassets(_assets);

								_bull->addActionAnimation("bullIdle", _assets->get<Texture>("bullIdle"), 3, 4, 12, 0.5f);
								_bull->addActionAnimation("bullAttack", _assets->get<Texture>("bullAttack"), 5, 5, 25, 2.0f);
								_bull->addActionAnimation("bullTelegraph", _assets->get<Texture>("bullTelegraph"), 3, 4, 10, 1.0f);
								_bull->addActionAnimation("bullRun", _assets->get<Texture>("bullRun"), 3, 3, 8, 1.0f);
								_bull->addActionAnimation("bullStunned", _assets->get<Texture>("bullStunned"), 3, 4, 12, 1.0f);
								_bull->addActionAnimation("bullStomp", _assets->get<Texture>("bullStomp"), 5, 5, 22, 2.0f);
								_bull->addActionAnimation("bullDazedtoIdle", _assets->get<Texture>("bullDazedtoIdle"), 2, 2, 4, 2.0f);
								_bull->addActionAnimation("bullCrash", _assets->get<Texture>("bullCrash"), 4, 4, 15, 3.0f);
								_bull->addActionAnimation("bullTurn", _assets->get<Texture>("bullTurn"), 4, 4, 16, 0.75f);

								scene.addObstacle(_bull, spritenode);

								scene.setAssets(_assets);
								scene.setScale(scene.getScale());
								scene.setBackground(_background);
								scene.setAvatar(_avatar);
								scene.setEnemies(_enemies);
								scene.setGoalDoor(_goalDoor);
								scene.setBull(_bull);
							}
						}
						// WE COMMENTED OUT ALL THIS FOR NOW BECAUSE JUST TRYING TO TEST MOVING BETWEEN LEVELS
			
						else if (type == "enemy") {
							CULog("Enemy layer spotted!");
							// sprite = scene2::PolygonNode::allocWithTexture(image);
							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;
							Vec2 enemyPos = Vec2(colNum, rowPos);
							std::shared_ptr<EnemyModel> new_enemy;
							std::shared_ptr<EntitySpriteNode> spritenode;

							//right now we are only going to be checking for rice,
							// because animations are not done yet for other enemies

							if (pathWeWant.find("rice") != std::string::npos) {
								scene.spawnRice(enemyPos, false);

								/*std::unordered_map<IngredientType, int> map = { {IngredientType::boiledEgg, 2}};
								scene.spawnPlate(enemyPos, map);*/
								numOfRice += 1;
							}

							else if (pathWeWant.find("carrot") != std::string::npos) {
								scene.spawnCarrot(enemyPos);

								/*image = _assets->get<Texture>("carrotIdle");
								spritenode = EntitySpriteNode::allocWithSheet(image, 1, 1, 1);
								Size singularSpriteSize = Size(image->getWidth(), image->getHeight());
								Size s = Size(2.25f, 6.0f);
								new_enemy = Egg::allocWithConstants(enemyPos, s, scene.getScale(), _assets);
								new_enemy->setSceneNode(spritenode);
								new_enemy->setDebugColor(DEBUG_COLOR);
								spritenode->setAnchor(0.5, 0.35);
								scene.addObstacle(new_enemy, spritenode);
								_enemies.push_back(new_enemy);*/
							}

							else if (pathWeWant.find("beef") != std::string::npos) {

								scene.spawnBeef(enemyPos);
								//image = _assets->get<Texture>("beefIdle");
								//spritenode = EntitySpriteNode::allocWithSheet(image, 3, 3, 7);
								//float imageWidth = image->getWidth() / 3;
								//float imageHeight = image->getHeight() / 3;
								//Size singularSpriteSize = Size(imageWidth, imageHeight);
								//// enemyPos.y -= 100.0f;
								//Size beefSize = cugl::Size(8.0f, 8.0f);
								//new_enemy = Beef::allocWithConstants(enemyPos, beefSize, scene.getScale(), _assets);
								//new_enemy->setSceneNode(spritenode);
								//new_enemy->setDebugColor(DEBUG_COLOR);
								//new_enemy->setLimit(cugl::Spline2(enemyPos, Vec2(enemyPos.x, enemyPos.y + 1.0)));
								//scene.addObstacle(new_enemy, spritenode);
								//_enemies.push_back(new_enemy);
							}
							else if (pathWeWant.find("egg") != std::string::npos) {
								scene.spawnEgg(enemyPos);
							}
							else if (pathWeWant.find("shrimp") != std::string::npos) {
								scene.spawnShrimp(enemyPos);
								//scene.spawnStation(enemyPos, StationType::BOIL);
							}
						}
						else {
							CULog("we're not reading anything what the heck man??");
						}
					}
				}
			}

			else if (objects != nullptr && objects->isArray()) {
				//we are looking at the object layer now
				CULog("we are now looking at the object layer");
				for (int i = 0; i < objects->size(); i++) {
					//here we are just initializing boundaries, not images or anything like that
					std::shared_ptr<JsonValue> object = objects->get(i);
					/*if (object->getString("name") == "avatar") {
						loadAvatar(object);
					}*/
					if (object->getString("name") == "Floating_Platform") {
						CULog("We are in loadFloatingBox!");
						CULog(object->getString("id").c_str());
						// loadFloatingBoxPlatform(object, scene, sprite, window_height * 32.0f);
						loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					else if (object->getString("name") == "Main_Platform") {
						CULog("We are in loadMainPlatform!");
						CULog(object->getString("id").c_str());
						mainPlatform = loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					else {
						CULog(object->getString("name").c_str());
						CULog("We not doing anything right now");
					}
				}
			}


		}
	}


	//hardcode :3

	Vec2 boilStation = p + Vec2(10.0f, 40.0f);
	Vec2 cutStation = p + Vec2(40.0f, 40.0f);
	Vec2 fryStation = p + Vec2(80.0f, 30.0f);
	Vec2 plate = p ;

	scene.spawnStation(boilStation, StationType::BOIL);
	scene.spawnStation(cutStation, StationType::CUT);
	scene.spawnStation(fryStation, StationType::FRY);
	std::unordered_map<IngredientType, int> map = { {IngredientType::boiledEgg, 2}, {IngredientType::boiledRice, 2}, {IngredientType::cookedBeef, 1} };
	scene.spawnPlate(plate, map);


	scene.setAssets(_assets);
	scene.setScale(scene.getScale());
	scene.setBackground(_background);
	scene.setAvatar(_avatar);
	//scene.setEnemies(_enemies);
	scene.setGoalDoor(_goalDoor);

	CULog("we have reached the end of populate in levelModel!!!!!!");
}

std::string LevelModel::getLevelScenery(std::string levelNumber) {
	return "textures\\dream-background.png";
}

void LevelModel::removeBackgroundImages(GameScene& scene) {
	for (std::shared_ptr<scene2::SceneNode > node: scene.getChildren()) {
		if (node->getName() == "background") {
			scene.removeChild(node);
		}
	}
}

std::shared_ptr<physics2::PolygonObstacle> LevelModel::loadMainPlatform(const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height) {

	float startingX = json->getFloat("x");
	float startingY = json->getFloat("y");

	if (startingX < 0.0f) {
		startingX = 0.0f;
	}
	if (startingY < 0.0f) {
		startingY = 0.0f;
	}

	/*startingX -= (210.0f / 40.0f);
	startingY += (210.0f / 40.0f);*/


	std::shared_ptr<cugl::JsonValue> platform_vertices = json->get("polygon");
	std::shared_ptr<physics2::PolygonObstacle> platobj;
	std::vector<Vec2> polygon_points = {};

	if (platform_vertices != nullptr && platform_vertices->isArray()) {
		for (int i = 0; i < platform_vertices->size(); i++) {
			//current x, y pairing
			std::shared_ptr<JsonValue> point = platform_vertices->get(i);
			// one possible issue: point->getFloat("y") also contains negative numbers,
			// so we may want to add a % window_height or something, but not sure
			float refactor_scale = 210.0f / 40.0f;
			polygon_points.push_back(Vec2(((startingX + point->getFloat("x"))) / 32.0f, ((level_height - (startingY + point->getFloat("y")))) / 32.0f));
			std::string value = "Point " + std::to_string(i);
			CULog(value.c_str());
			std::string x_print = "x: " + std::to_string((startingX + point->getFloat("x")) / 32.0f);
			std::string y_print = "y: " + std::to_string((level_height - (startingY + point->getFloat("y"))) / 32.0f);
			std::string x_refactored = "x refactored: " + std::to_string((startingX + point->getFloat("x")) * refactor_scale / 32.0f);
			std::string y_refactored = "y refactored: " + std::to_string((level_height - (startingY + point->getFloat("y"))) * refactor_scale / 32.0f);
			CULog(x_print.c_str());
			CULog(y_print.c_str());
			CULog(x_refactored.c_str());
			CULog(y_refactored.c_str());
			CULog("-------------------------");

		}
	}

	Poly2 platform(polygon_points);
	// platform *= (scene.getScale() / (210.0 / 40.0));
	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	// platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_BOTTOM_LEFT);
	platobj = physics2::PolygonObstacle::alloc(platform);
	platobj->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	// platobj->setPosition(5.0, 0.0);
	platobj->setName(std::string(PLATFORM_NAME));

	platobj->setBodyType(b2_staticBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);

	platform *= scene.getScale();

	// std::shared_ptr<cugl::Texture> text;
	// text->init();

	sprite = scene2::PolygonNode::allocWithTexture(nullptr, platform);
	sprite->setColor(Color4::CLEAR);
	sprite->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	scene.addObstacle(platobj, sprite, 1);

	return platobj;
	CULog("we reached the end of loadMainPlatform!!");

}

void LevelModel::loadFloatingBoxPlatform(const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height) {

	float startingX = json->getFloat("x");
	float startingY = json->getFloat("y");

	float height = json->getFloat("height");
	float width = json->getFloat("width");

	std::shared_ptr<physics2::PolygonObstacle> platobj;
	float scene_refactor = 210.0f / 40.0f;
	float DIMENSIONS[8] = { startingX / 32.0f, (level_height - startingY) / 32.0f, startingX / 32.0f, (level_height - startingY - height) / 32.0f, (startingX + width) / 32.0f, (level_height - startingY - height) / 32.0f, (startingX + width) / 32.0f, (level_height - startingY) / 32.0f };


	for (int i = 0; i < 8; i++) {
		// DIMENSIONS[i] = DIMENSIONS[i] * (2);
		std::string val = "Point " + std::to_string(i) + ": " + std::to_string(DIMENSIONS[i]);
		CULog(val.c_str());
	}
	CULog("Did we get here?");
	Poly2 platform(reinterpret_cast<Vec2*>(DIMENSIONS), sizeof(DIMENSIONS) / sizeof(float) / 2);

	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_BOTTOM_LEFT);
	// CULog(std::to_string(platobj->isRemoved()).c_str());
	platobj->setName(std::string(PLATFORM_NAME));

	platobj->setBodyType(b2_staticBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);

	platform *= scene.getScale();

	std::shared_ptr<Texture> image = _assets->get<Texture>("textures\\placeholder_block_PLATFORM.png");

	sprite = scene2::PolygonNode::allocWithTexture(nullptr, platform);
	sprite->setColor(Color4::CLEAR);
	sprite->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	scene.addObstacle(platobj, sprite, 1);
}
