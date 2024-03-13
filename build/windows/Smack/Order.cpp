#include "Order.h"

using namespace cugl;

Order::Order() {
	Order("", 0);
}

Order::Order(std::string station, float startTime) {
	_station = station;
	_startTime = startTime;
}
