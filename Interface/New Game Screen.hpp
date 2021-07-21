#pragma once

// Number of buttons on the new game screen.
#define NUM_NEW_GAME_BUTTONS 4

// Number of CodeButtons in the universe customization screen.
#define NUM_GALAXY_CREATION_BUTTONS 5

// Number of TextBoxes in the universe customization screen.
# define NUM_GALAXY_CREATION_BOXES 3

// Number of CodeButtons in the Random galaxy design screen.
#define NUM_RAND_DESIGN_BUTTONS 1

// Number of TextBoxes in the Random galaxy design screen.
#define NUM_RAND_DESIGN_BOXES 6

// Number of CodeButtons in the Spiral galaxy design screen.
#define NUM_SPIRAL_DESIGN_BUTTONS 1

// Number of TextBoxes in the Spiral galaxy design screen.
#define NUM_SPIRAL_DESIGN_BOXES 8

// Number of CodeButtons in the Ring galaxy design screen.
#define NUM_RING_DESIGN_BUTTONS 1

// Number of TextBoxes in the Ring galaxy design screen.
#define NUM_RING_DESIGN_BOXES 7

// Number of galaxies that can be added to the universe.
#define MAXIMUM_GALAXIES 100

// Height of a panel in the universe creation screen.
#define GALAXY_PANEL_HEIGHT 110

// Minimum height or width of the universe.
#define MIN_UNIVERSE_DIM 200

// Generic object representing a galaxy in the customizeUniverse interface.
class NewGalaxyPanel;

// Object representing the ScrollBar for NewGalaxyPanels.
class GalaxyPanelScrollBar;

// Manages the process of creating a new game. Links to the universe generator and other detailers.
ButtonCode newGameStart();

// Manages the process of choosing galaxies.
ButtonCode customizeUniverse(int* minDistance, GalaxyInstruction** galInstructions);

// Creates a list of NewGalaxyPanels based on the inputed GalaxyInstructions.
void loadGalaxyPanels(GalaxyInstruction* galInstructions, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyPanelScrollBar* panelScroll);

// Renders the universe customization screen.
void renderCustomizeUniverse(int* index, SDL_Rect panelViewport, TextBox* universeDimBoxes[NUM_GALAXY_CREATION_BOXES],
	CodeButton* universeCreateButtons[NUM_GALAXY_CREATION_BUTTONS], std::unique_ptr<NewGalaxyPanel>* galaxyPanels,
	GalaxyPanelScrollBar* panelScroll);

// Renders the universe customization screen's Panels.
void renderPanelViewport(SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int *index);

// Changes the dimensions of the universe based on universe customization TextBoxes, then reassigns
// the maximum heights/widths of galaxies based on these changes.
void reassignDimensions(TextBox* universeDimBoxes[NUM_GALAXY_CREATION_BOXES], std::unique_ptr<NewGalaxyPanel>* galaxyPanels);

// Used to add a NewGalaxyPanel to the universe customization screen after a mouse event.
void addGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyType galType, GalaxyPanelScrollBar* panelScroll);

// Adds a NewGalaxyPanel to the universe customization screen from a GalaxyInstruction.
void addGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, GalaxyInstruction galInstruction, int* index, GalaxyPanelScrollBar* panelScroll);

// Deletes a NewGalaxyPanel from the universe customization screen.
void removeGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int i, GalaxyPanelScrollBar* panelScroll);

// Finds the number of NewGalaxyPanels in an array.
int numPanels(std::unique_ptr<NewGalaxyPanel>* galaxyPanels);

// Moves the index so that the bottom of the list is visible.
void indexToBottom(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyPanelScrollBar* panelScroll);

// Used to move panels on the universe customization screen.
void incrementIndex(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int distance, GalaxyPanelScrollBar* panelScroll);

// Used to decrement index on the universe customization screen.
void decrementIndex(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int distance, GalaxyPanelScrollBar* panelScroll);

// Clears all panels in galaxyPanels.
void clearGalaxyPanels(int* index, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, GalaxyPanelScrollBar* panelScroll);

// Converts all galaxyPanels into an array of GalaxyInstructions.
GalaxyInstruction* getPanelInstructions(std::unique_ptr<NewGalaxyPanel>* galaxyPanels);

// Chooses which galaxy design function to call based on the type of the inputed instruction.
ButtonCode designGalaxy(GalaxyInstruction* instr);

// Builds the screen for designing a Random galaxy.
ButtonCode designRandomGalaxy(GalaxyInstruction* instr);

// Builds the screen for designing a Spiral galaxy.
ButtonCode designSpiralGalaxy(GalaxyInstruction* instr);

// Builds the screen for designing a Ring galaxy.
ButtonCode designRingGalaxy(GalaxyInstruction* instr);

// Contains the running loop for designing any galaxy.
ButtonCode designGalaxyLoop(GalaxyInstruction* instr, CodeButton* designButtons[], int numButtons, TextBox* designBoxes[], int numBoxes);

// Renders a galaxy design screen.
void renderDesignGalaxy(CodeButton* designButtons[], int numButtons, TextBox* designBoxes[], int numBoxes);

// Assigns an instruction for the designGalaxyLoop based on the inputed instr.
void assignInstruction(GalaxyInstruction* instr, TextBox* designBoxes[]);

/*
Screen used to determine what galaxies should be generated for a new game.
*/

