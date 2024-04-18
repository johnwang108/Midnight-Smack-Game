#include "MenuScene.h"


using namespace cugl;

#define SCENE_SIZE 1024

void MenuScene::dispose() {
	_assets = nullptr;
}

enum class MenuType {
	MAIN_MENU,
	LEVEL_SELECT,
	OPTIONS,
	PAUSE,
};

MenuType strToMenuType(std::string str) {
	if (str == "main") {
		return MenuType::MAIN_MENU;
	}
	else if (str == "level") {
		return MenuType::LEVEL_SELECT;
	}
	else if (str == "options") {
		return MenuType::OPTIONS;
	}
	else if (str == "pause") {
		return MenuType::PAUSE;
	}
	else {
		return MenuType::MAIN_MENU;
	}
}

bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets, std::string id) {
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    if (dimen.width > dimen.height) {
        dimen *= SCENE_SIZE / dimen.width;
    }
    else {
        dimen *= SCENE_SIZE / dimen.height;
    }
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    _assets = assets;
	_assets->loadDirectory("json/assets.json");

	_rootNode = _assets->get<scene2::SceneNode>(id);
	_rootNode->setContentSize(dimen);
	_rootNode->doLayout(); // Repositions the HUD

	_started = false;
	

	_buttons = std::vector<std::shared_ptr<scene2::Button>>();
	std::shared_ptr<scene2::SceneNode> menu = _rootNode->getChildByName("startmenu")->getChildByName("menu");

	std::shared_ptr<scene2::Button> b = std::dynamic_pointer_cast<scene2::Button>(menu->getChildByName("start_button"));
	b->addListener([=](const std::string& name, bool down) {
		this->_active = false;
		this->setTransition(true);
		this->setTarget("day");
		});
	b->activate();
	_buttons.push_back(b);

	b = std::dynamic_pointer_cast<scene2::Button>(menu->getChildByName("exit_button"));
	b->addListener([=](const std::string& name, bool down) {
		if (down){
			Application::get()->quit();
		}
		});
	b->activate();
	_buttons.push_back(b);

    switch (strToMenuType(id)) {
        case MenuType::MAIN_MENU:
			//listeners

			break;
		case MenuType::LEVEL_SELECT:
			break;
		case MenuType::OPTIONS:
			break;
		case MenuType::PAUSE:
			break;
		default:
			CULog("ERROR INVALID MENU");
			break;
    }

    
    addChild(_rootNode);

	this->setActive(true);

	setName("main_menu");
	setTransition(false);
	setTarget("");
    return true;
}

void MenuScene::setActive(bool b) {
	_active = b;
	for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
		auto button = *it;
		if (b) {
			button->activate();
		}
		else {
			button->deactivate();
		}
	}
}

void MenuScene::update(float dt) {

}

