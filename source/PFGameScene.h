//
//  PFGameScene.h
//  PlatformDemo
//
//  This is the most important class in this demo.  This class manages the gameplay
//  for this demo.  It also handles collision detection. There is not much to do for
//  collisions; our WorldController class takes care of all of that for us.  This
//  controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt 
//  well to data driven design. This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together. However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden (2007).
//  This file has been refactored to support the physics changes in CUGL 2.5.
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/24
//
#ifndef __PF_GAME_SCENE_H__
#define __PF_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include "PFInput.h"
#include "PFAttack.h"
#include "PFDudeModel.h"
#include "PFRopeBridge.h"
#include "PFSpinner.h"
#include "PFDollarScene.h"
#include "Levels/Level1.h"
#include "Levels/Level2.h"
// #include "Levels/Level3.h"


/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    std::shared_ptr<PlatformInput> _input;
    
    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _worldnode;
    /** Reference to the debug root of the scene graph */
    std::shared_ptr<cugl::scene2::SceneNode> _debugnode;
    /** Reference to the win message label */
    std::shared_ptr<cugl::scene2::Label> _winnode;
    /** Reference to the lose message label */
    std::shared_ptr<cugl::scene2::Label> _losenode;
    /** Reference to the left joystick image */
    std::shared_ptr<cugl::scene2::PolygonNode> _leftnode;
    /** Reference to the right joystick image */
    std::shared_ptr<cugl::scene2::PolygonNode> _rightnode;

    std::shared_ptr<Scene2> _bgScene;
    std::shared_ptr<Scene2> _uiScene;

    std::string _feedbackMessages[3] = { "Bad", "Good", "Perfect" };




    /** Reference to the quick-time event scene node */
    std::shared_ptr<DollarScene> _dollarnode;

    /** The Box2D world */
    std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

    /** Whether or not time is being slowed */
    bool _slowed;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<cugl::physics2::BoxObstacle>    _goalDoor;

    std::shared_ptr<cugl::scene2::PolygonNode>    _background;
    /** Reference to the player avatar */
    std::shared_ptr<DudeModel>			  _avatar;

    std::vector<std::shared_ptr<EnemyModel>> _enemies;


    //Valid targets for cook-time
    std::vector<std::shared_ptr<EnemyModel>> _vulnerables;

    //Current target for cook-time
    std::shared_ptr<EnemyModel> _target;


    /** Reference to the spinning barrier */
    std::shared_ptr<Spinner>			  _spinner;
    /** Reference to the rope bridge */
    std::shared_ptr<RopeBridge>			  _ropebridge;

    //temp bad code
    std::vector<std::shared_ptr<Attack>>  _attacks;
    time_t start;

    /** Whether we have completed this "game" */
    bool _complete;
    /** Whether or not debug mode is active */
    bool _debug;
    /** Whether we have failed at this world (and need a reset) */
    bool _failed;
    /** Countdown active for winning or losing */
    int _countdown;

    //camera
    //std::shared_ptr<cugl::OrthographicCamera> _camera;
    cugl::Vec3 _cameraOffset = Vec3::ZERO;
    float _smoothTime = 0.25f;
    cugl::Vec3 _velocity = Vec3::ZERO;

      
    /** Mark set to handle more sophisticated collision callbacks */
    std::unordered_set<b2Fixture*> _sensorFixtures;

    std::shared_ptr<Levels> currentLevel;

    std::shared_ptr<BullModel>			  _Bull;

    std::shared_ptr<Level2> level2 = std::make_shared<Level2>();

    std::shared_ptr<Level1> level1 = std::make_shared<Level1>();

    /** Whether or not this scene initiated a transfer to the other gameplay mode scene*/
    bool _transitionScenes;
    float healthPercentage;
    std::shared_ptr<cugl::scene2::PolygonNode> _healthBarForeground;
    std::shared_ptr<cugl::scene2::PolygonNode> _healthBarBackground;

    std::shared_ptr<cugl::scene2::Label> _buffLabel;

    std::vector<std::tuple<std::shared_ptr<cugl::scene2::Label>, cugl::Timestamp>> _popups;

#pragma mark Internal Object Management
    /**
     * Lays out the game geography.
     *
     * Pay close attention to how we attach physics objects to a scene graph.
     * The simplest way is to make a subclass, like we do for the dude.  However,
     * for simple objects you can just use a callback function to lightly couple
     * them.  This is what we do with the crates.
     *
     * This method is really, really long.  In practice, you would replace this
     * with your serialization loader, which would process a level file.
     */
  //  void populate();
    
    /**
     * Adds the physics object to the physics world and loosely couples it to the scene graph
     *
     * There are two ways to link a physics object to a scene graph node on the
     * screen.  One way is to make a subclass of a physics object, like we did
     * with dude.  The other is to use callback functions to loosely couple
     * the two.  This function is an example of the latter.
     *
     * @param obj    The physics object to add
     * @param node   The scene graph node to attach it to
     * @param useObjPosition  Whether to update the node's position to be at the object's position
     */

    /**
     * Returns the active screen size of this scene.
     *
     * This method is for graceful handling of different aspect
     * ratios
     */
    cugl::Size computeActiveSize() const;

    std::string getGestureText(std::string gest, float sim);
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, 
              const cugl::Rect& rect, std::shared_ptr<PlatformInput> input);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, 
              const cugl::Rect& rect, const cugl::Vec2& gravity, std::shared_ptr<PlatformInput> input);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
    bool isDebug( ) const { return _debug; }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
    void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
	void setComplete(bool value);

	/**
	* Returns true if the level is failed.
	*
	* If true, the level will reset after a countdown
	*
	* @return true if the level is failed.
	*/
	bool isFailure() const { return _failed; }

	/**
	* Sets whether the level is failed.
	*
	* If true, the level will reset after a countdown
	*
	* @param value whether the level is failed.
	*/
	void setFailure(bool value);
    
