#ifndef __SHRIMP_H__
#define __SHRIMP_H__
#include "Enemy.h"

#define SHRIMP_SPEED 3.0f

class Shrimp : public EnemyModel {
protected:

private:

public:
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    /**init with gesture sequences*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2);

    /**should not be used*/
    static std::shared_ptr<Shrimp> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Shrimp> result = std::make_shared<Shrimp>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Shrimp> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets) {
        std::shared_ptr<Shrimp> result = std::make_shared<Shrimp>();
        bool res = result->init(pos, size, scale);

        if (res) {
            result->loadAnimationsFromConstant("shrimp", _assets);
        }

        //result->addActionAnimation("shrimpRoll", _assets->get<Texture>("shrimpRollToIdle"), 2, 3, 6, 0.1f);
        //result->setAction("shrimpRoll", { 1 }, 0.1f);

        auto info = result->getInfo("shrimpIdleDeath");
        result->addActionAnimation("shrimpRespawn", _assets->get<Texture>("shrimpIdleDeath"), std::get<0>(info), std::get<1>(info), std::get<2>(info), std::get<3>(info) * 8.0f, true);
        return res ? result : nullptr;
    }

    void markForDeletion() override {
        if (_killMeCountdown != 0.0f) return;
        EnemyModel::markForDeletion();
        _killMeCountdown = getActionDuration("shrimpStandDeath");
    }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    b2Vec2 handleMovement(b2Vec2 velocity) override;

    void setState(std::string state) override;

    std::string getNextState(std::string state) override;
};

#endif /*__SHRIMP_H__*/