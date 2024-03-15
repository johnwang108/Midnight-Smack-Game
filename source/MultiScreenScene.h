#ifndef __MULTI_SCREEN_SCENE_H__
#define __MULTI_SCREEN_SCENE_H__

#include <cugl/cugl.h>
#include "PFInput.h"
#include "PFDollarScene.h"
#include "Order.h"
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>


class MultiScreenScene : public cugl::Scene2 {
private:
	cugl::Timestamp _startTime;

	// current time, in seconds with decimals
	float _currentTime;
	std::map<std::string, int> _stationMap;
	bool _finishedOrders;


protected:
	std::shared_ptr<cugl::AssetManager> _assets;

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

	std::shared_ptr<cugl::scene2::Label> _timer;

	std::vector<Order> _orders; 
	// the index in the _orders vector where we will find the first new order
	int _newOrderIndex;
public:
	MultiScreenScene();

	~MultiScreenScene() { dispose(); }

	void dispose();

	bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input);

	void initStations(std::string arr[], int size);

	void setScene(Uint32 slot, std::shared_ptr<DollarScene> scene) { _scenes[slot] = scene; }

	std::shared_ptr<cugl::Scene2> getScene(Uint32 slot) const;

	void update(float timestep);

	void preUpdate(float timestep);

	void fixedUpdate(float timestep);

	void postUpdate(float timestep);

	void transition(bool t);

	bool transitionedAway() { return _transitionScenes; }

	int determineSwipeDirection();

	void readLevel(std::shared_ptr<JsonValue> level);

	void renderUI(std::shared_ptr<SpriteBatch> batch);

	void tempPopulate();
	
	void unfocusAll();

	void focusCurr();
};

#endif /* __MULTI_SCREEN_SCENE_H__ */