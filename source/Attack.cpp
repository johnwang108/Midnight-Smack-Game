
#include "Attack.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#pragma mark -
#pragma mark Initializers

using namespace cugl;

//lifetime in frames

bool Attack::init(cugl::Vec2 pos, const cugl::Size& size) {


    if (BoxObstacle::init(pos, size)) {
		setDensity(1);
		_killme = false;
        _lifetime = ATTACK_LIFETIME;
		_faceright = false;
		_direction = -1;
		_shoot = true;
		_straight = Vec2(-87,-87);
		_go = false;
        _norotate= false;
		_speed = DEFAULT_SPEED;
		_die = true;
		_uppp = 0;
		_follow = false;

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
void Attack::update(float dt) {
	BoxObstacle::update(dt);
    
    if (_faceright) {
        scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
        if (image != nullptr) {
            image->flipHorizontal(!image->isFlipHorizontal());
        }
        _direction = 1;
        _faceright = false;
    }


	if (_node != nullptr) {
		_node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
	}
}

void Attack::fixedUpdate(float dt) {
	if (_body == nullptr) {
		return;
	}

	if (_go) {
		_body->SetLinearVelocity(b2Vec2(8 * _direction, 0));
	}
	if(_die) {
		if (_lifetime == 0) {
			_killme = true;
		}
		else {
			_lifetime = (_lifetime > 0 ? _lifetime - 1 : 0);
		}
	}
	if (_uppp > 0) {
		if (_uppp == 1.5) {
			_body->ApplyLinearImpulseToCenter(b2Vec2((60 * static_cast<float>(rand()) / static_cast<float>(RAND_MAX))-50, 100), true);
		}
		_uppp -= dt;
	}
	else if (_shoot) {
		_body->SetLinearVelocity(b2Vec2(0,0));
		_shoot = false;
		if (_rand) {
			_body->ApplyLinearImpulseToCenter(b2Vec2(20 * static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 5, 30), true);
		}
		else if (_straight != Vec2(-87, -87)) {
			setGravityScale(0);
			b2Vec2 targetDirection = b2Vec2(_straight.x - getPosition().x, _straight.y - getPosition().y);
			targetDirection.Normalize();
			_body->ApplyLinearImpulseToCenter(b2Vec2(targetDirection.x * _speed, targetDirection.y * _speed), true);
			_body->SetTransform(_body->GetPosition(), atan2(_straight.y - getPosition().y, _straight.x - getPosition().x) + M_PI);
		}
		else {
			setGravityScale(0);
			_body->ApplyLinearImpulseToCenter(b2Vec2(3 * _direction, 10), true);
		}
	}
	else if (_follow) {
		if (_straight != Vec2(-87, -87)) {
			b2Vec2 targetDirection = b2Vec2(_straight.x - getPosition().x, _straight.y - getPosition().y);
			targetDirection.Normalize();
			_body->SetLinearVelocity(b2Vec2(targetDirection.x*15, targetDirection.y*15));
		}
	}

	float currentAngle = _body->GetAngle();
	b2Vec2 velocity = _body->GetLinearVelocity();

	if (_straight == Vec2(-87, -87) && !_norotate) {
		if (velocity.x < 0) {
			_body->SetTransform(_body->GetPosition(), currentAngle + M_PI * dt);
		}
		else {
			_body->SetTransform(_body->GetPosition(), currentAngle - M_PI * dt);
		}
	}
	else if (!_norotate) {
		_body->SetTransform(_body->GetPosition(), currentAngle);
	}
}

/**
 * Disposes all resources and assets of this Attack
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a Attack may not be used until it is initialized again.
 */
void Attack::dispose() {
	_geometry = nullptr;
	_node = nullptr;
}

void Attack::releaseFixtures() {
	if (_body != nullptr) {
		return;
	}

	BoxObstacle::releaseFixtures();
	if (_sensorFixture != nullptr) {
		_body->DestroyFixture(_sensorFixture);
		_sensorFixture = nullptr;
	}

}

void Attack::createFixtures() {
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


