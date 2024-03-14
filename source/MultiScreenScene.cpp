#include "MultiScreenScene.h"
#include "PFDollarScene.h"
#include <algorithm> 
#include "Levels/Levels.h"

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

bool MultiScreenScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
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

	//MULTISCREEN IS RESPONSIBLE FOR INITING THE SHARED INPUT CONTROLLER. TEMPORARY SOLUTION
	_input = input;
	_input->init(getBounds());
	//_input = std::make_shared<PlatformInput>();
	//_input->init(getBounds());

	_stationMap;
	_stationMap["pot_station"] = 0;
	_stationMap["prep_station"] = 1;
	_stationMap["panfry_station"] = 2;
	_stationMap["cutting_station"] = 3;
	_stationMap["blending_station"] = 4;

	std::string stationTextures[5] = {"pot_station","prep_station" ,"panfry_station" ,"cutting_station" ,"blending_station"};
	initStations(stationTextures, 5);


	_scenes[2]->setFocus(true);


	//init inactive
	setActive(false);
	_transitionScenes = false;
	
	_curr = 2;
	_animating = false;
	Application::get()->setClearColor(Color4::BLACK);


	_startTime = Timestamp();

	_uiScene = cugl::Scene2::alloc(_size);
	_uiScene->init(_size);
	_uiScene->setActive(true);
	
	_timer = scene2::Label::allocWithText("godfhohofgji", _assets->get<Font>(MESSAGE_FONT));

	_timer->setAnchor(Vec2::ANCHOR_CENTER);
	_timer->setPosition(_size.width/2, _size.height - _timer->getHeight());
	_timer->setForeground(Color4::BLACK);

	_uiScene->addChild(_timer);

	tempPopulate();

	_finishedOrders = false;

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

void MultiScreenScene::readLevel(std::shared_ptr<JsonValue> level) {
	std::shared_ptr<JsonValue> events = level->get("events");
	if (events->type() == JsonValue::Type::ArrayType) {
		for (int i = 0; i < events->size(); i++) {
			std::shared_ptr<JsonValue> item = events->get(i);
			
		}
	}
}
 

void MultiScreenScene::update(float timestep) {
	
}

void MultiScreenScene::preUpdate(float timestep) {
	Timestamp now = Timestamp();

	//CULog("%llu", now.ellapsedMillis(_startTime));

	_currentTime = now.ellapsedMillis(_startTime) / 1000;
	_timer->setText(std::to_string((int) _currentTime));

	
	if (!_finishedOrders && _orders[_newOrderIndex].getStartTime() <= _currentTime) {
		CULog("found order, %d", _newOrderIndex);
		Order upcomingOrder = _orders[_newOrderIndex];
		int stationIdx = _stationMap[upcomingOrder.getStation()];
		_scenes[stationIdx]->setTargetGestures(upcomingOrder.getGestures());

		if (_newOrderIndex < _orders.size() - 1) _newOrderIndex++;
		else {
			_finishedOrders = true;
		}
	}

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

	if (_input->didTransition()) {
		transition(true);
		CULog("______________________________________________________________________________________________________");
		return;
	}

	//if not animating, listen for screen change input. TODO for three fingered swipes to switch scenes
	if (!_animating) {
		if (_input->getHorizontal() > 0) {
			if ((_curr == 1) || (_curr == 2)) {
				_scenes[_curr]->setFocus(false);
				_curr++;
				_scenes[_curr]->setFocus(true);
				_animating = true;
			}
		}
		else if (_input->getHorizontal() < 0) {
			if ((_curr == 2) || (_curr == 3)) {
				_scenes[_curr]->setFocus(false);
				_curr--;
				_scenes[_curr]->setFocus(true);
				_animating = true;
			}
		}

		else if (_input->getVertical() > 0) {
			if ((_curr == 2) || (_curr == 4)) {
				_scenes[_curr]->setFocus(false);
				_curr -= 2;
				_scenes[_curr]->setFocus(true);
				_animating = true;
			}
		}
		else if (_input->getVertical() < 0) {
			if ((_curr == 0) || (_curr == 2)) {
				_scenes[_curr]->setFocus(false);
				_curr += 2;
				_scenes[_curr]->setFocus(true);
				_animating = true;
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

//Marks transitioning between cooking and platforming. Call this method with t = true when you want to transition away from this scene
void MultiScreenScene::transition(bool t) {
	_transitionScenes = t;
}

void MultiScreenScene::renderUI(std::shared_ptr<cugl::SpriteBatch> batch) {
	_uiScene->render(batch);
}

void MultiScreenScene::tempPopulate() {
	std::vector<std::string> newGests = { "pigtail", "circle", "v" };
	Order order1 = Order("pot", newGests, 4.0);
	std::vector<std::string> newGests2 = { "circle", "v", "v" };
	Order order2 = Order("panfry", newGests2, 6.0);

	_orders = { order1, order2 };
	_newOrderIndex = 0;
	// TODO: Sort orders by time so we can just keep track of the index we've sent orders up to
	// i.e. once we've sent an order in update we can update the index to be the next new order
}