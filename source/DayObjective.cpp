#include "DayObjective.h"

using namespace cugl;
using namespace std;

DayObjective::DayObjective() {
	DayObjective(-1, "");
}
DayObjective::DayObjective(int id, string type) {
	_id = id;
	_type = type;
	_complete = false;
	_quantity = -1;
	_requiredAccuracy = -1.0f;
	_requiredTime = -1.0f;
}

void DayObjective::init(int id, string type) {
	_id = id;
	_type = type;
	_complete = false;
	_quantity = -1;
	_requiredAccuracy = -1.0f;
	_requiredTime = -1.0f;
}