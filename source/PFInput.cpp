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
#include "PFInput.h"

using namespace cugl;

#pragma mark Input Constants

/** The key to use for reseting the game */
#define RESET_KEY KeyCode::R
/** The key for toggling the debug display */
#define DEBUG_KEY KeyCode::Q
/** The key for exitting the game */
#define EXIT_KEY  KeyCode::ESCAPE
/** The key for firing a bullet */
#define FIRE_KEY KeyCode::O
/** The key for jumping up */
#define JUMP_KEY KeyCode::SPACE
#define DASH_KEY KeyCode::LEFT_SHIFT

/** Slow key */
#define SLOW_KEY KeyCode::TAB

/** How close we need to be for a multi touch */
#define NEAR_TOUCH      100
/** The key for the event handlers */
#define GESTURE_LISTENER_KEY      1

/** The key for the controller event handlers */
#define CONTROLLER_LISTENER_KEY 2

#define MOUSE_LISTENER_KEY 3

/** The key for the swipe evevnt handlers */
#define SWIPE_LISTENER_KEY 4

/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  15
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    25
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    80
/** How far we must swipe up for a jump gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400


#define TRIGGER_DEADZONE 0

// The screen is divided into four zones: Left, Bottom, Right and Main/
// These are all shown in the diagram below.
//
//   |---------------|
//   |   |       |   |
//   | L |   M   | R |
//   |   |       |   |
//   |---------------|
//
// The meaning of any touch depends on the zone it begins in.

/** The portion of the screen used for the left zone */
#define LEFT_ZONE       0.35f
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.35f

#pragma mark -
#pragma mark Input Controller
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
PlatformInput::PlatformInput() :
_active(false),
_resetPressed(false),
_debugPressed(false),
_exitPressed(false),
_firePressed(false),
_jumpPressed(false),
_slowPressed(false),
_keyJump(false),
_keyFire(false),
_keyReset(false),
_keyDebug(false),
_keyExit(false),
_keyLeft(false),
_keyRight(false),
_keySlow(false),
_horizontal(0.0f),
_vertical(0.0f),
_dashKey(false),
_hasJumped(false) {
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void PlatformInput::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
        Input::deactivate<Mouse>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(GESTURE_LISTENER_KEY);
        touch->removeEndListener(GESTURE_LISTENER_KEY);
        touch->removeMotionListener(GESTURE_LISTENER_KEY);
            
        //_gameCont->removeAxisListener(CONTROLLER_LISTENER_KEY);
#endif
        _active = false;
    }
}

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
bool PlatformInput::init(const Rect bounds) {
    if (_active) {
        CULog("ALREADY INITED"); 
        return false;
    }
    CULog("HI I INITEDDDD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! :D");
    id = rand();
    CULog("ID: %f", id);
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    _touchPath = cugl::Path2();

    bool contSuccess = Input::activate<GameControllerInput>();
    if (contSuccess) {
        GameControllerInput* controller = Input::get<GameControllerInput>();
        std::vector<std::string> deviceUUIDs = controller->devices();
       
        if (!deviceUUIDs.empty()) {
            _gameCont = controller->open(deviceUUIDs.front());
            CULog("Controller Obtained, Name: %s", _gameCont->getName().c_str());

            _xAxis = 0;
            _yAxis = 0;

        }
        else {
            CULog("no uuids");
        }
    }
    else {
        CULog("cont failed");
    }

#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
    CULog("no touch screen sad");

    success = Input::activate<Mouse>();
    CULog("%s", success ? "Mouse Activated" : "Mouse Failed");
    Mouse* mouse = Input::get<Mouse>();
    mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
    mouse ->addPressListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        		this->mousePressCB(event, focus);
         });

    mouse -> addDragListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, const Vec2& previous, bool focus) {
        		this->mouseDragCB(event, focus);
                		});

    mouse->addReleaseListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseReleaseCB(event, focus);
        });
