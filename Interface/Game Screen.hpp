#pragma once

// Initializes the window for BigSpace and initializes all component viewports.
ButtonCode initGameScreen();

// Initializes and runs the gameplay loop for BigSpace.
ButtonCode interfaceLoop();

// Performs a command when ctrl and another key have been pressed.
void ctrlCommand(SDL_Event* event);

// Boolean tracking whether the user has pressed ctrl.
bool ctrl;

// Pointer to the View that the user is interacting with.
void (*view)(SDL_Event) = universeView;

// Tells the compiler about gameSpeed and highSpeedMode.
extern int gameSpeed;
extern bool highSpeedMode;

/*
Initializes the Game Screen.
Initializes the window, renderer, and textures, then and initializes the component views.
*/
ButtonCode initGameScreen() {

	// Initializes ctrl to false.
	ctrl = false;

	// Attempts to initialize the window. If initialization fails, returns OPENING_QUIT.
	screenWidth = 896;
	screenHeight = 896;
	if (!initGameWindow(screenWidth, screenHeight)) {
		printf("Failed to initialize window.");
		return OPENING_QUIT;

	}

	// Attempts to intialize sprites. If initialization fails, returns OPENING_QUIT.
	if (!initTextures()) {
		printf("Failed to initialize textures.");
		return OPENING_QUIT;

	}

	// Initializes the ToolBar View.
	initToolBarView();

	// Resizes the celestial view.
	resizeCelestialView(screenWidth, screenHeight);

	// Initializes the Universe View.
	initUniverseView();

	// Initializes the System View.
	initSystemView();

	// Initializes the Planet View.
	initPlanetView();

	// Initializes the Report View.
	initReportView();

	// Renders the above Views.
	SDL_RenderPresent(renderer);

}

/*
Contains the loop for the interface. The main thread will run here since SDL specifications require
that the thread that initialized video handling be the one that polls events.
TODO contemplate splitting a thread instead of using the main thread.
*/
ButtonCode interfaceLoop() {
	SDL_Event event;

	// Allows the gameLoop thread to begin.
	threadingSemaphore.post();

	// Will read events until the program exits.
	while (true) {

		// Event reader.
		if (SDL_WaitEvent(&event) != 0) {

			// Quits BigSpace if the X button is pressed.
			if (event.type == SDL_QUIT) {

				// Raises the exit flag.
				exitFlag = 1;

				// Joins the gameLoop thread.
				gameLoopThread->join();

				// Joins all threads.
				joinAllThreads();

				// Lowers the exit flag.
				exitFlag = 0;

				// Returns OPENING_QUIT.
				return OPENING_QUIT;

			}
			// When the user changes the window's size, reassigns screenWidth and screenHeight,
			// resizes each View and continues looping.
			// Also rerenders the screen when the window gains focus.
			else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					screenWidth = event.window.data1;
					screenHeight = event.window.data2;

				}

				// Resizes the Toolbar View and Universe View.
				resizeToolBarView(screenWidth, screenHeight);
				resizeUniverseView(screenWidth, screenHeight);

				// Resizes the celestial view.
				resizeCelestialView(screenWidth, screenHeight);

				// Resizes based on the current celestial view.
				if (activeHabitable != nullptr) {
					resizeSystemViewNoRender(screenWidth, screenHeight);
					resizePlanetView(screenWidth, screenHeight, activeHabitable->size);

				}
				else if (activeBarren != nullptr) {
					resizeSystemViewNoRender(screenWidth, screenHeight);
					resizePlanetView(screenWidth, screenHeight, activeBarren->size);

				}
				else resizeSystemView(screenWidth, screenHeight);

				// Renders the activity bars of each relevant View.
				if (view == systemView || view == habitableView) renderSystemActive();
				else renderSystemInactive();

				// Resizes the Report View.
				resizeReportView(screenWidth, screenHeight);

				// Renders the above to the window.
				SDL_RenderPresent(renderer);

				// Continues event handling.
				continue;

			}
			// If the user clicks or scrolls, checks which View they interacted with and makes that
			// the active one.
			else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP ||
				event.type == SDL_MOUSEWHEEL) {
				int xPos, yPos;
				SDL_GetMouseState(&xPos, &yPos);

				// Makes the Universe View active.
				if (withinBounds(xPos, yPos, universeViewport)) view = universeView;
				// Makes the ToolBar View active.
				if (withinBounds(xPos, yPos, toolBarViewport)) view = toolBarView;
				// Makes the System View or a Planet View active.
				if (withinBounds(xPos, yPos, systemViewViewport)) {
					if (activeHabitable != nullptr) view = habitableView;
					else if (activeBarren != nullptr) view = barrenView;
					else if (activeSystem != nullptr) view = systemView;
					renderSystemActive();

				}
				// Renders the activity bar as inactive if the System View is not active.
				else renderSystemInactive();
				// Makes the Report View active.
				if (withinBounds(xPos, yPos, reportViewport)) view = reportView;

				// Renders the above to the window.
				SDL_RenderPresent(renderer);

			}
			// Renders a new frame if an SDL_USEREVENT is inputted.
			else if (event.type == SDL_USEREVENT) {
				if (activeHabitable != nullptr) renderHabitableView();
				else if (activeBarren != nullptr) renderBarrenView();
				else if (activeSystem != nullptr) renderSystemView();
				renderUniverse();
				renderToolBar();
				renderReportView();
				SDL_RenderPresent(renderer);

			}
			// Assigns ctrl to true if a ctrl key has been pressed.
			else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_RCTRL || event.key.keysym.sym == SDLK_LCTRL)) {
				ctrl = true;
				continue;

			}
			// Assigns ctrl to false if a ctrl key has been lifted.
			else if (event.type == SDL_KEYUP && (event.key.keysym.sym == SDLK_RCTRL || event.key.keysym.sym == SDLK_LCTRL)) {
				ctrl = false;
				continue;

			}
			// Executes a ctrl move if ctrl has been pressed.
			else if (ctrl && event.type == SDL_KEYDOWN) {
				ctrlCommand(&event);
				continue;

			}
			// Pauses and unpauses the game when the Spacebar is pressed.
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
				gameSpeed = gameSpeed ? 0 : 1;

			}
			// Places the game in high speed mode when 0 is pressed.
			// TODO DEBUG REMOVE
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_0) {
				highSpeedMode = !highSpeedMode;

			}

			// If the user did not quit or resize the window, sends their input to the appropriate View.
			(*view)(event);

			// If a button was released, renders it again.
			if (event.type == SDL_MOUSEBUTTONUP && clickedButton != nullptr) {
				clickedButton->changeColour(-BUTTON_RECOLOUR);
				clickedButton->render();
				clickedButton = nullptr;
				SDL_RenderPresent(renderer);

			}

			// Changes the active view between System View and Planet View.
			if (view == systemView && activeSystem == nullptr) view = habitableView;

		}
	}
}

/*
Performs a command when ctrl and another key have been pressed.
*/
void ctrlCommand(SDL_Event* event) {

	// Exits from the current planet view if ctrl+q is pressed.
	if (event->key.keysym.sym == SDLK_q) {

		// Exits from the activeHabitable and renders activeSystem.
		if (activeHabitable != nullptr || activeBarren != nullptr) {
			view = systemView;
			enterSystemPlanet();

		}
		// TODO DEBUG REMOVE
		// Finds the next inhabited system.
		else changeActiveSystem(findNextCol());

	}
}