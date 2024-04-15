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
	movementForce = 0;
	currentPathNode = 0;
	path.push_back(Vec3(-1, -1, -1));
	float pathNodeCoolDown = -1;

	ogX = 0;
	ogY = 0;
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

	ogX = _obj->getX();
	ogY = _obj->getY();

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

void Wall::initiatePath(std::vector<Vec3> paath, int movementForce)
{
	pathNodeCoolDown = -1;
	std::vector<Vec3> temp;
	for (Vec3 pathNode : paath) {
		pathNode.set(pathNode.x / _scale + getOGX(), pathNode.y / _scale + getOGY(), pathNode.z);
		temp.push_back(pathNode);
	}
	path.clear();
	path = temp;
	this->movementForce = movementForce;
}

void Wall::update(float dt) {
	applyPathMovement(dt);
}

Vec3 Wall::queryPath(int temp) {
	return this->path[temp];
}

void Wall::applyPathMovement(float step) {
	Vec3 target = this->queryPath(currentPathNode);
	Vec3 pos = Vec3(_obj->getX(), _obj->getY(), 0);

	float distanceX = std::min(movementForce * step, std::abs(target.x - pos.x));
	float distanceY = std::min(movementForce * step, std::abs(target.y - pos.y));

	if (pos.x < target.x) {
		pos.x += distanceX;
	}
	else if (pos.x > target.x) {
		pos.x -= distanceX;
	}
	if (pos.y < target.y) {
		pos.y += distanceY;
	}
	else if (pos.y > target.y) {
		pos.y -= distanceY;
	}

	_obj->setPosition(pos.x, pos.y);

	scene2::TexturedNode* image = dynamic_cast<scene2::TexturedNode*>(sprite.get());
	image->setPositionX(this->_obj->getX());
	image->setPositionY(this->_obj->getY());
	image->setPosition(this->_obj->getPosition() * _scale);

	if (std::abs(pos.x - target.x) < 0.1 && std::abs(pos.y - target.y) < 0.1) {
		if (pathNodeCoolDown == -1) {
			pathNodeCoolDown = target.z;
		}
		else {
			if (pathNodeCoolDown > 0) {
				pathNodeCoolDown--;
			}
			else {
				currentPathNode = (currentPathNode + 1) % path.size();
				pathNodeCoolDown = -1;
			}

		}

	}
}


float Wall::getOGX()
{
	return ogX;
}
float Wall::getOGY()
{
	return ogY;
}


//Wall::Wall() {};