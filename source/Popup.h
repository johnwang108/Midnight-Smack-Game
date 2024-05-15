#ifndef __POPUP_H__
#define __POPUP_H__

#include <cugl/cugl.h>
#include "EntitySpriteNode.h"

using namespace cugl;

#define LABEL_MAX_LENGTH 30

//This class potentially has a background, text content, and animated children. LIMITATIONS: each entity sprite node can only have one sprite sheet...
class Popup : public scene2::SceneNode {
private:
	/** The background of the popup */
	std::shared_ptr<scene2::PolygonNode> _background;
	/** The text content of the popup */
	std::shared_ptr<scene2::Label> _text;

	std::shared_ptr<scene2::SceneNode> _childrenAnimBox;
	/** The animated children of the popup */
	std::vector<std::shared_ptr<EntitySpriteNode>> _children;

	std::shared_ptr<scene2::ActionManager> _actionManager;

	bool _isActive;

	/** The current animation */
	std::string _animation;

	/** The animation data for the popup. Map from state name, to a map of child names, to sequential action vector */
	std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::shared_ptr<scene2::Action>>>> _animationData;

	std::unordered_map<std::string, Vec2> _childStartingPositions;

	/** scale relative to childAnimBox. 1.0 means height = height of childAnimBox*/
	std::unordered_map<std::string, float> _childScale;

	/** The current state of the popup */
	int _stateIndex;
	std::vector<std::tuple<std::string, float>> _states;

	Size _targetSize;
	
	float _timer;
protected:

public:


	bool initWithData(const Scene2Loader* loader, const std::shared_ptr<JsonValue>& data) override {
		if (SceneNode::initWithData(loader, data)) {
			setName(data->key());
			_isActive = false;
			_states = std::vector<std::tuple<std::string, float>>();
			_stateIndex = 0;
			_animationData = std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::shared_ptr<scene2::Action>>>>();
			_timer = 0;
			_children = std::vector<std::shared_ptr<EntitySpriteNode>>();
			_targetSize = getSize();
			_childrenAnimBox = scene2::SceneNode::allocWithBounds(getSize());
			_childrenAnimBox->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
			//auto temp = scene2::PolygonNode::allocWithPoly(Poly2(Rect(Vec2::ZERO, getSize())));
			//temp->setColor(Color4::RED);
			//_childrenAnimBox->addChild(temp);
			_childStartingPositions = std::unordered_map<std::string, Vec2>();
			_childScale = std::unordered_map<std::string, float>();
			return true;
		}
		return false;
	}

	static std::shared_ptr<Popup> alloc() {
		std::shared_ptr<Popup> node = std::make_shared<Popup>();
		return (node->init() ? node : nullptr);
	}

