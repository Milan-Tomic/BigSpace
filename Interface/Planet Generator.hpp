#pragma once

// Number of elements in a habitable page. Should be around 1MB.
#define HABITABLE_PAGE_SIZE ((1048576 - 8) / sizeof(HabitablePlanet))

// Number of elements in a barren page. Should be around 1MB.
#define BARREN_PAGE_SIZE ((1048576 - 8) / sizeof(BarrenPlanet))

// Block size used when reallocating HabitablePage page table.
#define NUM_HABITABLE_PAGES 256

// Block size used when reallocating BarrenPage page table.
#define NUM_BARREN_PAGES 256

// Page of Habitable Planets.
struct HabitablePage {
	HabitablePlanet planets[HABITABLE_PAGE_SIZE];
	uint16_t arrCurrPlanet;
	bool active;

	// 5 padding

};

// Page of Barren Planets.
struct BarrenPage {
	BarrenPlanet planets[BARREN_PAGE_SIZE];
	uint16_t arrCurrBarren;
	bool active;

	// 5 padding

};

// Contains all HabitablePlanets. Minimizes memory use from allocations.
int numHabitablePages;
HabitablePage** habitablePages;

// Contains all BarrenPlanets. Minimizes memory use from allocations.
int numBarrenPages;
BarrenPage** barrenPages;

// Stores all planet tiles.
std::vector<PlanetTile*> allPlanetTiles;

// Initializes the habitablePages array.
void initHabitablePlanets(int numHabitable);

// Places a HabitablePlanet into habitablePages.
HabitablePlanet* placeHabitable(int size);

// Finds the location of a given HabitablePlanet within the paging table.
inline void findHabitableIndex(int& page, int& index, HabitablePlanet* planet);

// Initializes the barrenPlanets array.
void initBarrenPlanets(int numBarren);

// Places a barren planet into barrenPlanets.
BarrenPlanet* placeBarren();

// Generates a random planet from scratch.
HabitablePlanet* generateNewPlanet(GalaxyInstruction* galInstr, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas, float heatMultiple);

// Generates a planet of the inputed size on the inputed PlanetTile array.
void planetGenerator(int size, HabitablePlanet* planet, int** dummyPlanet, int** moistureDummy,
	int** heatDummmy, int** riverDummy, int** buildingDummy, int** canvas);

// Generates splotches of a given PlanetTile type on a planet.
void generateSplotch(int size, int** dummyPlanet, int xBeg, int yBeg, int splotchSize, int tileID);

// Generates continents made out of splotches of Desert PlanetTiles.
void generateContinent(int size, int** dummyPlanet, int xBeg, int yBeg, int splotchSize);

// Fills gaps in biomes on a planet.
void fillGaps(int size, int** dummyPlanet, int** canvas, int threshold, int tileID);

// Randomizes a planet's atmoshpere.
void randomizeAtmosphere(HabitablePlanet* planet);

// Assigns moisture level 5 to ocean tiles.
void wetOceans(int size, int** dummyPlanet, int** moistureDummy);

// Fills gaps in moisture on a planet.
void fillMoisture(int size, int** dummyPlanet, int** moistureDummy, int** canvas, int moisture, int adjMoisture);

// Creates winds that carry moisture from the ocean.
void generateMoisture(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** canvas);

// Generates coastal tiles adjacent to land.
void generateCoastline(int size, int** dummyPlanet);

// Creates rivers on the planet.
void generateRivers(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** riverDummy, HabitablePlanet* planet);

// Makes the tiles surrounding river tiles more moist.
void moistenRivers(int size, HabitablePlanet* planet, int** moistureDummy);

// Finds the t4 of a planet with the inputed heatMultiple and albedo.
double findT4(double heatMultiple, double albedo);

// Finds the heat of a planet with the inputed t4 and tau.
double findHeat(double t4, double tau);

// Finds the heat of a planet with the inputed heatMultiple, albedo, and tau.
double findHeat(double heatMultiple, double albedo, double tau);

// Generates the planet's heat gradient.
void generateHeat(int size, int** heatDummy, int planetTemperature);

// Generates mountains on the inputed planet.
void generateMountains(int size, int** dummyPlanet, int rangeSize);

// Generates trees on the planet.
void generateTrees(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** buildingDummy, int**canvas, int chance);

// Generates deposits on the planet.
void generateDeposits(HabitablePlanet* planet);

// Wraps mountains over the planet's ice caps and horizontal edges.
void wrapAroundMountains(int size, int* xPos, int* yPos, int* slope);

// Gets the next planet coordinate in the current direction.
void getPlanetCoordAtDir(int size, int* xPos, int* yPos, Direction dir);

// Reassigns tileIDs based on moisture and heat.
void reassignID(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy);

// Converts a dummyPlanet of tileID's to a planet's tileID's.
void dummyToTileID(int size, HabitablePlanet* planet, int** dummyPlanet, int** buildingDummy);

/*
Places an array of planetTiles.
Should be called with init only once.
*/
PlanetTile* placePlanetTiles(int size, bool init) {
	static int currTile = 0;
	static int currTiles = 0;
	static int remainingTiles = 0;
	static PlanetTile* tiles = nullptr;
	static std::shared_mutex placePlanetTilesMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(placePlanetTilesMutex);

	// Initializes the PlanetTiles.
	if (init) {

		// Deletes existent tiles.
		for (std::vector<PlanetTile*>::iterator i = allPlanetTiles.begin(); i != allPlanetTiles.end(); ++i) delete* i;
		allPlanetTiles.clear();

		// Places a new array of tiles into allPlanetTiles.
		currTile = 0;
		currTiles = 1;
		remainingTiles = size;
		allPlanetTiles.push_back((PlanetTile*)calloc(size, sizeof(PlanetTile)));
		return tiles = allPlanetTiles.back();

	}

	// Allocates the tiles. Will allocate more tiles if necessarry.
	if ((remainingTiles -= size) <= 0) {
		remainingTiles = 250000;
		tiles = (PlanetTile*)calloc(remainingTiles, sizeof(PlanetTile));
		allPlanetTiles.push_back(tiles);
		currTile = 0;
		++currTiles;

	}

	// Places the planet within the tiles and returns.
	currTile += size;
	return &tiles[currTile - size];

}

/*
Initializes the habitablePages array.
*/
void initHabitablePlanets(int numHabitable) {

	// Frees habitablePages for reuse.
	if (numHabitablePages) {
		for (int i = 0; i < numHabitablePages; ++i) {
			if (habitablePages[i]) delete habitablePages[i];
			habitablePages[i] = nullptr;

		}
		delete[] habitablePages;

	}

	// Initializes habitablePages.
	numHabitablePages = numHabitable / HABITABLE_PAGE_SIZE + 1;
	habitablePages = (HabitablePage**)calloc(numHabitablePages + NUM_HABITABLE_PAGES - (numHabitablePages % NUM_HABITABLE_PAGES), sizeof(HabitablePage*));
	for (int i = 0; i < numHabitablePages; ++i)
		habitablePages[i] = (HabitablePage*)calloc(1, sizeof(HabitablePage));

	// Sets the number of HabitablePlanets to 0.
	numHabitablePlanets = 0;

}

