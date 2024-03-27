#ifndef __INGREDIENT_H__
#define __INGREDIENT_H__

#include <cugl/cugl.h>



class Ingredient {
private: 
	std::string _name;
	std::string _station;
	std::vector<std::string> _gestureNames;
	float _startTime;
	bool _falling;
	bool _inPot;

	std::shared_ptr<cugl::scene2::Button> _button;
	bool _beingHeld;
public: 
	
	
	Ingredient();

	/**
	* Creates a new Order object for cook station station and starting at startTime seconds after the day begins.
	*
	*/
	Ingredient(std::string station, std::vector<std::string> gestures, float startTime);

	std::string getName() { return _name; }
	void setName(std::string newName) { _name = newName; }

	float getStartTime() { return _startTime; }

	void setStartTime(float time) { _startTime = time; };


	std::string getStation() { return _station + "_station"; }

	void setStation(std::string station) { _station = station; };

	
	std::vector<std::string> getGestures() { return _gestureNames; }

	void setGestures(std::vector<std::string> gestures) {_gestureNames = gestures;};

	void init(std::shared_ptr<cugl::Texture> texture);

	bool getBeingHeld() { return _beingHeld; }

	std::shared_ptr<cugl::scene2::Button> getButton() { return _button; }

	bool isFalling() { return _falling; }
	void setFalling(bool val) { _falling = val; };

	bool inPot() { return _inPot; }
	void setInPot(bool val) { _inPot = val; };
};

#endif // __INGREDIENT_H__
