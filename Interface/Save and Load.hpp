#pragma once

// Saves a file describing the current play session.
void saveFile(std::string fileName);

// Saves metadata for the game.
void saveMetaData(std::ofstream * saveFile);

// Saves the current universe to the file describing the current play session.
void saveUniverseTiles(std::ofstream* saveFile);

// Saves the systems of the universe to the saveFile.
void saveSystemTiles(std::ofstream* saveFile);

// Saves the metadata of the universe's stars to the saveFile.
void saveStars(std::ofstream* saveFile);

// Saves the metadata of the universe's planets to the saveFile.
void savePlanets(std::ofstream* saveFile);

// Saves the metadata of the universe's barrens to the saveFile.
void saveBarrens(std::ofstream * saveFile);

// Saves the planetTiles of each planet.
void savePlanetTiles(std::ofstream* saveFile);

// Saves the Rivers of each planet.
void saveRivers(std::ofstream* saveFile);

// Saves the Deposits of each planet.
void saveDeposits(std::ofstream* saveFile);

// Saves all RaceTemplates.
void saveRaces(std::ofstream* saveFile);

// Saves all Markets.
void saveMarkets(std::ofstream* saveFile);

// Saves all Colonies.
void saveColonies(std::ofstream* saveFile);

// Saves all Governments.
void saveGovernments(std::ofstream* saveFile);

// Saves all HabitablePlanet Owners.
void saveHabitableOwners(std::ofstream* saveFile);

// Saves all BarrenPlanet Owners.
void saveBarrenOwners(std::ofstream* saveFile);

// Saves the Battles of each planet.
void saveBattles(std::ofstream* saveFile);

// Loads a file describing a play session.
void loadFile(std::string fileName);

// Loads metadata for the game.
void loadMetaData(std::ifstream* saveFile);

// Loads a universe contained in the file describing a play session.
void loadUniverseTiles(std::ifstream* saveFile);

// Loads the systems of the saveFile into the universe.
void loadSystemTiles(std::ifstream* saveFile);

// Loads the metadata of the saveFile's stars into the universe.
void loadStars(std::ifstream* saveFile);

// Loads the metadata of the saveFiles planets into the universe.
void loadPlanets(std::ifstream* saveFile);

// Loads the metadata of the saveFile's barrens into the universe.
void loadBarrens(std::ifstream* saveFile);

// Loads the planetTiles of each planet.
void loadPlanetTiles(std::ifstream* saveFile);

// Loads the Rivers of each planet.
void loadRivers(std::ifstream* saveFile);

// Loads the Deposits of each planet.
void loadDeposits(std::ifstream* saveFile);

// Loads all RaceTemplates.
void loadRaces(std::ifstream* saveFile);

// Loads all Markets.
void loadMarkets(std::ifstream* saveFile);

// Loads all Colonies.
void loadColonies(std::ifstream* saveFile);

// Loads all Governments.
void loadGovernments(std::ifstream* saveFile);

// Loads all HabitablePlanet Owners.
void loadHabitableOwners(std::ifstream* saveFile);

// Loads all BarrenPlanet Owners.
void loadBarrenOwners(std::ifstream* saveFile);

// Loads the Battles of each planet.
void loadBattles(std::ifstream* saveFile);

// Initializes the arrays of habitables and barrens while loading.
void loadInitPlanets();

// Confirms that a token is found in the next n characters. Assumes that the
// token is preceded and followed by newline chars and ignores those.
// Crashes if the correct token is not found.
void confirmToken(std::ifstream* saveFile, const char desired[]);

/*
Saves a file describing the current play session.
1. Creates a file to save in.
2.
3. Saves the universe by calling saveUniverseTiles.
TODO cont
*/
void saveFile(std::string fileName) {

	// Creates the fileLocation string.
	std::string fileLocation("Save Files/");
	fileLocation.append(fileName.c_str());
	fileLocation.append(".txt");

	// Opens a file of the inputed fileName.
	std::ofstream saveFile{};
	saveFile.open(fileLocation, std::ios::binary);

	// Saves metadata for the game.
	saveMetaData(&saveFile);

	// Saves the universe's tiles.
	saveUniverseTiles(&saveFile);

	// Saves the systems' metadata.
	saveSystemTiles(&saveFile);

	// Saves the stars' metadata.
	saveStars(&saveFile);

	// Saves the planets' metadata.
	savePlanets(&saveFile);

	// Saves the barrens' metadata.
	saveBarrens(&saveFile);

	// Saves the planets' tiles and buildings.
	savePlanetTiles(&saveFile);

	// Saves the planets' rivers.
	saveRivers(&saveFile);

	// Saves the planets' deposits.
	saveDeposits(&saveFile);

	// Saves all RaceTemplates.
	saveRaces(&saveFile);

	// Saves all Markets.
	saveMarkets(&saveFile);

	// Saves all Colonies.
	saveColonies(&saveFile);

	// Saves all Governments.
	saveGovernments(&saveFile);

	// Saves all HabitablePlanet Owners.
	saveHabitableOwners(&saveFile);

	// Saves all BarrenPlanet Owners.
	saveBarrenOwners(&saveFile);

	// Saves the battle's of each planet.
	saveBattles(&saveFile);

	// Close the file and return true.
	saveFile.close();

}

/*
Saves metadata for the game.
Saved in the form {dummySize:largeDummySize}
*/
void saveMetaData(std::ofstream* saveFile) {
	std::string meta;
	int dummySize;

	// Places an extra newline for confirmToken.
	meta += '\n';

	// Starts with the designation meta.
	meta += "meta\n";

	// Saves dummySize.
	meta.append((char*)((int*)threadDummies - 1), sizeof(int));

	// Saves largeDummySize.
	meta.append((char*)((int*)threadLargeDummies - 1), sizeof(int));

	// Closes with a newline.
	meta += '\n';

	// Saves the metadata tiles.
	*saveFile << meta;

}

/*
Saves the tiles of a given universe to the saveFile.
Saved in the form {tileID}.
*/
void saveUniverseTiles(std::ofstream* saveFile) {
	std::string tiles;

	// Starts with the designation utiles.
	tiles += "utiles\n";

	// Saves universeWidth and universeHeight.
	tiles.append((char*)&universeWidth, sizeof(universeWidth));
	tiles.append((char*)&universeHeight, sizeof(universeHeight));
	tiles += '\n';

	// Parses through the universe and places every tile in the file.
	for (int i = 0; i < universeWidth; ++i) {

		// Saves each row of tiles.
		for (int j = 0; j < universeHeight; ++j)
			tiles.append((char*)&uIndex(i, j).tileID, sizeof(uIndex(i, j).tileID));

		// If tiles is large, empties it.
		if (tiles.length() > 3600) {
			*saveFile << tiles;
			tiles.clear();

		}
	}

	// Closes with a newline.
	tiles += '\n';

	// Saves the remaining universe tiles.
	*saveFile << tiles;

}

/*
Saves the systems of the universe to the saveFile.
Saves them in the form {numStars:numHabitable:numBarren}.
*/
void saveSystemTiles(std::ofstream* saveFile) {
	std::string tiles;
	System* tile;

	// Starts with the designation systems.
	tiles += "systems\n";

	// Saves the metadata of each system to the saveFile.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {

			// Saves a system's metadata.
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Saves numStars, numHabitable, and numBarren.
				tiles.append((char*)&tile->numStars, sizeof(tile->numStars));
				tiles.append((char*)&tile->numHabitable, sizeof(tile->numHabitable));
				tiles.append((char*)&tile->numBarren, sizeof(tile->numBarren));

				// If tiles is large, empties it.
				if (tiles.length() > 3600) {
					*saveFile << tiles;
					tiles.clear();

				}
			}
		}
	}

	// Closes with a newline.
	tiles += '\n';

	// Saves the system tiles.
	*saveFile << tiles;

}

