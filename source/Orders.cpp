#include "Orders.h"

bool Orders::init(const std::shared_ptr<AssetManager>& assets) {
	//_orders = std::map<int, std::vector<std::shared_ptr<scene2::SceneNode>>>();
	if (scene2::SceneNode::init()) {
		_plateIds = std::vector<int>();
		_orderList = std::vector<std::vector<std::shared_ptr<scene2::SceneNode>>>();
		_colors = std::vector<Color4>();
		_rootNode = scene2::SceneNode::alloc();
		_orderPlateIndex = 0;
		std::shared_ptr<scene2::SceneNode> _l = scene2::SceneNode::alloc();
		std::shared_ptr<scene2::SceneNode> _r = scene2::SceneNode::alloc();
		_l->addChild(scene2::PolygonNode::allocWithTexture(assets->get<Texture>("backButton")));
		auto t = assets->get<Texture>("backButton");
		auto p = scene2::PolygonNode::allocWithTexture(t);
		p->flipHorizontal(true);
		_r->addChild(p);
		_leftButton = scene2::Button::alloc(_l);
		_rightButton = scene2::Button::alloc(_r);
		_leftButton->setName("leftButton");
		_leftButton->setContentSize(p->getContentSize());
		_rightButton->setName("rightButton");
		_rightButton->setContentSize(p->getContentSize());
		_leftButton->addListener([=](const std::string& name, bool down) {
			if (down) {
				this->changeIndex(this->_orderPlateIndex - 1);
				CULog("left button!");
			}
			});

		_rightButton->addListener([=](const std::string& name, bool down) {
			if (down) {
				this->changeIndex(this->_orderPlateIndex + 1);
				CULog("right button!");
			}
			});
		_leftButton->setAnchor(Vec2::ANCHOR_CENTER);
		_rightButton->setAnchor(Vec2::ANCHOR_CENTER);
		_plateNode = scene2::PolygonNode::allocWithTexture(assets->get<Texture>("plate"));
		_plateNode->setPosition(5.0, 25.0);
		_plateNode->setScale(0.6f);
		_rootNode->addChild(_leftButton);
		_rootNode->addChild(_rightButton);
		_rootNode->addChild(_plateNode);
		_rootNode->setContentSize(1280, 800);
		addChild(_rootNode);
		setActive(false);
		return true;
	}
	return false;
}

void Orders::changeIndex(int i) {
	_orderPlateIndex = i;
	if (_orderPlateIndex < 0) {
		_orderPlateIndex = _plateIds.size() - 1;
	}
	else if (_orderPlateIndex >= _plateIds.size()) {
		_orderPlateIndex = 0;
	}
	layoutOrders();
}

void Orders::layoutOrders() {
	if (_plateIds.size() == 0) {
		return;
	}
	if (_orderPlateIndex >= _plateIds.size()) {
		return;
	}
	if (_orderPlateIndex < 0) {
		return;
	}
	for (auto& i : _orderList) {
		for (auto& j : i) {
			j->setVisible(false);
		}
	}
	std::vector<std::shared_ptr<SceneNode>> orders = _orderList[_orderPlateIndex];
	float totalHeight = ORDER_HEIGHT * orders.size();
	float start = 0.0f;
	for (int i = 0; i < orders.size(); i++) {
		orders[i]->setPositionX(22.5f);
		orders[i]->setVisible(true);
		orders[i]->setAnchor(Vec2::ANCHOR_CENTER);
		orders[i]->setPositionY(start);
		start -= ORDER_HEIGHT * 1.2f;
		CULog("Position of child: %f %f", orders[i]->getPositionX(), orders[i]->getPositionY());
	}
	_leftButton->setAnchor(Vec2::ANCHOR_CENTER);
	_rightButton->setAnchor(Vec2::ANCHOR_CENTER);
	_leftButton->setPosition(-80.0, 25.0);
	_rightButton->setPosition(80.0, 25.0);
	_plateNode->setColor(_colors[_orderPlateIndex]);
	CULog("laid out %i orders", orders.size());
}

void Orders::addOrder(int index, std::shared_ptr<scene2::SceneNode> order) {
	if (std::find(_plateIds.begin(), _plateIds.end(), index) == _plateIds.end()) {
		_orderList.push_back(std::vector<std::shared_ptr<scene2::SceneNode>>());
		Color4 c;
		if (_plateIds.size() == 0) {
			c = Color4::WHITE;
		}
		else if (_plateIds.size() == 1) {
			c = Color4::BLUE;
		}
		else if (_plateIds.size() == 2) {
			c = Color4::RED;
		}
		else if (_plateIds.size() == 3) {
			c = Color4::BLACK;
		}
		else if (_plateIds.size() == 4) {
			c = Color4::CYAN;
		}
		_plateIds.push_back(index);
		_colors.push_back(c);
	}
	order->setVisible(false);
	_orderList.back().push_back(order);
	_rootNode->addChild(order);
	layoutOrders();
}

void Orders::setActive(bool b) {
	if (b == _isActive) return;
	_isActive = b;
	if (b) {
		_leftButton->setDown(false);
		_rightButton->setDown(false);
		_leftButton->activate();
		_rightButton->activate();
	}
	else {
		_leftButton->setDown(false);
		_rightButton->setDown(false);
		_leftButton->deactivate();
		_rightButton->deactivate();
	}
	_rootNode->setVisible(b);
	
}

void Orders::removeOrder(int index, IngredientType t) {
	CULog("trying to remove order!");
	if (std::find(_plateIds.begin(), _plateIds.end(), index) != _plateIds.end()) {
		int ix = std::find(_plateIds.begin(), _plateIds.end(), index) - _plateIds.begin();
		auto orders = _orderList[ix];
		for (auto i = 0; i < orders.size(); i++) {
			auto it = orders[i];
			if (it != nullptr && it->getName() == (Ingredient::getIngredientStringFromType(t) + "Order")) {
				std::shared_ptr<scene2::SceneNode> order = it;
				_rootNode->removeChild(order);
				_orderList[ix].erase(_orderList[ix].begin() + i);
				order->dispose();
				it = nullptr;
				layoutOrders();
				CULog("removed order!");
				return;
			}
		}
	}
}

void Orders::reset() {
	for (auto& i : _orderList) {
		for (auto& j : i) {
			_rootNode->removeChild(j);
			j = nullptr;
		}
	}
	_orderList.clear();
	_orderPlateIndex = 0;
	_plateIds.clear();
	_colors.clear();
}