/*
Manages the process of creating a new game.

The Universe CodeButton can move the user to the universe customization screen, where galaxies are
added to the universe.

The Advanced Settings CodeButton is unfinished TODO.

The Generate Universe CodeButton can generate a new universe with the universes inputed from the
universe customization screen.

The Begin Game Codebutton can move to the play loop.
*/
ButtonCode newGameStart() {

	// Initializes minDistance, the minimum distance between galaxies in the universe, to 20.
	int minDistance = 20;

	// Initiaizes numGals to 0.
	numGals = 0;

	// Initialize universeWidth and universeHeight.
	universeWidth = 500;
	universeHeight = 500;

	// Keeps the width and height of the last generated universe to allow for deletion if the user
	// decides to generate a new one.
	int prevWidth = 0;
	int prevHeight = 0;

	// Initializes the array of new game buttons.
	CodeButton* newGameButtons[NUM_NEW_GAME_BUTTONS];
	newGameButtons[0] = new CodeButton(150, 86, 200, 40, 80, 80, 80, NEW_GAME_GALAXIES, "Universe");
	newGameButtons[1] = new CodeButton(150, 172, 200, 40, 80, 80, 80, NEW_GAME_ADVANCED, "Advanced");
	newGameButtons[2] = new CodeButton(150, 258, 200, 40, 80, 80, 80, NEW_GAME_GENERATE, "Generate Universe");
	newGameButtons[3] = new CodeButton(150, 344, 200, 40, 80, 80, 80, NEW_GAME_PLAY, "Begin Game");

	// Creates the array of galInstructions. Intitializes it to nullptr.
	GalaxyInstruction* galInstructions = nullptr;

	// Running loop for the opening screen.
	SDL_Event event;
	while (true) {

		// Renders all buttons to the window.
		SDL_RenderClear(renderer);
		for (int i = 0; i < NUM_NEW_GAME_BUTTONS; ++i) newGameButtons[i]->render(0, 0);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);

		// Event reading loop.
		while (SDL_PollEvent(&event) != 0) {

			// Returns to the Opening Screen if exited.
			if (event.type == SDL_QUIT) {
				closeWindow();
				return OPENING_QUIT;

			}
			// Handles left clicking.
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Checks to see if any of the buttons has been clicked. If one has, lightens its colour.
				for (int i = 0; i < NUM_NEW_GAME_BUTTONS; ++i) {
					if (withinBounds(xPos, yPos, newGameButtons[i]->area)) {
						newGameButtons[i]->setColour(120, 120, 120);

					}
				}
			}
			// Handles finishing a left click.
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Performs the action corresponding to the clicked button. Nothing will happen if no button
				// was clicked.
				switch (getCode(newGameButtons, NUM_NEW_GAME_BUTTONS, xPos, yPos)) {

				// Moves the user to the universe customization screen, which prepares
				// galaxies to be generated.
				// Quits if the user presses the X button in that screen.
				case(NEW_GAME_GALAXIES):
					if (customizeUniverse(&minDistance, &galInstructions) == OPENING_QUIT)
						return OPENING_QUIT;
					break;

				// TODO advanced settings
				case (NEW_GAME_ADVANCED):
					break;

				// Generates a universe with the inputed parameters if there are galaxies to generate.
				// Deletes the previous universe if a new one is generated.
				// Also, stores the current width and height as prevWidth and prevHeight to allow for deletion
				// of the current galaxy opon regeneration.
				case (NEW_GAME_GENERATE):
					if (numGals) {
						deallocateUniverse(prevWidth, prevHeight);
						generateUniverse(universeWidth, universeHeight, numGals, minDistance, galInstructions);
						prevWidth = universeWidth;
						prevHeight = universeHeight;
							
					}
					break;

				// Moves to the play loop if the universe has been initialized.
				case (NEW_GAME_PLAY):
					if (universe != nullptr) {
						deallocateButtons(newGameButtons, NUM_NEW_GAME_BUTTONS);
						return NEW_GAME_PLAY;

					}
					break;

				}

				// Recolours all buttons to the default.
				resetOpeningColours(newGameButtons, NUM_NEW_GAME_BUTTONS);

			}
		}
	}
}


/*
Class representing a galaxy in the customizeUniverse screen.
*/
class NewGalaxyPanel : public Panel {
public:
	// GalaxyInstruction corresponding to this NewGalaxyPanel.
	GalaxyInstruction instr;

	// Unused default constructor for NewGalaxyPanel.
	NewGalaxyPanel();

	// Constructor for a NewGalaxyPanel.
	NewGalaxyPanel(int code, int numButtons, int numBoxes, int numSprites, int x, int y, int w, int h);

	// Deconstructor for a NewGalaxyPanel.
	~NewGalaxyPanel();

};

// Unused default constructor for NewGalaxyPanel.
NewGalaxyPanel::NewGalaxyPanel() {
	textBoxes = nullptr;
	area = { 0, 0, 0, 0 };
	code = -1;
	numBoxes = 0;

};

/*
Constructor for a NewGalaxyPanel. Mostly identical to the constructor for a Panel.

Initializes the instr based on the inputed code.
*/
NewGalaxyPanel::NewGalaxyPanel(int code, int numButtons, int numBoxes, int numSprites, int x, int y, int w, int h) {
	this->code = code;
	this->area = { x, y, w, h };
	this->numBoxes = numBoxes;
	this->numButtons = numButtons;
	this->numSprites = numSprites;
	textBoxes = new TextBox * [numBoxes];
	buttons = new CodeButton * [numButtons];
	sprites = new Sprite * [numSprites];

	// Initializes instr based on code.
	instr = {(GalaxyType)code, 0, 0, 0, 0, 0, 0};

}

/*
Deconstructor for a NewGalaxyPanel.
*/
NewGalaxyPanel::~NewGalaxyPanel() {

	// Unfinished.

}

/*
ScrollBar used to scroll through GalaxyPanels in the universe creation screen.
*/
class GalaxyPanelScrollBar : public ScrollBar {
public:
	// Default constructor for GalaxyPanelScrollBar (unused).
	GalaxyPanelScrollBar();

	// Standard constructor for the GalaxyPanelScrollBar.
	GalaxyPanelScrollBar(int x, int y, int w, int h, int barSize, int barSpace);

	// Moves the bar. Overloads and calls moveBar from ScrollBar.
	void moveBar(int x, int y, SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index);

	// Moves the grab bar while the user holds down left click.
	void performMovement(SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index);

};

// Unused default constructor for GalaxyPanelScrollBar.
GalaxyPanelScrollBar::GalaxyPanelScrollBar() {}

/*
Constructor for the ScrollBar. Assigns area, barSize, and barSpace. Initializes distance to 0.

Note: This is identical to the ScrollBar's constructor.
*/
GalaxyPanelScrollBar::GalaxyPanelScrollBar(int x, int y, int w, int h, int barSize, int barSpace) {
	area = { x, y, w, h };
	distance = 0;
	ScrollBar::barSize = barSize;
	ScrollBar::barSpace = barSpace;

	// Moves the grab bar while the user holds down left click.
	// Note: does not override the original moveBar.
	void moveBar(int x, int y, std::unique_ptr<NewGalaxyPanel> * galaxyPanels, int* index);

	// Moves the galaxy panels when the grab bar is moved.
	void performMovement(std::unique_ptr<NewGalaxyPanel> * galaxyPanels, int* index);

}

