#pragma once

// Size (in pixels) of any tile as rendered in the universe viewport.
int universeTileSize;

// Maximum size of a GalaxyTile in the renderer.
// 64 is the pixel size of the sprites.
#define MAX_TILE_SIZE 64

// Initializes the Universe View.
void initUniverseView();

// Handles events within the universe view.
void universeView(SDL_Event event);

// Manages movement within the universe.
void universeMovement(SDL_Event event);

// Manages scrolling within the universe.
void universeScrolling(int xPos, int yPos, SDL_Event event);

// Enters a system after the user clicks it.
void enterSystem(int xPos, int yPos);

// Finds the tile that the mouse has clicked.
// The tile will be placed in xPos and yPos.
void findUniverseCoords(int* xPos, int* yPos);

// Resizes the Universe View to fit in a window of the inputed width and height.
void resizeUniverseView(int windowW, int windowH);

// Makes sure that universeX will not result in a left side out of bounds error.
inline void checkUniverseLeft();

// Makes sure that universeX will not result in a right side out of bounds error.
inline void checkUniverseRight();

// Makes sure that universeY will not result in a upper side out of bounds error.
inline void checkUniverseUp();

// Makes sure that universeY will not result in a lower side out of bounds error.
inline void checkUniverseDown();

// Renders the universe to the window.
void renderUniverse();

// Renders a system on the universe level.
void renderUniverseSystem(SDL_Rect tile, int x, int y);

/*
Initializes the Universe View.
*/
void initUniverseView() {

	// Initializes universe viewport coordinates.
	universeX = 0;
	universeY = 0;

	// Sets the user to the maximum zoom level by default.
	universeTileSize = 2;

	// Resizes and renders the Universe View.
	resizeUniverseView(screenWidth, screenHeight);

}

/*
///////////
// DEBUG //
///////////
Prints some information about a certain tile when it is right clicked.
*/
void debugUniverseTile(int xPos, int yPos) {
	Squadron* squadron;

	// Prints general data.
	printf("tile      : {%6d,%6d}\n", xPos, yPos);
	printf("closure   : %16d\n", uIndex(xPos, yPos).closure);
	printf("  start   : {%6d,%6d}\n", uClosure(xPos, yPos).start.x, uClosure(xPos, yPos).start.y);
	printf("owner   : %16p\n", uOwner(xPos, yPos));

	// Prints squadrons.
	///*
	printf("numSquadrons : %16d\n", uIndex(xPos, yPos).numSquadrons);
	for (int i = 0; i < uIndex(xPos, yPos).numSquadrons; ++i) {
		squadron = uSquadron(xPos, yPos, i);

		// Prints owner.
		printf("owner : %16p\n", squadron->owner);

		// Prints ships.
		/*
		if (squadron->numShips) printf("   ships  : %16d\n", squadron->numShips);
		for (int j = 0; j < squadron->numShips; ++j) printf("      num : %16d\n", squadron->ships[j].num);
		//*/

		// Prints activity.
		///*
		{
			printf("activity : ");
			if (squadron->behaviour == nullptr) printf("null");
			else if (squadron->behaviour == squadronIdle) printf("squadronIdle");
			else if (squadron->behaviour == squadronOccupy) printf("squadronOccupy");
			else if (squadron->behaviour == squadronGuard) printf("squadronGuard");
			else if (squadron->behaviour == squadronPursue) {
				printf("squadronPursue d(%d, %d) l(%d, %d)", squadron->getDest().x, squadron->getDest().y,
					(*((Squadron**)squadron->data))->loc.x, (*((Squadron**)squadron->data))->loc.y);

			}
			else if (squadron->behaviour == squadronMove) printf("squadronMove");
			else if (squadron->behaviour == squadronInvadeOrcish) printf("squadronInvadeOrcish");
			else printf("undefined");
			printf("\n");
		}
		//*/

		// Prints threats.
		/*
		printf("threats : ");
		for (Threat t : squadron->threats) printf("(%d, %d) %d, ", t.coord.x, t.coord.y, t.time);
		printf("\n");
		//*/
	}
	//*/

	// Prints the contents of tradeLanes.
	/*
	printf("tradeLanes : ");
	int currPos = 0;
	if (uSystem(xPos, yPos)) {
		while (uSystem(xPos, yPos)->tradeLanes[currPos].x != 0 || uSystem(xPos, yPos)->tradeLanes[currPos].y != 0) {
			int bound = uSystem(xPos, yPos)->tradeLanes[currPos].x + currPos + 1;
			for (int i = currPos; i < bound; ++i) {
				printf("(%d, %d) ", uSystem(xPos, yPos)->tradeLanes[i].x, uSystem(xPos, yPos)->tradeLanes[i].y);
				++currPos;

			}
		}
	}
	printf("\n");
	//*/

	/////////////////////////////////////////////////////////////////////
	// TODO Remember to remove the debug things in renderUniverse TODO //
	/////////////////////////////////////////////////////////////////////

}

