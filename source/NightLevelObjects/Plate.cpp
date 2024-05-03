#include "Plate.h"

bool Plate::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size) {
	if (GestureInteractable::init(texture, pos, size)) {
		setCapacity(-1);
		setBodyType(b2_staticBody);
		b2Filter filter = getFilterData();
		filter.groupIndex = -1;
		setFilterData(filter);
		return true;
	}
	return false;
}

bool Plate::interact(IngredientType t) {
	GestureInteractable::interact(t);
	//process interaction: add ingredient to the plate!
	return addIngredient(t);
}

bool Plate::isSuccess() {
	if (!isDone()) {
		return false;
	}
	
	bool b1 = getCount(IngredientType::rice) == _targetIngredients[IngredientType::rice];
	bool b2 = getCount(IngredientType::carrot) == _targetIngredients[IngredientType::carrot];
	bool b3 = getCount(IngredientType::beef) == _targetIngredients[IngredientType::beef];
	bool b4 = getCount(IngredientType::egg) == _targetIngredients[IngredientType::egg];
	bool b5 = getCount(IngredientType::shrimp) == _targetIngredients[IngredientType::shrimp];
	bool b6 = getCount(IngredientType::riceCooked) == _targetIngredients[IngredientType::riceCooked];
	bool b7 = getCount(IngredientType::carrotCooked) == _targetIngredients[IngredientType::carrotCooked];
	bool b8 = getCount(IngredientType::beefCooked) == _targetIngredients[IngredientType::beefCooked];
	bool b9 = getCount(IngredientType::eggCooked) == _targetIngredients[IngredientType::eggCooked];
	bool b0 = getCount(IngredientType::shrimpCooked) == _targetIngredients[IngredientType::shrimpCooked];

	return b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9 && b0;
}

bool Plate::isDone() {
	return isFull();
}

bool Plate::isFull() {
	return (getTotalCount()) >= getCapacity();
}