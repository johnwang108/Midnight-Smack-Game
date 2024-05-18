
#include <cugl/assets/CUJsonLoader.h>
#include "LevelModel.h"
#include "../PFDudeModel.h"
#include "../PFGameScene.h"
#include "../NightLevelObjects/Wall.h"
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

	removeBackgroundImages(scene);

	std::shared_ptr<cugl::JsonReader> levelReader;
	//have to set the filepath before this

	levelReader = cugl::JsonReader::allocWithAsset(getFilePath());

	CULog(getFilePath().c_str());
	std::shared_ptr<JsonValue> level_json_file = levelReader->readJson();

	if (level_json_file->get("height") == nullptr) {
		CUAssertLog(false, "No level height");
	}
	if (level_json_file->get("width") == nullptr) {
		CUAssertLog(false, "No level width");
	}

	int window_height = level_json_file->getInt("height");
	// std::string window_height_string = json->getString("height");
	int window_width = level_json_file->getInt("width");


	if (level_json_file == nullptr) {
		CUAssertLog(false, "Failed to load level file");
	}
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
			CULog(tileSource.c_str());
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
						else if (type == "interactable") {

							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;
							Vec2 interactablePos;

							// knife, pan, plate, pot, sink

							if (pathWeWant.find("knife") != std::string::npos) {
								interactablePos = Vec2(colNum + ((169.0 / 32.0) / 2), rowPos - ((108.0 / 32.0)));

								scene.spawnStation(interactablePos, StationType::CUT);								
							}
							else if (pathWeWant.find("pan") != std::string::npos) {
								interactablePos = Vec2(colNum + ((148.0 / 32.0) / 2), rowPos - ((120.0 / 32.0)));

								scene.spawnStation(interactablePos, StationType::FRY);

							}
							else if (pathWeWant.find("pot") != std::string::npos) {
								interactablePos = Vec2(colNum + ((132.0 / 32.0) / 2), rowPos - ((105.0 / 32.0)));

								scene.spawnStation(interactablePos, StationType::BOIL);
							}

						}
						//else if (type == "golden_door") {
						//	CULog("identified class field as golden door");
						//	sprite = scene2::PolygonNode::allocWithTexture(image);
						//	// std::shared_ptr<scene2::WireNode> draw;
						//	// Create obstacle
						//	int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;
						//	Vec2 goalPos = Vec2(colNum, rowPos);
						//	Size goalSize(image->getSize().width / scene.getScale(),
						//		image->getSize().height / scene.getScale());
						//	_goalDoor = physics2::BoxObstacle::alloc(goalPos, goalSize);
						//	// _goalDoor->setPosition(0, 0);
						//	// Set the physics attributes
						//	_goalDoor->setBodyType(b2_staticBody);
						//	_goalDoor->setDensity(0.0f);
						//	_goalDoor->setFriction(0.0f);
						//	_goalDoor->setRestitution(0.0f);
						//	_goalDoor->setSensor(true);
						//	// _goalDoor->setEnabled

						//	// Add the scene graph nodes to this object
						//	_goalDoor->setDebugColor(DEBUG_COLOR);
						//	scene.addObstacle(_goalDoor, sprite);
						//}
						else if (type == "avatar") {
							CULog("identified class field as avatar");
							sprite = scene2::PolygonNode::allocWithTexture(image);
							// harcoded window_height to 25

							// float w = ( (window_height - rowNum) % window_height + window_height) % window_height;

							int rowPos = ((window_height - rowNum) % window_height + window_height) % window_height;

							// float scale_refactor = (210.0f / 40.0f);
							//WE WILL HAVE TO CHANGE THIS

							// Vec2 dudePos = Vec2(colNum + (3 * scale_refactor), rowPos + (3 * scale_refactor));
							Vec2 dudePos = Vec2(colNum + ((80.0 / 32.0) / 2), rowPos - ((160.0 / 32.0) / 2));

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
							if (_avatar == nullptr) {
								_avatar = DudeModel::allocWithConstants(dudePos, size, scene.getScale(), _assets);
								std::shared_ptr<EntitySpriteNode> spritenode = EntitySpriteNode::allocWithSheet(image, 4, 4, 16);
								spritenode->setAnchor(Vec2(0.5, 0.35));
								_avatar->setSceneNode(spritenode);
								_avatar->setDebugColor(DEBUG_COLOR);
								scene.addObstacle(_avatar, spritenode);
							}
							else {
								_avatar->reset();
								_avatar->setPosition(dudePos);
								scene.addObstacle(_avatar, _avatar->getSceneNode());
							}
							scene.setSpawn(dudePos);
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
								_bull->addActionAnimation("bullDeath", _assets->get<Texture>("bullDeath"), 5, 5, 22, 1.0f);
								_bull->addActionAnimation("bullD", _assets->get<Texture>("bullD"), 1, 1, 1, 1.0f);

								scene.addObstacle(_bull, spritenode);

								scene.setAssets(_assets);
								scene.setScale(scene.getScale());
								scene.setBackground(_background);
								scene.setAvatar(_avatar);
								scene.setEnemies(_enemies);
								scene.setGoalDoor(_goalDoor);
								scene.setBull(_bull);
							}
							else if (enemy_name == "sfr") {
								CULog("We have started loading in the shrimp boss");
								image = _assets->get<Texture>("SFR_Idle");
								
								std::shared_ptr<ShrimpRice> _shrimprice = ShrimpRice::alloc(enemyPos, SHR_SIZE_DEFAULT, scene.getScale());
								std::shared_ptr<EntitySpriteNode>  spritenode = EntitySpriteNode::allocWithSheet(image, 6, 6, 31);
								spritenode->setAnchor(Vec2(0.5f, 0.55f));
								//spritenode->setPosition(shrimp_pos);
								_shrimprice->setSceneNode(spritenode);
								_shrimprice->setName("shrimpBoss");
								_shrimprice->setDebugColor(DEBUG_COLOR);
								_shrimprice->setassets(_assets);

								_shrimprice->addActionAnimation("SFR_Idle", _assets->get<Texture>("SFR_Idle"), 6, 6, 31, 1.291f);
								_shrimprice->addActionAnimation("SFR_Move", _assets->get<Texture>("SFR_Move"), 4, 4, 14, 0.583f);
								_shrimprice->addActionAnimation("SFR_Attack", _assets->get<Texture>("SFR_Attack"), 6, 5, 28, 1.125f);
								_shrimprice->addActionAnimation("SFRJoustState1", _assets->get<Texture>("SFRJoustState1"), 4, 5, 18, 2.25f);
								_shrimprice->addActionAnimation("SFRJoustState2", _assets->get<Texture>("SFRJoustState2"), 3, 4, 12, 1.125f);
								_shrimprice->addActionAnimation("SFRJoustState3", _assets->get<Texture>("SFRJoustState3"), 2, 2, 3, 0.6f);
								_shrimprice->addActionAnimation("SFRWave1", _assets->get<Texture>("SFRWave1"), 3, 3, 8, 1.125f);
								_shrimprice->addActionAnimation("SFRWave2", _assets->get<Texture>("SFRWave2"), 3, 3, 7, 1.125f);
								_shrimprice->addActionAnimation("SFRWave3", _assets->get<Texture>("SFRWave3"), 4, 5, 18, 1.125f);
								_shrimprice->addActionAnimation("SFRWheelofDoom", _assets->get<Texture>("SFRWheelofDoom"), 3, 4, 12, 0.1f);
								_shrimprice->addActionAnimation("SFRMoveState1", _assets->get<Texture>("SFRMoveState1"), 4, 4, 14, 1.0f);
								_shrimprice->addActionAnimation("SFRMoveState2", _assets->get<Texture>("SFRMoveState2"), 3, 4, 11, 0.7f);
								_shrimprice->addActionAnimation("SFRTurn", _assets->get<Texture>("SFRTurn"), 3, 4, 11, 0.61f);
								_shrimprice->addActionAnimation("SFRStunState2", _assets->get<Texture>("SFRStunState2"), 3, 3, 7, 0.46f);
								_shrimprice->addActionAnimation("SFRStunState1", _assets->get<Texture>("SFRStunState1"), 3, 3, 7, 0.6f);
								_shrimprice->addActionAnimation("SFR_Death", _assets->get<Texture>("SFR_Death"), 5, 6, 27, 1.5f);
								_shrimprice->addActionAnimation("SFR_D", _assets->get<Texture>("SFR_D"), 1, 1, 1, 1.0f);

								scene.addObstacle(_shrimprice, spritenode);

								scene.setAssets(_assets);
								scene.setScale(scene.getScale());
								scene.setBackground(_background);
								scene.setAvatar(_avatar);
								scene.setEnemies(_enemies);
								scene.setGoalDoor(_goalDoor);
								scene.setShrimpRice(_shrimprice);



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
								numOfRice += 1;
							}

							else if (pathWeWant.find("carrot") != std::string::npos) {
								scene.spawnCarrot(enemyPos);
							}

							else if (pathWeWant.find("beef") != std::string::npos) {
								scene.spawnBeef(enemyPos);
							}
							else if (pathWeWant.find("egg") != std::string::npos) {
								scene.spawnEgg(enemyPos);
							}
							else if (pathWeWant.find("shrimp") != std::string::npos) {
								CULog("shwimp");
								scene.spawnShrimp(enemyPos);
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
						loadMainPlatform(object, scene, sprite, window_height * 32.0f);
						// loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					if (object->getString("name") == "Floating_Breakable_Platform") {
						CULog("We are in loadBreakablePlatform!");
						CULog(object->getString("id").c_str());
						loadBreakablePlatform(nullptr, object, scene, sprite, window_height * 32.0f);
						// loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					else if (object->getString("name") == "Main_Platform") {
						CULog("We are in loadMainPlatform!");
						CULog(object->getString("id").c_str());
						mainPlatform = loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					else if (object->getString("name") == "Wall") {
						CULog("We are in loadWall!");
						CULog(object->getString("id").c_str());
						mainPlatform = loadMainPlatform(object, scene, sprite, window_height * 32.0f);
					}
					else if (object->getString("name") == "Damaging_Platform") {
						//fill in later
						CULog("We are in damageablePlatform");
						loadDamagingPlatform(nullptr, object, scene, sprite, window_height * 32.0f);

					}
					else if (object->getString("name") == "Moving_Platform") {
						// fill in later
						// Here, we gonna have to set up wall while not letting it be a breakable platform
						CULog("We are in loadMovingPlatform!");
						CULog(object->getString("id").c_str());
						loadMovingPlatform(nullptr, object, scene, sprite, window_height * 32.0f);
					}
					else if (object->getString("name") == "Plate") {
						CULog("we are loading in a plate station");
						std::shared_ptr<JsonValue> plateProperties = object->get("properties");
						std::unordered_map<IngredientType, int> map = {};
						if (plateProperties != nullptr && plateProperties->isArray()) {
							for (int j = 0; j < plateProperties->size(); j++) {
								std::shared_ptr<JsonValue> ingredientSpec = plateProperties->get(j);
								for (auto& child : ingredientSpec->children()) {
									CULog(child->asString().c_str());
								}
								// egg, rice, carrot, beef, shrimp
								if (ingredientSpec->getString("name") == "cutCarrot") {
									map.emplace(IngredientType::cutCarrot, std::stoi(ingredientSpec->getString("value")));
									CULog("we added cutCarrot, no errors here");
								}
								else if (ingredientSpec->getString("name") == "fryEgg") {
									map.emplace(IngredientType::scrambledEgg, std::stoi(ingredientSpec->getString("value")));
								}
								else if (ingredientSpec->getString("name") == "boilEgg") {
									map.emplace(IngredientType::boiledEgg, std::stoi(ingredientSpec->getString("value")));
								}
								else if (ingredientSpec->getString("name") == "boilRice") {
									map.emplace(IngredientType::boiledRice, std::stoi(ingredientSpec->getString("value")));
								}
								else if (ingredientSpec->getString("name") == "fryBeef") {
									map.emplace(IngredientType::cookedBeef, std::stoi(ingredientSpec->getString("value")));
								}
								else if (ingredientSpec->getString("name") == "fryShrimp") {
									map.emplace(IngredientType::cookedShrimp, std::stoi(ingredientSpec->getString("value")));
								}
							}
						}
						CULog("now, we are going to spawn the plate into the scene");
						scene.spawnPlate(Vec2(object->getFloat("x") / 32.0f, level_height - (object->getFloat("y") / 32.0f)), map);
					}
					else if (object->getString("name") == "Popup") {
						CULog("we are loading in a popup");
						std::shared_ptr<JsonValue> propsproperties = object->get("properties");
						std::string popupName;
						for (int j = 0; j < propsproperties->size(); j++) {
							std::shared_ptr<JsonValue> prop = propsproperties->get(j);
							CULog(prop->getString("value").c_str());
							popupName = "tutorialPopup" + prop->getString("value");
						}
						scene.spawnTutorialSign(Vec2(object->getFloat("x") / 32.0f, level_height - (object->getFloat("y") / 32.0f)), popupName);
					}
					else {
						CULog(object->getString("name").c_str());
						CULog("We not doing anything right now");
					}
				}
			}


		}
	}


	// Use this as future reference for plate later
	/*std::unordered_map<IngredientType, int> map = { {IngredientType::boiledEgg, 2}, {IngredientType::boiledRice, 2}, {IngredientType::cookedBeef, 1} };
	scene.spawnPlate(plate, map);*/

	//hardcode :3
	/*if (getFilePath() == "json/intermediate.json") {
		Vec2 boilStation = p + Vec2(10.0f, 40.0f);
		Vec2 cutStation = p + Vec2(40.0f, 40.0f);
		Vec2 fryStation = p + Vec2(80.0f, 30.0f);
		Vec2 plate = p;

		scene.spawnStation(boilStation, StationType::BOIL);
		scene.spawnStation(cutStation, StationType::CUT);
		scene.spawnStation(fryStation, StationType::FRY);
		std::unordered_map<IngredientType, int> map = { {IngredientType::boiledEgg, 2}, {IngredientType::boiledRice, 2}, {IngredientType::cookedBeef, 1} };
		scene.spawnPlate(plate, map);
	}*/


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
	for (std::shared_ptr<scene2::SceneNode > node : scene.getChildren()) {
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
	std::shared_ptr<Platform> platobj;
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
	platobj = Platform::alloc(platform);
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

	scene.getPlatform().push_back(platobj);

	return platobj;
	CULog("we reached the end of loadMainPlatform!!");

}

