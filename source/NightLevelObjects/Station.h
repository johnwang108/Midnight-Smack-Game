#ifndef __STATION_H__
#define __STATION_H__

#include <cugl/cugl.h>
#include "GestureInteractable.h"
using namespace cugl;

enum class StationType {
	CUT,
	FRY,
	BOIL,
};

namespace std {
	template <> struct hash<StationType> {
		size_t operator() (const StationType& t) const { return size_t(t); }
	};
}

class Station : public GestureInteractable {
private:
	std::shared_ptr<Ingredient> _ingredient;

	StationType _type;

protected:

public:
	bool init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size);
	
	static std::shared_ptr<Station> alloc(const std::shared_ptr<Texture>& texture, const Vec2& pos, const Size& s, StationType type) {
		std::shared_ptr<Station> result = std::make_shared<Station>();
		bool res = result->init(texture, pos, s);
		if (!res) return nullptr;
		result->setType(type);
		return result;
	}

	bool interact(IngredientType t) override;

	void setType(StationType type) {
		_type = type;
		std::string s;
		switch (type) {
			case StationType::CUT:
			{
				s = "cut";
				break;
			}
			case StationType::FRY:
			{
				s = "pan";
				break;
			}
			case StationType::BOIL:
			{
				s = "pot";
				break;
			}
			default: {
				s = "";
				break;
			}
		}
		setName(getName() + "_" + s);
	}

	void setIngredientPtr(std::shared_ptr<Ingredient>);

	std::shared_ptr<Ingredient> getIngredientPtr() {
		return _ingredient;
	}

	IngredientType getCookedType(IngredientType t);

	void hit() override;
};

#endif /* __STATION_H__ */