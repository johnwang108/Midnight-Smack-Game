//
//  PFGameScene.cpp
//  PlatformDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our ObstacleWorld class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design. This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together. However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden (2007).
//  This file has been refactored to support the physics changes in CUGL 2.5.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/24
//

#include "PFGameScene.h"
#include "PFDollarScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>


#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>
#include <algorithm>

#define WIDTH 25


#define SMALL_MSG "retrosmall"  

#define DOLLAR_THRESHOLD 0.7

#define GOOD_THRESHOLD 0.7
#define PERFECT_THRESHOLD 0.85
#define CONVEYOR_SPEED 2.0f
#define FALL_SPEED 3.0f
#define LAUNCH_X_POS 1100
#define HORIZ_LAUNCH_SPEED 4.0f
#define VERT_LAUNCH_SPEED -3.0f

using namespace cugl;

float SHAPE[] = { 0,300,300,300,300,0,0,0} ;

//float SHAPE[] = { 0, 50, 10.75, 17, 47, 17,
//17.88, -4.88, 29.5, -40.5, 0, -18.33,
//-29.5, -40.5, -17.88, -4.88, -47, 17,
//-10.75, 17 };


DollarScene::DollarScene() : scene2::SceneNode() {
	_assets = nullptr;
	_stationHitbox = nullptr;
}

void DollarScene::dispose() {
	//input will be disposed by root node dispose call
	//_input.dispose();
	_assets = nullptr;
}

bool DollarScene::init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::string texture) {
	//FIX - JOHN
	std::shared_ptr<cugl::Texture> t = assets->get<cugl::Texture>(texture);
	cugl::Rect rect = cugl::Rect(Vec2::ZERO, t->getSize());
	
	return init(assets, input, rect, texture);
}

//main init
bool DollarScene::init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures) {
	_input = input;
	_assets = assets;
	_se = cugl::SimpleExtruder();
	_childOffset = -1;
	_combined = Affine2::IDENTITY;
	_focus = false;
	countdown = 0;
	_currentTargetGestures = gestures;
	_currentTargetIndex = 0;
	_completed = false;
	_currentSimilarity = 0;
	_lastResult = -1;
	_justCompletedGesture = false;
	_currentlyHeldIngredient = nullptr;
	_submittedIngredient = nullptr;
	_validIngredients = std::vector<std::string>();
	//todo fix this
	_readyToCook = false;
	initGestureRecognizer();
	//reflection across the x axis is necessary for polygon path
	/**
	* 1 0
	* 0 -1
	* transform is screen width/2, screen height/2 for mouse->screen coordinates
	*/
	_transf = Affine2(1, 0, 0, -1, 0, SCENE_HEIGHT);
	_poly = cugl::scene2::PolygonNode::alloc();
	_box = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>(texture), rect);
	_box->setAnchor(Vec2::ANCHOR_CENTER);

	//default pigtail
	_currentTargetGestures = gestures;

	_header = scene2::Label::allocWithText("Gestures, Similarity: t tosdgodfho figjgoj ghkohko ", _assets->get<Font>(SMALL_MSG));
	_header->setAnchor(Vec2::ANCHOR_CENTER);
	_header->setPosition(cugl::Vec2(640, 600));
	_header->setForeground(cugl::Color4::RED);
	_header->setHorizontalAlignment(HorizontalAlign::CENTER);
	_header->setVisible(false);

	_currentGestureLabel = scene2::Label::allocWithText("Current Target Gesture: filling out text bc i swear this always breaks", _assets->get<Font>(SMALL_MSG));
	_currentGestureLabel->setAnchor(Vec2::ANCHOR_CENTER);
	_currentGestureLabel->setPosition(cugl::Vec2(640, 540));
	_currentGestureLabel->setForeground(cugl::Color4::BLACK);
	_currentGestureLabel->setHorizontalAlignment(HorizontalAlign::CENTER);
	_currentGestureLabel->setVisible(false);

	_indicatorGroup = scene2::SceneNode::alloc();
	_indicatorGroup->setPosition(Vec2(640, 400));
	std::shared_ptr<scene2::PolygonNode> indicator = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("indicator"));
	indicator->setAnchor(Vec2::ANCHOR_CENTER);
	indicator->setContentSize(indicator->getSize() * .3);
	indicator->setPosition(-25, 50);
	_indicatorGroup->addChild(indicator);
	_indicatorGroup->setVisible(false);


	addChild(_box);
	if (_stationHitbox != nullptr) {
		addChild(_stationHitbox);
	}
	addChild(_poly);
	addChild(_header);
	addChild(_currentGestureLabel);
	addChild(_indicatorGroup);

	_inputtedGestures = std::vector<Path2>();

	update(0);

	return true;
}

