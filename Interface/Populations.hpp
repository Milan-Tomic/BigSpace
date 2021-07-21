#pragma once

/*
Template for a race.
*/
struct RaceTemplate {

	// This race's parent.
	RaceTemplate* parent;

	// This race's name.
	std::string name;

	// Gases that this race consumes.
	uint_least8_t consumedGases[NUM_GASES];

	// Gases that this race produces.
	uint_least8_t producedGases[NUM_GASES];

	// Goods that this race consumes for its life needs.
	// The last two indices are dedicated to veg/meat food.
	uint_least8_t consumed[NUM_GOODS_ALL];

	// Additive bonus productivities for this race.
	// The last two indices are dedicated to veg/meat food.
	uint_least8_t productitivities[NUM_GOODS_ALL];

	// TODO: Temperature preference of this race.

	// TODO ethical proclivities.

};

/*
Object representing a race on a planet.
*/
struct RaceInstance {

	// Points to this raceInstance's actual race
	// TODO (may be replaced with an id, arr).
	RaceTemplate* race;

	// Number of pops for this race on this planet.
	uint_fast32_t numPops;

	// Employment numbers.
	uint_fast32_t numUnemployed;

	// TODO religions.

	// TODO political factions.

};

// Number of elements in a RacePage. Should be around 1MB.
#define RACE_PAGE_SIZE ((1048576 - 8) / sizeof(RaceTemplate))

// Default number of RaceTemplate pages. This is also the number used when expanding the roster of
// RaceTemplate pages.
#define NUM_RACE_PAGES 256

// Page of Races.
struct RacePage {
	RaceTemplate races[RACE_PAGE_SIZE];
	uint16_t arrCurrRace;
	bool active;

	// 5 bytes of padding.

};

// Contains all RacePages. Minimizes memory use from allocations.
int numRacePages;
RacePage** racePages;

// Default races that always exist.
RaceTemplate orcs;

/*
Initializes Orcs.
*/
void initOrcs() {

	// Does nothing if Orcs already exist.
	if (!strcmp(orcs.name.c_str(), "Orcs")) return;

	// Names Orcs.
	orcs.name = "Orcs";

	// Orcs do not consume or produce any gases.

	// Orcs must consume food to live.
	orcs.consumed[Veg] = 1;
	orcs.consumed[Meat] = 1;

	// Orcs have no bonus productivities.

}

/*
Retrieves Orcs.
*/
RaceTemplate* getOrcs() {
	return &orcs;

}

/*
Creates a paging table of RaceTemplates. Initialized with capacity for numRaces.
It is expected that this function will be called exactly once per universe and in a single-threaded manner.

TODO initialize Organic, Lithic, Robotic, etc. (Humanoid, etc.?)
Initializes Orcs.
*/
void initRaces(int numRaces) {

	// Frees racePages for reuse.
	if (numRacePages) {
		for (int i = 0; i < numRacePages; ++i) {
			if (racePages[i]) delete racePages[i];
			racePages[i] = nullptr;

		}
		delete[] racePages;

	}

	// Initializes racePages.
	numRacePages = numRaces / RACE_PAGE_SIZE + 1;
	racePages = (RacePage**)calloc(numRacePages + NUM_RACE_PAGES - (numRacePages % NUM_RACE_PAGES), sizeof(RacePage*));
	for (int i = 0; i < numRacePages; ++i) racePages[i] = (RacePage*)calloc(1, sizeof(RacePage));

	// Initializes Orcs.
	initOrcs();

}

/*
Places a RaceTemplate. Generates a new RacePage if there are none available.
Resizes racePages in increments of 256 when the object pool is insufficiently large.
*/
RaceTemplate* placeRace() {
	static std::shared_mutex placeRaceMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(placeRaceMutex);

	// Attempts to place the RaceTemplate within one of the current pages.
	RaceTemplate* race = nullptr;
	for (int page = 0; page < numRacePages; ++page) {
		if (racePages[page]->arrCurrRace < RACE_PAGE_SIZE) {
			race = &racePages[page]->races[racePages[page]->arrCurrRace];
			++racePages[page]->arrCurrRace;
			break;

		}
	}

	// If placement failed, creates a new page and places the RaceTemplate there.
	if (race == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numRacePages + 1 % NUM_RACE_PAGES))
			racePages = (RacePage**)realloc(racePages, (numRacePages + NUM_RACE_PAGES) * sizeof(RacePage*));

		// Creates a new RacePage.
		racePages[numRacePages] = (RacePage*)calloc(1, sizeof(RacePage));

		// Places the RaceTemplate in the first index of the new page.
		race = &(racePages[numRacePages]->races[0]);
		++racePages[numRacePages]->arrCurrRace;

		// Moves on to the next page.
		++numRacePages;

	}

	// Returns a pointer to the RaceTemplate.
	return race;

}

/*
Finds the location of an inputed RaceTemplate within the paging table.
*/
inline void findRaceIndex(int& page, int& index, RaceTemplate* race) {

	// Finds the page.
	for (page = 0; page < numRacePages; ++page) {

		// Returns the value if the RaceTemplate is within bounds.
		if ((uint_least64_t)race >= (uint_least64_t)racePages[page] &&
			(uint_least64_t)race < (uint_least64_t)racePages[page] + sizeof(RacePage)) {
			index = ((uint_least64_t)race - (uint_least64_t)racePages[page]) / sizeof(RaceTemplate);
			return;

		}
	}
}