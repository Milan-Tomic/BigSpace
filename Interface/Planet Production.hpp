#pragma once

/*
Manages production for a page of planets.
*/
void buildingProduction() {
	int page = requestHabitablePage();

	// Returns if there is no available page.
	if (page < 0) return;

	// Performs planetary production for all HabitablePlanets on a page.
	for (int i = 0; i < habitablePages[page]->arrCurrPlanet; ++i) {
		habitablePages[page]->planets[i].ownersProduction();

	}

	// Continues until there are no remaining HabitablePages.
	buildingProduction();

}

/*
Manages production of resources for each planet. This is mostly copied from Climate Change.
*/
void planetProduction() {
	buildingProduction();

}