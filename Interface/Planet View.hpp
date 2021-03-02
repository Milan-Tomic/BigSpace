#pragma once

// Coordinates within the Planet View (upper left).
int planetX;
int planetY;

// Size (in pixels) of any tile as rendered in the planet viewport.
int planetTileSize;

// Active HabitablePlanet of the Planet View.
HabitablePlanet* activeHabitable;

// Active BarrenPlanet of the Planet (Barren) View.
BarrenPlanet* activeBarren;

// Enum representing ButtonCodes for the habitable panel.
enum HabitablePanelCodes {
	PLANET_INFO_VIEW,
	PLANET_RESOURCE_VIEW,
	NUM_HABITABLE_PANELS

};

// Maximum size of a GalaxyTile in the renderer.
// 64 is the pixel size of the sprites.
#define MAX_TILE_SIZE 64

// Initializes the Planet View.
void initPlanetView();

// Handles events within the Planet View.
void habitableView(SDL_Event event);

// Manages movement within the Planet View.
void planetMovement(SDL_Event event, int planetSize);

// Manages mouse scrolling (zooming) within the planetViewport.
void planetScrolling(int xPos, int yPos, SDL_Event event, int planetSize);

// Resizes the Planet View to fit in a window of the inputed width and height.
void resizePlanetView(int windowW, int windowH, int planetSize);

// Resizes and moves the buttons in the Habitable View.
void resizeHabitablePanelButtons();

// Changes the system view based on user mouse click.
void changeHabitablePanelView(int xPos, int yPos);

// Finds the tile corresponding to the given x and y coordinates.
void findPlanetCoords(int* xPos, int* yPos);

// Makes sure that planetX will not result in a left side out of bounds error.
void checkPlanetLeft(int planetSize);

// Makes sure that planetX will not result in a right side out of bounds error.
void checkPlanetRight(int planetSize);

// Makes sure that planetY will not result in a upper side out of bounds error.
void checkPlanetUp(int planetSize);

// Makes sure that planetY will not result in a lower side out of bounds error.
void checkPlanetDown(int planetSize);

// Changes the activeHabitable.
void changeActiveHabitable(HabitablePlanet* newPlanet);

// Renders the habitable view including activeHabitable and the relevant buttons.
void renderHabitableView();

// Renders activeHabitable within the System View.
void renderHabitable();

// Renders the barren view including activeBarren and the relevant buttons.
void renderBarrenView();

// Renders activeBarren within the System View.
void renderBarren();

/*
Interface representing a panel displaying the activeHabitable's data.
*/
class HabitablePanel {
public:
	virtual void init() { ; }
	virtual void render() { ; }
	virtual void rerender() { ; }

};

/*
Stores the interface components and code necessary to render activeHabitable's
basic info.
*/
#define NUM_PLANET_INFO_NAMES 3
class HabitableInfoPanel : public HabitablePanel {
private:

	// Sprites representing names of planet data.
	SDL_Texture* planetInfoSprites[NUM_PLANET_INFO_NAMES];

	// Sprites representing values of planet data.
	SDL_Texture* valSprites[NUM_PLANET_INFO_NAMES];
	uint_least8_t valLens[NUM_PLANET_INFO_NAMES];

public:

	// Constructor. Zeros valSprites and initializes planetInfoSprites.
	HabitableInfoPanel();

	// Deallocates all allocated memory.
	~HabitableInfoPanel();

	// Initializes the panel.
	// Will delete previously allocated memory if possible.
	void init();

	// Renders the panel.
	void render();

	// Rerenders the panel. Overwrites the previous panel.
	void rerender();

};

/*
Constructor for a HabitableInfoPanel. Initializes sprites for the panel's info names.
Also zeros some memory.
*/
HabitableInfoPanel::HabitableInfoPanel() {

	// Zeros memory.
	memset(valSprites, 0, sizeof(valSprites));

	// Creates textures for all planetInfoSprites.
	for (int i = 0; i < NUM_PLANET_INFO_NAMES; ++i) {
		SDL_Surface* surface = TTF_RenderText_Blended(defaultFont, planetInfoNames[i].c_str(), { 0, 0, 0, 0xFF });
		planetInfoSprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

	}
}

/*
Deconstructor for a HabitableInfoPanel. Deallocates all allocated memory.
*/
HabitableInfoPanel::~HabitableInfoPanel() {

	// Deletes sprites representing names of planet data.
	for (int i = 0; i < NUM_PLANET_INFO_NAMES; ++i)
		if (planetInfoSprites[i]) SDL_DestroyTexture(planetInfoSprites[i]);

	// Deletes sprites representing values of planet data.
	for (int i = 0; i < NUM_PLANET_INFO_NAMES; ++i)
		if (valSprites[i]) SDL_DestroyTexture(valSprites[i]);

}

/*
Initializes all of the valSprites for activeHabitable. Will delete previously allocated
valSprites as needed.
*/
void HabitableInfoPanel::init() {
	std::string valString;
	SDL_Surface* surface;

	// Initializes size.
	valString = std::to_string(activeHabitable->size);
	valLens[0] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[0]) SDL_DestroyTexture(valSprites[0]);
	valSprites[0] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initializes temperature.
	valString = std::to_string(activeHabitable->temperature);
	valLens[1] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[1]) SDL_DestroyTexture(valSprites[1]);
	valSprites[1] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initializes distance.
	valString = std::to_string(activeHabitable->distance);
	valLens[2] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[2]) SDL_DestroyTexture(valSprites[2]);
	valSprites[2] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

}

/*
Renders the info for this panel. Will not rerender names.
TODO this may be used later.
*/
void HabitableInfoPanel::render() {

}

