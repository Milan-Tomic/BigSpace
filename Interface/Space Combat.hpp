#pragma once

// Defines damage types for ships.
enum {
	KINETIC,
	LASER,
	EXPLOSIVE,
	RADIOACTIVE,
	NUM_DAMAGE_TYPES

};

/*
Template for a Ship. Contains important information for the Ship.
Note that == should be updated if this struct is updated.

14 bytes
*/
struct ShipTemplate {

	// Damage levels of the Ship.
	uint_least8_t damage[NUM_DAMAGE_TYPES]; // 4 bytes.

	// Armor levels of the Ship.
	uint_least8_t armor[NUM_DAMAGE_TYPES]; // 4 bytes.

	// Dodge of the Ship.
	uint_least8_t dodge; // 1 byte.

	// Accuracy of the Ship.
	uint_least8_t accuracy; // 1 byte.

	// Range of the Ship.
	uint_least8_t range; // 1 byte.

	// Speed of the Ship.
	uint_least8_t speed; // 1 byte.

	// Scanner strength of the Ship.
	uint_least8_t scan; // 1 byte.

	// Sprite of the Ship.
	uint_least8_t sprite; // 1 byte.

	// No bytes padding

};

/*
Checks equivalency between two ShipTemplates.
*/
bool operator==(const ShipTemplate& first, const ShipTemplate& second) {
	return (*(uint_least64_t*)&first) == (*(uint_least64_t*)&second) && // Bytes 0-7.
		*((uint_least32_t*)&first + 2) == *((uint_least32_t*)&second + 2) && // Bytes 8-11.
		*((uint_least16_t*)&first + 6) == *((uint_least16_t*)&second + 6);  // Byte 12-13

}

/*
Stores a Ship type for a Squadron.

18 bytes
*/
struct Ship {
	ShipTemplate type; // 14 bytes
	uint_least16_t num; // 2 bytes
	uint_least16_t desired; // 2 bytes

};

/*
Stores a Squadron. Squadrons are composed of Ships.

TODO include commanding officers.

72 bytes, 0 bytes padding.
sizeof is 72 bytes.
*/
class Squadron {
public:

	// Threat queue of the Squadron.
	std::vector<Threat> threats; // 32 bytes.

	// Miscellaneous data for this Squadron.
	// Paths are accessed via getPath();
	void* data;

	// Owner of this Squadron.
	Government* owner; // 8 bytes.

	// Behaviour of this Squadron.
	void (*behaviour)(Squadron* squadron); // 8 bytes.

	// Squadrons can have any number of Ships.
	Ship* ships; // 8 bytes

	// Estimated strength of the Squadron.
	uint_least16_t strength; // 4 bytes

	// Current location of the Squadron.
	CoordU loc; // 4 bytes.

	// Number of ships in the squadron.
	uint_least16_t numShips; // 2 bytes

	// No bytes padding.

	// Constructor for the Squadron. Assigns inputed values, initializes threats,
	// and places the Squadron in the inputed tile.
	Squadron(Government* owner, void (*behaviour)(Squadron* squadron), CoordU loc);

	// Deconstructor for the squadron. Frees all allocated memory and removes the
	// Squadron from the universe.
	~Squadron();

	// Adds some Ships to the Squadron.
	void addShip(ShipTemplate* ship, uint_least16_t numShips);

	// Reassigns the strength of this Squadron.
	void reassignStrength();

	// Begins tracking a Threat to this Squadron.
	// Threat related functions are all placed in Event Handling.hpp
	inline void addThreat(Threat threat);
	inline void addThreat(Squadron* squadron);

	// Stops tracking a Threat to this Squadron.
	inline void removeThreat(Threat threat);
	inline void removeThreat(Squadron* squadron);

	// Handles tracking all Threats to this Squadron.
	inline void handleThreats();
	void trackThreat(Threat& threat);

	// Moves the Squadron to an inputed tile.
	inline void move(uint_least16_t xPos, uint_least16_t yPos);

	// Moves the Squadron to an inputed tile.
	inline void move(CoordU tile);

