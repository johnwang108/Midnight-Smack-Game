#ifndef __LEVEL2_H__
#define __LEVEL2_H__
#include "Levels.h"




class Level2 : public Levels {
protected:
    std::shared_ptr<AssetManager> _assets;
    float _scale;
    std::shared_ptr<cugl::scene2::PolygonNode> _background;
    std::shared_ptr<DudeModel>			  _avatar;
    std::vector<std::shared_ptr<EnemyModel>> _enemies;
    std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;
    std::vector<std::shared_ptr<Wall>> _walls;
public:
    Level2() {

    }

    void populate(GameScene& scene) override;

};

#endif /* __LEVEL2_H__ */