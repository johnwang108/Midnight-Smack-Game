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

bool Inventory::init(shared_ptr<AssetManager>& assets, Size size) {
    if (assets == nullptr) {
        return false;
    }
    _assets = assets;
    //_childOffset = -1;
    _selectedSlot = 0;
    _enlarged = false;
    _currentlyHeldIngredient = nullptr;
    _ingredientToRemove = nullptr;
    _bottomBar = nullptr;
    initWithBounds(size);
    Rect slotRect = Rect(0.0f, 0.0f, INV_WIDTH / NUM_SLOTS, INV_HEIGHT);

    for (int i = 0; i < NUM_SLOTS; i++) {
        shared_ptr<scene2::PolygonNode> invSlot = createInventoryNode(slotRect, i);
        invSlot->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
        invSlot->setPosition((i * INV_WIDTH / NUM_SLOTS) + (i * 50), 10);
        addChild(invSlot);
        _slots[i] = invSlot;
    }
    return true;
}

void Inventory::update(float timestep) {
    // Update logic here
}


void Inventory::addIngredient(std::shared_ptr<Ingredient> ingredient) {
    if (_currentIngredients.size() == NUM_SLOTS) {
        shared_ptr<Ingredient> oldestIng = _currentIngredients.back();
        removeIngredientFromSlotNode(oldestIng, _currentIngredients.size() - 1);
        _currentIngredients.pop_back();
        shiftAllIngredientsUp(0);
    }
    _currentIngredients.push_front(ingredient);
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
    if (slotToClear >= _currentIngredients.size()) {
        CULog("tried to pop an empty or invalid slot");
        return nullptr;
    }
    
    shared_ptr<Ingredient> ingToRemove = _currentIngredients[slotToClear];
    removeIngredientFromSlotNode(ingToRemove, slotToClear);
    _currentIngredients.erase(_currentIngredients.erase(_currentIngredients.begin() + slotToClear));
    return ingToRemove;
}   

void Inventory::removeIngredientFromSlotNode(shared_ptr<Ingredient> ing, int slotNumber) {
    string slotName = "slot" + slotNumber;
    shared_ptr<scene2::SceneNode> slotNode = getChildByName(slotName);
    slotNode->removeChild(ing->getButton());
}

std::shared_ptr<Ingredient> Inventory::findHeldIngredient() {
    for (std::shared_ptr<Ingredient> ing : _currentIngredients) {
        if (ing->getBeingHeld() == true) {
            ing->setFalling(false);
            ing->setLaunching(false);
            //todo handle removing from slot scene graph (caller will move it to uiScene?)

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

void Inventory::reset() {
    _currentIngredients.clear();
    _ingredientToRemove = nullptr;
    _currentlyHeldIngredient = nullptr;
    _selectedSlot = 0;
    _enlarged = false;
}

shared_ptr<scene2::PolygonNode> Inventory::createInventoryNode(Rect invRect, int invIndex) {
    shared_ptr<scene2::PolygonNode> slotNode = scene2::PolygonNode::allocWithPoly(invRect);
    string slotName = "slot" + to_string(invIndex);

    slotNode->setName(slotName);
    slotNode->setColor(Color4::GRAY);

    return slotNode;
}