	// Moves along the Squadron's path.
	// Paths are formatted such that the first CoordU contains {currProgress, pathLength}.
	void move();

	// Returns this Squadron's path.
	inline CoordU* getPath();

	// Returns the destination of the Squadron's path.
	inline CoordU getDest();

	// Creates a path to a specific point for this Squadron.
	void squadronPath(int xDest, int yDest);

	// Returns the speed of this Squadron. Speed is the minimum speed of any Ship.
	inline uint_least8_t speed();

	// Returns the scanning strength of this Squadron. Scanning strength is the
	// maximum scan value of any Ship.
	inline uint_least8_t scan();

	// Returns true if the Squadron can scan a given tile, otherwise false.
	inline bool scan(CoordU dest, int distance);
	inline bool scan(CoordU dest);

	// Performs the behaviour associated with this squadron.
	inline void action();

};

/*
Estimates the strength of a certain number of ships of a Ship Template.

Ship strength is num * (damage / 2 + armor / 2 + all else / 4 + 1).
*/
inline uint_least16_t estimateShipStrength(ShipTemplate temp, uint_least16_t num) {
	uint_least16_t strength = 0;

	// Adds 1 / 2 all else.
	strength = (temp.dodge + temp.accuracy + temp.speed + temp.scan) / 2;

	// Adds armor and damage.
	for (int i = 0; i < NUM_DAMAGE_TYPES; ++i) strength += temp.damage[i] + temp.armor[i];

	// Divides armor and damage by 2 and all else by 4.
	strength /= 2;

	// Returns the strength of all the ships in the Ship.
	return num * (strength + 1);

}

/*
Estimates the strength of a Ship item.

Ship strength is num * (damage / 2 + armor / 2 + all else / 4) + 1.
*/
inline uint_least16_t estimateShipStrength(Ship ship) {
	return estimateShipStrength(ship.type, ship.num);

}

/*
Estimates the strength of a Squadron.
*/
uint_least16_t estimateSquadronStrength(Squadron* squadron) {
	uint_least16_t strength = 0;

	// Sums the strength of all Ships in the Squadron.
	for (int i = 0; i < squadron->numShips; ++i) {
		strength += estimateShipStrength(squadron->ships[i]);

	}

	// Returns the estimated strength of the Squadron.
	return strength;

}

/*
Constructor for the Squadron. Assigns inputed values, initializes threats,
and places the Squadron in the inputed tile.
*/
Squadron::Squadron(Government* owner, void (*behaviour)(Squadron* squadron), CoordU loc) :
	owner(owner), loc(loc), behaviour(behaviour), data(nullptr), numShips(0), strength(0) {
	threats = std::vector<Threat>();
	uIndex(loc.x, loc.y).addSquadron(this);

}

/*
Deconstructor for the squadron. Frees all allocated memory and removes the
Squadron from the universe.

Note: It is assumed that this function will be mutexed elsewhere.

TODO make threat removal use a maximum range, or otherwise make it more efficient.
*/
Squadron::~Squadron() {
	uIndex(loc.x, loc.y).removeSquadron(this);
	if (data) free(data);
	free(ships);
	extern void prepareRemovedSquadron(Squadron*);
	prepareRemovedSquadron(this);

}

/*
Adds a Ship to a Squadron.
*/
void Squadron::addShip(ShipTemplate* ship, uint_least16_t add) {

	// Array size will be incremented by 8.
	const int inc = 8;

	// If ships is empty, initializes it.
	if (!numShips) ships = (Ship*)malloc(sizeof(Ship) * inc);

	// Attempts to place the ships within the Squadron.
	for (int i = 0; i < numShips; ++i) {
		if (*ship == ships[i].type) {
			ships[i].num += add;
			strength += estimateShipStrength(*ship, add);
			return;

		}
	}

	// If ships could not be placed within the squadron, resizes ships if it is too small.
	if (numShips && !(numShips % inc)) ships = (Ship*)realloc(ships, sizeof(Ship) * (numShips + inc));

	// Adds the Ship to the Squadron.
	ships[numShips] = { *ship, add, add };
	++numShips;

	// Reassigns the strength of the Squadron.
	strength += estimateShipStrength(*ship, add);

}

