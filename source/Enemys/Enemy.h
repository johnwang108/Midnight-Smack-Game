#ifndef __ENEMY_MODEL_H__
#define __ENEMY_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include "../Attack.h"
#include "../PFDudeModel.h"
#include "../EntitySpriteNode.h"
#include "../Entity.h"

#define ENEMY_SENSOR_NAME     "enemysensor"
#define SHRIMP_TEXTURE    "shrimpBoss"
#define EGG_TEXTURE    "eggEnemy"
#define RICE_TEXTURE    "riceEnemy"
#define BEEF_TEXTURE    "beef"
#define CARROT_TEXTURE    "carrot"

#define ENEMY_FORCE      3.75f
#define ENEMY_DAMPING    5.0f
#define ENEMY_MAXSPEED   10.0f
#define ENEMY_JUMP       2.5f
#define ENEMY_VSHRINK    0.8f
#define ENEMY_HSHRINK    0.7f
#define ENEMY_DENSITY    1.0f

#define SENSOR_HEIGHT 0.1f

#define CHASE_THRESHOLD 10.0f  
#define CHASE_SPEED 0.03f


#define ENEMY_ATTACK_CHANCE 0.001f

#define SIGNUM(x)  ((x > 0) - (x < 0))

class GameScene;

/**
 * Enum for enemy types.
 * Add additional enemy types as needed.
 * Shrimp: rolling enemy, fast and can jump but telegraphed in building up speed
 * Rice: default aggressive walking enemy, can jump on top of each other. Probably needs to be a box because of this
 * Beef: mole enemy, can jump and attack. Individually dangerous
 * Carrots: fast enemy, low detection range
 * Egg: tall enemy. dangerous at range, easier to kill up close
 */
enum class EnemyType {
    shrimp, 
    rice, 
    rice_soldier,
    egg,
    carrot,
    beef
};

class EnemyModel : public Entity {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(EnemyModel);

protected:
    /** The type of the enemy */
    EnemyType _type;
    /** The current horizontal movement direction of the enemy (-1 for left, 1 for right) */
    int _direction;
    /** Whether the enemy is currently on the ground */
    //bool _isGrounded;
    /** Whether the enemy is aggroed*/
    bool _isChasing;

    bool _isTangible;

    /** Enemy state*/
    std::string _state;

    /** The node for debugging the sensor */
 //   std::shared_ptr<cugl::scene2::WireNode> _sensorNode;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    //float _drawScale;

    std::string _sensorName;

    b2Fixture* _sensorFixture;

    int _lastDirection;

    float _changeDirectionInterval; 
    float _nextChangeTime;

    //float _health;

    //float _healthCooldown;

    float _knockbackTime;

    bool _attacktime;
    float _preparetime;
    bool _shooted;

    //true if quick time can be initiated
    bool _vulnerable;

    //placeholder, name of the gesture to input for this enemy
    std::vector<std::string> _gestureSeq1;

    std::vector<std::string> _gestureSeq2;

    float _behaviorCounter;

    //distance to player = player position - enemy position
    cugl::Vec2 _distanceToPlayer;

    float _killMeCountdown;

    ///**used for soldier rice */
    //cugl::Vec2 _targetPosition;
    //float _closeEnough;

    /**Limits on movement for egg and beef*/
    cugl::Spline2 _limit;

    bool _killMe;


public:
    EnemyModel() : Entity(), _sensorName(ENEMY_SENSOR_NAME) { }

    /**
     * Initializes a new enemy at the given position with the specified size and type.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the enemy in world units
     * @param scale The drawing scale (world to screen)
     * @param type  The type of the enemy
     *
     * @return true if the enemy is initialized properly, false otherwise.
     */

    /** default constructor, sets both gesture sequences to the default for that enemy type*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2);

    /**
     * Creates and returns a new enemy at the given position with the specified size and type.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the enemy in world units
     * @param scale The drawing scale (world to screen)
     * @param type  The type of the enemy
     *
     * @return A newly allocated EnemyModel at the given position with the given size, scale, and type
     */
    static std::shared_ptr<EnemyModel> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type);



    /**Allocs with animations defined from json. Don't use.
    * 
    * */
   // virtual  std::shared_ptr<EnemyModel> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets) { return nullptr; };
        //std::shared_ptr<EnemyModel> result = std::make_shared<EnemyModel>();
        //bool res = result->init(pos, size, scale);

        //if (res) {
        //    CULog(typeToStr(type).c_str());
        //    result->loadAnimationsFromConstant(typeToStr(type), _assets);
        //}

        //return res ? result : nullptr;

    /**
     * Sets the scene graph node representing this enemy.
     *
     * @param node The scene graph node representing this enemy.
     */

    void setGrounded(bool value) { 
        _body->SetLinearDamping(40 * value);
        _isGrounded = value; };

    std::string* getSensorName() { return &_sensorName; };

    bool isGrounded() const { return _isGrounded; }

    int getDirection() const { return _direction; }

    void setDirection(int d) { _direction = d; }

    void setIsChasing(bool isChasing);

    void updatePlayerDistance(cugl::Vec2 playerPosition);

    bool isChasing() const { return _isChasing; }

    void takeDamage(float damage, const int attackDirection);

    float getHealth() { return _health; }


    bool getattacktime() { return _attacktime; }
    void setattacktime(bool attacktime) { _attacktime = attacktime; }
    void setshooted(bool shooted) { _shooted = shooted; }