/*
Saves the metadata of the universe's stars to the saveFile.
Saves them in the form {size:starID:luminosity}.
*/
void saveStars(std::ofstream* saveFile) {
	std::string stars;
	System* tile;

	// Starts with the designation stars.
	stars += "stars\n";

	// Saves the metadata of each star to the saveFile.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Saves the metadata of each star to the saveFile.
				for (int s = 0; s < tile->numStars; ++s) {
					stars.append((char*)&tile->stars[s].size, sizeof(tile->stars[s].size));
					stars.append((char*)&tile->stars[s].tileID, sizeof(tile->stars[s].tileID));
					stars.append((char*)&tile->stars[s].luminosity, sizeof(tile->stars[s].luminosity));

					// If stars is large, empties it.
					if (stars.length() > 3600) {
						*saveFile << stars;
						stars.clear();

					}
				}
			}
		}
	}

	// Closes with a newline.
	stars += '\n';

	// Saves the stars.
	*saveFile << stars;

}

/*
Saves the metadata of the universe's planets to the saveFile.
Saved in the form {size:distance}.

Note: heatMultiple and temperature will be calculated from the save data upon loading.
*/
void savePlanets(std::ofstream* saveFile) {
	std::string planets;
	System* tile;

	// Starts with the designation planets.
	planets += "planets\n";

	// Saves the metadata of each planet to the saveFile.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Saves the metadata of each planet.
				for (int p = 0; p < tile->numHabitable; ++p) {
					planets.append((char*)&tile->planets[p]->size, sizeof(tile->planets[p]->size));
					planets.append((char*)&tile->planets[p]->distance, sizeof(tile->planets[p]->distance));

					// If planets is large, empties it.
					if (planets.length() > 3600) {
						*saveFile << planets;
						planets.clear();

					}
				}
			}
		}
	}

	// Closes with a newline.
	planets += "\n";

	// Saves the planets.
	*saveFile << planets;

}

/*
Saves the metadata of the universe's barrnes to the saveFile.
Saved in the form {size:distance}.

Note: heatMultiple and temperature will be calculated form the save data upon loading.
*/
void saveBarrens(std::ofstream* saveFile) {
	std::string barrens;
	System* tile;

	// Starts with the designation barrens.
	barrens += "barrens\n";

	// Saves the metadata of each barren to the saveFile.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Saves the metadata of each planet.
				for (int p = 0; p < tile->numBarren; ++p) {
					barrens.append((char*)&tile->barrens[p]->size, sizeof(tile->barrens[p]->size));
					barrens.append((char*)&tile->barrens[p]->distance, sizeof(tile->barrens[p]->distance));

					// If barrens is large, empties it.
					if (barrens.length() > 3600) {
						*saveFile << barrens;
						barrens.clear();

					}
				}
			}
		}
	}

	// Closes with a newline.
	barrens += "\n";

	// Saves the barrens.
	*saveFile << barrens;

}

/*
Saves the planetTiles of each planet.
Saved in the form {tileData:wayDir:wayLevel:buildingOwner:buildingID:buildingData}
*/
void savePlanetTiles(std::ofstream* saveFile) {
	std::string tiles;
	System* tile;

	// Starts with the designation planetTiles.
	tiles += "planetTiles\n";

	// Saves the planetTiles of each planet to the saveFile.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Saves the tiles of each planet.
				for (int p = 0; p < tile->numHabitable; ++p) {

					// Saves the PlanetTiles.
					for (int w = 0; w < tile->planets[p]->size; ++w) {
						for (int h = 0; h < tile->planets[p]->size; ++h)
							tiles.append((char*)&pIndex(w, h, tile->planets[p]), sizeof(pIndex(w, h, tile->planets[p])));


						// If tiles is large, empties it.
						if (tiles.length() > 3600) {
							*saveFile << tiles;
							tiles.clear();

						}
					}
				}
			}
		}
	}

	// Closes with a newline.
	tiles += "\n";

	// Saves the planetTiles.
	*saveFile << tiles;

}

/*
Saves the rivers of each planet.
Saved in the form {size:startingX:startingY:{parse1:parse2:...}}

Note: This could be refactored to save Rivers as one large chunk. This has not been done since
Rivers may eventually become less coherent.
*/
void saveRivers(std::ofstream* saveFile) {
	std::string rivers;
	HabitablePlanet* currHabitable;
	River* river;

	// Starts with the designation rivers.
	rivers += "rivers\n";

	// Saves the rivers of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {
					currHabitable = uSystem(i, j)->planets[plan];

					// Saves each river.
					for (int riv = 0; riv < NUM_RIVERS; ++riv) {
						river = &currHabitable->rivers[riv];

						// Saves metadata for each river.
						rivers.append((char*)&river->size, sizeof(river->size));
						rivers.append((char*)&river->startingX, sizeof(river->startingX));
						rivers.append((char*)&river->startingY, sizeof(river->startingY));

						// Saves parse data for each river.
						for (int currTile = 0; currTile < NUM_RIVER_TILES; ++currTile) {
							rivers.append((char*)&river->riverParse[currTile], sizeof(river->riverParse[currTile]));

						}

						// If rivers is large, empties it.
						if (rivers.length() > 3600) {
							*saveFile << rivers;
							rivers.clear();

						}
					}
				}
			}
		}
	}

	// Closes with a newline.
	rivers += "\n";

	// Saves the rivers.
	*saveFile << rivers;

}

/*
Saves the deposits of each planet.
Saved in the form {good:quantity:{xPos1:xPos2:...}:{yPos1:yPos2:...}}

Note: This could be refactored to save deposits as one large chunk. This has not been done since 
deposits may eventually become less coherent.
*/
void saveDeposits(std::ofstream* saveFile) {
	std::string deposits;
	HabitablePlanet* currHabitable;
	SurfaceDeposit* deposit;

	// Starts with the designation deposits.
	deposits += "deposits\n";

	// Saves the deposits of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {
					currHabitable = uSystem(i, j)->planets[plan];

					// Saves each deposit.
					for (int dep = 0; dep < NUM_DEPOSITS_SURFACE; ++dep) {
						deposit = &currHabitable->surfaceDeposits[dep];

						// Saves metadata for each deposit.
						deposits.append((char*)&deposit->good, sizeof(deposit->good));
						deposits.append((char*)&deposit->quantity, sizeof(deposit->quantity));

						// Saves xPos and yPos for each deposit.
						deposits.append((char*)deposit->xPos, NUM_DEPOSIT_COORDS * sizeof(deposit->xPos[0]));
						deposits.append((char*)deposit->yPos, NUM_DEPOSIT_COORDS * sizeof(deposit->yPos[0]));

						// If deposits is large, empties it.
						if (deposits.length() > 3600) {
							*saveFile << deposits;
							deposits.clear();

						}
					}
				}
			}
		}
	}

	// Closes with a newline.
	deposits += "\n";

	// Saves the deposits.
	*saveFile << deposits;

}