/*
Renders the info for this panel. Will blot out all previous items.
*/
void HabitableInfoPanel::rerender() {
	int width;
	int height;
	int xBuffer;
	int yBuffer;
	SDL_Rect rect;
	SDL_Rect tempRect;
	SDL_Surface* surface;

	// Calculates some values for rendering.
	xBuffer = systemPanel.w / 25;
	yBuffer = systemPanel.h / 25;
	width = 200 < systemPanel.w ? 200 : systemPanel.w - 8 - xBuffer * 2; // One pair will be this long.
	height = width / 10;
	rect = { systemPanel.x + xBuffer, systemPanel.y + yBuffer, width, height };

	// Sets the renderer to the background colour.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

	// Blots out the previous panel.
	SDL_RenderFillRect(renderer, &systemPanel);

	// Renders all of the panels.
	int currVal = 0;
	int len;
	for (; rect.y < systemPanel.y + systemPanel.h - 2 * yBuffer - rect.h && currVal < NUM_PLANET_INFO_NAMES; rect.y += rect.h) {
		for (; rect.x < systemPanel.x + systemPanel.w - 2 * xBuffer - rect.w && currVal < NUM_PLANET_INFO_NAMES; rect.x += rect.w) {

			// Renders the name.
			len = rect.w < (planetInfoNames[currVal].length() * rect.w) / 20 ?
				rect.w : (planetInfoNames[currVal].length() * rect.w) / 20;
			tempRect = { rect.x, rect.y, len / 2, rect.h };
			SDL_RenderCopy(renderer, planetInfoSprites[currVal], NULL, &tempRect);

			// Renders the value.
			len = rect.w < (valLens[currVal] * rect.w) / 20 ?
				rect.w : (valLens[currVal] * rect.w) / 20;
			tempRect.x += rect.w / 2;
			tempRect.w = len / 2;
			SDL_RenderCopy(renderer, valSprites[currVal], NULL, &tempRect);

			// Moves on to the next sprite.
			++currVal;

		}

		// Resets x after each row.
		rect.x = systemPanel.x + xBuffer;

	}
}

/*
Stores the interface components and code necessary to render activeHabitable's
resources to the Celestial View.
*/
class HabitableResourcePanel : public HabitablePanel {
private:

	// Note: Defines contains goodsNames which stores string names of goods.

	// Stores SDL_Texture names of goods.
	SDL_Texture* goodsNameSprites[NUM_GOODS];

	// Sprites representing quantities of goods.
	SDL_Texture* goodsSprites[NUM_GOODS];

	// Quantities of goods. Used to save rendering on update.
	uint_least32_t goods[NUM_GOODS];

	// Number of characters for goodsSprites (goods are stored in goodsNames).
	uint_least8_t goodsLens[NUM_GOODS];


public:

	// Initializes the values for goods.
	HabitableResourcePanel();

	// Deallocates memory for goods.
	~HabitableResourcePanel();

	// Initializes the panel.
	// Will delete previously allocated memory if possible.
	void init();

	// Renders the panel.
	void render();

	// Rerenders the panel. Used in the case that the values are updated.
	void rerender();

};

/*
Constructor for a HabitableResourcePanel. Initializes resources and some sprites.
Also, performs an initial rendering.
*/
HabitableResourcePanel::HabitableResourcePanel() {

	// Zeros memory.
	memset(goodsSprites, 0, sizeof(goodsSprites));
	
	// Creates textures for all goodsNames.
	for (int i = 0; i < NUM_GOODS; ++i) {
		SDL_Surface* surface = TTF_RenderText_Blended(defaultFont, goodsNames[i].c_str(), { 0, 0, 0, 0xFF });
		goodsNameSprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

	}
}

/*
Deconstructor for a HabitableResourcePanel. Deallocates Sprites representing good
names and quantities.
*/
HabitableResourcePanel::~HabitableResourcePanel() {
	for (int currGood = 0; currGood < NUM_GOODS; ++currGood) {
		if (goodsSprites[currGood]) SDL_DestroyTexture(goodsSprites[currGood]);
		if (goodsNameSprites[currGood]) SDL_DestroyTexture(goodsSprites[currGood]);

	}
}

/*
Initializes the HabitableResourcePanel for a new HabitablePlanet.
*/
void HabitableResourcePanel::init() {
	std::string goodString;
	SDL_Surface* surface;

	// Initializes goods and the sprites for each good.
	for (int i = 0; i < NUM_GOODS; ++i) {

		// Stores all goods' quantities and their string length.
		// TODO change to appropriate owner (government).
		goods[i] = activeHabitable->owners[1].colony->market->goods[i].quantity;
		goodString = std::to_string(goods[i]);
		goodsLens[i] = goodString.length();

		// Creates textures for all goods' counts.
		surface = TTF_RenderText_Blended(defaultFont, goodString.c_str(), { 0, 0, 0, 0xFF });
		if (goodsSprites[i]) SDL_DestroyTexture(goodsSprites[i]);
		goodsSprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

	}
}

/*
Renders the resources to the celestial view. Will only render changed values, leaving the scene
otherwise unchanged.
*/
void HabitableResourcePanel::render() {
	int width;
	int height;
	int xBuffer;
	int yBuffer;
	SDL_Rect rect;
	SDL_Rect tempRect;
	SDL_Surface* surface;

	// TODO TEMP REMOVE
	// Finds an appropriate owner to display.
	int owner = 0;
	for (; owner < NUM_HABITABLE_OWNERS; ++owner) if (activeHabitable->owners[owner].colony) break;
	if (owner == NUM_HABITABLE_OWNERS) return;

	// Sets the rendered to grey for rendering backgrounds.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

	// Calculates some values for rendering.
	width = 200; // One pair will be this long.
	height = 20;
	xBuffer = 10;
	yBuffer = 10;
	rect = { systemPanel.x + xBuffer, systemPanel.y + yBuffer, width, height };

	// Renders all of the panels.
	// Will only render panels if they have been updated.
	int currGood = 0;
	int len;
	for (; rect.y < systemPanel.y + systemPanel.h - 2 * yBuffer - rect.h && currGood < NUM_GOODS; rect.y += rect.h) {
		for (; rect.x < systemPanel.x + systemPanel.w - 2 * xBuffer - rect.w && currGood < NUM_GOODS; rect.x += rect.w) {

			// Updates the current good if there has been a change.
			if (goods[currGood] != activeHabitable->owners[owner].colony->market->goods[currGood].quantity) {

				// Updates goods and goodsLens.
				goods[currGood] = activeHabitable->owners[owner].colony->market->goods[currGood].quantity;
				goodsLens[currGood] = strlen(std::to_string(goods[currGood]).c_str());

				// Destroys and replaces the texture for the good.
				SDL_DestroyTexture(goodsSprites[currGood]);
				surface = TTF_RenderText_Blended(defaultFont, std::to_string(goods[currGood]).c_str(), { 0, 0, 0, 0xFF });
				goodsSprites[currGood] = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_FreeSurface(surface);

				// Renders the name.
				/*len = rect.w < (goodsNames[currGood].length() * rect.w) / 20 ?
					rect.w : (goodsNames[currGood].length() * rect.w) / 20;
				tempRect = { rect.x, rect.y, len / 2, rect.h };
				SDL_RenderCopy(renderer, names[currGood], NULL, &tempRect);*/

				// Finds the place to render the quantity.
				len = rect.w < (goodsLens[currGood] * rect.w) / 20 ?
					rect.w : (goodsLens[currGood] * rect.w) / 20;
				tempRect = { rect.x + rect.w / 2, rect.y, len / 2, rect.h };

				// Blots out the previous value and renders the new value.
				SDL_RenderFillRect(renderer, &tempRect);
				SDL_RenderCopy(renderer, goodsSprites[currGood], NULL, &tempRect);

			}

			// Moves on to the next sprite.
			++currGood;

		}

		// Resets x after each row.
		rect.x = systemPanel.x + xBuffer;

	}
}