#pragma mark -
#pragma mark Physics Methods
    /**
     * Creates the physics Body(s) for this object, adding them to the world.
     *
     * This method overrides the base method to keep your ship from spinning.
     *
     * @param world Box2D world to store body
     *
     * @return true if object allocation succeeded
     */
    void createFixtures() override;

    /**
     * Release the fixtures for this body, reseting the shape
     *
     * This is the primary method to override for custom physics objects.
     */
    void releaseFixtures() override;

    /**
     * Updates the object's physics state (NOT GAME LOGIC).
     *
     * We use this method to reset cooldowns.
     *
     * @param delta Number of seconds since last animation frame
     */
    virtual void update(float dt) override;

    virtual void fixedUpdate(float step);

    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce(float h, float v);

    /**
  * Destroys this DudeModel, releasing all resources.
  */
    virtual ~EnemyModel(void) { dispose(); }

    /**
     * Disposes all resources and assets of this DudeModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a DudeModel may not be used until it is initialized again.
     */
    void dispose();

    bool didAttack();

    virtual std::tuple<std::shared_ptr<Attack>, std::shared_ptr<cugl::scene2::PolygonNode>> createAttack(std::shared_ptr<cugl::AssetManager> _assets, float scale) { 
        return std::tuple< std::shared_ptr<Attack>, std::shared_ptr<cugl::scene2::PolygonNode>>(nullptr, nullptr);
    };


    void setVulnerable(bool vulnerable) { _vulnerable = vulnerable; }

    bool isVulnerable() {return _vulnerable; }

    std::vector<std::string> getGestureSeq1() { return _gestureSeq1; }
    std::vector<std::string> getGestureSeq2() { return _gestureSeq2; }

    void setGestureSeq1(std::vector<std::string> gestures) { _gestureSeq1 = gestures; };
    void setGestureSeq2(std::vector<std::string> gestures) { _gestureSeq2 = gestures; };

    EnemyType getType() { return _type; }

    virtual b2Vec2 handleMovement(b2Vec2 velocity);

    virtual void setState(std::string state);

    virtual std::string getNextState(std::string state) { return ""; };

    /**Sets the predefined path limits, still wip */
    void setLimit(cugl::Spline2 limit) { _limit = limit; }

    std::string getState() { return _state; }

    void jump(Vec2 dir);

    //void syncStateTimes();

    bool isTangible() { return _isTangible; }

    void setActiveAction(std::string action) {
        Entity::setActiveAction(action);
    };

    virtual void markForDeletion() {
        _killMe = true;
        _isTangible = false;
    }

    bool shouldDelete() {
		return _killMe && _killMeCountdown < 0;
	}

    bool isDying() { return _killMe; }

    static std::string typeToStr(EnemyType type) {
        switch (type) {
        case EnemyType::shrimp:
            return "shrimp";
        case EnemyType::rice:
            return "rice";
        case EnemyType::rice_soldier:
            return "rice_soldier";
        case EnemyType::egg:
            return "egg";
        case EnemyType::carrot:
            return "carrot";
        case EnemyType::beef:
            return "beef";
        default:
            return "";
        }
    };

    /**
    Gives the default sequence of gestures for each enemy type. 
    */
    static std::vector<std::string> defaultSeq(EnemyType type) {
        switch (type) {
        case EnemyType::shrimp:
            return { "pigtail", "v", "circle" };
        case EnemyType::rice:
            return { "circle", "v", "pigtail" };
        case EnemyType::rice_soldier:
            return { "circle", "v", "pigtail" };
        case EnemyType::egg:
            return { "v", "v", "v", };
        case EnemyType::carrot:
            return { "v", "v", "v" };
        case EnemyType::beef:
            return { "pigtail", "circle", "circle" };
        default:
            return {};
        }
    };

    static std::vector<std::string> defaultSeqAlt(EnemyType type) {
        /*switch (type) {
        case EnemyType::shrimp:
            return { "pigtail", "v", "circle" };
        case EnemyType::rice:
            return { "pigtail", "v", "pigtail" };
        case EnemyType::rice_soldier:
            return { "pigtail", "v", "pigtail" };
        case EnemyType::egg:
            return { "v", "v", "v", };
        case EnemyType::carrot:
            return { "v", "circle", "pigtail" };
        case EnemyType::beef:
            return { "pigtail", "v", "pigtail" };
        default:
            return {};
        }*/
        return defaultSeq(type);
    };

    //Dict for enemy type to buff 
    static buff typeToBuff(EnemyType type) {
        switch (type) {
        case EnemyType::shrimp:
            return buff::attack;
        case EnemyType::rice:
            return buff::defense;
        case EnemyType::rice_soldier:
            return buff::defense;
        case EnemyType::egg:
            return buff::health;
        case EnemyType::carrot:
            return buff::jump;
        case EnemyType::beef:
            return buff::speed;
        }
        return buff::none;
    };

    /**Dict for enemy type to aggro range. */
    static float typeToAggroRange(EnemyType type) {
		switch (type) {
		case EnemyType::shrimp:
			return 20.0f;
		case EnemyType::rice:
			return 20.0f;
		case EnemyType::rice_soldier:
			return 20.0f;
		case EnemyType::egg:
			return 20.0f;
		case EnemyType::carrot:
			return 20.0f;
		case EnemyType::beef:
			return 20.0f;
		}
		return 0.0f;
	};

};
#endif /* __ENEMY_MODEL_H__ */
