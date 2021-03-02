#pragma once

// Active system in the System View.
System* activeSystem;

// Coordinates within the system viewport.
static int systemX;
static int systemY;

// Divisor for sprite size and distance. Should always be a power of 2.
static int systemZoom;

// Areas where stars are located.
static SDL_Rect* starAreas;

// Areas where planets are located.
static SDL_Rect* planetAreas;

// Textures of the planets currently being viewed.
static SDL_Texture** planetSprites;

// Buttons relevant to the SystemView.
#define NUM_SYSTEM_BUTTONS 1
static CodeButton* systemButtons[NUM_SYSTEM_BUTTONS];

// Places the planets. Can be changed.
void (*planetPlacer)(System* system, SDL_Rect* planetAreas);

// Enum representing the possible panels within the star system.
enum SystemPanelCodes {
	SYSTEM_INFO_VIEW,
	NUM_SYSTEM_PANELS

};

// Width of the bar showing whether or not the System View is active.
#define ACTIVITY_BAR_WIDTH 8

// Fraction of the screen covered by moving.
#define MOVEMENT_FRACTION 4

// Maximum zoom level (higher is further out).
#define MAXIMUM_ZOOM 32

// Initializes the System View.
void initSystemView();

// Initializes the buttons for the SystemView.
void initSystemButtons();

// Handles events within the System View.
void systemView(SDL_Event event);

// Manages movement within a system.
void systemMovement(SDL_Event event);

// Returns the to the center of a system.
void systemCenter();

// Checks to see if the user has pressed a button within the SystemView.
// If the user has, performs an action.
void pressSystemButton(int xPos, int yPos);

// Enters a planet after the user clicks it.
void enterPlanet(int xPos, int yPos);

// Enters a system after exiting from a planet.
void enterSystemPlanet();

// Changes the current activeSystem.
void changeActiveSystem(System* newSystem);

// Places the planets of a SystemSpaceTile into the system.
void placePlanets(System* system, SDL_Rect* planetAreas);

// Places planets naturally.
void placePlanetsNatural(System* system, SDL_Rect* planetAreas);

// Places planets in a line.
void placePlanetsLine(System* system, SDL_Rect* planetAreas);

// Places the planets of a SystemSpaceTile into the system in a line.
void placePlanetsLine(System* system, SDL_Rect* planetAreas);

// Deletes the planetSprites of the current activeSystem.
void deletePlanetSprites();

// Adds the sprites of the current activeSystem to planetSprites.
void addPlanetSprites();

// Manages scrolling within a system.
void systemScrolling(int xPos, int yPos, SDL_Event event);

// Resizes the System View to fit in a window of the inputed width and height.
void resizeSystemView(int windowW, int windowH);

// Resizes and moves the buttons in the System View.
void resizeSystemPanelButtons();

// Renders the System View.
void renderSystemView();

// Renders activeSystem within the System View.
void renderSystem();

// Renders the buttons for the SystemView.
void renderSystemButtons();

// Renders the bar signifying activity in the System View.
void renderSystemActive();

// Renders the bar signifying inactivity in the System View.
void renderSystemInactive();

/*
Interface representing a panel displaying the activeSystem's data.
*/
class SystemPanel {
public:
	virtual void init() { ; }
	virtual void render() { ; }
	virtual void rerender() { ; }

};

/*
Stores the code and interface components necessarry to render the System's info panels.
*/
#define NUM_SYSTEM_INFO_NAMES 5
class SystemInfoPanel : public SystemPanel {
private:

	// Sprites representing names of system data.
	SDL_Texture* systemInfoSprites[NUM_SYSTEM_INFO_NAMES];

	// Sprites representing values of system data.
	SDL_Texture* valSprites[NUM_SYSTEM_INFO_NAMES];
	uint_least8_t valLens[NUM_SYSTEM_INFO_NAMES];

public:

	// Constructor. Does nothing.
	SystemInfoPanel();

	// Deconstructor.
	~SystemInfoPanel();

	// Initializes the panel.
	// Will delete previously allocated memory if possible.
	void init();

	// Renders the panel.
	void render();

	// Rerenders the panel. Overwrites the previous panel.
	void rerender();


};

