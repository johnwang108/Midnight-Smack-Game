#ifndef __ORDER_H__
#define __ORDER_H__

#include <cugl/cugl.h>

class Order {
private: 
	std::string _station;
	std::string _gestureName;
	float _startTime;
public: 
	
	Order();

	/**
	* Creates a new Order object for cook station station and starting at startTime seconds after the day begins.
	*
	*/
	Order(std::string station, std::string gesture, float startTime);
	
	float getStartTime() { return _startTime; }
};

#endif // __ORDER_H__