/*
Places a habitable planet into habitablePages.

Returns the int depositIndex of the planet in habitablePages cast to a HabitablePlanet.
These ints will later be recast to SystemPlanetTiles in beginGenerateSystems.
*/
HabitablePlanet* placeHabitable(int size) {
	static std::shared_mutex placeHabitableMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(placeHabitableMutex);

	// Attempts to place the planet within one of the current pages.
	HabitablePlanet* planet = nullptr;
	for (int i = 0; i < numHabitablePages; ++i) {
		if (habitablePages[i]->arrCurrPlanet < HABITABLE_PAGE_SIZE) {
			planet = &(habitablePages[i]->planets[habitablePages[i]->arrCurrPlanet]);
			++habitablePages[i]->arrCurrPlanet;
			break;

		}
	}

	// If placement failed, creates a new page and places the planet there.
	// Will only place when all threads are at this function or returned.
	if (planet == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numHabitablePages + 1 % NUM_HABITABLE_PAGES))
			habitablePages = (HabitablePage**)realloc(habitablePages, (numHabitablePages + NUM_HABITABLE_PAGES) * sizeof(HabitablePage*));

		// Creates a new HabitablePage.
		habitablePages[numHabitablePages] = (HabitablePage*)calloc(1, sizeof(HabitablePage));
			
		// Places the planet in the first index of the new page.
		planet = &habitablePages[numHabitablePages]->planets[0];
		++habitablePages[numHabitablePages]->arrCurrPlanet;

		// Moves on to the next page.
		++numHabitablePages;
		
	}

	// Increments numHabitablePlanets.
	++numHabitablePlanets;

	// Places the planet's size.
	planet->size = size;

	// Returns a pointer to the planet.
	return planet;

}

/*
Requests a HabitablePage. Returns the index of the next available page in habitablePages, otherwise -1.
TODO if infrastructure permits it, use a static global to increment through habitable pages.
*/
int requestHabitablePage() {
	static std::shared_mutex requestHabitablePageMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(requestHabitablePageMutex);

	// Parses through each galaxy and returns the next available one.
	for (int i = 0; i < numHabitablePages; ++i) {
		if (!habitablePages[i]->active) {
			habitablePages[i]->active = true;
			return i;

		}
	}

	// Returns -1 if no HabitablePage was found.
	return -1;

}

/*
Returns a HabitablePage.
*/
void releaseHabitablePage(int index) {
	habitablePages[index]->active = false;

}

// Returns all HabitablePage.
void releaseAllHabitablePages() {
	for (int i = 0; i < numHabitablePages; ++i) habitablePages[i]->active = false;

}

/*
Finds the location of a given HabitablePlanet within the paging table.
*/
inline void findHabitableIndex(int& page, int& index, HabitablePlanet* planet) {

	// Finds the page.
	for (page = 0; page < numHabitablePages; ++page) {

		// Returns the value if the HabitablePlanet is within bounds.
		if ((char*)planet >= (char*)habitablePages[page] &&
			(char*)planet < (char*)habitablePages[page] + sizeof(HabitablePage)) {
			index = ((char*)planet - (char*)habitablePages[page]) / sizeof(HabitablePlanet);
			return;

		}
	}
}

/*
Initializes the barrenPlanets array.
*/
void initBarrenPlanets(int numBarren) {

	// Frees barrenPages for reuse.
	if (numBarrenPages) {
		for (int i = 0; i < numBarrenPages; ++i) {
			if (barrenPages[i]) delete barrenPages[i];
			barrenPages[i] = nullptr;

		}
		delete[] barrenPages;

	}

	// Initializes barrenPages.
	numBarrenPages = numBarren / BARREN_PAGE_SIZE + 1;
	barrenPages = (BarrenPage**)calloc(numBarrenPages + NUM_BARREN_PAGES - (numBarrenPages % NUM_BARREN_PAGES), sizeof(BarrenPage*));
	for (int i = 0; i < numBarrenPages; ++i)
		barrenPages[i] = (BarrenPage*)calloc(1, sizeof(BarrenPage));

	// Sets the number of BarrenPlanets to 0.
	numBarrenPlanets = 0;

}

/*
Places a barren planet into barrenPlanets.
*/
BarrenPlanet* placeBarren() {
	static std::shared_mutex placeBarrenMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(placeBarrenMutex);

	// Attempts to place the planet within one of the current pages.
	BarrenPlanet* planet = nullptr;
	for (int i = 0; i < numBarrenPages; ++i) {
		if (barrenPages[i]->arrCurrBarren < BARREN_PAGE_SIZE) {
			planet = &(barrenPages[i]->planets[barrenPages[i]->arrCurrBarren]);
			++(barrenPages[i]->arrCurrBarren);
			break;

		}
	}

	// If placement failed, creates a new page and places the planet there.
	// Will only place when all threads are at this function or returned.
	if (planet == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numBarrenPages + 1 % NUM_BARREN_PAGES))
			barrenPages = (BarrenPage**)realloc(barrenPages, (numBarrenPages + NUM_BARREN_PAGES) * sizeof(BarrenPage*));

		// Creates a new BarrenPage.
		barrenPages[numBarrenPages] = (BarrenPage*)calloc(1, sizeof(BarrenPage));

		// Places the planet in the first index of the new page.
		planet = &(barrenPages[numBarrenPages]->planets[0]);
		++barrenPages[numBarrenPages]->arrCurrBarren;

		// Moves on to the next page.
		++numBarrenPages;

	}

	// Increments numBarrenPlanets.
	++numBarrenPlanets;

	// Returns a pointer to the planet.
	return planet;

}

// Undefines constants which are used only for initializing planets.
#undef NUM_HABITABLE_PAGES
#undef NUM_BARREN_PAGES

/*
Generates a habitable from scratch, with no size parameter and no inputed array of PlanetTiles.

Used only for debugging.
*/
HabitablePlanet* generateNewPlanet(GalaxyInstruction* galInstr, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas, float heatMultiple) {

	// Initializes the size of the planet to be between 20 and planetSize.
	int size = randB(8) % (galInstr->planetSize - MIN_PLANET_WIDTH + 1) + MIN_PLANET_WIDTH;
	HabitablePlanet* planet = placeHabitable(size);
	planet->size = size;
	planet->heatMultiple = heatMultiple;

	// Generates the planet.
	planetGenerator(size, planet, dummyPlanet, moistureDummy, heatDummy, riverDummy, buildingDummy, canvas);

	// Creates and returns a HabitablePlanet containing the above planet.
	return planet;

}

// Chance of a forest spawning on any given tile.
// TODO adjust this dynamically.
#define TREE_CHANCE 20