/*
Constructor. Zeros valsprites and initializes systemInfoSprites and starTypes.
*/
SystemInfoPanel::SystemInfoPanel() {

	// Zeros memory.
	memset(valSprites, 0, sizeof(valSprites));

	// Creates textures for all planetInfoSprites.
	for (int i = 0; i < NUM_SYSTEM_INFO_NAMES; ++i) {
		SDL_Surface* surface = TTF_RenderText_Blended(defaultFont, systemInfoNames[i].c_str(), { 0, 0, 0, 0xFF });
		systemInfoSprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

	}
}

/*
Deconstructor for a SystemInfoPanel. Deallocates all allocated memory.
*/
SystemInfoPanel::~SystemInfoPanel() {

	// Deletes sprites representing names of planet data.
	for (int i = 0; i < NUM_SYSTEM_INFO_NAMES; ++i)
		if (systemInfoSprites[i]) SDL_DestroyTexture(systemInfoSprites[i]);

	// Deletes sprites representing values of planet data.
	for (int i = 0; i < NUM_SYSTEM_INFO_NAMES; ++i)
		if (valSprites[i]) SDL_DestroyTexture(valSprites[i]);

}

/*
Initializes all of the valSprites for the activeSystem. Will delete previously allocated
valSprites as needed.
Specifically, initializes Star Size, Star Luminosity, Star Type, Num Habitable, and Num Barren.
*/
void SystemInfoPanel::init() {
	std::string valString;
	SDL_Surface* surface;

	// Initializes size.
	// TODO average size
	valString = std::to_string(activeSystem->stars[0].size);
	valLens[0] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[0]) SDL_DestroyTexture(valSprites[0]);
	valSprites[0] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Luminosity is the average luminosity of all stars in the system.
	double luminosity = 0;
	for (int i = 0; i < activeSystem->numStars; ++i) luminosity += activeSystem->stars[i].luminosity;
	valString.resize(100);
	std::snprintf(&valString[0], 100, "%.0f", (luminosity / activeSystem->numStars) / 1000000000000000000);
	valString.resize(strlen(&valString[0]));
	valString += "Q";
	valLens[1] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[1]) SDL_DestroyTexture(valSprites[1]);
	valSprites[1] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initializes type. Type will give the type of the main star in a single star system,
	// otherwise the type of star system.
	switch(activeSystem->numStars) {
	case 1:
		switch (activeSystem->stars[0].tileID) {
		case MStar: valString = "M"; break;
		case KStar: valString = "K"; break;
		case GStar: valString = "G"; break;
		case FStar: valString = "F"; break;
		case AStar: valString = "A"; break;
		default: valString = "quaternion"; break;

		}
	break;
	case 2: valString = "Binary"; break;
	case 3: valString = "Trinary"; break;
	case 4: valString = "Quaternary"; break;
	default: valString = "bepis"; break;

	}
	valLens[2] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[2]) SDL_DestroyTexture(valSprites[2]);
	valSprites[2] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initializes numHabitable.
	valString = std::to_string(activeSystem->numHabitable);
	valLens[3] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[3]) SDL_DestroyTexture(valSprites[3]);
	valSprites[3] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initializes numBarren.
	valString = std::to_string(activeSystem->numBarren);
	valLens[4] = valString.length();
	surface = TTF_RenderText_Blended(defaultFont, valString.c_str(), { 0, 0, 0, 0xFF });
	if (valSprites[4]) SDL_DestroyTexture(valSprites[4]);
	valSprites[4] = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

}

/*
Renders the contents of the panel.
*/
void SystemInfoPanel::render() {

}

