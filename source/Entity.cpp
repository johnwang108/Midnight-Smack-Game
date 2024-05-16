#include "Entity.h"

int Entity::ID = 0;

bool Entity::init(cugl::Vec2 pos, cugl::Size size) {
    /*cugl::Rect rect = cugl::Rect(Vec2::ZERO, size);
    _dimension = size;
    return PolygonObstacle::init(rect);*/

    if (CapsuleObstacle::init(pos, size, poly2::Capsule::FULL)) {
        _entityID = ID++;

        //CULog("Entity ID: %d", _entityID);
        _activated = false;
        _finished = false;
        _activeAction = "";
        _pausedFrame = 0;
        _activeFrame = 0;
        _paused = false;
        return true;
    }
    return false;
}
/** Register a new animation in the dict*/
void Entity::addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool reverse) {
    std::vector<int> forward;
    if (reverse) {
        for (int ii = size - 1; ii >= 0; ii--) {
			forward.push_back(ii);
		}
	}
    else {
        for (int ii = 0; ii < size; ii++) {
			forward.push_back(ii);
		}
	}
    _actions[action_name] = cugl::scene2::Animate::alloc(forward, duration);
    _sheets[action_name] = sheet;
    _info[action_name] = std::make_tuple(rows, cols, size, duration);
}

void Entity::setAction(std::string action_name, std::vector<int> vec, float duration) {
    _actions[action_name] = cugl::scene2::Animate::alloc(vec, duration);
}

/**Unsure if override needed. Begins an animation, switching the sheet if needed.*/
bool Entity::animate(std::string action_name) {
    std::string name = action_name;

    //info = {int rows, int cols, int size, float duration}
    auto info = _info[name];

    //first, switch the sheet
    if (name.find("bull") != std::string::npos) {
		_node->setScale(0.5/4);
    }
    else if (name.find("SFR") != std::string::npos) {
        _node->setScale(0.3);
    }
    else if (name == "idle") {
        _node->setScale(0.35 / 1.75);
    }
    else if (name == "skid" || name == "hurt" || name == "death") {
        _node->setScale(0.35 / 1.25);
    }
    else {
        _node->setScale(0.35 / 1.75);
    }

    if (getName() == "avatar") {
        if (name == "run" || name == "skid") {
            float offset = _node->isFlipHorizontal() ? -0.17 : 0.17;
            getSceneNode()->setAnchor(Vec2(0.5 + offset, 0.4));
        }
        else getSceneNode()->setAnchor(Vec2(0.5, 0.35));
    }
    if (getName().find("shrimp") != std::string::npos && getName().find("Death") != std::string::npos) {
        getSceneNode()->setAnchor(Vec2(0.5, 0.15));
    }
    
    changeSheet(name);

    _node->setFrame(0);

    setActiveAction(action_name);
    _activePriority = _priority;
    _activeAction = name;

    return true;
}

void Entity::changeSheet(std::string action_name) {
    //info = {int rows, int cols, int size, float duration, bool isPassive}
    try {
        auto info = _info[action_name];
  //      for (auto it = _info.begin(); it != _info.end(); ++it) {
		//	CULog("Action name: %s", it->first.c_str());
		//}
        _node->changeSheet(_sheets[action_name], std::get<0>(info), std::get<1>(info), std::get<2>(info));
    }
    catch (int e) {
        CULog("Error changing sheets. Is the action registered?");
    }
}

void Entity::loadAnimationsFromConstant(std::string entityName, std::shared_ptr<AssetManager> _assets) {
    auto reader = JsonReader::allocWithAsset("./json/constants.json");
    std::shared_ptr<JsonValue> json = reader->readJson();
    std::shared_ptr<JsonValue> entity = json->get(entityName);
    auto children = entity->children();
    for (auto it = children.begin(); it != children.end(); ++it) {
        std::shared_ptr<JsonValue> action = *it;
        std::string action_name = action->key();
        std::string sheet_name = action->getString("sheet");
        int rows = action->getInt("rows");
        int cols = action->getInt("cols");
        int size = action->getInt("frames");
        float duration = action->getFloat("duration");
        addActionAnimation(action_name, _assets->get<Texture>(sheet_name), rows, cols, size, duration);
    }
}