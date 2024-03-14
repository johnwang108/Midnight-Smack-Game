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
#include "PFInput.h"
#include "PFAttack.h"
#include "PFDudeModel.h"
#include "PFRopeBridge.h"
#include "PFSpinner.h"

/**Todo: Implement a basic dollar gesture input scene that can be represented as 
a child node of GameScene and has a basic rectangle that waits for input from a 
Input Controller and renders something in response to anything received.
 */
class DollarScene : public cugl::scene2::SceneNode {
protected:
    std::shared_ptr<PlatformInput> _input;

    std::shared_ptr<cugl::AssetManager> _assets;

    //Todo: turn these into nodes
    cugl::Path2 _path;

    std::shared_ptr<cugl::scene2::PolygonNode> _poly;

    std::shared_ptr<cugl::scene2::PolygonNode> _box;

    std::shared_ptr<cugl::scene2::Label> _header;

    std::shared_ptr<cugl::scene2::Label> _currentGestureLabel;

    cugl::SimpleExtruder _se;

    //transform for poly
    cugl::Affine2 _trans;

    //temp
    cugl::Spline2 _spline;

    bool _focus;

    std::vector<std::string> _currentTargetGestures;

   

    //Todo: need library of existing predetermined inputs to check against

public:

    DollarScene();

    DollarScene(const DollarScene&) {};

    ~DollarScene() { dispose(); }

    void dispose();

    bool init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input) {
        return init(assets, input, cugl::Rect(0, 0, 1000, 1000), "panfry_station");
    };

    bool init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture) {
        return init(assets, input, rect, texture, std::vector<std::string>());
    }

    bool init(std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, cugl::Rect rect, std::string texture, std::vector <std::string> gestures);

    void update(float timestep);

    bool isPending();
    
    bool isSuccess();

    void setFocus(bool focus);

    void setTargetGestures(std::vector<std::string> gestures) { _currentTargetGestures = gestures; }


    //virtual void draw(const std::shared_ptr<SpriteBatch>& batch, const Affine2& transform, Color4 tint);

};

#endif /* __PF_DOLLAR_SCENE_H__ */