/*
Reassigns the strength of this Squadron.
*/
void Squadron::reassignStrength() {
	strength = estimateSquadronStrength(this);

}

/*
Performs the behaviour associated with this squadron.
*/
inline void Squadron::action() {
	behaviour(this);

}

/*
Moves the Squadron to an inputed tile.

Note: Assumes that the tile (loc.x, loc.y) is already locked.
*/
inline void Squadron::move(uint_least16_t xPos, uint_least16_t yPos) {
	bool lock = xPos != loc.x;

	// Locks the mutex for this tile.
	// Note: Mutexes only cover xPos bands.
	if (lock) {
		while (!uTryLock(xPos, yPos)) {
			uUnlock(loc.x, loc.y);
			uLock(loc.x, loc.y);

		}
	}

	// Moves the Squadron from its current tile to its desired tile.
	uIndex(loc.x, loc.y).removeSquadron(this);
	uIndex(xPos, yPos).addSquadron(this);
	this->loc.x = xPos;
	this->loc.y = yPos;

	// Unlocks the mutex for this tile.
	if (lock) uUnlock(xPos, yPos);

}

/*
Moves the Squadron to an inputed tile.

Note: Assumes that the tile (loc.x, loc.y) is already locked.
*/
inline void Squadron::move(CoordU tile) {
	move(tile.x, tile.y);

}

/*
Moves along the Squadron's path.
Paths are formatted such that path[0] contains {currProgress, pathLength},
where currProgress is in days and pathLength is in tiles. Note that
path[path[0].y] is actually the final tile, since path[0] contains metadata.

Note: Assumes that the tile (loc.x, loc.y) is already locked.
*/
void Squadron::move() {
	CoordU* path = getPath();
	CoordU dest;
	int travelTime = 0;
	int i;
	
	// TODO DEBUG REMOVE
	static Squadron* squadron;
	if (!squadron) squadron = this;

	// Does noting if the Squadron is at its destination.
	if (loc == getDest()) return;

	// Moves the Squadron forwards at the speed of the slowest ship.
	path[0].x += speed();

	// Determines the appropriate tile based on time spent traveling.
	for (i = 1; i < path[0].y + 1 && path[0].x > travelTime; ++i) travelTime += uMovementCost(path[i].x, path[i].y);
	dest = path[i - 1];

	// Changes the Squadron's tile if it has moved forwards.
	if (loc != dest) move(dest);

}

/*
Returns this Squadron's path.
*/
inline CoordU* Squadron::getPath() {
	return (CoordU*)(((uint_least8_t*)data) + 8);

}

/*
Returns the destination of the Squadron's path.
*/
inline CoordU Squadron::getDest() {
	CoordU* path = getPath();
	return path[path[0].y];

}

