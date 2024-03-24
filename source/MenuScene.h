
#ifndef __MENU_SCENE__
#define __MENU_SCENE__

#include <cugl/cugl.h>

//This class is the base evel class for all menu scenes, including main menu and level select. It is a subclass of Scene2.
class MenuScene : public cugl::Scene2 {
protected:
    /** The asset manager for loading. */
    std::shared_ptr<cugl::AssetManager> _assets;

    //main node
    bool _isFullscreen;

    std::shared_ptr<cugl::scene2::SceneNode> _rootNode;

    cugl::Size computeActiveSize() const;

    bool _started;

private:

public:

    MenuScene() : Scene2() { }

    ~MenuScene() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::string id);

    void update(float timestep) override;

    bool started() { return _started; };

    void setStarted(bool b) { _started = b; };
};

#endif /* __MENU_SCENE__ */