#pragma once

// Initializes the ToolBar View.
void initToolBarView();

// Handles events within the ToolBar View.
void toolBarView(SDL_Event event);

// Resizes the ToolBar View to fit in a window of the inputed width and height.
void resizeToolBarView(int windowW, int windowH);

// Renders the ToolBar.
void renderToolBar();

/*
Initializes the ToolBar View.
*/
void initToolBarView() {

	// Initializes the toolBarViewport's size to the upper tenth of the window and renders it.
	resizeToolBarView(screenWidth, screenHeight);

}

/*
Handles events within the ToolBar View.
*/
void toolBarView(SDL_Event event) {

	// Manages clicking.
	if (event.type == SDL_MOUSEBUTTONDOWN) {

	}
	// Manages releasing a click.
	else if (event.type == SDL_MOUSEBUTTONUP) {

	}
}

/*
Resizes the ToolBar View to fit in a window of the inputed width and height.
Renders the ToolBar View after resizing.
*/
void resizeToolBarView(int windowW, int windowH) {

	// Resizes the ToolBar View to the upper tenth of the screen.
	toolBarViewport = {0, 0, screenWidth, (int)(0.1 * screenHeight)};

	// Renders the resized universe view.
	renderToolBar();

}

/*
Renders the ToolBar.
*/
void renderToolBar() {

	// Sets the colour to light grey.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

	// TEMP: Renders the toolBarViewport's background.
	SDL_RenderFillRect(renderer, &toolBarViewport);

	// Sets the viewport to the toolBarViewport.
	SDL_RenderSetViewport(renderer, &toolBarViewport);

	// TODO add items.

	// Sets the viewport to window.
	SDL_RenderSetViewport(renderer, NULL);

}