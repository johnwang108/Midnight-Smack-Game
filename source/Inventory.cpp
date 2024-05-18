#include "PFGameScene.h"
#include "Inventory.h"

using namespace cugl;
using namespace std;

Inventory::Inventory(){
    _selectedSlot = 0;
    _enlarged = false;
}


void Inventory::dispose() {
    _bottomBar = nullptr;
    _currentIngredients.clear();
    _ingredientToRemove = nullptr;
    _currentlyHeldIngredient = nullptr;
    _assets = nullptr;
}

bool Inventory::init(shared_ptr<AssetManager>& assets, std::shared_ptr<PlatformInput> input, Size size) {
    if (assets == nullptr) {
        return false;
    }
    _assets = assets;
    _input = input;
    _timeSinceLastInteract = 0.0f;

    //_childOffset = -1;
    _selectedSlot = 0;
    _enlarged = false;
    _currentlyHeldIngredient = nullptr;
    _ingredientToRemove = nullptr;
    _bottomBar = nullptr;
    initWithBounds(size);
    shared_ptr<Texture> tex = _assets->get<Texture>("inventorySlot");

    for (int i = 0; i < NUM_SLOTS; i++) {
        shared_ptr<scene2::PolygonNode> invSlot = createInventoryNode(tex, i);
        invSlot->setAnchor(Vec2::ANCHOR_TOP_LEFT);
        invSlot->setPosition((i * tex->getWidth()), 0);
        addChild(invSlot);
        _slots[i] = invSlot;
    }
    highlightSelectedSlot();


    return true;
}

void Inventory::update(float timestep) {
    if (_currentlyHeldIngredient == nullptr) {
        _currentlyHeldIngredient = findAndRemoveHeldIngredient();
    }
    else {
        if (!_currentlyHeldIngredient->getBeingHeld()) {
            _currentlyHeldIngredient = nullptr;
            //handle dropping held ingredient somehow
            //todo add it back
        }
    }
    if (_currentlyHeldIngredient != nullptr) {
         //ingredient is being held so transform it to mouse
         std::shared_ptr<scene2::Button> button = _currentlyHeldIngredient->getButton();
         //CULog("%f, %f", _input->getTouchPos().x, _input->getTouchPos().y);
         Vec2 transformedMouse = _input->getTouchPos();

         button->setPositionX(transformedMouse.x);// +_currentlyHeldIngredient->getButton()->getWidth() / 2);
         button->setPositionY(transformedMouse.y);

         //CULog("Button Pos X: %f, Button Y: %f", button->getPositionX(), button->getPositionY());
         //button->setPosition(_input->getTouchPos());
    }

}

void Inventory::fixedUpdate(float dt) {
    _timeSinceLastInteract += dt;
    if (_timeSinceLastInteract > 2.0f) {
        Color4 c = getColor();
        c.a = std::max(100.0f, c.a - 0.1f);
        setColor(c);
    }
    else {
        Color4 c = getColor();
        c.a = 255.0f;
        setColor(c);
        CULog("Time since last interact: %f", _timeSinceLastInteract);
    }
}


void Inventory::addIngredient(std::shared_ptr<Ingredient> ingredient) {
    _timeSinceLastInteract = 0.0f;
    if (_currentIngredients.size() == NUM_SLOTS) {
 /*       shared_ptr<Ingredient> oldestIng = _currentIngredients.back();
        removeIngredientFromSlotNode(oldestIng, _currentIngredients.size() - 1);
        _currentIngredients.pop_back();
        shiftAllIngredientsUp(0);*/
    }
    else {
        _currentIngredients.push_back(ingredient);
        ingredient->setCurrentInventorySlot(_currentIngredients.size() - 1);
        ingredient->getButton()->setAnchor(Vec2::ANCHOR_CENTER);
        _slots[_currentIngredients.size() - 1]->addChild(ingredient->getButton());
        ingredient->getButton()->setPosition(_slots[_currentIngredients.size() - 1]->getSize() / 2);
    }
}


/* 
    Once an ingredient is removed from the dequeue, we need to update the scene nodes to do the same thing
    Assumes start index has been cleared. AKA if the queue looked like [c, c, b, e, s], and we remove b
    [c, c, -, e, s], then you call this function with startInd = 2

    assumes size < NUM_SLOTS = size of _slots


    FRONT = 0, BACK = _currentIngredients.size();

*/
void Inventory::shiftAllIngredientsUp(int startInd) {
    for (int i = startInd; i < _currentIngredients.size(); i++) {
        for (std::shared_ptr<scene2::SceneNode> child : _slots[i+1]->getChildren()) {
            _slots[i + 1]->removeChild(child);
            _slots[i]->addChild(child);
        }
    }

}

