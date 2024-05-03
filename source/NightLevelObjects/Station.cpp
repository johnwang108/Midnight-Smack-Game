#include "Station.h"

std::map<StationType, std::unordered_set<IngredientType>> acceptedIngredientsMap = {
	{StationType::CUT, std::unordered_set<IngredientType>({IngredientType::carrot})},
	{StationType::FRY, std::unordered_set<IngredientType>({IngredientType::beef, IngredientType::shrimp})},
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
		//b2Filter filter = getFilterData();
		//filter.groupIndex = -1;
		//setFilterData(filter);
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

void Station::hit() {
	clearIngredients();
}