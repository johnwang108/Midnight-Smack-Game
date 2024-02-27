#ifndef __ENEMY_MODEL_H__
#define __ENEMY_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/physics2/CUBoxObstacle.h>
#include <cugl/physics2/CUCapsuleObstacle.h>

#define ENEMY_SENSOR_NAME     "enemysensor"
#define SHRIMP_TEXTURE    "shrimp"
/**
 * Enum for enemy types.
 * Add additional enemy types as needed.
 */
enum class EnemyType {
    shrimp, 
    FLYING, 
    CHARGER 
};

class EnemyModel : public cugl::physics2::CapsuleObstacle {
private:
    /** This macro disables the copy constructor (not allowed on physics objects) */
    CU_DISALLOW_COPY_AND_ASSIGN(EnemyModel);

protected:
    /** The type of the enemy */
    EnemyType _type;
    /** The current horizontal movement direction of the enemy (-1 for left, 1 for right) */
    int _direction;
    /** Whether the enemy is currently on the ground */
    bool _isGrounded;
    /** The node for visual representation of the enemy */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The node for debugging the sensor */
    std::shared_ptr<cugl::scene2::WireNode> _sensorNode;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _drawScale;

    std::string _sensorName;

    b2Fixture* _sensorFixture;

    int _lastDirection;

    float _changeDirectionInterval; 
    float _nextChangeTime;

    bool _remove;


public:
    EnemyModel() : CapsuleObstacle(), _sensorName(ENEMY_SENSOR_NAME) { }
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
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, EnemyType type);

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

    /**
     * Sets the scene graph node representing this enemy.
     *
     * @param node The scene graph node representing this enemy.
     */
    void setSceneNode(const std::shared_ptr<cugl::scene2::SceneNode>& node);

    void setGrounded(bool value) { _isGrounded = value; };

    std::string* getSensorName() { return &_sensorName; };

    bool isGrounded() const { return _isGrounded; }

    int getDirection() const { return _direction; }

    const std::shared_ptr<cugl::scene2::SceneNode>& getSceneNode() const { return _node; }

    void markRemoved(bool value) { _remove = value; }

    bool isRemoved() const { return _remove; }


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

    void removeFromGame();

};

#endif /* __ENEMY_MODEL_H__ */