/*
Creates a path to a specific point for this Squadron. The path will be stored
in the Squadron's data. The first 8 bytes of the Squadron's path will be
reserved for metadata, to be filled in the calling function.

Paths are formatted such that path[0] contains {currProgress, pathLength},
where currProgress is in days and pathLength is in tiles. Note that
path[pathLength] is actually the final tile, since path[0] contains metadata.

Note: Whatever is in data will be destroyed when this function is called.
Note: This will set data as nullptr in case of failure.

TODO this should eventually use links between systems as a small hack.
*/
void Squadron::squadronPath(int xDest, int yDest) {
	LinkedListNode<PathCoordinate>* currPos = nullptr;
	LinkedListNode<PathCoordinate>* startPos = nullptr;
	LinkedListNode<PathCoordinate>* coord;
	CoordU* path;
	int xPos, yPos;
	int length = 0;

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
	coord->value->hValue = calcTaxiDistance(loc.x, loc.y, xDest, yDest);
	coord->previous = nullptr;
	frontier.addc(coord);
	exploredMap[index(loc.x, loc.y, universeWidth)] = true;

	// Pathfinds until the desired coordinate is found.
	while (!frontier.isEmpty()) {

		// Starts exploring the node.
		currPos = frontier.popc();
		explored.addc(currPos);

		// Marks this tile explored.
		exploredMap[index(currPos->value->coord.x, currPos->value->coord.y, universeWidth)] = true;

		// If currPos meets the inputed criteria, stops looping.
		if (currPos->value->coord.x == xDest && currPos->value->coord.y == yDest) break;

		// Explores each coordinate adjacent to currPos.
		for (int i = -1; i < 2; ++i) {
			for (int j = -1; j < 2; ++j) {
				if (!i && !j) continue;
				xPos = currPos->value->coord.x + i;
				yPos = currPos->value->coord.y + j;
				if (!universeInBounds(xPos, yPos)) continue;

				// Places the coord if it has not been explored.
				if (!exploredMap[index(xPos, yPos, universeWidth)]) {

					// Adds the coord to the frontier.
					coord = coordList->popc();
					coord->value->coord.x = xPos;
					coord->value->coord.y = yPos;
					coord->value->gValue = currPos->value->gValue + 1;
					coord->value->hValue = calcTaxiDistance(xPos, yPos, xDest, yDest);
					coord->previous = currPos;
					frontier.addcAsc(coord);

				}
			}
		}
	}

	// Returns all values to the coordList.
	while (!frontier.isEmpty()) coordList->addc(frontier.popc());
	while (!explored.isEmpty()) coordList->addc(explored.popc());

	// If currPos does not contain the desired value, releases held resources
	// and returns.
	if (currPos->value->coord.x != xDest && currPos->value->coord.y != yDest) {
		if (data) free(data);
		data = nullptr;
		releaseCoordList(coordList);
		releaseDummy(exploredMap);
		return;

	}

	// Stores the starting position to allow multiple iterations along the path.
	startPos = currPos;

	// Calculates the path's length.
	while (currPos) {
		currPos = currPos->previous;
		++length;

	}
	currPos = startPos;

	// Allocates data.
	if (data) free(data);
	data = calloc(1, sizeof(CoordU) * (length + 1) + 8);
	path = getPath();

	// Places the path's length.
	path[0].y = length;
	
	// Places the path.
	while (currPos) {
		path[length].x = currPos->value->coord.x;
		path[length].y = currPos->value->coord.y;
		currPos = currPos->previous;
		--length;

	}

	// Returns held resources.
	releaseCoordList(coordList);
	releaseDummy(exploredMap);

}

/*
Returns the speed of this Squadron.Speed is the minimum speed of any Ship.
*/
inline uint_least8_t Squadron::speed() {
	uint_least8_t speed = 255;
	for (int i = 0; i < numShips; ++i) {
		if (ships[i].num && ships[i].type.speed < speed) {
			speed = ships[i].type.speed;

		}
	}
	return speed;
}

/*
Returns the scanning strength of this Squadron. Sacnning strength is the
maximum scan value of any Ship.
*/
inline uint_least8_t Squadron::scan() {
	uint_least8_t scan = 0;
	for (int i = 0; i < numShips; ++i) {
		if (ships[i].num && ships[i].type.scan > scan) {
			scan = ships[i].type.scan;

		}
	}
	return scan;
}

/*
Returns true if the Squadron can scan a given tile, otherwise false.
*/
inline bool Squadron::scan(CoordU dest, int distance) {
	uint_least8_t scan = this->scan();
	return 64 * scan > uSensorCost(dest.x, dest.y) * distance * distance;

}

/*
Returns true if the Squadron can scan a given tile, otherwise false.
*/
inline bool Squadron::scan(CoordU dest) {
	scan(dest, calcTaxiDistance(loc.x, loc.y, dest.x, dest.y));

}

// Vector which stores removed Squadrons.
std::vector<Squadron*> removedSquadrons = std::vector<Squadron*>();

/*
Below is a pair of functions used to track Squadrons which need to be removed from Threats.
*/

