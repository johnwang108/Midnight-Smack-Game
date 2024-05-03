#ifndef __GESTURE_INTERACTABLE_H__
#define __GESTURE_INTERACTABLE_H__

#include <cugl/cugl.h>
#include "../PFDollarScene.h"
#include "../EntitySpriteNode.h"
#include "../Ingredient.h"
using namespace cugl;


//This class will be used for any interactable object that can be interacted with through DollarScene.
class GestureInteractable : public physics2::BoxObstacle {
private:
    bool _isActive;

    //sprite node
    std::shared_ptr<EntitySpriteNode> _node;
    
    std::unordered_map<std::string, std::shared_ptr<Ingredient>> _ingredients;

    int _capacity;

    int _interactableId;
protected:

public:

    static int ID;

    bool init(const std::shared_ptr<Texture>& texture,  const cugl::Vec2& pos, const cugl::Size& size);

    std::shared_ptr<GestureInteractable> alloc(Vec2 pos, Size s, std::string type) {

    }

    std::shared_ptr<scene2::SceneNode> getSceneNode() { return _node; }
    std::shared_ptr<EntitySpriteNode> getSpriteNode() { return _node; }

    void setSpriteNode(const std::shared_ptr<EntitySpriteNode>& node);
    void setActive(bool active);
    bool isActive();
    bool interact();

    bool addIngredient(std::shared_ptr<Ingredient> i);

    bool popIngredient(std::shared_ptr<Ingredient> i);

    void clearIngredients();

    int getCapacity();

    int getId() { return _interactableId; }

    void setCapacity(int capacity);
};

#endif /* __GESTURE_INTERACTABLE_H__ */