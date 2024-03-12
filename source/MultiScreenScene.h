#ifndef __MULTI_SCREEN_SCENE_H__
#define __MULTI_SCREEN_SCENE_H__

#include <cugl/cugl.h>
#include "PFInput.h"
#include "PFDollarScene.h"
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>


class MultiScreenScene : public cugl::Scene2 {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;

	std::shared_ptr<PlatformInput> _input;

	/** The individual scenes */
	std::shared_ptr<DollarScene> _scenes[5];
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
public:
	MultiScreenScene();

	~MultiScreenScene() { dispose(); }

	void dispose();

	bool init(const std::shared_ptr<cugl::AssetManager>& assets);

	void initStations(std::string arr[], int size);

	void setScene(Uint32 slot, std::shared_ptr<DollarScene> scene) { _scenes[slot] = scene; }

	std::shared_ptr<cugl::Scene2> getScene(Uint32 slot) const;

	void update(float timestep);

	void preUpdate(float timestep);

	void fixedUpdate(float timestep);

	void postUpdate(float timestep);
};

#endif /* __MULTI_SCREEN_SCENE_H__ */