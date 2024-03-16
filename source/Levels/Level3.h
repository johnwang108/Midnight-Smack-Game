#ifndef __LEVEL3_H__
#define __LEVEL3_H__
#include "Levels.h"




class Level3 : public Levels {
protected:
    std::shared_ptr<AssetManager> _assets;
    float _scale;
    std::shared_ptr<cugl::physics2::BoxObstacle> _background;
    std::shared_ptr<DudeModel>			  _avatar;
    std::vector<std::shared_ptr<EnemyModel>> _enemies;
    std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;

public:
    Level3() {

    }

    void populate(GameScene& scene) override;
};

#endif /* __LEVEL3_H__ */