/*
Saves all RaceTemplates.
Saved in the form {numRacePages:{Metadata1:Metadata2:...}:{RaceTemplate1:RaceTemplate2:...}}
	Metadata {arrCurrRace}
	RaceTemplate {parentIndex:name[]:consumedGases[]:producedGases[]:consumed[]:productivities[]}
*/
void saveRaces(std::ofstream* saveFile) {
	std::string races;
	RaceTemplate* race;
	int a, b;
	int index;

	// Starts with the designation races.
	races += "races\n";

	// Saves the total number of RacePages.
	races.append((char*)&numRacePages, sizeof(numRacePages));

	// Saves the metadata of each RacePage.
	for (int i = 0; i < numRacePages; ++i)
		races.append((char*)&racePages[i]->arrCurrRace, sizeof(racePages[i]->arrCurrRace));

	// Saves each RaceTemplate.
	for (int i = 0; i < numRacePages; ++i) {
		for (int j = 0; j < racePages[i]->arrCurrRace; ++j) {
			race = &racePages[i]->races[j];

			// If the RaceTemplate has no parent, stores parent as -1.
			if (race->parent == nullptr) {
				index = -1;
				races.append((char*)&index, sizeof(index));

			}
			// If the RaceTemplate has a parent, stores its index.
			else {
				findRaceIndex(a, b, race->parent);
				index = a * RACE_PAGE_SIZE + b;
				races.append((char*)&index, sizeof(index));

			}

			// Saves the RaceTemplate's name.
			races += race->name + '\n';

			// Saves the RaceTemplate's consumedGases.
			races.append((char*)race->consumedGases, sizeof(race->consumedGases));

			// Saves the RaceTemplate's producedGases.
			races.append((char*)race->producedGases, sizeof(race->producedGases));

			// Saves the RaceTemplate's consumed goods.
			races.append((char*)race->consumed, sizeof(race->consumed));

			// Saves the RaceTemplate's productivities.
			races.append((char*)race->productitivities, sizeof(race->productitivities));

			// If races is large, empties it.
			if (races.length() > 3600) {
				*saveFile << races;
				races.clear();

			}
		}
	}

	// Closes with a newline.
	races += "\n";

	// Saves the races.
	*saveFile << races;

}

/*
Saves all Markets.

Saved in the form {numMarketPages:{Metadata1:Metadata2:...}:{Market1:Market2:...}}
	Metadata {arrCurrMarket}
	Market {goods[]:numFoods:foods[]}

Note that marketCapital is saved with Colonies.
*/
void saveMarkets(std::ofstream* saveFile) {
	std::string markets;
	Market* market;

	// Starts with the designation markets.
	markets += "markets\n";

	// Saves the total number of MarketPages.
	markets.append((char*)&numMarketPages, sizeof(numMarketPages));

	// Saves the metadata of each MarketPage.
	for (int i = 0; i < numMarketPages; ++i) {
		markets.append((char*)&marketPages[i]->arrCurrMarket, sizeof(marketPages[i]->arrCurrMarket));

	}

	// Saves each Market.
	for (int i = 0; i < numMarketPages; ++i) {
		for (int j = 0; j < marketPages[i]->arrCurrMarket; ++j) {
			market = &marketPages[i]->markets[j];

			// Saves Goods.
			markets.append((char*)&market->goods, sizeof(market->goods));

			// Saves numFoods.
			markets.append((char*)&market->numFoods, sizeof(market->numFoods));

			// Saves the Foods of the Market.
			markets.append((char*)market->foods, market->numFoods * sizeof(Food));

			// If markets is large, empties it.
			if (markets.length() > 3600) {
				*saveFile << markets;
				markets.clear();

			}
		}
	}


	// Closes with a newline.
	markets += "\n";

	// Saves the markets.
	*saveFile << markets;

}

/*
Saves a RaceInstance. Helper function for saveColonies.

Saved in the form {raceIndex:numPops:numUnemployed}
*/
inline void saveRaceInstance(std::string& races, RaceInstance* race) {
	int a, b;
	int index;

	// Saves the RaceInstance's RaceTemplate.
	findRaceIndex(a, b, race->race);
	index = a * RACE_PAGE_SIZE + b;
	races.append((char*)&index, sizeof(index));

	// Saves numPops.
	races.append((char*)&race->numPops, sizeof(race->numPops));

	// Saves numUnemployed.
	races.append((char*)&race->numUnemployed, sizeof(race->numUnemployed));

}

/*
Saves all Colonies.

Saved in the form {numColonyPages:{Metadata1:Metadata2:...}:{Colony1:Colony2:...}}
	Metadata {arrCurrColony}
	Colony {planetIndex:marketCapital:marketIndex:numRaces:races[]:governmentOwner}

Note that Government is saved with Governments.
*/
void saveColonies(std::ofstream* saveFile) {
	std::string colonies;
	Colony* colony;
	int index;
	int a, b;
	uint_least8_t capital;

	// Starts with the designation colonies.
	colonies += "colonies\n";

	// Saves the total number of ColonyPages.
	colonies.append((char*)&numColonyPages, sizeof(numColonyPages));

	// Saves the metadata of each ColonyPage.
	for (int i = 0; i < numColonyPages; ++i) {
		colonies.append((char*)&colonyPages[i]->arrCurrColony, sizeof(colonyPages[i]->arrCurrColony));

	}

	// Saves each colony.
	for (int i = 0; i < numColonyPages; ++i) {
		for (int j = 0; j < colonyPages[i]->arrCurrColony; ++j) {
			colony = &colonyPages[i]->colonies[j];

			// Saves the Colony's HabitablePlanet.
			findHabitableIndex(a, b, colony->planet);
			index = a * HABITABLE_PAGE_SIZE + b;
			colonies.append((char*)&index, sizeof(index));

			// Marks whether this Colony is the marketCapital of its Market.
			capital = colony->market->marketCapital == colony;
			colonies.append((char*)&capital, sizeof(capital));

			// Saves the Colony's Market.
			findMarketIndex(a, b, colony->market);
			index = a * MARKET_PAGE_SIZE + b;
			colonies.append((char*)&index, sizeof(index));

			// Saves the Colony's number of RaceTemplates.
			colonies.append((char*)&colony->numRaces, sizeof(colony->numRaces));

			// Saves the Colony's RaceInstances.
			for (int r = 0; r < colony->numRaces; ++r) saveRaceInstance(colonies, &colony->pops[r]);

			// Saves the Colony's governmentOwner.
			colonies.append((char*)&colony->governmentOwner, sizeof(colony->governmentOwner));

			// If colonies is large, empties it.
			if (colonies.length() > 3600) {
				*saveFile << colonies;
				colonies.clear();

			}
		}
	}

	// Closes with a newline.
	colonies += "\n";

	// Saves the colonies.
	*saveFile << colonies;

}