#else
    CULog("%d", CU_TOUCH_SCREEN);
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(GESTURE_LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(GESTURE_LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(GESTURE_LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });

#endif
    

    bool swipeSuccess = Input::activate<PanGesture>();
    PanGesture* swiper = Input::get<PanGesture>();
    if (CU_PLATFORM == CU_PLATFORM_LINUX) {
        swiper->setTouchScreen(true);
    }
    else {
        swiper->setTouchScreen(false);
    }

    swiper->addBeginListener(SWIPE_LISTENER_KEY, [=](const PanEvent& event, bool focus) {
        this->swipeBeganCB(event, focus);
    });

    swiper->addEndListener(SWIPE_LISTENER_KEY, [=](const PanEvent& event, bool focus) {
        this->swipeEndedCB(event, focus);
    });

    _active = success;
    return success;
}


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
void PlatformInput::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    if (!_gameCont) {
        // Map "keyboard" events to the current frame boundary
        _keyReset = keys->keyPressed(RESET_KEY);
        _keyDebug = keys->keyPressed(DEBUG_KEY);
        _keyExit = keys->keyPressed(EXIT_KEY);
        _keyFire = keys->keyPressed(FIRE_KEY);
        _keyJump = keys->keyPressed(JUMP_KEY);
        _keySlow = keys->keyPressed(SLOW_KEY);

        _dashKey = keys->keyPressed(DASH_KEY);

        _keyLeft = keys->keyDown(KeyCode::A);
        _keyRight = keys->keyDown(KeyCode::D);

        _keyUp = keys->keyDown(KeyCode::W);
        _keyDown = keys->keyDown(KeyCode::S);

        _keyTransition = keys->keyPressed(KeyCode::T);
    }
    else {
        _keyJump = _gameCont->isButtonPressed(GameController::Button::A);
        _keyDebug = _gameCont->isButtonPressed(GameController::Button::B);
        _keySlow = _gameCont->isButtonPressed(GameController::Button::X);
        _keyReset = _gameCont->isButtonPressed(GameController::Button::LEFT_SHOULDER);
        _keyExit = _gameCont->isButtonPressed(GameController::Button::RIGHT_SHOULDER);
        _keyTransition = _gameCont->isButtonPressed(GameController::Button::B);


        float lTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_LEFT);
        _dashKey = (lTriggerAmt > TRIGGER_DEADZONE) || _gameCont->isButtonPressed(GameController::Button::Y);

        float rTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_RIGHT);
        _keyFire = (rTriggerAmt > TRIGGER_DEADZONE);


        _xAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_X);
        _yAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_Y);

    }


#else 
    _keyJump = _gameCont->isButtonPressed(GameController::Button::A);
    _keyDebug = _gameCont->isButtonPressed(GameController::Button::B);
    _keySlow = _gameCont->isButtonPressed(GameController::Button::X);
    _keyReset = _gameCont->isButtonPressed(GameController::Button::LEFT_SHOULDER);
    _keyExit = _gameCont->isButtonPressed(GameController::Button::RIGHT_SHOULDER);
    _keyTransition = _gameCont->isButtonPressed(GameController::Button::B);



    float lTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_LEFT);
    _dashKey = (lTriggerAmt > TRIGGER_DEADZONE) || _gameCont->isButtonPressed(GameController::Button::Y);

    float rTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_RIGHT);
    _keyFire = (rTriggerAmt > TRIGGER_DEADZONE);


    _xAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_X);
    _yAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_Y);

#endif
   

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed = _keyExit;
    _firePressed = _keyFire;
    _jumpPressed = _keyJump;
    _slowPressed = _keySlow;
    _dashPressed = _dashKey;
    _transitionPressed = _keyTransition;

    // Directional controls
    _horizontal = 0.0f;
    _vertical = 0.0f;
    if (!_gameCont) {
        if (_keyRight) {
            _horizontal += 1.0f;
        }
        if (_keyLeft) {
            _horizontal -= 1.0f;
        }
        _vertical = 0.0f;
        if (_keyUp) {
            _vertical += 1.0f;
        }
        if (_keyDown) {
            _vertical -= 1.0f;
        }
    }
    else {
        if (std::abs(_xAxis) >= 0.2) {
            _horizontal += _xAxis;
        }
        if (std::abs(_yAxis) >= 0.2) {
            _vertical -= _yAxis;
        }
    }