bool DollarScene::init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures, Size hitboxSize) {
	_stationHitbox = scene2::PolygonNode::allocWithPoly(Rect(Vec2(0, 0), hitboxSize));
	//_stationHitbox = scene2::PolygonNode::allocWithBounds(hitboxSize);
	//_stationHitbox->setAnchor(Vec2::ANCHOR_CENTER);
	_stationHitbox->setPosition(640,400);
	//_stationHitbox->setColor(Color4::GRAY);
	_stationHitbox->setVisible(false);
	Rect box = _stationHitbox->getBoundingRect();
	CULog("Min X: %f, Max X: %f, Min Y: %f, Max Y: %f", box.getMinX(), box.getMaxX(), box.getMinY(), box.getMaxY());
	
	return init(assets, input, rect, texture, gestures);
}


//later on, we could pass all the gestures for the level in, so we don't recognize any gestures we don't need
bool DollarScene::initGestureRecognizer() {
	_dollarRecog = cugl::GestureRecognizer::alloc();
	if (!_dollarRecog->init()) return false;
	_dollarRecog->setAlgorithm(cugl::GestureRecognizer::Algorithm::ONEDOLLAR);

	std::vector<Vec2> vVertices = { Vec2(89,164),Vec2(90,162),Vec2(92,162),Vec2(94,164),Vec2(95,166),Vec2(96,169),Vec2(97,171),Vec2(99,175),Vec2(101,178),Vec2(103,182),Vec2(106,189),Vec2(108,194),Vec2(111,199),Vec2(114,204),Vec2(117,209),Vec2(119,214),Vec2(122,218),Vec2(124,222),Vec2(126,225),Vec2(128,228),Vec2(130,229),Vec2(133,233),Vec2(134,236),Vec2(136,239),Vec2(138,240),Vec2(139,242),Vec2(140,244),Vec2(142,242),Vec2(142,240),Vec2(142,237),Vec2(143,235),Vec2(143,233),Vec2(145,229),Vec2(146,226),Vec2(148,217),Vec2(149,208),Vec2(149,205),Vec2(151,196),Vec2(151,193),Vec2(153,182),Vec2(155,172),Vec2(157,165),Vec2(159,160),Vec2(162,155),Vec2(164,150),Vec2(165,148),Vec2(166,146) };
	cugl::Path2 vGesturePath = cugl::Path2(vVertices);

	if (!_dollarRecog->addGesture("v", vGesturePath, true)) {
		CULog("failed to initialize v");
		return false;
	}

	std::vector<Vec2> circVertices = { Vec2(127,141),Vec2(124,140),Vec2(120,139),Vec2(118,139),Vec2(116,139),Vec2(111,140),Vec2(109,141),Vec2(104,144),Vec2(100,147),Vec2(96,152),Vec2(93,157),Vec2(90,163),Vec2(87,169),Vec2(85,175),Vec2(83,181),Vec2(82,190),Vec2(82,195),Vec2(83,200),Vec2(84,205),Vec2(88,213),Vec2(91,216),Vec2(96,219),Vec2(103,222),Vec2(108,224),Vec2(111,224),Vec2(120,224),Vec2(133,223),Vec2(142,222),Vec2(152,218),Vec2(160,214),Vec2(167,210),Vec2(173,204),Vec2(178,198),Vec2(179,196),Vec2(182,188),Vec2(182,177),Vec2(178,167),Vec2(170,150),Vec2(163,138),Vec2(152,130),Vec2(143,129),Vec2(140,131),Vec2(129,136),Vec2(126,139) };
	cugl::Path2 circGesturePath = cugl::Path2(circVertices);

	if (!_dollarRecog->addGesture("circle", circGesturePath, true)) {
		CULog("failed to initialize circle");
		return false;
	}

	std::vector<Vec2> pigtailVert = { Vec2(81,219),Vec2(84,218),Vec2(86,220),Vec2(88,220),Vec2(90,220),Vec2(92,219),Vec2(95,220),Vec2(97,219),Vec2(99,220),Vec2(102,218),Vec2(105,217),Vec2(107,216),Vec2(110,216),Vec2(113,214),Vec2(116,212),Vec2(118,210),Vec2(121,208),Vec2(124,205),Vec2(126,202),Vec2(129,199),Vec2(132,196),Vec2(136,191),Vec2(139,187),Vec2(142,182),Vec2(144,179),Vec2(146,174),Vec2(148,170),Vec2(149,168),Vec2(151,162),Vec2(152,160),Vec2(152,157),Vec2(152,155),Vec2(152,151),Vec2(152,149),Vec2(152,146),Vec2(149,142),Vec2(148,139),Vec2(145,137),Vec2(141,135),Vec2(139,135),Vec2(134,136),Vec2(130,140),Vec2(128,142),Vec2(126,145),Vec2(122,150),Vec2(119,158),Vec2(117,163),Vec2(115,170),Vec2(114,175),Vec2(117,184),Vec2(120,190),Vec2(125,199),Vec2(129,203),Vec2(133,208),Vec2(138,213),Vec2(145,215),Vec2(155,218),Vec2(164,219),Vec2(166,219),Vec2(177,219),Vec2(182,218),Vec2(192,216),Vec2(196,213),Vec2(199,212),Vec2(201,211) };
	cugl::Path2 pigtailGesturePath = cugl::Path2(pigtailVert);

	if (!_dollarRecog->addGesture("pigtail", pigtailGesturePath, true)) {
		CULog("failed to initialize pigtail");
		return false;
	}

	return true;
}