//loading from wall class where getName() is BREAKABLE_PLATFORM_NAME
void LevelModel::loadBreakablePlatform(const std::shared_ptr<Texture> image, const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height) {

	float startingX = json->getFloat("x");
	float startingY = json->getFloat("y");

	float height = json->getFloat("height");
	float width = json->getFloat("width");

	std::shared_ptr<Wall> platobj;

	std::shared_ptr<cugl::JsonValue> platform_vertices = json->get("polygon");
	std::vector<Vec2> polygon_points = {};

	int numOfVerts = 0;

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
			numOfVerts++;

		}
	}


	Poly2 platform(polygon_points);

	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	// we gotta make sure that a proper image is actually being placed here

	// is this Vec2 pointer rly necessary or are we just creating additional memory
	Vec2 startingPos = Vec2(startingX, startingY);

	// we set image to nullptr because we should not have to worry about that when creating the colliders

	std::shared_ptr<physics2::PolygonObstacle> _obj = physics2::PolygonObstacle::alloc(platform);
	platobj = Wall::alloc(nullptr, _obj, scene.getScale(), BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION,
		DEBUG_COLOR, startingPos, numOfVerts, BREAKABLE_PLATFORM_NAME, false);
	platobj->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

	platobj->initBreakable(50,25);

	// CULog(std::to_string(platobj->isRemoved()).c_str());
	platobj->setName(std::string(BREAKABLE_PLATFORM_NAME));
	// platobj->setEnabled(false);
	// platobj->setSensor(true);
	platobj->setBodyType(b2_staticBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);
	platobj->setReadyToBeReset(false);

	platform *= scene.getScale();

	std::string msg = "Starting flag status of breakable platform: " + std::to_string(platobj->isFlagged());
	CULog(msg.c_str());

	// std::shared_ptr<Texture> image = _assets->get<Texture>("textures\\placeholder_block_PLATFORM.png");
	sprite = scene2::PolygonNode::allocWithTexture(nullptr, platform);
	sprite->setColor(Color4::CLEAR);
	sprite->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	scene.addObstacle(platobj, sprite, 1);
	_breakable_platforms.push_back(platobj);
}

