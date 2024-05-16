#ifndef __RICE_H__
#define __RICE_H__
#include "Enemy.h"

#define CLOSE_ENOUGH 1.0f + ((float)rand() / ((float)RAND_MAX))/2.0f
#define RICE_KILL_COUNTDOWN 0.5f

class Rice : public EnemyModel {
protected:

    bool _isSoldier;
    cugl::Vec2 _targetPosition;
    float _closeEnough;
    std::vector<std::shared_ptr<Rice>> _soldiers;
    float _force;

private:

public:

    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, bool isSoldier = true);

    /**init with gesture sequences*/
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::vector<std::string> seq1, std::vector<std::string> seq2, bool isSoldier = true);


    /**
     * Creates and returns a new enemy at the given position with the specified size and type.
     *
     * @param pos   Initial position in world coordinates
     * @param size  The size of the enemy in world units
     * @param scale The drawing scale (world to screen)
     * @param type  The type of the enemy
     *
     * @return A newly allocated EnemyModel at the given position with the given size, scale, and type
     */
     /**should not be used*/
    static std::shared_ptr<Rice> alloc(const cugl::Vec2& pos, const cugl::Size& size, float scale, bool isSoldier) {
        std::shared_ptr<Rice> result = std::make_shared<Rice>();
        return (result->init(pos, size, scale, isSoldier) ? result : nullptr);
    }



    /**Allocs with animations defined from json.
    *
    * */
    static std::shared_ptr<Rice> allocWithConstants(const cugl::Vec2& pos, const cugl::Size& size, float scale, std::shared_ptr<AssetManager> _assets, bool isSoldier) {
        std::shared_ptr<Rice> result = std::make_shared<Rice>();
        bool res = result->init(pos, size, scale, isSoldier);

        if (res) {
            result->loadAnimationsFromConstant("rice", _assets);
        }

        //manually add respawn (death reversed)
        auto info = result->getInfo("riceDeath");
        result->addActionAnimation("riceRespawn", _assets->get<Texture>("riceDeath"), std::get<0>(info), std::get<1>(info), std::get<2>(info), std::get<3>(info) * 4.0f, true);
        return res ? result : nullptr;
    }

    /**Sets the target location to move to for rice soldiers */
    void setTargetPosition(cugl::Vec2 target) { _targetPosition = target; }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    void setState(std::string state) override;

    std::string getNextState(std::string state) override;

    /**Adds a soldier to the rice leader's squad*/
    void addSoldier(std::shared_ptr<Rice> soldier) {
        assert(soldier->_isSoldier);
		_soldiers.push_back(soldier);
	}

    std::vector<std::shared_ptr<Rice>> getSoldiers() {
		return _soldiers;
	}

    void markForDeletion() override {
        if (_killMeCountdown != 0.0f) return;
        EnemyModel::markForDeletion();
        for (auto& soldier : _soldiers) {
			soldier = nullptr;
		}
        _soldiers.clear();
        _killMeCountdown = getActionDuration("riceDeath");
    }

    void leaderListener(physics2::Obstacle* obs) {
        _node->setPosition(obs->getPosition() * _drawScale);
        _node->setAngle(obs->getAngle());
        if (getState() == "pursuing") {
            for (auto& soldier : _soldiers) {
                soldier->setTargetPosition(getPosition() + _distanceToPlayer);
            };
        }
        else {
            for (auto& soldier : _soldiers) {
                soldier->setTargetPosition(getPosition());
            };
        }
    }
};

#endif /*__RICE_H__*/