//re-extrudes the path and updates the polygon node
void DollarScene::update(float timestep) {
	//pop new path if this node is focused on and the input controller contains a nonempty path.
	_justCompletedGesture = false;
	if (_focus && _readyToCook) {
		CULog("cooking");
		if (!(_input->getTouchPath().empty())) {
			// for spline
			_path = _input->getTouchPath();
		}
		if (!_isNighttime && !_currentTargetGestures.empty()) {
			if (matchWithTouchPath()) {
				_lastResult = gestureResult();
				_justCompletedGesture = true;
				if (_currentTargetIndex < _currentTargetGestures.size() - 1) _currentTargetIndex++;
				else {
					_completed = true;
				}
			}
			if (!_completed) {
				_currentGestureLabel->setText("Current Target Gesture: " + _currentTargetGestures[_currentTargetIndex]);
				_currentGestureLabel->setVisible(true);
				_currentGestureLabel->doLayout();
				_header->setText("Similarity: " + std::to_string(_currentSimilarity));
				_header->setVisible(true);
				_header->doLayout();
			}
			else {
				_currentGestureLabel->setVisible(false);
				_header->setVisible(false);
			}
		}
		//nighttime
		else if (_isNighttime && isFocus()) {
			if (_input->isGestureCompleted()) {
				Path2 gesture = _path;
				_input->popTouchPath();
				_inputtedGestures.push_back(gesture);
				if (_inputtedGestures.size() == NIGHT_SEQUENCE_LENGTH) {
					_completed = true;
				}
			}
		}

	}

	if (_readyToCook) {
		//re-extrude path
		_se.set(_path);
		_se.calculate(WIDTH);
		
		//have to reflect across x axis with _transf
		cugl::Affine2 t = cugl::Affine2(1, 0, 0, -1, 0,SCENE_HEIGHT);
		_poly->setPolygon(_se.getPolygon() * t);
		_poly->setColor(cugl::Color4::BLACK);
		_poly->setAnchor(cugl::Vec2::ANCHOR_CENTER);
		_poly->setScale(_scale);

		_poly->setAbsolute(true);
		_poly->setVisible(true);

		//TODO animate station
		//TODO show ingredient indicator
	}

	if (_completed) {
		if (_isNighttime) {
			//see which sequence's first gesture is more similar
			float sim1 = _dollarRecog->similarity(_currentTargetGesturesNighttime[0][0], _inputtedGestures[0]);
			float sim2 = _dollarRecog->similarity(_currentTargetGesturesNighttime[1][0], _inputtedGestures[0]);
			float totalSim;
			if (sim1 > sim2) {
				totalSim = (sim1 + _dollarRecog->similarity(_currentTargetGesturesNighttime[0][1], _inputtedGestures[1])
					+ _dollarRecog->similarity(_currentTargetGesturesNighttime[0][2], _inputtedGestures[2]))/3.0f;
				_isDurationSequence = true;
			}
			else {
				totalSim = (sim2 + _dollarRecog->similarity(_currentTargetGesturesNighttime[1][1], _inputtedGestures[1])
					+ _dollarRecog->similarity(_currentTargetGesturesNighttime[1][2], _inputtedGestures[2]))/3.0f;
				_isDurationSequence = false;
			}
			CULog("Total sim: %f", totalSim);
			_currentSimilarity = totalSim;
			_lastResult = gestureResult();
			return;
		}

		_completed = false;
		_readyToCook = false;
		_indicatorGroup->setVisible(false);


		//TODO spit out ingredient
		//remove ingredient from indicator 
		for (std::shared_ptr<scene2::SceneNode> child : _indicatorGroup->getChildren()) {
			std::shared_ptr<Ingredient> ing = getIngredientInStation();
			if (ing != nullptr && child == ing->getButton()) {
				_indicatorGroup->removeChild(child);
			}
		}
		handleCompletedIngredient(getIngredientInStation());
	}


	//_box->setPosition(cugl::Vec2(0, 0));


	//_header->setVisible(!isPending() && isSuccess());
	updateConveyor();

	//if not holding anything, check if we are
	if (_currentlyHeldIngredient == nullptr) {
		_currentlyHeldIngredient = findHeldIngredient();
	}
	if (_currentlyHeldIngredient != nullptr){
		//we think we are holding something, so check if it's still being held
		if (!_currentlyHeldIngredient->getBeingHeld()) {
			//let go
			_currentlyHeldIngredient->setFalling(true);
			_currentlyHeldIngredient = nullptr;
		}
		else {
			//ingredient is being held so transform to mouse
			std::shared_ptr<scene2::Button> button = _currentlyHeldIngredient->getButton();
			//CULog("%f, %f", _input->getTouchPos().x, _input->getTouchPos().y);
			Vec2 transformedMouse = _input->getTouchPos() * _transf;

			button->setPositionX(transformedMouse.x);// +_currentlyHeldIngredient->getButton()->getWidth() / 2);
			button->setPositionY(transformedMouse.y);

			//CULog("Button Pos X: %f, Button Y: %f", button->getPositionX(), button->getPositionY());
			//button->setPosition(_input->getTouchPos());
		}
	}

	for (std::shared_ptr<Ingredient> ing : _currentIngredients) {
		//CULog("Ing Pos: %f, Ing Pos Y: %f", ing->getButton()->getPosition().x, ing->getButton()->getPosition().y);

		if (!ing->inPot() && ing->isFalling() && !_readyToCook && _stationHitbox->inContentBounds(ing->getButton()->getPosition())) {
			//add ingredient to pot somehow lmao
			bool isValidIng = false;

			for (std::string station_name : ing->getValidStations()) {
				if (getName() == station_name) isValidIng = true;
			}

			if (isValidIng) {
				addIngredientToStation(ing);
			}
		}
		else if (ing->isLaunching()) {
			std::shared_ptr<scene2::Button> button = ing->getButton();
			if (button->getPositionX() >= LAUNCH_X_POS) {
				ing->setLaunching(false);
				ing->setFalling(true);
			}
			else {
				Vec2 curPos = button->getPosition();
				button->setPosition(curPos + Vec2(HORIZ_LAUNCH_SPEED, VERT_LAUNCH_SPEED));
			}
		}
	}
};

