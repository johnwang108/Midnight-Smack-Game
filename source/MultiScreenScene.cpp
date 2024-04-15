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
//
//namespace LayoutPositions {
//	Vec2 Top(const Vec2& size) { return Vec2(size.x/2, size.y*3/2); }
//	Vec2 MidLeft(const Vec2& size) { return Vec2(-size.x/2, size.y/2); }
//	Vec2 Mid(const Vec2& size) { return Vec2(size.x / 2, size.y / 2); }
//	Vec2 MidRight(const Vec2& size) { return Vec2(size.x*3/2, size.y / 2); }
//	Vec2 Bottom(const Vec2& size) { return Vec2(size.x/2, -size.y/2); }
//}

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
	_curr(0),
	_flag(0.0f){
}

void MultiScreenScene::dispose() {
	_assets = nullptr;
	_input = nullptr;
	_camera = nullptr;
	_scenes[0] = nullptr;
	_scenes[1] = nullptr;
	_scenes[2] = nullptr;
	_scenes[3] = nullptr;
	_scenes[4] = nullptr;
}

bool MultiScreenScene::init(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
	if (_flag < 0) {
		reset();
		return true;
	}
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

	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset("json/dayLevel1.json");
	readLevel(reader->readJson());

	_stationMap["potStation"] = 0;
	_stationMap["prepStation"] = 1;
	_stationMap["panfryStation"] = 2;
	_stationMap["cuttingStation"] = 3;
	_stationMap["mixingStation"] = 4;

	_stationIngredients[0] = std::vector<std::string>{ "rice", "egg"};
	_stationIngredients[1] = std::vector<std::string>{};
	_stationIngredients[2] = std::vector<std::string>{ "rice", "egg", "shrimp", "beef", "carrot"};
	_stationIngredients[3] = std::vector<std::string>{"shrimp", "carrot" };
	_stationIngredients[4] = std::vector<std::string>{ "egg" };



	std::string stationTextures[5] = {"potStation","prepStation" ,"panfryStation" ,"cuttingStation" ,"mixingStation"};
	initStations(stationTextures, 5);


	_scenes[2]->setFocus(true);


	//init inactive
	setActive(false);
	_transitionScenes = false;
	_targetScene = "";
	
	_curr = 2;
	_animating = false;
	Application::get()->setClearColor(Color4::BLACK);



	_uiScene = cugl::Scene2::alloc(_size);
	_uiScene->init(_size);
	_uiScene->setActive(true);

	//_uiNode = _assets->get<scene2::SceneNode>("lab0");
	//_uiNode->setContentSize(_size);
	//_uiNode->doLayout();

	_timer = scene2::Label::allocWithText("godfhohofgji", _assets->get<Font>(MESSAGE_FONT));

	_timer->setAnchor(Vec2::ANCHOR_CENTER);
	_timer->setPosition(_size.width/2, _size.height - _timer->getHeight());
	_timer->setForeground(Color4::BLACK);

	_gestureFeedback = scene2::Label::allocWithText("Perfect", _assets->get<Font>(MESSAGE_FONT));
	_gestureFeedback->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	_gestureFeedback->setPosition(_size.width / 2, _size.height - _gestureFeedback->getHeight());
	_gestureFeedback->setForeground(Color4::BLACK);
	_gestureFeedback->setVisible(false);


	std::shared_ptr<scene2::SceneNode> quotaRoot = _assets->get<scene2::SceneNode>("quotaScene");
	


	_expectationBar = quotaRoot->getChildByName("Quota")->getChildByName("BarExpectation");
	_expectationBar->setContentSize(_expectationBar->getSize());
	_expectationBar->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_expectationBar->setPosition(5, 0);

	_progBar = quotaRoot->getChildByName("Quota")->getChildByName("BarFilled");
	_progBar->setContentSize(_progBar->getSize());
	_progBar->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_progBar->setPosition(5,0);

	std::shared_ptr<scene2::SceneNode> uiRoot = _assets->get<scene2::SceneNode>("uiScene");
	

	_uiScene->addChild(_timer);
	_uiScene->addChild(_gestureFeedback);
	_uiScene->addChild(quotaRoot);
	_uiScene->addChild(uiRoot);
	//_uiScene->addChild(_uiNode);

	//std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset("exLevel");
	//std::shared_ptr<cugl::scene2::SceneNode> tempNode = _assets->get<scene2::SceneNode>("lab2");
	//_scenes[1]->addChild(tempNode);
	
	_flag = -1;
	_finishedIngredients = false;

	_ended = false;

	setName("day");

	transition(false);
	setTarget("");

	_currentScore = 0;
	_gameState = 0;
	_currentTime = 0;

	return true;

}

