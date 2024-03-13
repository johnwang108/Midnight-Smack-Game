#include "MultiScreenScene.h"
#include "PFDollarScene.h"
#include <algorithm> 

#define CAMERA_MOVE_SPEED 50.0f

using namespace cugl;

namespace LayoutPositions {
	Vec2 Top(const Vec2& size) { return Vec2(0, size.y); }
	Vec2 MidLeft(const Vec2& size) { return Vec2(-size.x, 0); }
	Vec2 Mid(const Vec2& size) { return Vec2(0, 0); }
	Vec2 MidRight(const Vec2& size) { return Vec2(size.x, 0); }
	Vec2 Bottom(const Vec2& size) { return Vec2(0, -size.y); }
 }


//basic math funcs
float abs_min(float a, float b) {
	if (abs(a) < abs(b)) {
		return a;
	}
	else {
		return b;
	}
}

float abs_max(float a, float b) {
	if (abs(a) > abs(b)) {
		return a;
	}
	else {
		return b;
	}
}

int sign(int x) {
	return (x > 0) - (x < 0);
}



MultiScreenScene::MultiScreenScene() : Scene2(),
	_assets(nullptr),
	_input(nullptr),
	_size(0, 0),
	_zoom(1.0f),
	_animating(false),
	_curr(0) {
}

void MultiScreenScene::dispose() {
	_assets = nullptr;
	_input = nullptr;
	_camera = nullptr;
	//_texture[0] = nullptr;
	//_texture[1] = nullptr;
	//_texture[2] = nullptr;
	//_texture[3] = nullptr;
	_scenes[0] = nullptr;
	_scenes[1] = nullptr;
	_scenes[2] = nullptr;
	_scenes[3] = nullptr;
	_scenes[4] = nullptr;
}

bool MultiScreenScene::init(const std::shared_ptr<AssetManager>& assets) {
	_size = Application::get()->getDisplaySize();
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene2::init(_size)) {
		return false;
	}

	_camera->setPosition(Vec2(0,0));
	_camera->update();

	_assets = assets;

	CULog("Size: %f %f", _size.width, _size.height);
	_input = std::make_shared<PlatformInput>();
	_input->init(getBounds());

	std::string stationTextures[5] = {"pot_station","prep_station" ,"panfry_station" ,"cutting_station" ,"panfry_station"};
	initStations(stationTextures, 5);


	_active = true;

	_curr = 2;
	_animating = false;
	Application::get()->setClearColor(Color4::BLACK);

	return true;

}

void MultiScreenScene::initStations(std::string textures[], int size) {
	std::shared_ptr<DollarScene> scene;
	
	cugl::Rect rect;
	std::vector<Vec2> positions = {
		LayoutPositions::Top(_size),
		LayoutPositions::MidLeft(_size),
		LayoutPositions::Mid(_size),
		LayoutPositions::MidRight(_size),
		LayoutPositions::Bottom(_size)
	};

	for (int i = 0; i < size; i++) {
		scene = std::make_shared<DollarScene>();
				
		rect = cugl::Rect(Vec2::ZERO, _size);
		scene->init(_assets, _input, rect, textures[i]);
		scene->setAnchor(Vec2::ANCHOR_CENTER);
		scene->setPosition(positions[i]);
		scene->setVisible(true);
		setScene(i, scene);
	}

	for (int i = 0; i < size; i++) {
		addChild(_scenes[i]);
	}
}
 

void MultiScreenScene::update(float timestep) {
	

}

void MultiScreenScene::preUpdate(float timestep) {
	//_scenes[_curr]->update();

	_input->update(timestep);

	_scenes[0]->update(timestep);
	_scenes[1]->update(timestep);
	_scenes[2]->update(timestep);
	_scenes[3]->update(timestep);
	_scenes[4]->update(timestep);

	if (_input->didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

	
	//if not animating, listen for screen change input. TODO for three fingered swipes to switch scenes
	if (!_animating) {
		if (_input->getHorizontal() > 0) {
			if ((_curr == 1) || (_curr == 2)) {
				_curr++;
				_animating = true;
				CULog("%d", _curr);
			}
		}
		else if (_input->getHorizontal() < 0) {
			if ((_curr == 2) || (_curr == 3)) {
				_curr--;
				_animating = true;
				CULog("%d", _curr);
			}
		}

		else if (_input->getVertical() > 0) {
			if ((_curr == 2) || (_curr == 4)) {
				_curr -= 2;
				_animating = true;
				CULog("%d", _curr);
			}
		}
		else if (_input->getVertical() < 0) {
			if ((_curr == 0) || (_curr == 2)) {
				_curr += 2;
				_animating = true;
				CULog("%d", _curr);
			}
		}
		// check for swipes now
		if (!_animating) {
			
		}

	}
	Vec2 dist = _scenes[_curr]->getPosition() - _camera->getPosition();
	if (dist != Vec2::ZERO) {
		_camera->translate(cugl::Vec2( abs_min(sign(dist.x) * CAMERA_MOVE_SPEED, dist.x), abs_min(sign(dist.y) * CAMERA_MOVE_SPEED, dist.y)));
		_camera->update();
	}
	else {
		_animating = false;
	}
}


int MultiScreenScene::determineSwipeDirection() {
	Vec2 swipeDelta = _input->getSwipeDelta();

	Vec2 normSwipeDelta = swipeDelta.getNormalization();

	if (std::fabs(normSwipeDelta.x) > std::fabs(normSwipeDelta.y)) {
		if (normSwipeDelta.x > 0) { 
			return 1;
		}
	}
}

void MultiScreenScene::fixedUpdate(float timestep) {

}

void MultiScreenScene::postUpdate(float timestep) {

}