	static std::shared_ptr<Popup> allocWithData(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<scene2::ActionManager>& actionManager, const Scene2Loader* loader,
		const std::shared_ptr< JsonValue >& data) {
		std::shared_ptr<Popup> node = std::make_shared<Popup>();
		auto res = node->initWithData(loader, data);
		if (!res) return nullptr;
		node->setActionManager(actionManager);

		if (data->has("text")) {
			std::shared_ptr<scene2::Label> text = scene2::Label::allocWithText(data->getString("text"), assets->get<Font>(data->getString("font")));
			std::string txt = node->processText(text->getText());
			text->setContentWidth(data->getFloat("textMaxWidth%age"));
			text->setText(txt);
			text->setBackground(Color4::GREEN);
			node->setText(text);
		}

		if (data->has("background")) {
			std::shared_ptr<scene2::PolygonNode> background = scene2::PolygonNode::allocWithPoly(Poly2(Rect(node->getPosition(), node->getSize())));
			background->setTexture(assets->get<Texture>(data->getString("background")));
			background->setContentSize(node->getSize());
			background->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
			background->setPosition(0, 0);
			node->setBackground(background);
		}

		if (data->has("children")) {
			std::vector<std::shared_ptr<EntitySpriteNode>> children = std::vector<std::shared_ptr<EntitySpriteNode>>();
			for (auto& child : data->get("children")->children()) {
				std::shared_ptr<EntitySpriteNode> childNode = EntitySpriteNode::allocWithSheet(assets->get<Texture>(child->getString("texture")), 
					child->getInt("rows"), child->getInt("cols"), child->getInt("frames"));
				childNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
				childNode->setName(child->key());
				//childNode->setVisible(false);
				childNode->setPosition(child->get("position")->asFloatArray()[0], child->get("position")->asFloatArray()[1]);
				node->_childStartingPositions[child->key()] = Vec2(child->get("position")->asFloatArray()[0], child->get("position")->asFloatArray()[1]);
				//todo?
				node->_childScale[child->key()] = child->get("scale")->asFloat();
				children.push_back(childNode);
			}
			node->setChildren(children);
		}

		if (data->has("states")) {
			//iterate over states
			for (auto& state : data->get("states")->children()) {
				std::string stateName = state->key();
				float stateDuration = state->get("duration")->asFloat();
				node->_states.push_back(std::make_tuple(stateName,stateDuration));

				//iterate over children (scenenodes)
				for (auto& child : state->get("children")->children()) {
					std::string childName = child->key();
					std::vector<std::shared_ptr<scene2::Action>> actions = std::vector<std::shared_ptr<scene2::Action>>();

					//iterate over actions of each child
					for (auto& action : child->children()) {
						//CULog("action type: %s", action->key());
						std::string actionType = action->get("type")->asString();
						std::shared_ptr<scene2::Action> act;
						if (actionType == "animate") {
							act = scene2::Animate::alloc(action->get("start")->asInt(), action->get("end")->asInt(), action->get("time")->asFloat(), action->get("repeat")->asInt());
						}
						else if (actionType == "fadeIn") {
							act = scene2::FadeIn::alloc(action->get("time")->asFloat());
						} 
						else if (actionType == "fadeOut") {
							act = scene2::FadeOut::alloc(action->get("time")->asFloat());
						}
						else if (actionType == "moveBy") {
							act = scene2::MoveBy::alloc(Vec2(action->get("x")->asFloat(), action->get("y")->asFloat()), action->get("time")->asFloat());
						}
						else if (actionType == "moveTo") {
							act = scene2::MoveTo::alloc(Vec2(action->get("x")->asFloat(), action->get("y")->asFloat()), action->get("time")->asFloat());
						}
						else if (actionType == "rotateBy") {
							act = scene2::RotateBy::alloc(action->get("angle")->asFloat(), action->get("time")->asFloat());
						}
						else if (actionType == "rotateTo") {
							act = scene2::RotateTo::alloc(action->get("angle")->asFloat(), action->get("time")->asFloat());
						}
						else if (actionType == "scaleBy") {
							act = scene2::ScaleBy::alloc(Vec2(action->get("x")->asFloat(), action->get("y")->asFloat()), action->get("time")->asFloat());
						}
						else if (actionType == "scaleTo") {
							act = scene2::ScaleTo::alloc(Vec2(action->get("x")->asFloat(), action->get("y")->asFloat()), action->get("time")->asFloat());
						}
						else if (actionType == "nothing") {
							act = nullptr;
						}
						actions.push_back(act);
					}

					node->setChildBehavior(childName, stateName, actions);
				}
			}
		}

		//set children

		node->addChild(node->_background);
		node->addChild(node->_text);
		for (auto& child : node->_children) {
			node->_childrenAnimBox->addChild(child);
		}
		node->addChild(node->_childrenAnimBox);
		node->layoutEverything();

		return node;

	}

	void setBackground(std::shared_ptr<scene2::PolygonNode> background);
	void setText(std::shared_ptr<scene2::Label> label);
	void setChildren(std::vector<std::shared_ptr<EntitySpriteNode>> children);
	void setChildBehavior(std::string childName, std::string stateName, std::vector<std::shared_ptr<scene2::Action>> actions);
	void setActionManager(const std::shared_ptr<scene2::ActionManager>& actionManager) {
		_actionManager = actionManager;
	}

	void update(float dt);

	void nextState();

	bool isActive();

	void setActive(bool active);

	void reset();

	void loadCurrentState();

	void toggle() {
		setActive(!isActive());
	}

	/**
	This function has to:
	1. Get the size
	2. Set the background size equal to the node size.
	3. Automatically position the children inside a children region.
	4. Automatically position the text inside the text region.
	5. Position the two relative to each other
	6. Calculate a scale such that the scaled parent size equals the original size.
	*/
	void layoutEverything();

	void dispose();

	std::string processText(std::string text);
};
#endif /* __POPUP_H__ */
