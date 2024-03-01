//
//  PFAttack.cpp
//  PlatformDemo
//
//  This class is a simple extension of WheelObstacle in order to simplify the
//  process of adding and removing Attacks from the game world, as well as for
//  drawing the sprite of the attack.
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#include "PFAttack.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>

#pragma mark -
#pragma mark Initializers

using namespace cugl;

//lifetime in frames


/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void Attack::update(float dt) {
	BoxObstacle::update(dt);
	if (_lifetime == 0) {
		_killme = true;
	}
	else {
		_lifetime = (_lifetime > 0 ? _lifetime - 1 : 0);
	}

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
void Attack::dispose() {
	_node = nullptr;
}




