#ifndef __ORDERS_H__
#define __ORDERS_H__

#include <cugl/cugl.h>
#include "Ingredient.h"

using namespace cugl;

#define ORDER_HEIGHT 80.0f

class Orders: public cugl::scene2::SceneNode {
protected:
    //std::map<int, std::vector<std::shared_ptr<scene2::SceneNode>>> _orders;
    std::vector< std::vector<std::shared_ptr<scene2::SceneNode>>> _orderList;
    std::vector<int> _plateIds;
    std::vector<Color4> _colors;
    std::shared_ptr<scene2::SceneNode> _rootNode;
    int _orderPlateIndex;

    std::shared_ptr<scene2::Button> _leftButton;
    std::shared_ptr<scene2::Button> _rightButton;
    std::shared_ptr<scene2::PolygonNode> _plateNode;
    bool _isActive;

private:


public:

    bool init(const std::shared_ptr<AssetManager>& assets);

    void setActive(bool b);

    void changeIndex(int i);

    void layoutOrders();

    void addOrder(int index, std::shared_ptr<scene2::SceneNode> order);

    void removeOrder(int index, IngredientType t);

    void reset();
};

#endif /* __ORDERS_H__ */