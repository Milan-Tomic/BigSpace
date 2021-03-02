#pragma once

// Main window for the application.
SDL_Window* window;

// The renderer for the application.
SDL_Renderer* renderer = NULL;

// Spritesheet containing the sprites for BigSpace's tiles.
// Sprites are 64x64.
SDL_Texture* tileSheet;

// Spritesheet containing the sprites for BigSpace's buildings.
// Sprites are 64x64.
SDL_Texture* buildingSheet;

// Spritesheet containing the sprites for BigSpace's GroundUnits.
// Sprites are 16x16.
SDL_Texture* groundUnitSheet;

// Spritesheet containing the sprites for BigSpace's Ships.
// Sprites are 16x16
SDL_Texture* shipSheet;

// The font for all text.
TTF_Font* defaultFont;

// Initializes the window.
bool initWindow(int width, int height);

// Closes the current window.
void closeWindow();

// Initializes the font.
bool initFont();

// Initializes the galaxy tiles.
bool initTextures();

// Checks to see if the mouse is within bounds of a certain space.
bool withinBounds(int xPos, int yPos, SDL_Rect bounds);

/*
Initializes the window for BigSpace. Returns true if successful, otherwise false.
-Initializes SDL, returns false if failed.
-Initializes window, returns false if failed.
-Initializes a texture renderer for window, returns false if failed.
*/
bool initWindow(int width, int height) {

	// Initializes SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

	// Sets texture filtering to linear.
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		printf("Warning: Linear texture filtering not enabled!\n");

	}

	// Initializes window.
	window = SDL_CreateWindow("BigSpace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL) return false;

	// Creates the renderer for window. Allows hardware acceleration but not VSYNC.
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
	if (renderer == NULL) return false;

	//Initializes renderer color.
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// Initializes the font.
	if (!initFont()) return false;

	// Begins text input.
	SDL_StartTextInput();

	// If all goes well, returns true.
	return true;

}

/*
Initializes the window for BigSpace. Returns true if successful, otherwise false.
-Initializes SDL, returns false if failed.
-Initializes window, returns false if failed.
-Initializes a texture renderer for window, returns false if failed.
-Enables alpha blending, returns flase if failed.

This function differs from initWindow in that it makes the window resizeable.
*/
bool initGameWindow(int width, int height) {

	// Initializes SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

	// Sets texture filtering to linear.
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) printf("Linear texture filtering not enabled\n");

	// Initializes window.
	window = SDL_CreateWindow("BigSpace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) return false;

	// Creates the renderer for window. Allows hardware acceleration and VSYNC.
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) return false;

	//Initializes renderer color.
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// Enables alpha blending.
	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)) {
		printf("Failed to enable alpha blending.\n");
		return false;

	}

	// Initializes the font.
	if (!initFont()) return false;

	// Begins text input.
	SDL_StartTextInput();

	// If all goes well, returns true.
	return true;

}

/*
Closes the current window.
*/
void closeWindow() {

	// Destroys the window and renderer.
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	// Quits SDL systems.
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

}

/*
Initializes the font for BigSpace.
*/
bool initFont() {

	// Initializes TTF. Returns true if successful, otherwise false.
	if (TTF_Init() < 0) return false;

	// Initializes defaultFont. Returns true if successful, otherwise false.
	defaultFont = TTF_OpenFont("VCROSDMONO.ttf", 12);
	return !(defaultFont == NULL);

}

/*
Initializes the tilesheet.
*/
bool initTextures() {

	// Loads the Tile spritesheet.
	SDL_Surface* spriteSheetSurface = SDL_LoadBMP("BigSpaceTiles.bmp");
	if (spriteSheetSurface == NULL) return false;

	// Formats the surface such that white is colour keyed.
	SDL_SetColorKey(spriteSheetSurface, 1, SDL_MapRGB(spriteSheetSurface->format, 255, 255, 255));

	// Creates a texture spritesheet from the loaded surface.
	tileSheet = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);
	if (tileSheet == NULL) return false;

	// Loads the Building spritesheet.
	spriteSheetSurface = SDL_LoadBMP("BigSpaceBuildings.bmp");
	if (spriteSheetSurface == NULL) return false;

	// Formats the surface such that white is colour keyed.
	SDL_SetColorKey(spriteSheetSurface, 1, SDL_MapRGB(spriteSheetSurface->format, 255, 255, 255));

	// Creates a texture spritesheet from the loaded surface.
	buildingSheet = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);
	if (buildingSheet == NULL) return false;

	// Loads the GroundUnit spritesheet.
	spriteSheetSurface = SDL_LoadBMP("BigSpaceGroundUnits.bmp");
	if (spriteSheetSurface == NULL) return false;

	// Formats the surface such that white is colour keyed.
	SDL_SetColorKey(spriteSheetSurface, 1, SDL_MapRGB(spriteSheetSurface->format, 255, 255, 255));

	// Creates a texture spritesheet from the loaded surface.
	groundUnitSheet = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);
	if (groundUnitSheet == NULL) return false;

	// Loads the Ship spritesheet.
	spriteSheetSurface = SDL_LoadBMP("BigSpaceShips.bmp");
	if (spriteSheetSurface == NULL) return false;

	// Formats the surface such that white is colour keyed.
	SDL_SetColorKey(spriteSheetSurface, 1, SDL_MapRGB(spriteSheetSurface->format, 255, 255, 255));

	// Creates a texture spritesheet from the loaded surface.
	shipSheet = SDL_CreateTextureFromSurface(renderer, spriteSheetSurface);
	if (shipSheet == NULL) return false;

	// If all goes well, returns true.
	return true;

}

/*
Checks to see if a coordinate is within the bounds of the inputed SDL_Rect.
*/
bool withinBounds(int xPos, int yPos, SDL_Rect bounds) {

	// Returns true if the mouse is within the bounds of the SDL_Rect.
	if (xPos >= bounds.x && yPos >= bounds.y && xPos <= bounds.x + bounds.w && yPos <= bounds.y + bounds.h) {
		return true;

	}

	// Returns true if the mouse is not within the bounds of the SDL_Rect.
	return false;

}