/*
Moves the GalaxyPanelScrollBar and the galaxy Panels.
*/
void GalaxyPanelScrollBar::moveBar(int x, int y, SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index) {

	// Running loop for moving the ScrollBar.
	SDL_Event event;
	while (true) {

		// If the user stops holding left click, returns to the main loop.
		// All events other than releasing left click are ignored.
		if (SDL_PollEvent(&event) != 0 && event.type == SDL_MOUSEBUTTONUP) return;

		// Calls ScrollBar's moveBar function.
		ScrollBar::moveBar();

		// Performs the movement in the galaxyPanels.
		performMovement(panelViewport, galaxyPanels, index);

		// Renders the ScrollBar.
		render(x, y);

	}
}

/*
Moves within the panelViewport based on the GalaxyPanelScrollBar's current distance.
*/
void GalaxyPanelScrollBar::performMovement(SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index) {
	*index = distance;

	// Assigns each galaxyPanel to the appropriate height.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

		// Exits the loop if an empty index is found.
		if (galaxyPanels[i] == nullptr) {
			break;

		}

		// Assigns each panel the correct height.
		galaxyPanels[i]->area.y = i * GALAXY_PANEL_HEIGHT - *index;

	}

	// Renders the galaxies' panels.
	renderPanelViewport(panelViewport, galaxyPanels, index);

}

/*
Creates the window for choosing galaxies. Manages the user interface for Galaxy Creation.

Modifies numGalaxies, minDistance, and galInstructions.

There are a few TextBoxes representing universeHeight, universeWidth, and minDistance.
Galaxies and their properties are conceptualized as Panels, which can be created by a handful of code buttons.
	Galaxies' Panels contain buttons that allow the user to access and modify their properties.
*/
ButtonCode customizeUniverse(int* minDistance, GalaxyInstruction** galInstructions) {

	// Clears the window and makes it grey.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderClear(renderer);

	// Creates the universe height and width boxes.
	TextBox* universeDimBoxes[NUM_GALAXY_CREATION_BOXES];
	universeDimBoxes[0] = new TextBox(0, 0, 120, 36, MIN_UNIVERSE_DIM, 1000000, std::to_string(universeHeight), "Universe Height");
	universeDimBoxes[1] = new TextBox(0, 40, 120, 36, MIN_UNIVERSE_DIM, 1000000, std::to_string(universeWidth), "Universe Width");
	universeDimBoxes[2] = new TextBox(0, 80, 120, 36, 0, 1000, std::to_string(*minDistance), "Intergalactic Space");

	// Creates the universes new galaxy and return buttons. The enums are found in the galaxy creation
	// screen, and correspond exactly the to galaxy type codes that GalaxyInstruction uses.
	CodeButton* universeCreateButtons[NUM_GALAXY_CREATION_BUTTONS];
	universeCreateButtons[0] = new CodeButton(0, 140, 120, 24, 100, 100, 100, (ButtonCode)RANDOM, "Add Random");
	universeCreateButtons[1] = new CodeButton(0, 170, 120, 24, 100, 100, 100, (ButtonCode)SPIRAL, "Add Spiral");
	universeCreateButtons[2] = new CodeButton(0, 200, 120, 24, 100, 100, 100, (ButtonCode)RING, "Add Ring");
	universeCreateButtons[3] = new CodeButton(0, 440, 120, 24, 100, 100, 100, UNI_CUSTOM_CLEAR, "Clear Galaxies");
	universeCreateButtons[4] = new CodeButton(0, 470, 120, 24, 100, 100, 100, UNI_CUSTOM_QUIT, "Finish");

	// Creates a group of panels representing galaxies to be added to the universe.
	// Also creates an int to keep track of where the user is in the list of galaxies.
	int index = 0;
	std::unique_ptr<NewGalaxyPanel>* galaxyPanels = new std::unique_ptr<NewGalaxyPanel>[MAXIMUM_GALAXIES];

	// Delineates the area wherein the panels will be shown and scrolling will move said panels.
	SDL_Rect panelViewport = {140, 0, 350, 500};

	// Creates the GalaxyPanelScrollBar for the panelViewport.
	GalaxyPanelScrollBar* panelScroll = new GalaxyPanelScrollBar(490, 0, 10, 500, 500, 0);

	// Adds all previously chosen galaxyInstructions to the current interface as NewGalaxyPanels.
	if (galInstructions != nullptr) loadGalaxyPanels(*galInstructions, galaxyPanels, &index, panelScroll);

	// Holds an active TextBox to monitor the user's text input.
	TextBox* activeTextBox = nullptr;

	// Holds an active CodeButton to monitor when the CodeButton should be recoloured.
	CodeButton* activeCodeButton = nullptr;

	// Running loop for the universe cusomization screen.
	SDL_Event event;
	while (true) {

		// Renders the screen.
		renderCustomizeUniverse(&index, panelViewport, universeDimBoxes, universeCreateButtons, galaxyPanels, panelScroll);

		// Event reading loop.
		while (SDL_PollEvent(&event) != 0) {

			// If there is an active TextBox, performs an action in it.
			if (activeTextBox != nullptr) {
				event = activeTextBox->textInput();

				// Reassign UniverseWidth, UniverseHeight, and some Panel's maxes/mins if any 
				// whenever a TextBox has been interacted with.
				reassignDimensions(universeDimBoxes, galaxyPanels);

			}

			// Quits if the X button is pressed.
			if (event.type == SDL_QUIT) return OPENING_QUIT;
			// Handles left clicking.
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// To begin, sets activeTextBox to nullptr.
				activeTextBox = nullptr;

				// Checks to see if any of the galaxy creation CodeButtons has been clicked. If one has,
				// sets it as active.
				activeCodeButton = getClickedButton(universeCreateButtons, NUM_GALAXY_CREATION_BUTTONS, xPos, yPos);

				// Checks to see if any of the universe dimension TextBoxes has been clicked.
				// If one has, sets it as active.
				activeTextBox = getClickedTextBox(universeDimBoxes, NUM_GALAXY_CREATION_BOXES, xPos, yPos);

				// Checks to see if any of the NewGalaxyPanel's TextBoxes or buttons has been clicked.
				// If a TextBox was clicked, sets it to activeTextBox and begins typing in it.
				// If a Button was clicked, sets it to activeCodeButton and recolours it.
				for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

					// Breaks if the end of the list has been reached.
					if (galaxyPanels[i] == nullptr) break;

					// Checks to see if the mouse is in bounds of any of the galaxy panels.
					// If it is, queries the panel to see if one of its TextBoxes has been clicked.
					if (withinBounds(xPos, yPos, galaxyPanels[i]->area)) {

						// Quits searching if a CodeButton or TextBox has been found.
						if (activeTextBox != nullptr) break;
						if (activeCodeButton != nullptr) break;

						// Attempts to find a clicked TextBox or CodeButton within the NewGalaxyPanels.
						activeTextBox = galaxyPanels[i]->getTextBox(0, 0, xPos, yPos);
						activeCodeButton = galaxyPanels[i]->getCodeButton(0, 0, xPos, yPos);

					}
				}

				// If there is an activeCodeButton, recolours it.
				if (activeCodeButton != nullptr) activeCodeButton->changeColour(40);

				// If the user has left clicked the GalaxyPanelScrollBar, calls its moveBar function.
				if (withinBounds(xPos, yPos, panelScroll->area) && numPanels(galaxyPanels) > 5)
					panelScroll->moveBar(0, 0, panelViewport, galaxyPanels, &index);

			}
			// Handles finishing a left click.
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Recolours the clicked button to its default, and sets it as inactive.
				if (activeCodeButton != nullptr) {
					activeCodeButton->changeColour(-40);
					activeCodeButton = nullptr;

				}

				// Finds out if any buttons within the galaxyPanels have been clicked.
				for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

					// Exits the loop if an empty index is found.
					if (galaxyPanels[i] == nullptr) break;

					// Performs the action appropriate to the returned ButtonCode.
					switch (galaxyPanels[i]->getButtonCode(0, 0, xPos, yPos)) {

					// Edits a Panel when the Edit Panel CodeButton is clicked.
					// Returns OPENING_QUIT if the galaxy design screen does.
					case(GAL_DESIGN):
						if (designGalaxy(&(galaxyPanels[i]->instr)) == OPENING_QUIT) return OPENING_QUIT;
						break;

					// Deletes a Panel when the Remove Panel CodeButton is clicked.
					case(UNI_CUSTOM_DELETE):
						removeGalaxyPanel(galaxyPanels, &index, i, panelScroll);
						break;

					}
				}

				// Checks to see if any of the buttons has been clicked. If one has, acts on its
				// ButtonCode.
				switch (getCode(universeCreateButtons, NUM_GALAXY_CREATION_BUTTONS, xPos, yPos)) {

				// Adds a NewRandomPanel.
				case (RANDOM):
					addGalaxyPanel(galaxyPanels, &index, RANDOM, panelScroll);
					break;
				// Adds a NewSpiralPanel.
				case (SPIRAL):
					addGalaxyPanel(galaxyPanels, &index, SPIRAL, panelScroll);
					break;
				// Adds a NewRingPanel.
				case(RING):
					addGalaxyPanel(galaxyPanels, &index, RING, panelScroll);
					break;
				// Clears the galaxy Panels.
				case (UNI_CUSTOM_CLEAR):
					clearGalaxyPanels(&index, galaxyPanels, panelScroll);
					break;
				// Deallocates resources, assigns minDistance and galInstructions, and returns.
				case (UNI_CUSTOM_QUIT):
					*minDistance = universeDimBoxes[2]->toInt();
					deallocateButtons(universeCreateButtons, NUM_GALAXY_CREATION_BUTTONS);
					deallocateBoxes(universeDimBoxes, NUM_GALAXY_CREATION_BOXES);
					delete panelScroll;
					*galInstructions = getPanelInstructions(galaxyPanels);
					return NULL_CODE;
				// Does nothing when no CodeButton is clicked.
				case (NULL_CODE):
					break;

				}
			}
			// Handles mouse scrolling. Specifically, handles scrolling through universePanels.
			else if (event.type == SDL_MOUSEWHEEL) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Scrolls only while within the panelViewport.
				if (withinBounds(xPos, yPos, panelViewport)) {

					// Scrolls up.
					if (event.wheel.y > 0) {
						decrementIndex(galaxyPanels, &index, 15, panelScroll);

					}
					// Scrolls down.
					else if (event.wheel.y < 0) {
						incrementIndex(galaxyPanels, &index, 15, panelScroll);

					}
				}
			}
		}
	}
}

