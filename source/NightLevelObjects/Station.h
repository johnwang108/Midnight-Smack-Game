#ifndef __STATION_H__
#define __STATION_H__

#include <cugl/cugl.h>
#include "GestureInteractable.h"
using namespace cugl;

class Station : public GestureInteractable {
private:

protected:

public:
	bool init(const std::shared_ptr<Texture>& texture, const cugl::Vec2& pos, const cugl::Size& size, float scale);
};

#endif /* __STATION_H__ */