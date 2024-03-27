#include "Ingredient.h"

using namespace cugl;
using namespace scene2;

Ingredient::Ingredient() {
	Ingredient("", std::vector<std::string>(), 0);
}

Ingredient::Ingredient(std::string station, std::vector<std::string> gestures, float startTime) {
	_station = station;
	_gestureNames = gestures;
	_startTime = startTime;
	_beingHeld = false;
	_falling = false;
	_inPot = false;
}

void Ingredient::init(std::shared_ptr<Texture> texture) {
	_inPot = false;
	std::shared_ptr<PolygonNode> texturedPoly =  PolygonNode::allocWithTexture(texture);
	_button = Button::alloc(texturedPoly);
	_button->addListener([=](const std::string& name, bool down) {
		_beingHeld = down;
	});
	_button->activate();
	_button->setAnchor(Vec2::ANCHOR_CENTER);
	_button->setScale(Vec2(.5, .5));
	//_button->setContentSize(_button->getSize())
	_button->setPosition(Vec2(0, 0));
	_button->setVisible(true);
}