/*
Handles events within the Universe View.
	Moves with WASD, movement keys.
	Zooms with Q, E, and MouseWheel.

	Renders after each action.
*/
void universeView(SDL_Event event) {

	//If the user presses a key, moves them within the universe viewport.
	if (event.type == SDL_KEYDOWN) {

		// Moves within the viewport if WASD, the movement keys, or E or Q are pressed.
		universeMovement(event);

		// Does nothing if the user presses any other key.

	}
	// Manages scrolling if user uses the mousewheel.
	else if (event.type == SDL_MOUSEWHEEL) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Scrolls within the universeViewport.
		universeScrolling(xPos, yPos, event);

	}
	// Manages clicking.
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int xPos, yPos;
		SDL_GetMouseState(&xPos, &yPos);

		// Gets mouse position in terms of tiles.
		findUniverseCoords(&xPos, &yPos);

		///////////
		// DEBUG //
		///////////
		if (event.button.button == SDL_BUTTON_RIGHT) debugUniverseTile(xPos, yPos);

		// Enters any clicked star system.
		enterSystem(xPos, yPos);

	}
	// Manages releasing a click.
	else if (event.type == SDL_MOUSEBUTTONUP) {

		// TODO empty for now

	}
}

/*
Manages movement with the WASD, movement, and QE keys.
TODO return to home planet with r key.
*/
void universeMovement(SDL_Event event) {

	// Moves within the universe viewport based on key presses.
	switch (event.key.keysym.sym) {

	// Moves left.
	// Will not move such that the view extends beyond the universe's left bound.
	case SDLK_LEFT:
	case SDLK_a:
		universeX -= 128 / universeTileSize;
		checkUniverseLeft();
		break;

	// Moves right.
	// Will not move such that the view extends beyond the universe's right bound.
	case SDLK_RIGHT:
	case SDLK_d:
		universeX += 128 / universeTileSize;
		checkUniverseRight();
		break;

	// Moves up.
	// Will not move such that the View extends beyond the universe's upper bound.
	case SDLK_UP:
	case SDLK_w:
		universeY -= 128 / universeTileSize;
		checkUniverseUp();
		break;

	// Moves down.
	// Will not move such that the View extends beyond the universe's lower bound.
	case SDLK_DOWN:
	case SDLK_s:
		universeY += 128 / universeTileSize;
		checkUniverseDown();
		break;

	// Zooms in on center if not below the minimum width.
	case SDLK_e:

		// Increases the size of rendered GalaxyTiles if the size is below MAX_TILE_SIZE.
		if (universeTileSize < MAX_TILE_SIZE) {
			universeX += (universeViewport.w / universeTileSize) / 4;
			universeY += (universeViewport.h / universeTileSize) / 4;
			universeTileSize *= 2;

		}
		break;

	// Zooms out on center.
	case SDLK_q:

		// Increases the size of rendered tiles.
		// Will do so only when the map can accomodate the new number of tiles.
		if (universeTileSize > 1 && !(universeViewport.w / (universeTileSize / 2) > universeWidth)
			&& !(universeViewport.h / (universeTileSize / 2) > universeHeight)) {
			universeTileSize /= 2;
			universeX -= (universeViewport.w / universeTileSize) / 4;
			universeY -= (universeViewport.h / universeTileSize) / 4;
			 
			// Ensures that universeX and universeY are not out of bounds.
			checkUniverseLeft();
			checkUniverseRight();
			checkUniverseUp();
			checkUniverseDown();

		}
		break;

	}

	// Renders the universe after moving.
	renderUniverse();
	SDL_RenderPresent(renderer);

}

