#pragma once

// Tells the compiler about HabitablePlanets and Governments.
extern class HabitablePlanet;
extern class Government;

/*
Class representing a colony on a planet.

165 bytes, 3 bytes padding.
sizeof is 168.
*/
class Colony {
public:

	// Event queue of the Colony.
	std::vector<Event> events; // 32 bytes

	// Threat queue of the Colony.
	std::vector<Threat> threats; // 32 bytes

	// The government controlling this Colony.
	Government* government; // 8 bytes

	// The planet that this colony occupies.
	HabitablePlanet* planet; // 8 bytes.

	// The market of this colony.
	Market* market; // 8 bytes.

	// Population of this colony.
	RaceInstance* pops; // 8 bytes.

	// Queue storing land expansion targets.
	Coord landTargets[NUM_TARGETS]; // 2 * NUM_TARGETS = 32 bytes.

	// Queue storing water expansion targets.
	Coord waterTargets[NUM_TARGETS]; // 8 * NUM_TARGETS = 32 bytes.

	// Number of races in this colony.
	uint_least16_t numRaces; // 2 bytes.

	// Owner corresponding to this colony's government.
	uint_least8_t governmentOwner; // 1 byte.

	// Index within landTargets.
	uint_least8_t landTarget; // 1 byte.

	// Index within waterTargets.
	uint_least8_t waterTarget; // 1 byte.

	// 3 bytes padding.

	// Deconstructor for the Colony.
	~Colony();

	// Begins tracking a Threat to this Colony.
	// Threat related functions are all placed in Event Handling.hpp
	inline void addThreat(Threat threat);
	inline void addThreat(Squadron* squadron);

	// Stops tracking a Threat to this Colony.
	inline void removeThreat(Threat threat);
	inline void removeThreat(Squadron* squadron);

	// Handles tracking all Threats to this Colony.
	inline void handleThreats();
	void trackThreat(Threat& threat);

	// Returns the location of the Colony.
	inline CoordU loc();

	// Returns the scanning strength of this Colony. Scanning strength is the
	// maximum scan value of any Ship.
	inline uint_least8_t scan();

	// Returns true if the Colony can scan a given tile, otherwise false.
	inline bool scan(CoordU dest, int distance);
	inline bool scan(CoordU dest);

	// Adds an Event to this Colony.
	inline void addEvent(Event event);

	// Handles all Events in this Colony.
	inline void handleEvents();

	// Fills the landTargets array.
	void findLand();

	// Fills the waterTargets array.
	void findWater();

	// Requests a single land tile from the landTargets array.
	Coord requestLand();

	// Requests a single water tile from the waterTargets array.
	Coord requestWater();

	// Finds employment and housing for pops in a colony.
	void findEmployment();

	// Places new pops of the given race on a planet.
	void newPops(RaceTemplate* race, int numPops);

};

/*
Adds a Colony to this Market (and vice versa).
*/
void Market::addColony(Colony* colony) {
	++numColonies;
	colony->market = this;

}

/*
Removes a Colony from this Market (and vice versa).
May delete the Market or change its capital.
*/
extern Colony* searchColonyTable(std::function<bool(Colony*)> search);
void Market::removeColony(Colony* colony) {

	// Removes the reference to this Market.
	colony->market = nullptr;
	--numColonies;

	// Deletes the Market if it has no references.
	if (!numColonies) freeMarket(this);
	// Finds a new Market capital if the deleted Colony was the previous capital and
	// another Colony remains.
	else if (marketCapital == colony) {
		marketCapital = searchColonyTable(
			[this](Colony* col)->bool { return col->market == this; });

	}
}

// Number of elements in a ColonyPage. Should be around 1MB.
#define COLONY_PAGE_SIZE ((1048576 - 8) / sizeof(Colony))

// Default number of Colony pages. This is also the number used when expanding the roster of
// Colony pages.
#define NUM_COLONY_PAGES 256

// Page of Colonies.
struct ColonyPage {
	Colony colonies[COLONY_PAGE_SIZE];
	uint16_t arrCurrColony;
	uint16_t numOpen;
	bool active;

	// 3 bytes of padding?

};

// Contains all ColonyPages. Minimizes memory use from allocations.
int numColonyPages;
ColonyPage** colonyPages;

