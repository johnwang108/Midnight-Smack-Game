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
#define LISTENER_KEY      1

/** The key for the controller event handlers */
#define CONTROLLER_LISTENER_KEY 2

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
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
        touch->removeMotionListener(LISTENER_KEY);
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
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();


    bool contSuccess = Input::activate<GameControllerInput>();

    if (contSuccess) {
        GameControllerInput* controller = Input::get<GameControllerInput>();
        std::vector<std::string> deviceUUIDs = controller->devices();
       
        if (!deviceUUIDs.empty()) {
            _gameCont = controller->open(deviceUUIDs.front());
            CULog("Controller Obtained, Name: %s", _gameCont->getName().c_str());

            ////using axis controllers for joystick
         /*   _gameCont->addAxisListener(CONTROLLER_LISTENER_KEY, [=](const GameControllerAxisEvent& event, bool focus) {
                this->getAxisAngle(event, focus);
                });*/

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
    _lastGestureSimilarity = -1;
    _lastGestureString = "No Touchscreen";
    CULog("no touch screen sad");
#else
    CULog("%d", CU_TOUCH_SCREEN);
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
    _dollarRecog = cugl::GestureRecognizer::alloc();

    if (!_dollarRecog->init()) CULog("Recognizer Init Failed");
    _dollarRecog->setAlgorithm(cugl::GestureRecognizer::Algorithm::ONEDOLLAR);


    std::vector<Vec2> swipeVertices = { Vec2(0,0), Vec2(0,249) };
    cugl::Path2 swipeGesturePath = cugl::Path2(swipeVertices);

    if (!_dollarRecog->addGesture("vertSwipe", swipeGesturePath, true)) CULog("failed to initialize vertSwipe");

    std::vector<Vec2> hSwipeVertices = { Vec2(0,0), Vec2(249,0) };
    cugl::Path2 hSwipeGesturePath = cugl::Path2(hSwipeVertices);

    if (!_dollarRecog->addGesture("horizSwipe", hSwipeGesturePath, true)) CULog("failed to initialize horizSwipe");

    std::vector<Vec2> vVertices = { Vec2(89,164),Vec2(90,162),Vec2(92,162),Vec2(94,164),Vec2(95,166),Vec2(96,169),Vec2(97,171),Vec2(99,175),Vec2(101,178),Vec2(103,182),Vec2(106,189),Vec2(108,194),Vec2(111,199),Vec2(114,204),Vec2(117,209),Vec2(119,214),Vec2(122,218),Vec2(124,222),Vec2(126,225),Vec2(128,228),Vec2(130,229),Vec2(133,233),Vec2(134,236),Vec2(136,239),Vec2(138,240),Vec2(139,242),Vec2(140,244),Vec2(142,242),Vec2(142,240),Vec2(142,237),Vec2(143,235),Vec2(143,233),Vec2(145,229),Vec2(146,226),Vec2(148,217),Vec2(149,208),Vec2(149,205),Vec2(151,196),Vec2(151,193),Vec2(153,182),Vec2(155,172),Vec2(157,165),Vec2(159,160),Vec2(162,155),Vec2(164,150),Vec2(165,148),Vec2(166,146) };
    cugl::Path2 vGesturePath = cugl::Path2(vVertices);

    if (!_dollarRecog->addGesture("v", vGesturePath, true)) CULog("failed to initialize v");

    std::vector<Vec2> circVertices = { Vec2(127,141),Vec2(124,140),Vec2(120,139),Vec2(118,139),Vec2(116,139),Vec2(111,140),Vec2(109,141),Vec2(104,144),Vec2(100,147),Vec2(96,152),Vec2(93,157),Vec2(90,163),Vec2(87,169),Vec2(85,175),Vec2(83,181),Vec2(82,190),Vec2(82,195),Vec2(83,200),Vec2(84,205),Vec2(88,213),Vec2(91,216),Vec2(96,219),Vec2(103,222),Vec2(108,224),Vec2(111,224),Vec2(120,224),Vec2(133,223),Vec2(142,222),Vec2(152,218),Vec2(160,214),Vec2(167,210),Vec2(173,204),Vec2(178,198),Vec2(179,196),Vec2(182,188),Vec2(182,177),Vec2(178,167),Vec2(170,150),Vec2(163,138),Vec2(152,130),Vec2(143,129),Vec2(140,131),Vec2(129,136),Vec2(126,139) };
    cugl::Path2 circGesturePath = cugl::Path2(circVertices);

    if (!_dollarRecog->addGesture("circle", circGesturePath, true)) CULog("failed to initialize circle");

    std::vector<Vec2> pigtailVert = { Vec2(81,219),Vec2(84,218),Vec2(86,220),Vec2(88,220),Vec2(90,220),Vec2(92,219),Vec2(95,220),Vec2(97,219),Vec2(99,220),Vec2(102,218),Vec2(105,217),Vec2(107,216),Vec2(110,216),Vec2(113,214),Vec2(116,212),Vec2(118,210),Vec2(121,208),Vec2(124,205),Vec2(126,202),Vec2(129,199),Vec2(132,196),Vec2(136,191),Vec2(139,187),Vec2(142,182),Vec2(144,179),Vec2(146,174),Vec2(148,170),Vec2(149,168),Vec2(151,162),Vec2(152,160),Vec2(152,157),Vec2(152,155),Vec2(152,151),Vec2(152,149),Vec2(152,146),Vec2(149,142),Vec2(148,139),Vec2(145,137),Vec2(141,135),Vec2(139,135),Vec2(134,136),Vec2(130,140),Vec2(128,142),Vec2(126,145),Vec2(122,150),Vec2(119,158),Vec2(117,163),Vec2(115,170),Vec2(114,175),Vec2(117,184),Vec2(120,190),Vec2(125,199),Vec2(129,203),Vec2(133,208),Vec2(138,213),Vec2(145,215),Vec2(155,218),Vec2(164,219),Vec2(166,219),Vec2(177,219),Vec2(182,218),Vec2(192,216),Vec2(196,213),Vec2(199,212),Vec2(201,211) };
    cugl::Path2 pigtailGesturePath = cugl::Path2(pigtailVert);

    if (!_dollarRecog->addGesture("pigtail", pigtailGesturePath, true)) CULog("failed to initialize pigtail");

    CULog("initialized all recognizer stuff");
    _lastGestureSimilarity = 0;
    _lastGestureString = "";


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
    }
    else {
        _keyJump = _gameCont->isButtonPressed(0);
        _keyDebug = _gameCont->isButtonPressed(1);
        _keySlow = _gameCont->isButtonPressed(2);
        _keyReset = _gameCont->isButtonPressed(4);
        _keyExit = _gameCont->isButtonPressed(5);
        


        float lTriggerAmt = _gameCont->getAxisPosition(4);
        _dashKey = (lTriggerAmt > TRIGGER_DEADZONE) || _gameCont->isButtonPressed(3);

        float rTriggerAmt = _gameCont->getAxisPosition(5);
        _keyFire = (rTriggerAmt > TRIGGER_DEADZONE);


        _xAxis = _gameCont->getAxisPosition(0);
        _yAxis = _gameCont->getAxisPosition(1);

    }

#else

    _keyJump = _gameCont->isButtonPressed(0);
    _keyDebug = _gameCont->isButtonPressed(1);
    _keySlow = _gameCont->isButtonPressed(2); //button 2 is X
    _keyReset = _gameCont->isButtonPressed(4);
    _keyExit = _gameCont->isButtonPressed(5);

    float lTriggerAmt = _gameCont->getAxisPosition(4);
    _dashKey = (lTriggerAmt > TRIGGER_DEADZONE) || _gameCont->isButtonPressed(3);

    float rTriggerAmt = _gameCont->getAxisPosition(5);
    _keyFire = (rTriggerAmt > TRIGGER_DEADZONE);


    _xAxis = _gameCont->getAxisPosition(0);
    _yAxis = _gameCont->getAxisPosition(1);

#endif
   

    _resetPressed = _keyReset;
    _debugPressed = _keyDebug;
    _exitPressed = _keyExit;
    _firePressed = _keyFire;
    _jumpPressed = _keyJump;
    _slowPressed = _keySlow;
    _dashPressed = _dashKey;

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
 * Processes movement for the floating joystick.
 *
 * This will register movement as left or right (or neither).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void PlatformInput::processJoystick(const cugl::Vec2 pos) {
    //Vec2 diff =  _ltouch.position-pos;

    //// Reset the anchor if we drifted too far
    //if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
    //    diff.normalize();
    //    diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
    //    _ltouch.position = pos+diff;
    //}
    //_ltouch.position.y = pos.y;
    //_joycenter = touch2Screen(_ltouch.position);
    //_joycenter.y += JSTICK_OFFSET;
    //
    //if (std::fabsf(diff.x) > JSTICK_DEADZONE) {
    //    _joystick = true;
    //    if (diff.x > 0) {
    //        _keyLeft = true;
    //        _keyRight = false;
    //    } else {
    //        _keyLeft = false;
    //        _keyRight = true;
    //    }
    //} else {
    //    _joystick = false;
    //    _keyLeft = false;
    //    _keyRight = false;
    //}
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
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus	Whether the listener currently has focus
 */
void PlatformInput::touchBeganCB(const TouchEvent& event, bool focus) {
    CULog("Touch began %lld", event.touch);
    Vec2 pos = event.position;

    _touchPath = cugl::Path2();
    _touchPath.push(pos);


    //switch (zone) {
    //    case Zone::LEFT:
    //        // Only process if no touch in zone
    //        if (_ltouch.touchids.empty()) {
    //            // Left is the floating joystick
    //            _ltouch.position = event.position;
    //            _ltouch.timestamp.mark();
    //            _ltouch.touchids.insert(event.touch);

    //            _joystick = true;
    //            _joycenter = touch2Screen(event.position);
    //            _joycenter.y += JSTICK_OFFSET;
    //        }
    //        break;
    //    case Zone::RIGHT:
    //        // Only process if no touch in zone
    //        if (_rtouch.touchids.empty()) {
    //            // Right is jump AND fire controls
    //            _keyFire = (event.timestamp.ellapsedMillis(_rtime) <= DOUBLE_CLICK);
    //            _rtouch.position = event.position;
    //            _rtouch.timestamp.mark();
    //            _rtouch.touchids.insert(event.touch);
    //            _hasJumped = false;
    //        }
    //        break;
    //    case Zone::MAIN:
    //        // Only check for double tap in Main if nothing else down
    //        if (_ltouch.touchids.empty() && _rtouch.touchids.empty() && _mtouch.touchids.empty()) {
    //            _keyDebug = (event.timestamp.ellapsedMillis(_mtime) <= DOUBLE_CLICK);
    //        }
    //        
    //        // Keep count of touches in Main zone if next to each other.
    //        if (_mtouch.touchids.empty()) {
    //            _mtouch.position = event.position;
    //            _mtouch.touchids.insert(event.touch);
    //        } else {
    //            Vec2 offset = event.position-_mtouch.position;
    //            if (offset.lengthSquared() < NEAR_TOUCH*NEAR_TOUCH) {
    //                _mtouch.touchids.insert(event.touch);
    //            }
    //        }
    //        break;
    //    default:
    //        CUAssertLog(false, "Touch is out of bounds");
    //        break;
    //}
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
    _touchPath.push(pos);

    float similarity = -1.0f;

    PathSmoother smoother = PathSmoother();
    smoother.set(_touchPath);
    smoother.calculate();
    _touchPath = smoother.getPath();
    std::string result = _dollarRecog->match(_touchPath, similarity);

    _lastGestureString = result;
    _lastGestureSimilarity = similarity;
    CULog("Gesture Guess: %s, Similarity: %f", result.c_str(), similarity);
    /*Zone zone = getZone(pos);
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        _ltouch.touchids.clear();
        _keyLeft = false;
        _keyRight = false;
        _joystick = false;
    } else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        _hasJumped = false;
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
    } else if (zone == Zone::MAIN) {
        if (_mtouch.touchids.find(event.touch) != _mtouch.touchids.end()) {
            _mtouch.touchids.erase(event.touch);
        }
        _mtime = event.timestamp;
    }*/
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
    _touchPath.push(pos);
    // Only check for swipes in the main zone if there is more than one finger.
    //if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
    //    processJoystick(pos);
    //} else if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
    //    if (!_hasJumped) {
    //        if ((_rtouch.position.y-pos.y) > SWIPE_LENGTH) {
    //            _keyJump = true;
    //            _hasJumped = true;
    //        }
    //    }
    //} else if (_mtouch.touchids.size() > 1) {
    //    // We only process multifinger swipes in main
    //    int swipe = processSwipe(_mtouch.position, event.position, event.timestamp);
    //    if (swipe == 1) {
    //        _keyReset = true;
    //    } else if (swipe == -1) {
    //        _keyExit = true;
    //    }
    //}
}

void PlatformInput::getAxisAngle(const cugl::GameControllerAxisEvent& event, bool focus) {
    //TODO: WHAT ARE AXIS INDICES?? HOW MANY??? 2 or 4
    _xAxis = _gameCont->getAxisPosition(0);
    _yAxis = _gameCont->getAxisPosition(1); 
}

std::string  PlatformInput::getGestureString() {
    return _lastGestureString;
}

float PlatformInput::getGestureSim() {
    return _lastGestureSimilarity;
}

cugl::Path2 PlatformInput::getTouchPath() {
    return _touchPath;
}