// If it does not support keyboard, we must reset "virtual" keyboard
//#ifdef CU_TOUCH_SCREEN
//    _keyExit = false;
//    _keyReset = false;
//    _keyDebug = false;
//    _keyJump  = false;
//    _keyFire  = false;
//
//#endif
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void PlatformInput::clear() {
    _resetPressed = false;
    _debugPressed = false;
    _exitPressed  = false;
    _jumpPressed = false;
    _firePressed = false;
    _dashPressed = false;
    _slowPressed = false;
    _transitionPressed = false;
}

#pragma mark -
#pragma mark Touch Controls



/**
 * Populates the initial values of the input TouchInstance
 */
void PlatformInput::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
}




/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 PlatformInput::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}


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
int PlatformInput::processSwipe(const Vec2 start, const Vec2 stop, Timestamp current) {
	// Look for swipes up that are "long enough"
	float xdiff = (stop.x-start.x);
    float thresh = SWIPE_LENGTH;
    if (xdiff > thresh) {
        return 1;
    } else if (xdiff < thresh * -1) {
        return -1;
    }
	return 0;
}



#pragma mark -
#pragma mark Touch and Mouse Callbacks


void PlatformInput::swipeBeganCB(const PanEvent& event, bool focus) {
    CULog("swipe began");
}

void PlatformInput::swipeEndedCB(const PanEvent& event, bool focus) {
    _swipeDelta = event.currPosition - event.origPosition;
    CULog("%f, %f", _swipeDelta.x, _swipeDelta.y);
}



/** Returns touch path.*/
cugl::Path2 PlatformInput::getTouchPath() {
    return _touchPath;
}

/** Returns touch path and sets it to empty. Also sets complete to false.*/
cugl::Path2 PlatformInput::popTouchPath() {
    if (_gestureCompleted) {
        cugl::Path2 temp = _touchPath;
        _touchPath = cugl::Path2();
        _gestureCompleted = false;
        return temp;
    }
    return Path2();
}

/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchBeganCB(const TouchEvent& event, bool focus) {
    CULog("Touch began %lld", event.touch);
    Vec2 pos = event.position;

    gestureStartCB(pos, focus);
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    gestureMoveCB(pos, focus);
}
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchEndedCB(const TouchEvent& event, bool focus) {
    // Reset all keys that might have been set
    Vec2 pos = event.position;
    gestureEndCB(pos, focus);
}

void PlatformInput::mousePressCB(const cugl::MouseEvent& event, bool focus) {
    Vec2 pos = event.position;
    gestureStartCB(pos, focus);
}

void PlatformInput::mouseDragCB(const cugl::MouseEvent& event, bool focus) {
	Vec2 pos = event.position;
    gestureMoveCB(pos, focus);
}

void PlatformInput::mouseReleaseCB(const cugl::MouseEvent& event, bool focus) {
    // Reset all keys that might have been set
    Vec2 pos = event.position;
    gestureEndCB(pos, focus);
}

void PlatformInput::gestureStartCB(Vec2 pos, bool focus) {
    _gestureCompleted = false;
    _touchPath = cugl::Path2();
    _touchPath.push(pos);
}

void PlatformInput::gestureMoveCB(Vec2 pos, bool focus) {
    _touchPath.push(pos);
}

void PlatformInput::gestureEndCB(Vec2 pos, bool focus) {
    CULog("Completed cb");
    _touchPath.push(pos);
    float similarity = -1.0f;
    PathSmoother smoother = PathSmoother();
    smoother.set(_touchPath);
    smoother.calculate();
    _touchPath = smoother.getPath();
    _gestureCompleted = true;
}