/*
Adds all inputed GalaxyInstructions to galaxyPanels as NewGalaxyPanels. Does so by calling
addGalaxyPanel for each inputed GalaxyInstruction.
*/
void loadGalaxyPanels(GalaxyInstruction* galInstructions, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyPanelScrollBar* panelScroll) {

	// Calls addGalaxyPanel for each galInstruction.
	for (int i = 0; i < numGals; ++i) {
		addGalaxyPanel(galaxyPanels, galInstructions[i], index, panelScroll);

	}

	// Cleans up the galInstructions array.
	delete[] galInstructions;

}

/*
Renders the universe customization screen and all elements thereof.
*/
void renderCustomizeUniverse(int* index, SDL_Rect panelViewport, TextBox* universeDimBoxes[NUM_GALAXY_CREATION_BOXES],
	CodeButton* universeCreateButtons[NUM_GALAXY_CREATION_BUTTONS],	std::unique_ptr<NewGalaxyPanel>* galaxyPanels,
	GalaxyPanelScrollBar* panelScroll) {

	// Clears the screen.
	SDL_RenderClear(renderer);

	// Renders the galaxy creation CodeButtons.
	for (int i = 0; i < NUM_GALAXY_CREATION_BUTTONS; ++i) universeCreateButtons[i]->render(0, 0);

	// Renders the TextBoxes containing the universe's dimensions.
	for (int i = 0; i < NUM_GALAXY_CREATION_BOXES; ++i) universeDimBoxes[i]->render(0, 0);

	// Renders the NewGalaxyPanels.
	for (int i = *index; i < *index + 6 * GALAXY_PANEL_HEIGHT; i += GALAXY_PANEL_HEIGHT) {

		// Breaks when an empty spot is reached.
		if (galaxyPanels[i / GALAXY_PANEL_HEIGHT] == nullptr) break;
		// Breaks if the maximum has been reached.
		else if (i / GALAXY_PANEL_HEIGHT >= MAXIMUM_GALAXIES) break;
		// Renders all galaxyPanels.
		else galaxyPanels[i / GALAXY_PANEL_HEIGHT]->render(0, 0);

	}

	// Renders the GalaxyPanelScrollBar.
	panelScroll->render(0, 0);

	// Draws the background and renders everything.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderPresent(renderer);

}

