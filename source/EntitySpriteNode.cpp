#include "EntitySpriteNode.h"

void EntitySpriteNode::changeSheet(std::shared_ptr<cugl::Texture> texture, int rows, int cols, int size) {
    Vec2 pos = getPosition();
    _cols = cols;
    _size = size;
    Rect oldBounds = _bounds;
    _bounds.size = texture->getSize();
    _bounds.size.width /= cols;
    _bounds.size.height /= rows;
    setPolygon(_bounds);
    setTexture(texture);
    setFrame(0);
    setPosition(pos);
}

void EntitySpriteNode::setTransform(cugl::Affine2 transform) {
    _combined = transform;
}

void EntitySpriteNode::setFrame(int frame) {
	int row = frame / _cols;
	int col = frame % _cols;
	_frame = frame;
	_bounds.origin.x = col * _size;
	_bounds.origin.y = row * _size;
	setPolygon(_bounds);
}