//loading from wall class where getName() is MOVING_PLATFORM_NAME
void LevelModel::loadMovingPlatform(const std::shared_ptr<Texture> image, const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height) {

	float startingX = json->getFloat("x");
	float startingY = json->getFloat("y");

	float height = json->getFloat("height");
	float width = json->getFloat("width");

	std::shared_ptr<Wall> platobj;

	std::shared_ptr<cugl::JsonValue> platform_vertices = json->get("polygon");
	std::vector<Vec2> polygon_points = {};

	int numOfVerts = 0;

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
			numOfVerts++;

		}
	}


	Poly2 platform(polygon_points);

	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	Vec2 startingPos = Vec2(startingX, startingY);

	std::shared_ptr<physics2::PolygonObstacle> _obj = physics2::PolygonObstacle::alloc(platform);

	platobj = Wall::alloc(nullptr, _obj, scene.getScale(), BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION,
		DEBUG_COLOR, startingPos, numOfVerts, PLATFORM_NAME, false);
	// platobj->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	CULog("setting the path of our moving platform");
	CULog("we need to iterate through the property values");

	std::shared_ptr<JsonValue> vectorVals = json->get("properties");
	std::vector<Vec3> path = {};
	int movement_force = 0;
	if (vectorVals != nullptr && vectorVals->isArray()) {
		for (int j = 0; j < vectorVals->size(); j++) {

			std::shared_ptr<JsonValue> vecVal = vectorVals->get(j);


			if (vecVal->get("name") != nullptr && vecVal->getString("name") == "movement_force") {
				movement_force = vecVal->getInt("value");
			}

			else {
				std::string nestedList = vecVal->getString("value");
				std::string delimiter = ",";
				// nestedList should be {2,5,4,6}
				Vec3 new_vec = Vec3(0, 0, 0);
				size_t pos = 0;
				std::string token;

				int counter = 0;
				while ((pos = nestedList.find(delimiter)) != std::string::npos) {
					token = nestedList.substr(0, pos);
					if (counter == 0) {
						new_vec.x = std::atof(token.c_str());
					}
					else if (counter == 1) {
						new_vec.y = std::atof(token.c_str());
					}
					else {
						CULog("we should never actually get here");
					}
					nestedList.erase(0, pos + delimiter.length());
					counter++;
				}
				CULog("counter value should be 2 here:");
				CULog(std::to_string(counter).c_str());
				new_vec.z = std::atof(nestedList.c_str());
				// after we parse through our nestedList, we can push to path vector
				path.push_back(new_vec);
			}
		}
	}

	platobj->initPath(path, movement_force);

	// Here, we can just use PLATFORM_NAME because we do not need to change the collision properties
	// of a moving platform
	platobj->setName(std::string(PLATFORM_NAME));
	// platobj->setEnabled(false);
	// platobj->setSensor(true);
	platobj->setBodyType(b2_kinematicBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);
	platobj->setReadyToBeReset(false);

	platform *= scene.getScale();
	// adding placeholder image to use over moving platform
	// std::shared_ptr<Texture> image = _assets->get<Texture>("textures\\placeholder_block_PLATFORM.png");
	sprite = scene2::PolygonNode::allocWithTexture(nullptr, platform);
	sprite->setColor(Color4::CLEAR);
	sprite->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	scene.addObstacle(platobj, sprite, 1);
	_moving_platforms.push_back(platobj);

}

