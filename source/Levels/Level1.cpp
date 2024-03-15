#include "Level1.h"
#include "../PFGameScene.h"
#include <direct.h>
// #include <tinyxml2.h> this is external library


/** The goal door position */
static float GOAL_POS[] = { 4.0f,14.0f };
// float GOAL_POS[] = { 6.0f, 5.0f };
/** The position of the spinning barrier */
static float SPIN_POS[] = { 13.0f,12.5f };
/** The initial position of the dude */
static float DUDE_POS[] = { 2.5f, 5.0f };
/** The position of the rope bridge */
static float BRIDGE_POS[] = { 9.0f, 3.8f };

static float SHRIMP_POS[] = { 22.0f, 16.0f };

static float EGG_POS[] = { 14.0f, 18.0f };

static float RICE_POS[] = { 25.0f, 14.0f };

static float BACKGROUND_POS[] = { 16.0f, 10.0f };


static float WALL[WALL_COUNT][WALL_VERTS] = {
	{16.0f, 20.0f,  0.0f, 20.0f,  0.0f,  0.0f,
	  1.0f,  0.0f,  1.0f, 19.5f, 16.0f, 19.5f },
	{32.0f, 20.0f, 16.0f, 20.0f, 16.0f, 19.5f,
	 31.0f, 19.5f, 31.0f,  0.0f, 32.0f,  0.0f }
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

static float PLATFORMS[PLATFORM_COUNT][PLATFORM_VERTS] = {
	{1.0f, 4.0f, 1.0f, 2.0f, 4.0f, 2.0f, 4.0f, 4.0f},
	{ 6.0f, 4.0f, 6.0f, 2.5f, 9.0f, 2.5f, 9.0f, 4.0f},
	{9.5f, 6.0f, 9.5f, 5.0f, 12.5f, 5.0f, 12.5f, 6.0f},
	{15.0f, 8.5f, 15.0f, 7.0f, 20.0f, 4.5f, 20.0f, 6.0f},
	{23.0f, 4.0f, 23.0f, 3.0f, 27.0f, 3.0f, 27.0f, 4.0f},
	{28.0f, 5.0f, 28.0f, 4.0f, 30.0f, 8.0f, 30.0f, 9.0f},
	{23.0f, 10.0f, 23.0f, 9.f, 27.0f, 9.f, 27.0f, 10.f},
	{16.0f, 12.f, 16.0f, 10.0f, 22.0f, 12.0f, 22.0f, 10.f},
	{6.0f, 15.0f, 6.0f, 14.5f, 14.0f, 12.5f, 14.0f, 13.0f},
	{ 1.0f,12.5f, 1.0f,12.0f, 7.0f,12.0f, 7.0f,12.5f}
};

/** The outlines of all of the platforms */
static float ALT_PLATFORMS[ALT_PLATFORM_COUNT][ALT_PLATFORM_VERTS] = {
	{ 1.0f, .5f, 1.0f, .0f, 33.0f, .0f, 33.0f, .50f}
};

std::shared_ptr<Texture> image;
std::shared_ptr<scene2::PolygonNode> sprite;
std::shared_ptr<physics2::PolygonObstacle> platobj;
int window_height;
int tile_height = 32;
int tile_width = 32;

void Level1::parseJson(const std::string &json, std::shared_ptr<AssetManager> &assets, GameScene& scene) {

	std::shared_ptr<cugl::JsonValue> jsonValue = cugl::JsonValue::allocWithJson(json);
	
	std::map<int, std::string> idToImage = {};

	std::shared_ptr<cugl::JsonReader> jsonReader;

	// this is to fetch the layers object -- jsonValue->get("layers");
	
	// screen height and width will always be 1280 x 800
	// this is equivalent to 40 by 25 in terms of 32 square
	// pixel tiles
	// the width 

	window_height = jsonValue->getInt("height");
	int width = jsonValue->getInt("width");

	std::shared_ptr<cugl::JsonValue> tilesets = jsonValue->get("tilesets");
	
	if (tilesets != nullptr && tilesets->isArray()) {
		for (int i = 0; i < tilesets->size(); i++) {
			std::shared_ptr<cugl::JsonValue> tile  = tilesets->get(i);
			int tileId = tile->getInt("firstgid");
			// this gets out egg_64.tsj for example, but its a relative path!!!!! In reality, egg_64.tsj exists in my local documents folder.
			// So, we got to move all the created json tilesets into our assets file
			// STILL HAVE TO DO

			// this grid is going to be important, as we will need to tell what pixel
			// is associated with what image (STILL HAVE TO DO)


			// it should be harder than just this
			std::string tileSource = tile->getString("source");
			
			tileSource.insert(0, "\\json\\");
			tileSource.insert(0, _getcwd(NULL, 0));
			// creates a reader that reads the tileset json in a level,
			// where we will get the image 
			// now, our tileSource string will be /../../assets/json/egg_64_enemy.tsj or something
			// So, every tileSet will needed to be added into the assets/json folder as well :(

			CULog(_getcwd(NULL, 0));

			jsonReader = cugl::JsonReader::alloc(tileSource);
			std::string absPath = "";
			CULog(tileSource.c_str());
			CULog("WE GOT HERE WITHOUT AN ERROR!");
			// jsonReader->readJsonString() != "image"

			// THIS IS CURRENTLY NOT WORKING, RIGHT NOW EXTERNAL JSON THING NOT BEING READ
			while (jsonReader->readJsonString() != "image") {
				CULog(jsonReader->readJsonString().c_str());
				continue;
			}
			CULog("Did we ever get here?");
			// absPath will now be the string RIGHT AFTER our image property
			// in our tileSet json
			absPath = jsonReader->readJsonString();

			// std::string absPath = tileSource->getString("image");
			size_t startPos = absPath.find("/textures");

			if (startPos == std::string::npos) {
				startPos = 0;
			}
			std::string imgPath = absPath.substr(startPos);
			std::replace(imgPath.begin(), imgPath.end(), '\/', '/');
			idToImage.emplace(tileId, imgPath);

			// for our example, idToImage will look like:
			// {1:'background-image-1', 705: 'blocky-landscape', 713: 'goaldoor', etc.}

			// now, we have the image and can initialize as done
			// in the populate method
		}
	}

	std::shared_ptr<cugl::JsonValue> layers = jsonValue->get("layers");
	int rowNum = 0;
	int colNum = 0;
	int platformNumber = 0;
	int borderNumber = 0;
	if (layers != nullptr && layers->isArray()) {
		for (int i = 0; i < layers->size(); i++) {
			std::shared_ptr<cugl::JsonValue> layer = layers->get(i);
			// for each layer in layers, we must get its data array
			std::shared_ptr<cugl::JsonValue> data = layer->get("data");
			std::shared_ptr<scene2::PolygonNode> sprite;

			if (i == 0) { // this will always be the background level
				// background id will always be 1
				// in theory, this should set up the background
				std::string pathWeWant = idToImage.at(1);
				assets->load<Texture>(pathWeWant, pathWeWant);
				image = assets->get<Texture>(pathWeWant);
				sprite = scene2::PolygonNode::allocWithTexture(image);
				Size background_size(image->getSize().width / _scale, image->getSize().height / _scale);
				_background = physics2::BoxObstacle::alloc(Vec2(0,window_height), background_size);
				_background->setName(BACKGROUND_NAME);
				_background->setBodyType(b2_staticBody);
				_background->setDensity(0.0f);
				_background->setFriction(0.0f);
				_background->setRestitution(0.0f);
				_background->setEnabled(false);
				_background->setSensor(true);
				scene.addObstacle(_background, sprite);
				// breaks as we don't want to do anything else in this case
				continue;
			}
			// in this example, our display size is 1280 by 800, where the width is 40 tiles and the height is 25
			// so, there will 40 columns and 25 rows

			// rowNum, colNum now will represent gridMapping of tiles
			// now, we have to iterate through data array and see what our 
			// values are there

			//in our level_designer, there will be enums of possible things to add in
			// WALL, PLATFORM, BACKGROUND, DUDE, ENEMY, AND OTHERS THAT ARISE.
			// Each of our images in assets/textures will thus end in _BACKGROUND, or _ENEMY, or etc.

			if (data != nullptr && data->isArray()) {
				for (int j = 0; j < data->size(); j++) {
					colNum = j % width;
					if (colNum == 0 && j != 0) {
						rowNum += 1;
					}
					int tileId = data->get(j)->asInt();

					if (idToImage.find(tileId) != idToImage.end()) {
						std::string value = idToImage.at(tileId);

						//checking what type exists here
						if (value.find("_ENEMY.tsj") != std::string::npos) {
							createEnemy(value, assets, scene, rowNum, colNum);
						}
						else if (value.find("_GOAL_DOOR.tsj") != std::string::npos) {
							createGoalDoor(value, assets, scene, rowNum, colNum);
						}
						else if (value.find("_DUDE.tsj") != std::string::npos) {
							createDude(value, assets, scene, rowNum, colNum);
						}
						else if (value.find("_BORDER.tsj") != std::string::npos) {
							borderNumber = borderNumber + 1;
							createPlatform(value, assets, scene, rowNum, colNum, borderNumber);
							
						}
						else if (value.find("_PLATFORM.tsj") != std::string::npos) {
							platformNumber = platformNumber + 1;
							createPlatform(value, assets, scene, rowNum, colNum, platformNumber);
						}
						// WE WILL NEED TO ADD MORE?

					}


				}
			}

		}
	}


}

//COME BACK TO THIS ONE
//void Level1::createBorder(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int borderNumber) {
//
//	assets->load<Texture>(path, path);
//	image = assets->get<Texture>(path);
//
//	std::string wname = "border";
//	//for (int ii = 0; ii < WALL_COUNT; ii++) {
//	//	std::shared_ptr<physics2::PolygonObstacle> wallobj;
//
//	//	Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
//	//	// Call this on a polygon to get a solid shape
//	//	EarclipTriangulator triangulator;
//	//	triangulator.set(wall.vertices);
//	//	triangulator.calculate();
//	//	wall.setIndices(triangulator.getTriangulation());
//	//	triangulator.clear();
//
//	//	wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
//	//	// You cannot add constant "".  Must stringify
//	//	wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
//	//	wallobj->setName(wname);
//
//	//	// Set the physics attributes
//	//	wallobj->setBodyType(b2_staticBody);
//	//	wallobj->setDensity(BASIC_DENSITY);
//	//	wallobj->setFriction(BASIC_FRICTION);
//	//	wallobj->setRestitution(BASIC_RESTITUTION);
//	//	wallobj->setDebugColor(DEBUG_COLOR);
//
//	//	wall *= _scale;
//	//	sprite = scene2::PolygonNode::allocWithTexture(image, wall);
//	//	scene.addObstacle(wallobj, sprite, 1);  // All walls share the same texture
//	//}
//}


void Level1::createPlatform(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int platformNumber) {


	float x = float(col * tile_width);
	float y = float(window_height - (row * tile_height));

	float platform_arr[8] = {x, y, x, y - 32.0f, x + 32.0f, y - 32.0f, x + 32.0f, y};

	Poly2 platform(reinterpret_cast<Vec2*>(platform_arr), sizeof(platform_arr) / sizeof(float) / 2);

	EarclipTriangulator triangulator;
	triangulator.set(platform.vertices);
	triangulator.calculate();
	platform.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_TOP_LEFT);
	platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(platformNumber));

	platobj->setBodyType(b2_staticBody);
	platobj->setDensity(BASIC_DENSITY);
	platobj->setFriction(BASIC_FRICTION);
	platobj->setRestitution(BASIC_RESTITUTION);
	platobj->setDebugColor(DEBUG_COLOR);

	platform *= _scale;
	sprite = scene2::PolygonNode::allocWithTexture(image, platform);
	scene.addObstacle(platobj, sprite, 1);

}

