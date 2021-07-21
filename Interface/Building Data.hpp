#pragma once

/*
Notes on buildings:

Each index in the below function arrays corresponds exactly to a buildingID.

The build/destroy funcs' inputs should accept planet, market, and owner.
The add/remove funcs' inputs should accept planet, market, owner, and race.
// TODO handle shortages of resources by shutting down a portion of output equivalent to the portion of shortfall.
	// Calculate later, when things are decided.
*/

// Enables building functions to access deconstructing functions and vice-versa.
extern void (*buildingFuncs[])(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData);
extern void (*deconstructingFuncs[])(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData);

/*
Corresponds to Land. The destruction function simply removes the tile from the owner's control.
*/
void initNullBuilding(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the land.
	pBuilding(xPos, yPos, planet) = NoBuilding;

	// Places the Land in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

	// Gives the owner a Land or Water tile.
	if (pIsLand(xPos, yPos, planet)) ++pOwnerBuildingQ(owner, LAND_OWNED, planet, 0);
	else ++pOwnerBuildingQ(owner, LAND_OWNED, planet, 2);

}
void destroyNullBuilding(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	if (pIsLand(xPos, yPos, planet)) --pOwnerBuildingQ(owner, LAND_OWNED, planet, 0);
	else --pOwnerBuildingQ(owner, LAND_OWNED, planet, 2);

}

/*
Boreal forests consume one unit of atmospheric pollutant and output one unit of oxygen.
*/
void initBorealForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the forest.
	++planet->netGases[Oxygen];
	--planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = BorealForest;
	++pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}
void destroyBorealForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	--planet->netGases[Oxygen];
	++planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = NoBuilding;
	--pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}

/*
Deciduous forests consume one unit of atmospheric pollutant and output one unit of oxygen.
*/
void initDeciduousForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the forest.
	++planet->netGases[Oxygen];
	--planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = DeciduousForest;
	++pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}
void destroyDeciduousForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	--planet->netGases[Oxygen];
	++planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = NoBuilding;
	--pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}

/*
Temperate forests consume one unit of atmospheric pollutant and output one unit of oxygen.
*/
void initTemperateForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the forest.
	++planet->netGases[Oxygen];
	--planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = TemperateForest;
	++pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}
void destroyTemperateForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	--planet->netGases[Oxygen];
	++planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = NoBuilding;
	--pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}

/*
Arid forests consume one unit of atmospheric pollutant and output one unit of oxygen.
*/
void initAridForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the forest.
	++planet->netGases[Oxygen];
	--planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = AridForest;
	++pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}
void destroyAridForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	--planet->netGases[Oxygen];
	++planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = NoBuilding;
	--pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}

/*
Tropical forests consume one unit of atmospheric pollutant and output one unit of oxygen.
*/
void initTropicalForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the forest.
	++planet->netGases[Oxygen];
	--planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = TropicalForest;
	++pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}
void destroyTropicalForest(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	--planet->netGases[Oxygen];
	++planet->netGases[Pollutants];
	pBuilding(xPos, yPos, planet) = NoBuilding;
	--pOwnerBuildingQ(owner, LAND_OWNED, planet, 1);

}

