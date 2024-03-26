#include "MultiScreenScene.h"
#include "PFDollarScene.h"
#include <algorithm> 
#include "Levels/Levels.h"

#define CAMERA_MOVE_SPEED 50.0f
#define FEEDBACK_DURATION 2.0f

using namespace cugl;

namespace LayoutPositions {
	Vec2 Top(const Vec2& size) { return Vec2(0, size.y); }
	Vec2 MidLeft(const Vec2& size) { return Vec2(-size.x, 0); }
	Vec2 Mid(const Vec2& size) { return Vec2(0, 0); }
	Vec2 MidRight(const Vec2& size) { return Vec2(size.x, 0); }
	Vec2 Bottom(const Vec2& size) { return Vec2(0, -size.y); }
 }

std::string targets[5] = { "pigtail", "circle", "vertSwipe", "horizSwipe", "v" };


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

	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset("json/exLevel.json");
	readLevel(reader->readJson());
	
	//MULTISCREEN IS RESPONSIBLE FOR INITING THE SHARED INPUT CONTROLLER. TEMPORARY SOLUTION
	_input = input;
	_input->init(getBounds());

	//_input = std::make_shared<PlatformInput>();
	//_input->init(getBounds());

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

	/*_uiNode = _assets->get<scene2::SceneNode>("lab");
	_uiNode->setContentSize(_size);
	_uiNode->doLayout();*/

	_timer = scene2::Label::allocWithText("godfhohofgji", _assets->get<Font>(MESSAGE_FONT));

	_timer->setAnchor(Vec2::ANCHOR_CENTER);
	_timer->setPosition(_size.width/2, _size.height - _timer->getHeight());
	_timer->setForeground(Color4::BLACK);

	_gestureFeedback = scene2::Label::allocWithText("Perfect", _assets->get<Font>(MESSAGE_FONT));
	_gestureFeedback->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	_gestureFeedback->setPosition(_size.width / 2, _size.height - _gestureFeedback->getHeight());
	_gestureFeedback->setForeground(Color4::BLACK);
	_gestureFeedback->setVisible(false);

	_uiScene->addChild(_timer);
	_uiScene->addChild(_gestureFeedback);
	//_uiScene->addChild(_uiNode);

	//std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset("exLevel");
	//std::shared_ptr<cugl::scene2::SceneNode> tempNode = _assets->get<scene2::SceneNode>("lab2");
	//_scenes[1]->addChild(tempNode);
	

	_finishedIngredients = false;

	_ended = false;

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

		std::string uiKey = "lab"; 
		uiKey += std::to_string(i);
		std::shared_ptr<cugl::scene2::SceneNode> tempNode = _assets->get<scene2::SceneNode>(uiKey);
		tempNode->setAnchor(Vec2::ANCHOR_CENTER);
		tempNode->setPosition(Vec2(0, 0));
		scene->setBottomBar(tempNode);
		scene->addChild(tempNode);
		setScene(i, scene);
	}

	for (int i = 0; i < size; i++) {
		addChild(_scenes[i]);
	}
}

void MultiScreenScene::readLevel(std::shared_ptr<JsonValue>& leveljson) {
	_newIngredientIndex = 0;
	_dishToPrepare = leveljson->get("cooking")->asString();
	_quota = leveljson->get("quota")->asInt();
	_dayDuration = leveljson->get("duration")->asInt();

	std::shared_ptr<JsonValue> events = leveljson->get("ingredients");
	if (events->type() == JsonValue::Type::ArrayType) {
		for (int i = 0; i < events->size(); i++) {
			std::shared_ptr<JsonValue> item = events->get(i);
			if (item != nullptr) {
				std::shared_ptr<Ingredient> newOrder = std::make_shared<Ingredient>();
				newOrder->setName(item->get("ingredient")->asString());
				newOrder->setStartTime(item->get("time")->asFloat());
				newOrder->setStation(item->get("station")->asString());
				std::vector<std::string> gestures = {};
				std::shared_ptr<JsonValue> jGests = item->get("gestures");
				if (jGests->type() == JsonValue::Type::ArrayType) {
					for (int j = 0; j < jGests->size(); j++) {
						gestures.push_back(jGests->get(j)->asString());
					}
				}
				else {
					CULogError("Gestures is not an array type");
				}

				newOrder->setGestures(gestures);
				_ingredients.push_back(newOrder);
			}
		}
	}
	else {
		CULogError("Events is not an array type");
	}
}
 