void Level1::createDude(std::string path, std::shared_ptr<AssetManager>& assets,
	GameScene& scene, int row, int col) {

	assets->load<Texture>(path, path);
	image = assets->get<Texture>(path);

	Vec2 dudePos = Vec2(col * tile_width, window_height - (row * tile_height));;
	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_avatar->setSceneNode(sprite);
	_avatar->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_avatar, sprite);

}

void Level1::createGoalDoor(std::string path, std::shared_ptr<AssetManager>& assets,
	GameScene& scene, int row, int col) {

	assets->load<Texture>(path, path);
	std::shared_ptr<Texture> image = assets->get<Texture>(path);

	sprite = scene2::PolygonNode::allocWithTexture(image);
	// std::shared_ptr<scene2::WireNode> draw;

	// Create obstacle
	Vec2 goalPos = Vec2(col * tile_width, window_height - (row * tile_height));
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



}

void Level1::createEnemy(std::string path, std::shared_ptr<AssetManager>& assets,
	GameScene& scene, int row, int col) {

	EnemyType enemyType;

	if (path.find("egg") != std::string::npos) {
		enemyType = EnemyType::egg;
	}
	else if (path.find("shrimp") != std::string::npos) {
		enemyType = EnemyType::shrimp;
	}
	else if (path.find("rice") != std::string::npos) {
		enemyType = EnemyType::rice;
	}

	assets->load<Texture>(path, path);
	std::shared_ptr<Texture> image = assets->get<Texture>(path);
	// 32 is hardcoded in as both the tile width and height
	Vec2 enemy_pos = Vec2(col * tile_width, window_height - (row * tile_height));

	std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc(enemy_pos, image->getSize() / _scale, _scale, enemyType);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);

}

