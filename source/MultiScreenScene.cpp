#include "MultiScreenScene.h"
#include "PFDollarScene.h"
#include <algorithm> 
#include "Levels/Levels.h"
#include <cctype>
#include <iomanip> 

#define CAMERA_MOVE_SPEED 50.0f
#define FEEDBACK_DURATION 2.0f

#define OBJ_CARD_HEIGHT 500
#define OBJ_CARD_WIDTH 300

//left side spacing, (1280 - 3*300)/4
#define OBJ_CARD_SPACING 95.0f

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
	
	_stationLabel = std::dynamic_pointer_cast<scene2::Label>(uiRoot->getChildByName("Status")->getChildByName("STATION"));
	_stationLabel->setText("PANFRY STATION");
	_stationLabel->doLayout();


	_twelveHourTimer = std::dynamic_pointer_cast<scene2::Label>(uiRoot->getChildByName("Status")->getChildByName("Sunny")->getChildByName("Time"));
	_currentHour = 6;
	_currentMinute = 0;


	_winScreenRoot = _assets->get<scene2::SceneNode>("dayWinScreen");

	std::shared_ptr<scene2::Button> b = std::dynamic_pointer_cast<scene2::Button>(_winScreenRoot->getChildByName("continuebutton"));
	b->addListener([=](const std::string& name, bool down) {
		this->_active = false;
		this->setTransition(true);
		this->setTarget("night");
		});
	_buttons.push_back(b);


	std::shared_ptr<scene2::Label> statUps = scene2::Label::allocWithText("", _assets->get<Font>("winter drinkRegular25"));
	statUps->setName("statUps");
	_winScreenRoot->addChild(statUps);
	//b = std::dynamic_pointer_cast<scene2::Button>(_winScreenRoot->getChildByName("retrybutton"));
	//b->addListener([=](const std::string& name, bool down) {
	//	this->reset();
	//	});
	//_buttons.push_back(b);


	//b = std::dynamic_pointer_cast<scene2::Button>(_winScreenRoot->getChildByName("menubutton"));
	//b->addListener([=](const std::string& name, bool down) {
	//	this->_active = false;
	//	this->setTransition(true);
	//	this->setTarget("main_menu");
	//});
	//_buttons.push_back(b);

	_winScreenRoot->setVisible(false);


	

	_uiScene->addChild(_gestureFeedback);
	_uiScene->addChild(quotaRoot);
	_uiScene->addChild(uiRoot);
	_uiScene->addChild(_winScreenRoot);

	for (int i = 0; i < _bonusObjectives.size(); i++) {
		std::shared_ptr<scene2::PolygonNode> bonusObj = createObjectiveNode(_bonusObjectives[i]);
		bonusObj->setPosition(OBJ_CARD_WIDTH / 2 + OBJ_CARD_SPACING + (OBJ_CARD_SPACING * i) + (OBJ_CARD_WIDTH * i), 400);
		_uiScene->addChild(bonusObj);
	}

	


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
		_scenes[i]->setNighttime(false);
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

	//todo load in bonus objectives
	_bonusObjectives = {};
	std::shared_ptr<JsonValue> objectives = leveljson->get("goals");
	if (objectives->type() == JsonValue::Type::ArrayType) {
		for (int i = 0; i < objectives->size(); i++) {
			std::shared_ptr<JsonValue> item = objectives->get(i);
			if (item == nullptr) {
				CULogError("Objective JSON Object %d is nullptr", i);
				continue;
			}

			std::shared_ptr<DayObjective> newObjective = std::make_shared<DayObjective>();
			newObjective->setComplete(false);
			try {

				newObjective->setId(i);
				newObjective->setName(item->get("name")->asString());
				newObjective->setType(item->get("type")->asString());
				newObjective->setQuantity(item->get("quantity")->asInt());
				newObjective->setReqTime(item->get("time")->asFloat());
				newObjective->setReqAccuracy(item->get("accuracy")->asFloat());
				newObjective->setTargetName(item->get("ingredient")->asString());
				newObjective->setReward(item->get("reward")->asString());
			}
			catch (...) {
				CULogError("Error creating objective %d", i);
			}
			_bonusObjectives.push_back(newObjective);
		}
	}
	else {
		CULogError("Objectives is not an array type");
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

	//if game state is in pre state, show bonus objectives
	if (_gameState == 0) {
		if (_currentTime >= 5.0f) {
			_gameState = 1;
			showObjectiveNodes(false);
			_currentTime = 0;
		}
		else {
			return;
		}
		
	} 



	//timer stuff
	_currentMinute = (5 * (int) _currentTime) - (60 * (_currentHour-6));
	if (_currentMinute >= 60) {
		_currentMinute = _currentMinute % 60;
		_currentHour += 1;
	}
	std::string minString = std::to_string(_currentMinute);
	if (_currentMinute < 10) {
		minString = "0" + std::to_string(_currentMinute);
	}
	_twelveHourTimer->setText(std::to_string(_currentHour) + ":" + minString);
	_twelveHourTimer->doLayout();
	

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

	//todo bonus objectives
	bool oneBonusObjectiveIncomplete = false;

	for (int i = 0; i < _bonusObjectives.size(); i++) {
		if (!_bonusObjectives[i]->getComplete()) oneBonusObjectiveIncomplete = true;
	}

	if (_currentTime >= _dayDuration || (!oneBonusObjectiveIncomplete && _currentScore >= _quota)) {
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
			_ingredientCompletionTimes[submittedIng->getName()] = _currentTime;
			_ingredientCompletionCounts[submittedIng->getName()] += 1;
		}
	}

	

	

	if (_scenes[_curr]->getJustCompletedGesture()) {
		_gestureInitiatedTime = _currentTime;
		_gestureFeedback->setPositionY(_size.height - _gestureFeedback->getHeight());
		int lastResult = _scenes[_curr]->getLastResult();
		_gestureFeedback->setText(_feedbackMessages[lastResult], true);
		std::shared_ptr<Ingredient> ing = _scenes[_curr]->getIngredientInStation();
		if (ing != nullptr) {
			float sim = _scenes[_curr]->getCurrentSimilarity();

			_ingredientAccuracy[ing->getName()] = sim;
		}
		
	}
	if (_currentTime - _gestureInitiatedTime < FEEDBACK_DURATION) {
		int lastResult = _scenes[_curr]->getLastResult();
		if (lastResult != -1) {
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
			//CULog("Button Pos Before: %f %f", button->getPositionX(), button->getPositionY());
			button->setPosition(button->getPosition() + movementAmount);
			//CULog("Button Pos After: %f %f", button->getPositionX(), button->getPositionY());
		}
	
	}
	else {
		_animating = false;
	}


	
	
	// find current touch position, set curHeld->button to be that pos
	for (int i = 0; i < _bonusObjectives.size(); i++) {
		checkObjectiveCompletion(_bonusObjectives[i]);
	}
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

	std::string newStationName = _scenes[targId]->getName();
	for (char& c : newStationName) {
		c = toupper(c);
	}
	_stationLabel->setText(newStationName + " STATION");
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
	_currentHour = 6;
	_currentMinute = 0;
	_gestureFeedback->setVisible(false);
	_gestureFeedback->setText("");
	_currentScore = 0;
	_gameState = 0;
	_ingredientAccuracy.clear();
	_ingredientCompletionTimes.clear();
	_ingredientCompletionCounts.clear();


	for (int i = 0; i < _bonusObjectives.size(); i++) {
		_bonusObjectives[i]->setComplete(false);
	}

	

	showObjectiveNodes(true);


	
	for (int i = 0; i < 5; i++) {
		_scenes[i]->reset();
	}
	_winScreenRoot->setVisible(false);

	focusCurr();
}

