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

namespace GesturePaths {
    const std::vector<cugl::Vec2> vVertsPrimary = { Vec2(89,164),Vec2(90,162),Vec2(92,162),Vec2(94,164),Vec2(95,166),Vec2(96,169),Vec2(97,171),Vec2(99,175),Vec2(101,178),Vec2(103,182),Vec2(106,189),Vec2(108,194),Vec2(111,199),Vec2(114,204),Vec2(117,209),Vec2(119,214),Vec2(122,218),Vec2(124,222),Vec2(126,225),Vec2(128,228),Vec2(130,229),Vec2(133,233),Vec2(134,236),Vec2(136,239),Vec2(138,240),Vec2(139,242),Vec2(140,244),Vec2(142,242),Vec2(142,240),Vec2(142,237),Vec2(143,235),Vec2(143,233),Vec2(145,229),Vec2(146,226),Vec2(148,217),Vec2(149,208),Vec2(149,205),Vec2(151,196),Vec2(151,193),Vec2(153,182),Vec2(155,172),Vec2(157,165),Vec2(159,160),Vec2(162,155),Vec2(164,150),Vec2(165,148),Vec2(166,146) };
    const std::vector<cugl::Vec2> circVertsPrimary = { Vec2(127,141),Vec2(124,140),Vec2(120,139),Vec2(118,139),Vec2(116,139),Vec2(111,140),Vec2(109,141),Vec2(104,144),Vec2(100,147),Vec2(96,152),Vec2(93,157),Vec2(90,163),Vec2(87,169),Vec2(85,175),Vec2(83,181),Vec2(82,190),Vec2(82,195),Vec2(83,200),Vec2(84,205),Vec2(88,213),Vec2(91,216),Vec2(96,219),Vec2(103,222),Vec2(108,224),Vec2(111,224),Vec2(120,224),Vec2(133,223),Vec2(142,222),Vec2(152,218),Vec2(160,214),Vec2(167,210),Vec2(173,204),Vec2(178,198),Vec2(179,196),Vec2(182,188),Vec2(182,177),Vec2(178,167),Vec2(170,150),Vec2(163,138),Vec2(152,130),Vec2(143,129),Vec2(140,131),Vec2(129,136),Vec2(126,139) };
    const std::vector<cugl::Vec2> pigtailVertsPrimary = { Vec2(81,219),Vec2(84,218),Vec2(86,220),Vec2(88,220),Vec2(90,220),Vec2(92,219),Vec2(95,220),Vec2(97,219),Vec2(99,220),Vec2(102,218),Vec2(105,217),Vec2(107,216),Vec2(110,216),Vec2(113,214),Vec2(116,212),Vec2(118,210),Vec2(121,208),Vec2(124,205),Vec2(126,202),Vec2(129,199),Vec2(132,196),Vec2(136,191),Vec2(139,187),Vec2(142,182),Vec2(144,179),Vec2(146,174),Vec2(148,170),Vec2(149,168),Vec2(151,162),Vec2(152,160),Vec2(152,157),Vec2(152,155),Vec2(152,151),Vec2(152,149),Vec2(152,146),Vec2(149,142),Vec2(148,139),Vec2(145,137),Vec2(141,135),Vec2(139,135),Vec2(134,136),Vec2(130,140),Vec2(128,142),Vec2(126,145),Vec2(122,150),Vec2(119,158),Vec2(117,163),Vec2(115,170),Vec2(114,175),Vec2(117,184),Vec2(120,190),Vec2(125,199),Vec2(129,203),Vec2(133,208),Vec2(138,213),Vec2(145,215),Vec2(155,218),Vec2(164,219),Vec2(166,219),Vec2(177,219),Vec2(182,218),Vec2(192,216),Vec2(196,213),Vec2(199,212),Vec2(201,211) };
    const std::vector<cugl::Vec2> triangleVertsPrimary = { Vec2(137,139),Vec2(135,141),Vec2(133,144),Vec2(132,146),Vec2(130,149),Vec2(128,151),Vec2(126,155),Vec2(123,160),Vec2(120,166),Vec2(116,171),Vec2(112,177),Vec2(107,183),Vec2(102,188),Vec2(100,191),Vec2(95,195),Vec2(90,199),Vec2(86,203),Vec2(82,206),Vec2(80,209),Vec2(75,213),Vec2(73,213),Vec2(70,216),Vec2(67,219),Vec2(64,221),Vec2(61,223),Vec2(60,225),Vec2(62,226),Vec2(65,225),Vec2(67,226),Vec2(74,226),Vec2(77,227),Vec2(85,229),Vec2(91,230),Vec2(99,231),Vec2(108,232),Vec2(116,233),Vec2(125,233),Vec2(134,234),Vec2(145,233),Vec2(153,232),Vec2(160,233),Vec2(170,234),Vec2(177,235),Vec2(179,236),Vec2(186,237),Vec2(193,238),Vec2(198,239),Vec2(200,237),Vec2(202,239),Vec2(204,238),Vec2(206,234),Vec2(205,230),Vec2(202,222),Vec2(197,216),Vec2(192,207),Vec2(186,198),Vec2(179,189),Vec2(174,183),Vec2(170,178),Vec2(164,171),Vec2(161,168),Vec2(154,160),Vec2(148,155),Vec2(143,150),Vec2(138,148),Vec2(136,148) };
    const std::vector<cugl::Vec2> caretVertsPrimary = { Vec2(79,245),Vec2(79,242),Vec2(79,239),Vec2(80,237),Vec2(80,234),Vec2(81,232),Vec2(82,230),Vec2(84,224),Vec2(86,220),Vec2(86,218),Vec2(87,216),Vec2(88,213),Vec2(90,207),Vec2(91,202),Vec2(92,200),Vec2(93,194),Vec2(94,192),Vec2(96,189),Vec2(97,186),Vec2(100,179),Vec2(102,173),Vec2(105,165),Vec2(107,160),Vec2(109,158),Vec2(112,151),Vec2(115,144),Vec2(117,139),Vec2(119,136),Vec2(119,134),Vec2(120,132),Vec2(121,129),Vec2(122,127),Vec2(124,125),Vec2(126,124),Vec2(129,125),Vec2(131,127),Vec2(132,130),Vec2(136,139),Vec2(141,154),Vec2(145,166),Vec2(151,182),Vec2(156,193),Vec2(157,196),Vec2(161,209),Vec2(162,211),Vec2(167,223),Vec2(169,229),Vec2(170,231),Vec2(173,237),Vec2(176,242),Vec2(177,244),Vec2(179,250),Vec2(181,255),Vec2(182,257) };
};

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



    //technically unnecessary because ingredient knows if it is in pot but also easier to just store pointer
    std::shared_ptr<Ingredient> _ingredientInStation;

    bool _readyToCook;

    bool _isNighttime;

    bool _isStation;

    
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

    void setIsStation(bool b) { _isStation = b; }

    bool isStation() { return _isStation; }
    
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


  

    bool isFocus() { return _focus; };

    bool initGestureRecognizer();
    bool initAlternateGestures();


    bool matchWithTouchPath();

    bool getJustCompletedGesture() { return _justCompletedGesture; }

    bool getIsDurationSequence() { return _isDurationSequence; }





    void addIngredientToStation(std::shared_ptr<Ingredient>);


    void reset();

    void setReadyToCook(bool ready) { _readyToCook = ready; }

    void setNighttime(bool b) { _isNighttime = b; }
};

#endif /* __PF_DOLLAR_SCENE_H__ */
