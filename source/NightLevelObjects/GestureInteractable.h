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
    
    //map from ingredient type to count currently in the interactable
    std::unordered_map<IngredientType, int> _ingredients;

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
    virtual bool interact(IngredientType t);

    virtual bool addIngredient(IngredientType i);

    bool popIngredient(IngredientType i);

    void changeTexture(const std::shared_ptr<Texture>& texture){ _node->setTexture(texture); }

    void clearIngredients();

    virtual void hit() {};

    int getCount(IngredientType type) {
		return _ingredients[type];
	}

    int getTotalCount() {
        return getCount(IngredientType::rice) + getCount(IngredientType::carrot) +
            getCount(IngredientType::beef) + getCount(IngredientType::egg) +
            getCount(IngredientType::shrimp) + getCount(IngredientType::riceCooked) + getCount(IngredientType::carrotCooked) +
            getCount(IngredientType::beefCooked) + getCount(IngredientType::eggCooked) +
            getCount(IngredientType::shrimpCooked);
    }

    int getCapacity();

    int getId() { return _interactableId; }

    void setCapacity(int capacity);

    std::unordered_map<IngredientType, int> getIngredients() {
		return _ingredients;
	}
};

#endif /* __GESTURE_INTERACTABLE_H__ */