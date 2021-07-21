#pragma once

// Enum containing all PlanetTile types.
enum PlanetTileID {
	Tundra,
	Scrubland,
	Desert,
	Cold_Steppe,
	Temperate_Steppe,
	Dry_Savannah,
	Savannah,
	Cold_Grassland,
	Temperate_Grassland,
	Hot_Grassland,
	Cold_Wetland,
	Temperate_Wetland,
	Hot_Wetland,
	Permafrost,
	Barren,
	Seared,
	Mountain,
	// <= is land, > is water.
	LAND_TILE = Mountain,
	Lake,
	Coast,
	Ocean,
	Ice

};

/*
Function that maps PlanetTileID to planet tile name.
*/
const char* getPlanetTileName(PlanetTileID id) {
	switch (id) {
	case (Tundra):
		return "Tundra";
	case (Scrubland):
		return "Scrubland";
	case (Desert):
		return "Desert";
	case (Cold_Steppe):
		return "Cold Steppe";
	case (Temperate_Steppe):
		return "Temperate Steppe";
	case (Dry_Savannah):
		return "Dry Savannah";
	case (Savannah):
		return "Savannah";
	case (Cold_Grassland):
		return "Cold Grassland";
	case (Temperate_Grassland):
		return "Temperate Grassland";
	case (Hot_Grassland):
		return "Hot Grassland";
	case (Cold_Wetland):
		return "Cold Wetland";
	case (Temperate_Wetland):
		return "Temperate Wetland";
	case (Hot_Wetland):
		return "Hot Wetland";
	case (Permafrost):
		return "Permafrost";
	case (Barren):
		return "Barren";
	case (Seared):
		return "Seared";
	case (Mountain):
		return "Mountain";
	case (Lake):
		return "Lake";
	case (Coast):
		return "Coast";
	case (Ocean):
		return "Ocean";
	case (Ice):
		return "Ice";

	}
}


/*
Object representing a tile on a planet. The tile has (implicit) heat and moisture values which
determine characteristics and habitability for the tile.
*/
class PlanetTile {
public:

	// Type of tile. Represents heat, moisture, and sprite.
	uint_least8_t tileData : 5;

	// Level of this PlanetTile's way.
	uint_least8_t wayLevel : 3;

	// Direction of this PlanetTile's way.
	Direction wayDir;

	// Owner of this tile.
	uint_least8_t owner;

	// This PlanetTile's building.
	uint_least8_t buildingID;

	// Numerical data for the building.
	// Stores number of workers, number of inhabitants, harbour direction.
	uint_least8_t numData : 7;

	// Deposit marker for this tile. Stored with numData sicne it is accessed
	// infrequently.
	uint_least8_t deposit : 1;

	// Unused default constructor for a PlanetTile.
	PlanetTile();

	// Constructor for a PlanetTile used when initializing a planet.
	PlanetTile(int tileID);

	// Standard deconstructor for a PlanetTile.
	~PlanetTile();

	// Returns this tile's sprite.
	SDL_Rect getSprite();

	// Return's this tile's building's sprite,
	SDL_Rect getBuildingSprite();

};

// Unused default constructor for a PlanetTile.
PlanetTile::PlanetTile() {
	tileData = 0;

}

/*
Standard constructor for a PlanetTile. Initializes the tileID for the type of PlanetTile. Also initializes
heat and moisture.
*/
PlanetTile::PlanetTile(int tileID) {
	this->tileData = tileID;

}

/*
Standard deconstructor for a PlanetTile.
*/
PlanetTile::~PlanetTile() {

}

/*
Returns the appropriate sprite for a PlanetTile. Does so via switch to minimize the amount
of SDL_Rects stored.
*/
SDL_Rect PlanetTile::getSprite() {
	switch (tileData) {

	case(Tundra):
		return { 0, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Scrubland):
		return { SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Desert):
		return { 2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Cold_Steppe):
	case(Temperate_Steppe):
		return { 3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Dry_Savannah):
	case(Savannah):
		return { 4 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Cold_Grassland):
	case(Temperate_Grassland):
	case(Hot_Grassland):
		return { 5 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Cold_Wetland):
	case(Temperate_Wetland):
	case(Hot_Wetland):
		return { 6 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Permafrost):
		return { 7 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Barren):
		return { 8 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Seared):
		return { 9 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Mountain):
		return { 10 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Lake):
		return { SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Coast):
		return { 2 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Ocean):
		return { 3 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };

	}
}