#pragma mark -
#pragma mark Collision Handling
	/**
	* Processes the start of a collision
	*
	* This method is called when we first get a collision between two objects.  We use
	* this method to test if it is the "right" kind of collision.  In particular, we
	* use it to test if we make it to the win door.  We also us it to eliminate bullets.
	*
	* @param  contact  The two bodies that collided
	*/
	void beginContact(b2Contact* contact);

	/**
	* Processes the end of a collision
	*
	* This method is called when we no longer have a collision between two objects.  
	* We use this method allow the character to jump again.
	*
	* @param  contact  The two bodies that collided
	*/
	void endContact(b2Contact* contact);

#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to indicate the start of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the beginning of the core application loop,
     * which is concluded with a call to {@link #postUpdate}.
     *
     * This method should be used to process any events that happen early in
     * the application loop, such as user input or events created by the
     * {@link schedule} method. In particular, no new user input will be
     * recorded between the time this method is called and {@link #postUpdate}
     * is invoked.
     *
     * Note that the time passed as a parameter is the time measured from the
     * start of the previous frame to the start of the current frame. It is
     * measured before any input or callbacks are processed. It agrees with
     * the value sent to {@link #postUpdate} this animation frame.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void preUpdate(float dt);
    
    /**
     * The method called to provide a deterministic application loop.
     *
     * This method provides an application loop that runs at a guaranteed fixed
     * timestep. This method is (logically) invoked every {@link getFixedStep}
     * microseconds. By that we mean if the method {@link draw} is called at
     * time T, then this method is guaranteed to have been called exactly
     * floor(T/s) times this session, where s is the fixed time step.
     *
     * This method is always invoked in-between a call to {@link #preUpdate}
     * and {@link #postUpdate}. However, to guarantee determinism, it is
     * possible that this method is called multiple times between those two
     * calls. Depending on the value of {@link #getFixedStep}, it can also
     * (periodically) be called zero times, particularly if {@link #getFPS}
     * is much faster.
     *
     * As such, this method should only be used for portions of the application
     * that must be deterministic, such as the physics simulation. It should
     * not be used to process user input (as no user input is recorded between
     * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
     *
     * The time passed to this method is NOT the same as the one passed to
     * {@link #preUpdate}. It will always be exactly the same value.
     *
     * @param step  The number of fixed seconds for this step
     */
    void fixedUpdate(float step);

    /**
     * The method called to indicate the end of a deterministic loop.
     *
     * This method is used instead of {@link #update} if {@link #setDeterministic}
     * is set to true. It marks the end of the core application loop, which was
     * begun with a call to {@link #preUpdate}.
     *
     * This method is the final portion of the update loop called before any
     * drawing occurs. As such, it should be used to implement any final
     * animation in response to the simulation provided by {@link #fixedUpdate}.
     * In particular, it should be used to interpolate any visual differences
     * between the the simulation timestep and the FPS.
     *
     * This method should not be used to process user input, as no new input
     * will have been recorded since {@link #preUpdate} was called.
     *
     * Note that the time passed as a parameter is the time measured from the
     * last call to {@link #fixedUpdate}. That is because this method is used
     * to interpolate object position for animation.
     *
     * @param remain    The amount of time (in seconds) last fixedUpdate
     */
    void postUpdate(float remain);


    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

    /**
    * Adds a new bullet to the world and sends it in the right direction.
    */
    void createAttack();

    /**
    * Removes the input Bullet from the world.
    *
    * @param  bullet   the bullet to remove
    */
    template<typename T>
    void removeAttack(T* attack);

    void removeEnemy(EnemyModel* enemy);

    std::shared_ptr<AssetManager> getAssets() const { return _assets; }

    float getScale() const { return _scale; }

    std::shared_ptr<cugl::scene2::PolygonNode> getBackground() const { return _background; }

    void addObstacle(const std::shared_ptr<cugl::physics2::Obstacle>& obj,
        const std::shared_ptr<cugl::scene2::SceneNode>& node,
        bool useObjPosition = true);

    std::shared_ptr<cugl::physics2::BoxObstacle> getGoalDoor() const { return _goalDoor; }

    std::shared_ptr<DudeModel> getAvatar() const { return _avatar; }

    std::vector<std::shared_ptr<EnemyModel>> getEnemies() const { return _enemies; }

    void loadLevel(std::shared_ptr<Levels> level) {
        level->populate(*this);
        currentLevel = level;
    }

    void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets; }
    void setScale(float scale) { _scale = scale; }
    void setBackground(const std::shared_ptr<cugl::scene2::PolygonNode>& background) { _background = background; }
    void setAvatar(const std::shared_ptr<DudeModel>& avatar) { _avatar = avatar; }
    void setEnemies(const std::vector<std::shared_ptr<EnemyModel>>& enemies) { _enemies = enemies; }
    void setGoalDoor(const std::shared_ptr<cugl::physics2::BoxObstacle>& goalDoor) { _goalDoor = goalDoor; }

    void unzoomCamera();
    void zoomCamera(float scale);

    std::shared_ptr<BullModel> getBull() const { return _Bull; }
    void setBull(const std::shared_ptr<BullModel>& bull) { _Bull = bull; }

    void transition(bool t);

    void renderBG(std::shared_ptr<cugl::SpriteBatch> batch);

    void renderUI(std::shared_ptr<cugl::SpriteBatch> batch);

    bool transitionedAway() { return _transitionScenes; };

    //creates a popup message that dissapates. Position is in word coords, not physics.
    void GameScene::popup(std::string s, Vec2 pos);
  };

#endif /* __PF_GAME_SCENE_H__ */