/*
Saves all governments.

Saved in the form {numGovernmentPages:{Metadata1:Metadata2:...}:{Government1:Government2:...}}
	Metadata {arrCurrGovernment}
	Government {behaviours[]:parentIndex:numColonies:colonies[]:units[]:flagColour}

TODO if unitTable is associated with parent that should be marked somehow.
*/
void saveGovernments(std::ofstream* saveFile) {
	std::string governments;
	Government* government;
	int index;
	uint_least8_t behaviour;
	int a, b;

	// Starts with the designation governments.
	governments += "governments\n";

	// Saves the total number of GovernmentPages.
	governments.append((char*)&numGovernmentPages, sizeof(numGovernmentPages));

	// Saves the metadata of each GovernmentPage.
	for (int i = 0; i < numGovernmentPages; ++i) {
		governments.append((char*)&governmentPages[i]->arrCurrGovernment, sizeof(governmentPages[i]->arrCurrGovernment));

	}

	// Saves each Government.
	for (int page = 0; page < numGovernmentPages; ++page) {
		for (int currGovernment = 0; currGovernment < governmentPages[page]->arrCurrGovernment; ++currGovernment) {
			government = &governmentPages[page]->governments[currGovernment];

			// Saves the Government's behaviours.
			for (int b = 0; b < NUM_GOVERNMENT_BEHAVIOURS; ++b) {

				// Determines which behaviour to save.
				if (government->behaviours[b] == OrcGroundExpand) behaviour = ORC_EXPAND;
				if (government->behaviours[b] == OrcProduce) behaviour = ORC_PRODUCE;
				if (government->behaviours[b] == OrcReinforce) behaviour = ORC_REINFORCE;
				// TODO more behaviours
				else behaviour = NULL_BEHAVIOUR;

				// Saves the behaviour.
				governments.append((char*)&behaviour, sizeof(behaviour));

			}

			// If the Government has no parent, stores parent as -1.
			if (government->parent == nullptr) {
				index = -1;
				governments.append((char*)&index, sizeof(index));

			}
			// If the Government has a parent, stores its index.
			else {
				findGovernmentIndex(a, b, government->parent);
				index = a * GOVERNMENT_PAGE_SIZE + b;
				governments.append((char*)&index, sizeof(index));

			}

			// Saves the Government's number of Colonies.
			governments.append((char*)&government->numColonies, sizeof(government->numColonies));

			// Saves the Government's Colonies.
			for (int col = 0; col < government->numColonies; ++col) {

				// Saves the Colony.
				findColonyIndex(a, b, government->colonies[col]);
				index = a * COLONY_PAGE_SIZE + b;
				governments.append((char*)&index, sizeof(int));

			}

			// Saves the Government's unitTable.
			a = -1;
			if (government->unitTable == nullptr) governments.append((char*)&a, sizeof(a));
			else governments.append((char*)government->unitTable, (*(int*)government->unitTable + 1) * sizeof(GroundUnitTemplate));

			// Saves the Government's flagColour.
			governments.append((char*)&government->flagColour, sizeof(government->flagColour));

			// If governments is large, empties it.
			if (governments.length() > 3600) {
				*saveFile << governments;
				governments.clear();

			}
		}
	}

	// Closes with a newline.
	governments += "\n";

	// Saves the governments.
	*saveFile << governments;

}

/*
Saves all HabitablePlanet Owners.

Saved in the form {ownedBuildings[]:colony:owner}

TODO distinguish between types of owners (if that happens)
TODO save BarrenPlanet Owners
*/
void saveHabitableOwners(std::ofstream* saveFile) {
	std::string owners;
	HabitablePlanet* planet;
	Owner* owner;
	int a, b;
	int ind;

	// Starts with the designation owners.
	owners += "owners\n";

	// Saves the owners of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {
					planet = uSystem(i, j)->planets[plan];

					// Saves each owner on the HabitablePlanet.
					for (int own = 0; own < NUM_HABITABLE_OWNERS; ++own) {
						owner = &planet->owners[own];

						// Saves the Owner's ownedBuildings.
						owners.append((char*)&owner->ownedBuildings, sizeof(owner->ownedBuildings));

						// If the Owner has no Colony, stores Colony as -1.
						if (owner->colony == nullptr) {
							ind = -1;
							owners.append((char*)&ind, sizeof(ind));

						}
						// If the Owner has a Colony, stores its index.
						else {
							findColonyIndex(a, b, owner->colony);
							ind = a * COLONY_PAGE_SIZE + b;
							owners.append((char*)&ind, sizeof(ind));

						}

						// TODO make this account for different types of Owner.
						// If the Owner has no owner, stores owner as -1.
						if (owner->owner == nullptr) {
							ind = -1;
							owners.append((char*)&ind, sizeof(ind));

						}
						// If the Owner has an owner, stores its index.
						else {
							findGovernmentIndex(a, b, (Government*)owner->owner);
							ind = a * GOVERNMENT_PAGE_SIZE + b;
							owners.append((char*)&ind, sizeof(ind));

						}

						// If owners is large, empties it.
						if (owners.length() > 3600) {
							*saveFile << owners;
							owners.clear();

						}
					}
				}
			}
		}
	}

	// Closes with a newline.
	owners += "\n";

	// Saves the owners.
	*saveFile << owners;

}

/*
Saves all BarrenPlanet Owners.

Saved in the form {ownedBuildings[]:colony:owner}
TODO distinguish between types of owners (if that happens)
*/
void saveBarrenOwners(std::ofstream* saveFile) {
	std::string owners;
	BarrenPlanet* planet;
	Owner* owner;
	int a, b;
	int ind;

	// Starts with the designation owners.
	owners += "owners\n";

	// If owners is large, empties it.
	if (owners.length() > 3600) {
		*saveFile << owners;
		owners.clear();

	}

	// Closes with a newline.
	owners += "\n";

	// Saves the owners.
	*saveFile << owners;

}

/*
Saves the battle of each planet.
Saved in the form {numOwners:owners[]:numFronts:fronts[]:compSize:comp:numMovements:movements:GroundBattleTiles[]}
	Owner {colonyIndex: TODO unit}
	ComparisonMatrix {numRows:matrix[]}
	Movement {movementSize:coords[]}

Note: planet will be found while loading.

TODO save groundUnitTemplate reference here. Presently inferred from Colony.
*/
void saveBattles(std::ofstream* saveFile) {
	std::string battles;
	Battle* battle;
	GroundOwner* owner;
	int size;
	int ind;
	int a, b;

	// Starts with the designation battles.
	battles += "battles\n";

	// Saves the Battles of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {
					battle = uSystem(i, j)->planets[plan]->battle;

					// If there is a Battle on the current planet, saves it.
					if (battle) {
						size = uSystem(i, j)->planets[plan]->size;

						// Marks the battle by saving 1.
						ind = 1;
						battles.append((char*)&ind, sizeof(ind));

						// Saves the number of Owners for this Battle.
						battles.append((char*)&battle->comp.byte1, sizeof(battle->comp.byte1));

						// Saves the Battle's owners.
						for (int own = 1; own < battle->comp.byte1; ++own) {
							owner = &battle->owners[own];

							// If the Owner has a Colony, stores its index.
							findColonyIndex(a, b, owner->owner);
							ind = a * COLONY_PAGE_SIZE + b;
							battles.append((char*)&ind, sizeof(ind));

							// TODO save groundUnitTemplate here. For now it is inferred from Colony.

						}

						// Saves the number of GroundFronts for this Battle.
						battles.append((char*)&battle->comp.short1, sizeof(battle->comp.short1));
						
						// Saves the Battle's GroundFronts.
						battles.append((char*)battle->fronts, battle->comp.short1 * sizeof(GroundFront));

						// Saves the size of the Battle's comp.
						battles.append((char*)&battle->comp.numRows, sizeof(battle->comp.numRows));

						// Saves the Battle's comp.
						battles.append((char*)battle->comp.matrix, battle->comp.size() * sizeof(battle->comp.matrix[0]));

						// Saves the number of movements for the Battle.
						ind = battle->movements.size();
						battles.append((char*)&ind, sizeof(ind));

						// Saves the Battle's movements.
						for (LinkedListNode<Coord>* currNode = battle->movements.root; currNode != nullptr; currNode = currNode->child) {

							// Saves the length of the movement.
							battles.append((char*)&currNode->value->x, sizeof(currNode->value->x));

							// Saves the movement.
							battles.append((char*)currNode->value, (currNode->value->x + 1) * sizeof(currNode->value[0]));

						}

						// Saves the Battle's GroundBattleTiles.
						battles.append((char*)battle->battlefield, size * size * sizeof(battle->battlefield[0]));

					}
					// If there is no battle on the current planet, marks it with 0.
					else {
						ind = 0;
						battles.append((char*)&ind, sizeof(ind));

					}

					// If battles is large, empties it.
					if (battles.length() > 3600) {
						*saveFile << battles;
						battles.clear();

					}
				}
			}
		}
	}

	// Closes with a newline.
	battles += "\n";

	// Saves the battles.
	*saveFile << battles;

}

