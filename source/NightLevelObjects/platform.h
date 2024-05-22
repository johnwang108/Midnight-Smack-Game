#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include <cugl/cugl.h>


using namespace cugl;

class Platform : public physics2::PolygonObstacle {
private:


protected:
	
    std::string _sensorName;
    b2Fixture* _sensorFixture;

public:
    
    static std::shared_ptr<Platform> alloc(const Poly2& poly) {
        std::shared_ptr<Platform> result = std::make_shared<Platform>();
        return (result->init(poly) ? result : nullptr);
    }

    void createFixtures() override;


    void releaseFixtures() override;

    std::string* getSensorName() { return &_sensorName; }
    
};

#endif /* __PLATFORM_H__ */