/*
Rerenders the resources to the celestial view. Renders the panel name and values.
*/
void HabitableResourcePanel::rerender() {
	int width;
	int height;
	int xBuffer;
	int yBuffer;
	SDL_Rect rect;
	SDL_Rect tempRect;
	SDL_Surface* surface;

	// Sets the rendered to grey for rendering backgrounds.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

	// Blots out the previous panel.
	SDL_RenderFillRect(renderer, &systemPanel);

	// Calculates some values for rendering.
	width = 200; // One pair will be this long.
	height = 20;
	xBuffer = 10;
	yBuffer = 10;
	rect = { systemPanel.x + xBuffer, systemPanel.y + yBuffer, width, height };

	// Renders all of the panels.
	int currGood = 0;
	int len;
	for (; rect.y < systemPanel.y + systemPanel.h - 2 * yBuffer - rect.h && currGood < NUM_GOODS; rect.y += rect.h) {
		for (; rect.x < systemPanel.x + systemPanel.w - 2 * xBuffer - rect.w && currGood < NUM_GOODS; rect.x += rect.w) {

			// Renders the name.
			len = rect.w < (goodsNames[currGood].length() * rect.w) / 20 ?
				rect.w : (goodsNames[currGood].length() * rect.w) / 20;
			tempRect = { rect.x, rect.y, len / 2, rect.h };
			SDL_RenderCopy(renderer, goodsNameSprites[currGood], NULL, &tempRect);

			// Renders the value.
			len = rect.w < (goodsLens[currGood] * rect.w) / 20 ?
				rect.w : (goodsLens[currGood] * rect.w) / 20;
			tempRect.x += rect.w / 2;
			tempRect.w = len / 2;
			SDL_RenderCopy(renderer, goodsSprites[currGood], NULL, &tempRect);

			// Moves on to the next sprite.
			++currGood;

		}

		// Resets x after each row.
		rect.x = systemPanel.x + xBuffer;

	}
}

// Current panel for the activeHabitable.
uint_least8_t currHabitablePanel;

// Panels used to relay information about planets.
HabitablePanel* habitablePanels[NUM_HABITABLE_PANELS];

// List of buttons used by the activeHabitable.
uint_least8_t activeHabitablePanelButtons[NUM_HABITABLE_PANELS];
CodeButton habitablePanelButtons[NUM_HABITABLE_PANELS];

/*
NOTE: This View uses System View's viewports and System View's resize function.
*/

/*
Initializes the Planet View.
*/
void initPlanetView() {

	// Initializes the planet viewport coordinates to 0.
	planetX = 0;
	planetY = 0;

	// Initializes planetTileSize to 64.
	planetTileSize = 64;

	// The info panel is always available.
	activeHabitablePanelButtons[0] = 1;

	// Initializes the array of panels.
	habitablePanels[PLANET_INFO_VIEW] = new HabitableInfoPanel();
	habitablePanels[PLANET_RESOURCE_VIEW] = new HabitableResourcePanel();
	
}

/*
///////////
// DEBUG //
///////////
changes the building of a certain tile.
*/
void debugBuildings(int xPos, int yPos) {
	findPlanetCoords(&xPos, &yPos);
	wrapAroundPlanet(activeHabitable->size, &xPos, &yPos);

	////////////////////////
	// Reveals tile data. //
	////////////////////////
	printf("coord : [%d, %d]\n", xPos, yPos);
	printf("tileData : %s\n", getPlanetTileName((PlanetTileID)activeHabitable->planet[index(xPos, yPos, activeHabitable->size)].tileData));
	printf("owner : %d\n", activeHabitable->planet[index(xPos, yPos, activeHabitable->size)].owner);
	printf("building: %d\n", activeHabitable->planet[index(xPos, yPos, activeHabitable->size)].buildingID);
	printf("numData: %d\n", activeHabitable->planet[index(xPos, yPos, activeHabitable->size)].numData);
	if (activeHabitable->battle) {
		printf("battle owner : %d\n", pBattleIndex(xPos, yPos, activeHabitable).owner);
		printf("battle unit  : %d\n", pBattleIndex(xPos, yPos, activeHabitable).units[0].unit);
		printf("battle num   : %d\n", pBattleIndex(xPos, yPos, activeHabitable).units[0].quantity);
		printf("battle move  : %d\n", pBattleIndex(xPos, yPos, activeHabitable).units[0].moving);
	}

	////////////////////////////
	// Places a new building. //
	////////////////////////////
	//int building = activeHabitable->planet[index(xPos, yPos, activeHabitable->size)].buildingID;

	//// Creates a colony for the planet if there is not one.
	//if (!activeHabitable->numColonies) {
	//	generateInitialColony(activeHabitable, generateGovernment());

	//}
	//	
	////Deletes the tile's current building.
	//deconstructingFuncs[building](activeHabitable, activeHabitable->colonies[0].market, 1, xPos, yPos, nullptr);

	//// Places in government hands.
	//if (activeHabitable->numColonies && building < Pasture5) {
	//	buildingFuncs[++building](activeHabitable, activeHabitable->colonies[0].market, 1, xPos, yPos, nullptr);

	//}
	renderHabitable();
	SDL_RenderPresent(renderer);

}

