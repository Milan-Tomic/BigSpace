#pragma once


// Struct describing an instruction for generating a galaxy.
// TODO when barrens are added, allow dummySize (search initThreadDummies) to account for barrens
struct GalaxyInstruction {

	// Galaxy Type. SPIRAL and RING are used for generation,
	// RANDOM will be replaced with either SPIRAL or RING.
	int type;

	// Width of the Galaxy.
	int width;

	// Height of the Galaxy. Not valid for SPIRAL.
	int height;

	// Number of arms for the galaxy. Valid only for SPIRAL.
	int numArms;

	// Fraction determining arm size of the galaxy. Valid only for SPIRAL.
	int armSizeFraction;

	// Fraction determining rotation of arms in this galaxy. Valid only for SPIRAL.
	int radFraction;

	// Fraction determining the core size of the galaxy.
	int coreSizeFraction;

	// Chance of generating a system from a ThickSpaceTile.
	int starChance;

	// Maximum number of HabitablePlanets in a system.
	int numHabitable;

	// Maximum number of BarrenPlanets in a system.
	int numBarren;

	// Maximum size of a HabitablePlanet.
	int planetSize;

	// Chance of spawning an empire (in thousandths) in any given system.
	int empireChance;

};

/*
Struct containing certain values pertinent to a Galaxy.
*/
struct Galaxy {

	// Area of the Galaxy measured in Tiles.
	SDL_Rect area;

	// Number of unowned tiles in this Galaxy.
	int unowned;

	// Tracks whether a thread is currently using this galaxy.
	bool active;

};

// List of galaxies in the universe.
Galaxy* galaxies;
int numGals;

/*
Finds the Galaxy that contains the inputed coordinate.
Returns nullptr if there is no such Galaxy.
*/
extern bool withinBounds(int, int, SDL_Rect);
inline Galaxy* findGalaxy(int x, int y) {

	// Finds and returns the Galaxy associated with the inputed coordinate.
	for (int i = 0; i < numGals; ++i) {
		if (withinBounds(x, y, galaxies[i].area)) {
			return &galaxies[i];

		}
	}

	// Returns nullptr if the Galaxy is not found.
	return nullptr;

}