void Level1::populate(GameScene& scene) {

	
	std::string jsonStr = R"({ "compressionlevel":-1,
 "height":18,
 "infinite":false,
 "layers":[
        {
         "data":[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
            65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
            97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
            129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
            161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
            193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
            225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256,
            257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288,
            289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320,
            321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352,
            353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384,
            385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416,
            417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448,
            449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480,
            481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512,
            513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544,
            545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575, 576],
         "height":18,
         "id":1,
         "name":"Tile Layer 1",
         "opacity":1,
         "type":"tilelayer",
         "visible":true,
         "width":32,
         "x":0,
         "y":0
        },
        {
         "data":[705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 713, 714, 715, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 716, 717, 718, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 778, 779, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 719, 720, 721, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 780, 781, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 705, 705, 705, 705, 705, 0, 0, 0, 0, 0, 705, 705, 705, 705, 705, 705, 705, 0, 783, 784, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 705, 705, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 785, 786, 0, 705, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 0, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 0, 0, 0, 0, 787, 788, 0, 0, 0, 705, 705, 705, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 705, 0, 0, 0, 0, 0, 789, 790, 0, 0, 0, 0, 705, 705, 0, 0, 705,
            705, 0, 782, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 0, 0, 0, 0, 0, 0, 0, 791, 792, 0, 705, 0, 0, 0, 0, 0, 0, 705,
            705, 705, 705, 705, 0, 0, 705, 705, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 705, 705, 0, 0, 0, 0, 0, 0, 705,
            705, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705, 705, 705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 705,
            705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705, 705],
         "height":18,
         "id":2,
         "name":"Tile Layer 2",
         "opacity":1,
         "type":"tilelayer",
         "visible":true,
         "width":32,
         "x":0,
         "y":0
        }],
 "nextlayerid":3,
 "nextobjectid":1,
 "orientation":"orthogonal",
 "renderorder":"right-down",
 "tiledversion":"1.10.2",
 "tileheight":32,
 "tilesets":[
        {
         "firstgid":1,
         "source": "background-image-1.json"
        },
        {
         "firstgid":705,
         "source": "placeholder_block_PLATFORM.tsj"
        },
        {
         "firstgid":713,
         "source":"goaldoor_GOAL_DOOR.tsj"
        },
        {
         "firstgid":778,
         "source": "egg_64_ENEMY.tsj"
        },
        {
         "firstgid":782,
         "source": "dude_32_DUDE.tsj"
        },
        {
         "firstgid":783,
         "source": "rice_64_ENEMY.tsj"
        },
        {
         "firstgid":787,
         "source": "shrimp_96_ENEMY.tsj"
        }],
 "tilewidth":32,
 "type":"map",
 "version":"1.10",
 "width":32
})";


	_assets = scene.getAssets();
	_scale = scene.getScale();
	_background = scene.getBackground();
	_avatar = scene.getAvatar();
	_enemies = scene.getEnemies();
	_goalDoor = scene.getGoalDoor();
	// "" needs to be replaced with json file of the 
	parseJson(jsonStr, _assets, scene);

