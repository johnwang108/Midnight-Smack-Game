//
//  PFDudeModel.h
//  PlatformDemo
//
//  This encapsulates all of the information for the character avatar.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#ifndef __PF_DUDE_MODEL_H__
#define __PF_DUDE_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#include "EntitySpriteNode.h"
#include "Attack.h"

#pragma mark -
#pragma mark Drawing Constants
/** The texture for the character avatar */
#define DUDE_TEXTURE    "dude"
/** Identifier to allow us to track the sensor in ContactListener */
#define SENSOR_NAME     "dudesensor"

#define BODY_SENSOR_NAME "dudebodysensor"


#pragma mark -
#pragma mark Physics Constants
/** The factor to multiply by the input */
#define DUDE_FORCE      sqrt(2 * (9.8) * getHeight() * 30 ) * getMass()
/** The amount to slow the character down */
#define DUDE_DAMPING    10.0f
/** The maximum character speed */
#define DUDE_MANUEL_MAXSPEED   5.0f

#define MAX_METER 100.0f

#define METER_COST 60.0f


#pragma mark -
#pragma mark Dude Model
/**
* Player avatar for the plaform game.
*
* Note that this class uses a capsule shape, not a rectangular shape.  In our
* experience, using a rectangular shape for a character will regularly snag
* on a platform.  The round shapes on the end caps lead to smoother movement.
*/

enum class buff{
    attack,
    health,
    jump,
    defense,
    speed,
    none
};

enum class modifier {
    duration,
    effect,
    none
};

#define DEFAULT_BUFF 1.0f

#define BASE_ATTACK_BUFF 1.5f
#define BASE_HEALTH_BUFF 5.0f
#define BASE_JUMP_BUFF 1.5f
//defense <1 because damage multiplied by defense
#define BASE_DEFENSE_BUFF 0.5f
#define BASE_SPEED_BUFF 2.0f

#define SUPER_ATTACK_BUFF 3.0f
#define SUPER_HEALTH_BUFF 5.0f
#define SUPER_JUMP_BUFF 3.0f
#define SUPER_DEFENSE_BUFF 0.0f
#define SUPER_SPEED_BUFF 5.0f

#define BASE_DURATION 10.0f

/** The keys for the attack texture in asset manager*/
#define ATTACK_TEXTURE_R  "attack_r"
#define ATTACK_TEXTURE_L  "attack_l"
/**Scalar for height of a box attack, hacky*/
#define ATTACK_H        0.5f

#define ATTACK_OFFSET_X 0.5f
#define ATTACK_OFFSET_Y 0.5f

class DudeModel : public cugl::physics2::CapsuleObstacle {
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(DudeModel);

protected:
	/** The current horizontal movement of the character */
	float _movement;
	/** Which direction is the character facing */
	bool _faceRight;
	/** How long until we can jump again */
	int  _jumpCooldown;
	/** Whether we are actively jumping */
	bool _isJumping;
	/** How long until we can shoot again */
	int  _shootCooldown;
	/** Whether our feet are on the ground */
	bool _isGrounded;
	/** Whether we are actively shooting */
	bool _isShooting;
	/** Ground sensor to represent our feet */
	b2Fixture*  _sensorFixture;
	/** Reference to the sensor name (since a constant cannot have a pointer) */
	std::string _sensorName;

    b2Fixture* _bodySensorFixture;

    std::string _bodySensorName;
	/** The node for debugging the sensor */
	std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

    std::shared_ptr<cugl::scene2::WireNode> _bodySensorNode;

	/** The scene graph node for the Dude. */
	std::shared_ptr<EntitySpriteNode> _node;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;

    bool _dash;
    int _dashNum;
    float _dashCooldown;
    bool _contactingWall;

    float _health;

    float _healthCooldown;
    float _knockbackTime;
    float _lastDamageTime;

    float healthPercentage;
    std::shared_ptr<cugl::scene2::PolygonNode> _healthBarForeground;