void MultiScreenScene::initStations(std::string textures[], int size) {
	std::string stationNames[5] = { "pot","prep" ,"panfry" ,"cutting" ,"mixing" };
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
		//todo maybe make hitboxes different by station
		scene->init(_assets, _input, rect, textures[i], std::vector<std::string>(), Size(600, 300));
		scene->setContentSize(SCENE_WIDTH, SCENE_HEIGHT);
		scene->setName(stationNames[i]);
		scene->setAnchor(Vec2::ANCHOR_CENTER);
		scene->setPosition(positions[i]);
		scene->setVisible(true);
		scene->setValidIngredients(_stationIngredients[i]);

		initializeBottomBar(scene, i);
		setScene(i, scene);
	}

	for (int i = 0; i < size; i++) {
		addChild(_scenes[i]);
	}
}

void MultiScreenScene::initializeBottomBar(std::shared_ptr<DollarScene> scene, int sceneNum) {
	std::string uiKey = "lab";
	uiKey += std::to_string(sceneNum);
	std::shared_ptr<cugl::scene2::SceneNode> tempNode = _assets->get<scene2::SceneNode>(uiKey);
	tempNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	tempNode->setPosition(Vec2(0, 0));
	tempNode->doLayout();
	scene->setBottomBar(tempNode);
	scene->addChild(tempNode);
}

