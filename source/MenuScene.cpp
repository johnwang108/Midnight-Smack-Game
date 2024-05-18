#include "MenuScene.h"


using namespace cugl;

#define SCENE_SIZE 1280

void MenuScene::dispose() {
	_assets = nullptr;
	_buttons.clear();
	_sliders.clear();
	Scene2::dispose();
}

enum class MenuType {
	MAIN_MENU,
	LEVEL_SELECT,
	OPTIONS,
	PAUSE,
	LOSE,
	WIN
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
	}
	else if (str == "winScreen") {
		return MenuType::WIN;
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
	_advance = false;
	

	_buttons = std::vector<std::shared_ptr<scene2::Button>>();
	_sliders = std::vector<std::shared_ptr<scene2::Slider>>();

    switch (strToMenuType(id)) {
        case MenuType::MAIN_MENU:
			initMainMenu(dimen);
			break;
		case MenuType::LEVEL_SELECT:
			initLevelSelectMenu(dimen);
			break;
		case MenuType::OPTIONS:
			initSettingsMenu(dimen);
			break;
		case MenuType::PAUSE:
			initPauseMenu(dimen);
			break;
		case MenuType::LOSE:
			initLoseMenu(dimen);
			break;
		case MenuType::WIN:
			initWinMenu(dimen);
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

	//std::shared_ptr<scene2::SceneNode> menu = _rootNode->getChildByName("startmenu")->getChildByName("menu");
	auto kids = _rootNode->getChildren();
	for (auto it = kids.begin(); it != kids.end(); ++it) {
		std::shared_ptr<scene2::SceneNode> node = *it;
		std::string nodeName = node->getName();
		
		if (nodeName == "play") {
			CULog("play listener");
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("Button %s pressed in Main Menu, down: %d", name.c_str(), down);
				this->setTransition(true);
				this->setTarget("levelSelectMenu");
			//	this->setTarget("night");
			//	this->setSelectedLevel(1);
				});

			CULog("butt pos: %f %f", butt->getPosition().x, butt->getPosition().y);
		}
		else if (nodeName == "quit") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			std::string bName = butt->getName();

			butt->addListener([=](const std::string& name, bool down) {
				CULog("Button %s pressed in Main Menu, down: %d", name.c_str(), down);
				if (down) {
					Application::get()->quit();
				}
			});
		}
		else if (nodeName == "settings") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);

			butt->addListener([=](const std::string& name, bool down) {
				if (down){
					CULog("settin pressed");
					this->_active = false;
					this->setTransition(true);
					this->setTarget("settingsMenu");
				}
				});
			CULog("after listener");

		}
	}

	std::shared_ptr<Sound> source = _assets->get<Sound>("menu");
	AudioEngine::get()->play("menu", source, true, 0.8f, true);

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
					this->_active = false;
					this->_reset = true;
					});
			}
			else if (nodeName == "home") {
				std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
				_buttons.push_back(butt);
				butt->addListener([=](const std::string& name, bool down) {
					CULog("main menu button pressed");
					this->_active = false;
					this->setTransition(true);
					this->setTarget("main_menu");
					});
			}
			else if (nodeName == "settings") {
				CULog("settings button pressed");
				std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
				_buttons.push_back(butt);
				butt->addListener([=](const std::string& name, bool down) {
					CULog("settings button pressed");
					this->_active = false;
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
		if (nodeName == "exit") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("leave settings button pressed");
				this->setTransition(true);
				if (this->getTransitionedFrom() != "") {
					this->setTarget(this->getTransitionedFrom());
				}
				else {
					this->setTarget("main_menu");
				}
			});
		}
		else if (nodeName == "toggle") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->setToggle(true);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("settings toggle button pressed down: %d", down);
				});
		}
		else if (nodeName == "slider1") {
			std::shared_ptr<scene2::Slider> slid = std::dynamic_pointer_cast<scene2::Slider>(node);
			_sliders.push_back(slid);
			_musicVolume = slid->getValue();
			
			slid->addListener([this](const std::string& name, float value) {
				if (value != _musicVolume) {
					_musicVolume = value;
					//_label->setText("Slider value is " + cugl::strtool::to_string(_musicVolume, 1));
				}
			});
		}
		else if (nodeName == "slider2") {
			std::shared_ptr<scene2::Slider> slid = std::dynamic_pointer_cast<scene2::Slider>(node);
			_sliders.push_back(slid);
			_sfxVolume = slid->getValue();
			slid->addListener([this](const std::string& name, float value) {
				if (value != _sfxVolume) {
					_sfxVolume = value;
					//_label->setText("Slider value is " + cugl::strtool::to_string(_musicVolume, 1));
				}
			});
		}
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

void MenuScene::initWinMenu(Size dimen) {
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
		} else if (nodeName == "next") {
			std::shared_ptr<scene2::Button> butt = std::dynamic_pointer_cast<scene2::Button>(node);
			_buttons.push_back(butt);
			butt->addListener([=](const std::string& name, bool down) {
				CULog("next button pressed");
				this->_advance = true;
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
	for (auto it = _sliders.begin(); it != _sliders.end(); ++it) {
		auto slider = *it;
		if (b && !slider->isActive()) {
			slider->activate();
		}
		else if (!b && slider->isActive()) {
			slider->deactivate();
		}
	}

	_rootNode->setVisible(b);
}

void MenuScene::update(float dt) {
	//if (_active && _sliders.size() != 0) {
	//	CULog("%f", _musicVolume);
	//}
	//if (_active && _rootNode->getName() == "menu") {
	//	for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
	//		auto button = *it;
	//		CULog("Button Name: %s, Button Activity: %d", button->getName().c_str(), button->isActive());
	//	}
	//}
}

void MenuScene::setOrders(std::shared_ptr<scene2::SceneNode> orderNode) {
	if (_orderNode == nullptr) {
		//reposition pause
		std::vector<std::shared_ptr<scene2::SceneNode>> kids = _rootNode->getChildren();
		for (auto it = kids.begin(); it != kids.end(); ++it) {
			(*it)->setPositionX((*it)->getPositionX() + _orderNode->getWidth());
		}
	}
	_orderNode = orderNode;
	_orderNode->setPositionX(0);
	_orderNode->setPositionY(800 - 100);
	_rootNode->addChild(_orderNode);
}

void MenuScene::setHighestLevel(int i) { 
	//CULog("Scene Name: %s", _rootNode->getName().c_str());
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
	_transitionedFrom = "";
	_advance = false;
	//for (auto it = _buttons.begin(); it != _buttons.end(); ++it) {
	//	auto button = *it;
	//	button->setDown(false);	
	//}

}

