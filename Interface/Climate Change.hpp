#pragma once

/*
Manages atmosphere change for a page of planets.
*/
void atmosphereChange() {
	int page = requestHabitablePage();

	// Returns if there is no available page.
	if (page < 0) return;

	// Updates the gases for each planet.
	for (int i = 0; i < habitablePages[page]->arrCurrPlanet; ++i) {
		for (int j = 0; j < NUM_GASES; ++j) {
			habitablePages[page]->planets[i].gases[j] += habitablePages[page]->planets[i].netGases[j] / 256;

		}
	}

	// Continues until there are no remaining HabitablePages.
	atmosphereChange();

}

/*
Manages climate change for each planet.
*/
void climateChange() {
	atmosphereChange();

}