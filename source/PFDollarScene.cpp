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

#define WIDTH 5

using namespace cugl;

float STAR[] = { 0,    50,  10.75,    17,   47,     17,
				 17.88, -4.88,   29.5, -40.5,    0, -18.33,
				 -29.5, -40.5, -17.88, -4.88,  -47,     17,
				-10.75,    17 };


DollarScene::DollarScene() : scene2::SceneNode() {
	_assets = nullptr;
	cugl::Vec2* verts = reinterpret_cast<Vec2*>(STAR);
	_spline.set(verts, sizeof(STAR) / sizeof(float) / 2);
	sp = cugl::SplinePather();
	sp.set(&_spline);
	sp.calculate();
	_path = sp.getPath();
}

void DollarScene::dispose() {
	//input will be disposed by root node dispose call
	//_input.dispose();
	_assets = nullptr;
}


//main init
bool DollarScene::init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
	_input = input;
	_assets = assets;
	_se = cugl::SimpleExtruder();
	_poly = cugl::scene2::PolygonNode::alloc();

	return true;
}

//updates the rendered polygon
void DollarScene::update() {
	_se.set(_path);
	_se.calculate(WIDTH);
	_poly->setPolygon(_se.getPolygon());
};

//is gesture inputting still in progress?
bool DollarScene::isPending() {
	return true;
};

//is gesture inputting a success?
bool DollarScene::isSuccess() {
	return true;
};