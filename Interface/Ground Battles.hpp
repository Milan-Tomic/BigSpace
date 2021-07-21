#pragma once

/*
Manages battles for a page of planets.
*/
void planetBattle() {
	int page;

	// Performs planetary battles for each HabitablePage.
	while ((page = requestHabitablePage()) >= 0) {

		// Performs planetary battles for all HabitablePlanets on a page.
		for (int i = 0; i < habitablePages[page]->arrCurrPlanet; ++i)
			if (habitablePages[page]->planets[i].battle) habitablePages[page]->planets[i].battle->battle();

	}
}

/*
Performs Battles on every planet containing a Battle.
*/
void groundBattles() {
	planetBattle();

}