/*
Loads a file describing the current play session.
1. Creates the file object to load from.
2.
3. Loads the universe by calling loadUniverseTiles.
TODO cont
*/
void loadFile(std::string fileName) {

	// Creates the fileLocation string.
	std::string fileLocation("Save Files/");
	fileLocation.append(fileName.c_str());
	fileLocation.append(".txt");

	// Opens a file of the inputed fileName.
	std::ifstream saveFile{};
	saveFile.open(fileLocation, std::ios::binary);

	// Loads metadata for the game.
	loadMetaData(&saveFile);

	// Loads the universe's tiles.
	loadUniverseTiles(&saveFile);

	// Loads the system's metadata.
	loadSystemTiles(&saveFile);

	// Loads the stars' metadata.
	loadStars(&saveFile);

	// Allocates memory for planets.
	loadInitPlanets();

	// Loads the planets' metadata.
	loadPlanets(&saveFile);

	// Loads the barrens' metadata.
	loadBarrens(&saveFile);

	// Loads the planetTiles of each planet.
	loadPlanetTiles(&saveFile);

	// Loads the Rivers of each planet.
	loadRivers(&saveFile);

	// Loads the Deposits of each planet.
	loadDeposits(&saveFile);

	// Loads all RaceTemplates.
	loadRaces(&saveFile);

	// Loads all Markets.
	loadMarkets(&saveFile);

	// Loads all Colonies.
	loadColonies(&saveFile);

	// Loads all Governments.
	loadGovernments(&saveFile);

	// Loads all HabitablePlanet Owners.
	loadHabitableOwners(&saveFile);

	// Loads all BarrenPlanet Owners.
	loadBarrenOwners(&saveFile);

	// Loads the Battles of each planet.
	loadBattles(&saveFile);

	// Close the file and return true.
	saveFile.close();

}

/*
Loads metadata for the game.
*/
void loadMetaData(std::ifstream* saveFile) {
	int dummySize;

	// Confirms that meta has been found.
	confirmToken(saveFile, "meta");

	// Loads dummySize.
	saveFile->read((char*)&dummySize, sizeof(dummySize));
	initDummies(dummySize);

	// Loads largeDummySize.
	saveFile->read((char*)&dummySize, sizeof(dummySize));
	initLargeDummies(dummySize);

}

/*
Loads the tiles of the inputed file to the universe.
*/
void loadUniverseTiles(std::ifstream* saveFile) {
	uint_least8_t buff;
	int** dummyUniverse;

	// Confirms that utiles has been found.
	confirmToken(saveFile, "utiles");

	// Finds universeWidth and universeHeight.
	saveFile->read((char*)&universeWidth, sizeof(universeWidth));
	saveFile->read((char*)&universeHeight, sizeof(universeHeight));

	// Parses through \n
	saveFile->ignore(1);

	// Allocates a universe canvas.
	dummyUniverse = createCanvas(universeWidth, universeHeight);

	// Loads the universe into a dummy.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			saveFile->read((char*)&buff, sizeof(uint_least8_t));
			dummyUniverse[i][j] = buff;

		}
	}

	// Creates a new universe of the appropriate size.
	universeToSpace(dummyUniverse, universeWidth, universeHeight);

	// Deallocates the universe canvas.
	deallocateCanvas(dummyUniverse, universeWidth, universeHeight);

}

/*
Loads the systems of the saveFile into the universe.
*/
void loadSystemTiles(std::ifstream* saveFile) {
	uint_least64_t buff[1];
	System* currSystem;

	// Confirms that systems has been found.
	confirmToken(saveFile, "systems");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				currSystem = uSystem(i, j);

				// Places the systems' metadata.
				saveFile->read((char*)&currSystem->numStars, sizeof(currSystem->numStars));
				saveFile->read((char*)buff, sizeof(currSystem->numHabitable));
				currSystem->initHabitable(*(uint_least8_t*)buff);
				saveFile->read((char*)buff, sizeof(currSystem->numBarren));
				currSystem->initBarren(*(uint_least8_t*)buff);

			}
		}
	}
}

/*
Loads the metadata of the saveFile's stars into the universe.
*/
void loadStars(std::ifstream* saveFile) {
	System* tile;

	// Confirms that stars has been found.
	confirmToken(saveFile, "stars");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Places the stars' metadata.
				for (int s = 0; s < tile->numStars; ++s) {
					saveFile->read((char*)&tile->stars[s].size, sizeof(tile->stars[s].size));
					saveFile->read((char*)&tile->stars[s].tileID, sizeof(tile->stars[s].tileID));
					saveFile->read((char*)&tile->stars[s].luminosity, sizeof(tile->stars[s].luminosity));

				}
			}
		}
	}
}

/*
Loads the metadata of the saveFile's planets into the universe.
*/
void loadPlanets(std::ifstream* saveFile) {
	int numTiles = 0;
	uint_least8_t size;
	int distance;
	int temperature;
	float heatMultiple;
	System* tile;

	// Confirms that planets has been found.
	confirmToken(saveFile, "planets");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Places the planets and loads their metadata.
				for (int p = 0; p < tile->numHabitable; ++p) {

					// Loads the current planet's attributes.
					saveFile->read((char*)&size, sizeof(size));
					saveFile->read((char*)&distance, sizeof(distance));
					heatMultiple = findHeatMultiple((double)distance * PIXEL_METERS, findSystemLuminosity(tile));
					temperature = (int)findHeat(heatMultiple, 0.3, 1.0);

					// Places the planet.
					tile->planets[p] = placeHabitable(size);
					tile->planets[p]->size = size;
					tile->planets[p]->distance = distance;
					tile->planets[p]->heatMultiple = heatMultiple;
					tile->planets[p]->temperature = temperature;
					numTiles += (int)size * (int)size;

				}
			}
		}
	}

	// Allocates planet tiles.
	placePlanetTiles(numTiles + 1, true);

	// Places planet tiles for each system.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeWidth; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Places planet tiles for each planet within each system.
				for (int p = 0; p < tile->numHabitable; ++p) {
					tile->planets[p]->planet = placePlanetTiles(tile->planets[p]->size * tile->planets[p]->size, false);

				}
			}
		}
	}
}

