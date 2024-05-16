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
	pathNodeCoolDown = -1;

	ogX = 0;
	ogY = 0;

	breakableCoolDown = -1;
	respawnTime = -1;
	breakingClock = -1;
	activeDisplay = true;
    

}

bool Wall::init(std::shared_ptr<Texture> image, std::shared_ptr<PolygonObstacle> _collisionPoly, float _scale, float BASIC_DENSITY, float BASIC_FRICTION,
	float BASIC_RESTITUTION, Color4 DEBUG_COLOR, Vec2* WALL_POS, int WALL_VERTS, std::string name, bool doesDamage) {
	
	// this was all being done in Wall init, but instead we will do this within levelmodel
	//Poly2 _collisionPoly(WALL_POS, WALL_VERTS / 2);
	//// Call this on a polygon to get a solid shape
	//EarclipTriangulator triangulator;
	//triangulator.set(_collisionPoly.vertices);
	//triangulator.calculate();
	//_collisionPoly.setIndices(triangulator.getTriangulation());
	//triangulator.clear();

	CULog("we are in wall init method right now");


	PolygonObstacle::init(_collisionPoly->getPolygon(), Vec2(WALL_POS->x, WALL_POS->y));
	// we called init of PolygonObstacle
	_obj = _collisionPoly;
	// You cannot add constant "".  Must stringify

	// Set the physics attributes
	// _obj->setBodyType(b2_staticBody);
	// _obj->setDensity(BASIC_DENSITY);
	// _obj->setFriction(BASIC_FRICTION);
	// _obj->setRestitution(BASIC_RESTITUTION);
	// _obj->setDebugColor(DEBUG_COLOR);

	// _collisionPoly *= _scale;
	// sprite = scene2::PolygonNode::allocWithTexture(image, _collisionPoly);



	this->_obj = _obj;

	// I added this, does it work? Idk
	// this->_collisionPoly = _collisionPoly->getPolygon();
	//this->_collisionPoly = Poly2 collisionPoly(reinterpret_cast<Vec2*>(WALL[ii]), WALL_VERTS / 2);

	this->image = image;

	this->_scale = _scale;
	// we are setting flag to false because avatar has not touched it
	// at initialization
	this->setFlag(false);

	this->BASIC_DENSITY = BASIC_DENSITY;
	this->BASIC_FRICTION = BASIC_FRICTION;
	this->BASIC_RESTITUTION = BASIC_RESTITUTION;
	this->DEBUG_COLOR = DEBUG_COLOR;
	this->WALL_POS = WALL_POS;
	this->WALL_VERTS = WALL_VERTS;

	ogX = _obj->getX();
	ogY = _obj->getY();

	this->doesDamage = doesDamage;
	this->name = name + (doesDamage ? "dd" : "");
	_obj->setName(this->name);
	this->breakableCoolDown = -1;
	this->respawnTime = -1;
	this->breakingClock = -1;
	return true;
}

std::shared_ptr<Wall> Wall::alloc(std::shared_ptr<Texture> image, std::shared_ptr<physics2::PolygonObstacle> _collisionPoly, float _scale, float BASIC_DENSITY, float BASIC_FRICTION,
	float BASIC_RESTITUTION, Color4 DEBUG_COLOR, Vec2* WALL_POS, int WALL_VERTS, std::string name, bool doesDamage) {
	std::shared_ptr<Wall> result = std::make_shared<Wall>();
	return (result->init(image, _collisionPoly, _scale, BASIC_DENSITY, BASIC_FRICTION, BASIC_RESTITUTION, DEBUG_COLOR, WALL_POS, WALL_VERTS, name, doesDamage) ? result : nullptr);
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

void Wall::initBreakable(int duration, int respawnTime)
{
	this->breakableCoolDown = duration;
	this->respawnTime = respawnTime;
	this->breakingClock = this->breakableCoolDown;
	name = name + "breakable";
	this->_obj->setName(name);
}

void Wall::applyBreaking() {
	this->breakingClock--;
	//CULog("%f", breakingClock);
	CULog("we in applyBreaking");
	CULog(std::to_string(this->breakingClock).c_str());
	if (activeDisplay) {
		if (this->breakingClock == 0) {
			this->breakingClock = this->respawnTime;
			this->setActive(false);
		}
	}
	else {
		if (this->breakingClock == 0) {
			this->breakingClock = this->breakableCoolDown;
			this->setActive(true);
		}
	}
}

void Wall::resetBreaking() {
	breakingClock = breakableCoolDown;
}

int Wall::getBreakingClock() {
	return breakingClock;
}

void Wall::setActive(bool state) {
	CULog("we in setActive now");
	activeDisplay = state;
	CULog("%d", state);
	if (state) {
		CULog("1");
		// setEnabled(true);
		this->setSensor(true);
	}
	else {
		CULog("2");
		CULog("we want our platform to no longer have collision properties now");
		// setEnabled(false);
		this->setSensor(false);
		this->setFlag(false);
		//image->dispose();
	}
}

void Wall::initPath(std::vector<Vec3> paath, int movementForce)
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

void Wall::fixedUpdate(float step) {
	applyBreaking();
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
	// this was given us a nullptr error with the setPosition of image
	// this is because we commented out sprite previously, will have to add back in
	// image->setPositionX(this->_obj->getX());
	// image->setPositionY(this->_obj->getY());
	// image->setPosition(this->_obj->getPosition() * _scale);

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