/*
Places and destroys level one Harbours.

Directions for harbours are EAST, SOUTH, WEST, NORTH. These indicate the direction from the City/Land.
EAST = 1 to optimize the rotation calculation.

TODO add building manifest for harbours?
TODO inherit direction from previous harbours?
TODO create ways to other Harbours.
*/
void initHarbour1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	int x, y;
	char city = -1;
	char land = -1;

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Assigns the inputed Harbour position if it is included in extraData.
	if (extraData != nullptr) {
		city = (char)extraData;

	}
	// Retains the previous Harbour position if there is already a Harbour in the inputed tile.
	else if (pBuilding(xPos, yPos, planet) >= Harbour1 && pBuilding(xPos, yPos, planet) <= Harbour5) {
		goto placement;

	}
	// Attempts to find a place for the Harbour.
	else {

		// Attempts to find a city to the west of the Harbour.
		x = xPos - 1;
		y = yPos;
		wrapAroundPlanet(planet->size, &x, &y);
		if (pBuilding(x, y, planet) == OrcCity || (pBuilding(x, y, planet) >= City1 && pBuilding(x, y, planet) <= City5)) {
			city = 1;
			goto placement;

		}
		// Attempts to find Land to the east of the Harbour.
		else if (pTileID(x, y, planet) <= LAND_TILE) land = 1;

		// Attempts to find a city to the north of the Harbour.
		x = xPos;
		y = yPos - 1;
		wrapAroundPlanet(planet->size, &x, &y);
		if (pBuilding(x, y, planet) == OrcCity || (pBuilding(x, y, planet) >= City1 && pBuilding(x, y, planet) <= City5)) {
			city = 2;
			goto placement;

		}
		// Attempts to find Land to the south of the Harbour.
		else if (pTileID(x, y, planet) <= LAND_TILE) land = 2;

		// Attempts to find a city to the east of the Harbour.
		x = xPos + 1;
		y = yPos;
		wrapAroundPlanet(planet->size, &x, &y);
		if (pBuilding(x, y, planet) == OrcCity || (pBuilding(x, y, planet) >= City1 && pBuilding(x, y, planet) <= City5)) {
			city = 3;
			goto placement;

		}
		// Attempts to find Land to the west of the Harbour.
		else if (pTileID(x, y, planet) <= LAND_TILE) land = 3;

		// Attempts to find a city to the south of the Harbour.
		x = xPos;
		y = yPos + 1;
		wrapAroundPlanet(planet->size, &x, &y);
		if (pBuilding(x, y, planet) == OrcCity || (pBuilding(x, y, planet) >= City1 && pBuilding(x, y, planet) <= City5)) {
			city = 4;
			goto placement;

		}
		// Attempts to find Land to the south of the Harbour.
		else if (pTileID(x, y, planet) <= LAND_TILE) land = 4;

	}

	// Places the Harbour's direction.
	placeDir:;

	// Place Harbour's direction away from any nearby city.
	if (city != -1) pIndex(xPos, yPos, planet).numData = city - 1;
	// Places the Harbour's direction away from any nearby land.
	else pIndex(xPos, yPos, planet).numData = land - 1;

	// Places the Harbour.
	placement:;

	// Places the level one Harbour.
	pBuilding(xPos, yPos, planet) = Harbour1;

	// Places the level one Harbour in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

}
void destroyHarbour1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places and destroys level two Harbours.

TODO
*/
void initHarbour2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}
void destroyHarbour2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}

/*
Places and destroys level three Harbours.

TODO
*/
void initHarbour3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}
void destroyHarbour3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}

/*
Places and destroys level four Harbours.

TODO
*/
void initHarbour4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}
void destroyHarbour4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}

/*
Places and destroys level five Harbours.

TODO
*/
void initHarbour5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}
void destroyHarbour5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

}

/*
Places and destroys level one Mines.
*/
void initMine1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level one mine.
	pBuilding(xPos, yPos, planet) = Mine1;

	// Places the level one mine in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

	// Gives the owner a level one mine.
	++planet->owners[owner].ownedBuildings[MINE].numBuildings[0];

	// Opens one position for the owner of the mine.
	++planet->owners[owner].ownedBuildings[MINE].numOpen += 1;

}
void destroyMine1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

	// Removes a level one mine from the owner.
	--planet->owners[owner].ownedBuildings[MINE].numBuildings[0];

	// Closes one position for the owner of the mine.
	--planet->owners[owner].ownedBuildings[MINE].numOpen;

}

