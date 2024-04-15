#ifndef __ENTITY_SPRITE_NODE_H__
#define __ENTITY_SPRITE_NODE_H__

#include <cugl/cugl.h>

//This class is almost exactly the same as cugl::scene2::SpriteNode, but it has a changeSheet method that allows you to change the texture of the sprite
class EntitySpriteNode : public cugl::scene2::SpriteNode {
protected:
private:
public:

    static std::shared_ptr<EntitySpriteNode> allocWithSheet(const std::shared_ptr<cugl::Texture>& texture,
        int rows, int cols, int size) {
        std::shared_ptr<EntitySpriteNode> node = std::make_shared<EntitySpriteNode>();
        return (node->initWithSheet(texture, rows, cols, size) ? node : nullptr);
    }

	void changeSheet(std::shared_ptr<cugl::Texture> texture, int rows, int cols, int size) {
        _cols = cols;
        _size = size;
        _bounds.size = texture->getSize();
        _bounds.size.width /= cols;
        _bounds.size.height /= rows;
        setPolygon(_bounds);
        setTexture(texture);
        setFrame(0);
	}

    void setTransform(cugl::Affine2 transform) {
		_combined = transform;
	}
};

#endif /* __ENTITY_SPRITE_NODE_H__ */