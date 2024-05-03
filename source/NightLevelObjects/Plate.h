#ifndef __PLATE_H__
#define __PLATE_H__

#include <cugl/cugl.h>
#include "GestureInteractable.h"
using namespace cugl;

class Plate : public GestureInteractable {
private:

	std::unordered_map<IngredientType, int> _ingredients;

protected:


public:

	bool init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size);

	static std::shared_ptr<Plate> alloc(const std::shared_ptr<Texture>& texture, const Vec2& pos, const Size& s, std::unordered_map<IngredientType, int> ingredients) {
		std::shared_ptr<Plate> result = std::make_shared<Plate>();
		bool res = result->init(texture, pos, s);
		if (!res) return nullptr;
		result->setIngredients(ingredients);
		return result;
	}

	void interact() {};

	void setIngredients(std::unordered_map<IngredientType, int> ingredients) {
		_ingredients = ingredients;
	}
	
};

#endif /* __PLATE_H__ */