/*
Loads the metadata of the saveFile's barrens into the universe.
*/
void loadBarrens(std::ifstream* saveFile) {
	uint_least8_t size;
	int distance;
	int temperature;
	float heatMultiple;
	System* tile;

	// Confirms that barrens has been found.
	confirmToken(saveFile, "barrens");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Places the planets and loads their metadata.
				for (int p = 0; p < tile->numBarren; ++p) {

					// Loads the current planet's attributes.
					saveFile->read((char*)&size, sizeof(size));
					saveFile->read((char*)&distance, sizeof(distance));
					heatMultiple = findHeatMultiple((double)distance * PIXEL_METERS, findSystemLuminosity(tile));
					temperature = (int)findHeat(heatMultiple, 0.3, 1.0);

					// Places the planet.
					tile->barrens[p] = placeBarren();
					tile->barrens[p]->size = size;
					tile->barrens[p]->distance = distance;
					tile->barrens[p]->heatMultiple = heatMultiple;
					tile->barrens[p]->temperature = temperature;

				}
			}
		}
	}
}

/*
Loads the planetTiles of each planet.
*/
void loadPlanetTiles(std::ifstream* saveFile) {
	System* tile;

	// Confirms that planetTiles has been found.
	confirmToken(saveFile, "planetTiles");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Loads the PlanetTiles for every planet.
				for (int p = 0; p < tile->numHabitable; ++p) {

					// Loads the PlanetTiles.
					for (int w = 0; w < tile->planets[p]->size; ++w) {
						for (int h = 0; h < tile->planets[p]->size; ++h) {
							saveFile->read((char*)&pIndex(w, h, tile->planets[p]), sizeof(pIndex(w, h, tile->planets[p])));

						}
					}
				}
			}
		}
	}
}

/*
Loads the Rivers of each planet.
*/
void loadRivers(std::ifstream* saveFile) {
	System* tile;
	HabitablePlanet* currPlanet;
	River* river;

	// Confirms that rivers has been found.
	confirmToken(saveFile, "rivers");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Loads the Rivers for each planet.
				for (int p = 0; p < tile->numHabitable; ++p) {
					currPlanet = tile->planets[p];

					// Loads each River.
					for (int r = 0; r < NUM_RIVERS; ++r) {
						river = &currPlanet->rivers[r];

						// Reads the river's size.
						saveFile->read((char*)&river->size, sizeof(river->size));

						// Reads the river's startingX.
						saveFile->read((char*)&river->startingX, sizeof(river->startingX));

						// Reads the river's startingY.
						saveFile->read((char*)&river->startingY, sizeof(river->startingY));

						// Loads each river parse.
						for (int t = 0; t < NUM_RIVER_TILES; ++t) {
							saveFile->read((char*)&river->riverParse[t], sizeof(river->riverParse[t]));

						}
					}
				}
			}
		}
	}
}

/*
Loads the Deposits of each planet.
*/
void loadDeposits(std::ifstream* saveFile) {
	System* tile;
	HabitablePlanet* currPlanet;
	SurfaceDeposit* deposit;

	// Confirms that deposits has been found.
	confirmToken(saveFile, "deposits");

	// Finds the system to work with.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				tile = uSystem(i, j);

				// Loads the deposits for each planet.
				for (int p = 0; p < tile->numHabitable; ++p) {
					currPlanet = tile->planets[p];

					// Loads each deposit.
					for (int dep = 0; dep < NUM_DEPOSITS_SURFACE; ++dep) {
						deposit = &currPlanet->surfaceDeposits[dep];

						// Loads the good and quantity.
						saveFile->read((char*)&deposit->good, sizeof(deposit->good));
						saveFile->read((char*)&deposit->quantity, sizeof(deposit->quantity));
						
						// Loads xPos for each deposit.
						for (int pos = 0; pos < NUM_DEPOSIT_COORDS; ++pos) {
							saveFile->read((char*)&deposit->xPos[pos], sizeof(deposit->xPos[pos]));

						}

						// Loads yPos for each deposit.
						for (int pos = 0; pos < NUM_DEPOSIT_COORDS; ++pos) {
							saveFile->read((char*)&deposit->yPos[pos], sizeof(deposit->yPos[pos]));

						}
					}
				}
			}
		}
	}
}

/*
Loads all RaceTemplates.
*/
void loadRaces(std::ifstream* saveFile) {
	uint_least64_t buff[1];
	char cbuff[50];
	RaceTemplate* race;

	// Confirms that races has been found.
	confirmToken(saveFile, "races");

	// Initializes the RacePage table.
	saveFile->read((char*)buff, sizeof(numRacePages));
	initRaces(*(int*)buff * RACE_PAGE_SIZE - 1);

	// Loads the metadata for each RacePage.
	for (int page = 0; page < numRacePages; ++page) {
		saveFile->read((char*)&racePages[page]->arrCurrRace, sizeof(racePages[page]->arrCurrRace));

	}

	// Loads all RaceTemplates.
	for (int page = 0; page < numRacePages; ++page) {
		for (int currRace = 0; currRace < racePages[page]->arrCurrRace; ++currRace) {
			race = &racePages[page]->races[currRace];

			// Assigns parent.
			saveFile->read((char*)buff, sizeof(int));

			// If the saved value is negative, assigns parent to nullptr.
			if (*(int*)buff < 0) race->parent = nullptr;
			// If the saved value is non-negative, finds the parent.
			else race->parent = &racePages[(*(int*)buff / RACE_PAGE_SIZE)]->races[*(int*)buff % RACE_PAGE_SIZE];

			// Assigns name.
			saveFile->getline(cbuff, 50, '\n');
			race->name = std::string(cbuff);

			// Assigns consumedGases.
			saveFile->read((char*)&race->consumedGases, sizeof(race->consumedGases));

			// Assigns producedGases.
			saveFile->read((char*)&race->producedGases, sizeof(race->producedGases));

			// Assigns consumed.
			saveFile->read((char*)&race->consumed, sizeof(race->consumed));

			// Assigns productivities.
			saveFile->read((char*)&race->productitivities, sizeof(race->productitivities));

		}
	}
}

/*
Loads all Markets.
*/
void loadMarkets(std::ifstream* saveFile) {
	uint_least64_t buff[1];
	Market* market;

	// Confirms that markets has been found.
	confirmToken(saveFile, "markets");

	// Initializes the MarketPage table.
	saveFile->read((char*)buff, sizeof(numMarketPages));
	initMarkets(*(int*)buff * MARKET_PAGE_SIZE - 1);

	// Loads the metadata for each MarketPage.
	for (int page = 0; page < numMarketPages; ++page) {
		saveFile->read((char*)&marketPages[page]->arrCurrMarket, sizeof(marketPages[page]->arrCurrMarket));

	}

	// Loads all Markets.
	for (int page = 0; page < numMarketPages; ++page) {
		for (int currMarket = 0; currMarket < marketPages[page]->arrCurrMarket; ++currMarket) {
			market = &marketPages[page]->markets[currMarket];

			// Loads Goods.
			saveFile->read((char*)&market->goods, sizeof(market->goods));

			// Loads numFoods.
			saveFile->read((char*)&market->numFoods, sizeof(market->numFoods));
			
			// Loads foods.
			market->foods = new Food[market->numFoods];
			saveFile->read((char*)market->foods, market->numFoods * sizeof(Food));

		}
	}
}

