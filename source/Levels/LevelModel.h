#ifndef __LEVEL_MODEL_H__
#define __LEVEL_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUObstacleWorld.h>
#include <vector>
#include <cugl/assets/CUAsset.h>
#include <cugl/io/CUJsonReader.h>
#include "/../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/PFInput.h"
#include "/../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/PFAttack.h"
#include "/../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/PFDudeModel.h"
#include "/../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/Enemys/Enemy.h"
#include "/../Users/benlo/source/repos/Midnight-Smack-Folder/Midnight-Smack-Game/source/Enemys/Bull.h"
#include "Levels.h"

// using namespace cugl;

#pragma mark -
#pragma mark Level Model

// class GameScene;
// class PFDudeModel;

class LevelModel : public Levels {

public:

	// The root node of a level
	std::shared_ptr<scene2::SceneNode> _root;
	//bounds of this level
	Rect _bounds;
	//level drawing scale
	Vec2 _scale;
	//Reference to the physics node of the scene graph
	std::shared_ptr<scene2::SceneNode> _worldNode;
	//Reference to the physics world; part of the model
	std::shared_ptr<physics2::ObstacleWorld> _world;
	// Reference to the avatar
	std::shared_ptr<DudeModel> _avatar;
	// Reference to the goal door
	std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;
	// Reference to the background image
	std::shared_ptr<cugl::scene2::PolygonNode> _background;
	// Reference to the number of enemies that make up a level
	std::vector<std::shared_ptr<EnemyModel>> _enemies;
	//The main platform image
	std::shared_ptr<cugl::physics2::PolygonObstacle> _main_platform;

	// The external platforms (that are not connected to main platform)
	// Vector because there will be multiple platforms
	std::vector<std::shared_ptr<cugl::physics2::PolygonObstacle>> _floating_platforms;

	//Reference to the asset manager
	std::shared_ptr<cugl::AssetManager> _assets;

	std::map<int, std::string> idToImage;

	std::string level_file_path;

	// virtual ~LevelModel() = default;

	// virtual void populate(GameScene& scene) = 0;

	//we will invoke these methods when parsing object layer
	// when we create each of these objects, we will need to couple them
	// to the scene graph

	/** loads the player */
	// void loadAvatar(const std::shared_ptr<JsonValue>& json);

	/** loads the goal door, this will load the physical parts of the goal door */
	// void loadGoalDoor(const std::shared_ptr<JsonValue>& json);

	/** loads the physical parts of the floating platform*/
	void loadFloatingBoxPlatform(const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height);

	/** loads the main platform, specifically its physical parts*/
	void loadMainPlatform(const std::shared_ptr<JsonValue>& json, GameScene& scene, std::shared_ptr<scene2::PolygonNode> sprite, float level_height);

	/** Clears the root scene graph node for the level*/
	// void clearRootNode();

	/**gives levelsceneray keyword*/
	std::string getLevelScenery(const std::string levelNumber);

	/**
	 * Adds the physics object to the physics world and loosely couples it to the scene graph
	 *
	 * There are two ways to link a physics object to a scene graph node on the
	 * screen.  One way is to make a subclass of a physics object, like we did
	 * with rocket.  The other is to use callback functions to loosely couple
	 * the two.  This function is an example of the latter.
	 *
	 * param obj    The physics object to add
	 * param node   The scene graph node to attach it to
	 */
	void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
		const std::shared_ptr<cugl::scene2::SceneNode>& node);

	/** creates a new game level with no source file*/
	//static std::shared_ptr<LevelModel> alloc() {
	//	std::shared_ptr<LevelModel> result = std::make_shared<LevelModel>();
	//	return (result->init("") ? result : nullptr);
	//}

	///** creates a new game level with the given source file */
	//static std::shared_ptr<LevelModel> alloc(std::string file) {
	//	std::shared_ptr<LevelModel> result = std::make_shared<LevelModel>();
	//	return (result->init(file) ? result : nullptr);
	//}

#pragma mark Model Access

	/** returns the avatar for this level */

	/** returns the goaldoor for this level */

	/** returns the obstacle world for this level */

#pragma mark Physical Attributes

	/** returns the bounds of this level in physics coordinates */
	const Rect& getBounds() const { return _bounds; };

#pragma mark Drawing Methods

	/** returns the scene graph node for drawing purposes */
	const std::shared_ptr<scene2::SceneNode>& getRootNode() const { return _root; };

	void setRootNode(const std::shared_ptr<scene2::SceneNode>& root);

	/** sets the loaded assets for this game level */
	void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets; };

	/** sets the world for our level*/
	void setWorld(const std::shared_ptr<physics2::ObstacleWorld>& world) {
		_world = world;
	}

	std::shared_ptr<physics2::ObstacleWorld> getWorld() {
		return _world;
	}

	void setFilePath(std::string path) {
		level_file_path = path;
	}

	std::string getFilePath() {
		return level_file_path;
	}



#pragma Asset Loading 
	/**
	* Loads this game level from the source file
	*
	* @param file the name of ths source file to load from
	*
	* @return true if successfully loaded the asset from a file
	*/

	// virtual bool preload(const std::string file) override;

	/**
	* Loads this game level from a JsonValue containing all datra from a source Json file
	*
	* @param json the json loaded from the source file to use when loading this game level
	*
	* @return true if successfully loaded the asset from the input JsonValue
	*/

	// virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;

	/** Unloads this game level, releasing all sources*/

	// void unload();

#pragma mark Initializers

	//Creates a new, empty level
	LevelModel(void);

	LevelModel(std::string path);

	//Destroys this level
	// virtual ~LevelModel(void);

	void populate(GameScene& scene) override;
};


#endif