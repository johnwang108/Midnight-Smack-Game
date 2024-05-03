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

void Plate::interact() {
	GestureInteractable::interact();
	//process interaction: add ingredient to dollar scene

}