/*
Places and destroys level two Mines.
*/
void initMine2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level two mine.
	pBuilding(xPos, yPos, planet) = Mine2;

	// Gives the owner a level two mine.
	++planet->owners[owner].ownedBuildings[MINE].numBuildings[1];

}
void destroyMine2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes a level two mine from the owner.
	--planet->owners[owner].ownedBuildings[MINE].numBuildings[1];

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places and destroys level three Mines.
*/
void initMine3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level three mine.
	pBuilding(xPos, yPos, planet) = Mine3;

	// Gives the owner a level three mine.
	++planet->owners[owner].ownedBuildings[MINE].numBuildings[2];

}
void destroyMine3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes a level three mine from the owner.
	--planet->owners[owner].ownedBuildings[MINE].numBuildings[2];

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places and destroys level four Mines.
*/
void initMine4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level four mine.
	pBuilding(xPos, yPos, planet) = Mine4;

	// Gives the owner a level four mine.
	++planet->owners[owner].ownedBuildings[MINE].numBuildings[3];

}
void destroyMine4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes a level four mine from the owner.
	--planet->owners[owner].ownedBuildings[MINE].numBuildings[3];

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places and destroys level five Mines.
*/
void initMine5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level five mine.
	pBuilding(xPos, yPos, planet) = Mine5;

	// Gives the owner a level five mine.
	++planet->owners[owner].ownedBuildings[MINE].numBuildings[4];

}
void destroyMine5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes a level five mine from the owner.
	--planet->owners[owner].ownedBuildings[MINE].numBuildings[4];

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level one cities require XXXXXXX mainainence.
TODO modify
*/
void initCity1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the level one city.
	pBuilding(xPos, yPos, planet) = City1;

	// Places the level one city in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

	// Gives the owner a level one city.
	//++planet->owners[owner].ownedBuildings[City].numBuildings[0];

	// Opens a position for the owner of the city.
	//++planet->owners[owner].ownedBuildings[City].numOpen;

}
void destroyCity1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Clears the tile.
	pBuilding(xPos, yPos, planet) = NoBuilding;

	// Removes a level one city from the owner.
	//--planet->owners[owner].ownedBuildings[City].numBuildings[0];

	// Closes one position for the owner of the city.
	//--planet->owners[owner].ownedBuildings[City].numOpen;

}

