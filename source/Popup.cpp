#include "Popup.h"

void Popup::setBackground(std::shared_ptr<scene2::PolygonNode> background){
		_background = background;
};
void Popup::setText(std::shared_ptr<scene2::Label> label) {
	_text = label;
};

void Popup::setChildren(std::vector<std::shared_ptr<EntitySpriteNode>> children) {
	_children = children;
}

void Popup::setChildBehavior(std::string childName, std::string stateName, std::vector<std::shared_ptr<scene2::Action>> actions) {
	_animationData[stateName][childName] = actions;
}

void Popup::update(float dt) {
	if (_isActive) {
		setVisible(true);
		if (_timer > std::get<1>(_states[_stateIndex])) {
			CULog("NEXT STATING");
			nextState();
			_timer = 0;
		}
		else {
			_timer += dt;
		}
	}
	else {
		setVisible(false);
	}
}

void Popup::nextState()	{
	_stateIndex++;
	if (_stateIndex >= _states.size()) {
		reset();
	}
	loadCurrentState();
}

void Popup::loadCurrentState() {
	for (auto& child : _children) {
		//load in new actions
		_actionManager->clearAllActions(child);

		std::string stateName = std::get<0>(_states[_stateIndex]);
		std::string childName = child->getName();
		if (_animationData[stateName].find(childName) == _animationData[stateName].end()) {
			child->setVisible(false);
			continue;
		}
		child->setVisible(true);
		child->setFrame(0);
		std::vector<std::shared_ptr<scene2::Action>> actions = _animationData[stateName][childName];
		int i = 0;
		for (auto& action : actions) {
			if (action != nullptr) _actionManager->activate(getName() + stateName + childName + std::to_string(i), action, child);
			i++;
		}
	}
}

bool Popup::isActive() {
	return _isActive;
}

void Popup::reset() {
	layoutEverything();
	_timer = 0;
	_stateIndex = 0;
}

void Popup::setActive(bool active) {
	if (active && !_isActive) {
		setVisible(true);
		reset();
		loadCurrentState();
	}
	else if (!active && _isActive) {
		setVisible(false);
	}
	_isActive = active;
}

void Popup::layoutEverything() {
	//3
	_text->setPosition(getPosition());
	//4, 5
	_text->setContentWidth(std::min(_text->getTextBounds().size.width, getContentWidth()));

	//unscaled size of the children box. Todo polish :)
	Size childrenSize = Size(getContentSize().width, getContentSize().height - _text->getContentHeight());
	_childrenAnimBox->setContentSize(childrenSize);

	_childrenAnimBox->setAnchor(Vec2::ANCHOR_TOP_CENTER);
	_childrenAnimBox->setPosition(getWidth()/2, getHeight());

	for (auto& child : _children) {
		child->setPosition(_childStartingPositions[child->getName()]);
		child->setPositionX(child->getPositionX() + _childrenAnimBox->getWidth()/2);
		child->setScale(_childScale[child->getName()] * _childrenAnimBox->getHeight()/child->getContentHeight());
		Color4 c = child->getColor();
		c.a = 255;
		child->setColor(c);
		_actionManager->clearAllActions(child);
	}

	_text->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
	_text->setPosition(getWidth()/2, 0);
	
	//6
	Rect r1 = _text->getBoundingBox();
	Rect r2 = _childrenAnimBox->getBoundingBox();

	float width = std::max(r1.size.width, r2.size.width);
	float height = r1.size.height + r2.size.height;
	float targetScale = _targetSize.width * _targetSize.height / (width * height);
	setScale(targetScale);

	CULog("DONE AUTOLAYOUT");
	CULog("width: %f, height: %f, scale: %f", width, height, targetScale);
}

std::string Popup::processText(std::string input) {
	//int chunkSize = LABEL_MAX_LENGTH;
	//std::string text = input;
	//for (size_t i = chunkSize; i < text.length(); i += chunkSize) {
	//	// Find the last space within the chunk
	//	size_t pos = text.rfind(' ', i);
	//	if (pos == std::string::npos || pos == i - 1) {
	//		// If there's no space or space is at the end, insert newline
	//		text.insert(i, "\n");
	//	}
	//	else {
	//		// Replace the space with a newline
	//		text[pos] = '\n';
	//	}
	//}
	//return text;
	return input;
}

void Popup::dispose() {
	_background = nullptr;
	_text = nullptr;
	_children.clear();
	_childrenAnimBox = nullptr;
	_actionManager = nullptr;
	for (auto& state : _animationData) {
		for (auto& child : state.second) {
			for (auto& action : child.second) {
				action = nullptr;
			}
			child.second.clear();
		}
		state.second.clear();
	}
	_animationData.clear();
	_childStartingPositions.clear();
	_childScale.clear();
	_states.clear();
	_parent = nullptr;
	SceneNode::dispose();
}