/*
Generates a planet. Does so by modifying an inputed HabitablePlanet.
*/
void planetGenerator(int size, HabitablePlanet* planet, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas) {

	// Initializes dummyPlanet, moistureDummy, riverDummy, and buildingDummy.
	zeroCanvas(size, dummyPlanet, Ocean);
	zeroCanvas(size, moistureDummy, 0);
	zeroCanvas(size, riverDummy, 0);
	zeroCanvas(size, buildingDummy, 0);

	// Randomizes the atmosphere.
	randomizeAtmosphere(planet);

	// Initializes heatDummy.
	generateHeat(size, heatDummy, planet->temperature);

	// Generates splotches of desert to mime continents.
	int quarterSize = size / 4;
	for (int i = 0; i < randB(4) % 10 + 6; ++i)
		generateContinent(size, dummyPlanet, randB(8) % size, randB(10) % (size + quarterSize) - quarterSize, randB(6) % quarterSize + size / 8);

	// Smooths edges of continents.
	fillGaps(size, dummyPlanet, canvas, 5, Desert);
	fillGaps(size, dummyPlanet, canvas, 4, Ocean);
	fillGaps(size, dummyPlanet, canvas, 4, Ocean);

	// Places Mountain ranges.
	for (int i = 0; i < size / 8; ++i) generateMountains(size, dummyPlanet, randB(6) % (size / 4) + 5);

	// Fills the Mountain ranges. Will fill less aggressively for smaller planets.
	if (size < 40) fillGaps(size, dummyPlanet, canvas, 3, Mountain);
	else fillGaps(size, dummyPlanet, canvas, 2, Mountain);

	// Generates the moisture gradient.
	wetOceans(size, dummyPlanet, moistureDummy);
	generateMoisture(size, dummyPlanet, moistureDummy, heatDummy, canvas);
	generateCoastline(size, dummyPlanet);
	generateRivers(size, dummyPlanet, moistureDummy, heatDummy, riverDummy, planet);
	moistenRivers(size, planet, moistureDummy);

	// Generates trees.
	generateTrees(size, dummyPlanet, moistureDummy, heatDummy, buildingDummy, canvas, TREE_CHANCE);

	// Reassigns the tileIDs of the planet based on heat and moisture.
	reassignID(size, dummyPlanet, moistureDummy, heatDummy);

	// Assigns dummyPlanet's tileIDs and buildingDummy's buildings to planet.
	dummyToTileID(size, planet, dummyPlanet, buildingDummy);

	// Generates deposits.
	generateDeposits(planet);

}

// Tree chance is not needed outside of this header.
#undef TREE_CHANCE

/*
Generates a randomized splotch of the inputed splotchSize and tileID on the inputed planet.
Does so by changing the tileID of a given depositIndex within the inputed area to the inputed tileID.

This function allows for wrapping over poles and across east/west map borders.
*/
void generateSplotch(int size, int** dummyPlanet, int xBeg, int yBeg, int splotchSize, int tileID) {

	// Generates the part of the splotch that is in the middle of the area.
	int iPos = 0;
	int jPos = 0;
	for (int i = xBeg; i < xBeg + splotchSize; ++i) {
		for (int j = yBeg; j < yBeg + splotchSize; ++j) {

			// Sets iPos and jPos when above the northern border.
			if (j < 0) {
				iPos = i + size / 2;
				jPos = j - yBeg;

			}
			// Sets iPos and jPos when below the southern border.
			else if (j >= size) {
				iPos = i + size / 2;
				jPos = 2 * size - j - 1;

			}
			// Sets iPos and jPos when not handling poles.
			else {
				iPos = i;
				jPos = j;

			}

			// Wraps around when the right edge is reached.
			while (iPos >= size) iPos -= size;
			dummyPlanet[iPos][jPos] = tileID;

		}
	}
}

/*
Generates a continent composed of desert.
Does so by recursively generating splotches of descending size centered around the inputed
xBeg and yBeg.
*/
void generateContinent(int size, int** dummyPlanet, int xBeg, int yBeg, int splotchSize) {
	int halfSize = size / 2;
	int halfSplotch = splotchSize / 2;

	// Base case. Quits when size 0 is reached.
	if (splotchSize <= 1) return;

	// Moves xBeg and yBeg by a fraction of this splotch's size.
	if (randB(1)) xBeg -= randB(8) % halfSplotch;
	else xBeg += splotchSize + randB(8) % halfSplotch;
	if (randB(1)) yBeg -= randB(8) % halfSplotch;
	else yBeg += splotchSize + randB(8) % halfSplotch;
	while (yBeg >= size) {
		yBeg = 2 * size - yBeg - 1;
		xBeg += halfSize;

	}
	while (yBeg < 0 - size - splotchSize) {
		yBeg += halfSize;
		xBeg += halfSize;

	}
	if (xBeg >= size) xBeg -= size;
	else if (xBeg < 0) xBeg += size;

	// Generates the currrent splotch.
	generateSplotch(size, dummyPlanet, xBeg, yBeg, splotchSize, Desert);

	// Generates four splotches which are 1/4 the size of the current splotch.
	for (int i = 0; i < 4; ++i) generateContinent(size, dummyPlanet, xBeg, yBeg, halfSplotch);

}

/*
Fills gaps in biomes on a planet. Does so by making each PlanetTile with threshold adjacent PlanetTiles
that have tileID are assigned to also have tileID.
*/
void fillGaps(int size, int** dummyPlanet, int** canvas, int threshold, int tileID) {
	int poleDistance = size / 2;

	// Copies planet's tileIDs to canvas.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			canvas[i + 1][j + 1] = dummyPlanet[i][j];

		}
	}

	// Copies planet's horizontal wraparound tileIDs.
	for (int j = 0; j < size; ++j) {
		canvas[size + 1][j + 1] = dummyPlanet[0][j]; // Copies left to right.
		canvas[0][j + 1] = dummyPlanet[size - 1][j]; // Copies right to left.

	}

	// Copies planet's polar wraparound tileIDs.
	int polarDistance;
	for (int i = 0; i < size; ++i) {
		if (i < size / 2) polarDistance = size / 2;
		else polarDistance = -size / 2;

		canvas[i][0] = dummyPlanet[i + polarDistance][0]; // Copies across north pole.
		canvas[i][size + 1] = dummyPlanet[i + polarDistance][size - 1]; // Copies across south pole.

	}

	// Checks for gaps in the inputed planet's main body and fills them.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// If the current depositIndex has an instance of the desired PlanetTile, keeps the existing copy.
			if (canvas[i + 1][j + 1] == tileID);
			// If the current depositIndex does not have an instance of the desired PlanetTile, counts the
			// number of adjacent tileID PlanetTiles.
			else {
				int counter = 0;

				// Checks all tiles within a 3x3 area centered around the current tile. Increments counter
				// for each tile that matches the inputed tileID.
				for (int x = i; x < i + 3; ++x) {
					for (int y = j; y < j + 3; ++y) {
						if (canvas[x][y] == tileID) ++counter;

					}
				}

				// If the counter is at or above the threshold, assigns the chosen tile the inputed tileID.
				if (counter >= threshold) dummyPlanet[i][j]  = tileID;

			}
		}
	}
}

