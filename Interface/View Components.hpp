#pragma once

// Used to store viewports, as the viewports within the Views are codependent.

// Viewport containing the ToolBar View.
SDL_Rect toolBarViewport;

// Viewport containing the System View and the system.
SDL_Rect systemViewViewport; // Stores whole view.
SDL_Rect systemViewport; // System or planet.
SDL_Rect systemPanel; // Contains a panel for the system view.
SDL_Rect systemSelectionPanel; // Contains selection items for the system view.

// Viewport containing the Universe View.
SDL_Rect universeViewport;

// Viewport containing the Report View.
SDL_Rect reportViewport;

#define ACTIVITY_BAR_WIDTH 8

/*
Resizes the Celestial View (used by System View and Planet View).
*/
void resizeCelestialView(int windowW, int windowH) {

	// Resizes the System View to the area ACTIVITY_BAR_WIDTH pixels below the universeViewport.
	systemViewViewport = { 0, universeViewport.h + toolBarViewport.h + ACTIVITY_BAR_WIDTH, universeViewport.w,
		screenHeight - universeViewport.h - toolBarViewport.h - ACTIVITY_BAR_WIDTH };

	// Resizes the system view to a box on the left half of the System View.
	int viewportWidth = systemViewViewport.w / 2 - systemViewViewport.w / 64;
	viewportWidth -= viewportWidth % 64;
	int viewportHeight = systemViewViewport.h - systemViewViewport.h / 32;
	viewportHeight -= viewportHeight % 64;
	int xBuffer = systemViewViewport.w / 4 - viewportWidth / 2;
	int yBuffer = systemViewViewport.h / 2 - viewportHeight / 2;
	systemViewport = {systemViewViewport.x + xBuffer, systemViewViewport.y + yBuffer,
		viewportWidth, viewportHeight};
	systemSelectionPanel = { systemViewport.x + systemViewport.w + xBuffer * 2, systemViewport.y + yBuffer,
		viewportWidth, viewportHeight / 10 > 20 ?  20 : viewportHeight / 8};
	systemPanel = {systemViewport.x + systemViewport.w + xBuffer * 2, systemViewport.y + systemSelectionPanel.h + yBuffer,
		viewportWidth, viewportHeight - systemSelectionPanel.h - 2 * yBuffer };

	// Renders the systemViewViewport's background.
	SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
	SDL_RenderFillRect(renderer, &systemViewViewport);

}