/*
Creates a paging table of Colonies. Initialized with capacity for numColonies.
It is expected that this function will be called exactly once per universe and in a single-threaded manner.
*/
void initColonies(int numColonies) {

	// Frees colonyPages for reuse.
	if (numColonyPages) {
		for (int i = 0; i < numColonyPages; ++i) {
			if (colonyPages[i]) delete colonyPages[i];
			colonyPages[i] = nullptr;

		}
		free(colonyPages);

	}

	// Initializes colonyPages.
	numColonyPages = numColonies / COLONY_PAGE_SIZE + 1;
	colonyPages = (ColonyPage**)calloc(numColonyPages + NUM_COLONY_PAGES - (numColonyPages % NUM_COLONY_PAGES), sizeof(ColonyPage*));
	for (int i = 0; i < numColonyPages; ++i) colonyPages[i] = (ColonyPage*)calloc(1, sizeof(ColonyPage));

}

/*
Places a Colony. Generates a new ColonyPage if there are none available.
Resizes colonyPages in increments of 256 when the object pool is insufficiently large.
*/
Colony* placeColony() {
	static std::shared_mutex placeColonyMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex > lock(placeColonyMutex);

	// Attempts to place the colony within one of the current pages.
	Colony* colony = nullptr;
	for (int page = 0; page < numColonyPages; ++page) {
		if (colonyPages[page]->arrCurrColony < COLONY_PAGE_SIZE) {
			colony = &colonyPages[page]->colonies[colonyPages[page]->arrCurrColony];
			++colonyPages[page]->arrCurrColony;
			break;

		}
	}

	// If placement failed, creates a new page and places the colony there.
	if (colony == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numColonyPages + 1 % NUM_COLONY_PAGES))
			colonyPages = (ColonyPage**)realloc(colonyPages, (numColonyPages + NUM_COLONY_PAGES) * sizeof(ColonyPage*));

		// Creates a new ColonyPage.
		colonyPages[numColonyPages] = (ColonyPage*)calloc(1, sizeof(ColonyPage));

		// Places the colony in the first index of the new page.
		colony = &(colonyPages[numColonyPages]->colonies[0]);
		++colonyPages[numColonyPages]->arrCurrColony;

		// Moves on to the next page.
		++numColonyPages;

	}

	// Initializes the Colony.
	colony->threats = std::vector<Threat>();
	colony->events = std::vector<Event>();

	// Returns a pointer to the colony.
	return colony;

}

/*
Requests a ColonyPage. Returns the index of the next available page in colonyPages, otherwise -1.
TODO if infrastructure permits it, use a static global to increment through colony pages.
*/
int requestColonyPage() {
	static std::shared_mutex requestColonyPageMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(requestColonyPageMutex);

	// Parses through each galaxy and returns the next available one.
	for (int i = 0; i < numColonyPages; ++i) {
		if (!colonyPages[i]->active) {
			colonyPages[i]->active = true;
			return i;

		}
	}

	// Returns -1 if no ColonyPage was found.
	return -1;

}

/*
Returns a ColonyPage.
*/
void releaseColonyPage(int index) {
	colonyPages[index]->active = false;

}

// Returns all ColonyPages.
void releaseAllColonyPages() {
	for (int i = 0; i < numColonyPages; ++i) colonyPages[i]->active = false;

}

/*
Searches through the entire paging table in order to find a Given colony.
Return the desired Colony if it is found, else nullptr.
*/
Colony* searchColonyTable(std::function<bool(Colony*)> search) {

	// Searches through the paging table.
	for (int page = 0; page < numColonyPages; ++page) {
		for (int col = 0; col < colonyPages[page]->arrCurrColony; ++col) {

			// Returns the desired Colony if it is found.
			if (search(&colonyPages[page]->colonies[colonyPages[page]->arrCurrColony])) {
				return &colonyPages[page]->colonies[colonyPages[page]->arrCurrColony];

			}
		}
	}

	// Returns nullptr if the desired Colony is not found.
	return nullptr;

}

/*
Finds the page of a given Colony within the paging table.
*/
inline void findColonyPage(int& page, Colony* colony) {

	// Finds the page.
	for (page = 0; page < numColonyPages; ++page) {

		// Returns the value if the Colony is within bounds.
		if ((char*)colony >= (char*)colonyPages[page] &&
			(char*)colony < (char*)colonyPages[page] + sizeof(ColonyPage)) {
			return;

		}
	}
}

