//
//  PFApp.h
//  Platform Demo
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#include "PFApp.h"

using namespace cugl;

#define MULTI_SCREEN true


#pragma mark -
#pragma mark Application State
/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void PlatformApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    
    // Start-up basic input
#ifdef CU_TOUCH_SCREEN
    Input::activate<Touchscreen>();
    CULog("Touchscreen seen");
#else
    Input::activate<Mouse>();
#endif
   

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());

    // Create a "loading" screen
    _loaded = false;
    _loading.init(_assets);

    std::shared_ptr<PlatformInput> input = std::make_shared<PlatformInput>();
    
    // Que up the other assets
    AudioEngine::start();
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    _currentScene = "";
    

    Application::onStartup(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void PlatformApp::onShutdown() {
    //_gameplay.save();

    _menu.dispose();
    _multiScreen.dispose();
    _loading.dispose();
    _gameplay.dispose();
    _assets = nullptr;
    _batch = nullptr;
    
    // Shutdown input
#ifdef CU_TOUCH_SCREEN
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    
    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void PlatformApp::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void PlatformApp::onResume() {
    AudioEngine::get()->resume();
}


#pragma mark -
#pragma mark Application Loop

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void PlatformApp::update(float dt) {
    if (!_loaded && _loading.isActive()) {//!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode

        std::shared_ptr<PlatformInput> input = std::make_shared<PlatformInput>();

        _multiScreen.init(_assets, input);
        _multiScreen.setActive(false);

        /*_dayUIScene = std::make_shared<cugl::scene2::SceneNode>();
        _dayUIScene->init();
        _dayUIScene->setActive(MULTI_SCREEN);*/

        _gameplay.init(_assets, input);
        _gameplay.setActive(false);

        _menu.init(_assets, "menu");
        _menu.setActive(true);
        _currentScene = "main_menu";

        _loaded = true;

        // Switch to deterministic mode
        setDeterministic(true);
    }
}

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
void PlatformApp::preUpdate(float dt) {
    transitionScenes();
    if (_menu.isActive()) {
        _menu.update(dt);
    }
    else if (_gameplay.isActive()) {
        _gameplay.preUpdate(dt);
    }
    else if (_multiScreen.isActive()) {
        _multiScreen.preUpdate(dt);
    }
    else {

    }
 //   if (_gameplay.didTransition()) {

 //       CULog("1");
 //       _gameplay.setActive(false);
 //       _gameplay.transition(false);

 //       _multiScreen.setActive(true);
 //       _multiScreen.preUpdate(dt);
 //       _multiScreen.focusCurr();
 //   }
 //   else if (_multiScreen.didTransition()) {
 //       CULog("2");
	//	_multiScreen.transition(false);
	//	_multiScreen.setActive(false);
 //       _multiScreen.unfocusAll();


	//	_gameplay.setActive(true);
	//	_gameplay.preUpdate(dt);
 //      
 //   }
 //   else if (_gameplay.isActive()) {
 //       CULog("3");
	//	_gameplay.preUpdate(dt);
 //   }
 //   else if (_multiScreen.isActive() || _menu.started()) {
 //       CULog("4");
	//	_multiScreen.preUpdate(dt);
 //   }
 //   else {
 //       _menu.setStarted(false);
 //       _menu.setActive(true);
 //       _menu.update(dt);
	//}
}

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
 */
void PlatformApp::fixedUpdate() {
    // Compute time to report to game scene version of fixedUpdate
    float time = getFixedStep()/1000000.0f;
    if (_gameplay.isActive()) {
        _gameplay.fixedUpdate(time);
    }
    else if (_multiScreen.isActive()){
        _multiScreen.fixedUpdate(time);
    }
    
    
}

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
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #preUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void PlatformApp::postUpdate(float dt) {
    // Compute time to report to game scene version of postUpdate
    float time = getFixedRemainder()/1000000.0f;
    if (_gameplay.isActive()) {
		_gameplay.postUpdate(time);
	}
    else if (_multiScreen.isActive()){
		_multiScreen.postUpdate(time);
	}
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void PlatformApp::draw() {
    if (!_loaded && _loading.isActive()) {
        _loading.render(_batch);
    }
    if (_menu.isActive()) {
        _menu.render(_batch);
    }
    else if (_gameplay.isActive()) {
        _gameplay.renderBG(_batch);
        _gameplay.render(_batch);
        _gameplay.renderUI(_batch);
    }
    else if (_multiScreen.isActive()) {
        _multiScreen.render(_batch);
        _multiScreen.renderUI(_batch);
    }
    else {

    }
    //if (_menu.isActive()) {
    //    //_loading.render(_batch);
    //    _menu.render(_batch);
    //} else {
    //    if (_gameplay.isActive()) {
    //        _gameplay.renderBG(_batch);
    //        _gameplay.render(_batch);
    //        _gameplay.renderUI(_batch);
    //    }
    //    else {
    //        _multiScreen.render(_batch);
    //        _multiScreen.renderUI(_batch);
    //    }
    //}
}


void PlatformApp::transitionScenes() {
    if (_gameplay.didTransition()) {
		_gameplay.setActive(false);
		_gameplay.transition(false);

		_currentScene = _gameplay.getTarget();
        _gameplay.setTarget("");
        if(_currentScene == "day") {
			_multiScreen.setActive(true);
			_multiScreen.focusCurr();
        }
        else if (_currentScene == "main_menu"){
			_menu.setActive(true);
        }
        CULog("Transed");
        CULog("From gameplay");
	}
	else if (_multiScreen.didTransition()) {
		_multiScreen.transition(false);
		_multiScreen.setActive(false);
		_multiScreen.unfocusAll();

		_currentScene = _multiScreen.getTarget();
        _multiScreen.setTarget("");
        if (_currentScene == "night") {
            _gameplay.setActive(true);
        }
        else if (_currentScene == "main_menu") {
            _menu.setActive(true);
        }

        CULog("Transed");
        CULog("From Multi");
    }
    else if (_menu.didTransition()) {
        _menu.setActive(false);
        _menu.setTransition(false);

        _currentScene = _menu.getTarget();  
        _menu.setTarget("");
		if (_currentScene == "night") {
			_gameplay.setActive(true);
            
		}
		else if (_currentScene == "day") {
			_multiScreen.setActive(true);
			_multiScreen.focusCurr();
		}

        CULog("Transed");
        CULog("From menu");
    }
}