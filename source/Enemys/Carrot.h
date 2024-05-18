#ifndef __CARROT_H__
#define __CARROT_H__
#include "Enemy.h"

class Carrot : public EnemyModel {
protected:

private:

public:
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale);

    /**init with gesture sequences*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2);

    /**should not be used*/
    static std::shared_ptr<Carrot> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale) {
        std::shared_ptr<Carrot> result = std::make_shared<Carrot>();
        return (result->init(pos, size, scale) ? result : nullptr);
    }

    static std::shared_ptr<Carrot> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets) {
        std::shared_ptr<Carrot> result = std::make_shared<Carrot>();
        bool res = result->init(pos, size, scale);

        if (res) {
            result->loadAnimationsFromConstant("carrot", _assets);
        }

        //manually add respawn (death reversed)
        auto info = result->getInfo("carrotDeath");
        result->addActionAnimation("carrotRespawn", _assets->get<Texture>("carrotDeath"), std::get<0>(info), std::get<1>(info), std::get<2>(info), std::get<3>(info) * 8.0f, true);
        return res ? result : nullptr;
    }

    void markForDeletion() override {
        if (_killMeCountdown != 0.0f) return;
        EnemyModel::markForDeletion();
        _killMeCountdown = getActionDuration("carrotDeath");
    }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    void setState(std::string state) override;

    b2Vec2 handleMovement(b2Vec2 velocity) override;

    std::string getNextState(std::string state) override;

};

#endif /*__CARROT_H__*/