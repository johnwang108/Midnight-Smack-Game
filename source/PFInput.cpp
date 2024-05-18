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

#define LEVEL1_KEY KeyCode::NUM_1
#define LEVEL2_KEY KeyCode::NUM_2
#define LEVEL3_KEY KeyCode::NUM_3

//#define MUSIC_KEY KeyCode::M
#define ANIMATE_KEY KeyCode::N
#define BACKGROUND_KEY KeyCode::B

/** Slow key */
#define SLOW_KEY KeyCode::TAB

#define INV_LEFT_KEY KeyCode::ARROW_LEFT
#define INV_RIGHT_KEY KeyCode::ARROW_RIGHT

#define INTERACT_KEY KeyCode::E

/** How close we need to be for a multi touch */
#define NEAR_TOUCH      100
/** The key for the event handlers */
#define GESTURE_LISTENER_KEY  1
#define MOUSE_LISTENER_KEY 3

#define TRIGGER_DEADZONE 0

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
    _keyJump(false),
    _keyFire(false),
    _keyReset(false),
    _keyDebug(false),
    _keyExit(false),
    _keyLeft(false),
    _keyRight(false),
    _keySlow(false),
    _keySlowReleased(false),
    _keyInteract(false),
    _keyInventoryLeft(false),
    _keyInventoryRight(false),
    _keyMinimap(false),
    _keyPause(false),
    _keyLevel1(false),
    _keyLevel2(false),
    _keyLevel3(false),
    _keyAnimate(false),
    _keyBackground(false),
    _gestureCompleted(false),
    _keyDown(false),
    _keyUp(false),
    _keyDash(false),
    _resetPressed(false),
    _debugPressed(false),
    _exitPressed(false),
    _firePressed(false),
    _jumpPressed(false),
    _interactPressed(false),
    _slowPressed(false),
    _slowHeldDuration(0.0f),
    _lastSlowHeldDuration(0.0f),
    _slowReleased(false),
    _inventoryLeftPressed(false),
    _inventoryRightPressed(false),
    _horizontal(0.0f),
    _vertical(0.0f),
    _dashPressed(false),
    _minimapPressed(false),
    _pausePressed(false),
    _level1Pressed(false),
    _level2Pressed(false),
    _level3Pressed(false),
    _animatePressed(false),
    _backgroundPressed(false),
    _zoomIn(false),
    _zoomOut(false),
    _xAxis(0.0f),
    _yAxis(0.0f),
    _touchPos(Vec2(0, 0)),
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
        Mouse* mouse = Input::get<Mouse>();
        mouse->removePressListener(MOUSE_LISTENER_KEY);
        mouse->removeDragListener(MOUSE_LISTENER_KEY);
        mouse->removeReleaseListener(MOUSE_LISTENER_KEY);
        Input::deactivate<Mouse>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(GESTURE_LISTENER_KEY);
        touch->removeEndListener(GESTURE_LISTENER_KEY);
        touch->removeMotionListener(GESTURE_LISTENER_KEY);
        GameControllerInput* controller = Input::get<GameControllerInput>();
        if (_gameCont != nullptr) {
            controller->close(_gameCont->getUID());
    }
        Input::deactivate<GameControllerInput>();
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
    //CULog("ID: %f", id);
    bool success = true;

    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    _touchPath = cugl::Path2();
    _touchPos = Vec2(0,0);
    _gestureCompleted = false;
    _slowHeldDuration = 0.0f;
    _lastSlowHeldDuration = _slowHeldDuration;
   

#ifndef CU_TOUCH_SCREEN
    CULog("Not a touch screen compatible OS");
    success = Input::activate<Keyboard>();
    success = success && Input::activate<Mouse>();
    CULog("%s", success ? "Mouse and Keyboard Activated" : "Mouse and/or Keyboard Failed");

    Mouse* mouse = Input::get<Mouse>();
    mouse->setPointerAwareness(Mouse::PointerAwareness::DRAG);
    mouse->addPressListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        //CULog("STARTING!");
        this->mousePressCB(event, focus);
        });

    mouse->addDragListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, const Vec2& previous, bool focus) {
        //CULog("DRAGGIN!");
        this->mouseDragCB(event, focus);
        });

    mouse->addReleaseListener(MOUSE_LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseReleaseCB(event, focus);
        });
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(GESTURE_LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event, focus);
        });
    touch->addEndListener(GESTURE_LISTENER_KEY, [=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event, focus);
        });
    touch->addMotionListener(GESTURE_LISTENER_KEY, [=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
        });

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
            contSuccess = false;
        }
    }
    
    if (!contSuccess) {
        CULogError("Failed to initialize game controller");
    }
    

