#pragma once

// Initializes the Report View.
void initReportView();

// Handles events within the Report View.
void reportView(SDL_Event event);

// Resizes the Report View to fit in a window of the inputed width and height.
void resizeReportView(int windowW, int windowH);

// Renders the Report View.
void renderReportView();

/*
Initializes the Report View.
*/
void initReportView() {

	// Initializes the reportViewport's size to the right 384 pixels of the window and renders it.
	resizeReportView(screenWidth, screenHeight);

}

/*
Handles events within the Report View.
*/
void reportView(SDL_Event event) {
	
	// Manages clicking.
	if (event.type == SDL_MOUSEBUTTONDOWN) {

	}
	// Manages releasing a click.
	else if (event.type == SDL_MOUSEBUTTONUP) {

	}
}

/*
Resizes the Report View to fit in a window of the inputed width and height.
Renders the Report View after resizing.
*/
void resizeReportView(int windowW, int windowH) {

	// Resizes the Report View to the right 384 pixels of the window.
	reportViewport = {universeViewport.w, toolBarViewport.h, 384, screenHeight};

	// Renders the resized universe view.
	renderReportView();

}

/*
Renders the Report View.
*/
void renderReportView() {

	// TEMP : Sets the colour to white.
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	// TEMP: Renders the reportViewport's background.
	SDL_RenderFillRect(renderer, &reportViewport);

	// TODO add items.

}