//is gesture inputting still in progress?
bool DollarScene::isPending() {
	//TODO
	//return !_input->isGestureCompleted();
	return !_completed;
};

//if gesture is completed. pop gesture and calculate similarity. if nighttime, record the inputted gesture path
bool DollarScene::matchWithTouchPath() {
	if (_input->isGestureCompleted()) {
			CULog("COMPLETED GESTURE");
			Path2 gesture = _path;
			_input->popTouchPath();
			if (gesture.size() > 3) {
				float sim = _dollarRecog->similarity(_currentTargetGestures[_currentTargetIndex], gesture);
				if (sim >= 0) _currentSimilarity = sim;

			}
			return true;
	}
	return false;
}

//is gesture inputting a success?
int DollarScene::gestureResult() {
	//CULog("%d", (int)(_currentSimilarity > GOOD_THRESHOLD) + (int)(_currentSimilarity > PERFECT_THRESHOLD));
	return (int)(_currentSimilarity > GOOD_THRESHOLD) + (int)(_currentSimilarity > PERFECT_THRESHOLD);
};

void DollarScene::setFocus(bool focus) {
	_focus = focus;
	if (focus) {
		_input->popTouchPath();
	}
	if (_isNighttime) {
		CULog("clearing");
		_inputtedGestures.clear();
	}
}

