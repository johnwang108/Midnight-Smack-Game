#ifndef __SHRIMPRICE_H__
#define __SHRIMPRICE_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include "../Entity.h"

using namespace cugl;

#define SHRIMPRICE_SENSOR_NAME     "ShrimpRicesensor" // If the SHRIMPRICE requires a unique sensor
#define SHRIMPRICE_CHASE_SPEED     2.0f         // Using the CHASE_SPEED for consistency
#define SHRIMPRICE_DENSITY         2.0f         // Assuming the SHRIMPRICE is heavier than a regular enemy
#define SHRIMPRICE_FORCE           1.0f         // Force specific to the SHRIMPRICE's movement, potentially stronger
#define SHRIMPRICE_MAXSPEED        5.0f         // A reasonable max speed for the SHRIMPRICE, ensuring it's fast but manageable
#define SHRIMPRICE_KNOCKBACK_FORCE 15.0f        // Increased knockback force due to the SHRIMPRICE's strength
#define SHRIMPRICE_KNOCKBACK_FORCE_UP 5.0f      // Vertical knockback component

#define SHRIMPRICE_VSHRINK    0.8f
#define SHRIMPRICE_HSHRINK    0.7f

#define SENSOR_HEIGHT 0.1f

class GameScene;
class ShrimpRice : public Entity {
protected:
    float _drawScale;
    float _health;
    float _healthCooldown;
    float _lastDamageTime;
    bool _isChasing;
    int _direction;
    float _SFR_attack_chance;
    float _knockbackTime;
    float _nextChangeTime;
    b2Fixture* _sensorFixture;
    std::string _sensorName;
    std::shared_ptr<AssetManager> _assets;
    int _lastDirection;
    std::string _attacktype;
    std::string _act;
    float _acttime;


public:
    ShrimpRice() : Entity(), _drawScale(1.0f), _health(100.0f), _healthCooldown(0.2f), _lastDamageTime(0), _isChasing(true), _direction(-1) {}

    virtual ~ShrimpRice() { dispose(); }

    void dispose();

    bool init(const Vec2& pos, const Size& size, float scale);

    static std::shared_ptr<ShrimpRice> alloc(const Vec2& pos, const Size& size, float scale) {
        std::shared_ptr<ShrimpRice> result = std::make_shared<ShrimpRice>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    void update(float dt) override;

    void takeDamage(float damage, int attackDirection, bool knockback);

    void createFixtures() override;

    void releaseFixtures() override;

    std::string* getSensorName() { return &_sensorName; };
    void setassets(std::shared_ptr<AssetManager> assets) { _assets = assets; }

    float getHealth() { return _health; }
    bool isChasing() const { return _isChasing; }

    double getnextchangetime() { return _nextChangeTime; }
    void setDirection(int d) { _direction = d; }
    void setnextchangetime(double nextChangeTime) { _nextChangeTime = nextChangeTime; }
    float getknockbacktime() { return _knockbackTime; }

    std::string getattacktype() { return _attacktype; }
    void setattacktype(std::string type){_attacktype=type;}

    void setact(std::string act, float time) {
        _acttime = time;
        _act = act;
    }
    float getacttime() { return _acttime; }
    std::string getact() { return _act; }
};

#endif /* __SHRIMPRICE_H__ */
