
#ifndef __PF_ENEMY_ATTACK_MODEL_H__
#define __PF_ENEMY_ATTACK_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>

#define EA_SENSOR_NAME     "easensor" // If the Bull requires a unique sensor
#define EA_CHASE_SPEED     3.0f         // Using the CHASE_SPEED for consistency
#define EA_DENSITY         2.0f         // Assuming the Bull is heavier than a regular enemy
#define EA_FORCE           1.0f         // Force specific to the Bull's movement, potentially stronger
#define EA_MAXSPEED        5.0f         // A reasonable max speed for the Bull, ensuring it's fast but manageable
#define EA_KNOCKBACK_FORCE 5.0f        // Increased knockback force due to the Bull's strength
#define EA_KNOCKBACK_FORCE_UP 15.0f      // Vertical knockback component

#define EA_VSHRINK    0.8f
#define EA_HSHRINK    0.7f
#define SENSOR_HEIGHT 0.1f


#pragma mark -
#pragma mark Attack Model
class EnemyAttack : public cugl::physics2::BoxObstacle {
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(EnemyAttack);

protected:
	/** The scene graph node for the Attack. */
	std::shared_ptr<cugl::scene2::SceneNode> _node;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;

    //the time at which this object was init
    float _lifetime;

    //whether or not attack is marked for deletion
    bool _killme;

    bool _faceright;

    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

    int _direction;

    bool _shoot;

    std::string _sensorName;

    b2Fixture* _sensorFixture;

public:
#pragma mark Constructors
    /**
     * Creates a degenerate Attack object.
     *
     * This constructor does not initialize any of the Attack values beyond
     * the defaults.  To use a Attack, you must call init().
     */
    EnemyAttack() : BoxObstacle() {
        setSensor(true);
    }
    
    /**
     * Destroys this Attack, releasing all resources.
     */
    virtual ~EnemyAttack(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this Attack
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a Attack may not be used until it is initialized again.
     */
    void dispose();
    
#pragma mark -
#pragma mark Static Constructors
    /**
     * Creates a new Attack with the given shape.
     *
     * The attack is scaled so that 1 pixel = 1 Box2d unit
     *
     * The scene graph is completely decoupled from the physics system.
     * The node does not have to be the same size as the physics body. We
     * only guarantee that the scene graph node is positioned correctly
     * according to the drawing scale.
     *
     * @param  pos      Initial position in world coordinates
     * @param  radius   The radius of the Attack obstacle.
     *
     * @return  A newly allocated Attack at the given position, with the given radius
     */
    static std::shared_ptr<EnemyAttack> alloc(cugl::Vec2 pos, const cugl::Size& size) {
        std::shared_ptr<EnemyAttack> result = std::make_shared<EnemyAttack>();
        //return (result->init(pos, radius) ? result : nullptr);
        return (result->init(pos, size) ? result : nullptr);
    }

    bool init(cugl::Vec2 pos, const cugl::Size& size);
#pragma mark -
#pragma mark Animation
    /**
     * Returns the scene graph node representing this Attack.
     *
     * By storing a reference to the scene graph node, the model can update
     * the node to be in sync with the physics info. It does this via the
     * {@link Obstacle#update(float)} method.
     *
     * @return the scene graph node representing this Attack.
     */
    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }
    
    /**
     * Sets the scene graph node representing this Attack.
     *
     * @param node  The scene graph node representing this Attack, which has 
     *              been added to the world node already.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node) {
        _node = node;
    }
    
    /**
     * Sets the ratio of the Attack sprite to the physics body
     *
     * The Attack needs this value to convert correctly between the physics
     * coordinates and the drawing screen coordinates.  Otherwise it will
     * interpret one Box2D unit as one pixel.
     *
     * All physics scaling must be uniform.  Rotation does weird things when
     * attempting to scale physics by a non-uniform factor.
     *
     * @param scale The ratio of the Attack sprite to the physics body
     */
    void setDrawScale(float scale) {
        _drawScale = scale;
    }

#pragma mark -
#pragma mark Physics Methods
	/**
	* Updates the object's physics state (NOT GAME LOGIC).
	*
	* We use this method to reset cooldowns.
	*
	* @param delta Number of seconds since last animation frame
	*/
	void update(float dt) override;

    bool killMe() { return _killme; };

    void setFaceRight(bool faceRight) { _faceright = faceRight; }

    void releaseFixtures();
    void createFixtures();
    std::string* getSensorName() { return &_sensorName; };
};

#endif /* __PF_ENEMY_ATTACK_MODEL_H__ */