/*
Handles events within the Planet View.
*/
void habitableView(SDL_Event event) {
	
	// If the user presses a key, moves them within the systemViewport.
	if (event.type == SDL_KEYDOWN) {

		// Moves within the viewport if WASD, the movement keys, or E and Q are pressed.
		// Then renders activeHabitable.
		planetMovement(event, activeHabitable->size);
		renderHabitable();
		SDL_RenderPresent(renderer);

		// Manages exiting the planet view.

		// Does nothing if the user presses any other key.

	}
	// Manages scrolling if user uses the mousewheel.
	else if (event.type == SDL_MOUSEWHEEL) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Scrolls within the systemViewport and renders activeHabitable.
		planetScrolling(xPos, yPos, event, activeHabitable->size);
		renderHabitable();
		SDL_RenderPresent(renderer);

	}
	// Manages clicking.
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Manages clicking panel buttons.
		changeHabitablePanelView(xPos, yPos);

		// Manages clicking within the selection area.
		if (withinBounds(xPos, yPos, systemSelectionPanel));

		///////////
		// DEBUG //
		///////////
		debugBuildings(xPos, yPos);


	}


}

/*
Manages movement with the WASD, movement, and QE keys.
*/
void planetMovement(SDL_Event event, int planetSize) {

	// Moves within the system viewport based on key presses.
	switch (event.key.keysym.sym) {

	// Moves left.
	// Will not move such that the view extends beyond the planet's left bound.
	case SDLK_LEFT:
	case SDLK_a:
		planetX -= 128 / planetTileSize;
		checkPlanetLeft(planetSize);
		break;

	// Moves right.
	// Will not move such that the view extends beyond the planet's right bound.
	case SDLK_RIGHT:
	case SDLK_d:
		planetX += 128 / planetTileSize;
		checkPlanetRight(planetSize);
		break;

	// Moves up.
	// Will not move such that the View extends beyond the planet's upper bound.
	case SDLK_UP:
	case SDLK_w:
		planetY -= 128 / planetTileSize;
		checkPlanetUp(planetSize);
		break;

	// Moves down.
	// Will not move such that the View extends beyond the planet's lower bound.
	case SDLK_DOWN:
	case SDLK_s:
		planetY += 128 / planetTileSize;
		checkPlanetDown(planetSize);
		break;

	// Zooms in on center if not below the minimum width.
	case SDLK_e:

		// Increases the size of rendered PlanetTiles if the size is below MAX_TILE_SIZE.
		if (planetTileSize < MAX_TILE_SIZE) {
			planetX += (systemViewport.w / planetTileSize) / 4;
			planetY += (systemViewport.h / planetTileSize) / 4;
			planetTileSize *= 2;

		}
		break;

	// Zooms out on center.
	case SDLK_q:

		// Increases the size of rendered tiles.
		// Will do so only when the map can accomodate the new number of tiles.
		if (planetTileSize > 1 && !(systemViewport.w / (planetTileSize / 2) > planetSize)
			&& !(systemViewport.h / (planetTileSize / 2) > planetSize)) {
			planetTileSize /= 2;
			planetX -= (systemViewport.w / planetTileSize) / 4;
			planetY -= (systemViewport.h / planetTileSize) / 4;

			// Ensures that planetX and planetY are not out of bounds.
			checkPlanetLeft(planetSize);
			checkPlanetRight(planetSize);
			checkPlanetUp(planetSize);
			checkPlanetDown(planetSize);

		}
		break;

	}
}

/*
Manages mouse scrolling (zooming) within the systemViewport.
Proactively protects against out of bounds errors.

Scrolling is done by changing the size of rendered tiles and the location of planetX and planetY.
*/
void planetScrolling(int xPos, int yPos, SDL_Event event, int planetSize) {

	// Zooms in to the systemViewport when below maximum zoom.
	if (event.wheel.y > 0 && planetTileSize < MAX_TILE_SIZE) {
		planetX += ((xPos - systemViewport.x) / planetTileSize) / 2;
		planetY += ((yPos - systemViewport.y) / planetTileSize) / 2;
		planetTileSize *= 2;

	}
	// Zooms out to the systemViewport if above minimum zoom and doing so will not result in an
	// out of bounds error.
	else if (event.wheel.y < 0 && planetTileSize > 1 && !(systemViewport.w / (planetTileSize / 2) > planetSize) &&
		!(systemViewport.h / (planetTileSize / 2) > planetSize)) {
		planetTileSize /= 2;
		planetX -= ((float)(xPos - systemViewport.x) / planetTileSize) / 2;
		planetY -= ((float)(yPos - systemViewport.y) / planetTileSize) / 2;

	}

	// Makes sure that the scrolling movement does not move the systemViewport out of bounds.
	checkPlanetLeft(planetSize);
	checkPlanetRight(planetSize);
	checkPlanetUp(planetSize);
	checkPlanetDown(planetSize);

}

/*
Resizes the Planet View to fit in a window of the inputed width and height.
*/
void resizePlanetView(int windowW, int windowH, int planetSize) {

	// Places the Habitable panel buttons.
	resizeHabitablePanelButtons();

	// Zooms until the active planet can fit within the systemViewport.
	while (systemViewport.w / planetTileSize > planetSize || systemViewport.h / planetTileSize > planetSize) {
		if (planetTileSize < MAX_TILE_SIZE) {
			planetX += (universeViewport.w / planetTileSize) / 4;
			planetY += (universeViewport.h / planetTileSize) / 4;
			planetTileSize *= 2;

		}
	}

	// Ensures that the above zooming does not result in an out of bounds error.
	checkPlanetRight(planetSize);
	checkPlanetDown(planetSize);

	// Renders the activeHabitable and its associated panel.
	if (activeHabitable != nullptr) {

		// Renders the panel.
		habitablePanels[currHabitablePanel]->rerender();

		// Rerenders the buttons.
		for (int i = 0; i < NUM_HABITABLE_PANELS; ++i)
			if (activeHabitablePanelButtons[i]) habitablePanelButtons[i].render();

		// Rerenders the planet.
		renderHabitable();

	}
	// Renders the activeBarren and its associated panel.
	else if (activeBarren != nullptr) renderBarren();

}

