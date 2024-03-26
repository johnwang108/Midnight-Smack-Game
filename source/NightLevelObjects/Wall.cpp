#include "Wall.h"

using namespace cugl;

Wall::Wall() {
	image = nullptr;
	_scale = 0.0;
	BASIC_DENSITY = 0.0;
	BASIC_FRICTION = 0.0;
	BASIC_RESTITUTION = 0.0,
	DEBUG_COLOR = Color4::YELLOW;
	WALL_POS = nullptr;
	WALL_VERTS = 1;
	name = "";
	breakableCoolDown = 0;
	doesDamage = false;
}

bool Wall::init(std::shared_ptr<Texture> image, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, 
	float BASIC_RESTITUTION, Color4 DEBUG_COLOR, Vec2* WALL_POS, int WALL_VERTS, std::string name,
	int breakableCooldown, bool doesDamage) {
	Poly2 _collisionPoly(WALL_POS, WALL_VERTS / 2);
	// Call this on a polygon to get a solid shape
	EarclipTriangulator triangulator;
	triangulator.set(_collisionPoly.vertices);
	triangulator.calculate();
	_collisionPoly.setIndices(triangulator.getTriangulation());
	triangulator.clear();

	_obj = physics2::PolygonObstacle::allocWithAnchor(_collisionPoly, Vec2::ANCHOR_CENTER);
	// You cannot add constant "".  Must stringify

	// Set the physics attributes
	_obj->setBodyType(b2_staticBody);
	_obj->setDensity(BASIC_DENSITY);
	_obj->setFriction(BASIC_FRICTION);
	_obj->setRestitution(BASIC_RESTITUTION);
	_obj->setDebugColor(DEBUG_COLOR);

	_collisionPoly *= _scale;
	sprite = scene2::PolygonNode::allocWithTexture(image, _collisionPoly);



	this->_obj = _obj;

	this->_collisionPoly = _collisionPoly;
	//this->_collisionPoly = Poly2 collisionPoly(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);

	this->image = image;

	this->_scale = _scale;

	this->BASIC_DENSITY = BASIC_DENSITY;
	this->BASIC_FRICTION = BASIC_FRICTION;
	this->BASIC_RESTITUTION = BASIC_RESTITUTION;
	this->DEBUG_COLOR = DEBUG_COLOR;
	this->WALL_POS = WALL_POS;
	this->WALL_VERTS = WALL_VERTS;


	this->breakableCoolDown = breakableCoolDown;
	this->doesDamage = doesDamage;
	this->name = name + (doesDamage ? "dd" : "");
	this->name = name + (breakableCoolDown > -1 ? "breaks" : "");
	_obj->setName(this->name);
	return true;
}

std::shared_ptr<Wall> Wall::alloc(std::shared_ptr<Texture> image, float _scale, float BASIC_DENSITY, float BASIC_FRICTION, 
	float BASIC_RESTITUTION, Color4 DEBUG_COLOR, Vec2* WALL_POS, int WALL_VERTS, std::string name, int breakableCooldown, bool doesDamage) {
	std::shared_ptr<Wall> result = std::make_shared<Wall>();
	return (result->init(image, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, DEBUG_COLOR, WALL_POS, WALL_VERTS, name, 
		breakableCooldown, doesDamage) ? result : nullptr);
}

std::shared_ptr<physics2::PolygonObstacle> Wall::getObj()
{
	return this->_obj;
}

Poly2 Wall::getCollisionPoly()
{
	return this->_collisionPoly;
}

std::shared_ptr<scene2::SceneNode> Wall::getSprite()
{
	return this->sprite;
}


//Wall::Wall() {};