/*
    Remove an ingredient from a slot, both in internal representation as well as scene graph.
    Then return a reference to that Ingredient
*/

shared_ptr<Ingredient> Inventory::popIngredientFromSlot(int slotToClear) {
    _timeSinceLastInteract = 0.0f;
    if (slotToClear >= _currentIngredients.size()) {
        CULog("tried to pop an empty or invalid slot");
        return nullptr;
    }
    
    shared_ptr<Ingredient> ingToRemove = _currentIngredients[slotToClear];
    removeIngredientFromSlotNode(ingToRemove, slotToClear);
    _currentIngredients.erase(_currentIngredients.begin() + slotToClear);

    shiftAllIngredientsUp(slotToClear);
    return ingToRemove;
}   

void Inventory::removeIngredientFromSlotNode(shared_ptr<Ingredient> ing, int slotNumber) {
    _timeSinceLastInteract = 0.0f;
    string slotName = "slot" + to_string(slotNumber);
    shared_ptr<scene2::SceneNode> slotNode = getChildByName(slotName);
    slotNode->removeChild(ing->getButton());
}

std::shared_ptr<Ingredient> Inventory::findAndRemoveHeldIngredient() {
    _timeSinceLastInteract = 0.0f;
    for (std::shared_ptr<Ingredient> ing : _currentIngredients) {
        if (ing->getBeingHeld() == true) {
            ing->setFalling(false);
            ing->setLaunching(false);
            //todo handle removing from slot scene graph (caller will move it to uiScene?)
            popIngredientFromSlot(ing->getCurrentInventorySlot());
            addChild(ing->getButton());
           /* for (std::shared_ptr<scene2::SceneNode> child : _conveyorBelt->getChildren()) {
                if (child == ing->getButton()) {
                    _conveyorBelt->removeChild(ing->getButton());
                    break;
                }
            }
            if (ing->getButton()->getParent() == nullptr) addChild(ing->getButton());*/


            return ing;
        }
    }
    return nullptr;
}

IngredientType Inventory::getIngredientTypeFromSlot(int slotNumber) {
    if (slotNumber >= _currentIngredients.size()) {
		return IngredientType::null;
	}
	return Ingredient::getIngredientTypeFromString(_currentIngredients[slotNumber]->getName());
}

std::string Inventory::getIngredientnameFromSlot(int slotNumber) {
    if (slotNumber >= _currentIngredients.size()) {
        return "";
    }
    return _currentIngredients[slotNumber]->getName();
}

void Inventory::reset() {
    for (int i = 0; i < _currentIngredients.size(); i++) {
        removeIngredientFromSlotNode(_currentIngredients.at(i), i);
    }
    _currentIngredients.clear();
    _ingredientToRemove = nullptr;
    _currentlyHeldIngredient = nullptr;
    unhighlightSelectedSlot();
    _selectedSlot = 0;
    highlightSelectedSlot();
    _timeSinceLastInteract = 0.0f;
    _enlarged = false;

}

shared_ptr<scene2::PolygonNode> Inventory::createInventoryNode(shared_ptr<Texture> tex, int invIndex) {
    shared_ptr<scene2::PolygonNode> slotNode = scene2::PolygonNode::allocWithTexture(tex);
    string slotName = "slot" + to_string(invIndex);

    slotNode->setName(slotName);
    slotNode->setColor(Color4::GRAY);

    return slotNode;
}

void Inventory::selectNextSlot() {
    _timeSinceLastInteract = 0.0f;
    unhighlightSelectedSlot();
    if (_selectedSlot == NUM_SLOTS - 1) {
        _selectedSlot = 0;
    }
    else {
        _selectedSlot++;
    }
    highlightSelectedSlot();
}

void Inventory::selectPreviousSlot() {
    _timeSinceLastInteract = 0.0f;
    unhighlightSelectedSlot();
    if (_selectedSlot == 0) {
        _selectedSlot = NUM_SLOTS - 1;
    }
    else {
        _selectedSlot--;
    }
    highlightSelectedSlot();
}

void Inventory::unhighlightSelectedSlot() {
    _slots[_selectedSlot]->setTexture(_assets->get<Texture>("inventorySlot"));
}

void Inventory::highlightSelectedSlot() {
    _slots[_selectedSlot]->setTexture(_assets->get<Texture>("highlightedInventorySlot"));
}