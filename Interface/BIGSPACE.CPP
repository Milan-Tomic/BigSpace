#pragma once

// Hunt for memory leaks.
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/

// SDL libs.
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// Standard libs.
#define _CRT_RAND_S  
#include <stdlib.h> 
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <bitset>
#include <vector>
#include <functional>

// BigSpace libs.
#include "Defines.hpp"
#include "Galaxy.hpp"
#include "Utilities.hpp"
#include "MultiThreading.hpp"
#include "LinkedList.hpp"
#include "Comparison Matrix.hpp"
#include "Pathfinding.hpp"
#include "Interface Tools.hpp"
#include "Populations.hpp"
#include "Commerce.hpp"
#include "PlanetSide.hpp"
#include "Galaxy Tiles.hpp"
#include "Government.hpp"
#include "Universe.hpp"
#include "Colonies.hpp"
#include "Ground Armies.hpp"
#include "Space Combat.hpp"
#include "Governments.hpp"
#include "Building Data.hpp"
#include "Planet Generator.hpp"
#include "System Generator.hpp"
#include "Empire Generator.hpp"
#include "Universe Generator.hpp"
#include "Interface Components.hpp"
#include "Interface Component Tools.hpp"
#include "Opening Screen.hpp"
#include "New Game Screen.hpp"
#include "Save and Load.hpp"
#include "View Components.hpp"
#include "ToolBar View.hpp"
#include "Planet View.hpp"
#include "Planet (Barren) View.hpp"
#include "System View.hpp"
#include "Universe View.hpp"
#include "Planet View.hpp"
#include "Report View.hpp"
#include "Game Screen.hpp"
#include "Climate Change.hpp"
#include "Planet Production.hpp"
#include "Government Action.hpp"
#include "Ground Battles.hpp"
#include "Space Battles.hpp"
#include "Gameplay Loop.hpp"

// DEBUG : Visualize planets.
// Uses two DEBUG functions in the Universe View.
void debugPlanets() {
	GalaxyInstruction instr;

	// Initializes the habitable planets.
	initHabitablePlanets(500);

	// Allocates canvases for planet generation.
	int** dummyPlanet = createCanvas(MAX_PLANET_WIDTH);
	int** moistureDummy = createCanvas(MAX_PLANET_WIDTH);
	int** heatDummy = createCanvas(MAX_PLANET_WIDTH);
	int** riverDummy = createCanvas(MAX_PLANET_WIDTH);
	int** buildingDummy = createCanvas(MAX_PLANET_WIDTH);
	int** canvas = createCanvas(MAX_PLANET_WIDTH + 2);
	for (int i = 0; i < MAX_PLANET_WIDTH + 2; ++i) {

		// Assigns each index of canvas to Ocean.
		for (int j = 0; j < MAX_PLANET_WIDTH + 2; ++j) canvas[i][j] = Ocean;

	}

	SDL_Event event;
	HabitablePlanet* planet = new HabitablePlanet();
	while (true) {

		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);

			// Deallocates the dummyPlanets and canvas.
			deallocateCanvas(dummyPlanet, MAX_PLANET_WIDTH);
			deallocateCanvas(moistureDummy, MAX_PLANET_WIDTH);
			deallocateCanvas(heatDummy, MAX_PLANET_WIDTH);
			deallocateCanvas(canvas, MAX_PLANET_WIDTH + 2);
			return;

		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_e:
				planet = generateNewPlanet(&instr, dummyPlanet, moistureDummy, heatDummy, riverDummy, buildingDummy, canvas, 0);
				renderUniversePlanet(planet);
				break;
			case SDLK_w:
				renderPlanet(planet->size, planet->planet);
				break;
			case SDLK_s:
				renderUniversePlanet(planet);
				break;
			}

		}

		// Renders the above to the screen.
		SDL_RenderPresent(renderer);

	}
}

// DEBUG : Visualize systems.
void debugSystems() {

}

// Debugs stuff.
void debugMain() {

	// Initializes the window.
	if (!initWindow(screenWidth, screenHeight)) {
		printf("failed window init.\n");
		return;

	}

	// Initializes the textures.
	initTextures();

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	// Debug interface.
	SDL_Event event;
	while (true) {

		// Event reading.
		while (SDL_PollEvent(&event) != 0) {

			// Reads keys.
			if (event.type == SDL_KEYDOWN) {

				// read user input.
				switch (event.key.keysym.sym) {
				
				// Access planet interface with p.
				case SDLK_p:
					debugPlanets();
					break;

				}
			}
			// Quits.
			else if (event.type == SDL_QUIT) return;

		}
	}


}

/*
Main method. Manages transitions between screens.
*/
int main(int argc, char* args[]) {

	// Ensures that rand is random.
	srand(time(NULL));

	// Initializes multithreading items.
	initThreading();

	// Initializes the coordLists.
	initCoordLists();

	///////////////////////////////
	// FOR TESTING PURPOSES ONLY //
	///////////////////////////////
	//debugMain();
	//return 0;

	// Attempts to initialize the window. If initialization fails, returns OPENING_QUIT.
	if (!initWindow(500, 500)) return OPENING_QUIT;

	// Sets the first screen to the opening screen.
	ButtonCode buttonInstruction = OPENING_SCREEN;

	// Loop that performs actions based on the user's current place in the game's screens.
	// Exits when any screen returns OPENING_QUIT.
	while (true) {

		// Opens the performs an action based on user input.
		switch (buttonInstruction) {

		// Opens the opening screen.
		case (OPENING_SCREEN):
			buttonInstruction = openingScreen();
			break;
		// Opens the new game screen.
		case (OPENING_NEW_GAME):
			buttonInstruction = newGameStart();

			// TODO Automatically saves the current universe.
			 saveFile("Save");
			break;
		// TEMP loads from the default file.
		case (OPENING_LOAD_GAME):
			loadFile("Save");
			buttonInstruction = NEW_GAME_PLAY;
			break;
		// TEMP Does nothing.
		case (OPENING_OPTIONS):
			buttonInstruction = OPENING_SCREEN;
			break;
		// Opens the game screen and starts the gameplay loop.
		case (NEW_GAME_PLAY):
			threadingSemaphore.count = 0;
			threadingSemaphore.crit = 1;
			closeWindow();
			view = universeView;
			if (initGameScreen() == OPENING_QUIT) return OPENING_QUIT;
			gameLoopThread = new std::thread(gameLoop);
			buttonInstruction = interfaceLoop();
			break;
		// Quits.
		case (OPENING_QUIT):
			return 0;
			break;

		}
	}
}