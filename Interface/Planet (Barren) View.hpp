#pragma once

// Handles events within the Planet (Barren) View.
void barrenView(SDL_Event event);

// Changes the activeBarren.
void changeActiveBarren(BarrenPlanet* newPlanet);

/*
This header is responsible for managing the user's interaction with barren planets.
NOTE: It borrows heavily from the Planet View.
*/

/*
Handles events within the Planet (Barren) View.
*/
void barrenView(SDL_Event event) {

	//If the user presses a key, moves them within the systemViewport.
	if (event.type == SDL_KEYDOWN) {

		// Moves within the viewport if WASD, the movement keys, or E and Q are pressed.
		// Then renders activeBarren.
		planetMovement(event, activeBarren->size);
		renderBarren();
		SDL_RenderPresent(renderer);

		// Does nothing if the user presses any other key.

	}
	// Manages scrolling if user uses the mousewheel.
	else if (event.type == SDL_MOUSEWHEEL) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Scrolls within the systemViewport and renders activeBarren.
		planetScrolling(xPos, yPos, event, activeBarren->size);
		renderBarren();
		SDL_RenderPresent(renderer);

	}
	// Manages clicking.
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);


	}

}

/*
Changes the activeBarren. Ensures that activeBarren can fit within the systemViewport.
*/
void changeActiveBarren(BarrenPlanet* newPlanet) {
	activeBarren = newPlanet;

	// Zooms until activeBarren can fit within the systemViewport.
	while (systemViewport.w / planetTileSize > activeBarren->size || systemViewport.h / planetTileSize > activeBarren->size) {
		if (planetTileSize < MAX_TILE_SIZE) {
			planetX += (universeViewport.w / planetTileSize) / 4;
			planetY += (universeViewport.h / planetTileSize) / 4;
			planetTileSize *= 2;

		}
	}

	// Ensures that the above zooming does not result in an out of bounds error.
	checkPlanetRight(activeBarren->size);
	checkPlanetDown(activeBarren->size);

	// Renders the planet.
	renderBarren();
	SDL_RenderPresent(renderer);

}

/*
Renders the barren view including activeBarren and the relevant buttons.
*/
void renderBarrenView() {

	// Renders activeBarren.
	renderBarren();

}

/*
Renders activeBarren within the System View.
*/
void renderBarren() {
	SDL_Rect tile;
	SDL_Rect sprite;

	// Renders each tile on the planet.
	int index = 0;
	for (int i = 0; i < systemViewport.w; i += planetTileSize) {
		if (index / planetTileSize + planetX >= activeBarren->size) index = -planetX * planetTileSize;
		for (int j = 0; j < systemViewport.h; j += planetTileSize) {
			tile = { i + systemViewport.x, j + systemViewport.y, planetTileSize, planetTileSize };
			sprite = activeBarren->getSprite();
			SDL_RenderCopy(renderer, tileSheet, &sprite, &tile);

		}
		index += planetTileSize;

	}
}