/*
Manages mouse scrolling (zooming) within the universeViewport.
Proactively protects against out of bounds errors.

Scrolling is done by changing the size of rendered tiles and the location of universeX and universeY.
*/
void universeScrolling(int xPos, int yPos, SDL_Event event) {

	// Zooms in to the universeViewport when below maximum zoom.
	if (event.wheel.y > 0 && universeTileSize < MAX_TILE_SIZE) {
		universeX += ((xPos - universeViewport.x) / universeTileSize) / 2;
		universeY += ((yPos - universeViewport.y) / universeTileSize) / 2;
		universeTileSize *= 2;

	}
	// Zooms out for the universeViewport if above minimum zoom and doing so will not result in an
	// out of bounds error.
	else if (event.wheel.y < 0 && universeTileSize > 1 && !(universeViewport.w / (universeTileSize / 2) > universeWidth) &&
		!(universeViewport.h / (universeTileSize / 2) > universeHeight)) {
		universeTileSize /= 2;
		universeX -= ((float)(xPos - universeViewport.x) / universeTileSize) / 2;
		universeY -= ((float)(yPos - universeViewport.y) / universeTileSize) / 2;

	}

	// Makes sure that the scrolling movement does not move the universeViewport out of bounds.
	checkUniverseLeft();
	checkUniverseRight();
	checkUniverseUp();
	checkUniverseDown();

	// Renders the universe after scrolling.
	renderUniverse();
	SDL_RenderPresent(renderer);

}

/*
Enters a system after the user clicks it.
*/
void enterSystem(int xPos, int yPos) {

	// If the user has clicked a SYSTEM_TILE, assigns it as activeSystem and renders it.
	if (uIndex(xPos, yPos).getTileID() == SYSTEM_TILE) {
		changeActiveSystem(uIndex(xPos, yPos).system);

	}
}

/*
Finds the tile under the corresponding mouse position. The tile will be placed in xPos and yPos.
*/
void findUniverseCoords(int* xPos, int* yPos) {

	// Converts xPos and yPos to offsets within the universeViewport.
	*xPos -= universeViewport.x;
	*yPos -= universeViewport.y;

	// Converts pixel data to Tile data.
	*xPos /= universeTileSize;
	*yPos /= universeTileSize;

	// Adds offset from the view's position.
	*xPos += universeX;
	*yPos += universeY;

}

/*
Resizes the Universe View to fit in a window of the inputed width and height.
Renders the Universe View after resizing.
*/
void resizeUniverseView(int windowW, int windowH) {

	// Assigns universeViewport.
	universeViewport = {0, toolBarViewport.h, screenWidth - 384, screenHeight - toolBarViewport.h - 384};

	// Makes sure that the universeViewport's dimensions are divisible by MAX_TILE_SIZE.
	universeViewport.w -= universeViewport.w % MAX_TILE_SIZE;
	universeViewport.h -= universeViewport.h % MAX_TILE_SIZE;

	// Zooms until the universe can fit within the universeViewport.
	while (universeViewport.w / universeTileSize > universeWidth || universeViewport.h / universeTileSize > universeHeight) {
		if (universeTileSize < MAX_TILE_SIZE) {
			universeX += (universeViewport.w / universeTileSize) / 4;
			universeY += (universeViewport.h / universeTileSize) / 4;
			universeTileSize *= 2;

		}
	}

	// Ensures that the above zooming does not result in an out of bounds error.
	checkUniverseRight();
	checkUniverseDown();

	// Renders the resized universe view.
	renderUniverse();

}

