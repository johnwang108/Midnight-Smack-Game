#ifndef __MULTI_SCREEN_SCENE_H__
#define __MULTI_SCREEN_SCENE_H__

#include <cugl/cugl.h>
#include "PFInput.h"
#include "PFDollarScene.h"
#include "Ingredient.h"
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>


class MultiScreenScene : public cugl::Scene2 {
private:
	// current time, in seconds with decimals
	float _currentTime;
	std::map<std::string, int> _stationMap;
	std::map<int, std::vector<std::string>> _stationIngredients;
	bool _finishedIngredients;
	
	std::vector<int> _bonusObjectives;



protected:
	std::shared_ptr<cugl::AssetManager> _assets;
	std::shared_ptr<cugl::scene2::SceneNode> _uiNode;
	std::shared_ptr<cugl::scene2::SceneNode> _progBar;
	std::shared_ptr<cugl::scene2::SceneNode> _expectationBar;
	std::shared_ptr<cugl::scene2::SceneNode> _winScreenRoot;
	std::vector<std::shared_ptr<cugl::scene2::Button>> _buttons;


	std::shared_ptr<cugl::scene2::Label> _stationLabel;
	std::shared_ptr<cugl::scene2::Label> _twelveHourTimer;

	int _currentHour;
	int _currentMinute;



	std::shared_ptr<PlatformInput> _input;


	/** The individual scenes */
	std::shared_ptr<DollarScene> _scenes[5];

	std::shared_ptr<Scene2> _uiScene;

	///** The textures to display each scene, not sure if needed */
	//std::shared_ptr<cugl::Texture> _texture[5];

	/** Viewport Size */
	cugl::Size _size;

	/** The zoom out factor */
	float _zoom;
	/** Whether or not we are currently animating */
	bool _animating;
	/** The current scene */
	int _curr;

	/** Whether or not this scene initiated a transfer to the other gameplay mode scene*/
	bool _transitionScenes;

	std::string _targetScene;


	std::shared_ptr<cugl::scene2::Label> _gestureFeedback;

	float _gestureInitiatedTime;

	std::vector<std::shared_ptr<Ingredient>> _ingredients; 

	std::shared_ptr<Ingredient> _heldIngredient;

	std::string _feedbackMessages[3] = { "Bad", "Good", "Perfect" };

    /* This is the name of the dish we are making for the day, to display */
	std::string _dishToPrepare;
	float _dayDuration;
	int _quota;
	int _currentScore; 
	
	// 0 = playing, 1 = win, -1 = lose
	int _gameState;

	
	/* If the day has ended*/
	bool _ended;
	// the index in the _orders vector where we will find the first new order
	int _newIngredientIndex;
	float _flag;
public:
	MultiScreenScene();

	~MultiScreenScene() { dispose(); }

	void dispose();

	bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input);

	void initStations(std::string arr[], int size);

	void initializeBottomBar(std::shared_ptr<DollarScene> scene, int sceneNum);

	void setScene(Uint32 slot, std::shared_ptr<DollarScene> scene) { _scenes[slot] = scene; }

	std::shared_ptr<cugl::Scene2> getScene(Uint32 slot) const;

	void update(float timestep);

	void preUpdate(float timestep);

	void fixedUpdate(float timestep);

	void postUpdate(float timestep);

	void transition(bool t);

	int determineSwipeDirection();

	void readLevel(std::shared_ptr<cugl::JsonValue> level);

	void renderUI(std::shared_ptr<cugl::SpriteBatch> batch);

	void tempPopulate();
	
	void unfocusAll();

	void focusCurr();

	void switchStation(int currId, int targId);
	void endDay();


	void reset();

	bool didTransition() { return _transitionScenes; };

	void setTransition(bool b) { _transitionScenes = b; };

	std::string getTarget() { return _targetScene; };

	void setTarget(std::string s) { _targetScene = s; };

	void increaseQuotaProgress();

	void save();

	void changeCurrentLevel(int chapter, int level);

	bool loadSave(std::shared_ptr<JsonValue> save);
};

#endif /* __MULTI_SCREEN_SCENE_H__ */