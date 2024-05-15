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

using namespace cugl;

float SHAPE[] = { 0,300,300,300,300,0,0,0} ;

//float SHAPE[] = { 0, 50, 10.75, 17, 47, 17,
//17.88, -4.88, 29.5, -40.5, 0, -18.33,
//-29.5, -40.5, -17.88, -4.88, -47, 17,
//-10.75, 17 };


DollarScene::DollarScene() : scene2::SceneNode() {
	_assets = nullptr;
}

void DollarScene::dispose() {
	//input will be disposed by root node dispose call
	//_input.dispose();
	_assets = nullptr;
}

bool DollarScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::string texture) {
	//FIX - JOHN
	std::shared_ptr<cugl::Texture> t = assets->get<cugl::Texture>(texture);
	cugl::Rect rect = cugl::Rect(Vec2::ZERO, t->getSize());
	
	return init(assets, input, rect, texture);
}

//main init
bool DollarScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures) {
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
	_validIngredients = std::vector<std::string>();
	//todo fix this
	_readyToCook = false;
	initGestureRecognizer();
	//initAlternateGestures();
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


	addChild(_box);
	addChild(_poly);
	addChild(_header);
	addChild(_currentGestureLabel);

	_inputtedGestures = std::vector<Path2>();

	update(0);

	return true;
}

bool DollarScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures, Size hitboxSize) {
	return init(assets, input, rect, texture, gestures);
}


//later on, we could pass all the gestures for the level in, so we don't recognize any gestures we don't need
bool DollarScene::initGestureRecognizer() {
	_dollarRecog = cugl::GestureRecognizer::alloc();
	if (!_dollarRecog->init()) return false;
	_dollarRecog->setAlgorithm(cugl::GestureRecognizer::Algorithm::ONEDOLLAR);

	cugl::Path2 vGesturePath = cugl::Path2(GesturePaths::vVertsPrimary);

	if (!_dollarRecog->addGesture("v", vGesturePath, true)) {
		CULog("failed to initialize v");
		return false;
	}

	cugl::Path2 circGesturePath = cugl::Path2(GesturePaths::circVertsPrimary);

	if (!_dollarRecog->addGesture("circle", circGesturePath, true)) {
		CULog("failed to initialize circle");
		return false;
	}

	cugl::Path2 pigtailGesturePath = cugl::Path2(GesturePaths::pigtailVertsPrimary);

	if (!_dollarRecog->addGesture("pigtail", pigtailGesturePath, true)) {
		CULog("failed to initialize pigtail");
		return false;
	
	}

	cugl::Path2 caretGesturePath = cugl::Path2(GesturePaths::caretVertsPrimary);
	if(!_dollarRecog->addGesture("caret", caretGesturePath, true)) {
		CULog("failed to initialize caret");
		return false;
	}

	cugl::Path2 triangleGesturePath = cugl::Path2(GesturePaths::triangleVertsPrimary);
	if (!_dollarRecog->addGesture("triangle", triangleGesturePath, true)) {
		CULog("failed to initialize triangle");
		return false;
	}

	//_dollarRecog->addGestures(, false);
	return true;
}

bool DollarScene::initAlternateGestures() {
	std::vector<Vec2> reversedVVerts = GesturePaths::vVertsPrimary;
	std::reverse(reversedVVerts.begin(), reversedVVerts.end());
	cugl::Path2 reverseVGesturePath = cugl::Path2(reversedVVerts);
	if (!_dollarRecog->addGesture("v", reverseVGesturePath, true)) {
		CULog("failed to initialize reverse v");
		return false;
	}

	std::vector<Vec2> reversedCircVerts = GesturePaths::circVertsPrimary;
	std::reverse(reversedCircVerts.begin(), reversedCircVerts.end());
	cugl::Path2 reverseCircGesturePath = cugl::Path2(reversedCircVerts);
	if (!_dollarRecog->addGesture("circle", reverseCircGesturePath, true)) {
		CULog("failed to initialize reverse circle");
		return false;
	}

	std::vector<Vec2> reversedPigtailVerts = GesturePaths::pigtailVertsPrimary;
	std::reverse(reversedPigtailVerts.begin(), reversedPigtailVerts.end());
	cugl::Path2 reversePigtailGesturePath = cugl::Path2(reversedPigtailVerts);
	if (!_dollarRecog->addGesture("pigtail", reversePigtailGesturePath, true)) {
		CULog("failed to initialize reverse pigtail");
		return false;

	}

	std::vector<Vec2> reversedCaretVerts = GesturePaths::caretVertsPrimary;
	std::reverse(reversedCaretVerts.begin(), reversedCaretVerts.end());
	cugl::Path2 reverseCaretGesturePath = cugl::Path2(reversedCaretVerts);
	if (!_dollarRecog->addGesture("caret", reverseCaretGesturePath, true)) {
		CULog("failed to initialize reverse caret");
		return false;
	}

	std::vector<Vec2> reversedTriangleVerts = GesturePaths::triangleVertsPrimary;
	std::reverse(reversedTriangleVerts.begin(), reversedTriangleVerts.end());
	cugl::Path2 reverseTriangleGesturePath = cugl::Path2(reversedTriangleVerts);
	if (!_dollarRecog->addGesture("triangle", reverseTriangleGesturePath, true)) {
		CULog("failed to initialize reverse triangle");
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
			//float sim2 = _dollarRecog->similarity(_currentTargetGesturesNighttime[1][0], _inputtedGestures[0]);
			float totalSim;
			//if (sim1 > sim2) {
				totalSim = (sim1 + _dollarRecog->similarity(_currentTargetGesturesNighttime[0][1], _inputtedGestures[1])
					+ _dollarRecog->similarity(_currentTargetGesturesNighttime[0][2], _inputtedGestures[2]))/3.0f;
				_isDurationSequence = true;
			//}
			//else {
			//	totalSim = (sim2 + _dollarRecog->similarity(_currentTargetGesturesNighttime[1][1], _inputtedGestures[1])
			//		+ _dollarRecog->similarity(_currentTargetGesturesNighttime[1][2], _inputtedGestures[2]))/3.0f;
			//	_isDurationSequence = false;
			//}
			CULog("Total sim: %f", totalSim);
			_currentSimilarity = totalSim;
			_lastResult = gestureResult();
			return;
		}

		_completed = false;
		_readyToCook = false;

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
		_inputtedGestures.clear();
	}
}




void DollarScene::addIngredientToStation(std::shared_ptr<Ingredient> ing) {
	CULog("added to pot");

	//add it to the station, and let both ingredient and station know. Also store which is ing is in
	ing->setInPot(true);
	setIngredientInStation(ing);
	_readyToCook = true;
	// make sure it tosses out dragging from moving ingredient first
	_input->popTouchPath();

	//prep gestures and flag ready
	_currentTargetGestures = ing->getGestures();
	_currentTargetIndex = 0;
	_completed = false;
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
	

	//clear gestures
	_input->popTouchPath();
	_poly->setVisible(false);


	_ingredientInStation = nullptr;
}