/*
Randomizes a planet's atmoshpere.

Earth's atmosphere is 5000000000000000 tons.
Earth's atmospheric composition http://www.ux1.eiu.edu/~cfjps/1400/atmos_origin.html
*/
void randomizeAtmosphere(HabitablePlanet* planet) {
	int totWeight = 0;

	// Generates the gases.
	planet->gases[Oxygen] = (rand() % 2000000000000000) * ((double)planet->size / 70); // Max 200% EarthO2.
	planet->gases[Methane] = planet->gases[Oxygen] < 1000000000 ? (rand() % 2000000000000000) * ((double)planet->size / 70) :
		rand() % (rand() % 2000000000000000) * ((double)planet->size / 70); // Max 40% || small% totEarth.
	planet->gases[Null_Gases] = (rand() % 6000000000000000 + 4000000000000000) * ((double)planet->size / 70); // max 200% totEarth.
	planet->gases[Pollutants] = (rand() % 100000000000000)* ((double)planet->size / 70); // max 2% totEarth.
	planet->gases[Poisons] = (rand() % 1000000) * ((double)planet->size / 70); // max trivial.

	// Sums the weights of atmospheric gases and calculates the planet temperature.
	// Pollutants have 5x the weight.
	// Also initializes netGases to 0.
	for (int i = 0; i < 5; ++i) {
		totWeight += planet->gases[i];
		totWeight += planet->gases[Pollutants];
		planet->netGases[i] = 0;

	}

	// Assigns the planet's temperature assuming an albedo of 0.3.
	planet->temperature = (int)findHeat(planet->heatMultiple, 0.3, (totWeight / 5000000000000000.0));

}

/*
Assigns moisture level 5 to ocean tiles and 0 for all others.
*/
void wetOceans(int size, int** dummyPlanet, int** moistureDummy) {
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			if (dummyPlanet[i][j] == Ocean) moistureDummy[i][j] = 5;
			else moistureDummy[i][j] = 0;

		}
	}
}

/*
Makes all tiles adjacent to a tile of the inputed moisture at least as moist as adjMoisture,
except in the case of mountains, which area assigned to adjMoisture - 2.
*/
void fillMoisture(int size, int** dummyPlanet, int** moistureDummy, int** canvas, int moisture, int adjMoisture) {

	// Copies planet's moistures to canvas.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			canvas[i + 1][j + 1] = moistureDummy[i][j];

		}
	}

	// Copies planet's horizontal wraparound moistures.
	for (int j = 0; j < size; ++j) {
		canvas[size + 1][j + 1] = moistureDummy[0][j]; // Copies left to right.
		canvas[0][j + 1] = moistureDummy[size - 1][j]; // Copies right to left.

	}

	// Copies planet's polar wraparound moistures.
	int polarDistance;
	for (int i = 0; i < size; ++i) {
		if (i < size / 2) polarDistance = size / 2;
		else polarDistance = -size / 2;

		canvas[i][0] = moistureDummy[i + polarDistance][0]; // Copies across north pole.
		canvas[i][size + 1] = moistureDummy[i + polarDistance][size - 1]; // Copies across south pole.

	}

	// Makes any tile adjacent to a tile with the inputed moisture at least adjMoisture.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// If the current depositIndex has the desired moisture, changes the moisture of adjacent tiles.
			if (canvas[i + 1][j + 1] < moisture) {

				// Checks all tiles within a 3x3 area centered around the current tile. Reassigns moisture
				// for each tile that is below adjMoisture.
				for (int x = i; x < i + 3; ++x) {
					for (int y = j; y < j + 3; ++y) {
						if (canvas[x][y] == moisture) {
							if (dummyPlanet[i][j] == Mountain) moistureDummy[i][j] = adjMoisture - 2;
							else moistureDummy[i][j] = adjMoisture;

						}
					}
				}
			}
		}
	}
}

/*
Generates winds from the ocean that carry moisture and heat. Does so by doing Dijkstra style pathfinding from Ocean tiles.
Pathfinding on a given node will cease when the gValue is equal to planetSize, and gValue increases every tile by the
absolute value of the difference between 7 and the tile's heat.
*/
void generateMoisture(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** canvas) {
	int xPos;
	int yPos;
	int baseHeat;
	int gValue;
	LinkedListNode<PathCoordinate>* coord;
	LinkedListNode<PathCoordinate>* currentCoord;

	// Requests a coordList for this function.
	LinkedList<PathCoordinate>* coordList = requestCoordList();

	// Chooses a tile to generate moisture from.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// Chooses water tiles to start generating from. Generates in Dijkstra style away from the center.
			if (dummyPlanet[i][j] > Mountain && !randB(7)) {

				// Sets all values in canvas to 1.
				for (int x = 0; x < size + 2; ++x)
					for (int y = 0; y < size + 2; ++y)
						canvas[x][y] = 1;

				// Creates a LinkedList for storing the frontier.
				LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();

				// Creates the first coordinate.
				coord = coordList->popc();
				coord->value->gValue = 1;
				coord->value->coord = { i, j };
				frontier.addc(coord);
				baseHeat = heatDummy[coord->value->coord.x][coord->value->coord.y];

				// Pathfinds away from the chosen coordinates in Dijkstra fashion.
				while (!frontier.isEmpty()) {

					// Pops the a new coordinate and adds it to explored.
					currentCoord = frontier.popc();
					canvas[currentCoord->value->coord.x][currentCoord->value->coord.y] = 0;

					// Changes the current coordinate's tile's heat.
					heatDummy[currentCoord->value->coord.x][currentCoord->value->coord.y] +=
						(baseHeat - heatDummy[currentCoord->value->coord.x][currentCoord->value->coord.y])
						* (((float)size / currentCoord->value->gValue) / 4);

					// Places currentCoord into the moistureDummy.
					gValue = (size / 2) / currentCoord->value->gValue > 4 ? 4: (size / 2) / currentCoord->value->gValue;
					if (moistureDummy[currentCoord->value->coord.x][currentCoord->value->coord.y] < gValue)
						moistureDummy[currentCoord->value->coord.x][currentCoord->value->coord.y] = gValue;

					// Adds all surrounding coordinates to the frontier.
					for (int x = -1; x < 2; ++x) {
						for (int y = -1; y < 2; ++y) {

							// Gets the coordinates for the new path coordinate.
							xPos = currentCoord->value->coord.x + x;
							yPos = currentCoord->value->coord.y + y;
							wrapAroundPlanet(size, &xPos, &yPos);

							// Creates a new PathCoordinate.
							coord = coordList->popc();
							coord->value->coord = {xPos, yPos};

							// gValue increment is chosen based on planet heat and Mountain presence.
							if (dummyPlanet[xPos][yPos] > Mountain) gValue = 1;
							else if (dummyPlanet[xPos][yPos] == Mountain) gValue = size / 4;
							else gValue = abs(heatDummy[xPos][yPos] - 6) / 2 + 1;
							coord->value->gValue = {currentCoord->value->gValue + gValue};

							// Adds the new coordinate to the frontier if it has not already been explored.
							if (canvas[xPos][yPos] && coord->value->gValue <= size) {
								frontier.addc(coord);
								canvas[coord->value->coord.x][coord->value->coord.y] = 0;

							}
							else coordList->addc(coord);

						}
					}

					// Returns currentCoord's memory.
					coordList->addc(currentCoord);

				}
			}
		}
	}

	// Releases this thread's coordList.
	releaseCoordList(coordList);

}

