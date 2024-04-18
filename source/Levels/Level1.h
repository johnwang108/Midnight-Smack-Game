#ifndef __LEVEL1_H__
#define __LEVEL1_H__
#include "Levels.h"
#include "../NightLevelObjects/Wall.h"


class Level1 : public Levels {
protected:
    std::shared_ptr<AssetManager> _assets;
    float _scale;
    std::shared_ptr<cugl::scene2::PolygonNode> _background;
    std::shared_ptr<DudeModel>			  _avatar;
    std::vector<std::shared_ptr<EnemyModel>> _enemies;
    std::shared_ptr<cugl::physics2::BoxObstacle> _goalDoor;
    
public:
    Level1() {

    }

    void parseJson(const std::string &json, std::shared_ptr<AssetManager>& assets, GameScene& scene);

    void createGoalDoor(std::string path, std::shared_ptr<AssetManager>& assets,
        GameScene& scene, int row, int col);

    void createEnemy(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col);

    void createDude(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col);

    void createBorder(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int borderNumber);

    void createPlatform(std::string path, std::shared_ptr<AssetManager>& assets, GameScene& scene, int row, int col, int platformNumber);

    void populate(GameScene& scene) override;
    void update(float step) override;
};

#endif /* __LEVEL1_H__ */