    float _attack;

    //attack damage buff
    float _attackBuff;
    //health buff
    float _healthBuff;
    //jump magnitude buff
    float _jumpBuff;
    //dash magnitude buff
    float _defenseBuff;
    //max speed buff
    float _speedBuff;


    //duration of buff, 0 if one-time buff
    float _duration;

    bool _hasSuper;

    float _numberOfTouchingEnemies;


    std::unordered_map<std::string, std::shared_ptr<cugl::scene2::Animate>> _actions;

    //unordered map of strings -> sprite sheets for the corresponding action
    std::unordered_map<std::string, std::shared_ptr<cugl::Texture>> _sheets;

    //info about each action's sheet: rows, cols, size, duration
    std::unordered_map<std::string, std::tuple<int,int,int,float,bool>> _info;

    //the last action that was animated
    std::string _activeAction;

    float _meter;

    const float _maxMeter = MAX_METER;

    float _healthUpgrade;
    float _dashUpgrade;
    float _meterGainUpgrade;
    float _hitStunUpgrade;

	/**
	* Redraws the outline of the physics fixtures to the debug node
	*
	* The debug node is use to outline the fixtures attached to this object.
	* This is very useful when the fixtures have a very different shape than
	* the texture (e.g. a circular shape attached to a square texture).
	*/
	virtual void resetDebug() override;

public:
    
#pragma mark Hidden Constructors
    /**
     * Creates a degenerate Dude object.
     *
     * This constructor does not initialize any of the dude values beyond
     * the defaults.  To use a DudeModel, you must call init().
     */
    DudeModel() : CapsuleObstacle(), _sensorName(SENSOR_NAME), _bodySensorName(BODY_SENSOR_NAME) { }
    
    /**
     * Destroys this DudeModel, releasing all resources.
     */
    virtual ~DudeModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this DudeModel
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a DudeModel may not be used until it is initialized again.
     */
    void dispose();
    
    /**
     * Initializes a new dude at the origin.
     *
     * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init() override { return init(cugl::Vec2::ZERO, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude is unit square scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos) override { return init(pos, cugl::Size(1,1), 1.0f); }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2 pos, const cugl::Size size) override {
        return init(pos, size, 1.0f);
    }
    
    /**
     * Initializes a new dude at the given position.
     *
     * The dude is sized according to the given drawing scale.
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
     * @param scale The drawing scale (world to screen)
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);


    
#pragma mark -
#pragma mark Static Constructors
	/**
	 * Creates a new dude at the origin.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @return  A newly allocated DudeModel at the origin
	 */
	static std::shared_ptr<DudeModel> alloc() {
		std::shared_ptr<DudeModel> result = std::make_shared<DudeModel>();
		return (result->init() ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
	 *
	 * The dude is a unit square scaled so that 1 pixel = 1 Box2d unit
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
     * @param pos   Initial position in world coordinates
	 *
	 * @return  A newly allocated DudeModel at the given position
	 */
	static std::shared_ptr<DudeModel> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<DudeModel> result = std::make_shared<DudeModel>();
		return (result->init(pos) ? result : nullptr);
	}

    /**
	 * Creates a new dude at the given position.
	 *
     * The dude has the given size, scaled so that 1 pixel = 1 Box2d unit
	 *
 	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
	 *
	 * @return  A newly allocated DudeModel at the given position with the given scale
	 */
	static std::shared_ptr<DudeModel> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<DudeModel> result = std::make_shared<DudeModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

	/**
	 * Creates a new dude at the given position.
	 *
	 * The dude is sized according to the given drawing scale.
	 *
	 * The scene graph is completely decoupled from the physics system.
	 * The node does not have to be the same size as the physics body. We
	 * only guarantee that the scene graph node is positioned correctly
	 * according to the drawing scale.
	 *
	 * @param pos   Initial position in world coordinates
     * @param size  The size of the dude in world units
	 * @param scale The drawing scale (world to screen)
	 *
	 * @return  A newly allocated DudeModel at the given position with the given scale
	 */
	static std::shared_ptr<DudeModel> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
		std::shared_ptr<DudeModel> result = std::make_shared<DudeModel>();
		return (result->init(pos, size, scale) ? result : nullptr);
	}
    

#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this DudeModel.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this DudeModel.
     */
	const std::shared_ptr<cugl::scene2::SceneNode> getSceneNode() { return _node; }