#endif


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
    // Map "keyboard" events to the current frame boundary
    _keyReset = keys->keyPressed(RESET_KEY);
    _keyDebug = keys->keyPressed(DEBUG_KEY);
    _keyExit = keys->keyPressed(EXIT_KEY);
    _keyFire = keys->keyPressed(FIRE_KEY);
    _keyJump = keys->keyDown(JUMP_KEY);
    _keyInventoryLeft = keys->keyPressed(INV_LEFT_KEY);
    _keyInventoryRight = keys->keyPressed(INV_RIGHT_KEY);
    _keyInteract = keys->keyPressed(INTERACT_KEY);

    _keySlow = keys->keyPressed(SLOW_KEY);
    _keySlowReleased = keys->keyReleased(SLOW_KEY);

    if (keys->keyDown(SLOW_KEY)) {
        _slowHeldDuration += dt;
    }
    else {
        _lastSlowHeldDuration = _slowHeldDuration;
        _slowHeldDuration = 0.0f;
    }

    _keyDash = keys->keyPressed(DASH_KEY);

    _keyLeft = keys->keyDown(KeyCode::A);
    _keyRight = keys->keyDown(KeyCode::D);

    _keyUp = keys->keyDown(KeyCode::W);
    _keyDown = keys->keyDown(KeyCode::S);

    _keyPause = keys->keyPressed(KeyCode::P);

    _keyMinimap = keys->keyPressed(KeyCode::M);

    _keyAnimate = keys->keyPressed(ANIMATE_KEY);
    _keyBackground = keys->keyPressed(BACKGROUND_KEY);

    _keyLevel1 = keys->keyPressed(LEVEL1_KEY);
    _keyLevel2 = keys->keyPressed(LEVEL2_KEY);
    _keyLevel3 = keys->keyPressed(LEVEL3_KEY);
        //_keyMusic = keys->keyPressed(MUSIC_KEY);
    
#else 

    _keyJump = _gameCont->isButtonPressed(GameController::Button::A);
    //_keyDebug = _gameCont->isButtonPressed(GameController::Button::B);
    _keyInventoryLeft = _gameCont->isButtonPressed(GameController::Button::LEFT_SHOULDER);
    _keyInventoryRight = _gameCont->isButtonPressed(GameController::Button::RIGHT_SHOULDER);
    _keySlow = _gameCont->isButtonPressed(GameController::Button::Y);
    _keySlowReleased = _gameCont->isButtonReleased(GameController::Button::Y);

    _keyFire = _gameCont->isButtonReleased(GameController::Button::X);


    if (_gameCont->isButtonDown(GameController::Button::Y)) {
        _slowHeldDuration += dt;
    }
    else {
        _lastSlowHeldDuration = _slowHeldDuration;
        _slowHeldDuration = 0.0f;
    }

    _keyPause = _gameCont->isButtonPressed(GameController::Button::START);
    _keyMinimap = _gameCont->isButtonPressed(GameController::Button::BACK);
    _keyInteract = _gameCont->isButtonPressed(GameController::Button::B);


    float lTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_LEFT);
    _keyDash = (lTriggerAmt > TRIGGER_DEADZONE);

    /*  float rTriggerAmt = _gameCont->getAxisPosition(GameController::Axis::TRIGGER_RIGHT);
    _keyFire = (rTriggerAmt > TRIGGER_DEADZONE);*/


    _xAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_X);
    _yAxis = _gameCont->getAxisPosition(GameController::Axis::LEFT_Y);



#endif


    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed = _keyExit;
    _firePressed = _keyFire;
    _jumpPressed = _keyJump;
    _slowPressed = _keySlow;
    _slowReleased = _keySlowReleased;
    _dashPressed = _keyDash;
    _pausePressed = _keyPause;
    _minimapPressed = _keyMinimap;
    _interactPressed = _keyInteract;

    _level1Pressed = _keyLevel1;
    _level2Pressed = _keyLevel2;
    _level3Pressed = _keyLevel3;

    _inventoryLeftPressed = _keyInventoryLeft;
    _inventoryRightPressed = _keyInventoryRight;

    _animatePressed = _keyAnimate;
    _backgroundPressed = _keyBackground;
    //_musicPressed = _keyMusic;

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
    _exitPressed = false;
    _jumpPressed = false;
    _firePressed = false;
    _dashPressed = false;
    _slowPressed = false;
    _pausePressed = false;
    _minimapPressed = false;
    _animatePressed = false;
    _backgroundPressed = false;
    //_musicPressed = false;
    _inventoryLeftPressed = false;
    _inventoryRightPressed = false;
    _interactPressed = false;
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
    float px = pos.x / _tbounds.size.width - _tbounds.origin.x;
    float py = pos.y / _tbounds.size.height - _tbounds.origin.y;
    Vec2 result;
    result.x = px * _sbounds.size.width + _sbounds.origin.x;
    result.y = (1 - py) * _sbounds.size.height + _sbounds.origin.y;
    return result;
}



#pragma mark -
#pragma mark Touch and Mouse Callbacks



/** Returns touch path.*/
cugl::Path2 PlatformInput::getTouchPath() {
    return _touchPath;
}

/** Returns touch path and sets it to empty. Also sets complete to false.*/
cugl::Path2 PlatformInput::popTouchPath() {
    if (_gestureCompleted) {
        CULog("popped");
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
    _touchPos = pos;
}

void PlatformInput::gestureEndCB(Vec2 pos, bool focus) {
    _touchPath.push(pos);
    float similarity = -1.0f;
    PathSmoother smoother = PathSmoother();
    smoother.set(_touchPath);
    smoother.calculate();
    _touchPath = smoother.getPath();
    _gestureCompleted = true;
}