/*
Returns the appropriate sprite for a Building. Near identical to getSprite.
*/
SDL_Rect PlanetTile::getBuildingSprite() {

	// Determines sprite based on the building.
	switch (buildingID) {

	// No building.
	case(NoBuilding):
		return {0, 0, 0, 0};
	// Forests.
	case(BorealForest):
		return {0 * SPRITE_SIZE, 15 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(DeciduousForest):
		return {3 * SPRITE_SIZE, 15 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(TemperateForest):
		return {6 * SPRITE_SIZE, 15 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(AridForest):
		return {9 * SPRITE_SIZE, 15 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(TropicalForest):
		return {12 * SPRITE_SIZE, 15 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	// Harbours.
	case(Harbour1):
		return { 0 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Harbour2):
		return { 1 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Harbour3):
		return { 2 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Harbour4):
		return { 3 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Harbour5):
		return { 4 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Mines.
	case(Mine1):
		return { 0 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Mine2):
		return { 1 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Mine3):
		return { 2 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Mine4):
		return { 3 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(Mine5):
		return { 4 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Cities.
	case(City1):
		return { 0 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(City2):
		return { 1 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(City3):
		return { 2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(City4):
		return { 3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	case(City5):
		return { 4 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Farms.
	case(Farm1):
		return {0, 0, SPRITE_SIZE, SPRITE_SIZE};
	case(Farm2):
		return {1 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE};
	case(Farm3):
		return {2 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE};
	case(Farm4):
		return {3 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE};
	case(Farm5):
		return {4 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE};
	// Pastures.
	case(Pasture1):
		return {0, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(Pasture2):
		return {1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(Pasture3):
		return {2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(Pasture4):
		return {3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(Pasture5):
		return {4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	// Orc Buildings.
	case(OrcFarm):
		return {7 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE};
	case(OrcMine):
		return {7 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	case(OrcCity):
		return {7 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
	// Error.
	default:
		return {0, 0, 0, 0};

	}
}

/*
A list of Objects representing the stars, gas giants, moons, planets, etc. of a star system.
*/

/*
IDs for system elements.
*/
enum StarType {
	MStar,
	KStar,
	GStar,
	FStar,
	AStar,
	NUM_STAR_TYPES

};

/*
Object within a system representing a star.
Six bytes, two until 8 byte alignment.

Stores size and tileID for rendering.
Stores luminosity for calculating planet heats.
*/
class Star {
public:

	// Luminosity of the star.
	float luminosity;

	// StarType of the star.
	uint_least8_t tileID;

	// Size of star (in pixels).
	uint_least8_t size;

	// NOTE: Can store two more bytes.

	// No argument constructor for a Star.
	Star();

	// Default constructor for a Star. Assigns area, tileID, and luminosity.
	Star(int tileID, float radius, float luminosity);

	// Returns the sprite for a Star based on its tileID.
	SDL_Rect getSprite();

};

/*
Unused no argument constructor for a Star.
*/
Star::Star() {

}

// The width of the sun, in pixels.
#define SUN_WIDTH 100

/*
Default constructor for a Star. Assigns tileID to the inputed tileID,
luminosity to the inputed luminosity, and size to SUN_WIDTH * radius.
*/
Star::Star(int tileID, float radius, float luminosity) {
	this->tileID = tileID;
	this->luminosity = luminosity;
	size = (int)(SUN_WIDTH * radius);

}

/*
Returns the sprite for a Star based on its tileID.
*/
SDL_Rect Star::getSprite() {

	// Returns the Star's sprite.
	switch (tileID) {

	// Returns the sprite for a red star.
	case (MStar):
		return { 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Returns the sprite for an orange star.
	case (KStar):
		return { 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Returns the sprite for a sun-like star.
	case (GStar):
		return { 5 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Returns the sprite for a white star.
	case (FStar):
		return { 7 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	// Returns the sprite for a blue star.
	case (AStar):
		return { 8 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };

	}
}

/*
Wraps coordinates around a planet's ice caps and horizontal edges.
TODO consider making this a method of HabitablePlanet.
*/
inline void wrapAroundPlanet(int size, int* xPos, int* yPos) {

	// Wraps over the north pole.
	if (*yPos < 0) {
		*xPos += size / 2;
		*yPos = 0;

	}
	// Wraps over the south pole.
	else if (*yPos >= size) {
		*xPos += size / 2;
		*yPos = size - 1;

	}

	// Wraps around the right and left edges.
	while (*xPos >= size) *xPos -= size;
	while (*xPos < 0) *xPos += size;

}

// Tells the compiler about Battles.
extern struct Battle;

/*
Tile within a System that contains a solid, habitable celestial body.
Size: 3048bytes
*/
class HabitablePlanet {
public:

	// Stores property owners on the planet.
	Owner owners[NUM_HABITABLE_OWNERS];

	// Stores rivers.
	River rivers[NUM_RIVERS];

	// Stores some surface deposits.
	SurfaceDeposit surfaceDeposits[NUM_DEPOSITS_SURFACE];

	// Stores gases.
	// Gases: Air, methane, null gases, atmospheric pollutants, atmospheric poisons.
	uint_fast64_t gases[NUM_GASES];

	// Contains the planet in a two dimensional array of PlanetTiles.
	PlanetTile* planet;

	// Contains a battlefield associated with the planet.
	Battle* battle;

	// Contains the location of this planet in the universe.
	CoordU loc;

	// Contains a mutex that prevents shared access to the planet.
	// This mutex is used by battles too.
	std::shared_mutex mutex;
	
	// Stores net gas production.
	int_fast32_t netGases[NUM_GASES];

	// Heat multiple of the planet. Saved to reduce temperature calcs.
	float heatMultiple;

	// Temperature of the planet in Kelvin degrees.
	// I think this is the radiative equilibrium temperature.
	int temperature;

	// Distance of the planet to its parent star.
	int distance;

	// Size of the planet in tiles.
	uint_least8_t size;

	// No argument constructor for a HabitablePlanet.
	HabitablePlanet();

	// Standard constructor for a HabitablePlanet.
	HabitablePlanet(int size, float heatMultiple, int temperature);

	// Deconstructor for a HabitablePlanet.
	~HabitablePlanet();

	// Returns the sprite for a tile. Does so by rasterizing the planet's tiles onto a circle.
	SDL_Texture* getPlanetTexture();

	// Finds the next available owner on the HabitablePlanet.
	uint_least8_t nextOwner();

	// Parses through the planet's owners and adds their goods to the market.
	void ownersProduction();

	// Function that determines if there exists an adjacent tile matching the inputed condition.
	bool checkAdjacency(int x, int y, std::function<bool(int, int, PlanetTile)> condition);

	// Function that pathfinds to the nearest tile matching the inputed condition.
	// When the tile is found, performs the inputed action.
	// It also requires functions for calculating the gValue between two tiles
	// and the hValue of a given tile.
	bool planetPathfind(int x1, int y1, int maxG,
		std::function<bool(HabitablePlanet*, LinkedListNode<PathCoordinate>*)> endCondition,
		std::function<void(HabitablePlanet*, LinkedListNode<PathCoordinate>*)> action,
		std::function<int(HabitablePlanet*, int, int, int, int)> calcG,
		std::function<double(HabitablePlanet*, LinkedListNode<PathCoordinate>*)> calcH);

};

/*
Unused no argument constructor for a HabitablePlanet.
*/
HabitablePlanet::HabitablePlanet() {

}

/*
Standard constructor for a HabitablePlanet. Initializes size, heatMultiple, heat, and planet.
*/
HabitablePlanet::HabitablePlanet(int size, float heatMultiple, int temperature) {
	this->size = size;
	this->heatMultiple = heatMultiple;
	this->temperature = temperature;
	this->distance = 0;

}

/*
Deconstructor for a HabitablePlanet. Does nothing.
*/
HabitablePlanet::~HabitablePlanet() {

}

/*
Returns a texture corresponding to this planet's tiles. Does so by rasterizing the planet's tiles
to a circle inside of a texture.
*/
SDL_Texture* HabitablePlanet::getPlanetTexture() {
	SDL_Rect temp;
	SDL_Rect sprite;

	// Creates a texture to store the planet's appearance in and sets it as the render target.
	SDL_Texture* planetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size, size);
	SDL_SetRenderTarget(renderer, planetTexture);

	// Rasterizes the planet's tiles (left side) to a circle in planetTexture.
	for (double i = 0; i < 2 * M_PI; i += 0.01) {
		for (int j = 0; j < size; ++j) {
			temp = { (int)((j / 2) * sin(i) + size / 2), (int)((j / 2) * cos(i) + size / 2), 1, 1 };
			sprite = planet[index((int)((j / 4) * sin(i)) + size / 4, (int)((j / 2) * cos(i)) + size / 2, size)].getSprite();
			SDL_RenderCopy(renderer, tileSheet, &sprite, &temp);

		}
	}

	// Renders the planet to planetTexture.
	SDL_RenderPresent(renderer);

	// Reassigns the render target to window.
	SDL_SetRenderTarget(renderer, NULL);

	// Returns a pointer to the new texture.
	return planetTexture;

}

/*
Finds the next available owner on the HabitablePlanet.
*/
uint_least8_t HabitablePlanet::nextOwner() {
	for (int i = 1; i < NUM_HABITABLE_OWNERS; ++i)
		if (owners[i].colony == nullptr) return i;

}

// Tells the compiler about buildingProductionFuncs.
extern void (*buildingProductionFuncs[])(HabitablePlanet* planet, Owner* owner, void* extraData);

/*
Parses through the planet's owners and adds their goods to the market.
// TODO this should be more complex, should manage inventories etc.
// TODO pay for the chosen fraction of inputs.
// TODO check to see if profitable, flag unprofitable industry.
*/
void HabitablePlanet::ownersProduction() {

	// Manages production for each owner (excluding unowned).
	for (int currOwner = 1; currOwner < NUM_HABITABLE_OWNERS && this->owners[currOwner].owner != nullptr; ++currOwner) {

		// Manages production for each building (other than owned land).
		for (int currBuilding = 1; currBuilding < NUM_BUILDING_TYPES; ++currBuilding) {
			buildingProductionFuncs[currBuilding](this, &this->owners[currOwner], nullptr);

		}
	}
}

/*
Determines if there exists any adjacent tile matching the inputed condition.
*/
bool HabitablePlanet::checkAdjacency(int x, int y, std::function<bool(int, int, PlanetTile)> condition) {
	int xPos, yPos;

	// Checks all adjacent tiles.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(size, &xPos, &yPos);
			
			// If the condition is met by any tile, returns true.
			if (condition(xPos, yPos, planet[index(xPos, yPos, size)])) return true;

		}
	}

	// Returns false if no tile meets the condition.
	return false;

}

/*
TODO header comment.

Return false if and only if pathfinding fails, otherwise returns true.
*/
bool HabitablePlanet::planetPathfind(int x1, int y1, int maxG,
	std::function<bool (HabitablePlanet*, LinkedListNode<PathCoordinate>*)> endCondition,
	std::function<void (HabitablePlanet*, LinkedListNode<PathCoordinate>*)> action,
	std::function<int(HabitablePlanet*, int, int, int, int)> calcG,
	std::function<double(HabitablePlanet*, LinkedListNode<PathCoordinate>*)> calcH) {
	bool ret = false;
	int gValue;
	int xPos, yPos;
	LinkedListNode<PathCoordinate>* currPos = nullptr;
	LinkedListNode<PathCoordinate>* coord;

	// Uses a map to mark down explored tiles.
	uint_least8_t* exploredMap = requestDummy();
	memset(exploredMap, 0, sizeof(uint_least8_t) * size * size);

	// Requests a coordList for this function.
	LinkedList<PathCoordinate>* coordList = requestCoordList();

	// Creates a LinkedList for storing the frontier.
	LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();

	// Creates a LinkedList for storing explored.
	LinkedList<PathCoordinate> explored = LinkedList<PathCoordinate>();

	// Adds the initial tile to the frontier.
	coord = coordList->popc();
	coord->value->coord.x = x1;
	coord->value->coord.y = y1;
	coord->value->gValue = 0;
	coord->value->hValue = calcH(this, coord);
	coord->previous = nullptr;
	frontier.addc(coord);
	exploredMap[index(coord->value->coord.x, coord->value->coord.y, size)] = true;

	// Pathfinds until the desired coordinate is found.
	while (!frontier.isEmpty()) {

		// Starts exploring the node.
		currPos = frontier.popc();
		explored.addc(currPos);

		// If currPos meets the inputed criteria, stops looping.
		if (endCondition(this, currPos)) {
			action(this, currPos);
			ret = true;
			break;

		}

		// Adds all adjacent tiles to the frontier.
		// This is slower than the old code, so it is not used.
		//actionDirection(currPos->value->coord.x, currPos->value->coord.y,
		//	[this, &exploredMap, &coordList, &frontier, currPos, calcG, calcH, maxG](int x, int y, Dir d)->void {
		//		int gValue;
		//		LinkedListNode<PathCoordinate>* coord;

		//		// Places the coord if it has not been explored.
		//		wrapAroundPlanet(size, &x, &y);
		//		if (!exploredMap[index(x, y, size)]) {

		//			// Marks this tile explored.
		//			exploredMap[index(x, y, size)] = true;

		//			// Selects the gValue.
		//			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, x, y);

		//			// Adds the coord to the frontier if it has an appropriate gValue.
		//			if (gValue < maxG) {

		//				// Places the coord.
		//				coord = coordList->popc();
		//				coord->value->coord.x = x;
		//				coord->value->coord.y = y;
		//				coord->value->gValue = gValue;
		//				coord->value->hValue = calcH(this, coord);
		//				coord->previous = currPos;
		//				coord->value->dir.d = d;
		//				frontier.addcAsc(coord);

		//			}
		//		}
		//	}
		//);
		
		// Old pathfinding code.
		// This is consistently slightly faster than the new code, so it is kept.
		{

		//Adds all adjacent tiles to the frontier.
		//This is done verbosely because it allows for expression of direction.
		//Northwest.
		xPos = currPos->value->coord.x - 1;
		yPos = currPos->value->coord.y - 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = northwest;
				frontier.addcAsc(coord);

			}
		}
		// North.
		xPos = currPos->value->coord.x;
		yPos = currPos->value->coord.y - 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = north;
				frontier.addcAsc(coord);

			}
		}
		// Northeast.
		xPos = currPos->value->coord.x + 1;
		yPos = currPos->value->coord.y - 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = northeast;
				frontier.addcAsc(coord);

			}
		}
		// East.
		xPos = currPos->value->coord.x + 1;
		yPos = currPos->value->coord.y;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = east;
				frontier.addcAsc(coord);

			}
		}
		// West.
		xPos = currPos->value->coord.x - 1;
		yPos = currPos->value->coord.y;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = west;
				frontier.addcAsc(coord);

			}
		}
		// Southeast.
		xPos = currPos->value->coord.x + 1;
		yPos = currPos->value->coord.y + 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = southeast;
				frontier.addcAsc(coord);

			}
		}
		// South.
		xPos = currPos->value->coord.x;
		yPos = currPos->value->coord.y + 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = south;
				frontier.addcAsc(coord);

			}
		}
		// Southwest.
		xPos = currPos->value->coord.x - 1;
		yPos = currPos->value->coord.y + 1;
		wrapAroundPlanet(size, &xPos, &yPos);
		// Places the coord if it has not been explored.
		if (!exploredMap[index(xPos, yPos, size)]) {

			// Marks this tile explored.
			exploredMap[index(xPos, yPos, size)] = true;

			// Selects the gValue.
			gValue = currPos->value->gValue + calcG(this, currPos->value->coord.x, currPos->value->coord.y, xPos, yPos);

			// Adds the coord to the frontier if it has an appropriate gValue.
			if (gValue < maxG) {

				// Places the coord.
				coord = coordList->popc();
				coord->value->coord.x = xPos;
				coord->value->coord.y = yPos;
				coord->value->gValue = gValue;
				coord->value->hValue = calcH(this, coord);
				coord->previous = currPos;
				coord->value->dir.d = southwest;
				frontier.addcAsc(coord);

			}
		}
	}
	}

	// Returns values from frontier and explored to coordList.
	while (!frontier.isEmpty()) coordList->addc(frontier.popc());
	while (!explored.isEmpty()) coordList->addc(explored.popc());

	// Releases exploredMap.
	releaseDummy(exploredMap);

	// Releases the coordList.
	releaseCoordList(coordList);

	// Returns true if the action lambda was performed.
	return ret;

}

/*
Tile within a system that contains a solid, uninhabitable celestial body.
This includes moons and asteroids.
*/
class BarrenPlanet {
public:

	// Stores property owners on the planet.
	Owner owners[NUM_BARREN_OWNERS];

	// Heat multiple of the body.
	float heatMultiple;

	// Temperature of the planet in C.
	int temperature;

	// Distance of the planet to its parent star.
	int distance;

	// Size of the body.
	uint_least8_t size;

	// No argument constructor for a BarrenPlanet.
	BarrenPlanet();

	// Standard constructor for a BarrenPlanet.
	BarrenPlanet(int size);

	// Used within placeBarren to more efficiently modify the items within BarrenPlanet.
	void placeInit(int size);

	// Returns the dominant tile sprite for this planet.
	SDL_Rect getSprite();

	// Returns the texture for a tile. Does so by rasterizing the planet's tiles onto a circle.
	SDL_Texture* getPlanetTexture();

};

/*
Unused no argument constructor for a SystemBarrentTile.
*/
BarrenPlanet::BarrenPlanet() : size(0), heatMultiple(0), temperature(0) { }

/*
Standard constructor for a BarrenPlanet. Initializes size, heatMultiple, and temperature.
*/
BarrenPlanet::BarrenPlanet(int size) : size(size), heatMultiple(0), temperature(0) { }

/*
Used within placeBarren to more efficiently modify the items within BarrenPlanet.
*/
void BarrenPlanet::placeInit(int size) {
	this->size = size;

}

/*
Returns the dominant tile sprite for this planet.
*/
SDL_Rect BarrenPlanet::getSprite() {

	// Chooses the appropriate tile sprite for this planet based on its heat.
	if (temperature > 400) return { 9 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	else if (temperature < 250) return { 7 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };
	else return { 8 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE };

}

/*
Returns a texture corresponding to this planet's tiles. Does so by rasterizing the planet's tiles
to a circle inside of a texture.
*/
SDL_Texture* BarrenPlanet::getPlanetTexture() {

	// Creates a texture to store the planet's appearance in and sets it as the render target.
	SDL_Texture* planetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size, size);
	SDL_SetRenderTarget(renderer, planetTexture);

	// Gets tile sprite for this planet based on its heat.
	SDL_Rect sprite = this->getSprite();

	// Creates a circle of the planet's dominant tile sprite in planetTexture.
	for (double i = 0; i < 2 * M_PI; i += 0.01) {
		for (int j = 0; j < size; ++j) {
			SDL_Rect temp = { (j / 2) * sin(i) + size / 2, (j / 2) * cos(i) + size / 2, 1, 1 };
			SDL_RenderCopy(renderer, tileSheet, &sprite, &temp);

		}
	}

	// Renders the planet to planetTexture.
	SDL_RenderPresent(renderer);

	// Reassigns the render target to window.
	SDL_SetRenderTarget(renderer, NULL);

	// Returns a pointer to the new texture.
	return planetTexture;

}

/*
This header contains the classes for all Tiles in the Galaxy.
*/

// Enum representing types of GalaxyTile.
enum GalaxyTileID {
	SYSTEM_TILE,
	THICK_SPACE_TILE,
	THIN_SPACE_TILE

};

// Informs the compiler about Squadronss and Government.
extern struct Squadron;
extern class Government;

/*
A tile containing a System.

Contains stars (1-4), HabitablePlanets, and BarrenPlanets.

63  bytes, 1 byte of padding.
sizeof is 64.
*/
class System {
public:

	// Stars and Planets have locations, which allow them to be clicked and rendered.
	// There is no array of tiles like with planets and the universe.

	// Array of Stars.
	// Note: Can contain 1-4 stars.
	Star stars[4]; // 32 bytes.

	// Array of SystemPlanetTiles.
	HabitablePlanet** planets; // 8 bytes

	// Array of SystemBarrenTiles.
	BarrenPlanet** barrens; // 8 bytes.

	// Array of paths between this system and nearby systems.
	// Paths are composed of offsets from this system rather than being literal.
	// The first Coord.x in any path stores the length of the path (use to iterate).
	// The first Coord.y in any path stores metadata (whether the trade lane is open?).
	// The final Coord in the list will always be (0, 0).
	CoordI* tradeLanes; // 8 bytes.

	// Location of this System.
	CoordU loc; // 4 bytes.

	// Number of stars.
	uint_least8_t numStars; // 1 byte.

	// Number of planets.
	uint_least8_t numHabitable; // 1 byte.

	// Number of barrens.
	uint_least8_t numBarren; // 1 byte.

	// 1 bytes padding.

	// Unused no argument constructor for a SystemSpaceTile.
	System();

	// Default constructor for this SystemSpaceTile. Initializes stars and planets.
	System(int numStars, int numPlanets);

	// Deconstructor for this SystemSpaceTile. Deletes stars and planets.
	~System();

	// Initializes the stars array.
	void initStars(int numStars);

	// Initializes the planets array.
	void initHabitable(int numHabitable);

	// Initializes the barrens array.
	void initBarren(int numBarren);

	// Initializes trade lanes for this System.
	void initTradeLanes(int maxLength);

};

/*
Array storing travel times associated with each tile type.
*/
int universeMovementCost[] = {
	128, // SYSTEM_TILE
	128, // THICK_SPACE_TILE
	128  // THIN_SPACE TILE
};

/*
Array storing sensor difficulty associated with each tile type.
*/
int universeSensorDifficulty[] = {
	128, // SYSTEM_TILE
	128, // THICK_SPACE_TILE
	32   // THIN_SPACE_TILE
};

/*
Parent for any given GalaxyTile.

28 bytes, 4 bytes padding.
sizeof is 32.
*/
class GalaxyTile {
public:

	// Contains the System of this GalaxyTile if there is one.
	System* system; // 8 bytes.

	// Stores the Squadrons in this GalaxyTile.
	Squadron** squadrons; // 8 bytes.

	// Stores the de-jure owner of this GalaxyTile.
	Government* owner; // 8 bytes.

	// Stores the closure of this GalaxyTile.
	uint_least16_t closure; // 2 bytes.

	// Stores the number of Squadrons in this GalaxyTile.
	uint_least8_t numSquadrons; // 1 byte.

	// tileID for the GalaxyTile.
	uint_least8_t tileID; // 1 byte.

	// 4 bytes padding.

	// Gets the sprite and tileID for this tile.
	SDL_Rect getSprite();
	GalaxyTileID getTileID();

	// Adds a Squadron to this GalaxyTile.
	void addSquadron(Squadron* squadron);

	// Removes a Squadron from this GalaxyTile.
	void removeSquadron(Squadron* squadron);

	// Gets movement difficulty for this GalaxyTile.
	inline int movementCost();

	// Gets sensor difficulty for this GalaxyTile.
	inline int sensorCost();

};

/*
Unused no argument constructor for a SystemSpaceTile.
*/
System::System() {
	numStars = 0;
	numHabitable = 0;
	numBarren = 0;
	planets = nullptr;

}

/*
Default constructor for a SystemSpaceTile.
*/
System::System(int numStars, int numPlanets) {
	this->numStars = numStars;
	this->numHabitable = numPlanets;
	planets = new HabitablePlanet * [numPlanets];

}

/*
Deconstructor for this SystemSpaceTile. Deletes stars and planets.
*/
System::~System() {
	delete[] planets;
	delete[] barrens;

}

/*
Initializes the stars array.
*/
void System::initStars(int numStars) {
	this->numStars = numStars;

}

/*
Initializes the planets array.
*/
void System::initHabitable(int numHabitable) {
	this->numHabitable = numHabitable;
	planets = new HabitablePlanet * [numHabitable];

}

/*
Initializes the barrens array.
*/
void System::initBarren(int numBarren) {
	this->numBarren = numBarren;
	barrens = new BarrenPlanet * [numBarren];

}

/*
Initializes tradeLanes for this System. Does so by pathfinding out from this
System until maxLength is reached, then storing the paths between this System
and all discovered Systems.

Paths will only ever touch this System and the target System. i.e. if the path
from A to C crosses B, there will exist paths from A to B and B to C but not
A to C. This may not be reciprocal in the case where finicky pathfinding causes
the path from C to A to not touch B.

TODO remove non-reciprocal relationships?
*/
void System::initTradeLanes(int maxLength) {
	LinkedListNode<PathCoordinate>* currPos;
	LinkedListNode<PathCoordinate>* checkPos;
	LinkedListNode<PathCoordinate>* coord;
	int tSize = 1;
	int tempSize = 0;
	int curr = 0;

	// Informs the compiler about the universe.
	extern GalaxyTile* universe;

	// Uses a map to mark down explored tiles.
	uint_least8_t* exploredMap = requestDummy();
	memset(exploredMap, 0, sizeof(uint_least8_t) * universeWidth * universeHeight);

	// Requests a coordList for this function.
	LinkedList<PathCoordinate>* coordList = requestCoordList();

	// Creates a LinkedList for storing the frontier.
	LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();

	// Creates a LinkedList for storing explored.
	LinkedList<PathCoordinate> explored = LinkedList<PathCoordinate>();

	// Adds the initial tile to the frontier.
	coord = coordList->popc();
	coord->value->coord.x = loc.x;
	coord->value->coord.y = loc.y;
	coord->value->gValue = 0;
	coord->previous = nullptr;
	coord->value->info = false;
	frontier.addc(coord);
	exploredMap[index(loc.x, loc.y, universeWidth)] = true;

	// Pathfinds until there are no available tiles.
	while (!frontier.isEmpty()) {

		// Starts exploring the first node in the frontier.
		currPos = frontier.popc();
		explored.addc(currPos);

		// Adds all adjacent tiles to the frontier.
		actionDirection(currPos->value->coord.x, currPos->value->coord.y,
			[this, &exploredMap, &coordList, &frontier, currPos, maxLength](int x, int y)->void {
				int gValue;
				LinkedListNode<PathCoordinate>* coord;

				// Places the coord if it has not been explored.
				if (!exploredMap[index(x, y, universeWidth)] && uIndex(x, y).tileID != THIN_SPACE_TILE) {

					// Marks this tile explored.
					exploredMap[index(x, y, universeWidth)] = true;

					// Selects the gValue.
					gValue = currPos->value->gValue + 1;

					// Adds the coord to the frontier if it has an appropriate gValue.
					if (gValue < maxLength) {

						// Places the coord.
						coord = coordList->popc();
						coord->value->coord.x = x;
						coord->value->coord.y = y;
						coord->value->gValue = gValue;
						coord->previous = currPos;

						// Notes whether the Coord contains a System.
						coord->value->info = uSystem(x, y) != nullptr;

						// Adds the coord to the frontier for exploration.
						frontier.addc(coord);

					}
				}
			}
		);
	}

	// Ensures that there are no redundant paths. Does so by invalidating paths
	// that contain a system en-route to them.
	// The number of tiles needed for tradeLanes is also calculated here.
	// Note that frontier is reused here to store checked nodes.
	while (!explored.isEmpty()) {
		checkPos = explored.popc();

		// If the node is a path destination, attempts to invalidate it.
		if (checkPos->value->info) {
			currPos = checkPos;
			tempSize = 0;

			// Checks to see if there is a System en-route to the destination.
			while (currPos = currPos->previous) {
				++tempSize;

				// Marks the node as an invalid path if it has a System en-route.
				if (currPos->value->info) {
					checkPos->value->info = false;
					tempSize = 0;
					break;

				}
			}

			// Tracks the appropriate size of tradeLanes.
			if (tempSize) tSize += tempSize + 1;

		}

		// Stores the checked node in frontier.
		frontier.addc(checkPos);

	}

	// Allocates memory for tradeLanes.
	tradeLanes = (CoordI*)malloc(tSize * sizeof(CoordI));

	// Stores all valid paths within tradeLanes.
	// Note that frontier and explored have again been inverted.
	while (!frontier.isEmpty()) {
		checkPos = frontier.popc();

		// If the node is a path destination, places the path within tradeLanes.
		if (checkPos->value->info) {
			currPos = checkPos;
			tempSize = 0;

			// Places the path.
			// This algorithm ends up placing 0,0 in tradeLanes[curr].
			do {
				tradeLanes[curr + (int)currPos->value->gValue].x = currPos->value->coord.x - loc.x;
				tradeLanes[curr + (int)currPos->value->gValue].y = currPos->value->coord.y - loc.y;
				++tempSize;

			} while (currPos = currPos->previous);

			// Places the path's length.
			tradeLanes[curr].x = tempSize - 1;

			// Offsets to the next path location within tradeLanes.
			curr += tempSize;

		}

		// Stores the checked node in explored.
		explored.addc(checkPos);

	}

	// Places 0, 0 at the end of tradeLanes to mark its end.
	tradeLanes[tSize - 1].x = 0;
	tradeLanes[tSize - 1].y = 0;

	// Returns all explored nodes to the coordList.
	while (!explored.isEmpty()) coordList->addc(explored.popc());

	// Releases held data.
	releaseCoordList(coordList);
	releaseDummy(exploredMap);

}

/*
Adds a Squadron to this GalaxyTile.
*/
void GalaxyTile::addSquadron(Squadron* squadron) {

	// Array size will be incremented by 8.
	const int inc = 8;

	// If squadrons is empty, initializes it.
	if (!numSquadrons) squadrons = (Squadron**)malloc(sizeof(Squadron*) * inc);
	// If squadrons is too small, resizes it.
	else if (!(numSquadrons % inc)) squadrons = (Squadron**)realloc(squadrons, sizeof(Squadron*) * (numSquadrons + inc));

	// Assigns the Squadron to the GalaxyTile.
	squadrons[numSquadrons] = squadron;
	++numSquadrons;

}

/*
Removes a Squadron from this GalaxyTile.
*/
void GalaxyTile::removeSquadron(Squadron* squadron) {

	// Finds the squadron.
	int curr = 0;
	for (; squadrons[curr] != squadron && curr < numSquadrons; ++curr);

	// Removes the squadron.
	--numSquadrons;
	for (int i = curr; i < numSquadrons; ++i) squadrons[i] = squadrons[i + 1];

	// Deletes squadrons if numSquadrons is 0.
	if (!numSquadrons) free(squadrons);

}

/*
Gets movement difficulty for this GalaxyTile.
*/
inline int GalaxyTile::movementCost() {
	return universeMovementCost[tileID];

}

/*
Gets sensor difficulty for this GalaxyTile.
*/
inline int GalaxyTile::sensorCost() {
	return universeSensorDifficulty[tileID];

}

/*
Returns the appropriate sprite for this tile.
*/
SDL_Rect GalaxyTile::getSprite() {
	switch (tileID) {
	case(SYSTEM_TILE):
		return { 2 * SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE };
	case(THICK_SPACE_TILE):
		return { SPRITE_SIZE, 0, SPRITE_SIZE, SPRITE_SIZE };
	case(THIN_SPACE_TILE):
		return { 0, 0, SPRITE_SIZE, SPRITE_SIZE };

	}
}

/*
Returns this tile's tileID.
*/
GalaxyTileID GalaxyTile::getTileID() { return (GalaxyTileID)tileID; }