#ifndef __BEEF_H__
#define __BEEF_H__
#include "Enemy.h"

#define BEEF_BURROW_TIME 30.0f
#define BEEF_UNBURROW_TIME 30.0f
#define BEEF_ATTACK_TIME 20.0f
#define BEEF_SPEED 3.0f

class Beef : public EnemyModel {
protected:
    Spline2 _limit;

    Path2 _boundaries;

    Path2 _centerBoundary;

    std::shared_ptr<EntitySpriteNode> _dirtPile;
private:
public:

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, cugl::Spline2 limit);
    /**init with gesture sequences*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2);

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, cugl::Spline2 limit);


    /**should not be used*/
    static std::shared_ptr<Beef> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Beef> result = std::make_shared<Beef>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Beef> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets) {
        std::shared_ptr<Beef> result = std::make_shared<Beef>();
        bool res = result->init(pos, size, scale);

        if (res) {
            result->loadAnimationsFromConstant("beef", _assets);
        }
        return res ? result : nullptr;
    }

    static std::shared_ptr<Beef> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets, Spline2 limit) {
        std::shared_ptr<Beef> result = std::make_shared<Beef>();
        bool res = result->init(pos, size, scale, limit);

        if (res) {
            result->loadAnimationsFromConstant("beef", _assets);
        }
        return res ? result : nullptr;
    }

    void setLimit(cugl::Spline2 limit) { 
        _limit = limit; 
        _boundaries = cugl::SplinePather(&_limit).getPath();
    }

    Spline2 getLimit() { return _limit; }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    b2Vec2 handleMovement(b2Vec2 velocity) override;

    void setState(std::string state) override;

    std::string getNextState(std::string state) override;
    
};

#endif /*__BEEF_H__*/