inline void loadRaceInstances(std::ifstream* saveFile, Colony* colony) {
	RaceInstance* race;
	uint_least64_t buff[1];

	// Allocates the raceInstance array.
	colony->pops = new RaceInstance[colony->numRaces];

	// Loads the Colony's RaceInstances.
	for (int i = 0; i < colony->numRaces; ++i) {
		race = &colony->pops[i];

		// Loads the RaceInstance's RaceTemplate.
		saveFile->read((char*)buff, sizeof(int));
		race->race = &racePages[(*(int*)buff / RACE_PAGE_SIZE)]->races[*(int*)buff % RACE_PAGE_SIZE];

		// Loads the RaceInstance's numPops.
		saveFile->read((char*)&race->numPops, sizeof(race->numPops));

		// Loads the RaceInstance's numUnemployed.
		saveFile->read((char*)&race->numUnemployed, sizeof(race->numUnemployed));

	}
}

/*
Loads all Colonies.
*/
void loadColonies(std::ifstream* saveFile) {
	Colony* colony;
	uint_least64_t buff[1];
	uint_least8_t capital;

	// Confirms that colonies has been found.
	confirmToken(saveFile, "colonies");

	// Initializes the ColonyPage table.
	saveFile->read((char*)buff, sizeof(numColonyPages));
	initColonies(*(int*)buff * COLONY_PAGE_SIZE - 1);

	// Loads the metadata for each ColonyPage.
	for (int page = 0; page < numColonyPages; ++page) {
		saveFile->read((char*)&colonyPages[page]->arrCurrColony, sizeof(colonyPages[page]->arrCurrColony));

	}

	// Loads all Colonies.
	for (int page = 0; page < numColonyPages; ++page) {
		for (int currColony = 0; currColony < colonyPages[page]->arrCurrColony; ++currColony) {
			colony = &colonyPages[page]->colonies[currColony];

			// Loads the Colony's HabiablePlanet.
			saveFile->read((char*)buff, sizeof(int));
			colony->planet = &habitablePages[(*(int*)buff / HABITABLE_PAGE_SIZE)]->planets[*(int*)buff % HABITABLE_PAGE_SIZE];

			// Determines whether this Colony is the marketCapital of its Market.
			saveFile->read((char*)&capital, sizeof(capital));

			// Loads the Colony's market.
			saveFile->read((char*)buff, sizeof(int));
			colony->market = &marketPages[(*(int*)buff / MARKET_PAGE_SIZE)]->markets[*(int*)buff % MARKET_PAGE_SIZE];

			// Loads the Colony's number of RaceInstances.
			saveFile->read((char*)&colony->numRaces, sizeof(colony->numRaces));
			
			// Loads the Colony's RaceInstances.
			loadRaceInstances(saveFile, colony);

			// Loads the Colony's governmentOwner.
			saveFile->read((char*)&colony->governmentOwner, sizeof(colony->governmentOwner));

		}
	}
}

/*
Loads all Governments.
*/
void loadGovernments(std::ifstream* saveFile) {
	Government* government;
	int buff;
	uint_least8_t behaviour;

	// Confirms that governments has been found.
	confirmToken(saveFile, "governments");

	// Initializes the GovernmentPage table.
	saveFile->read((char*)&buff, sizeof(numGovernmentPages));
	initGovernments(buff * GOVERNMENT_PAGE_SIZE - 1);

	// Loads the metadata for each GovernmentPage.
	for (int page = 0; page < numGovernmentPages; ++page) {
		saveFile->read((char*)&governmentPages[page]->arrCurrGovernment, sizeof(governmentPages[page]->arrCurrGovernment));

	}

	// Loads all Governments.
	for (int page = 0; page < numGovernmentPages; ++page) {
		for (int currGovernment = 0; currGovernment < governmentPages[page]->arrCurrGovernment; ++currGovernment) {
			government = &governmentPages[page]->governments[currGovernment];

			// Loads the Government's behaviours.
			for (int b = 0; b < NUM_GOVERNMENT_BEHAVIOURS; ++b) {
				saveFile->read((char*)&behaviour, sizeof(behaviour));

				// Determines the behaviour to load.
				switch (behaviour) {
				case(NULL_BEHAVIOUR):
					government->behaviours[b] = nullptr;
					break;
				case(ORC_EXPAND):
					government->behaviours[b] = OrcGroundExpand;
					break;

				}
			}

			// Loads the Government's parent.
			saveFile->read((char*)&buff, sizeof(buff));

			// If the saved value is negative, assigns parent to nullptr.
			if (buff < 0) government->parent = nullptr;
			// If the saved value is non-negative, finds the parent.
			else government->parent = &governmentPages[(buff / GOVERNMENT_PAGE_SIZE)]->governments[buff % GOVERNMENT_PAGE_SIZE];

			// Loads the Government's number of Colonies.
			saveFile->read((char*)&government->numColonies, sizeof(government->numColonies));
			government->colonies = new Colony*[government->numColonies];

			// Loads the Government's Colonies.
			for (int col = 0; col < government->numColonies; ++col) {

				// Loads the Government's Colony.
				saveFile->read((char*)&buff, sizeof(int));
				government->colonies[col] = &colonyPages[(buff / COLONY_PAGE_SIZE)]->colonies[buff % COLONY_PAGE_SIZE];

				// Marks the Government in the Colony.
				government->colonies[col]->government = government;

			}

			// Loads the Government's unitTable size.
			saveFile->read((char*)&buff, sizeof(int));

			// If the saved value is negative, assigns the unitTable to nullptr.
			if (buff < 0) government->unitTable == nullptr;
			// If the saved value is non-negative, loads the unitTable.
			else  {

				// Initializes the unitTable.
				government->unitTable = new GroundUnitTemplate[buff + 1];
				*((int*)government->unitTable) = buff;

				// Ignores extra space after the size.
				saveFile->ignore(sizeof(GroundUnitTemplate) - sizeof(int));

				// Loads each unit.
				for (int u = 1; u < buff + 1; ++u)
					saveFile->read((char*)&government->unitTable[u], sizeof(GroundUnitTemplate));

			}

			// Loads the Government's flagColour.
			saveFile->read((char*)&government->flagColour, sizeof(government->flagColour));

		}
	}
}

/*
Loads all HabitablePlanet Owners.

TODO account for different types of owner.
*/
void loadHabitableOwners(std::ifstream* saveFile) {
	HabitablePlanet* planet;
	Owner* owner;
	uint_least64_t buff[1];

	// Confirms that owners has been found.
	confirmToken(saveFile, "owners");

	// Loads the owners of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {
					planet = uSystem(i, j)->planets[plan];

					// Loads each owner on the HabitablePlanet.
					for (int own = 0; own < NUM_HABITABLE_OWNERS; ++own) {
						owner = &planet->owners[own];

						// Loads the Owner's ownedBuildings.
						saveFile->read((char*)&owner->ownedBuildings, sizeof(owner->ownedBuildings));

						// Loads the Owner's Colony.
						saveFile->read((char*)buff, sizeof(int));

						// If the saved value is negative, assigns colony to nullptr.
						if (*(int*)buff < 0) owner->colony = nullptr;
						// If the saved value is non-negative, finds the Colony.
						else owner->colony = &colonyPages[(*(int*)buff / COLONY_PAGE_SIZE)]->colonies[*(int*)buff % COLONY_PAGE_SIZE];

						// Loads the Owner's owner.
						// TODO account for different types of owner.
						saveFile->read((char*)buff, sizeof(int));

						// If the saved value is negative, assigns owner to nullptr.
						if (*(int*)buff < 0) owner->owner = nullptr;
						// If the saved value is non-negative, finds the colony.
						else owner->owner = &governmentPages[(*(int*)buff / GOVERNMENT_PAGE_SIZE)]->governments[*(int*)buff % GOVERNMENT_PAGE_SIZE];

					}
				}
			}
		}
	}
}

