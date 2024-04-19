#ifndef __BEEF_H__
#define __BEEF_H__
#include "Enemy.h"

#define BEEF_BURROW_TIME 30.0f
#define BEEF_UNBURROW_TIME 30.0f
#define BEEF_ATTACK_TIME 30.0f
#define BEEF_SPEED 3.0f

class Beef : public EnemyModel {
protected:
    Spline2 _limit;

    Path2 _pathLimit;

    cugl::Vec2 _originalPos;

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

    void setLimit(cugl::Spline2 limit) { _limit = limit; }

    Spline2 getLimit() { return _limit; }

    void update(float dt) override;

    void fixedUpdate(float step) override;

    b2Vec2 handleMovement(b2Vec2 velocity) override;

    void setState(std::string state) override;

    std::string getNextState(std::string state) override;

    //linear projection of a point onto the internal pathLimit
  //  cugl::Vec2 projectOntoPath(Vec2 point) {
  //      std::vector<Vec2> vertices = _pathLimit.getVertices();
  //      Vec2 bestProj;

  //      for (int i = 0; i < vertices.size(); i++) {
		//	Vec2 a = vertices[i];
		//	Vec2 b = vertices[(i + 1) % vertices.size()];

		//	Vec2 ab = b - a;
		//	Vec2 ap = point - a;

		//	float t = ap.dot(ab) / ab.dot(ab);

  //          Vec2 proj;
  //          if (t < 0) {
		//		proj = a;
		//	}
  //          else if (t > 1) {
		//		proj = b;
		//	}
  //          else {
		//		proj = a + t * ab;
		//	}
  //          if (i == 0) {
		//		bestProj = proj;
		//	}
  //          else {
  //              if ((point - proj).length() < (point - bestProj).length()) {
		//			bestProj = proj;
		//		}
		//	}
		//}

  //      return bestProj;

  // }
    
};

#endif /*__BEEF_H__*/