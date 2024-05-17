#ifndef __TutorialSign_H__
#define __TutorialSign_H__

#include <cugl/cugl.h>
#include "../EntitySpriteNode.h"
#include "../Popup.h"
using namespace cugl;


class TutorialSign : public physics2::BoxObstacle {
private:
    bool _isActive;

    //sprite node
    std::shared_ptr<EntitySpriteNode> _node;
    
    std::string _popuptype;

    b2Filter filter;

    int _capacity;

    int _interactableId;
protected:

public:

    static int ID;

    bool init(const std::shared_ptr<Texture>& texture,  const cugl::Vec2& pos, const cugl::Size& size, const std::string type);

    static std::shared_ptr<TutorialSign> alloc(const std::shared_ptr<Texture>& texture, const Vec2& pos, const Size& s, std::string type) {
        std::shared_ptr<TutorialSign> result = std::make_shared<TutorialSign>();
        bool res = result->init(texture, pos, s, type);
        if (!res) return nullptr;
        result->setPopupType(type);
        return result;
    }

    std::shared_ptr<scene2::SceneNode> getSceneNode() { return _node; }
    std::shared_ptr<EntitySpriteNode> getSpriteNode() { return _node; }

    void setSpriteNode(const std::shared_ptr<EntitySpriteNode>& node);
    void setActive(bool active);
    bool isActive();


    void changeTexture(const std::shared_ptr<Texture>& texture){ _node->setTexture(texture); }

    void clearIngredients();

    virtual void hit() {};


    int getCapacity();

    int getId() { return _interactableId; }

    void setCapacity(int capacity);

    void setPopupType(std::string type) { _popuptype = type; }

    std::string getPopupType() { return _popuptype; }

};

#endif /* __TutorialSign_H__ */
