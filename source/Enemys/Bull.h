#ifndef __BULL_MODEL_H__
#define __BULL_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>

using namespace cugl;

#define BULL_SENSOR_NAME     "bullsensor" // If the Bull requires a unique sensor
#define BULL_CHASE_SPEED     5.0f         // Using the CHASE_SPEED for consistency
#define BULL_DENSITY         2.0f         // Assuming the Bull is heavier than a regular enemy
#define BULL_FORCE           1.0f         // Force specific to the Bull's movement, potentially stronger
#define BULL_MAXSPEED        5.0f         // A reasonable max speed for the Bull, ensuring it's fast but manageable
#define BULL_KNOCKBACK_FORCE 5.0f        // Increased knockback force due to the Bull's strength
#define BULL_KNOCKBACK_FORCE_UP 15.0f      // Vertical knockback component

#define BULL_VSHRINK    0.7f
#define BULL_HSHRINK    0.5f

#define SENSOR_HEIGHT 0.1f

#define BULL_ATTACK_CHANCE 0.001f

class GameScene;
class BullModel : public physics2::CapsuleObstacle {
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
    int _lastDirection;
    float _nextChangeTime;
    float _angrytime;
    float _sprintPrepareTime;
    bool _P2start;
    bool _shake;

public:
    BullModel() : CapsuleObstacle(), _drawScale(1.0f), _health(100.0f), _healthCooldown(0.2f), _lastDamageTime(0), _isChasing(true), _direction(-1) {}

    virtual ~BullModel() { dispose(); }

    void dispose();

    bool init(const Vec2& pos, const Size& size, float scale);

    static std::shared_ptr<BullModel> alloc(const Vec2& pos, const Size& size, float scale) {
        std::shared_ptr<BullModel> result = std::make_shared<BullModel>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    void update(float dt) override;

    void takeDamage(float damage, int attackDirection, bool knockback);

    void setSceneNode(const std::shared_ptr<scene2::SceneNode>& node);

    void createFixtures() override;

    void releaseFixtures() override;

    std::string* getSensorName() { return &_sensorName; };

    void setIsChasing(bool isChasing) { _isChasing = isChasing; }

    bool isChasing() const { return _isChasing; }

    float getHealth() { return _health; }

    int getDirection() const { return _direction; }

    void setDirection(int d) { _direction = d; }

    void setnextchangetime(double nextChangeTime) { _nextChangeTime = nextChangeTime; }

    double getnextchangetime() { return _nextChangeTime; }

    void createAttack(GameScene& scene);
    void createAttack2(GameScene& scene);
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }

    void setangrytime(float time) { _angrytime = time; }

    float getangrytime() { return _angrytime; }

    float getknockbacktime() { return _knockbackTime; }

    bool getshake() { return _shake; }

    void setshake(bool shake) { _shake = shake; }

};

#endif /* __BULL_MODEL_H__ */