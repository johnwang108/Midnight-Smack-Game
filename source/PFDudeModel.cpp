//
//  PFDudeModel.cpp
//  PlatformDemo
//
//  This encapsulates all of the information for the character avatar.  Note how this
//  class combines physics and animation.  This is a good template for models in
//  your game.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt well
//  to data driven design.  This demo has a lot of simplifications to make it a bit
//  easier to see how everything fits together.  However, the model classes and how
//  they are initialized will need to be changed if you add dynamic level loading.
//
//  Pay close attention to how this class designed.  Subclasses of Cocos2d classes
//  (which are all subclasses of the class Ref) should never have normal public
//  constructors.  Instead, you should organize their constructors into three parts,
//  just like we have done in this class.
//
//  NORMAL CONSTRUCTOR:
//  The standard constructor should be protected (not private).  It should only
//  initialize pointers to nullptr and primitives to their defaults (pointers are
//  not always nullptr to begin with).  It should NOT take any arguments and should
//  not allocate any memory or call any methods.
//
//  STATIC CONSTRUCTOR
//  This is a static method that allocates the object and initializes it.  If
//  initialization fails, it immediately disposes of the object.  Otherwise, it
//  returns an autoreleased object, starting the garbage collection system.
//  These methods all look the same.  You can copy-and-paste them from sample code.
//  The only difference is the init method called.
//
//  INIT METHOD
//  This is a protected method that acts like what how would normally think a
//  constructor might work.  It allocates memory and initializes all values
//  according to provided arguments.  As memory allocation can fail, this method
//  needs to return a boolean indicating whether or not initialization was
//  successful.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White and Anthony Perello
//  Version:  2/9/17
//
#include "PFDudeModel.h"
#include <cugl/scene2/graph/CUPolygonNode.h>
#include <cugl/scene2/graph/CUTexturedNode.h>
#include <cugl/assets/CUAssetManager.h>
#include <cmath>

#define SIGNUM(x)  ((x > 0) - (x < 0))

#define PASSIVE_KEY "passive_action"

#define ACTIVE_KEY "active_action"


/**Modif for the max height jump in ~(Sues +1)*/
float jmpHeight = 1;
/**Modif dash, as a multiple of DASH_JUMP*/
float dashModif = 1.3;
/**The aount of frames following a dash that SUe floats for*/
float floatyFrames = 10;

#pragma mark -
#pragma mark Physics Constants
/** Cooldown (in animation frames) for jumping */
#define JUMP_COOLDOWN   5
/** Cooldown (in animation frames) for shooting */
#define DASH_COOLDOWN  20
/** Cooldown (in animation frames) for shooting */
#define SHOOT_COOLDOWN  20
/** The amount to shrink the body fixture (vertically) relative to the image */
#define DUDE_VSHRINK  0.95f
/** The amount to shrink the body fixture (horizontally) relative to the image */
#define DUDE_HSHRINK  0.7f
/** The amount to shrink the sensor fixture (horizontally) relative to the image */
#define DUDE_SSHRINK  0.7f
/** Height of the sensor attached to the player's feet */
#define SENSOR_HEIGHT   0.1f
/** The density of the character */
#define DUDE_DENSITY    1.0f
/** The impulse for the character jump */
#define DUDE_JUMP       (sqrt( 3 * 2 * (9.8) * getHeight() * jmpHeight ) * getMass() * getJumpBuff())
/** The impulse for the character dash */
#define DUDE_DASH       (DUDE_JUMP * dashModif)
/** Debug color for the sensor */
#define DEBUG_COLOR     Color4::RED

//placeholder.  Will be replaced by a json file. action string name maps to action info.
#define ACTIONS_INFO {}


