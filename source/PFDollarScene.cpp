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


//main init
bool DollarScene::init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures) {
	_input = input;
	_assets = assets;
	_se = cugl::SimpleExtruder();
	_childOffset = -1;
	_combined = Affine2::IDENTITY;
	_focus = false;
	_currentTargetGestures = gestures;
	//reflection across the x axis is necessary for polygon path
	/**
	* 1 0
	* 0 -1
	* transform 0,0
	*/
	_trans = Affine2(1, 0, 0, -1, 0, 0);
	_poly = cugl::scene2::PolygonNode::alloc();
	_box = cugl::scene2::PolygonNode::allocWithTexture(assets->get<cugl::Texture>(texture), rect);

	_header = scene2::Label::allocWithText("Gestures, Similarity: t tosdgodfho figjgoj ghkohko ", _assets->get<Font>(SMALL_MSG));
	_header->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	_header->setScale(1.1f);
	_header->setPosition(cugl::Vec2(0, 0));
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

//re-extrudes the path and updates the polygon node
void DollarScene::update(float timestep) {
	//pop new path if this node is focused on and the input controller contains a nonempty path.
	if (_focus) {
		if (!_input->getTouchPath().empty()) {
			_path = _input->getTouchPath();
		}
	}
	


	
	//re-extrude path
	_se.set(_path);
	_se.calculate(WIDTH);


	//have to reflect across x axis with _trans
	_poly->setPolygon(_se.getPolygon() * _trans);
	_poly->setColor(cugl::Color4::BLACK);
	_poly->setAnchor(cugl::Vec2::ANCHOR_CENTER);

	_poly->setPosition(cugl::Vec2(0,0));
	_box->setPosition(cugl::Vec2(0, 0));

	//_header->setVisible(!isPending() && isSuccess());
	_header->setText("Target gesture: " + _input->getGestureString() + " | Similarity: " + std::to_string(_input->getGestureSim()));

	if (!_currentTargetGestures.empty()) {
		_currentGestureLabel->setText("Current Target Gesture: " + _currentTargetGestures.front());
	}
	
};

//is gesture inputting still in progress?
bool DollarScene::isPending() {
	//TODO
	return false;
};

//is gesture inputting a success?
bool DollarScene::isSuccess() {
	return _input->getGestureSim() > DOLLAR_THRESHOLD;
};

void DollarScene::setFocus(bool focus) {
	_focus = focus;
	if (focus) {
		_input->popTouchPath();
	}
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