/*
Marks that a reference to a Squadron should be removed from Threats.
Note that by the time this is called, the Squadron will already have been deleted.
*/
void prepareRemovedSquadron(Squadron* squadron) {
	static std::shared_mutex removeSquadronMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(removeSquadronMutex);

	// Adds the inputed Squadron to the list of removed Squadrons.
	removedSquadrons.push_back(squadron);

}

/*
Removes all deleted Squadrons as Threats to the inputed Squadron.
*/
void removeDeletedSquadrons(Squadron* squadron) {
	for (Squadron* remove : removedSquadrons)
		squadron->removeThreat(remove);

}

/*
Removes all deleted Squadrons as Threats to the inputed Colony.
*/
void removeDeletedSquadrons(Colony* colony) {
	for (Squadron* remove : removedSquadrons)
		colony->removeThreat(remove);

}

/*
Removes all Squadrons from removedSquadrons.
This should be called by the gameplay thread after SpaceBattles is finished.
*/
void emptyRemovedSquadrons() {
	removedSquadrons.clear();

}

/*
Below is an assortment of various useful helper functions relating to Squadrons.
*/

/*
Performs battle between two ships.
*/
void shipAttack(Ship& attacker, Ship& defender) {
	int damage, armor;
	uint_least16_t casualties = 0;

	// Compares all armor and defense types.
	for (int i = 0; i < NUM_DAMAGE_TYPES; ++i) {
		damage = attacker.num * attacker.type.damage[i];
		armor = defender.num * defender.type.armor[i];

		// Creates no casualties if damage is less than half of armour.
		if (damage < armor / 2) damage = 0;
		// Creates one casualty if damage is less than armour.
		else if (damage <= armor) damage = 1;
		// Does full damage if damage is greater than armour.
		else damage = damage - armor;

		// Increases casualties.
		casualties += damage;

	}

	// Reduces casualties by half if there are more than 3.
	if (casualties > 3) casualties /= 2;

	// Modifies casualties by accuracy and dodge.
	if (attacker.type.accuracy < defender.type.dodge)
		casualties = (casualties * attacker.type.accuracy) / defender.type.dodge;

	// Ensures that defenders do not take more casualties than they have units.
	casualties = casualties < defender.num ? casualties : defender.num;

	// Inflicts casualties on the defender.
	defender.num -= casualties;

}

/*
Performs battle between two Squadrons.

Ships with higher range attack first. When range is equal, defenders are
prioritized.

TODO make ships choose better targets.
*/
void squadronAttack(Squadron* attacker, Squadron* defender) {
	int maxRange = 65536;
	int currRange;

	// Attacks with all units.
	for (int currUnit = 0; currUnit < attacker->numShips + defender->numShips; ++currUnit) {
		currRange = 0;

		// Finds the highest ranged unit which has not yet fired among attackers.
		for (int unit = 0; unit < attacker->numShips; ++unit) {

			// Uses the ship with the highest range below maxRange.
			if (attacker->ships[unit].type.range > currRange &&
				attacker->ships[unit].type.range < maxRange) {
				currRange = attacker->ships[unit].type.range;

			}
		}

		// Finds the highest ranged unit which has not yet fired among defenders.
		for (int unit = 0; unit < defender->numShips; ++unit) {

			// Uses the ship with the highest range below maxRange.
			if (defender->ships[unit].type.range > currRange &&
				defender->ships[unit].type.range < maxRange) {
				currRange = defender->ships[unit].type.range;

			}
		}

		// Sets currRange as the maximum range for the next search.
		maxRange = currRange;

		// Incites all defenders of currRange to attack.
		for (int unit = 0; unit < defender->numShips; ++unit) {

			// TODO launches an attack against the first available target.
			for (int target = 0; target < attacker->numShips; ++target) {
				if (attacker->ships[target].num) {
					shipAttack(defender->ships[unit], attacker->ships[target]);
					break;

				}
			}
		}

		// Incites all attackers of currRange to attack.
		for (int unit = 0; unit < attacker->numShips; ++unit) {

			// TODO launches an attack against the first available target.
			for (int target = 0; target < defender->numShips; ++target) {
				if (defender->ships[target].num) {
					shipAttack(attacker->ships[unit], defender->ships[target]);
					break;

				}
			}
		}
	}

	// Recalculates Squadron strength.
	attacker->reassignStrength();
	defender->reassignStrength();

	// Deletes either Squadron if they are empty.
	if (!attacker->strength) delete attacker;
	if (!defender->strength) delete defender;

}