using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Initializes a new dude at the given position.
 *
 * The dude is sized according to the given drawing scale.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param pos   Initial position in world coordinates
 * @param size  The size of the dude in world units
 * @param scale The drawing scale (world to screen)
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool DudeModel::init(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
    setEnabled(false);
    Size nsize = size;
    //nsize.width  *= DUDE_HSHRINK;
    //nsize.height *= DUDE_VSHRINK;
    _drawScale = scale;
    
    if (CapsuleObstacle::init(pos,nsize)) {
        setDensity(DUDE_DENSITY);
        setFriction(0.0f);      // HE WILL STICK TO WALLS IF YOU FORGET
        setFixedRotation(true); // OTHERWISE, HE IS A WEEBLE WOBBLE
        // Gameplay attributes
        _isGrounded = false;
        _isShooting = false;
        _isJumping  = false;
        _faceRight  = true;
        _dash = true;
        _contactingWall = false;

        _dashCooldown = 0;
        _shootCooldown = 0;
        _jumpCooldown  = 0;
        _dashNum = 1;

        _health=100;

        _healthCooldown=0.2;
        _lastDamageTime=0;
        _knockbackTime = 0;

        _hasSuper = false;

        //default hardcode
        _attack = 34;

        //animation inits
        //_actionManager = cugl::scene2::ActionManager::alloc();
        _actions = std::unordered_map<std::string, std::shared_ptr<cugl::scene2::Animate>>();
        _sheets = std::unordered_map<std::string, std::shared_ptr<cugl::Texture>>();
        _info = std::unordered_map<std::string, std::tuple<int,int,int,float,bool>>();
        _activeAction = "";
        _numberOfTouchingEnemies = 0;

        _meter = 0;

        b2Filter filter = getFilterData();
        filter.groupIndex = -1;
        setFilterData(filter);

        setEnabled(true);

        return true;
    }
    return false;
}

/** Register a new animation in the dict*/
void DudeModel::addActionAnimation(std::string action_name, std::shared_ptr<cugl::Texture> sheet, int rows, int cols, int size, float duration, bool isPassive) {
    std::vector<int> forward;
    for (int ii = 0; ii < size; ii++) {
        forward.push_back(ii);
    }
    _actions[action_name] = cugl::scene2::Animate::alloc(forward, duration);
    _sheets[action_name] = sheet;
    _info[action_name] = std::make_tuple(rows, cols, size, duration, isPassive);
}

/**Unsure if override needed. Begins an animation, switching the sheet if needed.*/
void DudeModel::animate(std::string action_name) {
    //first, switch the sheet
    changeSheet(action_name);
    if (action_name == "idle") {
        _node->setScale(0.35/4);
    }
    else {
        _node->setScale(0.35/4);
    }
    _activeAction = action_name;

    //info = {int rows, int cols, int size, float duration, bool isPassive}
    auto info = _info[action_name];
}

void DudeModel::changeSheet(std::string action_name) {
    //info = {int rows, int cols, int size, float duration, bool isPassive}
    try {
        auto info = _info[action_name];
        _node->changeSheet(_sheets[action_name], std::get<0>(info), std::get<1>(info), std::get<2>(info));
    }
    catch (int e) {
        CULog("Error changing sheets. Is the action registered?");
    }
    
}

void DudeModel::sethealthbar(std::shared_ptr<cugl::AssetManager> asset) {
    auto healthBarBackground = scene2::PolygonNode::allocWithTexture(asset->get<Texture>("heartsbroken"));
    auto healthBarForeground = scene2::PolygonNode::allocWithTexture(asset->get<Texture>("heartsfull"));
    healthBarBackground->setPosition(Vec2(10, 10));
    healthBarForeground->setPosition(Vec2(10, 10));
    _node->addChild(healthBarBackground);
    _node->addChild(healthBarForeground);
    _healthBarForeground = healthBarForeground;
}
#pragma mark -
#pragma mark Attribute Properties

/**
 * Sets left/right movement of this character.
 *
 * This is the result of input times dude force.
 *
 * @param value left/right movement of this character.
 */
void DudeModel::setMovement(float h) {
    _movement = h;
    bool face = _movement > 0;
    if (_movement == 0 || _faceRight == face) {
        return;
    }
    
    // Change facing
    scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(_node.get());
    if (image != nullptr) {
        image->flipHorizontal(!image->isFlipHorizontal());
    }
    _faceRight = face;
}

#pragma mark -
#pragma mark Physics Methods
/**
 * Create new fixtures for this body, defining the shape
 *
 * This is the primary method to override for custom physics objects
 */
