#ifndef __ENTITY_ACTION_MANAGER_H__
#define __ENTITY_ACTION_MANAGER_H__

#include <cugl/cugl.h>
#include <cugl/scene2/CUActionManager.h>

using namespace cugl;

class EntityActionManager : public cugl::scene2::ActionManager {
private:

protected:

public:

	void EntityActionManager::update(float dt) {
		ActionManager::update(dt);
	}

}

#endif /*__ENTITY_ACTION_MANAGER_H__*/