void MultiScreenScene::update(float timestep) {
	
}

void MultiScreenScene::preUpdate(float timestep) {
	_input->update(timestep);

	if (_input->didExit()) {
		CULog("Shutting down");
		Application::get()->quit();
	}

	if (_ended && !_animating) return;

	Timestamp now = Timestamp();

	//CULog("%llu", now.ellapsedMillis(_startTime));

	_currentTime = now.ellapsedMillis(_startTime) / 1000;
	_timer->setText(std::to_string((int) _currentTime));

	if (_currentTime >= _dayDuration) {
		endDay();
	}
	
	if (!_finishedIngredients && _ingredients[_newIngredientIndex]->getStartTime() <= _currentTime) {
		CULog("found order, %d", _newIngredientIndex);
		std::shared_ptr<Ingredient> upcomingIngredient = _ingredients[_newIngredientIndex];
		std::shared_ptr<Texture> tex = _assets->get<Texture>(upcomingIngredient->getName());
		upcomingIngredient->init(tex);
		//upcomingIngredient.getButton()->setPosition(Vec2(0, 0));
		int stationIdx = _stationMap[upcomingIngredient->getStation()];

		//todo update this when you get new widgets
		_scenes[stationIdx]->addIngredient(upcomingIngredient);
		std::shared_ptr<scene2::SceneNode> bar = _scenes[stationIdx]->getBottomBar()->getChildByName("kitchenbar");
		bar->getChildByName("Conveyor")->getChildByName("ConveyorBelt")->addChild(upcomingIngredient->getButton());

		_scenes[stationIdx]->setTargetGestures(upcomingIngredient->getGestures());

		if (_newIngredientIndex < _ingredients.size() - 1) _newIngredientIndex++;
		else {
			_finishedIngredients = true;
		}
	}


	for (int i = 0; i < 5; i++) {
		_scenes[i]->update(timestep);
	}

	if (_scenes[_curr]->getJustCompletedGesture()) {
		_gestureInitiatedTime = Timestamp();
		_gestureInitiatedTime.mark();
		_gestureFeedback->setPositionY(_size.height - _gestureFeedback->getHeight());
	}
	if (now.ellapsedMillis(_gestureInitiatedTime) / 1000.0f < FEEDBACK_DURATION) {
		int lastResult = _scenes[_curr]->getLastResult();
		if (lastResult != -1) {
			_gestureFeedback->setText(_feedbackMessages[lastResult]);
			_gestureFeedback->setPositionY(_gestureFeedback->getPositionY() - 1.0f);
			_gestureFeedback->setVisible(true);
		}
		
	}
	else {
		_gestureFeedback->setText("");
		_gestureFeedback->setVisible(false);
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
	Ingredient order1 = Ingredient("pot", newGests, 4.0);
	std::vector<std::string> newGests2 = { "circle", "v", "circle" };
	Ingredient order2 = Ingredient("panfry", newGests2, 6.0);

	//_ingredients = { order1, order2 };
	_newIngredientIndex = 0;
	// TODO: Sort orders by time so we can just keep track of the index we've sent orders up to
	// i.e. once we've sent an order in update we can update the index to be the next new order
}

void MultiScreenScene::unfocusAll() {
	for (int i = 0; i < 5; i++) {
		if (_scenes[i] != nullptr) {
			_scenes[i]->setFocus(false);
		}
	}
}

void MultiScreenScene::focusCurr() {
	_scenes[_curr]->setFocus(true);
}

void MultiScreenScene::endDay() {
	_ended = true;
}