void DudeModel::createFixtures() {
    if (_body == nullptr) {
        return;
    }
    
    CapsuleObstacle::createFixtures();
    b2FixtureDef sensorDef;
    sensorDef.density = DUDE_DENSITY;
    sensorDef.isSensor = true;
    
    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[0].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    corners[1].x = -DUDE_SSHRINK*getWidth()/2.0f;
    corners[1].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[2].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[2].y = (-getHeight()-SENSOR_HEIGHT)/2.0f;
    corners[3].x =  DUDE_SSHRINK*getWidth()/2.0f;
    corners[3].y = (-getHeight()+SENSOR_HEIGHT)/2.0f;
    
    b2PolygonShape sensorShape;
    sensorShape.Set(corners,4);
    
    sensorDef.shape = &sensorShape;
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getSensorName());
    _sensorFixture = _body->CreateFixture(&sensorDef);
    b2Filter filter = getFilterData();
    filter.groupIndex = -1;
    _sensorFixture->SetFilterData(filter);



    corners[0].x = -getWidth()*0.85 / 2.0f;
    corners[0].y = getHeight() * 0.85 / 2.0f;
    corners[1].x = -getWidth() * 0.85 / 2.0f;
    corners[1].y = -getHeight() * 0.85 / 2.0f;
    corners[2].x = getWidth() * 0.85 / 2.0f;
    corners[2].y = -getHeight() * 0.85 /2.0f;
    corners[3].x = getWidth() * 0.85 / 2.0f;
    corners[3].y = getHeight() * 0.85 / 2.0f;

    sensorShape.Set(corners, 4);
    sensorDef.shape = &sensorShape;
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(getBodySensorName());
    sensorDef.isSensor = true;
    _bodySensorFixture = _body->CreateFixture(&sensorDef);


}

/**
 * Release the fixtures for this body, reseting the shape
 *
 * This is the primary method to override for custom physics objects.
 */
void DudeModel::releaseFixtures() {
    if (_body != nullptr) {
        return;
    }
    
    CapsuleObstacle::releaseFixtures();
    if (_sensorFixture != nullptr) {
        _body->DestroyFixture(_sensorFixture);
        _sensorFixture = nullptr;

        _body->DestroyFixture(_bodySensorFixture);
        _bodySensorFixture = nullptr;
    }
}

/**
 * Disposes all resources and assets of this DudeModel
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a DudeModel may not be used until it is initialized again.
 */
void DudeModel::dispose() {
    _core = nullptr;
    _node = nullptr;
    _sensorNode = nullptr;
    _bodySensorNode = nullptr;
}

/**
 * Applies the force to the body of this dude
 *
 * This method should be called after the force attribute is set.
 */
void DudeModel::applyForce(float h, float v) {
    if (!isEnabled()) {
        return;
    }

    // Don't want to be moving. Damp out player motion
    if (getMovement() == 0.0f || h*getVX()<=0 || fabs(getVX()) >= getMaxSpeed()) {
        if (isGrounded() && (_dashCooldown <= DASH_COOLDOWN * .1)) {
            // Instant friction on the ground
            b2Vec2 vel = _body->GetLinearVelocity();
            float LogVal = std::log(abs(vel.x) + 1);
            float whyDoesntSTDMinWorkpls = LogVal < .8 ? LogVal : .8;
            if (abs(getVX()) > 0) {
                int negativeAccounter = SIGNUM(vel.x);
                vel.x = negativeAccounter * vel.x * whyDoesntSTDMinWorkpls < negativeAccounter * .01 ? 0 : whyDoesntSTDMinWorkpls * vel.x; // If you set y, you will stop a jump in place
            }
            _body->SetLinearVelocity(vel);
        } else {
            // Damping factor in the air
            b2Vec2 force(-getDamping()*getVX(),0);
            _body->ApplyForce(force,_body->GetPosition(),true);
        }
    }
    
    // Velocity too high, clamp it
    b2Vec2 force(getMovement(),0);
    _body->ApplyForce(force,_body->GetPosition(),true);


    // Jump!

    if (isJumping() && isGrounded()) {
        setVY(0);
        b2Vec2 force(0, DUDE_JUMP);
        _body->ApplyLinearImpulse(force,_body->GetPosition(),true);
    }
    else if (isJumping() && contactingWall() && !isGrounded()) {
        setVY(0);
        b2Vec2 force(4 * DUDE_JUMP *5* (isFacingRight() ? 1: -1), DUDE_JUMP * 1.2);
        _body->ApplyLinearImpulse(force, _body->GetPosition(), true);
    }
    if (canDash() && getDashNum()>0) {
        //b2Vec2 force(DUDE_DASH*SIGNUM(h), DUDE_DASH * SIGNUM(v) * .8);
        b2Vec2 force(SIGNUM(h), SIGNUM(v) * .8);
        setVY(0);
        setVX(0);
        _body->ApplyLinearImpulse(DUDE_DASH * force, _body->GetPosition(), true);
        //deltaDashNum(-1);
    }
}

