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
#include "PFBullet.h"
#include "PFDudeModel.h"
#include "PFRopeBridge.h"
#include "PFSpinner.h"

/**Todo: Implement a basic dollar gesture input scene that can be represented as 
a child node of GameScene and has a basic rectangle that waits for input from a 
Input Controller and renders something in response to anything received.
 */
class DollarScene : public cugl::Scene2 {
protected:
    PlatformInput _input;

    std::shared_ptr<cugl::AssetManager> _assets;
+

public:

    DollarScene();

    ~DollarScene() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    void update(float timestep);

    bool isPending();
    
    bool isSuccess();
};

#endif /* __PF_GAME_SCENE_H__ */