/*
Generates coastal tiles adjacent to land. Does so by changing all water tiles with at least one adjacent land tile
into coast.
*/
void generateCoastline(int size, int** dummyPlanet) {
	int xPos, yPos;

	// Parses through the planet looking for land tiles.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// If a land tile is found, looks for adjacent water.
			if (dummyPlanet[i][j] < Lake) {
				for (int x = i - 1; x < i + 2; ++x) {
					for (int y = j - 1; y < j + 2; ++y) {
						xPos = x; yPos = y;
						wrapAroundPlanet(size, &xPos, &yPos);

						// If adjacent water is found, reassigns it to coast.
						if (dummyPlanet[xPos][yPos] > Lake) dummyPlanet[xPos][yPos] = Coast;

					}
				}
			}
		}
	}
}

/*
Reclaims the rivers after the inputed index. The rivers and their parse arrays are set to 0.
TODO remove
*/
void initRivers(HabitablePlanet* planet, int currRiver) {

	// Flags all rivers as unused.
	for (; currRiver < NUM_RIVERS; ++currRiver) {
		planet->rivers[currRiver].startingX = 255;

		// Zeros the size.
		planet->rivers[currRiver].size = 0;

		// Zeros the directions.
		for (int j = 0; j < NUM_RIVER_TILES; ++j) {
			planet->rivers[currRiver].riverParse[j].i = 0;

		}
	}
}

/*
Finds an open river slot on the inputed planet.
*/
int findNewRiver(HabitablePlanet* planet) {
	for (int newRiver = 0; newRiver < NUM_RIVERS; ++newRiver)
		if (planet->rivers[newRiver].startingX == 255)
			return newRiver;

}

/*
Calculates the size of all assigned rivers after the given currRiver.
*/
void calcRiverSizes(HabitablePlanet* planet, int currRiver) {
	planet->rivers[currRiver].size = NUM_RIVER_TILES;

	// Calculates the size of the inputed river and all valid rivers thereafter.
	for (; currRiver < NUM_RIVERS; ++currRiver) {
		if (planet->rivers[currRiver].startingX == 255) break;

		// Assigns size.
		for (int currTile = 0; currTile < NUM_RIVER_TILES; ++currTile) {
			if (!planet->rivers[currRiver].riverParse[currTile].i) {
				planet->rivers[currRiver].size = currTile;
				break;

			}
		}
	}
}

/*
Prunes currRiver if it is childless and below the given minSize.
*/
void pruneRivers(HabitablePlanet* planet, int currRiver, int minSize) {
	int size = 0;

	// Calculates the size of this river.
	for (int i = currRiver; i < NUM_RIVERS; ++i) {
		if (planet->rivers[i].startingX == 255) break;
		size += planet->rivers[i].size;

	}

	// Prunes currRiver if it is invalid.
	if (size < minSize) initRivers(planet, currRiver);

}

/*
Checks that the river is not intersecting another river.
This will sometimes return false when there is no actual intersection.
*/
bool checkRiverCorners(int oldX, int oldY, int i, int j, int size, int** riverDummy) {
	int tempX, tempY;
	int intersections = 0;

	// Checks the adjacent x coord.
	tempX = oldX + i;
	tempY = oldY;
	wrapAroundPlanet(size, &tempX, &tempY);
	if (riverDummy[tempX][tempY]) ++intersections;

	// Checks the adjacent y coord.
	tempX = oldX;
	tempY = oldY + j;
	wrapAroundPlanet(size, &tempX, &tempY);
	if (riverDummy[tempX][tempY]) ++intersections;

	// Returns true iff there were two river tiles adjacent this one (intersection).
	return intersections / 2;

}

/*
Generates an individual river.
*/
void generateRiver(int size, int x, int y, int** dummyPlanet, int** heatDummy, int** riverDummy, HabitablePlanet* planet, int currRiver, int gValue) {
	int newX, newY;
	int randCoeff = 4; // TODO what does randCoeff do?

	// Places the river in this tile.
	riverDummy[x][y] = 1;

	// Will always return if the river has moved over 1 / 4th the planet.
	if (gValue >= size / 4) return;

	// Attempts to create a river in each valid direction.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			newX = x + i;
			newY = y + j;
			wrapAroundPlanet(size, &newX, &newY);
			if (dummyPlanet[newX][newY] <= LAND_TILE && heatDummy[newX][newY] >= 0 &&
				heatDummy[newX][newY] <= 15 && !riverDummy[newX][newY] && !(randS() % randCoeff)) {

				// Continues the river in the chosen direction.
				if (randCoeff == 4) {

					// Gets the direction to the next tile.
					if (i < 0 && j > 0) {
						if (checkRiverCorners(x, y, i, j, size, riverDummy)) continue;
						planet->rivers[currRiver].riverParse[gValue].d.sw = 1;

					}
					else if (i == 0 && j > 0) planet->rivers[currRiver].riverParse[gValue].d.s = 1;
					else if (i > 0 && j > 0) {
						if (checkRiverCorners(x, y, i, j, size, riverDummy)) continue;
						planet->rivers[currRiver].riverParse[gValue].d.se = 1;

					}
					else if (i < 0 && j == 0) planet->rivers[currRiver].riverParse[gValue].d.w = 1;
					else if (i > 0 && j == 0) planet->rivers[currRiver].riverParse[gValue].d.e = 1;
					else if (i < 0 && j < 0) {
						if (checkRiverCorners(x, y, i, j, size, riverDummy)) continue;
						planet->rivers[currRiver].riverParse[gValue].d.nw = 1;

					}
					else if (i == 0 && j < 0) planet->rivers[currRiver].riverParse[gValue].d.n = 1;
					else if (i > 0 && j < 0) {
						if (checkRiverCorners(x, y, i, j, size, riverDummy)) continue;
						planet->rivers[currRiver].riverParse[gValue].d.ne = 1;

					}

					// Generates the next river.
					generateRiver(size, newX, newY, dummyPlanet, heatDummy, riverDummy, planet, currRiver, gValue + 1);

				}
				// Splits the river.
				else {

					// Finds an acceptable river to use.
					for (int newRiver = 0; newRiver < NUM_RIVERS; ++newRiver) {
						if (planet->rivers[newRiver].startingX == 255) {

							// Creates the new river.
							planet->rivers[newRiver].startingX = x;
							planet->rivers[newRiver].startingY = y;
							generateRiver(size, x, y, dummyPlanet, heatDummy, riverDummy, planet, newRiver, 0);
							break;

						}
					}
				}

				// Decreases the chance of splitting as splits are created.
				randCoeff *= 32;

			}
		}
	}
}

