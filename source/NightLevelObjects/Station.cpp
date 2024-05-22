#include "Station.h"

std::map<StationType, std::unordered_set<IngredientType>> acceptedIngredientsMap = {
	{StationType::CUT, std::unordered_set<IngredientType>({IngredientType::carrot})},
	{StationType::FRY, std::unordered_set<IngredientType>({IngredientType::beef, IngredientType::shrimp, IngredientType::egg})},
	{StationType::BOIL, std::unordered_set<IngredientType>({IngredientType::egg, IngredientType::rice})}
};

std::map<IngredientType, std::string> ingredientTypeMap = {
	{IngredientType::carrot, "carrot"},
	{IngredientType::beef, "beef"},
	{IngredientType::shrimp, "shrimp"},
	{IngredientType::egg, "egg"},
	{IngredientType::rice, "rice"}
};

bool Station::init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size) {
	if (GestureInteractable::init(texture, pos, size)) {
		setCapacity(1);
		setBodyType(b2_staticBody);
		setSensor(true);
		return true;
	}
	return false;
}

bool Station::interact(IngredientType t) {
	GestureInteractable::interact(t);
	//process interaction: add ingredient if empty, otherwise do nothing
	if (getTotalCount() < getCapacity() && acceptedIngredientsMap[_type].find(t) != acceptedIngredientsMap[_type].end()) {
		return addIngredient(t);
	}
	return false;
}

void Station::setIngredientPtr(std::shared_ptr<Ingredient> i) {
	_ingredient = i;
}

IngredientType Station::getCookedType(IngredientType t) {
	switch (_type) {
	case StationType::CUT: {
		return IngredientType::cutCarrot;
	}

	case StationType::FRY:
	{
		if (t == IngredientType::beef) {
			return IngredientType::cookedBeef;
		}
		else if (t == IngredientType::shrimp) {
			return IngredientType::cookedShrimp;
		}
		else if (t == IngredientType::egg) {
			return IngredientType::scrambledEgg;
		}
		break;
	}
	case StationType::BOIL:
	{
		if (t == IngredientType::egg) {
			return IngredientType::boiledEgg;
		}
		else if (t == IngredientType::rice) {
			return IngredientType::boiledRice;
		}
		break;
	}
	default:
		return IngredientType::null;
		break;
	}
}

void Station::hit() {
	clearIngredients();
}