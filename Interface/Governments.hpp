#pragma once

// Declaration for Government.
class Government;

// Tells the compiler about battles.
extern class Battle;

// Tells the compiler about buildingFuncs.
extern void (*buildingFuncs[])(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData);

// Planetary expansion algorithm for an Orcish government.
void OrcGroundExpand(Government* tribe);

// Number of elements in a GovernmentPage. Should be around 1MB.
#define GOVERNMENT_PAGE_SIZE ((1048576 - 8) / sizeof(Government))

// Default number of government pages. This is also the number used when expanding the roster of
// government pages.
#define NUM_GOVERNMENT_PAGES 256

// Stores codes corresponding to each government action.
// Used for Save and Load.
enum GovernmentBehaviours {
	NULL_BEHAVIOUR,
	ORC_EXPAND,
	ORC_PRODUCE,
	ORC_REINFORCE

};

// Page of governments.
struct GovernmentPage {
	Government governments[GOVERNMENT_PAGE_SIZE];
	uint16_t arrCurrGovernment;
	uint16_t numOpen;
	bool active;

	// 3 bytes of padding?

};

// Contains all GovernmentsPages. Minimizes memory use from allocations.
int numGovernmentPages;
GovernmentPage** governmentPages;

/*
Creates a paging table of Governments. Initialized with capacity for numGovernments.
It is expected that this function will be called exactly once per universe and in a single-threaded manner.
*/
void initGovernments(int numGovernments) {

	// Frees governmentPages for reuse.
	if (numGovernmentPages) {
		for (int i = 0; i < numGovernmentPages; ++i) {
			if (governmentPages[i]) delete governmentPages[i];
			governmentPages[i] = nullptr;

		}
		delete[] governmentPages;

	}

	// Initializes governmentPages.
	numGovernmentPages = numGovernments / GOVERNMENT_PAGE_SIZE + 1;
	governmentPages = (GovernmentPage**)calloc(numGovernmentPages + NUM_GOVERNMENT_PAGES - (numGovernmentPages % NUM_GOVERNMENT_PAGES), sizeof(GovernmentPage*));
	for (int i = 0; i < numGovernmentPages; ++i)
		governmentPages[i] = (GovernmentPage*)calloc(1, sizeof(GovernmentPage));

}

/*
Places a Government.
*/
Government* placeGovernment() {
	static std::mutex placeGovernmentMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(placeGovernmentMutex);

	// Attempts to place the government within one of the current pages.
	Government* government = nullptr;
	for (int page = 0; page < numGovernmentPages; ++page) {
		if (governmentPages[page]->arrCurrGovernment < GOVERNMENT_PAGE_SIZE) {
			government = &governmentPages[page]->governments[governmentPages[page]->arrCurrGovernment];
			++governmentPages[page]->arrCurrGovernment;
			break;

		}
	}

	// If placement failed, creates a new page and places the government there.
	// Will only place when all threads are at this function or returned.
	if (government == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numGovernmentPages + 1 % NUM_GOVERNMENT_PAGES))
			governmentPages = (GovernmentPage**)realloc(governmentPages, (numGovernmentPages + NUM_GOVERNMENT_PAGES) * sizeof(GovernmentPage*));

		// Creates a new GovernmentPage.
		governmentPages[numGovernmentPages] = (GovernmentPage*)calloc(1, sizeof(GovernmentPage));

		// Places the government in the first index of the new page.
		government = &(governmentPages[numGovernmentPages]->governments[0]);
		++governmentPages[numGovernmentPages]->arrCurrGovernment;

		// Moves on to the next page.
		++numGovernmentPages;

	}

	// Returns a pointer to the government.
	return government;

}

/*
Requests a GovernmentPage. Returns the index of the next available page in governmentPages, otherwise -1.
TODO if infrastructure permits it, use a static global to increment through government pages.
*/
int requestGovernmentPage() {
	static std::mutex requestGovernmentPageMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(requestGovernmentPageMutex);

	// Parses through each galaxy and returns the next available one.
	for (int i = 0; i < numGovernmentPages; ++i) {
		if (!governmentPages[i]->active) {
			governmentPages[i]->active = true;
			return i;

		}
	}

	// Returns -1 if no GovernmentPage was found.
	return -1;

}

/*
Returns a GovernmentPage.
*/
void returnGovernmentPage(int index) {
	governmentPages[index]->active = false;

}

