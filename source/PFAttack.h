//
//  PFAttack.h
//  PlatformDemo
//
//  This class is a simple extension of WheelObstacle in order to simplify the process of adding and removing Attacks
//  from the game world, as well as for drawing the sprite of the attack.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#ifndef __PF_ATTACK_MODEL_H__
#define __PF_ATTACK_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/scene2/graph/CUWireNode.h>
#define ATTACK_LIFETIME  5

#pragma mark -
#pragma mark Attack Model
class Attack : public cugl::physics2::BoxObstacle {
private:
	/** This macro disables the copy constructor (not allowed on physics objects) */
	CU_DISALLOW_COPY_AND_ASSIGN(Attack);

protected:
	/** The scene graph node for the Attack. */
	std::shared_ptr<cugl::scene2::SceneNode> _node;
	/** The scale between the physics world and the screen (MUST BE UNIFORM) */
	float _drawScale;

    //the time at which this object was init
    float _lifetime;

    //whether or not attack is marked for deletion
    bool _killme;

    bool _go;

    bool _faceright;

    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;

public:
#pragma mark Constructors
    /**
     * Creates a degenerate Attack object.
     *
     * This constructor does not initialize any of the Attack values beyond
     * the defaults.  To use a Attack, you must call init().
     */
    Attack() : BoxObstacle() {
        setSensor(true);
        _lifetime = ATTACK_LIFETIME;
        _killme = false;
    }
    
    /**
     * Destroys this Attack, releasing all resources.
     */
    virtual ~Attack(void) { dispose(); }
    
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
    static std::shared_ptr<Attack> alloc(cugl::Vec2 pos, const cugl::Size& size) {
        std::shared_ptr<Attack> result = std::make_shared<Attack>();
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
    void setGo(bool go) { _go = go; }

};

#endif /* __PF_ATTACK_MODEL_H__ */
