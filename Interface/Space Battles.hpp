#pragma once

// Resources for spaceBattle multithreading.
std::shared_mutex spaceBattleMutex;
Semaphore spaceBattleSemaphore(0, 0);

/*
Performs space Battles and executes Squadron orders.

Does so by requesting an integer from a mutexed function, then acquiring a strip
of a Galaxy based on the returned integer.
*/
void spaceBattle() {
	Galaxy* gal;
	int strip;
	int xBeg, xEnd;
	int yBeg, yEnd;

	// Requests a strip of a Galaxy to perform orders in.
	for (int t = requestInt(); t < numGals * numThreads; t = requestInt()) {

		// Derives the appropriate Galaxy and strip to use from t.
		gal = &galaxies[t / numThreads];
		strip = t % numThreads;

		// Starts within the t / numThreads segment of the current Galaxy.
		xBeg = gal->area.x + strip * (gal->area.w / numThreads);

		// Ends at the t + 1 / numThreads segment of the current Galaxy.
		xEnd = gal->area.x + (strip + 1) * (gal->area.w / numThreads);

		// Makes sure that the final strip reaches the end.
		if (strip == numThreads - 1) xEnd = gal->area.x + gal->area.w;

		// Strips are vertical to maximize spatial locality (see index function).
		yBeg = gal->area.y;
		yEnd = gal->area.y + gal->area.h;

		// Parses through each assigned tile to find Squadrons.
		for (int x = xBeg; x < xEnd; ++x) {
			for (int y = yBeg; y < yEnd; ++y) {

				// Parses through each Squadron to perform actions.
				for (int s = 0; s < uIndex(x, y).numSquadrons; ++s) {
					uSquadron(x, y, s)->action();

				}
			}
		}
	}
}

/*
Manages Threat for all Squadrons and Colonies.

For Squadrons, manages threats by requesting an integer from a mutexed function,
then managing Threats in a strip of a Galaxy based on the returned integer. This
is the same as spaceBattle.

For Colonies, manages threats by requesting a ColonyPage and managing threats
within said page.

TODO change threat range
TODO make some means of determining which Squadrons are enemies.
*/
void spaceThreats() {
	Galaxy* gal;
	Colony* colony;
	CoordU loc;
	int t;
	int strip;
	int page;
	int xBeg, xEnd;
	int yBeg, yEnd;
	int iBeg, iEnd;
	int jBeg, jEnd;

	// TODO temp
	// This is the maximum range to draw a threat from.
	const int range = 8;

	// Requests a strip of a Galaxy to find threat in.
	while ((t = requestInt()) < numGals * numThreads) {

		// Derives the appropriate Galaxy and strip to use from t.
		gal = &galaxies[t / numThreads];
		strip = t % numThreads;

		// Starts within the t / numThreads segment of the current Galaxy.
		xBeg = gal->area.x + strip * (gal->area.w / numThreads);

		// Ends at the t + 1 / numThreads segment of the current Galaxy.
		xEnd = gal->area.x + (strip + 1) * (gal->area.w / numThreads);

		// Makes sure that the final strip reaches the end.
		if (strip == numThreads - 1) xEnd = gal->area.x + gal->area.w;

		// Strips are vertical to maximize spatial locality (see index function).
		yBeg = gal->area.y;
		yEnd = gal->area.y + gal->area.h;

		// Parses through each assigned tile to find Squadrons.
		for (int x = xBeg; x < xEnd; ++x) {
			for (int y = yBeg; y < yEnd; ++y) {

				// Begins parsing if and only if there are Squadrons in the tile.
				if (uIndex(x, y).numSquadrons) {

					// Will not go over the bounds of the Galaxy.
					iBeg = x - range / 2 < gal->area.x ? gal->area.x : x - range / 2;
					iEnd = x + range / 2 > gal->area.x + gal->area.w ? gal->area.x + gal->area.w : x + range / 2;
					jBeg = y - range / 2 < gal->area.y ? gal->area.y : y - range / 2;
					jEnd = y + range / 2 > gal->area.y + gal->area.h ? gal->area.y + gal->area.h : y + range / 2;

					// Manages all Squadrons in the tile.
					for (int s = 0; s < uIndex(x, y).numSquadrons; ++s) {
						Squadron* squadron = uSquadron(x, y, s);

						// Removes deleted Threats from the Squadron.
						removeDeletedSquadrons(uSquadron(x, y, s));

						// Parses through all nearby tiles to find threats to the current Squadron.
						for (int i = iBeg; i < iEnd; ++i) {
							for (int j = jBeg; j < jEnd; ++j) {

								// Adds all nearby enemy Squadrons as Threats.
								// TODO create some means of determining who is an enemy.
								for (int k = 0; k < uIndex(i, j).numSquadrons; ++k) {

									// Adds threats to the Squadron.
									if (uSquadron(i, j, k)->owner != squadron->owner)
										squadron->addThreat(uSquadron(i, j, k));

								}
							}
						}

						// Handles threats for the current Squadron.
						squadron->handleThreats();

					}
				}
			}
		}
	}

	// Parses through all Colonies and manages threat for them.
	while ((page = requestColonyPage()) != -1) {
		for (int col = 0; col < colonyPages[page]->arrCurrColony; ++col) {
			colony = &colonyPages[page]->colonies[col];

			// Will not add threats if the Colony is unowned.
			if (!colony->government) continue;

			// Removes deleted Threats from the Colony.
			removeDeletedSquadrons(colony);

			// Stores the location and Galaxy.
			loc = colony->planet->loc;
			gal = findGalaxy(loc.x, loc.y);

			// Will not go over the bounds of the Galaxy.
			iBeg = loc.x - range / 2 < gal->area.x ? gal->area.x : loc.x - range / 2;
			iEnd = loc.x + range / 2 > gal->area.x + gal->area.w ? gal->area.x + gal->area.w : loc.x - range / 2;
			jBeg = loc.y - range / 2 < gal->area.y ? gal->area.y : loc.y - range / 2;
			jEnd = loc.y + range / 2 > gal->area.y + gal->area.h ? gal->area.y + gal->area.h : loc.y - range / 2;

			// Parses through all nearby tiles to find threats to the current Colony.
			for (int i = iBeg; i < iEnd; ++i) {
				for (int j = jBeg; j < jEnd; ++j) {

					// Adds all nearby enemy Squadrons as Threats.
					// TODO create some means of determining who is an enemy
					for (int k = 0; k < uIndex(i, j).numSquadrons; ++k) {
						if (uSquadron(i, j, k)->owner != colony->government) {
							colony->addThreat(uSquadron(i, j, k));

						}
					}
				}
			}

			// Handles threatsa for the current Colony.
			colony->handleThreats();

		}
	}

}

/*
Multithreaded closure management function.
*/
void manageClosures() {
	int page;

	// Performs closure management for each dirty ClosurePage.
	while ((page = requestDirtyClosurePage()) >= 0) {

		// Performs closure management for all closures on a dirty page.
		for (int i = 0; i < habitablePages[page]->arrCurrPlanet; ++i) manageClosures(page);

	}
}

/*
Manages spaceBattles. Ensures that spaceBattle (executing orders and battles)
is finished before threat handling and closure management.
*/
void spaceBattles() {

	// Manages space battles in a threaded fashion.
	spaceBattle();

	// Block that forces all threads wait until spaceBattle is over before proceeding.
	spaceBattleSemaphore.post();
	spaceBattleMutex.lock_shared();
	spaceBattleMutex.unlock_shared();

	// Manages space threats in a threaded fashion.
	spaceThreats();

	// Reconciles closures after battles are finished.
	manageClosures();

}