// Returns all GovernmentPages.
void returnAllGovernmentPages() {
	for (int i = 0; i < numGovernmentPages; ++i) governmentPages[i]->active = false;

}

/*
Finds the page of a given Government within the paging table.
*/
inline void findGovernmentPage(int& page, Government* government) {

	// Finds the page.
	for (page = 0; page < numGovernmentPages; ++page) {

		// Returns the value if the Government is within bounds.
		if ((char*)government >= (char*)governmentPages[page] &&
			(char*)government < (char*)governmentPages[page] + sizeof(GovernmentPage)) {
			return;

		}
	}
}

/*
Finds the location of a given Government within the paging table.
*/
inline void findGovernmentIndex(int& page, int& index, Government* government) {
	findGovernmentPage(page, government);
	index = ((char*)government - (char*)governmentPages[page]) / sizeof(Government);

}

/*
Marks a Government as being available for reuse.
*/
inline void freeGovernment(Government* government) {
	int page;

	// Marks that there is a free Government in the page.
	findGovernmentPage(page, government);
	++governmentPages[page]->numOpen;

	// Clears the Colony.
	government->~Government();

}

// Undefines constants which are used only for initializing Governments.
#undef NUM_GOVERNMENT_PAGES

/*
Default constructor for a Government. This will not be called when array allocated governments are used.
Initializes everything to null.
*/
Government::Government() {
	memset(this, 0, sizeof(this));

}

/*
Deconstructor for a Government.

TODO check for extant Colonies.
TODO account for inherited unitTables.
TODO currently removing owners is handled in ~Colony
*/
Government::~Government() {

	// Deletes this Government's array of Colonies.
	if (numColonies) delete colonies;

	// Deletes this Government's units.
	delete unitTable;

	// Zeros memory.
	memset(this, 0, sizeof(this));

}

/*
Adds an existing Colony to this Government.
*/
void Government::addColony(Colony* colony) {

	// Array size will be incremented by 8.
	const int inc = 8;

	// If colonies is empty, initializes it.
	if (!numColonies) colonies = (Colony**)malloc(sizeof(Colony*) * inc);
	// If colonies is too small, resizes it.
	else if (!(numColonies % inc)) colonies = (Colony**)realloc(colonies, sizeof(Colony*) * (numColonies + inc));

	// Assigns the Colony to the Government.
	colonies[numColonies] = colony;
	++numColonies;

	// Assigns the Government to the Colony.
	colony->government = this;

}

/*
Removes an existing Colony from this Government.

Assumes that the colony is currently within colonies.
*/
void Government::removeColony(Colony* colony) {

	// Finds the colony.
	int col = 0;
	for (; colonies[col] != colony && col < numColonies; ++col);

	// Removes the colony.
	--numColonies;
	for (int i = col; i < numColonies; ++i) colonies[i] = colonies[i + 1];

	// Deletes colonies if numColonies is 0.
	if (!numColonies) free(colonies);

}

/*
Adds an existing closure to this Government.
*/
void Government::addClosure(uint_least16_t closure) {

	// Array size will be incremented by 8.
	const int inc = 8;

	// If closures is empty, initializes it.
	if (!numClosures) closures = (uint_least16_t*)malloc(sizeof(uint_least16_t) * inc);
	// If closures is too small, resizes it.
	else if (!(numClosures % inc)) closures = (uint_least16_t*)realloc(closures, sizeof(uint_least16_t) * (numClosures + inc));

	// Assigns the closure to the Government.
	closures[numClosures] = closure;
	allClosures[closure].owner = this;
	++numClosures;

}

/*
Removes an existing closure from this Government.

Assumes that the closure is currently within closures.
*/
void Government::removeClosure(uint_least16_t closure) {

	// Finds the closure.
	int clos = 0;
	for (; closures[clos] != closure && clos < numClosures; ++clos);

	// Removes the closure.
	--numClosures;
	for (int i = clos; i < numClosures; ++i) closures[i] = closures[i + 1];

	// Deletes closures if numClosures is 0.
	if (!numClosures) free(closures);

}