//# pragma mark: Background
//	Vec2 background_pos = BACKGROUND_POS;
//	std::shared_ptr<Texture> image = _assets->get<Texture>("background-1");
//	std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
//	Size background_size(image->getSize().width / _scale, image->getSize().height / _scale);
//	_background = physics2::BoxObstacle::alloc(background_pos, background_size);
//	_background->setName(BACKGROUND_NAME);
//	_background->setBodyType(b2_staticBody);
//	_background->setDensity(0.0f);
//	_background->setFriction(0.0f);
//	_background->setRestitution(0.0f);
//	_background->setEnabled(false);
//	_background->setSensor(true);
//	scene.addObstacle(_background, sprite);


//#pragma mark : Goal door
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
//	// _goalDoor->setEnabled
//
//	// Add the scene graph nodes to this object
//	sprite = scene2::PolygonNode::allocWithTexture(image);
//	_goalDoor->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_goalDoor, sprite);

//#pragma mark : Walls
//	// All walls and platforms share the same texture
//	image = _assets->get<Texture>(EARTH_TEXTURE);
//	std::string wname = "wall";
//	for (int ii = 0; ii < WALL_COUNT; ii++) {
//		std::shared_ptr<physics2::PolygonObstacle> wallobj;
//
//		Poly2 wall(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);
//		// Call this on a polygon to get a solid shape
//		EarclipTriangulator triangulator;
//		triangulator.set(wall.vertices);
//		triangulator.calculate();
//		wall.setIndices(triangulator.getTriangulation());
//		triangulator.clear();
//
//		wallobj = physics2::PolygonObstacle::allocWithAnchor(wall, Vec2::ANCHOR_CENTER);
//		// You cannot add constant "".  Must stringify
//		wallobj->setName(std::string(WALL_NAME) + cugl::strtool::to_string(ii));
//		wallobj->setName(wname);
//
//		// Set the physics attributes
//		wallobj->setBodyType(b2_staticBody);
//		wallobj->setDensity(BASIC_DENSITY);
//		wallobj->setFriction(BASIC_FRICTION);
//		wallobj->setRestitution(BASIC_RESTITUTION);
//		wallobj->setDebugColor(DEBUG_COLOR);
//
//		wall *= _scale;
//		sprite = scene2::PolygonNode::allocWithTexture(image, wall);
//		scene.addObstacle(wallobj, sprite, 1);  // All walls share the same texture
//	}
//
//#pragma mark : Platforms
//	for (int ii = 0; ii < PLATFORM_COUNT; ii++) {
//		std::shared_ptr<physics2::PolygonObstacle> platobj;
//		Poly2 platform(reinterpret_cast<Vec2*>(PLATFORMS[ii]), sizeof(PLATFORMS[ii]) / sizeof(float) / 2);
//
//		EarclipTriangulator triangulator;
//		triangulator.set(platform.vertices);
//		triangulator.calculate();
//		platform.setIndices(triangulator.getTriangulation());
//		triangulator.clear();
//
//		platobj = physics2::PolygonObstacle::allocWithAnchor(platform, Vec2::ANCHOR_CENTER);
//		// You cannot add constant "".  Must stringify
//		platobj->setName(std::string(PLATFORM_NAME) + cugl::strtool::to_string(ii));
//
//		// Set the physics attributes
//		platobj->setBodyType(b2_staticBody);
//		platobj->setDensity(BASIC_DENSITY);
//		platobj->setFriction(BASIC_FRICTION);
//		platobj->setRestitution(BASIC_RESTITUTION);
//		platobj->setDebugColor(DEBUG_COLOR);
//
//		platform *= _scale;
//		sprite = scene2::PolygonNode::allocWithTexture(image, platform);
//		scene.addObstacle(platobj, sprite, 1);
//	}