/*
Resizes the buttons in the Habitable Panel.
Also recolours the background of the button area for convenience.
*/
void resizeHabitablePanelButtons() {
	int currX = systemSelectionPanel.x;
	int width = systemSelectionPanel.w / 8 < 20 ? 20 : systemSelectionPanel.w / 8;
	int height = 3 * systemSelectionPanel.h / 4 > width / 5 ? 3 * systemSelectionPanel.h / 4 : width / 5;

	// Recolours the background of the button panel.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderFillRect(renderer, &systemSelectionPanel);

	// The first button is used for the info view. It is active by default.
	habitablePanelButtons[PLANET_INFO_VIEW].reinit(currX += width / 40, systemSelectionPanel.y, width, height, (ButtonCode)PLANET_INFO_VIEW, "Info");
	habitablePanelButtons[PLANET_INFO_VIEW].changeColour(40);

	// The second button is used for the resource view.
	if (activeHabitablePanelButtons[PLANET_RESOURCE_VIEW])
		habitablePanelButtons[PLANET_RESOURCE_VIEW].reinit(currX += width / 40 + width, systemSelectionPanel.y, width, height, (ButtonCode)PLANET_RESOURCE_VIEW, "Resources");

}

/*
Performs some action within the systemSelectionPanel.
*/
void changeHabitablePanelView(int xPos, int yPos) {
	for (int i = 0; i < NUM_HABITABLE_PANELS; ++i) {
		if (activeHabitablePanelButtons[i] && withinBounds(xPos, yPos, habitablePanelButtons[i].area)) {

			// Changes the colour of the previous button.
			habitablePanelButtons[currHabitablePanel].changeColour(-40);
			habitablePanelButtons[currHabitablePanel].render();

			// Sets the new button.
			currHabitablePanel = i;

			// Changes the colour of the new button.
			habitablePanelButtons[currHabitablePanel].changeColour(40);
			habitablePanelButtons[currHabitablePanel].render();

			// Rerenders the panel and breaks.
			habitablePanels[i]->rerender();
			break;

		}
	}
}

/*
Finds the tile corresponding to the given x and y coordinates.
*/
void findPlanetCoords(int* xPos, int* yPos) {

	// Converts xPos and yPos to offsets within the systemViewport.
	*xPos -= systemViewport.x;
	*yPos -= systemViewport.y;

	// Converts pixel data to Tile data.
	*xPos /= planetTileSize;
	*yPos /= planetTileSize;

	// Adds offset from the view's position.
	*xPos += planetX;
	*yPos += planetY;

}

/*
Makes sure that planetX will not result in a left side out of bounds error.
Loops around when planetX is less than 0.
*/
void checkPlanetLeft(int planetSize) { if (planetX < 0) planetX += planetSize; }

/*
Makes sure that planetX will not result in a right side out of bounds error.
Loops around when planetX is greater than the planet's size.
*/
void checkPlanetRight(int planetSize) { if (planetX > planetSize) planetX %= planetSize; }

// Makes sure that planetY will not result in a upper side out of bounds error.
void checkPlanetUp(int planetSize) { if (planetY < 0) planetY = 0; }

// Makes sure that planetY will not result in a lower side out of bounds error.
void checkPlanetDown(int planetSize) {
	if (planetY > planetSize - systemViewport.h / planetTileSize)
		planetY = planetSize - systemViewport.h / planetTileSize;

}

/*
Changes the active HabitablePlanet. Ensures that activeHabitable can fit within the systemViewport. 
Reinitializes and rerenders the activeHabitablePanels and activeHabitablePanelButtons.
Renders the new HabitableView when finished.
*/
void changeActiveHabitable(HabitablePlanet* newPlanet) {
	activeHabitable = newPlanet;

	// Zooms until activeHabitable can fit within the systemViewport.
	while (systemViewport.w / planetTileSize > activeHabitable->size || systemViewport.h / planetTileSize > activeHabitable->size) {
		if (planetTileSize < MAX_TILE_SIZE) {
			planetX += (universeViewport.w / planetTileSize) / 4;
			planetY += (universeViewport.h / planetTileSize) / 4;
			planetTileSize *= 2;

		}
	}

	// Ensures that the above zooming does not result in an out of bounds error.
	checkPlanetRight(activeHabitable->size);
	checkPlanetDown(activeHabitable->size);

	// Sets the available buttons.
	// TODO replace 1
	activeHabitablePanelButtons[PLANET_RESOURCE_VIEW] = activeHabitable->owners[1].colony != nullptr;

	// Initializes panels as appropriate.
	for (int i = 0; i < NUM_HABITABLE_PANELS; ++i)
		if (activeHabitablePanelButtons[i]) habitablePanels[i]->init();

	// Sets currHabitablePanel to the info panel.
	habitablePanelButtons[currHabitablePanel].changeColour(-40);
	currHabitablePanel = PLANET_INFO_VIEW;
	habitablePanelButtons[currHabitablePanel].changeColour(40);
	habitablePanels[currHabitablePanel]->rerender();

	// Places the habitablePanelButtons.
	resizeHabitablePanelButtons();

	// Renders the buttons for the activeHabitable.
	for (int i = 0; i < NUM_HABITABLE_PANELS; ++i)
		if (activeHabitablePanelButtons[i]) habitablePanelButtons[i].render();

	// Renders the Planet View.
	renderHabitableView();
	SDL_RenderPresent(renderer);

}

/*
Renders the habitable view including activeHabitable and the relevant buttons.
Relevant classes are at the head of this file.

Buttons are rendered if and only if there is a change in button state.
*/
void renderHabitableView() {

	// Renders the current panel for activeHabitable.
	habitablePanels[currHabitablePanel]->render();

	// Renders activeHabitable.
	renderHabitable();

}

