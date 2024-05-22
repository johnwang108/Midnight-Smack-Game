#ifndef __INGREDIENT_H__
#define __INGREDIENT_H__

#include <cugl/cugl.h>

enum class IngredientType {
	carrot,
	beef,
	shrimp,
	egg,
	rice,
	cutCarrot,
	cookedBeef,
	cookedShrimp,
	boiledRice,
	boiledEgg,
	scrambledEgg,
	null
};
namespace std {
	template <> struct hash<IngredientType> {
		size_t operator() (const IngredientType& t) const { return size_t(t); }
	};
}
class Ingredient {
private: 
	std::string _name;
	std::string _startStation;
	std::vector<std::string> _gestureNames;
	std::vector <std::string> _validStations;
	float _startTime;
	bool _falling;
	bool _inPot;
	std::shared_ptr<cugl::scene2::PolygonNode> _poly;
	std::shared_ptr<cugl::scene2::Button> _button;
	bool _beingHeld;
	bool _launching;
	int _currentInventorySlot;

public: 
	
	
	Ingredient();

	/**
	* Creates a new Order object for cook station station and starting at startTime seconds after the day begins.
	*
	*/
	Ingredient(std::string station, std::vector<std::string> gestures, float startTime);

	std::string getName() { return _name; }
	void setName(std::string newName) { _name = newName; }

	void setCurrentInventorySlot(int slot) { _currentInventorySlot = slot; }
	int getCurrentInventorySlot() { return _currentInventorySlot; }

	float getStartTime() { return _startTime; }

	void setStartTime(float time) { _startTime = time; };


	std::string getStartStation() { return _startStation + "Station"; }

	void setStartStation(std::string station) { _startStation = station; };

	std::vector<std::string> getValidStations() { return _validStations; }

	void setValidStations(std::vector<std::string> stations) { _validStations = stations; };
	
	std::vector<std::string> getGestures() { return _gestureNames; }

	void setGestures(std::vector<std::string> gestures) {_gestureNames = gestures;};

	void init(std::shared_ptr<cugl::Texture> texture);

	bool getBeingHeld() { return _beingHeld; }

	std::shared_ptr<cugl::scene2::Button> getButton() { return _button; }

	bool isFalling() { return _falling; }
	void setFalling(bool val) { _falling = val; };

	bool inPot() { return _inPot; }
	void setInPot(bool val) { _inPot = val; };

	bool isLaunching() { return _launching; }
	void setLaunching(bool val) { _launching = val; };

	static IngredientType getIngredientTypeFromString(std::string type) {
		if (type == "carrot") return IngredientType::carrot;
		if (type == "beef") return IngredientType::beef;
		if (type == "shrimp") return IngredientType::shrimp;
		if (type == "egg") return IngredientType::egg;
		if (type == "rice") return IngredientType::rice;
		if (type == "cutCarrot") return IngredientType::cutCarrot;
		if (type == "cookedBeef") return IngredientType::cookedBeef;
		if (type == "cookedShrimp") return IngredientType::cookedShrimp;
		if (type == "boiledEgg") return IngredientType::boiledEgg;
		if (type == "boiledRice") return IngredientType::boiledRice;
		if (type == "scrambledEgg") return IngredientType::scrambledEgg;
		return IngredientType::null;
	}

	static std::string getIngredientStringFromType(IngredientType type) {
		if (type == IngredientType::carrot) return "carrot";
		if (type == IngredientType::beef) return "beef";
		if (type == IngredientType::shrimp) return "shrimp";
		if (type == IngredientType::egg) return "egg";
		if (type == IngredientType::rice) return "rice";
		if (type == IngredientType::cutCarrot) return "cutCarrot";
		if (type == IngredientType::cookedBeef) return "cookedBeef";
		if (type == IngredientType::cookedShrimp) return "cookedShrimp";
		if (type == IngredientType::boiledEgg) return "boiledEgg";
		if (type == IngredientType::boiledRice) return "boiledRice";
		if (type == IngredientType::scrambledEgg) return "scrambledEgg";
		return "null";
	}
};

#endif // __INGREDIENT_H__
