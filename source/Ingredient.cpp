#include "Ingredient.h"

using namespace cugl;
using namespace scene2;

Ingredient::Ingredient() {
	Ingredient("", std::vector<std::string>(), 0);
}

Ingredient::Ingredient(std::string startStation, std::vector<std::string> gestures, float startTime) {
	_startStation = startStation;
	_gestureNames = gestures;
	_startTime = startTime;
	_validStations = {};
	_beingHeld = false;
	_falling = false;
	_inPot = false;
	_launching = false;
}

void Ingredient::init(std::shared_ptr<Texture> texture) {
	_beingHeld = false;
	_falling = false;
	_inPot = false;
	_launching = false;
	_poly =  PolygonNode::allocWithTexture(texture);
	if (_button != nullptr) {
		_button->dispose();
		_button.reset();
	}
	_button = Button::alloc(_poly);
	_button->addListener([=](const std::string& name, bool down) {
		_beingHeld = down;
	});
	_button->activate();
	_button->setAnchor(Vec2::ANCHOR_CENTER);
	_button->setScale(Vec2(.3, .3));
	//_button->setContentSize(_button->getSize())
	_button->setPosition(Vec2(0, 0));
	_button->setVisible(true);
}