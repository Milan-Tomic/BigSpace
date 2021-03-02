#pragma once

// Macro for accessing a universe tile.
#define uIndex(x, y) universe[index(x, y, universeWidth)]

// Macro for accessing a universe tile's closure.
#define uClosure(x, y) allClosures[universe[index(x, y, universeWidth)].closure]

// Macro for accessing a unvierse tile's owner.
#define uOwner(x, y) allClosures[universe[index(x, y, universeWidth)].closure].owner

// Macro for accessing a universe tile's system.
#define uSystem(x, y) (universe[index(x, y, universeWidth)].system)

// Macro for accessing a planet tile.
#define pIndex(x, y, plan) plan->planet[index(x, y, plan->size)]

// Macro for accessing a PlanetTile tileData.
#define pTileID(x, y, plan) plan->planet[index(x, y, plan->size)].tileData

// Macro for accessing a planet tile's buildingID.
#define pBuilding(x, y, plan) plan->planet[index(x, y, plan->size)].buildingID

// Macro for accessing a planet tile's owner.
#define poIndex(x, y, plan) plan->planet[index(x, y, plan->size)].owner

// Macro for accessing an owner associated with a planet tile.
#define pOwner(x, y, plan) plan->owners[plan->planet[index(x, y, plan->size)].owner]

// Macro for accessing an owner's buildings.
#define pOwnerBuilding(owner, build, plan) plan->owners[owner].ownedBuildings[build]

// Macro for accessing an owner's building quantity.
#define pOwnerBuildingQ(owner, build, plan, lvl) plan->owners[owner].ownedBuildings[build].numBuildings[lvl]

// Macro function for accessing a battle tile from a planet.
#define pBattleIndex(x, y, planet) planet->battle->battlefield[index(x, y, planet->size)]

// Background colour.
#define BACKGROUND_R 180
#define BACKGROUND_G 180
#define BACKGROUND_B 180

// Total number of HabitablePlanets in the universe.
int numHabitablePlanets;

// Total number of BarrenPlanets in the universe.
int numBarrenPlanets;

// Globals.

// Stores the dimensions of the gameplay window.
int screenWidth = 512;
int screenHeight = 512;

// Coordinates (measured in Tiles) within the universe viewport.
int universeX;
int universeY;

// universe width and height.
int universeWidth;
int universeHeight;

// Typedefs and enums.

// Gases that a planet may store.
enum Gas {
	Oxygen,
	Methane,
	Null_Gases,
	Pollutants,
	Poisons,
	NUM_GASES

};

// Array relating Gases to their names.
std::string gasNames[] = {
	"Oxygen",
	"Methane",
	"Null Gases",
	"Pollutants",
	"Poisons"

};


// Enum listing all goods.
enum Goods {
	BaseMetals,
	NUM_GOODS,
	Veg = NUM_GOODS,
	Meat,
	NUM_GOODS_ALL

};

// Array relating Goods to their names.
std::string goodsNames[] = {
	"Base Metals"

};

// Names of all values used for the HabitableInfoPanel.
static const std::string planetInfoNames[] = {
		"Size", "Temperature", "Distance"

};

// Names of all values used for the SystemInfoPanel.
static const std::string systemInfoNames[] = {
	"Star Size", "Star Luminosity", "Star Type", "Num Habitable", "Num Barren"

};

// Minimum size of a planet.
#define MIN_PLANET_WIDTH 20

// Maximum size of a planet.
#define MAX_PLANET_WIDTH 256

// Minimum number of HabitablePlanets and BarrenPlanets for a System.
#define MIN_HABITABLE 1
#define MIN_BARREN 1

// Width of Earth.
#define EARTH_WIDTH 70

// Number of targets in each target queue.
#define NUM_TARGETS 16

// Number of river slots per planet.
#define NUM_RIVERS 64

// Number of river tiles per river slot.
#define NUM_RIVER_TILES 13

// Maximum of deposits on the surface of a planet.
#define NUM_DEPOSITS_SURFACE 16

// Number of deposit coordinates for each surface deposit.
#define NUM_DEPOSIT_COORDS 15

// Possible buildings. Includes NoBuilding. Does not include Farm, Pasture, or City.
#define NUM_BUILDINGS 11

// Number of Owners on a HabitablePlanet.
#define NUM_HABITABLE_OWNERS 20

// Number of Owners on a BarrenPlanet.
#define NUM_BARREN_OWNERS 6

// Inputs, outputs, etc. for an owner.
#define NUM_OWNER_VALS 4

// Size of a GalaxyTile or PlanetTile sprite.
#define SPRITE_SIZE 64

// Size of a GroundUnit or Ship sprite.
#define SMALL_SPRITE_SIZE 16

// Defs for PlanetTile heat. Items <= defs are in that bound.
#define PERMAFROST_TEMP -1
#define COLD_TEMP 4
#define TEMPERATE_TEMP 8
#define HOT_TEMP 15 // Above hot is scorched.