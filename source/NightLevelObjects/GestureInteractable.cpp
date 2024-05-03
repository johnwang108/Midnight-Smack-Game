#include "GestureInteractable.h"

int GestureInteractable::ID = 0;
bool GestureInteractable::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size) {
	if (BoxObstacle::init(pos, size)) {
		setFriction(0.0f); // Prevent sticking to walls
		setFixedRotation(false);
		setFriction(1.0f);
		_isActive = false;
		_node = EntitySpriteNode::allocWithSheet(texture, 1, 1, 1);
		_ingredients = std::unordered_map<IngredientType, int>();
		_ingredients[IngredientType::rice] = 0;
		_ingredients[IngredientType::carrot] = 0;
		_ingredients[IngredientType::beef] = 0;
		_ingredients[IngredientType::egg] = 0;
		_ingredients[IngredientType::shrimp] = 0;
		_ingredients[IngredientType::riceCooked] = 0;
		_ingredients[IngredientType::carrotCooked] = 0;
		_ingredients[IngredientType::beefCooked] = 0;
		_ingredients[IngredientType::eggCooked] = 0;
		_ingredients[IngredientType::shrimpCooked] = 0;
		_capacity = -1;
		_interactableId = ID++;
		setName("interactable");
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
bool GestureInteractable::addIngredient(IngredientType i) {
	assert(_capacity != -1);
	if (getTotalCount() < _capacity) {
		_ingredients[i]++;
		return true;
	}
	return false;
}
bool GestureInteractable::popIngredient(IngredientType i) {
	assert(_capacity != -1);
	if (_ingredients[i] > 0) {
		_ingredients[i]--;
		return true;
	}
	return false;
}
void GestureInteractable::clearIngredients() {
	//_ingredients.clear();
}


int GestureInteractable::getCapacity() {
	return _capacity;
}
void GestureInteractable::setCapacity(int capacity) {
	_capacity = capacity;
}
bool GestureInteractable::interact(IngredientType t) {
	if (!_isActive) return false;

	//transition in dollar node
	return true;
}