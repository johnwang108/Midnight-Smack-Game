#ifndef __WALL_H__
#define __WALL_H__

#include <cugl/cugl.h>
using namespace cugl;

class Wall : public physics2::PolygonObstacle {
private:
    std::shared_ptr<physics2::PolygonObstacle> _obj;
    std::shared_ptr<Texture> image;
    Vec2 WALL_POS;

    float _scale;
    float BASIC_DENSITY;
    float BASIC_FRICTION;
    float BASIC_RESTITUTION;
    Color4 DEBUG_COLOR;
    int WALL_VERTS;

    int breakableCoolDown;
    int respawnTime;
    int breakingClock;
    bool activeDisplay;

    int damageCoolDown;
    int damageRespawnTime;
    int damageClock;

    bool doesDamage;
    bool flag;
    bool readyToBeReset;

    float ogX;
    float ogY;
    int pathNodeCoolDown;
    std::vector<Vec3> path;
    float movementForce;
    int currentPathNode;

    Poly2 _collisionPoly;
    std::string name;
    std::shared_ptr<scene2::SceneNode> sprite;

    void setActive(bool state);
public:
    Wall();
    bool init(std::shared_ptr<Texture> image, std::shared_ptr<physics2::PolygonObstacle> _collisionPoly, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, float BASIC_RESTITUTION,
        Color4 DEBUG_COLOR, Vec2 WALL_POS, int WALL_VERTS, std::string name, bool doesDamage = false);
    static std::shared_ptr<Wall> alloc(std::shared_ptr<Texture> image, std::shared_ptr<physics2::PolygonObstacle> _collisionPoly, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, float BASIC_RESTITUTION,
        Color4 DEBUG_COLOR, Vec2 WALL_POS, int WALL_VERTS, std::string name, bool doesDamage = false);
    std::shared_ptr<physics2::PolygonObstacle> getObj();
    Poly2 getCollisionPoly();
    std::shared_ptr<scene2::SceneNode> getSprite();

    void initPath(std::vector<Vec3> path, int movementForce);
    void initBreakable(int duration, int respawnTime);
    void initDamage(int duration, int respawnTime);
    void update(float dt);
    void fixedUpdate(float step);
    Vec3 queryPath(int temp);
    void applyPathMovement(float step);
    void applyBreaking();
    void applyDamage();
    float getOGX();
    float getOGY();
    void resetBreaking();
    int getBreakingClock();

    bool isFlagged() {
        return flag;
    }

    bool isDamage() {
        return doesDamage;
    }

    void setFlag(bool f) {
        flag = f;
    }

    bool isReadyToReset() {
        return readyToBeReset;
    }

    void setReadyToBeReset(bool rr) {
        readyToBeReset = rr;
    }
};

#endif /* __WALL_H__ */