/*
Determines whetehr a Squadron belonging to the inputed Government is present in
the inputed tile.

Returns the first Squadron found which belongs to the inputed Government.
*/
inline Squadron* universeContainsOwnedSquadron(int xPos, int yPos, Government* government) {

	// Attempts to find a Squadron owned by the inputed Government.
	for (int i = 0; i < uIndex(xPos, yPos).numSquadrons; ++i) {
		if (uSquadron(xPos, yPos, i)->owner == government) {
			return uSquadron(xPos, yPos, i);

		}
	}

	// Returns nullptr if a Squadron owned by the inputed Government cannot be found.
	return nullptr;

}

/*
Determines whether the inputed tile contains an enemy to the inputed Government.

TODO create some means of storing national diplomacy.
TODO unused (except in scanEnemy, which is also unused)
*/
bool universeContainsEnemy(int xPos, int yPos, Government* government) {

	// Checks to see if the tile is owned by an enemy.
	// TODO implement diplomacy check
	//if (uOwner(xPos, yPos) is enemy to government) return true;

	// Checks to see if any squadron in the tile is owned by an enemy.
	// TODO implement diplomacy check
	for (int i = 0; i < uIndex(xPos, yPos).numSquadrons; ++i) {
		// if (uSquadron(xPos, yPos, i).owner is enemy to government) return true;

	}

	// If no enemies are found, returns false.
	return false;

}

/*
Determines whether the inputed tile contains the inputed enemy Squadron.
*/
inline bool universeContainsSquadron(int xPos, int yPos, Squadron* squadron) {

	// Returns true if the Squadron is found.
	for (int i = 0; i < uIndex(xPos, yPos).numSquadrons; ++i)
		if (uSquadron(xPos, yPos, i) == squadron) return true;
	
	// Returns false if the Squadron is not found.
	return false;

}

/*
Scans for enemies within vision range of the inputed Squadron.

TODO add some means of checking diplomacy
TODO unused
*/
bool scanEnemies(Squadron* squadron) {
	int xBeg, yBeg, xEnd, yEnd;
	int scan = 0;

	// Finds the maximum scanner range of the squadron.
	for (int i = 0; i < squadron->numShips; ++i)
		scan = squadron->ships[i].type.scan > scan ? squadron->ships[i].type.scan : scan;

	// Finds appropriate starting and ending points for the scan.
	xBeg = squadron->loc.x - scan > 0 ? squadron->loc.x - scan : 0;
	xEnd = squadron->loc.x + scan < universeWidth ? squadron->loc.x + scan : universeWidth - 1;
	yBeg = squadron->loc.y - scan > 0 ? squadron->loc.y - scan : 0;
	yEnd = squadron->loc.y + scan < universeHeight ? squadron->loc.y + scan : universeHeight - 1;

	// Returns true if an enemy is found within vision range of the inputed squadron.
	for (int x = xBeg; x < xEnd; ++x) {
		for (int y = yBeg; y < yEnd; ++y) {
			if (universeContainsEnemy(x, y, squadron->owner)) return true;

		}
	}

	// Returns false if no enemy is found.
	return false;

}

/*
Below is an assortment of various Squadron actions which a Squadron can perform.
Squadron actions are something like a state machine, and have transitions
between states based on commanders, governments, and thes tatus of the Squadron.

Squadron actions are divided into categories: Attack, move, reinforce, and idle.
	Which actions are possible and which actions are taken should be determined
	by both the Government and the Squadron commander.
*/

