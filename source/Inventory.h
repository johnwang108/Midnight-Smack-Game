#ifndef __INVENTORY__H__
#define __INVENTORY__H__

#include <cugl/cugl.h>
#include <unordered_set>
#include <vector>
#include <deque>
#include "PFInput.h"
#include "PFDudeModel.h"
#include "Ingredient.h"

#define NUM_SLOTS 5
#define INV_WIDTH 500.0f
#define INV_HEIGHT 100.0f

class Inventory : public cugl::scene2::SceneNode {
protected:
	std::shared_ptr<cugl::AssetManager> _assets;
	std::shared_ptr<PlatformInput> _input;

	std::shared_ptr<cugl::scene2::SceneNode> _bottomBar;
	std::deque<std::shared_ptr<Ingredient>> _currentIngredients;
	std::shared_ptr<Ingredient> _ingredientToRemove;
	std::shared_ptr<Ingredient> _currentlyHeldIngredient;	

	std::shared_ptr<scene2::PolygonNode> _slots[NUM_SLOTS];
	int _selectedSlot;
	bool _enlarged;
	bool _focus;
public: 
	Inventory();
	~Inventory() { dispose(); }
	void dispose();
	bool init( std::shared_ptr<cugl::AssetManager>& assets, std::shared_ptr<PlatformInput> input, Size size);
	void update(float timestep);
	void setFocus(bool focus) { _focus = focus; }
	int getSelectedSlot() { return _selectedSlot; }
	void unhighlightSelectedSlot();
	void highlightSelectedSlot();


	std::shared_ptr<Ingredient> getCurrentlyHeldIngredient() { return _currentlyHeldIngredient; }
	void addIngredient(std::shared_ptr<Ingredient> ingredient);
	/* Internal use, searches through all ingredients to see which is held */
	std::shared_ptr<Ingredient> findAndRemoveHeldIngredient();
	void Inventory::shiftAllIngredientsUp(int startInd);
	std::shared_ptr<scene2::PolygonNode> createInventoryNode(std::shared_ptr<Texture>, int);
	//remove ingredient from slot and return the ingredient
	std::shared_ptr<Ingredient> popIngredientFromSlot(int slotToClear);
	void removeIngredientFromSlotNode(std::shared_ptr<Ingredient> ing, int slotNumber);


	void selectNextSlot();
	void selectPreviousSlot();

	void reset();
};
#endif