/*
Renders the universe customization screen's panels and the backdrop of those panels.
*/
void renderPanelViewport(SDL_Rect panelViewport, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int *index) {

	// Renders the backdrop for the panelViewport.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderFillRect(renderer, &panelViewport);

	// Renders the NewGalaxyPanels.
	for (int i = *index; i < *index + 6 * GALAXY_PANEL_HEIGHT; i += GALAXY_PANEL_HEIGHT) {

		// Breaks when an empty spot is reached.
		if (galaxyPanels[i / GALAXY_PANEL_HEIGHT] == nullptr) break;
		// Breaks if the maximum has been reached.
		else if (i / GALAXY_PANEL_HEIGHT >= MAXIMUM_GALAXIES) break;
		// Renders all galaxyPanels.
		else galaxyPanels[i / GALAXY_PANEL_HEIGHT]->render(0, 0);

	}

	// Draws the background and renders everything.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderPresent(renderer);

}

/*
Reassigns universeWidth and universeHeight to their new values, if there are any. Also reassigns the
maximum dimensions of all galaxies if there was a change to universeHeight or universeWidth.
*/
void reassignDimensions(TextBox* universeDimBoxes[NUM_GALAXY_CREATION_BOXES], std::unique_ptr<NewGalaxyPanel>* galaxyPanels) {

	// Records the current maximum size for a galaxy, then changes universeWidth and universeHeight.
	int prevMaxSize = universeWidth < universeHeight ? universeWidth : universeHeight;
	universeWidth = universeDimBoxes[0]->toInt();
	universeHeight = universeDimBoxes[1]->toInt();

	// If the new maximum size is greater than the previous maximum size, quits without doing anything.
	int maxSize = universeWidth < universeHeight ? universeWidth : universeHeight;
	if (maxSize > prevMaxSize) return;

	// Reassigns the maximum size of each galaxy if a decrease of maximum size is recorded.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

	// Breaks if a null panel is found.
	if (galaxyPanels[i] == nullptr) break;

	// Reassigns the width and height for each galaxy.
	if (galaxyPanels[i].get()->instr.width >  maxSize) galaxyPanels[i].get()->instr.width = maxSize; // Width.
	if (galaxyPanels[i].get()->instr.height > maxSize) galaxyPanels[i].get()->instr.height = maxSize; // Height.

	}
}