/*
Renders tiles within the Planet View.
*/
void renderHabitableTiles() {
	SDL_Rect tile;
	SDL_Rect sprite;

	// Renders each tile on the planet.
	int tileIndex = 0;
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {

		// Wraps around the planet.
		if (tileIndex / planetTileSize + planetX >= activeHabitable->size) tileIndex = -planetX * planetTileSize;

		// Renders each column.
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {

			// Renders the tiles.
			tile = { i + systemViewport.x, j + systemViewport.y, planetTileSize, planetTileSize };
			sprite = activeHabitable->planet[index(tileIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable->size)].getSprite();
			SDL_RenderCopy(renderer, tileSheet, &sprite, &tile);

		}
		tileIndex += planetTileSize;

	}
}

/*
Renders rivers within the Celestial View.
TODO move and reuse.
*/
void renderRivers(River* rivers, int size) {
	bool boundsO;
	bool boundsN;
	int x;
	int y;
	int xPos;
	int yPos;
	int tileCenterX;
	int tileCenterY;
	int nextTileCenterX;
	int nextTileCenterY;
	River currRiver;

	// River water is light blue.
	SDL_SetRenderDrawColor(renderer, 77, 196, 240, 255);

	// Renders the rivers on the planet.
	for (int i = 0; i < NUM_RIVERS && rivers[i].startingX != 255; ++i) {
		currRiver = rivers[i];

		// Renders each river.
		x = currRiver.startingX;
		y = currRiver.startingY;
		for (int j = 0; j < currRiver.size; ++j) {
			xPos = x - planetX;
			yPos = y - planetY;
			while (xPos < 0) xPos += size;
			tileCenterX = systemViewport.x + xPos * planetTileSize + planetTileSize / 2;
			tileCenterY = systemViewport.y + yPos * planetTileSize + planetTileSize / 2;

			// Renders the riverParse.
			if (currRiver.riverParse[j].d.nw) {
				nextTileCenterX = tileCenterX - planetTileSize;
				nextTileCenterY = tileCenterY - planetTileSize;

			}
			if (currRiver.riverParse[j].d.n) {
				nextTileCenterX = tileCenterX;
				nextTileCenterY = tileCenterY - planetTileSize;

			}
			if (currRiver.riverParse[j].d.ne) {
				nextTileCenterX = tileCenterX + planetTileSize;
				nextTileCenterY = tileCenterY - planetTileSize;

			}
			if (currRiver.riverParse[j].d.w) {
				nextTileCenterX = tileCenterX - planetTileSize;
				nextTileCenterY = tileCenterY;

			}
			if (currRiver.riverParse[j].d.e) {
				nextTileCenterX = tileCenterX + planetTileSize;
				nextTileCenterY = tileCenterY;

			}
			if (currRiver.riverParse[j].d.sw) {
				nextTileCenterX = tileCenterX - planetTileSize;
				nextTileCenterY = tileCenterY + planetTileSize;

			}
			if (currRiver.riverParse[j].d.s) {
				nextTileCenterX = tileCenterX;
				nextTileCenterY = tileCenterY + planetTileSize;

			}
			if (currRiver.riverParse[j].d.se) {
				nextTileCenterX = tileCenterX + planetTileSize;
				nextTileCenterY = tileCenterY + planetTileSize;

			}

			// Stores bounds checks in x and y.
			boundsO = withinBounds(tileCenterX, tileCenterY, systemViewport);
			boundsN = withinBounds(nextTileCenterX, nextTileCenterY, systemViewport);

			// Handles rivers which are not inlets.
			if (activeHabitable->planet[index(x, y, activeHabitable->size)].tileData <= LAND_TILE) {

				// Renders the full river when both ends are within bounds.
			if (boundsO && boundsN) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, nextTileCenterX, nextTileCenterY);
				// Renders the latter half of the river when only the latter end is within bounds.
				else if (boundsN) SDL_RenderDrawLine(renderer, nextTileCenterX, nextTileCenterY, (tileCenterX + nextTileCenterX) / 2, (tileCenterY + nextTileCenterY) / 2);
				// Renders the first half of the river when only the first end is within bounds.
				else if (boundsO) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, (tileCenterX + nextTileCenterX) / 2, (tileCenterY + nextTileCenterY) / 2);

			}
			// Handles inlets.
			else if (boundsN) SDL_RenderDrawLine(renderer, nextTileCenterX, nextTileCenterY, (tileCenterX + nextTileCenterX) / 2, (tileCenterY + nextTileCenterY) / 2);

			// Moves to the next tile in the river.
			getPlanetCoordAtDir(size, &x, &y, currRiver.riverParse[j]);

		}
	}
}

/*
Renders deposits within the Celestial View. Will not render the deposit if there is a mine over it.
*/
void renderDepositsHabitable(SurfaceDeposit* deposits, int size) {
	int tileX;
	int tileY;
	SDL_Rect tile;
	SDL_Rect depositSprite;

	// Renders each deposit.
	for (int i = 0; i < NUM_DEPOSITS_SURFACE && deposits[i].good != 255; ++i) {

		// Gets the appropriate sprite for the current deposit.
		depositSprite = getDepositSprite(deposits[i].good);
		
		// Renders each item within each deposit.
		for (int j = 0; j < NUM_DEPOSIT_COORDS && deposits[i].xPos[j] != 255; ++j) {

			// Will not render the deposit if there is a mine present.
			if (activeHabitable->planet[index(deposits[i].xPos[j], deposits[i].yPos[j], activeHabitable->size)].buildingID >= Mine1 &&
				activeHabitable->planet[index(deposits[i].xPos[j], deposits[i].yPos[j], activeHabitable->size)].buildingID <= Mine5 ||
				activeHabitable->planet[index(deposits[i].xPos[j], deposits[i].yPos[j], activeHabitable->size)].buildingID == OrcMine)
				continue;

			// Converts tileX to be relative to planetX.
			tileX = deposits[i].xPos[j] - planetX;
			while (tileX < 0) tileX += size;

			// Finds the tile of the current deposit.
			tileX = systemViewport.x + tileX * planetTileSize;
			tileY = systemViewport.y + (deposits[i].yPos[j] - planetY) * planetTileSize;

			// Renders the current deposit if it is within bounds of the planet.
			// Note: + 1 to account for edge tiles.
			if (withinBounds(tileX + 1, tileY + 1, systemViewport)) {
				tile = {tileX, tileY, planetTileSize, planetTileSize};
				SDL_RenderCopy(renderer, tileSheet, &depositSprite, &tile);

			}
		}
	}
}