/*
Finds space for the Government to expand into.

TODO expand separately for different closures?
*/
void Government::findSpace() {
	int indices[NUM_TARGETS];
	std::vector<Coordinate> coordQueue;
	Coordinate coord;
	int currIndex = 0;
	int xPos, yPos;
	int n;

	// Resets spaceTarget.
	spaceTarget = 0;

	// Acquires a dummy to hold explored tiles.
	uint_least8_t* explored = requestDummy();

	// Acquires a dummy to hold expansions.
	Coordinate* expansions = (Coordinate*)requestLargeDummy();

	// Finds space for expansion in every SpaceClosure.
	for (int c = 0; c < numClosures; ++c) {

		// Finds the starting coordinates of the current closure.
		xPos = allClosures[closures[c]].start.x;
		yPos = allClosures[closures[c]].start.y;

		// Will not expand in this closure if its Galaxy has no available space.
		if (!findGalaxy(xPos, yPos)->unowned) continue;

		// Places the first coord into the queue.
		coordQueue.push_back({ xPos, yPos });

		// Empties explored.
		memset(explored, 0, universeWidth * universeHeight * sizeof(explored[0]));

		// Pathdinds out from the start of the closure looking for available tiles.
		while (!coordQueue.empty()) {

			// Gets the next coord.
			coord = coordQueue.back();
			coordQueue.pop_back();

			// Does nothing if the tile has already been explored.
			if (explored[index(coord.x, coord.y, universeWidth)]) continue;

			// Does nothing if the tile is a ThinSpaceTile.
			if (uIndex(coord.x, coord.y).tileID == THIN_SPACE_TILE) continue;

			// Adds the tile to expansions if it is unowned.
			if (!uOwner(coord.x, coord.y)) {
				expansions[currIndex++] = { coord.x, coord.y };
				continue;

			}

			// Marks the current tile as explored. This is placed here to allow
			// tiles with multiple adjacent owned tiles to be placed in expansions
			// multiple times, therefore increasing their weight.
			explored[index(coord.x, coord.y, universeWidth)] = 1;

			// Does nothing if the tile is owned by another Government.
			if (uOwner(coord.x, coord.y) != this) continue;

			// Adds the adjacent tiles to the queue if it the tile is owned by this government.
			for (int x = -1; x < 2; ++x) {
				for (int y = -1; y < 2; ++y) {
					if (!x && !y) continue;
					coordQueue.push_back({ coord.x + x, coord.y + y });

				}
			}
		}
	}

	// Fills spaceTargets deterministically if there are few targets.
	if (currIndex < NUM_TARGETS * 4) {

		// Fills the array of spaceTargets.
		int i = 0;
		for (; i < currIndex && i < NUM_TARGETS; ++i) spaceTargets[i] = expansions[i];
		for (; i < NUM_TARGETS; ++i) spaceTargets[i] = { 0, 0 };

	}
	// Fills spaceTargets non-deterministically if there are many targets.
	else {

		// Finds an approriate number of bits for randB.
		for (n = 0; n < sizeof(currIndex) * 8; ++n) if ((1 << n) > currIndex) break;

		// Fills the array of spaceTargets.
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
			spaceTargets[i] = expansions[i];

			// Continues checking indices if the new index is invalid.
			exit2:;

		}
	}

	// Releases the dummies.
	releaseDummy(explored);
	releaseLargeDummy((uint_least8_t*)expansions);

}

/*
Requests space to expand into for this Government.
*/
Coordinate Government::requestSpace() {

	// Finds more space if no space is available.
	if (spaceTarget >= NUM_TARGETS || !spaceTargets[spaceTarget]) {
		findSpace();

	}

	// Moves on to the next spaceTarget.
	++spaceTarget;

	// Returns the spaceTarget.
	return spaceTargets[spaceTarget - 1];

}