/*
Rerenders the panel.
*/
void SystemInfoPanel::rerender() {
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
	for (; rect.y < systemPanel.y + systemPanel.h - 2 * yBuffer - rect.h && currVal < NUM_SYSTEM_INFO_NAMES; rect.y += rect.h) {
		for (; rect.x < systemPanel.x + systemPanel.w - 2 * xBuffer - rect.w && currVal < NUM_SYSTEM_INFO_NAMES; rect.x += rect.w) {

			// Renders the name.
			len = rect.w < (systemInfoNames[currVal].length() * rect.w) / 20 ?
				rect.w : (systemInfoNames[currVal].length() * rect.w) / 20;
			tempRect = { rect.x, rect.y, len / 2, rect.h };
			SDL_RenderCopy(renderer, systemInfoSprites[currVal], NULL, &tempRect);

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

// Current panel for the activeSystem.
uint_least8_t currSystemPanel;

// Panels used to relay information about systems.
SystemPanel* systemPanels[NUM_SYSTEM_PANELS];

// List of buttons used by the activeSystem.
uint_least8_t activeSystemPanelButtons[NUM_SYSTEM_PANELS];
CodeButton systemPanelButtons[NUM_SYSTEM_PANELS];

/*
Finds the next colonized system. If there is no next colonized system, returns
bottom-right most system.
TODO DEBUG REMOVE
*/
System* findNextCol() {
	static int i = 0;
	static int j = 0;
	System* activeSystem = nullptr;

	// Will search for a system until one is found.
	while (activeSystem == nullptr) {

		// If possible, finds a system with a colony.
		for (; i < universeWidth; ++i) {
			for (; j < universeHeight; ++j) {
				if (uIndex(i, j).getTileID() == SYSTEM_TILE) {
					activeSystem = uSystem(i, j);
					for (int p = 0; p < activeSystem->numHabitable; ++p) {
						if (activeSystem->planets[p]->owners[1].colony != nullptr) {
							printf("col\n");

							// Moves to the next tile.
							if (++j > universeHeight) {
								j = 0;
								if (++i > universeWidth) i = 0;

							}
							return activeSystem;

						}
					}
				}
			}
			j = 0;
		}

		// Sets i and j to 0 if the end is reached with no colonized system found.
		i = 0;
		j = 0;

	}

	// Returns activeSystem when no colony is found.
	return activeSystem;

}

/*
Initializes the System View.
*/
void initSystemView() {

	// Sets the user to the maximum zoom level by default.
	systemZoom = 1;

	// Sets the planetPlacer to the natural one.
	planetPlacer = placePlanetsNatural;

	// Gets the next colonized system or the last available system.
	// TODO should be home system.
	activeSystem = findNextCol();

	// The info panel is always available.
	activeSystemPanelButtons[0] = 1;
	systemPanels[SYSTEM_INFO_VIEW] = new SystemInfoPanel();
	systemPanels[SYSTEM_INFO_VIEW]->init();

	// Initializes planetSprites.
	addPlanetSprites();

	// Initializes starAreas.
	starAreas = new SDL_Rect[4];
	placeStars(activeSystem, starAreas);

	// Initializes planetAreas.
	planetAreas = new SDL_Rect[activeSystem->numHabitable + activeSystem->numBarren];
	placePlanets(activeSystem, planetAreas);

	// Initializes the systemViewViewport's size to the 384 pixels below the universeViewport and renders it.
	resizeSystemView(screenWidth, screenHeight);

	// Initializes the buttons for the system.
	systemButtons[0] = nullptr;
	initSystemButtons();

	// Initializes universe viewport coordinates such that the view is centered about a system's center.
	systemCenter();

	// Initializes the systems active bar to inactive and renders it.
	renderSystemInactive();

}

/*
Initializes the buttons for the SystemView.
TODO inefficient.
*/
void initSystemButtons() {
	int buttonWidth = systemViewport.w / 8;
	int buttonHeight = buttonWidth / 4;
	if (systemButtons[0] != nullptr) deallocateButtons(systemButtons, NUM_SYSTEM_BUTTONS);
	systemButtons[0] = new CodeButton(systemViewport.x + systemViewport.w - buttonWidth, systemViewport.y, buttonWidth, buttonHeight, SYST_VIEW_CHANGE, "Change View");

}

/*
Handles events within the System View.
*/
void systemView(SDL_Event event) {

	//If the user presses a key, moves them within the systemViewport.
	if (event.type == SDL_KEYDOWN) {

		// Moves within the viewport if WASD, the movement keys, or E and Q are pressed.
		systemMovement(event);

		// Does nothing if the user presses any other key.

	}
	// Manages scrolling if user uses the mousewheel.
	else if (event.type == SDL_MOUSEWHEEL) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Scrolls within the systemViewport.
		systemScrolling(xPos, yPos, event);

	}
	// Manages clicking.
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Checks to see if the user has clicked a button within the SystemView.
		clickedButton = getClickedButton(systemButtons, NUM_SYSTEM_BUTTONS, xPos, yPos);
		if (clickedButton != nullptr) {
			clickedButton->changeColour(BUTTON_RECOLOUR);
			clickedButton->render();
			SDL_RenderPresent(renderer);

		}

		// Checks to see if the user has entered a planet.
		enterPlanet(xPos, yPos);

		// Renders the above to the window.
		SDL_RenderPresent(renderer);

	}
	// Manages releasing a click.
	else if (event.type == SDL_MOUSEBUTTONUP) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Presses any clicked buttons.
		pressSystemButton(xPos, yPos);

	}

}