//#pragma mark : Dude
//	Vec2 dudePos = DUDE_POS;
//	// node = scene2::SceneNode::alloc();
//	image = _assets->get<Texture>(DUDE_TEXTURE);
//	_avatar = DudeModel::alloc(dudePos, image->getSize() / (2 + _scale), _scale);
//	sprite = scene2::PolygonNode::allocWithTexture(image);
//	_avatar->setSceneNode(sprite);
//	_avatar->setDebugColor(DEBUG_COLOR);
//	scene.addObstacle(_avatar, sprite); // Put this at the very front
//
//	// Play the background music on a loop.
//	std::shared_ptr<Sound> source = _assets->get<Sound>(GAME_MUSIC);
//	AudioEngine::get()->getMusicQueue()->play(source, true, MUSIC_VOLUME);


	/*Vec2 shrimp_pos = SHRIMP_POS;
	image = _assets->get<Texture>(SHRIMP_TEXTURE);
	std::shared_ptr<EnemyModel> _enemy = EnemyModel::alloc(shrimp_pos, image->getSize() / _scale, _scale, EnemyType::shrimp);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);

	Vec2 rice_pos = RICE_POS;
	image = _assets->get<Texture>(RICE_TEXTURE);
	_enemy = EnemyModel::alloc(rice_pos, image->getSize() / _scale, _scale, EnemyType::rice);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);

	Vec2 egg_pos = EGG_POS;
	image = _assets->get<Texture>(EGG_TEXTURE);
	_enemy = EnemyModel::alloc(egg_pos, image->getSize() / (_scale), _scale, EnemyType::egg);
	sprite = scene2::PolygonNode::allocWithTexture(image);
	_enemy->setSceneNode(sprite);
	_enemy->setName(ENEMY_NAME);
	_enemy->setDebugColor(DEBUG_COLOR);
	scene.addObstacle(_enemy, sprite);
	_enemies.push_back(_enemy);*/

	scene.setAssets(_assets);
	scene.setScale(_scale);
	scene.setBackground(_background);
	scene.setAvatar(_avatar);
	scene.setEnemies(_enemies);
	scene.setGoalDoor(_goalDoor);

};