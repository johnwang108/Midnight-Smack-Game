
#include "EnemyAttack.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#pragma mark -
#pragma mark Initializers

using namespace cugl;

//lifetime in frames

bool EnemyAttack::init(cugl::Vec2 pos, const cugl::Size& size) {


    if (BoxObstacle::init(pos, size)) {
		setDensity(1);
		_killme = false;
		_lifetime = 3;
		_faceright = false;
		_direction = -1;
		_shoot = true;

        return true;
    }

    return false;
}
/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void EnemyAttack::update(float dt) {
	BoxObstacle::update(dt);
	if (_lifetime) {
		if (_lifetime == 0) {
			_killme = true;
		}
		else {
			_lifetime = (_lifetime > 0 ? _lifetime - 1 : 0);
		}
	}
	if (_faceright) {
		scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
		if (image != nullptr) {
			image->flipHorizontal(!image->isFlipHorizontal());
		}
		_direction = 1;
		_faceright = false;
	}
	if (_shoot) {
		_shoot = false;
		_body->ApplyLinearImpulseToCenter(b2Vec2(3* _direction, 6), true);
	}

	float currentAngle = _body->GetAngle();
	_body->SetTransform(_body->GetPosition(), currentAngle + M_PI * dt);

	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
	}
}

/**
 * Disposes all resources and assets of this Attack
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a Attack may not be used until it is initialized again.
 */
void EnemyAttack::dispose() {
	_node = nullptr;
}

void EnemyAttack::releaseFixtures() {
	if (_body != nullptr) {
		return;
	}

	BoxObstacle::releaseFixtures();
	if (_sensorFixture != nullptr) {
		_body->DestroyFixture(_sensorFixture);
		_sensorFixture = nullptr;
	}

}

void EnemyAttack::createFixtures() {
	if (_body == nullptr) {
		return;
	}

	BoxObstacle::createFixtures();


	b2FixtureDef sensorDef;
	sensorDef.density = 0;
	sensorDef.isSensor = true;

	b2PolygonShape sensorShape;
	b2Vec2 sensorVertices[4];
	sensorVertices[0].Set(-getWidth() * EA_HSHRINK / 2.0f, -getHeight() / 2.0f);
	sensorVertices[1].Set(getWidth() * EA_HSHRINK / 2.0f, -getHeight() / 2.0f);
	sensorVertices[2].Set(getWidth() * EA_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
	sensorVertices[3].Set(-getWidth() * EA_HSHRINK / 2.0f, -getHeight() / 2.0f - SENSOR_HEIGHT);
	sensorShape.Set(sensorVertices, 4);

	sensorDef.shape = &sensorShape;

	sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());


	_sensorFixture = _body->CreateFixture(&sensorDef);


}


