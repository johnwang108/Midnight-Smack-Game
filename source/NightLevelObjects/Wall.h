#ifndef __WALL_H__
#define __WALL_H__

#include <cugl/cugl.h>

class Wall : public cugl::physics2::PolygonObstacle {
private:
    std::shared_ptr<cugl::physics2::PolygonObstacle> _obj;
    std::shared_ptr<cugl::Texture> image;
    cugl::Vec2* WALL_POS;

    float _scale;
    float BASIC_DENSITY;
    float BASIC_FRICTION;
    float BASIC_RESTITUTION;
    cugl::Color4 DEBUG_COLOR;
    int WALL_VERTS;
    int breakableCoolDown;
    bool doesDamage;

    cugl::Poly2 _collisionPoly;
    std::string name;
    std::shared_ptr<cugl::scene2::SceneNode> sprite;
public:
    Wall();
    bool init(std::shared_ptr<cugl::Texture> image, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, float BASIC_RESTITUTION,
        cugl::Color4 DEBUG_COLOR, cugl::Vec2* WALL_POS, int WALL_VERTS, std::string name, int breakableCooldown = -1, bool doesDamage = false);
    static std::shared_ptr<Wall> alloc(std::shared_ptr<cugl::Texture> image, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, float BASIC_RESTITUTION,
        cugl::Color4 DEBUG_COLOR, cugl::Vec2* WALL_POS, int WALL_VERTS, std::string name, int breakableCooldown = -1, bool doesDamage = false);
    std::shared_ptr<cugl::physics2::PolygonObstacle> getObj();
    cugl::Poly2 getCollisionPoly();
    std::shared_ptr<cugl::scene2::SceneNode> Wall::getSprite();
};

#endif /* __WALL_H__ */