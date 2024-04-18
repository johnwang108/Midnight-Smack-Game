//
//  PFInput.h
//  PlatformDemo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author:  Walker White and Anthony Perello
//  Version: 2/9/21
//
#ifndef __PF_INPUT_H__
#define __PF_INPUT_H__
#include <cugl/cugl.h>
#include <unordered_set>

/**
 * This class represents player input in the Platform demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * In the case of touch devices, this implements the platformer controls
 * found in the popular game Oddmar.  The left side is a floating joystick
 * for left-right controls.  The right side is swipe up to jump and double
 * tap to shoot.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this
 * controller as a field without using pointers. We simply add the class to the
 * header file of its owner, and delay initialization (via the method init())
 * until later. This is one of the main reasons we like to avoid initialization
 * in the constructor.
 */
class PlatformInput {
private:
    /** Whether or not this input is active */
    bool _active;
    // KEYBOARD EMULATION
    /** Whether the jump key is down */
    bool  _keyJump;
    /** Whether the fire key is down */
    bool  _keyFire;
    /** Whether the reset key is down */
    bool  _keyReset;
    /** Whether the debug key is down */
    bool  _keyDebug;
    /** Whether the exit key is down */
    bool  _keyExit;
    /** Whether the left arrow key is down */
    bool  _keyLeft;
    /** Whether the right arrow key is down */
    bool  _keyRight;
    /** Whether the slow key is down */
    bool _keySlow;

    bool _keyTransition;

    //for Leon
    bool  _keyAnimate;
    bool  _keyBackground;
    bool  _keyMusic;


    //is the current path ready for recog and rendering
    bool _gestureCompleted;
  
    bool  _keyDown;
    bool  _keyUp;
    bool _dashKey;
   
    std::shared_ptr<cugl::GameController> _gameCont;



    
  
protected:
    // INPUT RESULTS
    /** Whether the reset action was chosen. */
    bool _resetPressed;
    /** Whether the debug toggle was chosen. */
    bool _debugPressed;
    /** Whether the exit action was chosen. */
    bool _exitPressed;
    /** Whether the fire action was chosen. */
    bool _firePressed;
    /** Whether the jump action was chosen. */
    bool _jumpPressed;
    /** Whether the slow action was chosen. */
    bool _slowPressed;
    /** How much did we move horizontally? */
    float _horizontal;
    float _vertical;
    bool _dashPressed;

    bool _transitionPressed;

    bool _animatePressed;
    bool _backgroundPressed;
    bool _musicPressed;

    bool _zoomIn;
    bool _zoomOut;

    //axes positions for joystick
    float _xAxis;
    float _yAxis;

    //mouse pos 
    cugl::Vec2 _touchPos;


#pragma mark Internal Touch Management   
	// The screen is divided into four zones: Left, Bottom, Right and Main/
	// These are all shown in the diagram below.
	//
	//   |---------------|
	//   |   |       |   |
	//   | L |   M   | R |
	//   |   |       |   |
	//   -----------------
	//
	// The meaning of any touch depends on the zone it begins in.

	/** Information representing a single "touch" (possibly multi-finger) */
	struct TouchInstance {
		/** The anchor touch position (on start) */
		cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
		/** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
	};


	/** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
	cugl::Rect _sbounds;
	
    
    /** Whether or not we have processed a jump for this swipe yet */
    bool _hasJumped;
    /** The timestamp for a double tap on the right */
    cugl::Timestamp _rtime;
	/** The timestamp for a double tap in the middle */
	cugl::Timestamp _mtime;

    cugl::Path2 _touchPath;
    cugl::Vec2 _swipeDelta;

  
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);

    
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;


    
    /**
     * Returns a nonzero value if this is a quick left or right swipe
     *
     * The function returns -1 if it is left swipe and 1 if it is a right swipe.
     *
     * @param  start    the start position of the candidate swipe
     * @param  stop     the end position of the candidate swipe
     * @param  current  the current timestamp of the gesture
     *
     * @return a nonzero value if this is a quick left or right swipe
     */
	int processSwipe(const cugl::Vec2 start, const cugl::Vec2 stop, cugl::Timestamp current);
  
public:
#pragma mark -
#pragma mark Constructors


    float id;



    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    PlatformInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~PlatformInput() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Initializes the input control for the given bounds
     *
     * The bounds are the bounds of the scene graph.  This is necessary because
     * the bounds of the scene graph do not match the bounds of the display.
     * This allows the input device to do the proper conversion for us.
     *
     * @param bounds    the scene graph bounds
     *
     * @return true if the controller was initialized successfully
     */
    bool init(const cugl::Rect bounds);
    
#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll the
     * keyboad and accelerometer.
     * 
     * This method also gathers the delta difference in the touches. Depending on 
     * the OS, we may see multiple updates of the same touch in a single animation
     * frame, so we need to accumulate all of the data together.
     */
    void  update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getHorizontal() const { return _horizontal; }
    /**
     * Returns the amount of sideways movement.
     *
     * -1 = left, 1 = right, 0 = still
     *
     * @return the amount of sideways movement.
     */
    float getVertical() const { return _vertical; }

    /**
     * Returns if the jump button was pressed.
     *
     * @return if the jump button was pressed.
     */
	float didJump() const { return _jumpPressed; }

    /**
     * Returns true if the fire button was pressed.
     *
     * @return true if the fire button was pressed.
     */
	bool didFire() const { return _firePressed; }

    /**
     * Returns true if the reset button was pressed.
     *
     * @return true if the reset button was pressed.
     */
	bool didReset() const { return _resetPressed; }

    /**
     * Returns true if the player wants to go toggle the debug mode.
     *
     * @return true if the player wants to go toggle the debug mode.
     */
	bool didDebug() const { return _debugPressed; }

	/**
	 * Returns true if the exit button was pressed.
	 *
	 * @return true if the exit button was pressed.
	 */
	bool didExit() const { return _exitPressed; }
    bool didSlow() const { 
            return _slowPressed; 
    }


    bool didDash() const { return _dashPressed; }

    bool didTransition() const { return _transitionPressed; }

    bool didAnimate() const { return _animatePressed; }

    bool didMusic() const { return _musicPressed; }
    bool didBackground() const { return _backgroundPressed; }

    cugl::Vec2 getTouchPos() { return _touchPos; }

#pragma mark -
#pragma mark Touch and Mouse Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus	Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);

    void swipeBeganCB(const cugl::PanEvent& event, bool focus);


    void swipeEndedCB(const cugl::PanEvent& event, bool focus);


  

  

    cugl::Path2 getTouchPath();

    cugl::Path2 popTouchPath();

    /**
     * Callback for the beginning of a mouse press event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void mousePressCB(const cugl::MouseEvent& event, bool focus);

    /**
     * Callback for the beginning of a mouse move event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void mouseDragCB(const cugl::MouseEvent& event, bool focus);

    /**
     * Callback for the beginning of a mouse release event
     *
     * @param event The associated event
     * @param focus	Whether the listener currently has focus
     */
    void mouseReleaseCB(const cugl::MouseEvent& event, bool focus);

    bool isGestureCompleted() { return _gestureCompleted; };


    cugl::Vec2 getSwipeDelta() { return _swipeDelta; }

    void gestureStartCB(cugl::Vec2 pos, bool focus);
    void gestureMoveCB(cugl::Vec2 pos, bool focus);
    void gestureEndCB(cugl::Vec2 pos, bool focus);

};

#endif /* __PF_INPUT_H__ */
