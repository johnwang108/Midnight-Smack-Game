#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cugl/cugl.h"
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/physics2/CUPolygonObstacle.h>
#include "EntitySpriteNode.h"

#include "Attack.h"

#define NUMBERS "0123456789"

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
	std::unordered_map<std::string, std::tuple<int, int, int, float>> _info;

	std::string _activeAction;

	bool _activated;

	bool _finished;

	int _entityID;

	bool _useID;

	/** animation priority */
	int _priority;
	std::string _requestedAction;

	/** current animation priority */
	int _activePriority;

	bool _paused;
	int _pausedFrame;
	int _activeFrame;

private:

public:

	static int ID;

	/**Default constructor*/
	Entity() : CapsuleObstacle() {};

	void setSceneNode(const std::shared_ptr<EntitySpriteNode> node) {
		_node = node;
		//_node->setPosition(getPosition() * _drawScale);
	}

	const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() { return _node; }

	void addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool reverse = false);

	void animate(std::string action_name);

	void changeSheet(std::string action_name);

	std::shared_ptr<cugl::scene2::Animate> getAction(std::string action_name) { return _actions[action_name]; };

	/** info = {int rows, int cols, int size, float duration}*/
	std::tuple<int, int, int, float> getInfo(std::string action_name) { return _info[action_name]; };

	std::string getActiveAction() { return _activeAction; };

	//default constructor. Should not be used
	virtual bool init(){ return false; }

	virtual bool init(cugl::Vec2 pos, cugl::Size size);

	void loadAnimationsFromConstant(std::string entityName, std::shared_ptr<AssetManager> _assets);

	virtual std::string updateAnimation() { return ""; };

	void setActiveAction(std::string actionName) { 
		_activeAction = actionName;
		_activated = false;
		_finished = false;
	};

	void setActivated(bool act) { 
		_activated = act; };

	bool isActivated() { return _activated; };

	void setFinished(bool fin) { _finished = fin; };

	bool isFinished() { return _finished; };

	std::string getId() {
		if (_useID) return std::to_string(_entityID);
		else return "";
	};

	bool usesID() { return _useID; };
	
	bool isAnimating(std::string actionName) { return _activeAction == actionName && isActivated() && !isFinished(); };

	std::shared_ptr<EntitySpriteNode> getSpriteNode() { return _node; };

	int getPriority() { return _priority; };

	void setPriority(int priority) { _priority = priority; };

	int getActivePriority() { return _activePriority; };

	void setRequestedAction(std::string action) { _requestedAction = action; };

	std::string getRequestedAction() { return _requestedAction; };

	void setRequestedActionAndPrio(std::string action, int prio) { _requestedAction = action; _priority = prio; };

	void setPausedAndFrame(bool pause, int frame) {
		_paused = pause;
		_activeFrame = _node->getFrame();
		_pausedFrame = frame;
	};

	bool getPaused() { return _paused; };

	int getPausedFrame() { return _pausedFrame; };
	
	int getActiveFrame() { return _activeFrame; };

	float getActionDuration(std::string actionName) {return std::get<3>(_info[actionName]); };

	void setHealth(float health) { _health = health; };

	void setAction(std::string action_name, std::vector<int> vec, float duration);
};
#endif /* __ENTITY_H__ */