/*
Manages an Orcish Tribe's expansion on all of its Colonies.
*/
void OrcGroundExpand(Government* tribe) {
	HabitablePlanet* currPlanet;
	Colony* colony;
	int x, y;
	std::shared_mutex* mutex;
	Coord coord;
	uint_least8_t ownerIndex;

	// Expands in all colonies.
	for (int col = 0; col < tribe->numColonies; ++col) {
		colony = tribe->colonies[col];
		ownerIndex = colony->governmentOwner;
		currPlanet = colony->planet;
		mutex = &currPlanet->mutex;

		// Should not concurrently access planet data.
		mutex->lock();

		// Does nothing if there is no remaining land.
		if (!pOwnerBuildingQ(0, LAND_OWNED, currPlanet, 0) && !pOwnerBuildingQ(0, LAND_OWNED, currPlanet, 1)) {
			mutex->unlock();
			continue;

		}

		// Will add at most four adjacent tile to the Orcish tribe. Each addition costs BaseMetals.
		for (int numAdded = 0; numAdded < 4 && colony->market->goods[BaseMetals].quantity > 10 && !!(coord = colony->requestLand()) &&
			!pIndex(coord.x, coord.y, currPlanet).owner; ++numAdded) {

			// Extends the fronts associated with the tile.
			if (colony->planet->battle) colony->planet->battle->extendFronts(coord.x, coord.y, colony);

			// If there is a deposit on the tile, an OrcMine will be built.
			if (currPlanet->planet[index(coord.x, coord.y, currPlanet->size)].deposit) {
				buildingFuncs[OrcMine](currPlanet, colony->market, colony->governmentOwner, coord.x, coord.y, nullptr);
				colony->market->goods[BaseMetals].quantity -= 5;

			}
			// If there is no deposit, there is a small chance that a city can be built on the tile.
			else if (!randB(3)) {
				buildingFuncs[OrcCity](currPlanet, colony->market, colony->governmentOwner, coord.x, coord.y, nullptr);
				colony->market->goods[BaseMetals].quantity -= 10;

				// Attempts to construct a harbour east of the city.
				x = coord.x + 1;
				y = coord.y;
				wrapAroundPlanet(currPlanet->size, &x, &y);
				if (pTileID(x, y, currPlanet) > LAND_TILE) {
					buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)1);
					if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
					goto harbour;

				}

				// Attempts to construct a harbour south of the city.
				x = coord.x;
				y = coord.y + 1;
				wrapAroundPlanet(currPlanet->size, &x, &y);
				if (pTileID(x, y, currPlanet) > LAND_TILE) {
					buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)2);
					if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
					goto harbour;

				}

				// Attempts to construct a harbour west of the city.
				x = coord.x - 1;
				y = coord.y;
				wrapAroundPlanet(currPlanet->size, &x, &y);
				if (pTileID(x, y, currPlanet) > LAND_TILE) {
					buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)3);
					if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
					goto harbour;

				}

				// Attempts to construct a harbour north of the city.
				x = coord.x;
				y = coord.y - 1;
				wrapAroundPlanet(currPlanet->size, &x, &y);
				if (pTileID(x, y, currPlanet) > LAND_TILE) {
					buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)4);
					if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
					goto harbour;

				}

				// Go here to avoid redundant harbour construction.
				harbour:;

			}
			// If nothing else is built on the tile, a farm will be constructed.
			else {
				buildingFuncs[OrcFarm](currPlanet, colony->market, colony->governmentOwner, coord.x, coord.y, nullptr);
				colony->market->goods[BaseMetals].quantity -= 2;

			}
		}

		// If this colony has not got any available tiles to expand but there are still unnocupied tiles,
		// attempts to create an overseas city. Settlement costs 20 BaseMetals.
		if (!coord && pOwnerBuildingQ(0, LAND_OWNED, currPlanet, 0) && pOwnerBuildingQ(0, LAND_OWNED, currPlanet, 1) &&
			colony->market->goods[BaseMetals].quantity > 20) {
			for (int i = 0; i < currPlanet->size; ++i) {
				for (int j = 0; j < currPlanet->size; ++j) {

					// Has a 1 / 256 chance to settle any unowned land tile adjacent to the coast.
					if (!pIndex(i, j, currPlanet).owner && pIndex(i, j, currPlanet).tileData <= LAND_TILE
						&& currPlanet->checkAdjacency(i, j, [](int x, int y, PlanetTile tile) {return tile.tileData > LAND_TILE; })
						&& !randB(8)) {

						// Extends the fronts if there is a city present.
						if (colony->planet->battle) colony->planet->battle->extendFronts(i, j, colony);

						// Builds the Orcish city.
						buildingFuncs[OrcCity](currPlanet, colony->market, colony->governmentOwner, i, j, nullptr);

						// Attempts to construct a harbour east of the city.
						x = coord.x + 1;
						y = coord.y;
						wrapAroundPlanet(currPlanet->size, &x, &y);
						if (pTileID(x, y, currPlanet) > LAND_TILE) {
							buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)1);
							if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
							goto exit2;

						}

						// Attempts to construct a harbour south of the city.
						x = coord.x;
						y = coord.y + 1;
						wrapAroundPlanet(currPlanet->size, &x, &y);
						if (pTileID(x, y, currPlanet) > LAND_TILE) {
							buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)2);
							if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
							goto exit2;

						}

						// Attempts to construct a harbour west of the city.
						x = coord.x - 1;
						y = coord.y;
						wrapAroundPlanet(currPlanet->size, &x, &y);
						if (pTileID(x, y, currPlanet) > LAND_TILE) {
							buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)3);
							if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
							goto exit2;

						}

						// Attempts to construct a harbour north of the city.
						x = coord.x;
						y = coord.y - 1;
						wrapAroundPlanet(currPlanet->size, &x, &y);
						if (pTileID(x, y, currPlanet) > LAND_TILE) {
							buildingFuncs[Harbour1](currPlanet, colony->market, colony->governmentOwner, x, y, (void*)4);
							if (colony->planet->battle) colony->planet->battle->extendFronts(x, y, colony);
							goto exit2;

						}

						// Exits the loop.
						goto exit2;

					}
				}
			}
		}

		// Exit for the above loop.
		exit2:;

		// Unlocks the planet's mutex.
		mutex->unlock();

	}
}