/*
Makes sure that universeX will not result in a left side out of bounds error.
*/
inline void checkUniverseLeft() { if (universeX < 0) universeX = 0; }

/*
Makes sure that universeX will not result in a right side out of bounds error.
*/
inline void checkUniverseRight() {
	if (universeX > universeWidth - universeViewport.w / universeTileSize)
		universeX = universeWidth - universeViewport.w / universeTileSize;

}

/*
Makes sure that universeY will not result in a upper side out of bounds error.
*/
inline void checkUniverseUp() { if (universeY < 0) universeY = 0; }

/*
Makes sure that universeY will not result in a lower side out of bounds error.
*/
inline void checkUniverseDown() {
	if (universeY > universeHeight - universeViewport.h / universeTileSize)
		universeY = universeHeight - universeViewport.h / universeTileSize;

}

/*
Chooses the appropriate sprite for a given ship number.
*/
inline SDL_Rect getShipSprite(int shipSprite) {
	return { SMALL_SPRITE_SIZE * (shipSprite % SMALL_SPRITE_SIZE),
		SMALL_SPRITE_SIZE* (shipSprite / SMALL_SPRITE_SIZE),
		SMALL_SPRITE_SIZE, SMALL_SPRITE_SIZE };

}

/*
Renders the Ships in a GalaxyTile.
*/
inline void renderShips(int x, int y, GalaxyTile* tile) {
	SDL_Rect shipSprite;
	int currShip = 0;
	int numShips;

	// Does nothing if the tile contains no Ships.
	if (!tile->numSquadrons) return;

	// Renders all Ships in the GalaxyTile.
	// TODO implement

	// TODO DEBUG REMOVE
	numShips = tile->squadrons[0]->ships[0].num;
	numShips = numShips > 255 ? 255 : numShips;

	// TODO DEBUG REMOVE
	shipSprite = getShipSprite(tile->squadrons[0]->ships[0].type.sprite);

	// TODO DEBUG REMOVE renders ships
	for (int h = 0; h < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++h) {
		for (int w = 0; w < SPRITE_SIZE / SMALL_SPRITE_SIZE; ++w) {
			SDL_Rect tile = { x + universeViewport.x + w * universeTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
				y + universeViewport.y + h * universeTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE),
				universeTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE), universeTileSize / (SPRITE_SIZE / SMALL_SPRITE_SIZE) };
			SDL_RenderCopy(renderer, shipSheet, &shipSprite, &tile);

			// Exits the loop when the correct number of units have been rendered.
			if (++currShip >= numShips) return;

		}
	}
}

/*
Renders the universe to be displayed.
*/
void renderUniverse() {
	SDL_Rect tile;
	SDL_Rect sprite;
	int x, y;

	// Renders each tile in the universe.
	for (int i = 0; i < universeViewport.w; i += universeTileSize) {
		x = i / universeTileSize + universeX;
		uLock(x, 0);
		for (int j = 0; j < universeViewport.h; j += universeTileSize) {
			y = j / universeTileSize + universeY;

			// Renders the tile.
			tile = { i + universeViewport.x, j + universeViewport.y, universeTileSize, universeTileSize };
			sprite = uIndex(x, y).getSprite();

			// TODO DEBUG REMOVE
			// Shows paths from current system outwards.
			for (int k = 0; activeSystem && (activeSystem->tradeLanes[k].x || activeSystem->tradeLanes[k].y); ++k) {
				if (x == activeSystem->loc.x + activeSystem->tradeLanes[k].x &&
					y == activeSystem->loc.y + activeSystem->tradeLanes[k].y) {
					sprite = uIndex(0, 0).getSprite();

				}
			}
			SDL_RenderCopy(renderer, tileSheet, &sprite, &tile);

			// Renders the tile's owner.
			if (uController(x, y)) {
				SDL_Colour governmentColour = uController(x, y)->flagColour;
				SDL_SetRenderDrawColor(renderer, governmentColour.r, governmentColour.g, governmentColour.b, governmentColour.a);
				SDL_RenderFillRect(renderer, &tile);

			}

			// If the tile was a SystemSpaceTile, calls renderUniverseSystem.
			if (uIndex(x, y).getTileID() == SYSTEM_TILE) renderUniverseSystem(tile, x, y);

			// Renders the tile's Ships.
			renderShips(i, j, &uIndex(x, y));

		}
		uUnlock(x, 0);
	}
}

