#ifndef __BULL_MODEL_H__
#define __BULL_MODEL_H__

#include <cugl/cugl.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include "../Entity.h"

using namespace cugl;

#define BULL_SENSOR_NAME     "bullsensor" // If the Bull requires a unique sensor
#define BULL_CHASE_SPEED     10.0f         // Using the CHASE_SPEED for consistency
#define BULL_DENSITY         2.0f         // Assuming the Bull is heavier than a regular enemy
#define BULL_FORCE           1.5f         // Force specific to the Bull's movement, potentially stronger
#define BULL_MAXSPEED        7.5f         // A reasonable max speed for the Bull, ensuring it's fast but manageable
#define BULL_KNOCKBACK_FORCE 5.0f        // Increased knockback force due to the Bull's strength
#define BULL_KNOCKBACK_FORCE_UP 15.0f      // Vertical knockback component

#define BULL_VSHRINK    0.7f
#define BULL_HSHRINK    0.5f

#define SENSOR_HEIGHT 0.1f

#define BULL_ATTACK_CHANCE 0.003f

class GameScene;
class BullModel : public Entity {
protected:
    Size scaledSize;
    float _drawScale;
    float _health;
    float _healthCooldown;
    float _lastDamageTime;
    bool _isChasing;
    int _direction;
    float _knockbackTime;
    b2Fixture* _sensorFixture;
    std::string _sensorName;
    int _lastDirection;
    float _nextChangeTime;
    float _angrytime;
    float _sprintPrepareTime;
    bool _P2start;
    bool _shake;
    float _bull_attack_chance;
    std::shared_ptr<AssetManager> _assets;
    bool _shoot;
    bool _summoned;
    float _CA;
    int _CAcount;
    float _running;
    float _breaking;
    std::string _attacktype;
    float _turing;
    std::shared_ptr<cugl::scene2::ActionManager> _actionM;
    int n;
    std::string _act;
    float _acttime;
    b2Filter filter;


public:
    BullModel() : Entity(), _drawScale(1.0f), _health(100.0f), _healthCooldown(0.2f), _lastDamageTime(0), _isChasing(true), _direction(-1) {}

    virtual ~BullModel() { dispose(); }

    void dispose();

    bool init(const Vec2& pos, const Size& size, float scale);

    static std::shared_ptr<BullModel> alloc(const Vec2& pos, const Size& size, float scale) {
        std::shared_ptr<BullModel> result = std::make_shared<BullModel>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    void update(float dt) override;

    void takeDamage(float damage, int attackDirection, bool knockback);


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

    void setangrytime(float time) { _angrytime = time; }

    float getangrytime() { return _angrytime; }

    float getknockbacktime() { return _knockbackTime; }
    void setknockbacktime(float time){_knockbackTime=time;}

    bool getshake() { return _shake; }

    void setshake(bool shake) { _shake = shake; }
    void createAttack3(GameScene& scene);
    bool getshoot() { return _shoot; }
    void setshoot(bool shoot) { _shoot = shoot; }
    void setassets(std::shared_ptr<AssetManager> assets) { _assets = assets; }
    void Summon(GameScene& scene);
    void setsummoned(bool summoned) { _summoned = summoned; };
    bool getsummoned() { return _summoned; };
    void setsprintpreparetime(float time) { _sprintPrepareTime = time; }
    float getsprintpreparetime() { return _sprintPrepareTime; }
    void setCAcount(int time) { _CAcount = time; }
    void setCA(float time) { _CA = time; }
    int getCAcount() { return _CAcount; }
    float getCA() { return _CA; }
    void circleattack(GameScene& scene);
    std::string getattacktype() { return _attacktype; }
    float getturing() { return _turing; }
    void setturing(float time){_turing=time;}
    void setact(std::string act, float time){_acttime=time;
        _act=act;}
    float getacttime(){return _acttime;}
    std::string getact(){return _act;}
    void setbreaking(float breaking){_breaking=breaking;}
    float getbreaking(){return _breaking;}
    float getrunning(){return _running;}
};

#endif /* __BULL_MODEL_H__ */