/*
Finds the page and index of a given Colony within the paging table.
*/
inline void findColonyIndex(int& page, int& index, Colony* colony) {

	// Finds the page.
	findColonyPage(page, colony);
	index = ((char*)colony - (char*)colonyPages[page]) / sizeof(Colony);

}

/*
Marks a Colony as being available for reuse.
*/
inline void freeColony(Colony* colony) {
	int page;

	// Marks that there is a free Colony in the page.
	findColonyPage(page, colony);
	++colonyPages[page]->numOpen;

	// Clears the Colony.
	colony->~Colony();

}

// Undefines variables which are used only for initializing Colonies.
#undef NUM_COLONY_PAGES
#undef ColonyPage
#undef numColonyPages
#undef colonyPages

/*
Deconstructs the Colony.

Market contents, pops, and tiles should be resolved elsewhere.
*/
Colony::~Colony() {

	// Clears all references to the Colony.
	government->removeColony(this);

	// Clears this Colony's market.
	market->removeColony(this);

	// Clears the Colony's owner.
	// TODO manage colony changing hands more specifically, i.e. manage corporations.
	memset(&planet->owners[governmentOwner], 0, sizeof(planet->owners[governmentOwner]));

	// Deletes this Colony's pops.
	delete pops;

	// Deletes the Event queue associated with this Colony.
	events.clear();
	threats.shrink_to_fit();

	// Removes the threats associated with this Colony.
	threats.clear();
	threats.shrink_to_fit();

	// Clears all memory in this Colony.
	memset(this, 0, sizeof(this));

}

/*
Returns the location of this Colony.
*/
inline CoordU Colony::loc() {
	return this->planet->loc;

}

/*
Returns the scanning strength of this Colony.

TODO presently returns 4.
*/
inline uint_least8_t Colony::scan() {
	return 4;

}

/*
Returns true if the Colony can scan a given tile, otherwise false.
*/
inline bool Colony::scan(CoordU dest, int distance) {
	uint_least8_t scan = this->scan();
	return 64 * scan > uSensorCost(dest.x, dest.y) * distance * distance;

}

/*
Returns true if the Colony can scan a given tile, otherwise false.
*/
inline bool Colony::scan(CoordU dest) {
	scan(dest, calcTaxiDistance(loc().x, loc().y, dest.x, dest.y));

}

/*
Adds an Event to this Colony.
*/
inline void Colony::addEvent(Event event) {
	events.push_back(event);

}

/*
Handles all Events in this Colony.
*/
inline void Colony::handleEvents() {

	// Handles every Event in this Colony.
	for (Event event : events) colonyEventHandler[event.id](event, this);

}

/*
Places a worker in an owner's building.
*/
void placeWorker(HabitablePlanet* planet, int owner, int building, int pop) {
	BuildingManifest* own = &planet->owners[owner].ownedBuildings[building];

	// Attempts to find the inputed pop within the owner.
	int i = 0;
	for (; i < own->numRaces; ++i) if (pop == own->workerRaces[i].workerRace) break;

	// Allocates a new race if the pop was not found.
	if (i == own->numRaces) {
		++own->numRaces;
		if (own->workerRaces == nullptr) own->workerRaces = (ManifestRace*)calloc(1, sizeof(ManifestRace) * own->numRaces);
		else own->workerRaces = (ManifestRace*)realloc(own->workerRaces, sizeof(ManifestRace) * own->numRaces);
		own->workerRaces[i].workerRace = pop;
		own->workerRaces[i].workerQuantity = 0;

	}

	// Places the worker.
	++own->workerRaces[i].workerQuantity;
	--own->numOpen;

}

