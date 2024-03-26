#ifndef __LEVELS_H__
#define __LEVELS_H__

#include <cugl/cugl.h>
#include "../PFInput.h"
#include "../PFAttack.h"
#include "../PFDudeModel.h"
#include "../Enemys/Enemy.h"
#include "../Enemys/Bull.h"
#include "../NightLevelObjects/Wall.h"


#pragma mark -
#pragma mark Physics Constants
/** The new heavier gravity for this world (so it is not so floaty) */
#define DEFAULT_GRAVITY -28.9f
/** The density for most physics objects */
#define BASIC_DENSITY   0.0f
/** The density for a bullet */
#define HEAVY_DENSITY   10.0f
/** Friction of most platforms */
#define BASIC_FRICTION  0.4f
/** The restitution for all physics objects */
#define BASIC_RESTITUTION   0.1f
/** The width of the rope bridge */
#define BRIDGE_WIDTH    14.0f
/** Offset for bullet when firing */
#define BULLET_OFFSET   0.5f
/** Offset for attack when firing, hacky */
#define ATTACK_OFFSET_H   1.0f
/** Offset for attack when firing, hacky*/
#define ATTACK_OFFSET_V   0.0f
/**Scalar for width of a box attack, hacky*/
#define ATTACK_W        2.0f
/**Scalar for height of a box attack, hacky*/
#define ATTACK_H        0.5f
/** The speed of the bullet after firing */
#define BULLET_SPEED   20.0f
/** The number of frame to wait before reinitializing the game */
#define EXIT_COUNT      240


#pragma mark -
#pragma mark Asset Constants
/** The key for the earth texture in the asset manager */
#define EARTH_TEXTURE   "earth"
/** The key for the win door texture in the asset manager */
#define GOAL_TEXTURE    "goal"
/** The key for the win door texture in the asset manager */
#define BULLET_TEXTURE  "bullet"
/** The keys for the attack texture in asset manager*/
#define ATTACK_TEXTURE_R  "attack_r"
#define ATTACK_TEXTURE_L  "attack_l"
#define SHAKE_TEXTURE  "shake"
/** The name of a bullet (for object identification) */
#define ATTACK_NAME     "attack"
/** The name of a wall (for object identification) */
#define WALL_NAME       "wall"
/** The name of an enemy for object id */
#define ENEMY_NAME	    "enemy"

/** The name of a platform (for object identification) */
#define PLATFORM_NAME   "platform"

#define BACKGROUND_NAME "background"
/** The font for victory/failure messages */
#define MESSAGE_FONT    "retro"

#define SMALL_MSG "retrosmall"  
/** The message for winning the game */
#define WIN_MESSAGE     "VICTORY!"
/** The color of the win message */
#define WIN_COLOR       Color4::YELLOW
/** The message for losing the game */
#define LOSE_MESSAGE    "FAILURE!"
/** The color of the lose message */
#define LOSE_COLOR      Color4::RED
/** The key the basic game music */
#define GAME_MUSIC      "game"
/** The key the victory game music */
#define WIN_MUSIC       "win"
/** The key the failure game music */
#define LOSE_MUSIC      "lose"
/** The sound effect for firing a bullet */
#define PEW_EFFECT      "pew"
/** The sound effect for a bullet collision */
#define POP_EFFECT      "pop"
/** The sound effect for jumping */
#define JUMP_EFFECT     "jump"
/** The volume for the music */
#define MUSIC_VOLUME    0.7f
/** The volume for sound effects */
#define EFFECT_VOLUME   0.8f
/** The image for the left dpad/joystick */
#define LEFT_IMAGE      "dpad_left"
/** The image for the right dpad/joystick */
#define RIGHT_IMAGE     "dpad_right"

#define BULL_TEXTURE    "bull"

/** Color to outline the physics nodes */
#define DEBUG_COLOR     Color4::YELLOW
/** Opacity of the physics outlines */
#define DEBUG_OPACITY   192
/** The number of platforms */
#define ALT_PLATFORM_VERTS  8
#define ALT_PLATFORM_COUNT  1

/** The number of platforms */
#define PLATFORM_VERTS  8
#define PLATFORM_COUNT  10

// Since these appear only once, we do not care about the magic numbers.
// In an actual game, this information would go in a data file.
// IMPORTANT: Note that Box2D units do not equal drawing units
/** The wall vertices */
#define WALL_VERTS 12
#define WALL_COUNT  2


class GameScene;

class Levels {
public:
    virtual ~Levels() = default;

    virtual void populate(GameScene& scene) = 0;

};

#endif /* __LEVELS_H__ */