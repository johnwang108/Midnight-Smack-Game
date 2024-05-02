#include "Station.h"

bool Station::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size, float scale) {
	if (GestureInteractable::init(texture, pos, size)) {
		setCapacity(1);
		return true;
	}
	return false;
}