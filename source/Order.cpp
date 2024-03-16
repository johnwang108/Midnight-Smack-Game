#include "Order.h"

using namespace cugl;

Order::Order() {
	Order("", std::vector<std::string>(), 0);
}

Order::Order(std::string station, std::vector<std::string> gestures, float startTime) {
	_station = station;
	_gestureNames = gestures;
	_startTime = startTime;
}