/**
 * Updates the object's physics state (NOT GAME LOGIC).
 *
 * We use this method to reset cooldowns.
 *
 * @param delta Number of seconds since last animation frame
 */
void DudeModel::update(float dt) {

    CapsuleObstacle::update(dt);

    if (_duration > 0) {
		_duration -= dt;
        _duration = std::max(0.0f, _duration);
        //reset buff state if duration is over
		if (_duration == 0) {
            resetBuff();
        }
        else {
            _node->setColor(Color4::BLACK);
        }
    }
    else if (_hasSuper) {
		_node->setColor(Color4::RED);
    } else {
		_node->setColor(Color4::WHITE);
	}


    if (_numberOfTouchingEnemies > 0) {
        CULog("Number of touching enemies: %f", _numberOfTouchingEnemies);
        takeDamage(34, 0);
    }

    if (_knockbackTime > 0) {
        if (int(_knockbackTime*10) % 2 <1) {
			_node->setVisible(true);
		}
        else {
			_node->setVisible(false);
		}
		_knockbackTime -= dt;
        if (_node != nullptr) {
            _node->setPosition(getPosition() * _drawScale);
            _node->setAngle(getAngle());
        }
        //return;
	}
    else {
        _lastDamageTime += dt;
	}

    if (_dashCooldown > DASH_COOLDOWN - floatyFrames) {
        setGravityScale(0);
    }
    else {
        setGravityScale(1);
    }

    // Apply cooldowns
    if (isJumping()) {
        _jumpCooldown = JUMP_COOLDOWN;
    } else {
        // Only cooldown while grounded
        _jumpCooldown = (_jumpCooldown > 0 ? _jumpCooldown-1 : 0);
    }
    
    if (isShooting()) {
        _shootCooldown = SHOOT_COOLDOWN;
    } else {
        _shootCooldown = (_shootCooldown > 0 ? _shootCooldown-1 : 0);
    }
    if (canDash() && _dashCooldown == 0) {
        _dashCooldown = DASH_COOLDOWN;
        deltaDashNum(-1);
    }
    else {
        _dashCooldown = (_dashCooldown > 0 ? _dashCooldown - 1 : 0);
        if (getDashNum() == 0 && _dashCooldown <= 0 && isGrounded()) {
            setDashNum(1);
            //TODO: remove hardcode limit on one dash
        }
    }
    

    
    if (_node != nullptr) {
        _node->setPosition(getPosition()*_drawScale);
        _node->setAngle(getAngle());
    }
}


#pragma mark -
#pragma mark Scene Graph Methods
/**
 * Redraws the outline of the physics fixtures to the debug node
 *
 * The debug node is use to outline the fixtures attached to this object.
 * This is very useful when the fixtures have a very different shape than
 * the texture (e.g. a circular shape attached to a square texture).
 */
void DudeModel::resetDebug() {
    CapsuleObstacle::resetDebug();
    float w = DUDE_SSHRINK*_dimension.width;
    float h = SENSOR_HEIGHT;
    Poly2 poly(Rect(-w/2.0f,-h/2.0f,w,h));

    _sensorNode = scene2::WireNode::allocWithTraversal(poly, poly2::Traversal::INTERIOR);
    _sensorNode->setColor(DEBUG_COLOR);
    _sensorNode->setPosition(Vec2(_debug->getContentSize().width/2.0f, 0.0f));


}


