#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cugl/cugl.h"
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/physics2/CUPolygonObstacle.h>
#include "EntitySpriteNode.h"

#include "Attack.h"

using namespace cugl;

/**Class that the player and enemy classes inherit from.Shared behavior : animation, debug nodes, health, attacks,. */

class Entity : public cugl::physics2::CapsuleObstacle {

protected:

	bool _isGrounded;

	/**Entity health*/
	float _health;

	float _healthCooldown;

	float _lastDamageTime;

	/**How much damage this entity does*/
	float _attack;

	/**SpriteNode*/
	std::shared_ptr<EntitySpriteNode> _node;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;

	/**unordered map of string (action name) -> cugl::Animate action*/
	std::unordered_map<std::string, std::shared_ptr<cugl::scene2::Animate>> _actions;

	/**unordered map of strings(action name)->sprite sheets for the corresponding action */
	std::unordered_map<std::string, std::shared_ptr<cugl::Texture>> _sheets;

	/** info about each action's sheet: rows, cols, size, duration. Currently unused*/
	std::unordered_map<std::string, std::tuple<int, int, int, float, bool>> _info;

	std::string _activeAction;

	cugl::Size _dimension;

private:

public:

	/**Default constructor*/
	Entity() : CapsuleObstacle() {};

	void setSceneNode(const std::shared_ptr<EntitySpriteNode> node) {
		_node = node;
		//_node->setPosition(getPosition() * _drawScale);
	}

	const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() { return _node; }

	void addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool isPassive = true);

	void animate(std::string action_name);

	void changeSheet(std::string action_name);

	std::shared_ptr<cugl::scene2::Animate> getAction(std::string action_name) { return _actions[action_name]; };

	/**Unused*/
	void getInfo(std::string action_name) {};

	std::string getActiveAction() { return _activeAction; };

	//default constructor. Should not be used
	virtual bool init(){ return false; }

	virtual bool init(cugl::Vec2 pos, cugl::Size size);

};
#endif /* __ENTITY_H__ */