void MultiScreenScene::save() {
	//save the current state of the game
	//should only change daytime and persistent save data
	std::string root = cugl::Application::get()->getSaveDirectory();
	std::string path = cugl::filetool::join_path({ root,"save.json" });

	auto reader = JsonReader::alloc(path);
	auto writer = JsonWriter::alloc(path);

	std::shared_ptr<JsonValue> json = JsonValue::allocObject();

	//placeholder values
	json->appendValue("chapter", 1.0f);
	json->appendValue("level", 1.0f);

	std::shared_ptr<JsonValue> day = JsonValue::allocArray();

	json->appendChild("day", day);

}

void MultiScreenScene::loadSave() {
	std::string root = cugl::Application::get()->getSaveDirectory();
	std::string path = cugl::filetool::join_path({ root,"save.json" });
	auto reader = JsonReader::alloc(path);
}



void MultiScreenScene::endDay() {
	_ended = true;
	if (_currentScore >= _quota) {
		_gameState = 1;
	}
	else {
		_gameState = -1;
	}

	std::shared_ptr<scene2::Label> statUps = std::dynamic_pointer_cast<scene2::Label>(_winScreenRoot->getChildByName("statUps"));
	std::ostringstream textStream;
	for (int i = 0; i < _bonusObjectives.size(); i++) {
		
		if (_bonusObjectives[i]->getComplete()) {
			if (i != 0) {
				textStream << ", ";
			}
			textStream << "+" << _bonusObjectives[i]->getReward();
		}
	}
	if (textStream.str() == "") {
		statUps->setText("no bonus objectives completed");
	}
	else {
		statUps->setText(textStream.str(), true);
	}
	statUps->setPosition(750, 220);
	statUps->setWrap(true);

	for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
		auto button = *it;
		button->activate();
	}
	_winScreenRoot->setVisible(true);
}


