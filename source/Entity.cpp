#include "Entity.h"


bool Entity::init(cugl::Vec2 pos, cugl::Size size) {
    /*cugl::Rect rect = cugl::Rect(Vec2::ZERO, size);
    _dimension = size;
    return PolygonObstacle::init(rect);*/

    return CapsuleObstacle::init(pos, size);
}
/** Register a new animation in the dict*/
void Entity::addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool isPassive) {
    std::vector<int> forward;
    for (int ii = 0; ii < size; ii++) {
        forward.push_back(ii);
    }
    _actions[action_name] = cugl::scene2::Animate::alloc(forward, duration);
    _sheets[action_name] = sheet;
    _info[action_name] = std::make_tuple(rows, cols, size, duration, isPassive);
}

/**Unsure if override needed. Begins an animation, switching the sheet if needed.*/
void Entity::animate(std::string action_name) {
    //info = {int rows, int cols, int size, float duration, bool isPassive}
    auto info = _info[action_name];

    //first, switch the sheet
    changeSheet(action_name);
    if (action_name == "idle") {
        _node->setScale(0.35 / 4);
    }
    else {
        _node->setScale(0.35 / 4);
    }
    _activeAction = action_name;
}

void Entity::changeSheet(std::string action_name) {
    //info = {int rows, int cols, int size, float duration, bool isPassive}
    try {
        auto info = _info[action_name];
        _node->changeSheet(_sheets[action_name], std::get<0>(info), std::get<1>(info), std::get<2>(info));
    }
    catch (int e) {
        CULog("Error changing sheets. Is the action registered?");
    }
}