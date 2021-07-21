#pragma once

// This function gets the deposit sprite corresponding to a deposit.
SDL_Rect getDepositSprite(int good);

/*
Notes (Outdated):

Each planet's race instance will store:
	A count of how many pops of said race are employed in certain jobs (industry, agriculture, etc.).
		is this true?
	Counts of pops aligned to certain political, ethical, religious factions.

Each race's 'character sheet' will store:
	That races productivity in certain fields.
	Innate racial qualities (innate ethical splits?).

When a pop is created, it checks to see if there is another pop on the planet that it can outcompete.
	When such a pop is found, then and only then begin reallocation.

This avoids having to store the number of pops and their types in an industry

The planet will store:
	A list of underfilled industries.
	A list of underfilled habitations.
	A list of unhoused/unemployed pops.

When a pop looks for housing, it will look at the list of underfilled habitations vs. the list of underfilled industries.
	When genocide/resettlement, infer position from planet data.
	This will minimize long run memory usage substantially (at the cost of processor cycles and immersion).

*/

/*
Stores races.
*/
struct ManifestRace {

	// Number of workers of this race.
	uint_least16_t workerQuantity;

	// Found within the race table of the planet.
	uint_least8_t workerRace;

};

// Number of levels for any given building.
#define NUM_BUILDING_LEVELS 5

// Stores bulding types within the Owner array.
enum BuildingTypes {
	LAND_OWNED, // {land, forest}.
	MINE,
	NUM_BUILDING_TYPES

};

/*
Struct representing a manifest of owned buildings of a certain type.
*/
struct BuildingManifest {

	// Races working in this building.
	ManifestRace* workerRaces;

	// Number of buildings of each level.
	// TODO decide if removal is necessary. Capital, human capital, and race traits
	// dictate productivity and numOpen is as reliable as numBuildings.
	// This may be useful for bookeeping, but that can be done ad-hoc.
	// Note: For Land, 0 is land, 1 is forest, 2 is water.
	uint_least16_t numBuildings[NUM_BUILDING_LEVELS];

	// Number of unfilled worker positions for these buildings.
	uint_least16_t numOpen;

	// Amount of capital belonging to this building type.
	uint_least16_t capital;

	// Number of races working in these buildings.
	uint_least8_t numRaces;

	// One byte before 8 byte alignment.
	// This may be used as an indicator of overflow in capital/numraces.

};

// TODO struct representing farms.

/*
Owner of industries, buildings and land on a planet.
*/
struct Owner {

	// Manifest of owned properties.
	// Note: Index 0 is NoBuilding (owned land).
	BuildingManifest ownedBuildings[NUM_BUILDING_TYPES];

	// Colony that this owner interacts with.
	Colony* colony;

	// TODO add planetary inventory.
	// int invLimit;
	// Market inv;

	// Owner whose stake is represented.
	// If this is equal to colony->government, this is a government.
	void* owner;

};

// IDs for buildings.
enum BuildingID {
	NoBuilding,
	BorealForest, DeciduousForest, TemperateForest, AridForest, TropicalForest, // TropicalForest bounds forests, do not change lightly.
	// Add rainforest/jungle
	Harbour1, Harbour2, Harbour3, Harbour4, Harbour5,
	Mine1, Mine2, Mine3, Mine4, Mine5,
	City1, City2, City3, City4, City5,
	Farm1, Farm2, Farm3, Farm4, Farm5,
	Pasture1, Pasture2, Pasture3, Pasture4, Pasture5,
	// This must be at the end.
	OrcFarm, OrcMine, OrcCity

};

/*
Planet Features.
*/

/*
This struct represents a single river on a planet.
Has a starting position (startingX and startingY).
Has a certain amount (size) of directions (riverParse) that tell the renderer where to go when rendering
the River.
Contains directions indicating directions in which the River's tiles flow (riverTiles).
Note: riverParse is the direction in which one linear river goes, riverTiles indicates
	  the direction in which the river flows including inlets and outlets.
	  riverParse is used for moving along a river, riverTiles is used for rendering a river.
	  This is tactic requires less memory than having a 'tree' of interconnected river objects
	  (3 bytes overhead as opposed to 8 byte pointer and 8*numChildren byte arr).

// TODO perhaps fold riverTiles into riverParse by starting rivers on top of eachother.
	// This would require the renderer rendering rivers backwards as well.
	// Would require a 'mouth' to connect river to ocean.
*/
struct River {
	Direction riverParse[NUM_RIVER_TILES];
	uint_least8_t startingX;
	uint_least8_t startingY;
	uint_least8_t size;

};

/*
Deposits for Habitable Planets.
*/
/*
Note: Deposits are measured in kilotons, since tons cannot fit into a double.

// Size of a double
		18,446,744,073,709,551,616

// Weight of earth's crust
	(US tons)
	26,124,700,000,000,000,000,000
	(US kilotons)
		26,124,700,000,000,000,000

// Amount of iron on earth
	(US tons)
	   522,495,000,000,000,000,000
	(US kilotons)
		   522,495,000,000,000,000

// Contrived amount of base metals on earth (crust weight / 10).
	(US tons)
	 2,612,470,000,000,000,000,000
	(US kilotons)
		 2,612,470,000,000,000,000

*/

/*
This struct represents a surface deposit of some mineable resource on a HabitablePlanet.
*/
struct SurfaceDeposit {
	uint_least64_t quantity;
	uint_least8_t xPos[NUM_DEPOSIT_COORDS]; // 255 connotes no tile.
	uint_least8_t yPos[NUM_DEPOSIT_COORDS]; // 255 connotes no tile.
	uint_least8_t good; // 255 no deposit.

	// One byte before 8 byte alignment

};

/*
This struct represents a deep deposit of some mineable resource on a HabitablePlanet.
// TODO implement
*/
struct HabitableDeepDeposit {
	uint_least64_t quantity;
	uint_least8_t good;

};

/*
This struct represents the core composition of a HabitablePlanet.
// TODO implement
*/
struct HabitableCoreDeposit {
	uint_least8_t good;
	uint_least64_t quantity;

};

/*
This function gets the deposit sprite corresponding to a deposit.
*/
SDL_Rect getDepositSprite(int good) {

	// Gets the appropriate sprite for the inputed deposit.
	switch (good) {
	case(BaseMetals):
		return {0 * SPRITE_SIZE, 5 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};

	}
}