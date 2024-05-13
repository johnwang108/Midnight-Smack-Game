#ifndef __EGG_H__
#define __EGG_H__
#include "Enemy.h"


#define EGG_ATTACK_SPEEDUP_MULTIPLIER 0.5f
class Egg : public EnemyModel {
protected:
    Spline2 _limit;
public:

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, cugl::Spline2 limit);
    /**init with gesture sequences*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2);

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, cugl::Spline2 limit);


    /**should not be used*/
    static std::shared_ptr<Egg> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Egg> result = std::make_shared<Egg>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Egg> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets) {
        std::shared_ptr<Egg> result = std::make_shared<Egg>();
        bool res = result->init(pos, size, scale);

        if (res) {
            result->loadAnimationsFromConstant("egg", _assets);
        }
        return res ? result : nullptr;
    }

    static std::shared_ptr<Egg> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets, Spline2 limit) {
        std::shared_ptr<Egg> result = std::make_shared<Egg>();
        bool res = result->init(pos, size, scale, limit);

        if (res) {
            result->loadAnimationsFromConstant("egg", _assets);
        }
        return res ? result : nullptr;
    }

    void markForDeletion() override {
        if (_killMeCountdown != 0.0f) return;
        EnemyModel::markForDeletion();
        _killMeCountdown = getActionDuration("eggDeath");
    }

    std::tuple<std::shared_ptr<Attack>, std::shared_ptr<scene2::PolygonNode>> createAttack(std::shared_ptr<AssetManager> _assets, float scale) override;

    void setLimit(cugl::Spline2 limit) { _limit = limit; }

    Spline2 getLimit() { return _limit; }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    void setState(std::string state) override;

    b2Vec2 handleMovement(b2Vec2 vel) override;

    std::string getNextState(std::string state) override;

private:

};

#endif /*__EGG_H__*/