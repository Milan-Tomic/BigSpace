#pragma once

// Generates empires within each galaxy.
void generateEmpiresUniverse(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Generates empires within each galaxy in a multithreaded manner.
void generateEmpiresUniverseThreaded(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Generates empries within a single galaxy.
void generateEmpiresGalaxy(const Galaxy* galaxy, const GalaxyInstruction* instr);

// Estimates the appropriate number of Markets.
inline int estimateNumMarkets(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Estimates the appropriate number of RaceTemplates.
inline int estimateNumRaces(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Estimates the appropriate number of Colonies.
inline int estimateNumColonies(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Estimates the appropriate number of Governments.
inline int estimateNumGovernments(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies);

// Generates appropriate RaceTemplates for an empire.
void generateRaces(Colony* colony);

/*
Generates empires within each galaxy.

generateEmpiresUniverseThreaded is called once no matter what. Therefore, since generateEmpiresUnvierseThreaded
is recursive and can cover all galaxies with a single thread, a single thread can execute this function.
*/
void generateEmpiresUniverse(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {

	// Allocates Markets.
	initMarkets(estimateNumMarkets(galaxies, instrs, numGalaxies));

	// Allocates RaceItems.
	initRaces(estimateNumRaces(galaxies, instrs, numGalaxies));

	// Allocates Colonies.
	initColonies(estimateNumColonies(galaxies, instrs, numGalaxies));

	// Allocates Governments.
	initGovernments(estimateNumGovernments(galaxies, instrs, numGalaxies));

	// TODO DEBUG REMOVE
	// Places colonies in all systems.
	for (int i = 0; i < numGalaxies; ++i) instrs[i].empireChance = 1001;

	// Generates empires in a multi threaded manner.
	for (int i = 0; i < numThreads - 1 && i < numGalaxies - 1; ++i) threads[demandThread()] = new std::thread(generateEmpiresUniverseThreaded, galaxies, instrs, numGalaxies);
	generateEmpiresUniverseThreaded(galaxies, instrs, numGalaxies);

	// Joins all threads.
	joinAllThreads();

	// Reclaims the galaxies.
	releaseAllGalaxies();

	// TODO DEBUG REMOVE Counts inhabited planets.
	int numInhabited = 0;
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				System* tile = uSystem(i, j);
				for (int p = 0; p < tile->numHabitable; ++p) {
					if (tile->planets[p]->owners[1].colony) ++numInhabited;

				}
			}
		}
	}
	printf("numInhabited : %d\n", numInhabited);

}

/*
Generates empires within each galaxy in a multithreaded manner.

This function will be completed even if only one thread is used for it.
*/
void generateEmpiresUniverseThreaded(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {

	// If there is an available galaxy, generates empires within it.
	for (int galaxy = requestGalaxy(); galaxy >= 0; galaxy = requestGalaxy())
		generateEmpiresGalaxy(&galaxies[galaxy], &instrs[galaxy]);

}

/*
Generates and returns a new Government.
*/
Government* generateGovernment() {
	Government* newGovernment = placeGovernment();

	// Generates the flag colour for this government.
	newGovernment->flagColour = {(uint_least8_t)randB(8), (uint_least8_t)randB(8), (uint_least8_t)randB(8), 200};

	// TODO DEBUG REMOVE
	// Adds Orcish behaviour to the Government.
	newGovernment->behaviours[0] = OrcGroundExpand;
	newGovernment->behaviours[1] = OrcReinforce;
	newGovernment->behaviours[2] = OrcProduce;
	newGovernment->behaviours[3] = OrcCapitulate;
	newGovernment->behaviours[4] = OrcShipbuilding;
	newGovernment->behaviours[5] = OrcSpaceExpand;

	// TODO TEMP
	// Adds a simple list of unit templates to the Government.
	newGovernment->unitTable = new GroundUnitTemplate[3]();
	newGovernment->unitTable[1] = {0, 1, BallisticDamage, 1, 0};
	newGovernment->unitTable[2] = {4, 2, BallisticDamage, 1, 16};
	groundUnitTemplateStrength(&newGovernment->unitTable[1]);
	groundUnitTemplateStrength(&newGovernment->unitTable[2]);
	*((int*)newGovernment->unitTable) = 2; // Assigns the number of units to two.

	// TODO TEMP
	// Adds a simple list of ship templates to the Government.
	newGovernment->shipTable = new ShipTemplate[1]();
	newGovernment->shipTable[0] = {{1, 0, 0, 0}, {0, 0, 0, 0}, 0, 1, 1, 0};

	// TODO add ethics, parliament, etc.

	// Returns the generated Government.
	return newGovernment;

}

/*
Generates the inital colony for a race.
// TODO add buildings based on ethics.
*/
void generateInitialColony(HabitablePlanet* planet, Government* government) {

	// Creates the Colony.
	Colony* colony = placeColony();

	// Gets the owner for the Government.
	uint_least8_t owner = planet->nextOwner();

	// Assigns the planet to the Colony.
	colony->planet = planet;
	colony->governmentOwner = owner;

	// Creates the Colony's market.
	placeMarket()->addColony(colony);

	// Assigns the Colony to the Government.
	government->addColony(colony);

	// Creates an owner corresponding to the planet's Government.
	planet->owners[owner].owner = government;
	planet->owners[owner].colony = colony;

}

/*
Generates housing nearby buildings.
// TODO refactor or remove.
*/
void generateNearbyHousing(HabitablePlanet* planet, int xPos, int yPos) {
	int xFin;
	int yFin;
	int length;

	// Generates the housing a random distance away.
	for (int i = 0; i < 100; ++i) {
		xFin = xPos + randB(3) % 5 - 2;
		yFin = yPos + randB(3) % 5 - 2;
		wrapAroundPlanet(planet->size, &xFin, &yFin);
		if (planet->planet[index(xFin, yFin, planet->size)].tileData <= Mountain && !planet->planet[index(xFin, yFin, planet->size)].buildingID) {
			buildingFuncs[City1](planet, planet->owners[1].colony->market, 1, xFin, yFin, nullptr);

			// Pathfinds across the planet and draws roads.
			planet->planetPathfind(xPos, yPos, 20,

				// endCondition lambda. Ends when the housing is reached.
				[xFin, yFin] (HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->bool {
					return currPos->value->coord.x == xFin && currPos->value->coord.y == yFin;

				},

				// action lambda. Draws roads along the path.
				[] (HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos) {
					Direction prevDir;

					// Draws the last road.
					planet->planet[index(currPos->value->coord.x, currPos->value->coord.y, planet->size)].wayDir.i |= getOppositeDir(currPos->value->dir).i;
					planet->planet[index(currPos->value->coord.x, currPos->value->coord.y, planet->size)].wayLevel = 1;

					// Draws the roads.
					for (; currPos->previous != nullptr; currPos = currPos->previous) {
						planet->planet[index(currPos->previous->value->coord.x, currPos->previous->value->coord.y, planet->size)].wayDir.i |=
							currPos->value->dir.i;
						planet->planet[index(currPos->value->coord.x, currPos->value->coord.y, planet->size)].wayDir.i |=
							getOppositeDir(currPos->value->dir).i;
						planet->planet[index(currPos->previous->value->coord.x, currPos->previous->value->coord.y, planet->size)].wayLevel = 1;

					}
				},

				// calcG lambda. gValue change is always one unless moving over water.
				[] (HabitablePlanet* planet, int xBeg, int yBeg, int xDest, int yDest)->int {
					return 256 * (pTileID(xBeg, yBeg, planet) > LAND_TILE) + 1;

				},

				// calcH lambda. hValue is the Euclidean distance between this tile and the destination.
				// Does not go over poles.
				[xFin, yFin] (HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->double {
					int xDist, yDist;

					// Determines x distance.
					// Normal.
					xDist = abs(xFin - currPos->value->coord.x);
					// Over right edge.
					if (planet->size - currPos->value->coord.x + xFin < xDist)
						xDist = planet->size - currPos->value->coord.x + xFin;
					// Over left edge.
					if (planet->size - xFin + currPos->value->coord.x < xDist)
						xDist = planet->size - xFin + currPos->value->coord.x;

					// Determines y distance.
					yDist = currPos->value->coord.y - yFin;

					// Returns the Euclidean distance between the two points.
					return xDist || yDist ? sqrt(pow(xDist, 2) + pow(yDist, 2)) : 0;

				}
			);

			// Returns after housing is placed.
			return;

		}
	}
}

/*
Generates the initial buildings for the colony.
// TODO make buildings dependent upon ethics.
*/
void generateInitialBuildings(HabitablePlanet* planet) {

	// TEMP generates mines on every deposit.
	for (int i = 0; i < NUM_DEPOSITS_SURFACE && planet->surfaceDeposits[i].good != 255; ++i) {

		// Generates mines on every mineable tile.
		// TEMP mines are level one and belong to the government.
		for (int j = 0; j < NUM_DEPOSIT_COORDS && planet->surfaceDeposits[i].xPos[j] != 255; ++j) {

			// Creates a mine on the deposit.
			buildingFuncs[Mine1](planet, planet->owners[1].colony->market, 1, planet->surfaceDeposits[i].xPos[j], planet->surfaceDeposits[i].yPos[j], nullptr);

			// Generates nearby housing for each inital mine.
			generateNearbyHousing(planet, planet->surfaceDeposits[i].xPos[j], planet->surfaceDeposits[i].yPos[j]);

		}
	}
}

/*
Generates a handful of Buildings surrounding a deposit on the planet.
*/
void generateOrcBuildings(HabitablePlanet* planet, int deposit, int owner) {
	Colony* colony;
	int xPos, yPos;
	int xFin, yFin;
	int x, y;

	// Generates mines on every mineable tile.
	// Mines are level one and belong to the Orcish government.
	for (int j = 0; j < NUM_DEPOSIT_COORDS && planet->surfaceDeposits[deposit].xPos[j] != 255; ++j) {
		colony = planet->owners[owner].colony;

		// Creates a mine on the deposit.
		buildingFuncs[OrcMine](planet, colony->market, owner, planet->surfaceDeposits[deposit].xPos[j], planet->surfaceDeposits[deposit].yPos[j], nullptr);

		// Generates nearby housing for each inital mine.
		xPos = planet->surfaceDeposits[deposit].xPos[j];
		yPos = planet->surfaceDeposits[deposit].yPos[j];
		for (int i = 0; i < 100; ++i) {
			xFin = xPos + randB(3) % 5 - 2;
			yFin = yPos + randB(3) % 5 - 2;
			wrapAroundPlanet(planet->size, &xFin, &yFin);
			if (planet->planet[index(xFin, yFin, planet->size)].tileData <= LAND_TILE && !planet->planet[index(xFin, yFin, planet->size)].owner) {
				buildingFuncs[OrcCity](planet, colony->market, owner, xFin, yFin, nullptr);

				// Attempts to construct a harbour east of the city.
				x = xFin + 1;
				y = yFin;
				wrapAroundPlanet(planet->size, &x, &y);
				if (pTileID(x, y, planet) > LAND_TILE) {
					buildingFuncs[Harbour1](planet, colony->market, colony->governmentOwner, x, y, (void*)1);
					break;

				}

				// Attempts to construct a harbour south of the city.
				x = xFin;
				y = yFin + 1;
				wrapAroundPlanet(planet->size, &x, &y);
				if (pTileID(x, y, planet) > LAND_TILE) {
					buildingFuncs[Harbour1](planet, colony->market, colony->governmentOwner, x, y, (void*)2);
					break;

				}

				// Attempts to construct a harbour west of the city.
				x = xFin - 1;
				y = yFin;
				wrapAroundPlanet(planet->size, &x, &y);
				if (pTileID(x, y, planet) > LAND_TILE) {
					buildingFuncs[Harbour1](planet, colony->market, colony->governmentOwner, x, y, (void*)3);
					break;

				}

				// Attempts to construct a harbour north of the city.
				x = xFin;
				y = yFin - 1;
				wrapAroundPlanet(planet->size, &x, &y);
				if (pTileID(x, y, planet) > LAND_TILE) {
					buildingFuncs[Harbour1](planet, colony->market, colony->governmentOwner, x, y, (void*)4);
					break;

				}

				// Breaks.
				break;

			}
		}
	}
}

/*
Generates a battlefield to accompany the Orcish colony.
*/
void generateOrcBattlefield(HabitablePlanet* planet) {
	 new Battle(planet); // Note: the battle is assigned to the planet within the constructor.

}

/*
Generates empires within a single galaxy.
// TODO currently only generates colonies
*/
void generateEmpiresGalaxy(const Galaxy* galaxy, const GalaxyInstruction* instr) {
	System* system;
	HabitablePlanet* planet;
	Government* government;

	// Generates empires on appropriate planets.
	for (int i = galaxy->area.x; i < galaxy->area.x + galaxy->area.w; ++i) {
		for (int j = galaxy->area.y; j < galaxy->area.y + galaxy->area.h; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE && randB(10) % 1000 < instr->empireChance) {
				system = uSystem(i, j);

				// Chooses a planet to place the race on.
				planet = system->planets[randB(4) % system->numHabitable];

				// Generates a colony for every deposit.
				for (int currDeposit = 0; currDeposit < NUM_DEPOSITS_SURFACE && planet->surfaceDeposits[currDeposit].good != 255; ++currDeposit) {

					// Generates a newGovernment.
					government = generateGovernment();

					// Generates an inital colony.
					generateInitialColony(planet, government);

					// Generates the initial buildings for the colony.
					// TODO DEBUG REMOVE
					// generateInitialBuildings(planet);
					generateOrcBuildings(planet, currDeposit, government->colonies[0]->governmentOwner);

					// Generates the initial races for the colony.
					// TODO change colonies
					//generateRaces(government->colonies[0]);
					// TODO TEMP Gives the Colony some Orcs.
					government->colonies[0]->newPops(getOrcs(), 200);

				}

				// Generates the initial battlefield for the Orcish colonies.
				generateOrcBattlefield(planet);

			}
		}
	}
}

/*
Estimates the appropriate number of Markets.
TODO create a real Market estimation system.
*/
inline int estimateNumMarkets(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {
	return 0;

}

/*
Estimates the appropriate number of RaceTemplates to allocate. Note that empireChance is in thousandths.
// TODO make racesPerEmpire an input.
*/
inline int estimateNumRaces(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {
	int numRaces = 0;
	int numSystems;
	int racesPerEmpire = 32;

	// For each galaxy, estimates the needed number of races.
	for (int gal = 0; gal < numGalaxies; ++gal) {
		numSystems = 0;
		for (int w = galaxies[gal].area.x; w < galaxies[gal].area.x + galaxies[gal].area.w; ++w) {
			for (int h = galaxies[gal].area.y; h < galaxies[gal].area.y + galaxies[gal].area.h; ++h) {
				if (uIndex(w, h).tileID == SYSTEM_TILE) ++numSystems;

			}
		}

		// Estimates the needed number of races for each galaxy.
		numRaces += ((numSystems * instrs[gal].empireChance) / 1000) * racesPerEmpire + 20;

	}

	// Returns the estimated number of races.
	return numRaces;

}

/*
Estimates the appropriate number of Colonies.
TODO create a real Colony estimation system.
*/
inline int estimateNumColonies(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {
	return /*numHabitablePlanets * 20*/0;

}

/*
Estimates the appropriate number of Governments.
TODO create a real Government estimation system.
*/
inline int estimateNumGovernments(Galaxy* galaxies, GalaxyInstruction* instrs, int numGalaxies) {
	return /*numHabitablePlanets * 20*/0;

}

/*
Generates an appropriate handful of races for the given empire.
// TODO more varied gas consumption.
// TODO more varied diet.
// TODO more varied productivities.
// TODO produce child races.
// TODO place varied number of initial pops.
// TODO create real race name
*/
void generateRaces(Colony* colony) {
	RaceTemplate* race = placeRace();

	// Generates a random string name.
	race->name = (std::string)std::to_string(randB(16));

	// Race should consume methane if on a methane rich world, otherwise oxygen.
	if (colony->planet->gases[Oxygen] > colony->planet->gases[Methane]) race->consumedGases[Oxygen] = 1;
	else race->consumedGases[Methane] = 1;

	// Race should output atmospheric pollutants equivalent to their inhaled gases.
	race->producedGases[Pollutants] = race->consumedGases[Methane] + race->consumedGases[Oxygen];

	// Race should consume one unit of vegetation, one unit of meat.
	race->consumed[Veg] = 1;
	race->consumed[Meat] = 1;

	// Race will have all productivities set to 1.
	// TODO change.
	for (int i = 0; i < NUM_GOODS_ALL; ++i) {
		race->productitivities[i] = 1;

	}

	// Race will have all consumptions set to 0.
	// TODO change.

	// Places 200 pops of the generated race into the colony.
	colony->newPops(race, 200);

}