/*
Adds a NewGalaxyPanel to the lowest unoccupied index of the galaxyPanels array.
*/
void addGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyType galType, GalaxyPanelScrollBar* panelScroll) {

	// Parses through the array until an empty spot is found.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

		// When an empty spot is found, resets it to a NewRandomPanel and increments the index if the
		// window cannot accomodate the new panel.
		if (galaxyPanels[i] == nullptr) {

			// Creates a panel of the inputed type.
			switch (galType) {

			// Creates a NewRandomPanel.
			case (RANDOM):
				galaxyPanels[i].reset(new NewGalaxyPanel((int)galType, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Random Galaxy"));
				break;

			// Creates a NewSpiralPanel.
			case (SPIRAL):
				galaxyPanels[i].reset(new NewGalaxyPanel((int)galType, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Spiral Galaxy"));
				break;

			// Creates a NewRingPanel.
			case (RING):
				galaxyPanels[i].reset(new NewGalaxyPanel((int)galType, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Ring Galaxy"));
				break;

			}

			// Adds the remove and edit buttons to all NewGalaxyPanels.
			galaxyPanels[i].get()->addButton(0, new CodeButton(175, 60, 155, 36, 100, 100, 100, UNI_CUSTOM_DELETE, "Remove Galaxy"));
			galaxyPanels[i].get()->addButton(1, new CodeButton(10, 60, 155, 36, 100, 100, 100, GAL_DESIGN, "Edit Galaxy"));

			// Adjusts the scroll bar and viewport.
			indexToBottom(galaxyPanels, index, panelScroll);
			break;

		}
	}
}

/*
Adds a NewGalaxyPanel to the lowest unoccupied index of the galaxyPanels array. Does so in the same
fashion as addGalaxyPanel, but adds galaxyPanels' TextBoxes with preinitialized values.
*/
void addGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, GalaxyInstruction galInstruction, int* index, GalaxyPanelScrollBar* panelScroll) {

	// Finds the maximum height and width for any galaxy.
	int maxSize = universeWidth < universeHeight ? universeWidth : universeHeight;

	// Parses through the array until an empty spot is found.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

		// When an empty spot is found, resets it to a NewRandomPanel and increments the index if the
		// window cannot accomodate the new panel.
		if (galaxyPanels[i] == nullptr) {

			// Creates a panel of the inputed type.
			switch (galInstruction.type) {

				// Creates a NewRandomPanel.
			case (RANDOM):
				galaxyPanels[i].reset(new NewGalaxyPanel(galInstruction.type, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Random Galaxy"));
				galaxyPanels[i].get()->instr = galInstruction;
				break;

				// Creates a NewSpiralPanel.
			case (SPIRAL):
				galaxyPanels[i].reset(new NewGalaxyPanel(galInstruction.type, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Spiral Galaxy"));
				galaxyPanels[i].get()->instr = galInstruction;
				break;

				// Creates a NewRingPanel.
			case (RING):
				galaxyPanels[i].reset(new NewGalaxyPanel(galInstruction.type, 2, 0, 1, 140, 0, 340, 100));
				galaxyPanels[i].get()->addSprite(0, new Sprite(0, 10, 120, 30, "New Ring Galaxy"));
				galaxyPanels[i].get()->instr = galInstruction;
				break;

			}

			// Adds the remove and edit buttons to all NewGalaxyPanels.
			galaxyPanels[i].get()->addButton(0, new CodeButton(175, 60, 155, 36, 100, 100, 100, UNI_CUSTOM_DELETE, "Remove Galaxy"));
			galaxyPanels[i].get()->addButton(1, new CodeButton(10, 60, 155, 36, 100, 100, 100, GAL_DESIGN, "Edit Galaxy"));

			// Adjusts the scroll bar and viewport.
			indexToBottom(galaxyPanels, index, panelScroll);
			break;

		}
	}
}

/*
Removes a NewGalaxyPanel from galaxyPanels and deletes it. Moves every following NewGalaxyPanel
downwards in galaxyPanels, and decrements its y coordinate. Sets index, distance, and barSpace
appropriately.
*/
void removeGalaxyPanel(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int i, GalaxyPanelScrollBar* panelScroll) {

	// Moves the items in every index following i one to the left.
	for (i; i < MAXIMUM_GALAXIES - 1; ++i) {
		galaxyPanels[i].reset(galaxyPanels[i + 1].release());

		// Breaks when the end of the NewGalaxyPanels is found.
		if (galaxyPanels[i] == nullptr) break;

		// Decrements the y coordinate of each moved panel, moving them all upwards.
		galaxyPanels[i]->area.y -= GALAXY_PANEL_HEIGHT;
	
	}

	// If deleting a NewGalaxyPanel would leave the viewport beyond the panels, decrements index.
	if (numPanels(galaxyPanels) * GALAXY_PANEL_HEIGHT - *index < 500) {
		decrementIndex(galaxyPanels, index, GALAXY_PANEL_HEIGHT, panelScroll);

	}

	// Decrements barSpace.
	panelScroll->barSpace -= GALAXY_PANEL_HEIGHT;

}

/*
Moves the index to the bottom of the list of galaxyPanels. Does so by reassigning index based
on the number of panels, then reassigning the height of the panels based on index.
*/
void indexToBottom(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, GalaxyPanelScrollBar* panelScroll) {

	// Moves index to the bottom.
	*index = (numPanels(galaxyPanels) - 4) * GALAXY_PANEL_HEIGHT - 60;

	// If index was negative, assigns it to 0.
	if (*index < 0) *index = 0;

	// Assigns each galaxyPanel to the appropriate height.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

		// Exits the loop if an empty index is found.
		if (galaxyPanels[i] == nullptr) break;

		// Assigns each panel the correct height.
		galaxyPanels[i]->area.y = i * GALAXY_PANEL_HEIGHT - *index;

	}

	// Assigns distance in panelScroll.
	panelScroll->barSpace += GALAXY_PANEL_HEIGHT;
	panelScroll->distance = panelScroll->barSpace - panelScroll->barSize;

}

/*
Returns the number of NewGalaxyPanels in the galaxyPanels array.
*/
int numPanels(std::unique_ptr<NewGalaxyPanel>* galaxyPanels) {

	// Counts the amount of NewGalaxyPanels in galaxyPanels.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {
		if (galaxyPanels[i] == nullptr) {
			return i;

		}
	}

	// If the array is full returns MAXIMUM_GALAXIES.
	return MAXIMUM_GALAXIES;

}

/*
Increments the index (location in galaxyPanels where rendering starts) and moves all panels accordingly.
This tantamounts to moving all NewGalaxyPanels upwards.
*/
void incrementIndex(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int distance, GalaxyPanelScrollBar* panelScroll) {

	// Moves index only when doing so does not push the viewing area above the current list of panels.
	if (*index < (numPanels(galaxyPanels) - 4) * GALAXY_PANEL_HEIGHT - 60) {

		// Moves each panel upwards.
		for (int j = 0; j < MAXIMUM_GALAXIES; ++j) {

			// Exits the loop if an empty index is found.
			if (galaxyPanels[j] == nullptr) break;

			// Decrements the y coordinate of each panel.
			galaxyPanels[j]->area.y -= distance;

		}

		// Increments index and panelScroll's distance.
		*index += distance;
		panelScroll->distance = *index;

	}
}

/*
Decrements the index (location in galaxyPanels where rendering starts) and moves all panels accordingly.
This tantamounts to moving all NewGalaxyPanels downwards.
*/
void decrementIndex(std::unique_ptr<NewGalaxyPanel>* galaxyPanels, int* index, int distance, GalaxyPanelScrollBar* panelScroll) {

	// Decrements index by distance only when doing so does not push the viewing area below 0.
	if (*index > distance) {

		// Moves each panel downwards.
		for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

			// Exits the loop if an empty index is found.
			if (galaxyPanels[i] == nullptr) break;

			// Increments the y coordinate of each panel.
			galaxyPanels[i]->area.y += distance;

		}

		// Decrements index and panelScroll's distance.
		*index -= distance;
		panelScroll->distance -= distance;

	}
	// If distance is greater than index, shifts down by index.
	else {

		// Moves each panel downwards.
		for (int i = 0; i < MAXIMUM_GALAXIES; ++i) {

			// Exits the loop if an empty index is found.
			if (galaxyPanels[i] == nullptr) break;

			// Decrements the y coordinate of each panel.
			galaxyPanels[i]->area.y += *index;

		}

		// Assigns index to 0 and panelScroll's distance to 0.
		*index = 0;
		panelScroll->distance = 0;

	}
}

/*
Clears all galaxy Panels in galaxyPanels, then resets the index and GalaxyPanelScrollBar.
*/
void clearGalaxyPanels(int* index, std::unique_ptr<NewGalaxyPanel>* galaxyPanels, GalaxyPanelScrollBar* panelScroll) {

	// Resets the contents of galaxyPanels.
	for (int i = 0; i < MAXIMUM_GALAXIES; ++i) galaxyPanels[i].reset();

	// Resets positions in the window.
	*index = 0;
	panelScroll->barSpace = 0;
	panelScroll->distance = 0;

}

/*
Converts an array of galaxy Panels to an array of GalaxyInstructions. Does so by copying each
NewGalaxyPanels instr into an array of GalaxyInstructions and returning it.
*/
GalaxyInstruction* getPanelInstructions(std::unique_ptr<NewGalaxyPanel>* galaxyPanels) {

	// numGals should equal the number of Panels created.
	numGals = numPanels(galaxyPanels);

	// Allocates the galInstructions array.
	GalaxyInstruction* galInstructions = new GalaxyInstruction[numGals];

	// Creates a GalaxyInstruction for each panel.
	// Deallocates each used galaxyPanel.
	for (int i = 0; i < numGals; ++i) {
		galInstructions[i] = galaxyPanels[i].get()->instr;
		galaxyPanels[i].reset();

	}

	// Deallocates the galaxyPanels array.
	delete[] galaxyPanels;

	// Returns galInstructions.
	return galInstructions;

}

/*
Chooses which galaxy design function to call based on the type of the inputed instruction.

TODO add more parameters to the galaxy design screens.
*/
ButtonCode designGalaxy(GalaxyInstruction* instr) {

	// Chooses the function.
	switch (instr->type) {

	// Calls designRandomGalaxy for Random galaxies.
	case(RANDOM):
		return designRandomGalaxy(instr);
	// Calls designSpiralGalaxy for Spiral galaxies.
	case(SPIRAL):
		return designSpiralGalaxy(instr);
	// Calls designRingGalaxy for Ring galaxies.
	case(RING):
		return designRingGalaxy(instr);

	}
}

/*
Screen for designing a Random galaxy. Sends the inputed instr and appropriate TextBoxes to the
designGalaxyLoop.

Allows the user to modify the Random galaxy's name, width, and height.
*/
ButtonCode designRandomGalaxy(GalaxyInstruction* instr) {

	// List of CodeButtons in the Random galaxy design screen.
	CodeButton* designButtons[NUM_RAND_DESIGN_BUTTONS];
	designButtons[0] = new CodeButton(0, 455, 200, 40, 100, 100, 100, GAL_DESIGN_FINISH, "FINISHED");

	// List of TextBoxes in the Random galaxy design screen.
	TextBox* designBoxes[NUM_RAND_DESIGN_BOXES];
	designBoxes[0] = new TextBox(0, 4, 120, 36, 0, universeWidth, std::to_string(instr->width), "Galaxy Width");
	designBoxes[1] = new TextBox(140, 4, 120, 36, 0, universeHeight, std::to_string(instr->height), "Galaxy Height");
	designBoxes[2] = new TextBox(0, 80, 120, 36, 0, 100, std::to_string(instr->starChance), "Star Chance");
	designBoxes[3] = new TextBox(140, 80, 120, 36, 0, 20, std::to_string(instr->numBarren), "Max Barrens");
	designBoxes[4] = new TextBox(280, 80, 120, 36, 0, 20, std::to_string(instr->numHabitable), "Max Habitable");
	designBoxes[5] = new TextBox(0, 120, 120, 36, 20, 100, std::to_string(instr->planetSize), "Planet Size");
	
	// Renders the galaxy design screen.
	SDL_RenderClear(renderer);
	renderDesignGalaxy(designButtons, NUM_RAND_DESIGN_BUTTONS, designBoxes, NUM_RAND_DESIGN_BOXES);

	// Enters the galaxy design loop and returns whatever ButtonCode it returns.
	return designGalaxyLoop(instr, designButtons, NUM_RAND_DESIGN_BUTTONS, designBoxes, NUM_RAND_DESIGN_BOXES);

}

/*
Screen for designing a Spiral galaxy. Sends the inputed instr and appropriate TextBoxes to the
designGalaxyLoop.

Allows the user to modify the Spiral galaxy's name, width (0-maxSize), number of arms (2-12), arm size (0-100), and spin (0-100).
*/
ButtonCode designSpiralGalaxy(GalaxyInstruction* instr) {

	// Finds the maximum height and width for any galaxy.
	int maxSize = universeWidth < universeHeight ? universeWidth : universeHeight;

	// List of CodeButtons in the Spiral galaxy design screen.
	CodeButton* designButtons[NUM_SPIRAL_DESIGN_BUTTONS];
	designButtons[0] = new CodeButton(0, 455, 200, 40, 100, 100, 100, GAL_DESIGN_FINISH, "FINISHED");

	// List of TextBoxes in the Spiral galaxy design screen.
	TextBox* designBoxes[NUM_SPIRAL_DESIGN_BOXES];
	designBoxes[0] = new TextBox(0, 4, 120, 36, 0, universeWidth, std::to_string(instr->width), "Galaxy Width");
	designBoxes[1] = new TextBox(140, 4, 120, 36, 2, 12, std::to_string(instr->numArms), "Number of Arms");
	designBoxes[2] = new TextBox(0, 44, 120, 36, 0, 100, std::to_string(instr->armSizeFraction), "Arm Size");
	designBoxes[3] = new TextBox(140, 44, 120, 36, 0, 100, std::to_string(instr->radFraction), "Galaxy Spin");
	designBoxes[4] = new TextBox(0, 80, 120, 36, 0, 100, std::to_string(instr->starChance), "Star Chance");
	designBoxes[5] = new TextBox(140, 80, 120, 36, 0, 20, std::to_string(instr->numBarren), "Max Barrens");
	designBoxes[6] = new TextBox(280, 80, 120, 36, 0, 20, std::to_string(instr->numHabitable), "Max Habitable");
	designBoxes[7] = new TextBox(0, 120, 120, 36, 0, 100, std::to_string(instr->planetSize), "Planet Size");

	// Renders the galaxy design screen.
	SDL_RenderClear(renderer);
	renderDesignGalaxy(designButtons, NUM_SPIRAL_DESIGN_BUTTONS, designBoxes, NUM_SPIRAL_DESIGN_BOXES);

	// Enters the galaxy design loop and returns whatever ButtonCode it returns.
	return designGalaxyLoop(instr, designButtons, NUM_SPIRAL_DESIGN_BUTTONS, designBoxes, NUM_SPIRAL_DESIGN_BOXES);

}

/*
Screen for designing a Ring galaxy. Sends the inputed instr and appropriate TextBoxes to the
designGalaxyLoop.

Allows the user to modify the Ring galaxy's name, width (0-maxSize), height (0-maxSize), and core size (0-100).
*/
ButtonCode designRingGalaxy(GalaxyInstruction* instr) {

	// Finds the maximum height and width for any galaxy.
	int maxSize = universeWidth < universeHeight ? universeWidth : universeHeight;

	// List of CodeButtons in the Ring galaxy design screen.
	CodeButton* designButtons[NUM_RING_DESIGN_BUTTONS];
	designButtons[0] = new CodeButton(0, 455, 200, 40, 100, 100, 100, GAL_DESIGN_FINISH, "FINISHED");

	// List of TextBoxes in the Ring galaxy desing screen.
	TextBox* designBoxes[NUM_RING_DESIGN_BOXES];
	designBoxes[0] = new TextBox(0, 4, 120, 36, 0, universeWidth, std::to_string(instr->width), "Galaxy Width");
	designBoxes[1] = new TextBox(140, 4, 120, 36, 0, universeHeight, std::to_string(instr->height), "Galaxy Height");
	designBoxes[2] = new TextBox(0, 44, 120, 36, 0, 100, std::to_string(instr->coreSizeFraction), "Core Size");
	designBoxes[3] = new TextBox(0, 80, 120, 36, 0, 100, std::to_string(instr->starChance), "Star Chance");
	designBoxes[4] = new TextBox(140, 80, 120, 36, 0, 20, std::to_string(instr->numBarren), "Max Barrens");
	designBoxes[5] = new TextBox(280, 80, 120, 36, 0, 20, std::to_string(instr->numHabitable), "Max Habitable");
	designBoxes[6] = new TextBox(0, 120, 120, 36, 0, 100, std::to_string(instr->planetSize), "Planet Size");

	// Renders the galaxy design screen.
	SDL_RenderClear(renderer);
	renderDesignGalaxy(designButtons, NUM_RING_DESIGN_BUTTONS, designBoxes, NUM_RING_DESIGN_BOXES);

	// Enters the galaxy design loop and returns whatever ButtonCode it returns.
	return designGalaxyLoop(instr, designButtons, NUM_RING_DESIGN_BUTTONS, designBoxes, NUM_RING_DESIGN_BOXES);

}

/*
Running loop for modifying any of the galaxy customization screen's TextBoxes or Buttons.
Modifies the inputed instr based on user input.
*/
ButtonCode designGalaxyLoop(GalaxyInstruction* instr, CodeButton* designButtons[], int numButtons, TextBox* designBoxes[], int numBoxes) {

	// Holds an active TextBox to monitor the user's text input.
	TextBox* activeTextBox = nullptr;

	// Holds an active CodeButton to monitor when the CodeButton should be recoloured.
	CodeButton* activeCodeButton = nullptr;

	// Running loop for the Random galaxy customization screen.
	SDL_Event event;
	while (true) {

		// Renders all changed items.
		SDL_RenderPresent(renderer);

		// Event reading loop.
		while (SDL_PollEvent(&event) != 0) {

			// If there is an active TextBox, performs an action in it.
			if (activeTextBox != nullptr) {
				event = activeTextBox->textInput();
				activeTextBox->renderBox(0, 0);

			}

			// Quits if the X button is pressed.
			if (event.type == SDL_QUIT)	return OPENING_QUIT;
			// Handles left clicking.
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// To begin, sets activeTextBox to nullptr.
				activeTextBox = nullptr;

				// Checks to see if any of the galaxy design CodeButtons has been clicked. If one has,
				// lightens its colour and sets it as active.
				for (int i = 0; i < numButtons; ++i) {
					if (withinBounds(xPos, yPos, designButtons[i]->area)) {
						activeCodeButton = designButtons[i];
						activeCodeButton->changeColour(40);
						activeCodeButton->render(0, 0);
						break;

					}
				}

				// Checks to see if any of the galaxy dimension TextBoxes has been clicked.
				// If one has, begins typing in it
				for (int i = 0; i < numBoxes; ++i) {
					if (withinBounds(xPos, yPos, designBoxes[i]->area)) {
						activeTextBox = designBoxes[i];

					}
				}
			}
			// Handles finishing a left click.
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Recolours the clicked button to its default, and sets it as inactive.
				if (activeCodeButton != nullptr) {
					activeCodeButton->changeColour(-40);
					activeCodeButton->render(0, 0);
					activeCodeButton = nullptr;

				}

				// Checks to see if any of the buttons has been clicked. If one has, acts on its
				// ButtonCode.
				for (int i = 0; i < numButtons; ++i) {
					if (withinBounds(xPos, yPos, designButtons[i]->area)) {

						// Adds a Panel of the desired type.
						switch (designButtons[i]->code) {

						// Assigns instr and quits the screen.
						case (GAL_DESIGN_FINISH):
							assignInstruction(instr, designBoxes);
							deallocateButtons(designButtons, numButtons);
							deallocateBoxes(designBoxes, numBoxes);
							return GAL_DESIGN_FINISH;

						}
					}
				}
			}
		}
	}
}

/*
Assigns an instruction for the designGalaxyLoop based on the inputed instr.
*/
void assignInstruction(GalaxyInstruction* instr, TextBox* designBoxes[]) {

	// Assigns the instruction based on the type of the inputed instr.
	switch (instr->type) {

	// Creates the instruction for a Random galaxy screen.
	case(RANDOM):
		*instr = {instr->type, designBoxes[0]->toInt(), designBoxes[1]->toInt(), 0, 0, 0, 0,
			designBoxes[2]->toInt(), designBoxes[3]->toInt(), designBoxes[4]->toInt(), designBoxes[5]->toInt()};
		break;
	// Creates the instruction for a Spiral galaxy screen.
	case(SPIRAL):
		*instr = {instr->type, designBoxes[0]->toInt(), 0, designBoxes[1]->toInt(), designBoxes[2]->toInt(), designBoxes[3]->toInt(), 0,
		designBoxes[4]->toInt(), designBoxes[5]->toInt(), designBoxes[6]->toInt(), designBoxes[7]->toInt()};
		break;
	// Creates the instruction for a Ring galaxy screen.
	case(RING):
		*instr = {instr->type, designBoxes[0]->toInt(), designBoxes[1]->toInt(), 0, 0, 0, designBoxes[2]->toInt(),
		designBoxes[3]->toInt(), designBoxes[4]->toInt(), designBoxes[5]->toInt(), designBoxes[6]->toInt()};
		break;

	}
}

/*
Renders the interface components of the Random galaxy design screen.
*/
void renderDesignGalaxy(CodeButton* designButtons[], int numButtons, TextBox* designBoxes[], int numBoxes) {

	// Draws the background.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_Rect screenRect = { 0, 0, 500, 500 };
	SDL_RenderFillRect(renderer, &screenRect);

	// Renders the CodeButtons.
	for (int i = 0; i < numButtons; ++i) designButtons[i]->render(0, 0);

	// Renders the TextBoxes.
	for (int i = 0; i < numBoxes; ++i) designBoxes[i]->render(0, 0);

	// Renders everything.
	SDL_RenderPresent(renderer);

}