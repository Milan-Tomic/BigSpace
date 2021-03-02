#pragma once

// Defines damage types.
enum {
	KINETIC,
	LASER,
	EXPLOSIVE,
	RADIOACTIVE,
	NUM_DAMAGE_TYPES

};

/*
Template for a Ship. Contains important information for the Ship.

12 bytes
*/
struct ShipTemplate {

	// Damage levels of the Ship.
	uint_least8_t damage[NUM_DAMAGE_TYPES]; // 4 bytes

	// Armor levels of the Ship.
	uint_least8_t armor[NUM_DAMAGE_TYPES]; // 4 bytes

	// Dodge of the Ship.
	uint_least8_t dodge; // 1 byte

	// Accuracy of the Ship.
	uint_least8_t accuracy; // 1 byte

	// Speed of the Ship.
	uint_least8_t speed; // 1 byte

	// Sprite of the Ship.
	uint_least8_t sprite;

	// No bytes padding

};

/*
Stores a Ship type for a Fleet.

16 bytes
*/
struct Ship {
	ShipTemplate type; // 12 bytes
	uint_least16_t num; // 2 bytes
	uint_least16_t desired; // 2 bytes

	// No bytes padding

};

/*
Stores a Fleet. Fleets are composed of Ships.

TODO include commanding officers.

24 bytes
*/
class Fleet {
public:

	// Owner of this Fleet.
	Government* owner; // 8 bytes.

	// Fleets can have any number of Ships.
	Ship* ships; // 8 bytes
	uint_least16_t numShips; // 2 bytes

	// Estimated strength of the Fleet.
	uint_least16_t strength; // 2 bytes

	// 4 bytes padding

	// Adds some Ships to the Fleet.
	void addShip(ShipTemplate* ship, uint_least16_t numShips);


};

/*
Adds a Ship to a Fleet.

TODO add ships to existing templates.
*/
void Fleet::addShip(ShipTemplate* ship, uint_least16_t add) {

	// Array size will be incremented by 8.
	const int inc = 8;

	// If ships is empty, initializes it.
	if (!numShips) ships = (Ship*)malloc(sizeof(Ship) * inc);
	// If ships is too small, resizes it.
	else if (!(numShips % inc)) ships = (Ship*)realloc(ships, sizeof(Ship) * (numShips + inc));

	// Adds the Ship to the Fleet.
	ships[numShips] = { *ship, add, add };
	++numShips;

}