/*
Fills the landTargets array.
Does so by finding tiles adjacent to this Colony's Government's land.
TODO make this use all owners that relate to this colony. (what did I mean by this?)

Other find functions are modeled after this one.
*/
void Colony::findLand() {
	int indices[NUM_TARGETS];
	int currIndex = 0;
	int maxCoords = planet->size * planet->size / 2;
	int xPos, yPos;
	int n;

	// Acquires a dummy.
	Coord* expansions = (Coord*)requestLargeDummy();

	// Resets landTarget.
	landTarget = 0;

	// Creates a list of all tiles adjacent to tiles owned by this government.
	for (int i = 0; i < planet->size; ++i) {
		for (int j = 0; j < planet->size; ++j) {

			// Adds a tile to the list if it is an empty land tile and there is any adjacent owned tile.
			if (!planet->planet[index(i, j, planet->size)].owner &&
				planet->planet[index(i, j, planet->size)].tileData <= LAND_TILE) {
				for (int x = -1; x < 2; ++x) {
					for (int y = -1; y < 2; ++y) {
						xPos = i + x;
						yPos = j + y;
						wrapAroundPlanet(planet->size, &xPos, &yPos);

						// Exits the loop when an adjacent tile is found.
						if (planet->planet[index(xPos, yPos, planet->size)].owner == governmentOwner) {
							expansions[currIndex++] = { (uint_least8_t)i, (uint_least8_t)j };
							goto exit1;

						}
					}
				}
			}

		// Exits the loop when an appropriate tile is found.
		exit1:;

		}
	}

	// Fills landTargets deterministically if there are few targets.
	if (currIndex < NUM_TARGETS * 2) {

		// Fills the array of landTargets.
		int i = 0;
		for (; i < currIndex && i < NUM_TARGETS; ++i) landTargets[i] = expansions[i];
		if (i < NUM_TARGETS) landTargets[i] = {0, 0};

	}
	// Fills landTargets non-deterministically if there are many targets.
	else {

		// Finds an approriate number of bits for randB.
		for (n = 0; n < sizeof(currIndex) * 8; ++n) if ((1 << n) > currIndex) break;

		// Fills the array of landTargets.
		for (int i = 0; i < NUM_TARGETS; ++i) {

			// Selects an index and checks its validity.
			indices[i] = randB(n) % currIndex;
			for (int j = 0; j < i; ++j) {
				if (indices[j] == indices[i]) {
					--i;
					goto exit2;

				}
			}
			
			// Places the coordinate if the index is valid.
			landTargets[i] = expansions[i];

			// Continues checking indices if the new index is invalid.
			exit2:;

		}
	}

	// Releases the dummy.
	releaseLargeDummy((uint_least8_t*)expansions);

}

/*
Requests a land tile from this colony.
This makes no guarantee that the returned land will still be empty.
Will return {0, 0} if there are no land targets.

Other request functions are modeled after this one.
*/
Coord Colony::requestLand() {

	// Will not attempt to find land if there is no unowned land.
	if (!pOwnerBuildingQ(0, LAND_OWNED, planet, 0) &&
		!pOwnerBuildingQ(0, LAND_OWNED, planet, 1))
		return { 0, 0 };

	// Finds more land if no land is available.
	if (landTarget >= NUM_TARGETS || !landTargets[landTarget]) {
		findLand();

	}

	// Moves on to the next landTarget.
	++landTarget;

	// Returns the landTarget.
	return landTargets[landTarget - 1];

}

/*
Fills the waterTargets array.
Does so by finding tiles adjacent to this Colony's Government's water/harbours.
TODO make this use all owners that relate to this colony. (what did I mean by this?)

This is largely modeled after findLand.
*/
void Colony::findWater() {
	int indices[NUM_TARGETS];
	int currIndex = 0;
	int maxCoords = planet->size * planet->size / 2;
	int xPos, yPos;
	int n;

	// Acquires a dummy.
	Coord* expansions = (Coord*)requestLargeDummy();

	// Resets waterTarget.
	waterTarget = 0;

	// Creates a list of all tiles adjacent to tiles owned by this government.
	for (int i = 0; i < planet->size; ++i) {
		for (int j = 0; j < planet->size; ++j) {

			// Adds an unowned tile to the list if it is an empty water tile and there is any adjacent owned water tile.
			if (!poIndex(i, j, planet) && pTileID(i, j, planet) > LAND_TILE) {

				// Searches for an adjacnent owned water tile.
				for (int x = -1; x < 2; ++x) {
					for (int y = -1; y < 2; ++y) {
						xPos = i + x;
						yPos = j + y;
						wrapAroundPlanet(planet->size, &xPos, &yPos);

						// Exits the loop when an adjacent owned water tile is found.
						if (pTileID(xPos, yPos, planet) > LAND_TILE && poIndex(xPos, yPos, planet) == governmentOwner) {
							expansions[currIndex++] = { (uint_least8_t)i, (uint_least8_t)j };
							goto exit1;

						}
					}
				}
			}

		// Exits the loop when an appropriate tile is found.
		exit1:;

		}
	}

	// Fills waterTargets deterministically if there are few targets.
	if (currIndex < NUM_TARGETS * 2) {

		// Fills the array of waterTargets.
		int i = 0;
		for (; i < currIndex && i < NUM_TARGETS; ++i) waterTargets[i] = expansions[i];
		if (i < NUM_TARGETS) waterTargets[i] = { 0, 0 };

	}
	// Fills waterTargets non-deterministically if there are many targets.
	else {

		// Finds an approriate number of bits for randB.
		for (n = 0; n < sizeof(currIndex) * 8; ++n) if ((1 << n) > currIndex) break;

		// Fills the array of waterTargets.
		for (int i = 0; i < NUM_TARGETS; ++i) {

			// Selects an index and checks its validity.
			indices[i] = randB(n) % currIndex;
			for (int j = 0; j < i; ++j) {
				if (indices[j] == indices[i]) {
					--i;
					goto exit2;

				}
			}

			// Places the coordinate if the index is valid.
			waterTargets[i] = expansions[i];

		// Continues checking indices if the new index is invalid.
		exit2:;

		}
	}

	// Releases the dummy.
	releaseLargeDummy((uint_least8_t*)expansions);

}