/*
Manages user movement within a system.
*/
void systemMovement(SDL_Event event) {

	// Moves within the system viewport based on key presses.
	switch (event.key.keysym.sym) {

	// Moves left.
	case SDLK_LEFT:
	case SDLK_a:
		systemX -= systemViewport.w * systemZoom / MOVEMENT_FRACTION;
		break;

	// Moves right.
	case SDLK_RIGHT:
	case SDLK_d:
		systemX += systemViewport.w * systemZoom / MOVEMENT_FRACTION;
		break;

	// Moves up.
	case SDLK_UP:
	case SDLK_w:
		systemY -= systemViewport.h * systemZoom / MOVEMENT_FRACTION;
		break;

	// Moves down.
	case SDLK_DOWN:
	case SDLK_s:
		systemY += systemViewport.h * systemZoom / MOVEMENT_FRACTION;
		break;

	// Returns the user to the center of a system.
	case SDLK_r:
		systemCenter();
		break;

	// Zooms in on center.
	// Will not set systemZoom below 1.
	case SDLK_e:
		systemZoom /= 2;

		// If the zoom is invalid, reverts it.
		if (!systemZoom) systemZoom = 1;
		// If the zoom is valid, moves within the viewport.
		else {
			systemX += (systemZoom * systemViewport.w) / 2;
			systemY += (systemZoom * systemViewport.h) / 2;

		}
		break;

	// Zooms out on center.
	// Will not set systemZoom above MAXIMUM_ZOOM.
	case SDLK_q:
		systemZoom *= 2;

		// If the zoom is invalid, reverts it.
		if (systemZoom > MAXIMUM_ZOOM) systemZoom = MAXIMUM_ZOOM;
		// If the zoom is valid, moves within the viewport.
		else {
			systemX -= (systemZoom * systemViewport.w) / 4;
			systemY -= (systemZoom * systemViewport.h) / 4;

		}
		break;

	}

	// Renders the System View after moving.
	renderSystem();
	SDL_RenderPresent(renderer);

}

/*
Returns the to the center of a system.
*/
void systemCenter() {

	// Assigns systemX and systemY such that the view is centered on systemViewport's center.
	systemX = (systemZoom * -systemViewport.w) / 2;
	systemY = (systemZoom * -systemViewport.h) / 2;

}

/*
Checks to see if the user has pressed a button within the SystemView. If the user has,
performs an action.
*/
void pressSystemButton(int xPos, int yPos) {
	CodeButton* button = clickedButton;

	// If there is no clickedButton, returns.
	if (clickedButton == nullptr) return;

	// Performs an action based on the button clicked
	switch (button->code) {

	// Changes the view style of the systemView.
	case(SYST_VIEW_CHANGE):
		if (planetPlacer == placePlanetsNatural) planetPlacer = placePlanetsLine;
		else planetPlacer = placePlanetsNatural;
		placePlanets(activeSystem, planetAreas);
		renderSystem();
		break;

	// If no button was pressed, restores clickedButton.
	default:
		clickedButton = button;
		break;

	}

}

/*
Checks to see if the user has clicked on a planet. If the user has, enters the Planet View.
*/
void enterPlanet(int xPos, int yPos) {

	// Centers xPos and yPos relative to the systemViewport.
	xPos -= systemViewport.x + systemViewport.w / 2;
	yPos -= systemViewport.y + systemViewport.h / 2;

	// Sets xPos and yPos relative to the center of the system.
	xPos = (xPos + systemX / systemZoom + (systemViewport.w / 2)) * systemZoom;
	yPos = (yPos + systemY / systemZoom + (systemViewport.h / 2)) * systemZoom;

	// Parses through each habitable planet until one within bounds is found.
	for (int i = 0; i < activeSystem->numHabitable; ++i) {

		// If a habitable planet within bounds is found, enters it and sets activeSystem to nullptr.
		if (withinBounds(xPos, yPos, planetAreas[i])) {
			changeActiveHabitable(activeSystem->planets[i]);
			activeBarren = nullptr;
			return;

		}
	}

	// Parses through each barren planet until one within bounds is found.
	for (int i = 0; i < activeSystem->numBarren; ++i) {

		// If a barren planet within bounds is found, enters it and sets activeSystem to nullptr.
		if (withinBounds(xPos, yPos, planetAreas[i + activeSystem->numHabitable])) {
			changeActiveBarren(activeSystem->barrens[i]);
			activeHabitable = nullptr;
			return;

		}
	}
}