/*
Generates rivers. Creates moisture surrounding rivers.
*/
void generateRivers(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** riverDummy, HabitablePlanet* planet) {
	int newRiver;
	int newX, newY;

	// Initializes the rivers.
	initRivers(planet, 0);

	// Finds suitable coast tiles to start rivers from.
	for (int w = 0; w < size; ++w) {
		for (int h = 0; h < size; ++h) {
			if (dummyPlanet[w][h] == Coast && heatDummy[w][h] <= 15 && heatDummy[w][h] >= 0 && randB(7) > 110) {

				// Finds a suitable adjacent land tile to start the river on.
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						newX = w + i;
						newY = h + j;
						wrapAroundPlanet(size, &newX, &newY);
						if (dummyPlanet[newX][newY] < Mountain && heatDummy[newX][newY] <= 15 && heatDummy[newX][newY] >= 0) {

							// Finds an open river.
							newRiver = findNewRiver(planet);

							// Places the new river.
							planet->rivers[newRiver].startingX = w;
							planet->rivers[newRiver].startingY = h;
							generateRiver(size, w, h, dummyPlanet, heatDummy, riverDummy, planet, newRiver, 0);

							// Finds the sizes of all new rivers.
							calcRiverSizes(planet, newRiver);

							// Prunes small rivers.
							pruneRivers(planet, newRiver, 5);

						}
					}
				}

			// Exit point for the loop.
			exit:;

			}
		}
	}
}

/*
Makes the tiles surrounding river tiles more moist.
*/
void moistenRivers(int size, HabitablePlanet* planet, int** moistureDummy) {
	int currX, currY, tempX, tempY;
	River currRiver;

	// Moistens each river.
	for (int i = 0; i < NUM_RIVERS; ++i) {
		currRiver = planet->rivers[i];
		currX = currRiver.startingX;
		currY = currRiver.startingY;

		// Moistens the tiles adjacent to each river tile.
		for (int currTile = 0; currTile < currRiver.size; ++currTile) {

			// Moistens the tiles.
			for (int w = -1; w < 2; ++w) {
				for (int h = -1; h < 2; ++h) {
					tempX = currX + w;
					tempY = currY + h;
					wrapAroundPlanet(size, &tempX, &tempY);
					++moistureDummy[tempX][tempY];

				}
			}

			// Moves along the river.
			getPlanetCoordAtDir(size, &currX, &currY, currRiver.riverParse[currTile]);

		}
	}
}

/*
Finds the T^4 of a planet with the inputed heat multiple and albedo.
*/
double findT4(double heatMultiple, double albedo) { return heatMultiple * (1.0 - albedo); }

/*
Finds the heat of a planet with the inputed T^4 and tau.
*/
double findHeat(double t4, double tau) { return std::pow(t4 * (1.0 + 0.75 * tau), 1.0 / 4.0); }

/*
Finds the heat of a planet with the inputed heatMultiple, albedo, and tau.
*/
double findHeat(double heatMultiple, double albedo, double tau) { return findHeat(findT4(heatMultiple, albedo), tau); }

/*
Generates the heat gradient for the planet. The equatorial temperature is planetTemperature + size / 2,
while the polar temperature is planetTemperature - size / 2.

TODO add atmospheric effects, currRiver.e. atmospheres * variance.
*/
void generateHeat(int size, int** heatDummy, int planetTemperature) {

	// Sets the equatorial and polar temperatures in terms of points.
	int equatorTemp = planetTemperature + size;
	int polarTemp = planetTemperature - size;
	polarTemp = (polarTemp - 200) / 10;
	equatorTemp = (equatorTemp - 200) / 10;

	// Sets the rate at which tile temperature increases.
	float gradient = (float)(polarTemp - equatorTemp) / (size / 2);

	// Adds the heat values to heatDummy.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size / 2; ++j) {
			heatDummy[i][size / 2 + j]  = equatorTemp + j * gradient;
			heatDummy[i][size / 2 - j] = equatorTemp + j * gradient;

		}
	}

	// Places the poles (they are otherwise ignored when size is odd).
	for (int i = 0; i < size; ++i) {
		heatDummy[i][size - 1] = polarTemp;
		heatDummy[i][0] = polarTemp;

	}
}

/*
Generates mountain ranges on a planet.
This is done by creating Mountain tiles in random-walk manner until rangeSize Mountains have been
created. When new MountainTiles are being generated, there is a strong bias towards a randomly
determined slope and a weaker bias towards the previously chosen direction.
*/
void generateMountains(int size, int** dummyPlanet, int rangeSize) {

	// Creates a slope between 0 and 7.
	int slope = randB(3);

	// Stores values to track the Mountain range vectors.
	int xPos = randB(8) % size;
	int yPos = randB(8) % size;

	// Finds a starting coordinate that is not in water. Does so by drawing a vector
	// from xBeg and yBeg along slope.
	bool inWater = true;
	for (int i = 0; i < 200 && inWater; ++i) {

		// Ceases looping when land is found.
		if (dummyPlanet[xPos][yPos] <= LAND_TILE) inWater = false;

		// Moves based on the assessed slope.
		switch (slope) {

		// Northwest.
		case(0):
			--xPos;
			--yPos;
			break;
		// North.
		case(1):
			--yPos;
			break;
		// Northeast.
		case(2):
			++xPos;
			--yPos;
			break;
		// West.
		case(3):
			--xPos;
			break;
		// East.
		case(4):
			++xPos;
			break;
		// Southwest.
		case(5):
			--xPos;
			++yPos;
			break;
		// South.
		case(6):
			++yPos;
			break;
		// Southeast.
		case(7):
			++xPos;
			++yPos;
			break;

		}

		// Wraps around the planets ice caps and horizontal edges.
		wrapAroundMountains(size, &xPos, &yPos, &slope);

	}

	// Draws the mountains.
	int xTarget = 0;
	int yTarget = 0;
	for (int i = 0; i < rangeSize && !inWater; ++i) {

		// Adds a 50% starting chance for the range to move towards the preferred target.
		if (randB(1)) {

			// Moves inn the previous direction.
			switch (slope) {

				// Northwest.
			case(0):
				xTarget = xPos - 1;
				yTarget = yPos - 1;
				break;
				// North.
			case(1):
				yTarget = yPos - 1;
				break;
				// Northeast.
			case(2):
				xTarget = xPos + 1;
				yTarget = yPos - 1;
				break;
				// West.
			case(3):
				xTarget = xPos - 1;
				break;
				// East.
			case(4):
				xTarget = xPos + 1;
				break;
				// Southwest.
			case(5):
				xTarget = xPos - 1;
				yTarget = yPos + 1;
				break;
				// South.
			case(6):
				yTarget = yPos + 1;
				break;
				// Southeast.
			case(7):
				xTarget = xPos + 1;
				yTarget = yPos + 1;
				break;

			}

			// Prevents the target from acting redundantly or going out of bounds.
			wrapAroundMountains(size, &xTarget, &yTarget, &slope);
			if (dummyPlanet[xTarget][yTarget] > LAND_TILE) {
				xTarget = xPos;
				yTarget = yPos;

			}
		}
		// Otherwise, moves in a random direction.
		else {

			// Runs until a suitable tile is found.
			for (int i = 0; i < 10; ++i) {

				// Chooses a random direction.
				slope = randB(3);

				// Moves based on the assessed direction.
				switch (slope) {

				// Northwest.
				case(0):
					xTarget = xPos - 1;
					yTarget = yPos - 1;
					break;
				// North.
				case(1):
					yTarget = yPos - 1;
					break;
				// Northeast.
				case(2):
					xTarget = xPos + 1;
					yTarget = yPos - 1;
					break;
				// West.
				case(3):
					xTarget = xPos - 1;
					break;
				// East.
				case(4):
					xTarget = xPos + 1;
					break;
				// Southwest.
				case(5):
					xTarget = xPos - 1;
					yTarget = yPos + 1;
					break;
				// South.
				case(6):
					yTarget = yPos + 1;
					break;
				// Southeast.
				case(7):
					xTarget = xPos + 1;
					yTarget = yPos + 1;
					break;

				}

				// Wraps around the planets ice caps and horizontal edges.
				wrapAroundMountains(size, &xTarget, &yTarget, &slope);

				// Places a mountain tile when a suitable tile is found.
				if (dummyPlanet[xTarget][yTarget] <= LAND_TILE) break;

			}
		}

		// Places a mountain tile at the acceptable spot.
		xPos = xTarget;
		yPos = yTarget;
		dummyPlanet[xPos][yPos] = Mountain;

	}
}

