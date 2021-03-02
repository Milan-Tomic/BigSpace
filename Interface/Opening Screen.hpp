#pragma once

// Initializes and runs the opening screen for BigSpace.
ButtonCode openingScreen();

// Resets the colour of the Openingbuttons.
void resetOpeningColours(CodeButton* openingButtons[], int numCodeButtons);

// Number of buttons on the opening screen.
#define NUM_OPENING_BUTTONS 4

/*
Opening screen for BigSpace.
*/
ButtonCode openingScreen() {

	// Initializes the Opening Buttons.
	CodeButton* openingButtons[NUM_OPENING_BUTTONS];
	openingButtons[0] = new CodeButton(150, 86, 200, 40, 80, 80, 80, OPENING_NEW_GAME, "New Game");
	openingButtons[1] = new CodeButton(150, 172, 200, 40, 80, 80, 80, OPENING_LOAD_GAME, "Load Game");
	openingButtons[2] = new CodeButton(150, 258, 200, 40, 80, 80, 80, OPENING_OPTIONS, "Options");
	openingButtons[3] = new CodeButton(150, 344, 200, 40, 80, 80, 80, OPENING_QUIT, "Quit");

	// Running loop for the Opening Screen.
	SDL_Event event;
	while (true) {

		// Renders all buttons to the window.
		SDL_RenderClear(renderer);
		for (int i = 0; i < NUM_OPENING_BUTTONS; ++i) openingButtons[i]->render(0, 0);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);
		
		// Event reading loop.
		while (SDL_PollEvent(&event) != 0) {

			// Quits BigSpace if the X button is pressed.
			if (event.type == SDL_QUIT) return OPENING_QUIT;
			// Handles left clicking.
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Checks to see if any of the buttons has been clicked. If one has, lightens its colour.
				for (int i = 0; i < NUM_OPENING_BUTTONS; ++i) {
					if (withinBounds(xPos, yPos, openingButtons[i]->area)) {
						openingButtons[i]->setColour(120, 120, 120);

					}
				}
			}
			// Handle's finishing a left click.
			else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Checks to see if any of the buttons has been clicked. If one has, returns its
				// ButtonCode.
				for (int i = 0; i < NUM_OPENING_BUTTONS; ++i) {
					if (withinBounds(xPos, yPos, openingButtons[i]->area)) {
						int returnCode = openingButtons[i]->code;

						// Deallocates the openingButtons.
						deallocateButtons(openingButtons, NUM_OPENING_BUTTONS);

						// Returns a ButtonCode for the interface to handle.
						return (ButtonCode)returnCode;

					}
				}

				// Recolours all buttons to the default.
				resetOpeningColours(openingButtons, NUM_OPENING_BUTTONS);
				
			}
		}
	}
}

/*
Resets the colour of all opening buttons to 80, 80, 80.
*/
void resetOpeningColours(CodeButton* openingButtons[], int numCodeButtons) {

	// Resets all buttons' colours.
	for (int i = 0; i < numCodeButtons; ++i) {
		openingButtons[i]->setColour(80, 80, 80);

	}
}