/*
Enters a system after exiting from a planet.
*/
void enterSystemPlanet() {

	// Exits activeHabitable or activeBarren.
	activeHabitable = nullptr;
	activeBarren = nullptr;

	// Deletes and re-adds the planet sprites. This is to prevent them from being overwritten by
	// the black background.
	deletePlanetSprites();
	addPlanetSprites();

	// Renders the system.
	renderSystemActive();
	renderSystemView();

	// Renders the panel.
	systemPanels[currSystemPanel]->rerender();

	// Renders the system buttons.
	for (int i = 0; i < NUM_HABITABLE_PANELS; ++i)
		if (activeSystemPanelButtons[i]) systemPanelButtons[i].render();

	// Renders the scene.
	SDL_RenderPresent(renderer);

}

/*
Changes the current activeSystem to the inputed system. Recenters the system, resets planetSprites,
positions the stars, sets activePlanet to nullptr, and rerenders the System View.
*/
void changeActiveSystem(System* newSystem) {

	// Changes the active system to this one.
	if (activeSystem != nullptr) deletePlanetSprites();
	activeHabitable = nullptr;
	activeBarren = nullptr;
	activeSystem = newSystem;

	// Places items within the system and gets the planets' sprites.
	addPlanetSprites();
	placeStars(activeSystem, starAreas);
	delete[] planetAreas;
	planetAreas = new SDL_Rect[activeSystem->numHabitable + activeSystem->numBarren];
	placePlanets(activeSystem, planetAreas);

	// Initializes panels as appropriate.
	for (int i = 0; i < NUM_SYSTEM_PANELS; ++i)
		if (activeSystemPanelButtons[i]) systemPanels[i]->init();

	// Sets currSystemPanel to the info panel.
	systemPanelButtons[currSystemPanel].changeColour(-40);
	currSystemPanel = 0;
	systemPanelButtons[currSystemPanel].changeColour(40);
	systemPanels[currSystemPanel]->rerender();

	// Places the systemPanelButtons.
	resizeSystemPanelButtons();

	// Renders the buttons for the activeSystem.
	for (int i = 0; i < NUM_HABITABLE_PANELS; ++i)
		if (activeSystemPanelButtons[i]) systemPanelButtons[i].render();

	// Initializes the system position.
	systemCenter();

	// Renders the system.
	renderSystem();
	SDL_RenderPresent(renderer);

}

/*
Places the planets of a SystemSpaceTile into the system.
*/
void placePlanets(System* system, SDL_Rect* planetAreas) {
	planetPlacer(system, planetAreas);

}

/*
Places the planets of a SystemSpaceTile into the system in the natural fashion.
*/
void placePlanetsNatural(System* system, SDL_Rect* planetAreas) {
	float rad = 0;

	// Places habitable planets.
	for (int i = 0; i < system->numHabitable; ++i) {
		planetAreas[i].x = (int)(cos(rad) * system->planets[i]->distance) - system->planets[i]->size / 2;
		planetAreas[i].y = (int)(sin(rad) * system->planets[i]->distance) - system->planets[i]->size / 2;
		planetAreas[i].w = system->planets[i]->size;
		planetAreas[i].h = system->planets[i]->size;

		rad += (2 * M_PI) / (system->numHabitable + system->numBarren);

	}

	// Places barren planets.
	for (int i = 0; i < system->numBarren; ++i) {
		planetAreas[i + system->numHabitable].x = (int)(cos(rad) * system->barrens[i]->distance) - system->barrens[i]->size / 2;
		planetAreas[i + system->numHabitable].y = (int)(sin(rad) * system->barrens[i]->distance) - system->barrens[i]->size / 2;
		planetAreas[i + system->numHabitable].w = system->barrens[i]->size;
		planetAreas[i + system->numHabitable].h = system->barrens[i]->size;

		rad += (2 * M_PI) / (system->numHabitable + system->numBarren);

	}
}

