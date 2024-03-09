#ifndef __BOSS3_H__
#define __BOSS3_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>

using namespace cugl;

#define Boss3_SENSOR_NAME     "Boss3sensor" // If the Boss3 requires a unique sensor
#define Boss3_CHASE_SPEED     2.0f         // Using the CHASE_SPEED for consistency
#define Boss3_DENSITY         2.0f         // Assuming the Boss3 is heavier than a regular enemy
#define Boss3_FORCE           1.0f         // Force specific to the Boss3's movement, potentially stronger
#define Boss3_MAXSPEED        5.0f         // A reasonable max speed for the Boss3, ensuring it's fast but manageable
#define Boss3_KNOCKBACK_FORCE 15.0f        // Increased knockback force due to the Boss3's strength
#define Boss3_KNOCKBACK_FORCE_UP 5.0f      // Vertical knockback component

#define Boss3_VSHRINK    0.8f
#define Boss3_HSHRINK    0.7f

#define SENSOR_HEIGHT 0.1f

class Boss3 : public physics2::CapsuleObstacle {
protected:
    float _drawScale;
    float _health;
    float _healthCooldown;
    float _lastDamageTime;
    bool _isChasing;
    int _direction;
    float _knockbackTime;
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    b2Fixture* _sensorFixture;
    std::string _sensorName;

public:
    Boss3() : CapsuleObstacle(), _drawScale(1.0f), _health(100.0f), _healthCooldown(0.2f), _lastDamageTime(0), _isChasing(true), _direction(-1) {}

    virtual ~Boss3() { dispose(); }

    void dispose();

    bool init(const Vec2& pos, const Size& size, float scale);

    static std::shared_ptr<Boss3> alloc(const Vec2& pos, const Size& size, float scale) {
        std::shared_ptr<Boss3> result = std::make_shared<Boss3>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    void update(float dt) override;

    void takeDamage(float damage, int attackDirection);

    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node);

    void createFixtures() override;

    void releaseFixtures() override;

    std::string* getSensorName() { return &_sensorName; };
};

#endif /* __BOSS3_H__ */