/*
Generates trees on the planet.
*/
void generateTrees(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy, int** buildingDummy, int** canvas, int chance) {

	// Generates trees at random positions.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// Places forests in appropriate places with chance% chance.
			if (dummyPlanet[i][j] < LAND_TILE && moistureDummy[i][j] > 0 && heatDummy[i][j] <= 15 && heatDummy[i][j] >= 0 && randB(7) % 100 < chance) {

				//Determines the appropriate type of forest.
				if (heatDummy[i][j] <= COLD_TEMP) {
					if (moistureDummy[i][j] == 1) buildingDummy[i][j] = BorealForest;
					else buildingDummy[i][j] = DeciduousForest;

				}
				else if (heatDummy[i][j] <= TEMPERATE_TEMP) {
					if (moistureDummy[i][j] == 1) buildingDummy[i][j] = AridForest;
					else buildingDummy[i][j] = TemperateForest;

				}
				else if (heatDummy[i][j] <= HOT_TEMP) {
					if (moistureDummy[i][j] == 1) buildingDummy[i][j] = AridForest;
					else buildingDummy[i][j] = TropicalForest;

				}
			}
		}
	}

	// Creates contiguous forests.
	fillGaps(size, buildingDummy, canvas, 2, BorealForest);
	fillGaps(size, buildingDummy, canvas, 2, DeciduousForest);
	fillGaps(size, buildingDummy, canvas, 2, TemperateForest);
	fillGaps(size, buildingDummy, canvas, 2, AridForest);
	fillGaps(size, buildingDummy, canvas, 2, TropicalForest);
	fillGaps(size, buildingDummy, canvas, 8, 0);

	// Cleans up after the above operation.
	// This is lazier than producing a modified fillGaps,
	// but the performance effect is trivial and short-term.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// Ensures that there are no forests on invalid tiles.
			if (dummyPlanet[i][j] >= Mountain) buildingDummy[i][j] = 0;

			// Ensures that every tile containing a forest has an acceptable moisture level.
			if (buildingDummy[i][j]) {
				if (moistureDummy[i][j] >= 5) moistureDummy[i][j] = 4;
				if (moistureDummy[i][j] <= 0) moistureDummy[i][j] = 1;

			}

			// Ensures that forests spread moisture and heat levels.
			switch (buildingDummy[i][j]) {
			case(BorealForest):
			case(DeciduousForest):
				heatDummy[i][j] = COLD_TEMP;
				break;
			case(TemperateForest):
				heatDummy[i][j] = TEMPERATE_TEMP;
				break;
			case(AridForest):
				if (heatDummy[i][j] > TEMPERATE_TEMP) heatDummy[i][j] = HOT_TEMP;
				else heatDummy[i][j] = TEMPERATE_TEMP;
				break;
			case(TropicalForest):
				heatDummy[i][j] = HOT_TEMP;
				break;

			}
		}
	}
}

/*
Generates surface deposits on a planet. Does so by finding a valid empty tile and creating a
handful of deposits around it.
Returns false upon failure to place.
*/
bool generateSurfaceDeposit(HabitablePlanet* planet, int currDeposit) {
	uint_least64_t lowerBound;
	uint_least64_t upperBound;
	int startingX;
	int startingY;
	int xPos;
	int yPos;

	// Writes every tile in the current deposit to 255.
	memset(planet->surfaceDeposits[currDeposit].xPos, 255, NUM_DEPOSIT_COORDS);
	
	// Determines which deposit to use.
	// TODO extend goods (%1 because must be 0).
	planet->surfaceDeposits[currDeposit].good = randB(1) % 1;

	// Sets the boundaries for deposit size based on deposit type.
	switch (planet->surfaceDeposits[currDeposit].good) {
	case(BaseMetals):
		lowerBound = 1000000000 * ((float)planet->size / EARTH_WIDTH);
		upperBound = 2612470000000000000 * ((float)planet->size / EARTH_WIDTH);
		break;

	}

	// Generates the size of this deposit.
	// TODO randU may not provide enough width.
	planet->surfaceDeposits[currDeposit].quantity = randU() % (upperBound - lowerBound) + lowerBound;

	// Finds a starting point for the deposit. Will return false after 1000 failed placement attempts.
	int attempts = 1000;
	do {
		startingX = randB(8) % planet->size;
		startingY = randB(8) % planet->size;

		// Fails after 1000 failed attempts.
		if (!--attempts) return false;

	} while (planet->planet[index(startingX, startingY, planet->size)].tileData > Mountain ||
		planet->planet[index(startingX, startingY, planet->size)].deposit);

	// Places nodes of the deposit at random points nearby.
	int depositIndex = 0;
	for (int i = 0; i < planet->size / 4 && i < NUM_DEPOSIT_COORDS; ++i) {

		// Generates a random position.
		xPos = startingX + randB(4) % (planet->size / 8) - planet->size / 16;
		yPos = startingY + randB(4) % (planet->size / 8) - planet->size / 16;
		wrapAroundPlanet(planet->size, &xPos, &yPos);

		// If the position is not taken and is valid, places a deposit there.
		if (!planet->planet[index(xPos, yPos, planet->size)].deposit &&
			planet->planet[index(xPos, yPos, planet->size)].tileData <= Mountain) {
			planet->surfaceDeposits[currDeposit].xPos[depositIndex] = xPos;
			planet->surfaceDeposits[currDeposit].yPos[depositIndex] = yPos;
			planet->planet[index(xPos, yPos, planet->size)].deposit = 1;
			++depositIndex;

		}
	}

	// Assigns the unoccupied xPos and yPos to 255.
	if (depositIndex < NUM_DEPOSIT_COORDS) {
		planet->surfaceDeposits[currDeposit].xPos[depositIndex] = 255;
		planet->surfaceDeposits[currDeposit].yPos[depositIndex] = 255;

	}

	// Returns false after failed execution, otherwise true.
	return (bool)depositIndex;

}