//loading from wall class where getName() is MOVING_PLATFORM_NAME
void LevelModel::loadDamagingPlatform(const std::shared_ptr<Texture> image, const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height) {

	float startingX = json->getFloat("x");
	float startingY = json->getFloat("y");

	float height = json->getFloat("height");
	float width = json->getFloat("width");

	std::shared_ptr<Wall> platobj;

	std::shared_ptr<cugl::JsonValue> platform_vertices = json->get("polygon");
	std::vector<Vec2> polygon_points = {};

	int numOfVerts = 0;

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
			numOfVerts++;

		}
	}


	Poly2 platform(polygon_points);

	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	// we gotta make sure that a proper image is actually being placed here

	// is this Vec2 pointer rly necessary or are we just creating additional memory
    Vec2 startingPos = Vec2(startingX, startingY);
	// we set image to nullptr because we should not have to worry about that when creating the colliders

	std::shared_ptr<physics2::PolygonObstacle> _obj = physics2::PolygonObstacle::alloc(platform);
	platobj = Wall::alloc(nullptr, _obj, scene.getScale(), BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION,
		DEBUG_COLOR, startingPos, numOfVerts, DAMAGING_PLATFORM_NAME, true);
	platobj->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

	platobj->initBreakable(50, 25);

	// CULog(std::to_string(platobj->isRemoved()).c_str());
	platobj->setName(std::string(DAMAGING_PLATFORM_NAME));
	// platobj->setEnabled(false);
	// platobj->setSensor(true);
	platobj->setBodyType(b2_staticBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);
	platobj->setReadyToBeReset(false);

	platform *= scene.getScale();

	// std::shared_ptr<Texture> image = _assets->get<Texture>("textures\\placeholder_block_PLATFORM.png");
	sprite = scene2::PolygonNode::allocWithTexture(nullptr, platform);
	sprite->setColor(Color4::CLEAR);
	sprite->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	scene.addObstacle(platobj, sprite, 1);
	CULog("we reached the end of damaging platform");

}