void DollarScene::setBottomBar(std::shared_ptr<cugl::scene2::SceneNode> bar) {
	//todo fix this 
	_readyToCook = false;
	_bottomBar = bar;
	if (_bottomBar != nullptr) {
		_conveyorBelt = _bottomBar->getChildByName("kitchenbar")->getChildByName("updateConveyorBelt");
		std::shared_ptr<cugl::scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(_bottomBar->getChildByName("kitchenbar")->getChildByName("Item")->getChildByName("Item")->getChildByName("RecipeBook"));
		butt->addListener([=](const std::string& name, bool down) {
			CULog("COOKBOOK PRESSED");
		});
		butt->activate();
	}
}

void DollarScene::addIngredient(std::shared_ptr<Ingredient> ing) {
	if (_bottomBar == nullptr) {
		CULogError("Trying to add ingredient to Dollar Scene Without Bottom Bar");
		return;
	}
	if (_conveyorBelt == nullptr) {
		CULogError("Trying to add ingredient to Dollar Scene Without Conveyor Belt");
		return;
	}
	_currentIngredients.push_back(ing);
	ing->getButton()->setPosition(_conveyorBelt->getWidth() - ing->getButton()->getWidth() / 2, _conveyorBelt->getHeight() / 2);
	_conveyorBelt->addChild(ing->getButton());
	//CULog("Conv x: %f, Conv y: %f", _conveyorBelt->getPositionX(), _conveyorBelt->getPositionY());
	//CULog("Conv width: %f, conv height: %f", _conveyorBelt->getWidth(), _conveyorBelt->getHeight());
}


void DollarScene::updateConveyor() {
	if (_bottomBar == nullptr) return;
	for (std::shared_ptr<Ingredient> ingredient : _currentIngredients) {
 		if (ingredient == _currentlyHeldIngredient || ingredient->inPot()) continue;
		std::shared_ptr<scene2::SceneNode> button = ingredient->getButton();

		if (ingredient->isFalling()) {
			if (button->getPositionY() <= _conveyorBelt->getHeight() / 2) {
				//error might occur here? might get mad if ingredient not in general scene but idk why it wouldn't be here
				Vec2 scenePos = button->getPosition();
				removeChild(button);
				_conveyorBelt->addChild(button);
				button->setPosition(scenePos - Vec2(_conveyorBelt->getWidth() + button->getWidth()/2, 0));
				ingredient->setFalling(false);
				button->setPositionY(_conveyorBelt->getHeight() / 2);
			}
			else {
				button->setPositionY(button->getPositionY() - FALL_SPEED);
			}
		}
		else if (!ingredient->isLaunching()){
			button->setPositionX(button->getPositionX() - CONVEYOR_SPEED);

			if (button->getPositionX() <= button->getWidth()/2) {
				// mark button switch to next pos
				_ingredientToRemove = ingredient;
			}
		}
		
	}
}