/*
Manages an Orcish Tribe's production on all of its Colonies.

TODO DEBUG REMOVE this may eventually be replaced if resource acquisition should not occur during battle.

TODO DEBUG REMOVE collection rates are differing when in battle vs. when not in battle.
*/
void OrcProduce(Government* tribe) {
	HabitablePlanet* planet;
	Colony* colony;
	Battle* battle;
	std::shared_mutex* mutex;
	int owner;

	// Manages production in all Colonies.
	for (int col = 0; col < tribe->numColonies; ++col) {
		colony = tribe->colonies[col];
		planet = colony->planet;
		mutex = &planet->mutex;

		// Should not concurrently access planet data.
		mutex->lock();

		// If there is a Battle, manages production using Battle owners.
		battle = colony->planet->battle;
		if (battle) {

			// Finds the GroundOwner that corresponds to the desire owner.
			for (owner = 0; owner < battle->comp.byte1; ++owner)
				if (battle->owners[owner].owner == colony) break;

			// Adds resources for all tiles owned by this tribe.
			for (int i = 0; i < planet->size * planet->size; ++i) {
				if (battle->battlefield[i].owner == owner) {
					
					// Adds veg for OrcFarms.
					if (planet->planet[i].buildingID == OrcFarm) ++colony->market->foods[0].vegQuantity += 4;
					// Adds BaseMetals for OrcMines.
					else if (planet->planet[i].buildingID == OrcMine) ++colony->market->goods[BaseMetals].quantity += 4;

				}
			}
		}
		// If there is no Battle, manages production using Colony owners.
		else {
			owner = colony->governmentOwner;

			// Adds resources for all tiles owned by this tribe.
			for (int i = 0; i < planet->size * planet->size; ++i) {
				if (planet->planet[i].owner == owner) {

					// Adds veg for OrcFarms.
					if (planet->planet[i].buildingID == OrcFarm) colony->market->foods[0].vegQuantity += 2;
					// Adds BaseMetals for OrcMines.
					else if (planet->planet[i].buildingID == OrcMine) ++colony->market->goods[BaseMetals].quantity += 1;

				}
			}
		}

		// Unlocks the planet's mutex.
		mutex->unlock();

	}
}

/*
Manages an Orcish Tribe's reinforcement on all of its Colonies.
*/
void OrcReinforce(Government* tribe) {
	Colony* colony;
	Battle* battle;
	std::shared_mutex* mutex;
	int numReinforcements;
	int vegUsed;

	// Manages reinforcement in all Colonies.
	for (int col = 0; col < tribe->numColonies; ++col) {
		colony = tribe->colonies[col];
		mutex = &colony->planet->mutex;

		// Should not concurrently access planet data.
		mutex->lock();

		// Does nothing if there is no ongoing Battle on the current planet.
		battle = colony->planet->battle;
		if (!battle) {
			mutex->unlock();
			continue;

		}

		// Determines the amount of units that can be bought.
		numReinforcements = colony->market->goods[BaseMetals].quantity < colony->market->foods[0].vegQuantity + colony->market->foods[0].meatQuantity
			? colony->market->goods[BaseMetals].quantity : colony->market->foods[0].vegQuantity + colony->market->foods[0].meatQuantity;

		// Spends resources to create units.
		colony->market->goods[BaseMetals].quantity -= numReinforcements;

		// Splits food spending between meat and veg.
		vegUsed = numReinforcements < colony->market->foods[0].vegQuantity ? numReinforcements : colony->market->foods[0].vegQuantity;
		colony->market->foods[0].vegQuantity -= vegUsed;
		colony->market->foods[0].meatQuantity -= numReinforcements - vegUsed;

		// Reinforces the battle.
		battle->reinforce(colony, numReinforcements, 1);

		// Unlocks the planet's mutex.
		mutex->unlock();

	}
}

