#ifndef __ENTITY_SPRITE_NODE_H__
#define __ENTITY_SPRITE_NODE_H__

#include <cugl/cugl.h>

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
        setFrame(1);
	}
};

#endif /* __ENTITY_SPRITE_NODE_H__ */