std::shared_ptr<scene2::PolygonNode> MultiScreenScene::createObjectiveNode(std::shared_ptr<DayObjective> obj) {
	std::shared_ptr<scene2::PolygonNode> objNode = scene2::PolygonNode::allocWithPoly(Rect(0,0,OBJ_CARD_WIDTH, OBJ_CARD_HEIGHT));
	std::string objSceneName = "dayObjective" + std::to_string(obj->getId());
	objNode->setName(objSceneName);
	objNode->setColor(Color4::GRAY);

	std::shared_ptr<scene2::Label> objName = scene2::Label::allocWithText(obj->getName(), _assets->get<Font>("winter drinkRegular40"));
	objName->setContentSize(Vec2(OBJ_CARD_WIDTH, OBJ_CARD_HEIGHT));
	objName->setWrap(true);
	objName->setHorizontalAlignment(HorizontalAlign::CENTER);
	//objName->setScale(Vec2(0.5, 0.5));
	
	objNode->addChild(objName);

	float descStartHeight = OBJ_CARD_HEIGHT / 4;
	float descPadding = 15;

	std::string objType = obj->getType();
	std::shared_ptr<scene2::PolygonNode> objImage;
	std::shared_ptr<scene2::Label> objText = scene2::Label::allocWithText("", _assets->get<Font>("winter drinkRegular25"));
	objText->setPosition(Vec2(descPadding, descStartHeight));

	if (objType == "rush") {

		std::ostringstream textStream;
		textStream << "Complete " << obj->getQuantity() << " dishes in " << std::fixed << std::setprecision(1) << obj->getReqTime() << " seconds";
		objText->setText(textStream.str(), true);

		objImage = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("rushObjective"));

	}
	else if (objType == "accuracy") {

		std::ostringstream textStream;
		textStream << "Cut carrots with an accuracy of at least " << std::fixed << std::setprecision(1) << obj->getReqAccuracy();
		objText->setText(textStream.str(), true);
		

		objImage = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("accuracyObjective"));
	}

	objText->setWrap(true);
	objText->setContentSize(Vec2(OBJ_CARD_WIDTH - descPadding, OBJ_CARD_HEIGHT / 2));

	objImage->setContentSize(Vec2(2 * OBJ_CARD_WIDTH / 3, 2 * OBJ_CARD_WIDTH / 3));
	objImage->setAnchor(Vec2::ANCHOR_CENTER);
	objImage->setPosition(OBJ_CARD_WIDTH/2, OBJ_CARD_HEIGHT/3);

	objNode->addChild(objText);
	objNode->addChild(objImage);


	std::shared_ptr<scene2::Label> rewardText = scene2::Label::allocWithText("Reward: " + obj->getReward(), _assets->get<Font>("winter drinkRegular25"));
	rewardText->setContentSize(Vec2(OBJ_CARD_WIDTH, OBJ_CARD_HEIGHT / 8));
	rewardText->setHorizontalAlignment(HorizontalAlign::CENTER);
	objNode->addChild(rewardText);

	return objNode;
}

void MultiScreenScene::showObjectiveNodes(bool val) {
	for (int i = 0; i < _bonusObjectives.size(); i++) {
		std::string childToGet = "dayObjective" + std::to_string(i);
		_uiScene->getChildByName(childToGet)->setVisible(val);
	}
}

void MultiScreenScene::checkObjectiveCompletion(std::shared_ptr<DayObjective> obj) {

	std::string objType = obj->getType();
	if (objType == "rush") {

		if (obj->getComplete()) return;

		//have to use iterator here, because accessing by [] on an undefined key will return a default value
		//which would be 0.0f which would always mark as complete
		float completionTime;
		auto iterator = _ingredientCompletionTimes.find(obj->getTargetName());
		if (iterator != _ingredientCompletionTimes.end()) {
			completionTime = iterator->second;
		}
		else {
			completionTime = FLT_MAX;
		}
		
		int completionCount = _ingredientCompletionCounts[obj->getTargetName()];
		if (completionCount >= obj->getQuantity() && completionTime <= obj->getReqTime()) {
			obj->setComplete(true);
			CULog("completed rush obj");
		}
	}
	else if (objType == "accuracy") {
		//no need for that here, because 0.0 will always be less than required accuracy
		float itemAccuracy = _ingredientAccuracy[obj->getTargetName()];
		if (itemAccuracy >= obj->getReqAccuracy()) {
			obj->setComplete(true);
			CULog("win accuracy");
		}
		else {
			obj->setComplete(false);
		}
	}
	else {
		CULogError("Attempted to Check Completion of Unimplemented Objective Type");
	}
}
