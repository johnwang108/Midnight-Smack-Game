#ifndef __SHRIMP_H__
#define __SHRIMP_H__
#include "Enemy.h"

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
        return res ? result : nullptr;
    }

    void markForDeletion() override {
        if (_killMeCountdown != 0.0f) return;
        EnemyModel::markForDeletion();
        _killMeCountdown = 0.1;
    }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    void setState(std::string state) override;

    std::string getNextState(std::string state) override;
};

#endif /*__SHRIMP_H__*/