std::shared_ptr<Ingredient> DollarScene::popIngredient() {
	if (_ingredientToRemove == nullptr) {
		return nullptr;
	}
	auto ingIt = std::remove(_currentIngredients.begin(), _currentIngredients.end(), _ingredientToRemove);
	_currentIngredients.erase(ingIt, _currentIngredients.end());
	
	std::shared_ptr<Ingredient> removedIngredient = _ingredientToRemove;
	_ingredientToRemove.reset();
	_conveyorBelt->removeChild(removedIngredient->getButton());
	return removedIngredient;
}

std::shared_ptr<Ingredient> DollarScene::findHeldIngredient() {
	for (std::shared_ptr<Ingredient> ing : _currentIngredients) {
		if (ing->getBeingHeld() == true) {
			ing->setFalling(false);
			ing->setLaunching(false);
			for (std::shared_ptr<scene2::SceneNode> child : _conveyorBelt->getChildren()) {
				if (child == ing->getButton()) {
					_conveyorBelt->removeChild(ing->getButton());
					break;
				}
			}
			if (ing->getButton()->getParent() == nullptr) addChild(ing->getButton());


			return ing;
		}
	}
	return nullptr;
}

void DollarScene::removeHeldIngredient() {
	//remove from this scenes scene node
	for (std::shared_ptr<scene2::SceneNode> child : getChildren()) {
		if (child == _currentlyHeldIngredient->getButton()) {
			removeChild(child);
			break;
		}
	}

	//remove it from current ingredients deque
	auto it = std::find(_currentIngredients.begin(), _currentIngredients.end(), _currentlyHeldIngredient);
	if (it != _currentIngredients.end()) {
		_currentIngredients.erase(it);
	}

	//set it to no longer be current held ingredient
	_currentlyHeldIngredient.reset();
}

void DollarScene::receiveHeldIngredient(std::shared_ptr<Ingredient> ing) {
	addChild(ing->getButton());
	_currentIngredients.push_back(ing);
	_currentlyHeldIngredient = ing;
}

void DollarScene::addIngredientToStation(std::shared_ptr<Ingredient> ing) {
	CULog("added to pot");
	//Remove ingredient from conveyor belt, and remove gravity
	//use loop because if it's not on conveyor belt then it'll error if u try to remove
	// todo maybe swap to un parenting from scene
	for (std::shared_ptr<scene2::SceneNode> child : _conveyorBelt->getChildren()) {
		if (child == ing->getButton()) {
			CULog("removed");
			_conveyorBelt->removeChild(ing->getButton());
			break;
		}
	}	
	ing->setFalling(false);

	// this could be time intensive?
	for (std::shared_ptr<scene2::SceneNode> child : getChildren()) {
		if (child == ing->getButton()) {
			CULog("removed");
			removeChild(ing->getButton());
			break;
		}
	}

	// any ingredient added to prep is submitted
	if (getName() == "prep") {
		submitIngredient(ing);
		return;
	}


	//add it to the station, and let both ingredient and station know. Also store which is ing is in
	ing->setInPot(true);
	setIngredientInStation(ing);
	_readyToCook = true;


	//indicator positioning
	std::shared_ptr<scene2::Button> button = ing->getButton();
	
	button->deactivate();
	//content size isn't changing??
	//button->setContentSize(button->getContentSize() / 4);
	button->setPosition(-button->getWidth()/2, button->getHeight()/2);
	
	//add the ingredient to indicator and show indicator
	_indicatorGroup->addChild(ing->getButton());
	_indicatorGroup->setVisible(true);

	// make sure it tosses out dragging from moving ingredient first
	_input->popTouchPath();

	//prep gestures and flag ready
	_currentTargetGestures = ing->getGestures();
	_currentTargetIndex = 0;
	_completed = false;
}