/*
Generates deposits on a planet.
Deposits are measured in kilotons since tons require too many bits.
*/
void generateDeposits(HabitablePlanet* planet) {

	// Generates surface deposits.
	for (int i = 0; i < NUM_DEPOSITS_SURFACE; ++i) {
		if (!generateSurfaceDeposit(planet, i)) {
			planet->surfaceDeposits[i].good = 255;
			break;

		}
	}
}

/*
Gets the next planet coordinate in the current direction.
*/
void getPlanetCoordAtDir(int size, int* xPos, int* yPos, Direction dir) {

	// Moves to the next tile.
	if (dir.d.nw) {
		--(*xPos);
		--(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.n) {
		--(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.ne) {
		++(*xPos);
		--(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.w) {
		--(*xPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.e) {
		++(*xPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.sw) {
		--(*xPos);
		++(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.s) {
		++(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
	else if (dir.d.se) {
		++(*xPos);
		++(*yPos);
		wrapAroundPlanet(size, xPos, yPos);

	}
}

/*
Wraps mountains around a planet's ice caps and horizontal edges.
*/
void wrapAroundMountains(int size, int* xPos, int* yPos, int* slope) {

	// Wraps over the north pole.
	if (*yPos < 0) {
		*xPos += size / 2;
		*yPos = 0;
		*slope += 5;

	}
	// Wraps over the south pole.
	else if (*yPos >= size) {
		*xPos += size / 2;
		*yPos = size - 1;
		*slope -= 5;

	}

	// Wraps around the right and left edges.
	while (*xPos >= size) *xPos -= size;
	while (*xPos < 0) *xPos += size;

}

/*
Reassigns planet tileIDs based on the PlanetTiles' moisture and heat.
*/
void reassignID(int size, int** dummyPlanet, int** moistureDummy, int** heatDummy) {

	// Parses through each planet and reassigns the ids of tiles based on their heat and moisture.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {

			// Ignores Mountains and bodies of water.
			if (dummyPlanet[i][j] < Mountain) {

				// Freezing.
				if (heatDummy[i][j] < 0) {
					dummyPlanet[i][j] = Permafrost;

				}
				// Cold.
				else if (heatDummy[i][j] <= 4) {

					// Desert.
					if (moistureDummy[i][j] == 0) dummyPlanet[i][j] = Desert;
					// Tundra.
					else if (moistureDummy[i][j] == 1) dummyPlanet[i][j] = Tundra;
					// Cold Steppe.
					else if (moistureDummy[i][j] == 2) dummyPlanet[i][j] = Cold_Steppe;
					// Cold Grassland.
					else if (moistureDummy[i][j] < 5) dummyPlanet[i][j] = Cold_Grassland;
					// Cold Wetland.
					else if (moistureDummy[i][j] >= 5) dummyPlanet[i][j] = Cold_Wetland;

				}
				// Temperate.
				else if (heatDummy[i][j] <= 8) {

					// Desert.
					if (moistureDummy[i][j] == 0) dummyPlanet[i][j] = Desert;
					// Scrubland.
					else if (moistureDummy[i][j] == 1) dummyPlanet[i][j] = Scrubland;
					// Steppe.
					else if (moistureDummy[i][j] == 2) dummyPlanet[i][j] = Temperate_Steppe;
					// Temperate_Grassland.
					else if (moistureDummy[i][j] < 5) dummyPlanet[i][j] = Temperate_Grassland;
					// Temperate Wetland.
					else if (moistureDummy[i][j] >= 5) dummyPlanet[i][j] = Temperate_Wetland;

				}
				// Warm.
				else if (heatDummy[i][j] <= 15) {

					// Desert.
					if (moistureDummy[i][j] == 0) dummyPlanet[i][j] = Desert;
					// Desert.
					else if (moistureDummy[i][j] == 1) dummyPlanet[i][j] = Dry_Savannah;
					// Savannah.
					else if (moistureDummy[i][j] == 2) dummyPlanet[i][j] = Savannah;
					// Hot Grassland.
					else if (moistureDummy[i][j] <= 5) dummyPlanet[i][j] = Hot_Grassland;
					// Hot Wetland.
					else if (moistureDummy[i][j] >= 5) dummyPlanet[i][j] = Hot_Wetland;

				}
				// Scorched.
				else {
					dummyPlanet[i][j] = Seared;

				}
			}

			//////////////////////////////
			// DEBUG: Creates heat map. //
			//////////////////////////////
			/*if (heatDummy[currRiver][j] < 0) dummyPlanet[currRiver][j] = Permafrost;
			else if (heatDummy[currRiver][j] < 5) dummyPlanet[currRiver][j] = Cold_Steppe;
			else if (heatDummy[currRiver][j] < 9) dummyPlanet[currRiver][j] = Cold_Grassland;
			else if (heatDummy[currRiver][j] < 15) dummyPlanet[currRiver][j] = Desert;
			else dummyPlanet[currRiver][j] = Seared;*/

		}
	}
}

/*
Converts a dummyPlanet of tileIDs to a planet's tileIDs. Also sets all buildings to 0.
All land will be put under ownership of the null owner.
*/
void dummyToTileID(int size, HabitablePlanet* planet, int** dummyPlanet, int** buildingDummy) {

	// Converts each dummyPlanet tile to a tileID on planet.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			planet->planet[index(i, j, planet->size)].tileData = dummyPlanet[i][j];

		}
	}

	// Adds all land and water on the planet to the null owner.
	for (int i = 0; i < size * size; ++i) {

		// Adds Land.
		if (planet->planet[i].tileData <= LAND_TILE && planet->planet[i].buildingID == NoBuilding) {
			++pOwnerBuildingQ(0, LAND_OWNED, planet, 0);

		}
		// Adds Water. TODO may need to add NoBuilding check.
		else if (planet->planet[i].tileData > LAND_TILE /*&& planet->planet[i].buildingID == NoBuilding*/) {
			++pOwnerBuildingQ(0, LAND_OWNED, planet, 2);

		}
	}

	// Adds each forest building's output to the planet.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			buildingFuncs[buildingDummy[i][j]](planet, nullptr, 0, i, j, nullptr);

		}
	}
}