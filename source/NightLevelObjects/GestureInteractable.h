#ifndef __GESTURE_INTERACTABLE_H__
#define __GESTURE_INTERACTABLE_H__

#include <cugl/cugl.h>
#include "../PFDollarScene.h"
#include "../EntitySpriteNode.h"
#include "../Ingredient.h"
using namespace cugl;

class GestureInteractable : public physics2::BoxObstacle {
private:
    bool _isActive;

    std::shared_ptr<EntitySpriteNode> _node;
    
    std::unordered_set<Ingredient> _ingredients;

    int _capacity;
    
protected:

public:


    bool init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size);

    void setSpriteNode(const std::shared_ptr<EntitySpriteNode>& node);
    void setActive(bool active);
    bool isActive();
    bool interact();

    bool addIngredient(Ingredient i);

    bool popIngredient(Ingredient i);

    void clearIngredients();

    int getCapacity();

    void setCapacity(int capacity);
};

#endif /* __GESTURE_INTERACTABLE_H__ */