/*
Level two cities require XXXXXXX mainainence.
// TODO modify.
*/
void initCity2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = City2;

}
void destroyCity2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level three cities require XXXXXXX mainainence.
// TODO modify.
*/
void initCity3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = City3;

}
void destroyCity3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level four cities require XXXXXXX mainainence.
// TODO modify.
*/
void initCity4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = City4;

}
void destroyCity4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level five cities require XXXXXXX mainainence.
// TODO modify.
*/
void initCity5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = City5;

}
void destroyCity5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level one farms require XXXXXXX mainainence.
// TODO modify.
*/
void initFarm1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Farm1;

}
void destroyFarm1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level two farms require XXXXXXX mainainence.
// TODO modify.
*/
void initFarm2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Farm2;

}
void destroyFarm2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level three farms require XXXXXXX mainainence.
// TODO modify.
*/
void initFarm3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Farm3;

}
void destroyFarm3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level four farms require XXXXXXX mainainence.
// TODO modify.
*/
void initFarm4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Farm4;

}
void destroyFarm4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level five farms require XXXXXXX mainainence.
// TODO modify.
*/
void initFarm5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Farm5;

}
void destroyFarm5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level one pastures require XXXXXXX mainainence.
// TODO modify.
*/
void initPasture1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Pasture1;

}
void destroyPasture1(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level two pastures require XXXXXXX mainainence.
// TODO modify.
*/
void initPasture2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Pasture2;

}
void destroyPasture2(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level three pastures require XXXXXXX mainainence.
// TODO modify.
*/
void initPasture3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Pasture3;

}
void destroyPasture3(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level four pastures require XXXXXXX mainainence.
// TODO modify.
*/
void initPasture4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Pasture4;

}
void destroyPasture4(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Level five pastures require XXXXXXX mainainence.
// TODO modify.
*/
void initPasture5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	pBuilding(xPos, yPos, planet) = Pasture5;

}
void destroyPasture5(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places an OrcFarm on the planet.
*/
void initOrcFarm(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was present prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the OrcFarm.
	pBuilding(xPos, yPos, planet) = OrcFarm;

	// Places the OrcFarm in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

}
void destroyOrcFarm(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes the OrcFarm.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places an OrcMine on the planet.
*/
void initOrcMine(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the OrcMine.
	pBuilding(xPos, yPos, planet) = OrcMine;

	// Places the OrcMine in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

}
void destroyOrcMine(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes the OrcMine.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

/*
Places an OrcCity on the planet.
*/
void initOrcCity(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Demolishes whatever was there prior.
	deconstructingFuncs[pBuilding(xPos, yPos, planet)](planet, market, poIndex(xPos, yPos, planet), xPos, yPos, nullptr);

	// Places the OrcCity.
	pBuilding(xPos, yPos, planet) = OrcCity;

	// Places the OrcCity in the owner's control.
	poIndex(xPos, yPos, planet) = owner;

}
void destroyOrcCity(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) {

	// Removes the OrcCity.
	pBuilding(xPos, yPos, planet) = NoBuilding;

}

// Jump table for building buildings.
void (*buildingFuncs[])(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) = {
	initNullBuilding,
	initBorealForest,
	initDeciduousForest,
	initTemperateForest,
	initAridForest,
	initTropicalForest,
	initHarbour1,
	initHarbour2,
	initHarbour3,
	initHarbour4,
	initHarbour5,
	initMine1,
	initMine2,
	initMine3,
	initMine4,
	initMine5,
	initCity1,
	initCity2,
	initCity3,
	initCity4,
	initCity5,
	initFarm1,
	initFarm2,
	initFarm3,
	initFarm4,
	initFarm5,
	initPasture1,
	initPasture2,
	initPasture3,
	initPasture4,
	initPasture5,
	initOrcFarm,
	initOrcMine,
	initOrcCity

};

// Jump table for destroying buildings.
// Destroying forest decreases land ownership.
void (*deconstructingFuncs[])(HabitablePlanet* planet, Market* market, int owner, int xPos, int yPos, void* extraData) = {
	destroyNullBuilding,
	destroyBorealForest,
	destroyDeciduousForest,
	destroyTemperateForest,
	destroyAridForest,
	destroyTropicalForest,
	destroyHarbour1,
	destroyHarbour2,
	destroyHarbour3,
	destroyHarbour4,
	destroyHarbour5,
	destroyMine1,
	destroyMine2,
	destroyMine3,
	destroyMine4,
	destroyMine5,
	destroyCity1,
	destroyCity2,
	destroyCity3,
	destroyCity4,
	destroyCity5,
	destroyFarm1,
	destroyFarm2,
	destroyFarm3,
	destroyFarm4,
	destroyFarm5,
	destroyPasture1,
	destroyPasture2,
	destroyPasture3,
	destroyPasture4,
	destroyPasture5,
	destroyOrcFarm,
	destroyOrcMine,
	destroyOrcCity

};

/*
Null buildings produce nothing.
Forests produce oxygen and consume pollutants.
*/
void productionLand(HabitablePlanet* planet, Owner* owner, void* extraData) {

}

/*
TODO comment
*/
void productionMines(HabitablePlanet* planet, Owner* owner, void* extraData) {

	// TODO ensures that there is no shortage of maintenance materials or inputs.

	// Produces one mineral per worker.
	// TODO implement.
	// Creates minerals for each worker.
	for (int i = 0; i < owner->ownedBuildings[MINE].numRaces; ++i) {
		owner->colony->market->goods[BaseMetals].quantity += owner->ownedBuildings[MINE].workerRaces[i].workerQuantity +
			owner->colony->pops[owner->ownedBuildings[MINE].workerRaces[i].workerRace].race->productitivities[BaseMetals];

	}
}

/*
Level one farms produce ...
*/
void productionFarms(HabitablePlanet* planet, Owner* owner, void* extraData) {

}

/*
Level one pastures produce ...
*/
void productionPastures(HabitablePlanet* planet, Owner* owner, void* extraData) {

}

// Jump table for building production.
void (*buildingProductionFuncs[])(HabitablePlanet* planet, Owner* owner, void* extraData) = {
	productionLand,
	productionMines,
	productionFarms,
	productionPastures

};

// bullding for worker funcs
void workerNoChange(HabitablePlanet* planet, Market* market, int owner, int race) {

}

// Array of functions for adding a worker to a building.
void (*addWorkerFuncs[])(HabitablePlanet* planet, Market* market, int owner, int race) = {
	workerNoChange

};

// Array of functions for removing a worker from a building.
void (*removeWorkerFuncs[])(HabitablePlanet * planet, Market* market, int owner, int race) = {
	workerNoChange

};