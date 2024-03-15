#ifndef __ORDER_H__
#define __ORDER_H__

#include <cugl/cugl.h>

class Order {
private: 
	std::string _station;
	std::vector<std::string> _gestureNames;
	float _startTime;
public: 
	
	Order();

	/**
	* Creates a new Order object for cook station station and starting at startTime seconds after the day begins.
	*
	*/
	Order(std::string station, std::vector<std::string> gestures, float startTime);
	
	float getStartTime() { return _startTime; }

	std::string getStation() { return _station + "_station"; }
	
	std::vector<std::string> getGestures() { return _gestureNames; }
};

#endif // __ORDER_H__
