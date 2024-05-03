#include "Station.h"

bool Station::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size) {
	if (GestureInteractable::init(texture, pos, size)) {
		setCapacity(1);
		setBodyType(b2_staticBody);
		b2Filter filter = getFilterData();
		filter.groupIndex = -1;
		setFilterData(filter);
		return true;
	}
	return false;
}

bool Station::interact(IngredientType t) {
	GestureInteractable::interact(t);
	//process interaction: add ingredient if empty, otherwise do nothing
	if (getTotalCount() < getCapacity()) {
		return addIngredient(t);
	}
	return true;
}

void Station::hit() {
	clearIngredients();
}