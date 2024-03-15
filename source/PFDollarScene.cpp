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

#include "PFDollarScene.h"
#include <box2d/b2_world.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_collision.h>


#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

#define WIDTH 25

#define SMALL_MSG "retrosmall"  

#define DOLLAR_THRESHOLD 0.5

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
	initGestureRecognizer();
	//reflection across the x axis is necessary for polygon path
	/**
	* 1 0
	* 0 -1
	* transform 0,0
	*/
	_trans = Affine2(1, 0, 0, -1, 0, 0);
	_poly = cugl::scene2::PolygonNode::alloc();
	_box = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>(texture), rect);

	//default pigtail
	_currentTargetGestures = std::vector <std::string>{ "pigtail" };

	_header = scene2::Label::allocWithText("Gestures, Similarity: t tosdgodfho figjgoj ghkohko ", _assets->get<Font>(SMALL_MSG));
	_header->setAnchor(Vec2::ANCHOR_CENTER);
	_header->setPosition(cugl::Vec2(0, 200));
	_header->setForeground(cugl::Color4::RED);

	_currentGestureLabel = scene2::Label::allocWithText("Current Target Gesture: filling out text bc i swear this always breaks", _assets->get<Font>(SMALL_MSG));
	_currentGestureLabel->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	_currentGestureLabel->setPosition(cugl::Vec2(0, 100));
	_currentGestureLabel->setForeground(cugl::Color4::BLACK);

	addChild(_box);
	addChild(_poly);
	addChild(_header);
	addChild(_currentGestureLabel);
	

	update(0);

	return true;
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
	if (_focus) {
		if (!(_input->getTouchPath().empty())) {
			_path = _input->getTouchPath();
		}
		if (!_currentTargetGestures.empty()) {
			matchWithTouchPath();
			if (isSuccess()) {
				_currentSimilarity = 0;
				if (_currentTargetIndex < _currentTargetGestures.size() - 1) _currentTargetIndex++;
				else {
					_completed = true;
				}
				CULog("succeed");
			}
			if (!_completed) _currentGestureLabel->setText("Current Target Gesture: " + _currentTargetGestures[_currentTargetIndex]);
			else _currentGestureLabel->setText("Done!");

		}
	}

	//TODO: handle rendering smarter
	if (countdown > 0) {
		countdown--;
	}
	else {
		countdown = 0;
	}


	//re-extrude path
	_se.set(_path);
	_se.calculate(WIDTH);


	//have to reflect across x axis with _trans
	_poly->setPolygon(_se.getPolygon() * _trans);
	_poly->setColor(cugl::Color4::BLACK);
	_poly->setAnchor(cugl::Vec2::ANCHOR_CENTER);

	//GET RID OF HARDCODE JOHN TODO
	_poly->setPosition(cugl::Vec2(0,0));
	_box->setPosition(cugl::Vec2(0, 0));




	//_header->setVisible(!isPending() && isSuccess());
	_header->setText("Target gesture: " + _currentTargetGestures[_currentTargetIndex] + " | Similarity: " + std::to_string(_currentSimilarity));

	
};

//is gesture inputting still in progress?
bool DollarScene::isPending() {
	//TODO
	return !_input->isGestureCompleted();
};


void DollarScene::matchWithTouchPath() {
	if (_input->isGestureCompleted()) {
		Path2 gesture = _input->popTouchPath();
		if (gesture.size() > 3) {
			float sim = _dollarRecog->similarity(_currentTargetGestures[_currentTargetIndex], gesture);
			if (sim >= 0) _currentSimilarity = sim;
			
		}
		
	}
}

//is gesture inputting a success?
bool DollarScene::isSuccess() {
	countdown = 60;
	return _currentSimilarity > DOLLAR_THRESHOLD;
};

void DollarScene::setFocus(bool focus) {
	_focus = focus;
	if (focus) {
		_input->popTouchPath();
	}
}

bool DollarScene::shouldIDisappear() {
	return countdown == 0;
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