/*
Renders ways within the Habitable View.
*/
void renderHabitableWays() {
	int tileCenterX;
	int tileCenterY;
	int tileIndexX;
	int tileIndexY;
	Direction dir;

	// Ways are light grey.
	SDL_SetRenderDrawColor(renderer, 195, 195, 195, 255);

	// Renders each tile on the planet.
	int tileIndex = 0;
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {

		// Wraps around the planet.
		if (tileIndex / planetTileSize + planetX >= activeHabitable->size) tileIndex = -planetX * planetTileSize;

		// Renders each column.
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {
			tileCenterX = systemViewport.x + i + planetTileSize / 2;
			tileCenterY = systemViewport.y + j + planetTileSize / 2;

			// Renders the ways.
			if (withinBounds(tileCenterX, tileCenterY, systemViewport)) {
				tileIndexX = tileIndex / planetTileSize + planetX;
				tileIndexY = j / planetTileSize + planetY;
				dir = activeHabitable->planet[index(tileIndexX, tileIndexY, activeHabitable->size)].wayDir;
				if (dir.d.nw) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX - planetTileSize / 2, tileCenterY - planetTileSize / 2);
				if (dir.d.n) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX, tileCenterY - planetTileSize / 2);
				if (dir.d.ne) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX + planetTileSize / 2, tileCenterY - planetTileSize / 2);
				if (dir.d.w) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX - planetTileSize / 2, tileCenterY);
				if (dir.d.e) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX + planetTileSize / 2, tileCenterY);
				if (dir.d.sw) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX - planetTileSize / 2, tileCenterY + planetTileSize / 2);
				if (dir.d.s) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX, tileCenterY + planetTileSize / 2);
				if (dir.d.se) SDL_RenderDrawLine(renderer, tileCenterX, tileCenterY, tileCenterX + planetTileSize / 2, tileCenterY + planetTileSize / 2);

			}
		}
		tileIndex += planetTileSize;

	}
}

/*
Renders buildings within the Habitable View.
*/
void renderHabitableBuildings() {
	SDL_Rect tile;
	SDL_Rect sprite;

	// Renders each tile on the planet.
	int buildingIndex = 0;
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {

		// Wraps around the planet.
		if (buildingIndex / planetTileSize + planetX >= activeHabitable->size) buildingIndex = -planetX * planetTileSize;

		// Renders each column.
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {

			// Renders the buildings.
			tile = { i + systemViewport.x, j + systemViewport.y, planetTileSize, planetTileSize };
			sprite = pIndex(buildingIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable).getBuildingSprite();

			// Rotates Harbours as appropriate.
			if (pBuilding(buildingIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable) >= Harbour1 &&
				pBuilding(buildingIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable) <= Harbour5) {
				SDL_RenderCopyEx(renderer, buildingSheet, &sprite, &tile,
					90.0 * (float)pIndex(buildingIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable).numData,
					NULL, SDL_FLIP_NONE);

			}
			// Renders other buildings.
			else SDL_RenderCopy(renderer, buildingSheet, &sprite, &tile);

		}
		buildingIndex += planetTileSize;

	}
}

// TODO DEBUG REMOVE
SDL_Texture** frontSprites;

/*
Chooses the appropriate sprite for a given unit number.
*/
inline SDL_Rect getArmySprite(int unitSprite) {
	return {SMALL_SPRITE_SIZE * (unitSprite % SMALL_SPRITE_SIZE),
		SMALL_SPRITE_SIZE * (unitSprite / SMALL_SPRITE_SIZE),
		SMALL_SPRITE_SIZE, SMALL_SPRITE_SIZE};

}

/*
Chooses the appropriate sprite for a given army.
unitSprite values correspond to a sprites in the spritesheet BigSpaceUnits.bmp.
Within said spritesheet, each row contains 16 sprites.
*/
inline SDL_Rect getArmySprite(int unit, int owner, Battle* battle) {
	return getArmySprite(battle->owners[owner].unitTable[unit].unitSprite);

}

/*
Renders units within a tile.
*/
inline void renderHabitableArmy(int i, int j, int x, int y, int numUnits, SDL_Rect tile, SDL_Rect armySprite, Battle* battle) {
	int currUnits = 0;
	Direction enemies = battle->enemyDirection(x, y);

	// Army faces north.
	if (enemies.d.n) {

		// Renders the appropriate amount of units for the tile.
		for (int h = 0; h < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++h) {
			for (int w = 0; w < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++w) {
				tile = { i + systemViewport.x + w * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					j + systemViewport.y + h * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE), planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE) };
				SDL_RenderCopy(renderer, groundUnitSheet, &armySprite, &tile);

				// Exits the loop when the correct number of units have been created.
				if (++currUnits >= numUnits) return;

			}
		}
	}
	// Army faces east.
	else if (enemies.d.e) {

		// Renders the appropriate amount of units for the tile.
		for (int w = SPRITE_SIZE / SMALL_SPRITE_SIZE - 1; w >= 0; --w) {
			for (int h = 0; h < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++h) {
				tile = { i + systemViewport.x + w * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					j + systemViewport.y + h * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE), planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE) };
				SDL_RenderCopy(renderer, groundUnitSheet, &armySprite, &tile);

				// Exits the loop when the correct number of units have been created.
				if (++currUnits >= numUnits) return;

			}
		}
	}
	// Army faces south.
	else if (enemies.d.s) {

		// Renders the appropriate amount of units for the tile.
		for (int h = SPRITE_SIZE / SMALL_SPRITE_SIZE - 1; h >= 0; --h) {
			for (int w = 0; w < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++w) {
				tile = { i + systemViewport.x + w * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					j + systemViewport.y + h * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE), planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE) };
				SDL_RenderCopy(renderer, groundUnitSheet, &armySprite, &tile);

				// Exits the loop when the correct number of units have been created.
				if (++currUnits >= numUnits) return;

			}
		}
	}
	// Army faces west.
	else {

		// Renders the appropriate amount of units for the tile.
		for (int w = 0; w < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++w) {
			for (int h = 0; h < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++h) {
				tile = { i + systemViewport.x + w * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					j + systemViewport.y + h * planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
					planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE), planetTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE) };
				SDL_RenderCopy(renderer, groundUnitSheet, &armySprite, &tile);

				// Exits the loop when the correct number of units have been created.
				if (++currUnits >= numUnits) return;

			}
		}
	}
}