/*
Places the planets of a SystemSpaceTile into the system in a line.
*/
void placePlanetsLine(System* system, SDL_Rect* planetAreas) {

	// Places each HabitablePlanet in a line.
	for (int i = 0; i < system->numHabitable; ++i) {
		planetAreas[i].x = system->planets[i]->distance - system->planets[i]->size / 2;
		planetAreas[i].y = -system->planets[i]->size / 2;
		planetAreas[i].w = system->planets[i]->size;
		planetAreas[i].h = system->planets[i]->size;

	}

	// Places each BarrenPlanet in a line.
	for (int i = 0; i < system->numBarren; ++i) {
		planetAreas[i + system->numHabitable].x = system->barrens[i]->distance - system->barrens[i]->size / 2;
		planetAreas[i + system->numHabitable].y = -system->barrens[i]->size / 2;
		planetAreas[i + system->numHabitable].w = system->barrens[i]->size;
		planetAreas[i + system->numHabitable].h = system->barrens[i]->size;

	}
}

/*
Deletes the planetSprites of the current activeSystem.
*/
void deletePlanetSprites() {

	// Goes through the array of planetSprites and deletes each one.
	for (int i = 0; i < activeSystem->numHabitable + activeSystem->numBarren; ++i) SDL_DestroyTexture(planetSprites[i]);

	// Deletes the planetSprites array.
	delete[] planetSprites;

}

/*
Adds the sprites of the current activeSystem to planetSprites.
*/
void addPlanetSprites() {
	int i = 0;

	// Assigns the planetSprites array based on numHabitable.
	planetSprites = new SDL_Texture* [activeSystem->numHabitable + activeSystem->numBarren];

	// Goes through the array of planets and adds each one's sprite to planetSprites.
	for (i; i < activeSystem->numHabitable; ++i)
		planetSprites[i] = activeSystem->planets[i]->getPlanetTexture();

	// Goes through the array of uninhabitable planets and adds each one's sprite to planetSprites.
	for (i; i < activeSystem->numHabitable + activeSystem->numBarren; ++i)
		planetSprites[i] = activeSystem->barrens[i - activeSystem->numHabitable]->getPlanetTexture();

}

/*
Manages user scrolling within a system.

Original scrolling formula (for reference).
((systemZoom * systemViewport.w) / 2) * (float)(1 + 2 * (xPos - systemViewport.x - 0.5 * systemViewport.w) / (float)systemViewport.w);
((systemZoom * systemViewport.h) / 2) * (float)(1 + 2 * (yPos - systemViewport.y - 0.5 * systemViewport.h) / (float)systemViewport.h);
*/
void systemScrolling(int xPos, int yPos, SDL_Event event) {

	// Zooms in on the mouse's position.
	// Will not set systemZoom below 1.
	if (event.wheel.y > 0) {
		systemZoom /= 2;

		// If the zoom is invalid, reverts it.
		if (!systemZoom) systemZoom = 1;
		// If the zoom is valid, moves within the viewport.
		else {
			systemX += systemZoom * (xPos - systemViewport.x);
			systemY += systemZoom * (yPos - systemViewport.y);

		}
	}
	// Zooms out on the mouse's position.
	// Will not set systemZoom above MAXIMUM_ZOOM.
	else if (event.wheel.y < 0) {
		systemZoom *= 2;

		// If the zoom is invalid, reverts it.
		if (systemZoom > MAXIMUM_ZOOM) systemZoom = MAXIMUM_ZOOM;
		// If the zoom is valid, moves within the viewport.
		else {
			systemX -= systemZoom * (xPos - systemViewport.x) / 2;
			systemY -= systemZoom * (yPos - systemViewport.y) / 2;

		}
	}

	// Renders the System View after scrolling.
	renderSystem();
	SDL_RenderPresent(renderer);

}

/*
Resizes the System View to fit in a window of the inputed width and height.
Does not renders the System View after resizing.
*/
void resizeSystemViewNoRender(int windowW, int windowH) {

	// Places the system buttons.
	resizeSystemPanelButtons();

	// Places the change view button.
	initSystemButtons();

	// Centers the system.
	systemCenter();

}

/*
Resizes the System View to fit in a window of the inputed width and height.
Renders the System View after resizing.
*/
void resizeSystemView(int windowW, int windowH) {

	// Resizes the components of the System View.
	resizeSystemViewNoRender(windowW, windowH);

	// Rerenders the current SystemPanel.
	systemPanels[currSystemPanel]->rerender();

	// Deletes and re-adds the planet sprites. This is to prevent them from being overwritten by
	// the black background.
	deletePlanetSprites();
	addPlanetSprites();

	// Renders the resized universe view.
	renderSystemView();

}

