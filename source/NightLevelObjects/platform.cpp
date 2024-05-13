#include "platform.h"



void Platform::createFixtures() {
	if (_body == nullptr) {
		CULog("Platform body is null");
		return;
	}

	PolygonObstacle::createFixtures();

	b2FixtureDef sensorDef;
	sensorDef.isSensor = true;
	sensorDef.density = 0.0f;

	float sensorWidth = 0.1f;
	float sensorHeight = getHeight()+0.5;

	b2PolygonShape sensorShape;
	b2Vec2 corners[4];

	// Left sensor
	corners[0].Set(-sensorWidth, 0);
	corners[1].Set(0, 0);
	corners[2].Set(0, sensorHeight);
	corners[3].Set(-sensorWidth, sensorHeight);
	sensorShape.Set(corners, 4);
	sensorDef.shape = &sensorShape;
	sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


	_body->CreateFixture(&sensorDef);
	//b2Filter filter = getFilterData();
	//filter.groupIndex = -1;
	//_sensorFixture->SetFilterData(filter);

	// Right sensor
	corners[0].Set(getWidth(), 0);
	corners[1].Set(getWidth() + sensorWidth, 0);
	corners[2].Set(getWidth() + sensorWidth, sensorHeight);
	corners[3].Set(getWidth(), sensorHeight);
	sensorShape.Set(corners, 4);
	sensorDef.shape = &sensorShape;

	_body->CreateFixture(&sensorDef);
	//filter = getFilterData();
	//filter.groupIndex = -1;
	//_sensorFixture->SetFilterData(filter);
}

void Platform::releaseFixtures() {
	if (_body != nullptr) {
		return;
	}

	physics2::PolygonObstacle::releaseFixtures();

}