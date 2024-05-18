#include "TutorialSign.h"

int TutorialSign::ID = 0;
bool TutorialSign::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size, const std::string type) {
	if (BoxObstacle::init(pos, size)) {
		setFriction(0.0f); // Prevent sticking to walls
		setFixedRotation(false);
		setFriction(1.0f);
		_isActive = false;
		_node = EntitySpriteNode::allocWithSheet(texture, 1, 1, 1);
		_node->setAnchor(Vec2::ANCHOR_CENTER);
		_node->setPosition(pos);
		_capacity = -1;
		_interactableId = ID++;
		_popuptype = type;
		filter = getFilterData();
		filter.groupIndex = -1;
		setFilterData(filter);
		setName("TutorialSign");
		setSensor(true);

		return true;
	}
    return false;
}

void TutorialSign::setSpriteNode(const std::shared_ptr<EntitySpriteNode>& node) {
	_node = node;
}
void TutorialSign::setActive(bool active) {
	_isActive = active;
}
bool TutorialSign::isActive() {
	return _isActive;
}

int TutorialSign::getCapacity() {
	return _capacity;
}
void TutorialSign::setCapacity(int capacity) {
	_capacity = capacity;
}