/*
Resizes the systemPanelButtons.
Also recolours the background of the button area for convenience.
*/
void resizeSystemPanelButtons() {
	int currX = systemSelectionPanel.x;
	int width = systemSelectionPanel.w / 8 < 20 ? 20 : systemSelectionPanel.w / 8;
	int height = 3 * systemSelectionPanel.h / 4 > width / 5 ? 3 * systemSelectionPanel.h / 4 : width / 5;

	// Recolours the background of the button panel.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderFillRect(renderer, &systemSelectionPanel);

	// The first button is used for the info view. It is active by default.
	systemPanelButtons[PLANET_INFO_VIEW].reinit(currX += width / 40, systemSelectionPanel.y, width, height, (ButtonCode)SYSTEM_INFO_VIEW, "Info");
	systemPanelButtons[PLANET_INFO_VIEW].changeColour(40);

}

/*
Renders the System View.
Will render a planet or system, depending on which is active.
*/
void renderSystemView() {

	// Renders the system viewport.
	renderSystem();

	// Renders the System's panel.
	systemPanels[currSystemPanel]->render();

	// Renders the change view button.
	renderSystemButtons();

}

/*
Renders activeSystem.
*/
void renderSystem() {
	SDL_Rect tempArea;
	SDL_Rect sprite;

	// Renders the system's background.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &systemViewport);

	// Confines rendering to the systemViewport.
	SDL_RenderSetViewport(renderer, &systemViewport);

	// Renders each star in activeSystem.
	for (int i = 0; i < activeSystem->numStars; ++i) {
		tempArea = {(starAreas[i].x - systemX) / systemZoom, (starAreas[i].y - systemY) / systemZoom,
			(starAreas[i].w) / systemZoom, (starAreas[i].h) / systemZoom};
		sprite = activeSystem->stars[i].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &tempArea);

	}

	// Renders each habitable planet in activeSystem.
	for (int i = 0; i < activeSystem->numHabitable; ++i) {
		tempArea = {(planetAreas[i].x - systemX) / systemZoom, (planetAreas[i].y - systemY) / systemZoom,
			(planetAreas[i].w) / systemZoom, (planetAreas[i].h) / systemZoom};
		SDL_RenderCopy(renderer, planetSprites[i], NULL, &tempArea);

	}

	// Renders each barren planet in activeSystem.
	for (int i = 0; i < activeSystem->numBarren; ++i) {
		tempArea = { (planetAreas[i + activeSystem->numHabitable].x - systemX) / systemZoom, (planetAreas[i + activeSystem->numHabitable].y - systemY) / systemZoom,
			(planetAreas[i + activeSystem->numHabitable].w) / systemZoom, (planetAreas[i + activeSystem->numHabitable].h) / systemZoom };
		SDL_RenderCopy(renderer, planetSprites[i + activeSystem->numHabitable], NULL, &tempArea);

	}

	// Returns the active viewport to the whole window.
	SDL_RenderSetViewport(renderer, NULL);

	// Renders the first button (the change view button).
	systemButtons[0]->render(0, 0);

}

/*
Renders the buttons for the SystemView.
*/
void renderSystemButtons() {
	for (int i = 0; i < NUM_SYSTEM_BUTTONS; ++i) systemButtons[i]->render(0, 0);

}

/*
Renders the bar signifying activity in the System View.
*/
void renderSystemActive() {

	// Creates a yellow rectangle ACTIVITY_BAR_WIDTH pixels wide above the System View.
	SDL_SetRenderDrawColor(renderer, 38, 54, 251, 255);
	SDL_Rect barRect = {systemViewViewport.x, systemViewViewport.y - ACTIVITY_BAR_WIDTH,
		systemViewViewport.w, ACTIVITY_BAR_WIDTH };
	SDL_RenderFillRect(renderer, &barRect);

}

/*
Renders the bar signifying inactivity in the System View.
*/
void renderSystemInactive() {

	// Creates blue rectangle ACTIVITY_BAR_WIDTH pixels wide above the System View.
	//SDL_SetRenderDrawColor(renderer, 38, 68, 201, 255);
	SDL_SetRenderDrawColor(renderer, 30, 70, 170, 255);
	SDL_Rect barRect = {systemViewViewport.x, systemViewViewport.y - ACTIVITY_BAR_WIDTH,
		systemViewViewport.w, ACTIVITY_BAR_WIDTH };
	SDL_RenderFillRect(renderer, &barRect);

}