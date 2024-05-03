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
#ifndef __PF_DOLLAR_SCENE_H__
#define __PF_DOLLAR_SCENE_H__
#include <cugl/cugl.h>
#include <box2d/b2_world_callbacks.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <vector>
#include <deque>
#include "PFInput.h"
#include "PFDudeModel.h"
#include "Ingredient.h"

//hardcode :3
#define SCENE_WIDTH 1280
#define SCENE_HEIGHT 800
#define NIGHT_SEQUENCE_LENGTH 3

/**
* 
* 
* 
 */
class DollarScene : public cugl::scene2::SceneNode {
protected:
    std::shared_ptr<PlatformInput> _input;

    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<cugl::GestureRecognizer> _dollarRecog;

    //nighttime enemy stuff
    std::vector<cugl::Path2> _inputtedGestures;

    cugl::Path2 _path;

    std::shared_ptr<cugl::scene2::PolygonNode> _poly;

    std::shared_ptr<cugl::scene2::PolygonNode> _box;

    std::shared_ptr<cugl::scene2::Label> _header;

    std::shared_ptr<cugl::scene2::Label> _currentGestureLabel;

    cugl::SimpleExtruder _se;

  
    //transform for poly
    cugl::Affine2 _transf;

    //temp
    cugl::Spline2 _spline;

    bool _focus;

    bool _readyForGestures;

    std::vector<std::string> _currentTargetGestures;

    int _currentTargetIndex;

    //todo:: integrate _currentTargetGestures into this. Right now, duration sequence is index 0, supereffect is index 1. Don't use index 1
    std::vector<std::vector<std::string>> _currentTargetGesturesNighttime;
    bool _isDurationSequence;

    // this is for cook time only
    bool _completed;
    int _lastResult;

    bool _justCompletedGesture;

     
    int countdown;

    float _currentSimilarity; 


    std::vector<std::string> _validIngredients;

    std::shared_ptr<cugl::scene2::SceneNode> _bottomBar;
    std::shared_ptr<cugl::scene2::SceneNode> _conveyorBelt;
    std::shared_ptr<cugl::scene2::PolygonNode> _stationHitbox;
    std::shared_ptr<cugl::scene2::SceneNode> _indicatorGroup;

    std::deque<std::shared_ptr<Ingredient>> _currentIngredients;

    std::shared_ptr<Ingredient> _ingredientToRemove; 
    std::shared_ptr<Ingredient> _currentlyHeldIngredient;
    std::shared_ptr<Ingredient> _submittedIngredient;

    //technically unnecessary because ingredient knows if it is in pot but also easier to just store pointer
    std::shared_ptr<Ingredient> _ingredientInStation;

    bool _readyToCook;

    bool _isNighttime;

    
    //Todo: need library of existing predetermined inputs to check against

public:

    DollarScene();

    DollarScene(const DollarScene&) {};

    ~DollarScene() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
        return init(assets, input, cugl::Rect(0, 0, 1000, 1000), "panfry_station");
    };

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, std::string texture);
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture) {
        return init(assets, input, rect, texture, std::vector<std::string>());
    }

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector <std::string> gestures);
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector<std::string> gestures, cugl::Size hitboxSize);
    
    std::shared_ptr<DollarScene> alloc(const std::shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
        std::shared_ptr<DollarScene> result = std::make_shared<DollarScene>();
		return (result->init(assets, input, "cooktime") ? result : nullptr);
    }

    void update(float timestep);

    bool isPending();
    
    int gestureResult();

    int getLastResult() { return _lastResult; }

    float getCurrentSimilarity() { return _currentSimilarity; }

 

    void setFocus(bool focus);

    void setTargetGestures(std::vector<std::string> gestures) { 
        _currentTargetGestures = gestures; 
        _currentTargetIndex = 0;
        _completed = false;
    }

    void setTargetGesturesNighttime(std::vector<std::vector<std::string>> v) {
        _currentTargetGesturesNighttime = v;
        _completed = false;
    }

    void setPending(bool pending) { _completed = !pending; }

    void setValidIngredients(std::vector<std::string> ingredients) { _validIngredients = ingredients; };

    void setIngredientInStation(std::shared_ptr<Ingredient> ing) { _ingredientInStation = ing; }
    
    std::shared_ptr<Ingredient> getIngredientInStation() { return _ingredientInStation; }

    std::shared_ptr<Ingredient> getCurrentlyHeldIngredient() { return _currentlyHeldIngredient; }

    /* Only use to transfer ingredient to other station */
    void removeHeldIngredient();
    void receiveHeldIngredient(std::shared_ptr<Ingredient> ing);

    bool isFocus() { return _focus; };

    bool initGestureRecognizer();

    bool matchWithTouchPath();

    bool getJustCompletedGesture() { return _justCompletedGesture; }

    bool getIsDurationSequence() { return _isDurationSequence; }

    std::shared_ptr<cugl::scene2::SceneNode> getBottomBar() { return _bottomBar; }
    void setBottomBar(std::shared_ptr<cugl::scene2::SceneNode> bar);

    void addIngredient(std::shared_ptr<Ingredient> ingredient);
    std::shared_ptr<Ingredient> popIngredient();


    void updateConveyor();
    /* Internal use, searches through all ingredients to see which is held */
    std::shared_ptr<Ingredient> findHeldIngredient();

    void addIngredientToStation(std::shared_ptr<Ingredient>);

    void handleCompletedIngredient(std::shared_ptr<Ingredient>);

    void reset();

    void setReadyToCook(bool ready) { _readyToCook = ready; }

    void launchIngredient(std::shared_ptr<Ingredient> ing);

    void submitIngredient(std::shared_ptr<Ingredient> ing);

    std::shared_ptr<Ingredient> getSubmittedIngredient() { return _submittedIngredient; }

    void clearSubmittedIngredient() { _submittedIngredient.reset(); }

    void setNighttime(bool b) { _isNighttime = b; }
};

#endif /* __PF_DOLLAR_SCENE_H__ */