/*
Renders a system on the universe level.
*/
void renderUniverseSystem(SDL_Rect tile, int x, int y) {
	SDL_Rect starArea;
	SDL_Rect sprite;

	// Renders only a yellow square if zoom is sufficiently far out.
	if (universeTileSize <= 1) {
		starArea = {192, 0, 64, 64};
		SDL_RenderCopy(renderer, tileSheet, &starArea, &tile);
		return;

	}

	// Makes it such that the stars are rendered only in a portion of the tile.
	tile = {tile.x + tile.w / 8, tile.y + tile.h / 8, (6 * tile.w) / 8, (6 * tile.h) / 8};

	// Gets the current tile.
	System* system = uSystem(x, y);

	// Renders the system's Star's to the universe map depending on their number.
	switch (system->numStars) {

	// Renders a SystemSpaceTile with one Star.
	case(1):
		starArea = {tile.x + tile.w / 4, tile.y + tile.h / 4, tile.w / 2, tile.h / 2};
		sprite = system->stars[0].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		break;
	// Renders a SystemSpaceTile with two SystemStarTiles.
	case(2):
		starArea = {tile.x, tile.y, tile.w / 2, tile.h / 2};
		sprite = system->stars[0].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x + starArea.w, tile.y + starArea.h, starArea.w, starArea.h};
		sprite = system->stars[1].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		break;
	// Renders a SystemSpaceTile with three SystemStarTiles.
	case(3):
		starArea = {tile.x + tile.w / 4, tile.y, tile.w / 2, tile.h / 2};
		sprite = system->stars[0].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x, tile.y + tile.h / 2, tile.w / 2, tile.h / 2};
		sprite = system->stars[1].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x + tile.w / 2, tile.y + tile.h / 2, tile.w / 2, tile.h / 2};
		sprite = system->stars[2].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		break;
	// Renders a SystemSpaceTile with four SystemStarTiles.
	case(4):
		starArea = {tile.x, tile.y, tile.w / 2, tile.h / 2};
		sprite = system->stars[0].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x + tile.w / 2, tile.y, tile.w / 2, tile.h / 2};
		sprite = system->stars[1].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x, tile.y + tile.h / 2, tile.w / 2, tile.h / 2};
		sprite = system->stars[2].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		starArea = {tile.x + tile.w / 2, tile.y + tile.h / 2, tile.w / 2, tile.h / 2};
		sprite = system->stars[3].getSprite();
		SDL_RenderCopy(renderer, tileSheet, &sprite, &starArea);
		break;

	}
}

/*
Renders a planet. Used for debugging.
TODO DEBUG REMOVE
*/
void renderPlanet(int size, PlanetTile* planet) {
	SDL_Rect temp;
	SDL_Rect sprite;

	// Clears
	SDL_RenderClear(renderer);

	int tileSize = screenWidth / size;

	// Renders each tile on a planet.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			temp = { i * tileSize, j * tileSize, tileSize, tileSize };
			sprite = planet[index(i, j, size)].getSprite();
			SDL_RenderCopy(renderer, tileSheet, &sprite, &temp);

		}
	}
}

/*
Renders a planet on the universe level.
TODO DEBUG REMOVE
*/
void renderUniversePlanet(HabitablePlanet* planet) {

	SDL_RenderClear(renderer);

	SDL_Texture* planetTexture = planet->getPlanetTexture();

	SDL_Rect bemp = {0, 0, planet->size, planet->size};
	SDL_RenderCopy(renderer, planetTexture, NULL, &bemp);

	SDL_RenderPresent(renderer);

}