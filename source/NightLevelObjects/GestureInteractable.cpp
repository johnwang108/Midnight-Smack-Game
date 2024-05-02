#include "GestureInteractable.h"

bool GestureInteractable::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size) {
	if (BoxObstacle::init(pos, size)) {
		setFriction(0.0f); // Prevent sticking to walls
		setFixedRotation(false);
		setFriction(1.0f);
		_isActive = false;
		_node = EntitySpriteNode::allocWithSheet(texture, 1, 1, 1);
		_ingredients = std::unordered_set<Ingredient>();
		_capacity = -1;
		return true;
	}
}

void GestureInteractable::setSpriteNode(const std::shared_ptr<EntitySpriteNode>& node) {
	_node = node;
}
void GestureInteractable::setActive(bool active) {
	_isActive = active;
}
bool GestureInteractable::isActive() {
	return _isActive;
}
bool GestureInteractable::addIngredient(Ingredient i) {
	if (_capacity == -1 || _ingredients.size() < _capacity) {
		_ingredients.insert(i);
		return true;
	}
	return false;
}
bool GestureInteractable::popIngredient(Ingredient i) {
	if (_ingredients.size() > 0) {
		_ingredients.erase(i);
		return true;
	}
	return false;
}
void GestureInteractable::clearIngredients() {
	_ingredients.clear();
}

int GestureInteractable::getCapacity() {
	return _capacity;
}
void GestureInteractable::setCapacity(int capacity) {
	_capacity = capacity;
}
bool GestureInteractable::interact() {
	if (!_isActive) return false;
	return true;
}