void DudeModel::takeDamage(float damage, const int attackDirection) {
    if (_lastDamageTime >= _healthCooldown) {
        _lastDamageTime = 0;
        _health -= damage * getDefenseBuff();
        if (_health < 0) {
            _health = 0;
        }
        else {
            b2Vec2 impulse = b2Vec2(attackDirection * 15, 10);
            /*_body->ApplyLinearImpulseToCenter(impulse, true);*/
            _body->SetLinearVelocity(impulse);
            _knockbackTime = 2;
        }
    }
}

bool DudeModel::useMeter(float f) {
    CULog("Meter: %f", _meter);
    if (_meter > f) {
        _meter -= f;
		return true;
    }
    else {
        return false;
    }
}


void DudeModel::applyBuff(const buff b, modifier m) {
    resetBuff();
    switch (b) {
    case buff::attack:
        if (m == modifier::duration) {
            _attackBuff = BASE_ATTACK_BUFF;
            _duration = BASE_DURATION;
            _hasSuper = false;
        }
        else {
            _attackBuff = SUPER_ATTACK_BUFF;
            _duration = 0;
            _hasSuper = true;
        }
        break;
    case buff::health:
        if (m == modifier::duration) {
            _healthBuff = BASE_HEALTH_BUFF;
            _duration = BASE_DURATION;
            _hasSuper = false;
        } else {
            _healthBuff = SUPER_HEALTH_BUFF;
            _duration = 0;
            _hasSuper = true;
        }
        break;
    case buff::jump:
        if (m == modifier::duration) {
            _jumpBuff = BASE_JUMP_BUFF;
            _duration = BASE_DURATION;
            _hasSuper = false;
        }
        else {
            _jumpBuff = SUPER_JUMP_BUFF;
            _duration = 0;
            _hasSuper = true;
        }
        break;
    case buff::speed:
        if (m == modifier::duration) {
            _speedBuff = BASE_SPEED_BUFF;
            _duration = BASE_DURATION;
            _hasSuper = false;
        } else {
			_speedBuff = SUPER_SPEED_BUFF;
			_duration = 0;
			_hasSuper = true;
		}
        break;
    case buff::defense:
        if (m == modifier::duration) {
            _defenseBuff = BASE_DEFENSE_BUFF;
            _duration = BASE_DURATION;
            _hasSuper = false;
        } else {
			_defenseBuff = SUPER_DEFENSE_BUFF;
			_duration = 0;
			_hasSuper = true;
		}
		break;
    default:
        CULog("NULL BUFF APPLIED");
    }
}


/**
 * Resets the buff to default values
 */
void DudeModel::resetBuff() {
	_attackBuff = DEFAULT_BUFF;
	_healthBuff = DEFAULT_BUFF;
	_jumpBuff = DEFAULT_BUFF;
	_defenseBuff = DEFAULT_BUFF;
	_speedBuff = DEFAULT_BUFF;
	_duration = 0;
    _hasSuper = false;
    _node->setColor(Color4::WHITE);
}

std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> DudeModel::createAttack(std::shared_ptr<cugl::AssetManager> _assets, float scale) {
    Vec2 pos = getPosition();
    pos.x += (_faceRight ? ATTACK_OFFSET_X : -ATTACK_OFFSET_X);
    pos.y += 0;

    std::shared_ptr<Texture> image = _assets->get<Texture>(ATTACK_TEXTURE_L);
    std::shared_ptr<Attack> attack = Attack::alloc(pos,
        cugl::Size(image->getSize().width * 1.5 / scale,
            ATTACK_H * image->getSize().height / scale));

    if (_faceRight) {
        attack->setFaceRight(true);
    }

    attack->setName("attack");
    attack->setGravityScale(0);
    attack->setDebugColor(DEBUG_COLOR);
    attack->setDrawScale(scale);
    attack->setDensity(10.0f);
    attack->setBullet(true);
    attack->setrand(false);
    attack->setShoot(false);
    attack->setnorotate(true);
   



    std::shared_ptr<scene2::PolygonNode> sprite = scene2::PolygonNode::allocWithTexture(image);
    attack->setSceneNode(sprite);
    sprite->setVisible(false);
    sprite->setPosition(pos);

    return std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>>(attack, sprite);
}