/*
Renders battles within the Habitable View.
TODO move and reuse
*/
void renderHabitableArmies() {
	int x, y;
	int armyIndex = 0;
	int numUnits, currUnits;
	SDL_Rect tile;
	SDL_Rect armySprite = {0, 0, SMALL_SPRITE_SIZE, SMALL_SPRITE_SIZE};
	SDL_Rect* sprite;
	Battle* battle;

	// TODO DEBUG REMOVE
	// TODO remove frontSprites and rendering
	if (!frontSprites) {
		frontSprites = new SDL_Texture*[256];
			for (int i = 0; i < 256; ++i) {
				SDL_Surface* surface = TTF_RenderText_Blended(defaultFont, std::to_string(i).c_str(), { 0, 0, 0, 0xFF });
				frontSprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
				SDL_FreeSurface(surface);

			}
	}

	// Does nothing if there is no battle occuring.
	if (!(battle = activeHabitable->battle)) return;

	// Renders armies on each tile on the planet.
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {

		// Wraps around the planet.
		if (armyIndex / planetTileSize + planetX >= activeHabitable->size) armyIndex = -planetX * planetTileSize;

		// Calculates position on the planet using position on the screen.
		// TODO armyIndex is only used for coordinate, yet for some reason is related to screen position.
		// Fix for other renderers also
		x = armyIndex / planetTileSize + planetX;

		// Renders each column.
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {

			// Calculates y position on the planet using position on the screen.
			y = j / planetTileSize + planetY;

			// Renders the armies in a tile.
			// TODO render all armies
			if (battle->battlefield[index(x, y, activeHabitable->size)].units[0].quantity) {
				numUnits = (battle->battlefield[index(x, y, activeHabitable->size)].units[0].quantity + 1) / 16;
				armySprite = getArmySprite(pBattleIndex(x, y, activeHabitable).units[0].unit, pBattleIndex(x, y, activeHabitable).owner, battle);
				renderHabitableArmy(i, j, x, y, numUnits, tile, armySprite, battle);
				goto exit;
				
			}
			// TODO DEBUG REMOVE
			else if (battle->battlefield[index(x, y, activeHabitable->size)].units[2].quantity) {
				numUnits = (battle->battlefield[index(x, y, activeHabitable->size)].units[0].quantity + 1) / 16;
				armySprite = getArmySprite(1);
				renderHabitableArmy(i, j, x, y, numUnits, tile, armySprite, battle);
				goto exit;

			}

		// Exit point for the loop.
		exit:;

			// TODO DEBUG REMOVE
			// Shows the front of the tile.
			tile = { i + systemViewport.x, j + systemViewport.y, planetTileSize, planetTileSize };
			//SDL_RenderCopy(renderer, frontSprites[pgbIndex(x, y, activeHabitable).front], NULL, &tile);

		}
		armyIndex += planetTileSize;

	}
}

/*
Renders flag colours within the Planet View.
*/
void renderHabitableNations() {
	SDL_Rect tile;
	SDL_Colour governmentColour;
	SDL_Colour occupierColour;
	uint_least8_t owner;
	Government* government;
	Government* occupierGovernment;

	// Renders each tile on the planet.
	int tileIndex = 0;
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {

		// Wraps around the planet.
		if (tileIndex / planetTileSize + planetX >= activeHabitable->size) tileIndex = -planetX * planetTileSize;

		// Renders each column.
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {

			// Finds the tile.
			tile = { i + systemViewport.x, j + systemViewport.y, planetTileSize, planetTileSize };

			// Finds the owner of the tile.
			owner = activeHabitable->planet[index(tileIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable->size)].owner;

			// Finds the government of the owner of the tile.
			// Does nothing if there is no such government.
			if (!owner || !(government = (Government*)activeHabitable->owners[owner].colony->government)) continue;

			// Renders the flag colour of the government.
			governmentColour = government->flagColour;

			// If there is a battle, the left side of the tile has the colour of the de jure owner and the right side of the tile has
			// the colour of the occupier.
			if (activeHabitable->battle) {
				occupierGovernment = (Government*)(activeHabitable->battle->owners[pBattleIndex(tileIndex / planetTileSize + planetX, j / planetTileSize + planetY, activeHabitable).owner].owner->government);
				occupierColour = occupierGovernment->flagColour;

				// Renders the tile as belonging to the occupier.
				SDL_SetRenderDrawColor(renderer, occupierColour.r, occupierColour.g, occupierColour.b, occupierColour.a);
				SDL_RenderFillRect(renderer, &tile);

				// Renders the original owner's colour in the upper right corner.
				if (occupierGovernment != government) {
					tile.w /= 4;
					tile.h /= 4;
					tile.x += tile.w * 3;
					SDL_SetRenderDrawColor(renderer, governmentColour.r, governmentColour.g, governmentColour.b, 255);
					SDL_RenderFillRect(renderer, &tile);

				}
			}
			// If the tile is not occupied, renders the government's colours.
			else {
				SDL_SetRenderDrawColor(renderer, governmentColour.r, governmentColour.g, governmentColour.b, governmentColour.a);
				SDL_RenderFillRect(renderer, &tile);

			}
		}
		tileIndex += planetTileSize;

	}
}

/*
Renders activeHabitable within the Celestial View.
*/
void renderHabitable() {

	// Renders this activeHabitable's tiles.
	renderHabitableTiles();

	// Renders this activeHabitable's deposits.
	renderDepositsHabitable(activeHabitable->surfaceDeposits, activeHabitable->size);

	// Renders this activeHabitable's Rivers.
	renderRivers(activeHabitable->rivers, activeHabitable->size);

	// Renders this activeHabitable's ways.
	renderHabitableWays();

	// Renders this activeHabitable's buildings.
	renderHabitableBuildings();

	// Renders this activeHabitable's political view.
	renderHabitableNations();

	// Renders this activeHabitable's armies.
	renderHabitableArmies();

}