/*
Requests a water tile from this colony.
This makes no guarantee that the returned water will still be empty.
Will return {0, 0} if there are no water targets.

This is largely modeled after requestLand.
*/
Coord Colony::requestWater() {

	// Will not attempt to find water if there is no unowned water.
	if (!pOwnerBuildingQ(0, LAND_OWNED, planet, 2)) return { 0, 0 };

	// Finds more water if no water is available.
	if (waterTarget >= NUM_TARGETS || !waterTargets[waterTarget]) {
		findWater();

	}

	// Moves on to the next waterTarget.
	++waterTarget;

	// Returns the waterTarget,
	return waterTargets[waterTarget - 1];

}

/*
Finds employment and housing on a planet.
TODO rewrite. This function is very temporary.
*/
void Colony::findEmployment() {
	int owner;

	// Goes through each pop.
	for (int pop = 0; pop < numRaces; ++pop) {

		// Finds a building without workers and places a worker within.
		for (int w = 0; w < planet->size; ++w) {
			for (int h = 0; h < planet->size; ++h) {

				// TODO only works with Mine1
				if (planet->planet[index(w, h, planet->size)].buildingID == Mine1) {
					owner = planet->planet[index(w, h, planet->size)].owner;
					while (planet->planet[index(w, h, planet->size)].numData < 2 &&
						pops[pop].numUnemployed) {
						++planet->planet[index(w, h, planet->size)].numData;
						placeWorker(planet, owner, MINE, pop);
						--pops[pop].numUnemployed;

					}
				}
				// Moves on to the next pop when there are no more unemployed of this pop.
				if (!pops[pop].numUnemployed) {
					++pop;
					if (pop == numRaces) return;

				}
			}
		}
	}
}

/*
Places a race on a planet. Will initialize a Food item in its market if none exists.
// TODO track employment.
*/
void Colony::newPops(RaceTemplate* race, int numPops) {
	RaceInstance* pop = nullptr;

	// Tries to find an existing instance of the inputed race.
	for (int i = 0; i < numRaces; ++i) {
		if (pops[i].race == race) {
			pop = &pops[i];
			break;

		}
	}

	// If the inputed race was not found, creates a new RaceInstance for it.
	if (pop == nullptr) {

		// Extends pops.
		if (pops == nullptr) pops = (RaceInstance*)calloc(++numRaces, sizeof(RaceInstance));
		else pops = (RaceInstance*)realloc(pops, ++numRaces * sizeof(RaceInstance));

		// Places the new race within pops.
		pops[numRaces - 1] = {race, (uint_fast32_t)numPops, (uint_fast32_t)numPops};

		// Creates a Food for the inputed race.
		market->tryAddMarketFood(race);

	}
	// If the inputed race was found, increments numPops within the given raceInstance.
	else {
		pop->numPops += numPops;
		pop->numUnemployed += numPops;

	}

	// Attempts to find employment for the new pops.
	this->findEmployment();

}