    /**
     * Sets the scene graph node representing this DudeModel.
     *
     * Note that this method also handles creating the nodes for the body parts
     * of this DudeModel. Since the obstacles are decoupled from the scene graph,
     * initialization (which creates the obstacles) occurs prior to the call to
     * this method. Therefore, to be drawn to the screen, the nodes of the attached
     * bodies must be added here.
     *
     * The bubbler also uses the world node when adding bubbles to the scene, so
     * the input node must be added to the world BEFORE this method is called.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @param node  The scene graph node representing this DudeModel, which has been added to the world node already.
     */
	void setSceneNode(const std::shared_ptr<EntitySpriteNode> node) {
        _node = node;
        _node->setPosition(getPosition() * _drawScale);
    }

    void addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool isPassive = true);

    void animate(std::string action_name);

    void changeSheet(std::string action_name);

    std::shared_ptr<cugl::scene2::Animate> getAction(std::string action_name) { return _actions[action_name]; };

    void getInfo(std::string action_name) {};

    std::string getActiveAction() { return _activeAction; };

    
#pragma mark -
#pragma mark Attribute Properties
    /**
     * Returns left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @return left/right movement of this character.
     */
    float getMovement() const { return _movement; }
    
    /**
     * Sets left/right movement of this character.
     *
     * This is the result of input times dude force.
     *
     * @param value left/right movement of this character.
     */
    void setMovement(float value);

    /**
     * Returns true if the dude is actively firing.
     *
     * @return true if the dude is actively firing.
     */
    bool isShooting() const { return _isShooting && _shootCooldown <= 0; }

    /**
     * Sets whether the dude is actively firing.
     *
     * @param value whether the dude is actively firing.
     */
    void setShooting(bool value) { _isShooting = value; }


    bool canDash() const { return _dash && _dashCooldown <= 0 && _dashNum>0; }

    void setDash(bool value) { _dash = value; }

    int getDashNum() { return _dashNum; }
    void setDashNum(int val) { _dashNum = val; }
    void deltaDashNum(int val) { _dashNum += val; }

    bool contactingWall() { return _contactingWall; }
    void setContactingWall(bool val) { _contactingWall = val;  }
    
    /**
     * Returns true if the dude is actively jumping.
     *
     * @return true if the dude is actively jumping.
     */
    bool isJumping() const { return _isJumping && _jumpCooldown <= 0; }
    
    /**
     * Sets whether the dude is actively jumping.
     *
     * @param value whether the dude is actively jumping.
     */
    void setJumping(bool value) { _isJumping = value; }
    
    /**
     * Returns true if the dude is on the ground.
     *
     * @return true if the dude is on the ground.
     */
    bool isGrounded() const { return _isGrounded; }
    
    /**
     * Sets whether the dude is on the ground.
     *
     * @param value whether the dude is on the ground.
     */
    void setGrounded(bool value) { _isGrounded = value; }
    
    /**
     * Returns how much force to apply to get the dude moving
     *
     * Multiply this by the input to get the movement value.
     *
     * @return how much force to apply to get the dude moving
     */
    float getForce() const { return DUDE_FORCE; }
    
    /**
     * Returns ow hard the brakes are applied to get a dude to stop moving
     *
     * @return ow hard the brakes are applied to get a dude to stop moving
     */
    float getDamping() const { return DUDE_DAMPING; }
    
    /**
     * Returns the upper limit on dude left-right movement.
     *
     * This does NOT apply to vertical movement.
     *
     * @return the upper limit on dude left-right movement.
     */
    float getMaxSpeed() { return DUDE_MANUEL_MAXSPEED * getSpeedBuff(); }
    
    /**
     * Returns the name of the ground sensor
     *
     * This is used by ContactListener
     *
     * @return the name of the ground sensor
     */
    std::string* getSensorName() { return &_sensorName; }

    std::string* getBodySensorName() { return &_bodySensorName; }
    
    /**
     * Returns true if this character is facing right
     *
     * @return true if this character is facing right
     */
    bool isFacingRight() const { return _faceRight; }

    
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
    void update(float dt) override;
    
    /**
     * Applies the force to the body of this dude
     *
     * This method should be called after the force attribute is set.
     */
    void applyForce(float h, float v);

    void takeDamage(float damage, const int attackDirection);
	
    float getHealth() { return _health; }

    void sethealthbar(std::shared_ptr<cugl::AssetManager> asset);

    //Apply buff to Sue with proper modifier.
    void applyBuff(buff b, modifier m);

    /**This function gets the attack, and resets the attack buff if it is super*/
    float getAttack() { return _attack * getAttackBuff(); }

    float getDuration() { return _duration; };

    void resetBuff();

    bool hasSuper() { return _hasSuper; };

    void addTouching() { _numberOfTouchingEnemies += 1; };

    void removeTouching() { _numberOfTouchingEnemies -= 1; };

    void addMeter(float f) {_meter += f; if (_meter > _maxMeter) _meter = _maxMeter; };

    float getMeter() { return _meter; };

    void setMeter(float f) { _meter = std::min(_maxMeter, f); };

    //uses a variable amount of meter, returns true if possible, returns false and doesn't do anything if not.
    bool useMeter(float f = METER_COST);

    std::tuple<std::shared_ptr<Attack>, std::shared_ptr<cugl::scene2::PolygonNode>> createAttack(std::shared_ptr<cugl::AssetManager> _assets, float scale);

    float getAttackBuff() {
        if (_duration > 0) {
            return _attackBuff;
        }
        if (_hasSuper && (_attackBuff != DEFAULT_BUFF)) {
            _hasSuper = false;
            return _attackBuff;
        }
        return DEFAULT_BUFF;
    };

    float getDefenseBuff() {
        if (_duration > 0) {
            return _defenseBuff;
        }
        if (_hasSuper && (_defenseBuff != DEFAULT_BUFF)) {
            _hasSuper = false;
            return _defenseBuff;
        }
        return DEFAULT_BUFF;
    }

    float getJumpBuff() {
        if (_duration > 0) {
            return _jumpBuff;
        }
        if (_hasSuper && (_jumpBuff != DEFAULT_BUFF)) {
            _hasSuper = false;
            return _jumpBuff;
        }
        return DEFAULT_BUFF;
    }

    float getSpeedBuff() {
        if (_duration > 0) {
            return _speedBuff;
        }
        if (_hasSuper && (_speedBuff != DEFAULT_BUFF)) {
            _hasSuper = false;
            return _speedBuff;
        }
        return DEFAULT_BUFF;
    }

    //maybe not needed
    float getHealthBuff() { return 0.0f; };

    static char* getStrForBuff(buff enumVal)
    {
        switch (enumVal)
        {
        case buff::attack:
            return "attack";
        case buff::jump:
            return "jump";
        case buff::speed:
            return "speed";
        case buff::defense:
            return "defense";
        case buff::health:
            return "health";
        default:
            return "Not recognized..";
        }
    }

    static char* getStrForModifier(modifier enumVal)
    {
        switch (enumVal)
        {
        case modifier::duration:
            return "duration";
        case modifier::effect:
            return "effect";
        case modifier::none:
            return "none";
        default:
            return "Not recognized..";
        }
    }
};

#endif /* __PF_DUDE_MODEL_H__ */
