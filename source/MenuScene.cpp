#include "MenuScene.h"


using namespace cugl;

#define SCENE_SIZE 1280

void MenuScene::dispose() {
	_assets = nullptr;
	_buttons.clear();
	Scene2::dispose();
}

enum class MenuType {
	MAIN_MENU,
	LEVEL_SELECT,
	OPTIONS,
	PAUSE,
	LOSE
};

std::unordered_map<std::string, int> nameToLevel =
{
	{"one", 1},
	{"two", 2},
	{"three", 3},
	{"four", 4},
	{"five", 5},
	{"six", 6},
	{"seven", 7},
	{"eight", 8},
	{"nine", 9},
	{"ten", 10},
	{"eleven", 11},
	{"twelve", 12},
	{"thirteen", 13},
	{"fourteen", 14},
	{"fifteen", 15},
	{"sixteen", 16},
	{"seventeen", 17},
	{"eighteen", 18},
	{"nineteen", 19},
	{"twenty", 20},
	{"twentyOne", 21},
	{"twentyTwo", 22}
};
	
MenuType strToMenuType(std::string str) {
	if (str == "main") {
		return MenuType::MAIN_MENU;
	}
	else if (str == "levelSelectMenu") {
		return MenuType::LEVEL_SELECT;
	}
	else if (str == "settingsMenu") {
		return MenuType::OPTIONS;
	}
	else if (str == "pause") {
		return MenuType::PAUSE;
	}
	else if (str == "loseScreen") {
		return MenuType::LOSE;
	} else {
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

	_rootNode = _assets->get<scene2::SceneNode>(id);
	_started = false;
	_reset = false;
	

	_buttons = std::vector<std::shared_ptr<scene2::Button>>();
	

    switch (strToMenuType(id)) {
        case MenuType::MAIN_MENU:
			initMainMenu(dimen);
			break;
		case MenuType::LEVEL_SELECT:
			initLevelSelectMenu(dimen);
			break;
		case MenuType::OPTIONS:
			//initSettingsMenu(dimen);
			break;
		case MenuType::PAUSE:
			initPauseMenu(dimen);
			break;
		case MenuType::LOSE:
			initLoseMenu(dimen);
			break;
		default:
			CULog("ERROR INVALID MENU");
			break;
    }

	_rootNode->doLayout(); // Repositions the HUD

    addChild(_rootNode);

	setTransition(false);
	setTarget("");
    return true;
}

void MenuScene::initMainMenu(Size dimen) {
	_rootNode->setContentSize(dimen);

	std::shared_ptr<scene2::SceneNode> menu = _rootNode->getChildByName("startmenu")->getChildByName("menu");
	auto kids = menu->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(*it);
		_buttons.push_back(butt);
		std::string bName = butt->getName();

		if (bName == "start_button") {
			butt->addListener([=](const std::string& name, bool down) {
				CULog("Button %s pressed in Main Menu, down: %d", name.c_str(), down);
				this->_active = false;
				this->setTransition(true);
				this->setTarget("levelSelectMenu");
				});
		}
		else if (bName == "exit_button") {
			butt->addListener([=](const std::string& name, bool down) {
				CULog("Button %s pressed in Main Menu, down: %d", name.c_str(), down);
				if (down) {
					Application::get()->quit();
				}
			});
		}
		else if (bName == "settings_button") {
			CULog("settings button pressed 123");
			butt->addListener([=](const std::string& name, bool down) {
				CULog("settin pressed");
				this->setTransition(true);
				this->setTarget("settingsMenu");
				});
			CULog("after listener");
		}
	}

	setName("main_menu");
	this->setActive(false);
}

void MenuScene::initLevelSelectMenu(Size dimen) {
	_rootNode->setContentSize(dimen);

	auto kids = _rootNode->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::SceneNode> node = *it;

		std::string nodeName = node->getName();
		CULog(nodeName.c_str());
		if (nodeName == "back") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("back button pressed");
				this->setTransition(true);
				this->setTarget("main_menu");
				});
		}
		else if (nameToLevel.find(nodeName) != nameToLevel.end()) {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("%i button pressed", nameToLevel[nodeName]);
				this->setTransition(true);
				this->setTarget("night");
				this->setSelectedLevel(nameToLevel[nodeName]);
				});
		}
	}

	setName("levelSelectMenu");
	this->setActive(false);
}

void MenuScene::initPauseMenu(Size dimen) {
	auto kids = _rootNode->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::SceneNode> node = *it;

		std::string nodeName = node->getName();
		if (nodeName == "pauseretry") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("reset button pressed");
				this->_reset = true;
				});
		}
		else if (nodeName == "home") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("main menu button pressed");
				this->setTransition(true);
				this->setTarget("main_menu");
				});
		}
		else if (nodeName == "settings") {
			CULog("settings button pressed");
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("main menu button pressed");
				this->setTransition(true);
				this->setTarget("settingsMenu");
				});
		}
	}
	//std::shared_ptr<scene2::Button> b = std::dynamic_pointer_cast<scene2::Button>(_rootNode->getChildByName("pauseretry"));
	//b->addListener([=](const std::string& name, bool down) {
	//	CULog("Button %s pressed in Pause Menu, down: %d", name.c_str(), down);
	//	});
	//b->activate();
	//_buttons.push_back(b);

	//_rootNode->setPosition(-90, -20);
	this->setActive(false);
}

void MenuScene::initSettingsMenu(Size dimen) {
	auto kids = _rootNode->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::SceneNode> node = *it;

		std::string nodeName = node->getName();
	}
	this->setActive(false);
}

void MenuScene::initLoseMenu(Size dimen) {
	auto kids = _rootNode->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::SceneNode> node = *it;

		std::string nodeName = node->getName();
		if (nodeName == "pauseretry") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("reset button pressed");
				this->_reset = true;
				});
		}
		else if (nodeName == "home") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("main menu button pressed");
				this->setTransition(true);
				this->setTarget("main_menu");
				});
		}
	}
	//std::shared_ptr<scene2::Button> b = std::dynamic_pointer_cast<scene2::Button>(_rootNode->getChildByName("pauseretry"));
	//b->addListener([=](const std::string& name, bool down) {
	//	CULog("Button %s pressed in Pause Menu, down: %d", name.c_str(), down);
	//	});
	//b->activate();
	//_buttons.push_back(b);

	//_rootNode->setPosition(-90, -20);
	this->setActive(false);
}

void MenuScene::setActive(bool b) {
	_active = b;
	for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
		auto button = *it;
		button->setDown(false);
		if (b) {
			button->activate();
		}
		else {
			button->deactivate();
		}
	}
	_rootNode->setVisible(b);
}

void MenuScene::update(float dt) {

}

void MenuScene::setHighestLevel(int i) { 
	if (!isActive()) return;
	_highestLevel = i; 
	for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
		auto button = *it;
		button->setDown(false);
		if (nameToLevel[button->getName()] <= _highestLevel + 1) {
			button->activate();
			button->setColor(Color4::WHITE);
		}
		else {
			button->setColor(Color4::GRAY);
			button->deactivate();
		}
	}
}

void MenuScene::reset() {
	_active = false;
	_reset = false;
	_transitionScenes = false;
	_targetScene = "";
	_started = false;
	//for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
	//	auto button = *it;
	//	button->setDown(false);	
	//}

}