/*
Loads all BarrenPlanet Owners.

// TODO save and load groundUnitTemplate
*/
void loadBarrenOwners(std::ifstream* saveFile) {

	// Confirms that owners has been found.
	confirmToken(saveFile, "owners");

}

/*
Loads all Battles.
*/
void loadBattles(std::ifstream* saveFile) {
	Battle* battle;
	GroundOwner* owner;
	Coord* movement;
	int buff;
	uint_least8_t buf;

	// Confirms that battles has been found.
	confirmToken(saveFile, "battles");

	// Loads the Battles of each planet in the universe.
	for (int i = 0; i < universeWidth; ++i) {
		for (int j = 0; j < universeHeight; ++j) {
			if (uIndex(i, j).tileID == SYSTEM_TILE) {
				for (int plan = 0; plan < uSystem(i, j)->numHabitable; ++plan) {

					// Checks whether a battle exists.
					saveFile->read((char*)&buff, sizeof(buff));

					// If the Battle exists, loads it.
					if (buff) {

						// Creates the Battle and associates it with its HabitablePlanet.
						battle = (Battle*)calloc(1, sizeof(Battle));
						uSystem(i, j)->planets[plan]->battle = battle;
						battle->planet = uSystem(i, j)->planets[plan];

						// Loads the number of Owners for this Battle.
						saveFile->read((char*)&battle->comp.byte1, sizeof(battle->comp.byte1));
						battle->owners = new GroundOwner[battle->comp.byte1];

						// Loads the Battle's owners.
						for (int own = 1; own < battle->comp.byte1; ++own) {
							owner = &battle->owners[own];

							// Loads the Owner's Colony.
							saveFile->read((char*)&buff, sizeof(buff));
							owner->owner = &colonyPages[(buff / COLONY_PAGE_SIZE)]->colonies[buff % COLONY_PAGE_SIZE];

							// Finds the Owner's groundUnitTemplate.
							// TODO load groundUnitTemplate here. For now it is inferred from Colony.
							owner->unitTable = owner->owner->government->unitTable;

						}

						// Loads the number of GroundFronts for this Battle.
						saveFile->read((char*)&battle->comp.short1, sizeof(battle->comp.short1));
						battle->fronts = new GroundFront[battle->comp.short1];

						// Loads the Battle's GroundFronts.
						saveFile->read((char*)battle->fronts, battle->comp.short1 * sizeof(GroundFront));

						// Loads the size of the Battle's comp.
						saveFile->read((char*)&buff, sizeof(buff));
						battle->comp.init(buff + 1);

						// Loads the Battle's comp.
						saveFile->read((char*)battle->comp.matrix, battle->comp.size() * sizeof(battle->comp.matrix[0]));

						// Loads the number of movements for the Battle.
						saveFile->read((char*)&buff, sizeof(buff));

						// Loads the Battle's movements.
						for (int mov = 0; mov < buff; ++mov) {

							// Loads the size of the current movement.
							saveFile->read((char*)&buf, sizeof(buf));
							++buf; // Increment because x is one below total size.
							movement = new Coord[buf];

							// Loads the movement.
							saveFile->read((char*)movement, buf * sizeof(movement[0]));
							battle->movements.add(movement);

						}

						// Loads the Battle's GroundBattleTiles.
						battle->battlefield = new GroundBattleTile[battle->planet->size * battle->planet->size];
						saveFile->read((char*)battle->battlefield, battle->planet->size * battle->planet->size * sizeof(battle->battlefield[0]));

					}
				}
			}
		}
	}
}

/*
Calls initHabitablePlanets and initBarrenPlanets to create the arrays of habitable and barren planets.
*/
void loadInitPlanets() {
	int numBarren = 0;
	int numHabitable = 0;

	// Counts the number of habitable planets and barren planets.
	for (int i = 0; i < universeWidth * universeHeight; ++i) {
		if (universe[i].tileID == SYSTEM_TILE) {
			numHabitable += universe[i].system->numHabitable;
			numBarren += universe[i].system->numBarren;

		}
	}

	// Calls initHabitable and initBarren.
	initHabitablePlanets(numHabitable);
	initBarrenPlanets(numBarren);

}

/*
Scans an int that goes from the current char to an inputed terminal char.
returns 0 if there is an error.

Note: the inputed save file should be on the char before the int.
*/
long int scanInt(std::ifstream* saveFile, char terminal) {
	char tempChar;
	std::string intString;

	// Parses through an int.
	*saveFile >> tempChar;
	while (tempChar != terminal) {

		// Adds the current char to inputChecker.
		std::string newStr(1, tempChar);
		intString.append(newStr);

		// Increments by one character.
		*saveFile >> tempChar;

	}

	// Returns the int that was read.
	std::string::size_type strSize;
	return std::stol(intString, &strSize);

}

/*
Scans an int that goes from the current char to any non-numerical char (incl. period, comma).
returns 0 if there is an error.

Note: the inputed save file should be on the char before the int.
*/
long int scanInt(std::ifstream* saveFile) {
	char tempChar;
	std::string intString;

	// Parses through an int.
	*saveFile >> tempChar;
	while (tempChar >= '0' && tempChar <= '9') {

		// Adds the current char to inputChecker.
		std::string newStr(1, tempChar);
		intString.append(newStr);

		// Increments by one character.
		*saveFile >> tempChar;

	}

	// Returns the int that was read.
	std::string::size_type strSize;
	return std::stol(intString, &strSize);

}

/*
Scans an unsigned int that goes from the current char to any non-numerical char (incl. period, comma).
returns 0 if there is an error.

Note: the inputed save file should be on the char before the int.
*/
uint_least64_t scanUint(std::ifstream* saveFile) {
	char tempChar;
	std::string intString;

	// Parses through an int.
	*saveFile >> tempChar;
	while (tempChar >= '0' && tempChar <= '9') {

		// Adds the current char to inputChecker.
		std::string newStr(1, tempChar);
		intString.append(newStr);

		// Increments by one character.
		*saveFile >> tempChar;

	}

	// Returns the int that was read.
	std::string::size_type strSize;
	return std::stoul(intString, &strSize);

}

/*
Scans a float from a file.
*/
float scanFloat(std::ifstream* saveFile, char terminal) {
	char tempChar;
	std::string floatString;

	// Parses through an int.
	*saveFile >> tempChar;
	while (tempChar != terminal) {

		// Adds the current char to inputChecker.
		std::string newStr(1, tempChar);
		floatString.append(newStr);

		// Increments by one character.
		*saveFile >> tempChar;

	}

	// Returns the int that was read.
	std::string::size_type strSize;
	return std::stof(floatString, &strSize);

}

/*
Confirms that a token is found in the next saveFile segment. Assumes that the token
is preceded and followed by newline chars and ignores those.

Will crash if the token does not match.
*/
void confirmToken(std::ifstream* saveFile, const char token[]) {
	int len = strlen(token);
	char buff[32];

	// Ignores the first newline.
	saveFile->ignore(1);

	// Checks that the next n chars spell token.
	saveFile->get(buff, len + 1);

	// Ignores the final newline.
	saveFile->ignore(1);

	// TODO DEBUG REMOVE
	printf("token : %s\n", buff);

	// Exits if the characters do not match the token.
	if (strcmp(buff, token)) {
		printf("Failure to load %s\n", token);
		exit(1);

	}
}