#ifndef __ORDER_H__
#define __ORDER_H__

#include <cugl/cugl.h>

class Order {
private:
	std::string _station;
	float _startTime;
public:

	Order();

	/**
	* Creates a new Order object for cook station station and starting at startTime seconds after the day begins.
	*
	*/
	Order(std::string station, float startTime);

};

#endif // __ORDER_H__