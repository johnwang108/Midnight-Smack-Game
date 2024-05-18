#include "EntitySpriteNode.h"

void EntitySpriteNode::changeSheet(std::shared_ptr<cugl::Texture> texture, int rows, int cols, int size) {
    Vec2 pos = getPosition();
    this->_cols = cols;
    this->_size = size;
    //Rect oldBounds = _bounds;
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
    _bounds.origin.x = (_frame % _cols) * _bounds.size.width;
    _bounds.origin.y = _texture->getSize().height - (1 + _frame / _cols) * _bounds.size.height;
    Vec2 coord = getPosition();
    setPolygon(_bounds);
    setPosition(coord);
}