void DollarScene::handleCompletedIngredient(std::shared_ptr<Ingredient> ing) {
	//remove ingredient from station for sure
	_currentTargetGestures.clear();
	if (ing == nullptr) { return; }

	_ingredientInStation.reset();
	ing->setInPot(false);

	//CULog("%s", getName().c_str());
	//CULog("Ing name: %s", ing->getName().c_str());
	//put this somewhere better
	std::shared_ptr<JsonReader> newTextureReader = JsonReader::allocWithAsset("json/dayIngredientGestures.json");
	std::shared_ptr<JsonValue> newTextureVals = newTextureReader->readJson();

	std::string ingName = ing->getName();
	ingName[0] = toupper(ingName[0]);
	//CULog("%s", (getName() + ingName).c_str());
	std::shared_ptr<JsonValue> newIngredientJSON = newTextureVals->get((getName() + ingName));
	ing->setName(getName() + ingName);


	std::shared_ptr<Texture> tex = _assets->get<Texture>(newIngredientJSON->get("texture")->asString());
	ing->init(tex);
	std::shared_ptr<scene2::Button> button = ing->getButton();


	std::vector<std::string> gestures = {};

	std::shared_ptr<JsonValue> jGests = newIngredientJSON->get("gestures");
	if (jGests->type() == JsonValue::Type::ArrayType) {
		for (int j = 0; j < jGests->size(); j++) {
			gestures.push_back(jGests->get(j)->asString());
		}
	}
	else {
		CULogError("Gestures is not an array type");
	}

	ing->setGestures(gestures);
	std::vector<std::string> stations = {};

	std::shared_ptr<JsonValue> jStations = newIngredientJSON->get("valid_stations");
	if (jStations->type() == JsonValue::Type::ArrayType) {
		for (int j = 0; j < jStations->size(); j++) {
			stations.push_back(jStations->get(j)->asString());
		}
	}
	else {
		CULogError("Valid Stations is not an array type");
	}

	ing->setValidStations(stations);

	addChild(button);
	button->activate();
	button->setPosition(Vec2(800, 400));

	launchIngredient(ing);
}


//draws a boundary rectangle
//void DollarScene::draw(const std::shared_ptr<SpriteBatch>& batch, const Affine2& transform, Color4 tint) {
//
//	batch->begin();
//
//	batch->setColor(tint);
//
//	cugl::Vec2* verts = reinterpret_cast<Vec2*>(SHAPE);
//	cugl::Poly2 poly = cugl::Poly2(verts, sizeof(SHAPE) / sizeof(float) / 2);
//
//	batch->fill(poly, cugl::Vec2(0,0), transform);
//
//	//batch->end();
//};
void DollarScene::reset() {
	_currentTargetIndex = 0;
	_header->setVisible(false);
	_header->setText("");
	_currentGestureLabel->setVisible(false);
	_currentGestureLabel->setText("");
	_currentSimilarity = 0;
	_justCompletedGesture = 0;
	_currentTargetGestures = {};
	_focus = false;
	_completed = false;
	_lastResult = -1;
	_inputtedGestures.clear();
	//todo ready to cook idk if it should be false
	_readyToCook = false;
	if (_currentlyHeldIngredient != nullptr) {
		for (std::shared_ptr<scene2::SceneNode> child : getChildren()) {
			if (child == _currentlyHeldIngredient->getButton()) removeChild(_currentlyHeldIngredient->getButton());
		}
		_currentlyHeldIngredient.reset();
	}
	

	// clear conveyor
	for (std::shared_ptr<scene2::SceneNode> child : _conveyorBelt->getChildren()) {
		_conveyorBelt->removeChild(child);
	}


	//clear indicator and station
	for (std::shared_ptr<scene2::SceneNode> child : _indicatorGroup->getChildren()) {
		std::shared_ptr<Ingredient> ing = getIngredientInStation();
		if (ing != nullptr && child == ing->getButton()) {
			_indicatorGroup->removeChild(child);
		}
	}
	_indicatorGroup->setVisible(false);


	//clear gestures
	_input->popTouchPath();
	_poly->setVisible(false);


	_ingredientInStation = nullptr;


	//clear all ingredients
	_currentIngredients.clear();

	_ingredientToRemove.reset();

	// Don't do this so we don't have to re-init bottom bar
	//_conveyorBelt = nullptr;
}

void DollarScene::launchIngredient(std::shared_ptr<Ingredient> ing) {
	ing->setFalling(false);
	ing->setLaunching(true);
}

void DollarScene::submitIngredient(std::shared_ptr<Ingredient> ing) {
	_submittedIngredient = ing;
	auto it = std::find(_currentIngredients.begin(), _currentIngredients.end(), ing);
	if (it != _currentIngredients.end()) {
		_currentIngredients.erase(it);
	}
	CULog("Submitted Ingredient");
}