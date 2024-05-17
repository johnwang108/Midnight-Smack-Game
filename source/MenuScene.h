
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

    /* Menu wants to reset the scene */
    bool _reset;

    bool _transitionScenes;

    std::string _targetScene;

    std::vector<std::shared_ptr<cugl::scene2::Button>> _buttons;

    int _selectedLevel;

    int _highestLevel;

    float _musicVolume;
    float _sfxVolume;

    std::string _transitionedFrom;

private:

    void initMainMenu(cugl::Size);

    void initPauseMenu(cugl::Size);

    void initLevelSelectMenu(cugl::Size);

    void initSettingsMenu(cugl::Size);

    void initLoseMenu(cugl::Size);

public:

    MenuScene() : Scene2() { }

    ~MenuScene() { dispose(); }

    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets, std::string id);

    void update(float timestep) override;

    bool started() { return _started; };

    void setStarted(bool b) { _started = b; };

    bool didTransition() { return _transitionScenes; };

    void setTransition(bool b) { _transitionScenes = b; };

    std::string getTarget() { return _targetScene; };

    void setTarget(std::string s) { _targetScene = s; };

    void setActive(bool b);
    void setReset(bool b) { _reset = b; };
    bool getReset() { return _reset; };

    void setSelectedLevel(int i) { _selectedLevel = i;}
    int getSelectedLevel() { return _selectedLevel;}

    void setHighestLevel(int i);

    void setTransitionedFrom(std::string sceneName) { _transitionedFrom = sceneName; };

    std::string getTransitionedFrom() { return _transitionedFrom; }

    void reset();
};

#endif /* __MENU_SCENE__ */