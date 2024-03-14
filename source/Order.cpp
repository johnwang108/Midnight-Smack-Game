#include "Order.h"

using namespace cugl;

Order::Order() {
	Order("", "", 0);
}

Order::Order(std::string station, std::string gesture, float startTime) {
	_station = station;
	_gestureName = gesture;
	_startTime = startTime;
}