/*
Manages capitulation of an Orcish Tribe when it is totally conquered.
*/
void OrcCapitulate(Government* tribe) {
	Colony* colony;
	Battle* battle;
	HabitablePlanet* planet;
	Government* gov = nullptr;
	System* tile;
	std::shared_mutex* mutex;
	int owner;
	int num;

	// Manages capitulation on all Colonies.
	for (int col = 0; col < tribe->numColonies; ++col) {
		colony = tribe->colonies[col];
		planet = colony->planet;
		mutex = &planet->mutex;

		// Should not concurrently access planet data.
		mutex->lock();

		// Does nothing if there is no ongoing Battle on the current planet.
		battle = planet->battle;
		if (!battle) {
			mutex->unlock();
			continue;

		}

		// Attempts to find a tile that belongs to the current Colony.
		owner = battle->getOwner(colony);
		for (int i = 0; i < battle->planet->size * battle->planet->size; ++i)
			if (battle->battlefield[i].owner == owner)
				goto end;

		// Transfers ownership of this Colony's tiles to the other Battle participants.
		for (int x = 0; x < planet->size; ++x) {
			for (int y = 0; y < planet->size; ++y) {
				if (poIndex(x, y, planet) == colony->governmentOwner) {
					owner = battle->owners[battle->battlefield[index(x, y, planet->size)].owner].owner->governmentOwner;
					buildingFuncs[pBuilding(x, y, planet)](planet, colony->market, owner, x, y, nullptr);

				}
			}
		}

		// Frees the Colony.
		freeColony(colony);

		// Frees the Government
		if (!tribe->numColonies) freeGovernment(tribe);

		// Finds the number of remaining Governments.
		// Also tracks the final Government.
		num = 0;
		for (int i = 1; i < NUM_HABITABLE_OWNERS; ++i) {
			if (planet->owners[i].colony) {
				gov = planet->owners[i].colony->government;
				++num;

			}
		}

		// If one tribe remains, finishes the Battle and allows the tribe to enter space.
		if (num == 1) {
			delete battle;
			changeUniverseOwner(planet->location, gov);

		}

		// Jump for the no capitulation case.
		end:;

		// Unlocks the planet's mutex.
		mutex->unlock();

	}
}

/*
Manages shipbuilding for an Orcish tribe.
*/
void OrcShipbuilding(Government* tribe) {
	Colony* colony;
	HabitablePlanet* planet;
	GalaxyTile* tile = nullptr;
	std::shared_mutex* mutex;

	// Manages shipbuilding on all Colonies.
	for (int col = 0; col < tribe->numColonies; ++col) {
		colony = tribe->colonies[col];
		planet = colony->planet;
		mutex = &planet->mutex;

		// Should not concurrently access planet data.
		mutex->lock();

		// Will perform no action if there is an ongoing Battle.
		if (planet->battle) {
			mutex->unlock();
			continue;

		}

		// Places ships in the tile if it is owned by the tribe.
		if (uOwner(planet->location.x, planet->location.y) == tribe) {
			tile = &uIndex(planet->location.x, planet->location.y);

			// Places a Ship in an existing Fleet if one exists.
			if (tile->fleets && tile->fleets[0]) ++tile->fleets[0]->ships[0].num;
			// Places a Ship in a new Fleet if no Fleet exists.
			else {
				Fleet* fleet = (Fleet*)calloc(1, sizeof(Fleet));
				fleet->addShip(&tribe->shipTable[0], 1);
				tile->addFleet(fleet);

			}
		}

		// Unlocks the planet's mutex.
		mutex->unlock();

	}
}

/*
Manages an Orcish Tribe's expansion in space.
*/
void OrcSpaceExpand(Government* tribe) {
	Coordinate coord;
	int inc = 0;

	// Expands to a handful of tiles.
	while (!!(coord = tribe->requestSpace()) && !uOwner(coord.x, coord.y) && inc++ < 8 && !randB(3)) {
		changeUniverseOwner(coord.x, coord.y, tribe);

	}
}