void MultiScreenScene::readLevel(std::shared_ptr<JsonValue> leveljson) {
	_newIngredientIndex = 0;
	_dishToPrepare = leveljson->get("cooking")->asString();
	_quota = leveljson->get("quota")->asInt();
	_dayDuration = leveljson->get("duration")->asInt();

	std::shared_ptr<JsonReader> gestureReader = JsonReader::allocWithAsset("json/dayIngredientGestures.json");
	std::shared_ptr<JsonValue> gestureVals = gestureReader->readJson();

	std::shared_ptr<JsonValue> events = leveljson->get("ingredients");
	if (events->type() == JsonValue::Type::ArrayType) {
		for (int i = 0; i < events->size(); i++) {
			std::shared_ptr<JsonValue> item = events->get(i);
			if (item != nullptr) {
				std::shared_ptr<Ingredient> newIngredient = std::make_shared<Ingredient>();
				newIngredient->setName(item->get("ingredient")->asString());
				newIngredient->setStartTime(item->get("time")->asFloat());
				newIngredient->setStartStation(item->get("station")->asString());

				std::shared_ptr<JsonValue> ingObject = gestureVals->get(newIngredient->getName());

				//gestures
				std::vector<std::string> gestures = {};

				std::shared_ptr<JsonValue> jGests = ingObject->get("gestures");
				if (jGests->type() == JsonValue::Type::ArrayType) {
					for (int j = 0; j < jGests->size(); j++) {
						gestures.push_back(jGests->get(j)->asString());
					}
				}
				else {
					CULogError("Gestures is not an array type");
				}
				newIngredient->setGestures(gestures);

				//stations

				std::vector<std::string> stations = {};

				std::shared_ptr<JsonValue> jStations = ingObject->get("valid_stations");
				if (jStations->type() == JsonValue::Type::ArrayType) {
					for (int j = 0; j < jStations->size(); j++) {
						stations.push_back(jStations->get(j)->asString());
					}
				}
				else {
					CULogError("Valid Stations is not an array type");
				}
				newIngredient->setValidStations(stations);

				_ingredients.push_back(newIngredient);
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

	//CULog("Camera position: %f, %f", _camera->getPosition().x, _camera->getPosition().y);
	//CULog("Scene position: %f, %f", _scenes[_curr]->getPosition().x, _scenes[_curr]->getPosition().y);

	if (_input->didExit()) {
		CULog("Shutting down");
		transition(true);
		setTarget("main_menu");
	}

	if (_input->didReset()) {
		reset();
	}

	if (_input->didTransition()) {
		transition(true);
		setTarget("night");
		return;
	}

	if (_ended && !_animating) return;

	_currentTime += timestep;
	_timer->setText(std::to_string((int) _currentTime));

	float reqRate = _quota / _dayDuration;

	float reqQuotaUnits = _currentTime * reqRate;
	float quotaFloatPercent = reqQuotaUnits / _quota;

	float clipHeight = _expectationBar->getHeight() * quotaFloatPercent;
	//CULog("%f", clipHeight);
	std::shared_ptr<Scissor> scissor = Scissor::alloc(Rect(0, 0, _expectationBar->getWidth(), clipHeight));
	_expectationBar->setScissor(scissor);

	float progPerc = (float) _currentScore / (float) _quota;

	clipHeight = progPerc * _progBar->getHeight();
	std::shared_ptr<Scissor> scissor2 = Scissor::alloc(Rect(0, 0, _progBar->getWidth(), clipHeight));
	_progBar->setScissor(scissor2);

	if (_currentTime >= _dayDuration) {
		endDay();
	}
	
	if (!_finishedIngredients && _ingredients[_newIngredientIndex]->getStartTime() <= _currentTime) {
		CULog("found ingredient, %d", _newIngredientIndex);
		std::shared_ptr<Ingredient> upcomingIngredient = _ingredients[_newIngredientIndex];
		std::shared_ptr<Texture> tex = _assets->get<Texture>(upcomingIngredient->getName());
		upcomingIngredient->init(tex);
		//upcomingIngredient.getButton()->setPosition(Vec2(0, 0));
		int stationIdx = _stationMap[upcomingIngredient->getStartStation()];

		//todo update this when you get new widgets
		_scenes[stationIdx]->addIngredient(upcomingIngredient);
		_scenes[stationIdx]->setTargetGestures(upcomingIngredient->getGestures());

		if (_newIngredientIndex < _ingredients.size() - 1) _newIngredientIndex++;
		else {
			_finishedIngredients = true;
		}
	}


	for (int i = 0; i < 5; i++) {
		_scenes[i]->update(timestep);
		//move ingredients between conveyor belts
		std::shared_ptr<Ingredient> poppedIng = _scenes[i]->popIngredient();
		if (poppedIng != nullptr) {
			int newIndex;
			if (i >= 4) {
				newIndex = 0;
			}
			else {
				newIndex = i + 1;
			}
			_scenes[newIndex]->addIngredient(poppedIng);
		}

		//submitted ingredients
		std::shared_ptr<Ingredient> submittedIng = _scenes[i]->getSubmittedIngredient();
		if (submittedIng != nullptr) {
			increaseQuotaProgress();
			_scenes[i]->clearSubmittedIngredient();
		}
	}

	

	

	if (_scenes[_curr]->getJustCompletedGesture()) {
		_gestureInitiatedTime = _currentTime;
		_gestureFeedback->setPositionY(_size.height - _gestureFeedback->getHeight());
	}
	if (_currentTime - _gestureInitiatedTime < FEEDBACK_DURATION) {
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

	//if not animating, listen for screen change input. TODO for three fingered swipes to switch scenes
	if (!_animating) {
		if (_input->getHorizontal() > 0) {
			if ((_curr == 1) || (_curr == 2)) {
				switchStation(_curr, _curr + 1);
			}
		}
		else if (_input->getHorizontal() < 0) {
			if ((_curr == 2) || (_curr == 3)) {
				switchStation(_curr, _curr - 1);
			}
		}
		else if (_input->getVertical() > 0) {
			if ((_curr == 2) || (_curr == 4)) {
				switchStation(_curr, _curr - 2);
			}
		}
		else if (_input->getVertical() < 0) {
			if ((_curr == 0) || (_curr == 2)) {
				switchStation(_curr, _curr + 2);
			}
		}
	}
	Vec2 dist = _scenes[_curr]->getPosition() - _camera->getPosition();
	if (dist != Vec2::ZERO) {
		Vec2 movementAmount = Vec2(abs_min(sign(dist.x) * CAMERA_MOVE_SPEED, dist.x), abs_min(sign(dist.y) * CAMERA_MOVE_SPEED, dist.y));
		_camera->translate(movementAmount);
		_camera->update();
		
		if (_scenes[_curr]->getCurrentlyHeldIngredient() != nullptr) {
			std::shared_ptr<scene2::Button> button = _scenes[_curr]->getCurrentlyHeldIngredient()->getButton();
			CULog("Button Pos Before: %f %f", button->getPositionX(), button->getPositionY());
			button->setPosition(button->getPosition() + movementAmount);
			CULog("Button Pos After: %f %f", button->getPositionX(), button->getPositionY());
		}
	
	}
	else {
		_animating = false;
	}


	
	
	// find current touch position, set curHeld->button to be that pos

}

/*
int MultiScreenScene::determineSwipeDirection() {
	Vec2 swipeDelta = _input->getSwipeDelta();

	Vec2 normSwipeDelta = swipeDelta.getNormalization();

	if (std::fabs(normSwipeDelta.x) > std::fabs(normSwipeDelta.y)) {
		if (normSwipeDelta.x > 0) { 
			return 1;
		}
	}
}
*/
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

void MultiScreenScene::switchStation(int currId, int targId) {
	_scenes[currId]->setFocus(false);
	_scenes[targId]->setFocus(true);
	_curr = targId;
	_animating = true;
	//handle moving ing
	std::shared_ptr<Ingredient> ing = _scenes[currId]->getCurrentlyHeldIngredient();
	if (ing != nullptr) {
		_scenes[currId]->removeHeldIngredient();
		_scenes[targId]->receiveHeldIngredient(ing);
	}
	CULog("don");
}

void MultiScreenScene::increaseQuotaProgress() {
	_currentScore += 1;
	CULog("score: %d", _currentScore);
}


/* 
*/
void MultiScreenScene::reset() {
	_camera->setPosition(Vec2(0, 0));
	_camera->update();
	_finishedIngredients = false;
	_newIngredientIndex = 0;
	_curr = 2;
	_ended = false;
	_currentTime = 0.0f;
	_gestureFeedback->setVisible(false);
	_gestureFeedback->setText("");
	_currentScore = 0;
	
	for (int i = 0; i < 5; i++) {
		_scenes[i]->reset();
	}

	focusCurr();
}

void MultiScreenScene::save() {
	//save the current state of the game
	//should only change daytime and persistent save data
	auto reader = JsonReader::alloc("./save.json");
	auto writer = JsonWriter::alloc("./save.json");

	std::shared_ptr<JsonValue> json = JsonValue::allocObject();

	//placeholder values
	json->appendValue("chapter", 1.0f);
	json->appendValue("level", 1.0f);

	std::shared_ptr<JsonValue> day = JsonValue::allocArray();

	json->appendChild("day", day);

}

void MultiScreenScene::loadSave() {
	auto reader = JsonReader::alloc("./save.json");
}

void MultiScreenScene::endDay() {
	_ended = true;
	if (_currentScore >= _quota) {
		_gameState = 1;
	}
	else {
		_gameState = -1;
	}
}