// These functions are declared here for ease of reference.
void squadronIdle(Squadron* squadron);
void squadronOccupy(Squadron* squadron);
void squadronGuard(Squadron* squadron);
void squadronPursue(Squadron* squadron);
void squadronMove(Squadron* squadron);
void squadronInvadeOrcish(Squadron* squadron);

/*
This Squadron will do nothing until provided further orders.
*/
void squadronIdle(Squadron* squadron) {

}

/*
This Squadron will attempt to occupy foreign territory.
*/
void squadronOccupy(Squadron* squadron) {

}

/*
This Squadron will remain in place until a Threat is discovered, at which
point the Squadron will implement squadronPursue towards said Threat.
*/
void squadronGuard(Squadron* squadron) {
	int distance;
	int min = 65536;
	int minIndex = 0;

	// If any threat is discovered, attacks the closest one.
	if (squadron->threats.size()) {
		for (int i = 0; i < squadron->threats.size(); ++i) {

			// Does not pursue non-threatening or unknown Squadrons.
			if (!squadron->threats[i]) continue;

			// Calculates the taxicab distance between this Squadron and the current threat.
			distance = calcTaxiDistance(squadron->loc.x, squadron->loc.y,
				squadron->threats[i].coord.x, squadron->threats[i].coord.y);

			// Assigns the minimum distance.
			if (distance < min) {
				min = distance;
				minIndex = i;

			}
		}

		// Attempts to pursue the nearest threat if a valid one was found.
		if (min != 65536) {
			squadron->squadronPath(squadron->threats[minIndex].coord.x, squadron->threats[minIndex].coord.y);
			*((Squadron**)squadron->data) = squadron->threats[minIndex].squadron;
			squadron->behaviour = squadronPursue;

		}
	}
}

/*
This Squadron will pursue a given Threat.

TODO implement an invalid behavior other than guarding.
*/
void squadronPursue(Squadron* squadron) {
	Squadron* enemy = *((Squadron**)squadron->data);
	std::vector<Threat>::iterator threat;
	int xPos, yPos;
	bool valid;

	// Locks the tile.
	// TODO possible error: wait on lock, destroyed elsewhere, unlock, continue with invalid Squadron.
	uLock(squadron->loc.x, squadron->loc.y);

	// Stores xPos and yPos so the correct mutex is unlocked after movement.
	xPos = squadron->loc.x;
	yPos = squadron->loc.y;

	// Determines whether the Threat being pursued still exists and is still
	// tracked by the Squadron.
	// Note: threat is assigned here if and only if the enemy is valid.
	valid = enemy->strength && (threat = std::find_if(squadron->threats.begin(),
		squadron->threats.end(), [enemy](Threat threat) { return threat.squadron == enemy; }))
		!= squadron->threats.end();

	// If the threat is no longer valid, finds a new action and returns.
	// TODO replace squadronGuard with means of determining new activity
	if (!valid) {
		free(squadron->data);
		squadron->data = nullptr;
		squadron->behaviour = squadronGuard;
		uUnlock(xPos, yPos);
		return;

	}

	// If in a tile with the desired enemy, attacks and returns.
	if (universeContainsSquadron(xPos, yPos, enemy)) {
		squadronAttack(squadron, enemy);
		uUnlock(xPos, yPos);
		return;

	}

	// If the threat still exists but has moved, recalculates path to continue
	// following the threat.
	if ((*threat).coord != squadron->getDest()) {
		squadron->squadronPath((*threat).coord.x, (*threat).coord.y);
		*((Squadron**)squadron->data) = enemy;

	}

	// Moves to approach the enemy.
	squadron->move();
	uUnlock(xPos, yPos);

}

/*
This Squadron will move along its path.

TODO implement.
*/
void squadronMove(Squadron* squadron) {
	uLock(squadron->loc.x, squadron->loc.y);
	squadron->move();
	uUnlock(squadron->loc.x, squadron->loc.y);

}

/*
This Orcish Squadron will invade the target at the end of its path.

TODO implement.
*/
void squadronInvadeOrcish(Squadron* squadron) {

}