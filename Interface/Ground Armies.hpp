#pragma once

// Macro for accessing a GroundBattleTile from a battle.
// Undefined at the end of this file.
#define gbIndex(x, y, planet) battlefield[index(x, y, planet->size)]

// Macro for accessing a GroundUnitTemplate from a battle tile.
// Undefined at the end of this file.
#define gbUnitTemplate(x, y, n, planet) owners[battlefield[index(x, y, planet->size)].owner].unitTable[battlefield[index(x, y, planet->size)].units[n].unit]

// Number of units that can be held in a given tile.
#define NUM_TILE_UNITS 2

// Assumed maximum number of fronts. 
// Note that this is not checked/binding and only informs memset/arr sizes.
#define MAX_GROUND_FRONTS 512

// TODO DEBUG REMOVE
extern HabitablePlanet* activeHabitable;
extern void changeActiveHabitable(HabitablePlanet*);
extern int gameSpeed;

// Enum signifying types of damage that a unit can deliver.
enum GroundDamageTypes {
	BallisticDamage,
	HeatDamage,
	PsionicDamage

};

// Enum signifying types of movement that a Ground Unit can perform.
// TODO add more including duck, wheel, rudder, etc. Remember to increase movementType size.
enum GroundMovementTypes {
	LimbMovement,
	TrackMovement,
	SailMovement,
	FlightMovement
};

/*
Template for a ground unit.

8 bytes.
*/
struct GroundUnitTemplate {

	// Armor types which nullify a certain amount of damage.
	uint_least8_t ballisticArmor;
	uint_least8_t heatArmor;
	uint_least8_t psionicArmor;

	// Damage done. Damage has precisely one type.
	uint_least8_t damage;
	uint_least8_t damageType : 2;

	// Movement types. Split with damageType to minimize memory footprint.
	uint_least8_t movement : 4;
	uint_least8_t movementType : 2;

	// Range of a unit. When a unit attacks another unit with lower range, there will
	// be no counterratack. Further, when a unit attacks a unit with higher range, the
	// longer ranged unit will be able to attack first.
	uint_least8_t range;

	// Sprite for this unit.
	uint_least8_t unitSprite;

	// Estimated strength of this ground unit.
	uint_least8_t estimatedStrength;

	// 8 byte aligned.

};

/*
Struct representing a Front on a planet.
*/
struct GroundFront {

	// Estimated strength of the front's owner and enemies.
	uint_least32_t strength;
	uint_least32_t enemyStrength;

	// Number of tiles for this front.
	uint_least16_t numTiles : 15;

	// Whether this front is a land or sea front.
	uint_least16_t isLand : 1;

	// Indices within owners for the front's owner and enemy.
	uint_least8_t owner;
	uint_least8_t enemy;

	// 12 bytes.

};

/*
Represents an owner in a ground battle. Note that unitTable points directly to the Government's unitTable.

TODO owner should not always be a Colony
TODO unitTable should be copied rather than directly referencing parent.
*/
struct GroundOwner {
	Colony* owner;
	GroundUnitTemplate* unitTable; // The first four bytes of unitTable[0] contains the number of GroundUnitTemplates.

};

/*
Struct representing a Ground Unit on a planet.

2 bytes.
*/
struct GroundUnit {

	// Quantity associated with the given unit.
	uint_least8_t quantity;

	// Type of unit in the tile. 0 connotes no unit.
	uint_least8_t unit : 7;

	// Indicates that this unit is presently moving.
	uint_least8_t moving : 1;

};

/*
Struct representing a tile on a planetary battlefield.
*/
struct GroundBattleTile {

	// Unit within this tile.
	// units[1] is used for garrisons and moving units.
	// TODO is this true?
	GroundUnit units[NUM_TILE_UNITS];

	// Front that this tile is on.
	// Note: front == 0 connotes unnocupied.
	uint_least16_t front;

	// Owner of this tile.
	// Used as an index in the array of owners in battles.
	uint_least8_t owner;

	// 1 byte of padding.

};
/*
Determines the casualties of a Ground battle between two inputed units.
Note: The new quantities are returned via attackers and defenders.

TODO this entire system should be reworked, the current function is strictly temporary.
TODO attacks from sea to land/vice versa should be highly range dependent
*/
inline void determineGroundCasualties(GroundUnitTemplate attackerUnit, GroundUnitTemplate defenderUnit, uint_least8_t* attackers, uint_least8_t* defenders) {
	uint_least8_t attackerRet = *attackers;
	uint_least8_t defenderRet = *defenders;
	int damage;

	// If the attacker does not outrange the defender, determines the attacker's casualties.
	if (attackerUnit.range <= defenderUnit.range) {

		// Calculates damage done to attackers.
		if (attackerUnit.ballisticArmor > defenderUnit.damage) damage = (*defenders * defenderUnit.damage * defenderUnit.damage / attackerUnit.ballisticArmor) / 16;
		else damage = (*defenders * defenderUnit.damage) / 8;

		// Determines attacker's casualties.
		attackerRet = *attackers - damage > 0 ? *attackers - damage : 0;

	}

	// Calculates damage done to defenders.
	if (defenderUnit.ballisticArmor > attackerUnit.damage) damage = (*attackers * attackerUnit.damage * attackerUnit.damage / defenderUnit.ballisticArmor) / 16;
	else damage = (*attackers * attackerUnit.damage) / 8;

	// Determines the defender's casualties.
	defenderRet = *defenders - damage > 0 ? *defenders - damage : 0;

	// Assigns the new values to the unites.
	*attackers = attackerRet;
	*defenders = defenderRet;

}

/*
Estimates the offensive strength of the inputed ground unit and stores the estimate within the unit.
TODO add remaining armour types
TODO armour should be average of all armours
ensure that maximum is below 255
*/
inline void groundUnitTemplateStrength(GroundUnitTemplate* unit) {
	unit->estimatedStrength = ((unit->ballisticArmor) + unit->range + unit->damage) / 4 + 1;

}

/*
Stores a Battle on  a planet.

This class uses 64 bytes.
*/
class Battle {
public:

	// List of movement orders for this planet.
	// The first Coord in every movement stores the number of tiles in x and the current tile in y.
	LinkedList<Coord> movements;

	// Comparison matrix for storing combatants in the battle.
	// Note: comp.byte1 stores numOwners and comp.short1 stores numFronts
	// to save memory from padding.
	ComparisonMatrix comp;

	// Array of owners in the battlefield.
	// owners[0] connotes unowned land. comp.byte1 stores numOwners.
	GroundOwner* owners;

	// Array of fronts in the battlefield.
	// fronts[0] connotes land which belongs to no front. comp.short1 stores numFronts.
	// numTiles == 0 connots an unused front.
	GroundFront* fronts;

	// Battlefield on the planet.
	GroundBattleTile* battlefield;

	// Stores the planet associated with this Battle.
	HabitablePlanet* planet;

	// TODO include generals and commanders as necessary.

	// Standard constructor for a Battle.
	// Note: the inputed planet is assigned this Battle in the constructor.
	Battle(HabitablePlanet* planet);

	// Deconstructor for a Battle.
	~Battle();

	// Manages the battle.
	void battle();

	// Extends a planet's fronts from the inputed tile.
	// Meant to be called by outside functions.
	void extendFronts(int x, int y, Colony* owner);

	// Reinforces the Battle. Returns the number of unused units.
	int reinforce(void* owner, int numUnits, int unit);

	// Places units into a tile.
	// Adjusts front strengths.
	inline void placeUnits(int x, int y, uint_least8_t quantity, uint_least8_t unit);

	// Returns a Direction showing which adjacent tiles contain enemies.
	// Used for renderHabitableArmy.
	Direction enemyDirection(int x, int y);

	// TODO DEBUG REMOVE
	void verifyFronts();

	// TODO DEBUG REMOVE
	bool checkFronts();

	// TODO DEBUG REMOVE
	bool checkPaths();

	// TODO DEBUG REMOVE
	bool checkUnits();

	// Finds the index of the inputed owner. Will be equal to comp.byte1 if there is no such index.
	int getOwner(void* owner);

private:

	// Finds an unused front.
	int getUnusedFront();

	// Initializes fronts for the Battle.
	void initFronts();

	// Initializes the owners for the Battle.
	// comp is also initialized here.
	void initOwners();

	// Checks if the inputed unit can occupy the inputed terrain.
	inline bool canOccupyTerrain(int x, int y, GroundUnitTemplate unit);

	// Checks if the inputed unit can occupy the inputed terrain.Used for reinforcing fronts.
	inline bool canOccupyTerrain(int isLand, GroundUnitTemplate unit);

	// Estimates the offensive strength of a certain quantity of a certain unit.
	inline int groundUnitStrength(uint_least8_t owner, uint_least8_t unit, uint_least8_t quantity);

	// Estimates the offensive strength of one unit in a tile.
	inline int groundUnitStrength(uint_least8_t owner, GroundUnit unit, uint_least8_t quantity);

	// Estimates the offensive strength of one unit in a tile.
	inline int groundUnitStrength(GroundBattleTile tile, uint_least8_t unit);

	// Sums the estimated offensive strength of the given tile.
	inline int groundUnitTileStrength(GroundBattleTile units);

	// Removes units from a tile.
	// Adjusts front strengths.
	inline void removeUnits(int x, int y, uint_least8_t quantity, uint_least8_t unit);

	// Moves units inwards.
	inline void moveUnitsInwards(int start, GroundBattleTile& tile);

	// Creates paths to move units to nearby fronts.
	void moveToFronts();

	// Creates paths to move units between fronts.
	void moveBetweenFronts();

	// Moves units along their designated paths.
	void moveUnits();

	// Diffuses units on fronts.
	void diffuseUnits();

	// Causes enemy units to attack each other.
	void attackUnits();

	// Plans naval invasions.
	void navalInvade();

	// Calculates and assigns front strength.
	void calcFrontStrength();

	// Checks to see if the inputed owner is in any of the 8 adjacent tiles.
	int checkOwnerAdjacency(int x, int y, int owner);

	// Checks to see if the enemy of the inputed front is in any of the 8 adjacent tiles.
	// Will return false in the case that front->enemy is unowned and an owned tile is found.
	int checkEnemyAdjacency(int x, int y, GroundFront* front);

	// Checks to see if the inputed front is in any of the 8 adjacent tiles.
	int checkFrontAdjacency(int x, int y, int front);

	// Attempts to find an adjacent enemy. Returns owner if no enemy is found.
	int findAdjacentEnemy(int x, int y, int owner);

	// Calculates the strength of all adjacent enemies.
	// Assumes that the inputed tile has not yet been taken by the inputed owner.
	int findAdjacentEnemyStrength(int x, int y, int front);

	// Changes the enemyStrenghts of all adjacent enemy fronts by strength.
	void adjustAdjacentEnemyStrengths(int x, int y, int strength);

	// Extends a planet's fronts around the inputed tile.
	void extendFronts(int x, int y, int owner);

	// Extends a planet's fronts from the inputed tile.
	void extendFrontsHelper(int x, int y, int owner);

	// Extends a front. Will call either extendGroundFront or extendSeaFront.
	inline void extendFront(int currX, int currY, int front);

	// Extends a ground front from an inputed tile.
	void extendLandFront(int currX, int currY, int front);

	// Extends a sea front from an inputed tile.
	void extendSeaFront(int currX, int currY, int front);

	// Reinforces a front.
	void reinforceFront(int front, int numUnits, int unit);

};

/*
TODO DEBUG REMOVE
Checks to see if fronts have improper values.
will only ever return 1 once.
*/
bool Battle::checkFronts() {
	static int ret = 0;
	int tot1 = 0, tot2 = 0;
	int xPos, yPos;
	uint_least32_t strengths[MAX_GROUND_FRONTS];
	uint_least32_t enemyStrengths[MAX_GROUND_FRONTS];
	uint_least32_t numTiles[MAX_GROUND_FRONTS];
	uint_least8_t* dummy;
	memset(strengths, 0, sizeof(int) * MAX_GROUND_FRONTS);
	memset(enemyStrengths, 0, sizeof(int) * MAX_GROUND_FRONTS);
	memset(numTiles, 0, sizeof(numTiles[0]) * MAX_GROUND_FRONTS);
	
	if (ret) return 0;

	// Get dummy.
	dummy = requestDummy();

	// Find strengths and numTiles.
	for (int i = 0; i < planet->size * planet->size; ++i) {
		if (battlefield[i].owner) {
			strengths[battlefield[i].front] += groundUnitTileStrength(battlefield[i]);
			++numTiles[battlefield[i].front];

		}
	}

	// Find enemyStrengths for each front.
	for (int currFront = 1; currFront < comp.short1; ++currFront) {
		memset(dummy, 0, planet->size * planet->size * sizeof(uint_least8_t));

		// Checks every tile.
		for (int i = 0; i < planet->size; ++i) {
			for (int j = 0; j < planet->size; ++j) {

				// Gets enemy strength.
				if (gbIndex(i, j, planet).front == currFront) {
					for (int x = -1; x < 2; ++x) {
						for (int y = -1; y < 2; ++y) {
							if (!x && !y) continue;
							xPos = i + x;
							yPos = j + y;
							wrapAroundPlanet(planet->size, &xPos, &yPos);

							// If at war, not moving, and not already counted, adds value. 
							if (comp.comp(gbIndex(i, j, planet).owner, gbIndex(xPos, yPos, planet).owner) && !dummy[index(xPos, yPos, planet->size)]) {
								enemyStrengths[currFront] += groundUnitTileStrength(gbIndex(xPos, yPos, planet));
								dummy[index(xPos, yPos, planet->size)] = 1;

							}
						}
					}
				}
			}
		}
	}

	// Release the dummy.
	releaseDummy(dummy);

	// Compare strengths and enemyStrengths. Return 1 if there is a discrepency.
	for (int i = 1; i < comp.short1; ++i) {
		if ((uint_least32_t)strengths[i] != (uint_least32_t)fronts[i].strength) { ret = 1;  return 1; }
		if ((uint_least32_t)enemyStrengths[i] != (uint_least32_t)fronts[i].enemyStrength) {	ret = 1;  return 1;	}
	}

	// Totals for strengths.
	for (int i = 1; i < comp.short1; ++i) {
		tot1 += strengths[i];
		tot2 += fronts[i].strength;
	}

	// Compares numTiles.
	for (int i = 1; i < comp.short1; ++i) {
		if (numTiles[i] != fronts[i].numTiles) { ret = 1;  return 1; }

	}

	// return 0 if all goes well.
	return 0;
}

/*
* TODO DEBUG REMOVE
Checks that all paths have units at their associated positions and all units are associated correctly with paths.
*/
bool Battle::checkPaths() {
	int ret = false;
	LinkedListNode<Coord>* currNode = movements.root;
	uint_least8_t* dummy = requestDummy();
	memset(dummy, 0, sizeof(uint_least8_t) * planet->size * planet->size);

	// Reviews every path.
	while (currNode != nullptr) {

		// Reviews every coord in the path.
		for (int curr = 2; curr < currNode->value->x; ++curr) {

			// Dummy is 1 for being part of the path, 2 for containing the unit.
			dummy[index(currNode->value[curr].x, currNode->value[curr].y, planet->size)] = 1 + (curr == currNode->value->y);

		}

		// Moves on to the next path.
		currNode = currNode->child;

	}

	// Debug code to display the current movement paths.
	//if (planet == activeHabitable) {
	//	printf("   ");
	//	for (int i = 0; i < planet->size; ++i) printf("%3d", i);
	//	printf("\n");
	//	for (int i = 0; i < planet->size; ++i) {
	//		printf("%3d", i);
	//		for (int j = 0; j < planet->size; ++j) {
	//			printf("%3d", dummy[index(i, j, planet->size)]);

	//		}
	//		printf("\n");
	//	}
	//}

	// Checks for discrepencies between the battle and the paths.
	for (int i = 0; i < planet->size * planet->size; ++i) {

		// There is a discrepency when there is a moving unit and the dummy is not assigned.
		if ((battlefield[i].units[1].quantity && battlefield[i].units[1].moving) && !dummy[i]) {
			int x;
			int y;
			deIndex(x, y, i, planet->size);
			printf("%p : discrepency at tile [%d, %d], dummy : %d, moving : %d\n", planet, x, y, dummy[i], battlefield[i].units[1].quantity && battlefield[i].units[1].moving);
			ret = true;

		}
	}

	// Releases the dummy.
	releaseDummy(dummy);

	// Returns false if all goes well.
	return ret;

}

/*
TODO DEBUG REMOVE
verifies that fronts have proper values
*/
void Battle::verifyFronts() {
	int tot1 = 0, tot2 = 0;
	int xPos, yPos;
	uint_least32_t strengths[MAX_GROUND_FRONTS];
	uint_least32_t enemyStrengths[MAX_GROUND_FRONTS];
	uint_least32_t numTiles[MAX_GROUND_FRONTS];
	uint_least8_t* dummy;
	memset(strengths, 0, sizeof(strengths[0]) * MAX_GROUND_FRONTS);
	memset(enemyStrengths, 0, sizeof(enemyStrengths[0]) * MAX_GROUND_FRONTS);
	memset(numTiles, 0, sizeof(numTiles[0]) * MAX_GROUND_FRONTS);

	// Get dummy.
	dummy = requestDummy();

	// Find strengths and numTiles.
	for (int i = 0; i < planet->size * planet->size; ++i) {
		if (battlefield[i].owner) {
			strengths[battlefield[i].front] += groundUnitTileStrength(battlefield[i]);
			++numTiles[battlefield[i].front];

		}
	}

	// Find enemyStrengths for each front.
	for (int currFront = 1; currFront < comp.short1; ++currFront) {

		// Clear dummy.
		memset(dummy, 0, planet->size * planet->size * sizeof(uint_least8_t));

		// Checks every tile.
		for (int i = 0; i < planet->size; ++i) {
			for (int j = 0; j < planet->size; ++j) {

				// Gets enemy strength.
				if (gbIndex(i, j, planet).front == currFront) {
					for (int x = -1; x < 2; ++x) {
						for (int y = -1; y < 2; ++y) {
							if (!x && !y) continue;
							xPos = i + x;
							yPos = j + y;
							wrapAroundPlanet(planet->size, &xPos, &yPos);

							// If at war and not already counted, adds value. 
							if (comp.comp(gbIndex(i, j, planet).owner, gbIndex(xPos, yPos, planet).owner) && !dummy[index(xPos, yPos, planet->size)]) {
								enemyStrengths[currFront] += groundUnitTileStrength(gbIndex(xPos, yPos, planet));
								dummy[index(xPos, yPos, planet->size)] = 1;

							}
						}
					}
				}
			}
		}
	}

	// Release the dummy.
	releaseDummy(dummy);

	// Compare strengths, enemyStrengths and numTiles.
	for (int i = 1; i < comp.short1; ++i) {
		if ((uint_least32_t)strengths[i] != (uint_least32_t)fronts[i].strength) printf("front[%d] (stren) : measured %u, front %d\n", i, (uint_least32_t)strengths[i], fronts[i].strength);
		if ((uint_least32_t)enemyStrengths[i] != (uint_least32_t)fronts[i].enemyStrength) printf("front[%d] (enemy) : measured %u, front %d\n", i, (uint_least32_t)enemyStrengths[i], fronts[i].enemyStrength);
		if (numTiles[i] != fronts[i].numTiles) printf("front[%d] (numtl) : measured : %u, front : %u\n", i, numTiles[i], fronts[i].numTiles);

	}

	// Totals for strengths.
	for (int i = 1; i < comp.short1; ++i) {
		tot1 += strengths[i];
		tot2 += fronts[i].strength;
	}
	printf("totals: measured %d, front %d, difference %d\n", tot1, tot2, (int)(abs(tot1 - tot2)));

	// Finds the total strength of all enemyStrengths.
	tot2 = 0;
	int tot3 = 0;
	for (int i = 1; i < comp.short1; ++i) {
		tot2 += fronts[i].enemyStrength;
		tot3 += enemyStrengths[i];

	}

	// Finds the total strength on the planet.
	int tot4 = 0;
	for (int i = 0; i < planet->size * planet->size; ++i) tot4 += battlefield[i].units[0].quantity;
	printf("total units / total front units / measured enemyStrengths / enemyStrengths : %d, %d, %d, %d\n", tot4, tot1, tot3, tot2);

	// Prints comp.short1
	printf("numFronts : %d\n", (int)comp.short1);

}

// TODO DEBUG REMOVE
// Checks that all units have a unit specifier.
bool Battle::checkUnits() {
	for (int i = 0; i < planet->size * planet->size; ++i)  for (int j = 0; j < NUM_TILE_UNITS; ++j)
		if (battlefield[i].units[j].quantity && !battlefield[i].units[j].unit) return true;

	return false;
}

/*
Standard constructor for the Battle. Creates the battlefield and assigns some initial soldiers to it.
The above includes assigning owners to BattleTiles, drawing fronts, and assigning soldiers to fronts.
Note: Assigns the inputed planet to contain this battle.
*/
Battle::Battle(HabitablePlanet* planet) {

	// 'Initializes' comp.
	// TODO DEBUG REMOVE
	comp = ComparisonMatrix();

	// Initializes the battlefield.
	battlefield = new GroundBattleTile[planet->size * planet->size]();

	// Makes the HabitablePlanet and Battle reference one another.
	this->planet = planet;
	planet->battle = this;

	// Initializes owners. The first owner symbolizes unowned land and is empty.
	comp.byte1 = 1;
	owners = new GroundOwner[comp.byte1]();

	// Initializes fronts. The first front belongs to nobody.
	comp.short1 = 1;
	fronts = new GroundFront[comp.short1]();

	// Initializes the owners for the Battle.
	// comp.byte1 and owners are initialzed here.
	// comp is also initialized here.
	initOwners();

	// Initializes the fronts for the Battle.
	initFronts();

	// Assigns movement orders to units without fronts.
	moveToFronts();

}

/*
Deconstructor for a Battle.
Deletes arrays and zeros memory.
*/
Battle::~Battle() {

	// Dissociates the Battle from the planet.
	planet->battle = nullptr;

	// Deletes arrays.
	delete owners;
	delete fronts;
	delete battlefield;

	// Empties movements.
	movements.clear();

	// Clears comp.
	comp.clear();

}

/*
Main method of this class. Assigns and carries out movement orders for units on
the battlefield.
*/
void Battle::battle() {

	// Assigns movement orders to units without fronts.
	moveToFronts();

	// Moves units along their designated paths.
	moveUnits();

	if (checkPaths()) { printf("Path Discrepency\n"); gameSpeed = 0; changeActiveHabitable(planet); };

	// Assigns movement orders between fronts.
	//moveBetweenFronts();

	// Plans naval invasions.
	//navalInvade();

	// Diffuses units on fronts.
	diffuseUnits();

	// Causes units to attack one another.
	attackUnits();

	// Calculates the strength values for each front.
	calcFrontStrength();

	// TODO DEBUG REMOVE
	// REMOVE extern changeActiveHabitable
	//if (checkFronts()) { printf("Front Discrepency\n"); verifyFronts(); gameSpeed = 0; changeActiveHabitable(planet); }

}

/*
Finds the index of the inputed owner. Will be equal to comp.byte1 if there is no such index.
*/
int Battle::getOwner(void* owner) {
	int own = 0;
	for (; own < comp.byte1; ++own) if (owners[own].owner == owner) break;
	return own;

}

/*
Gets the first available unused front. If there is no such front, creates a new one.
The returned front will always have numTiles, strength, and enemyStrength equal to zero.
It is assumed that enemy and owner will be assigned immediately after this method is used.
*/
int Battle::getUnusedFront() {

	// Attempts to find an empty front.
	for (int i = 1; i < comp.short1; ++i) {
		if (!fronts[i].numTiles) {
			fronts[i].strength = 0;
			fronts[i].enemyStrength = 0;
			return i;

		}
	}

	// Creates a new GroundFront if no front was found.
	++comp.short1;
	fronts = (GroundFront*)realloc(fronts, sizeof(GroundFront) * comp.short1);
	fronts[comp.short1 - 1].numTiles = 0;
	fronts[comp.short1 - 1].strength = 0;
	fronts[comp.short1 - 1].enemyStrength = 0;
	return comp.short1 - 1;

}

// Includes government.
extern class Government;

/*
Creates the array of owners on the planet. Then, assigns appropriate tiles to each owner.
*/
void Battle::initOwners() {

	// Places each colony into owners and assigns owners in the battlefield.
	for (int i = 0; i < planet->size; ++i) {
		for (int j = 0; j < planet->size; ++j) {
			
			// Attempts to find the current tile's colony in owners.
			int own = 0;
			for (; own < comp.byte1; ++own) {

				// If the colony is found, assigns the owner.
				if (owners[own].owner == pOwner(i, j, planet).colony) {
					gbIndex(i, j, planet).owner = own;
					break;

				}
			}

			// If the colony is not found, creates a new owner and then assigns it.
			if (own == comp.byte1) {
				++comp.byte1;
				owners = (GroundOwner*)realloc(owners, sizeof(GroundOwner) * comp.byte1);
				owners[own].owner = pOwner(i, j, planet).colony;
				owners[own].unitTable = pOwner(i, j, planet).colony->government->unitTable;
				gbIndex(i, j, planet).owner = own;

			}
		}
	}

	// Creates the comparison matrix.
	// TODO TEMP make war declaration use relationships
	comp.init(comp.byte1);

	// Assigns all nations except unowned to be at war with one another.
	// TODO TEMP
	// TODO this is superfluous as war is bidirectional, half should declare against half
	for (int i = 1; i < comp.byte1 - 1; ++i) {
		for (int j = 1; j < comp.byte1 - 1; ++j) {
			comp.assign(i, j, 1);

		}
	}
}

/*
Checks if the inputed unit can occupy the inputed terrain.
*/
inline bool Battle::canOccupyTerrain(int x, int y, GroundUnitTemplate unit) {
	return unit.movementType == FlightMovement || // Flight is compatible with all types.
		(pBuilding(x, y, planet) >= Harbour1 && pBuilding(x, y, planet) <= Harbour5) || // All types are compatible with Harbours.
		(pIsLand(x, y, planet) && (unit.movementType == LimbMovement || unit.movementType == TrackMovement)) || // Land compatible.
		(!pIsLand(x, y, planet) && unit.movementType == SailMovement); // Sea compatible.

}

/*
Checks if the inputed unit can occupy the inputed terrain. Used for reinforcing fronts.
*/
inline bool Battle::canOccupyTerrain(int isLand, GroundUnitTemplate unit) {
	return unit.movementType == FlightMovement || // Flight is compatible with all types.
		(isLand && (unit.movementType == LimbMovement || unit.movementType == TrackMovement)) || // Land compatible.
		(!isLand && unit.movementType == SailMovement); // Sea compatible.
}

/*
Estimates the offensive strength of a certain quantity of a certain unit.
*/
inline int Battle::groundUnitStrength(uint_least8_t owner, uint_least8_t unit, uint_least8_t quantity) {
	return quantity * owners[owner].unitTable[unit].estimatedStrength;

}

/*
Estimates the offensive strength of a certain quantity of a certain unit.
*/
inline int Battle::groundUnitStrength(uint_least8_t owner, GroundUnit unit, uint_least8_t quantity) {
	return quantity * owners[owner].unitTable[unit.unit].estimatedStrength;

}

/*
Estimates the offensive strength of one army in a tile.
*/
inline int Battle::groundUnitStrength(GroundBattleTile tile, uint_least8_t unit) {
	if (tile.units[unit].unit) return tile.units[unit].quantity * owners[tile.owner].unitTable[tile.units[unit].unit].estimatedStrength;
	else return 0;

}

/*
Sums the estimated offensive strength of the given tile.
Moving units are not counted.
*/
inline int Battle::groundUnitTileStrength(GroundBattleTile tile) {
	int strength = 0;
	for (int i = 0; i < NUM_TILE_UNITS; ++i) if (tile.units[i].unit && !tile.units[i].moving) strength += tile.units[i].quantity * owners[tile.owner].unitTable[tile.units[i].unit].estimatedStrength;
	return strength;

}

/*
Returns the first owner adjacent to the owner on the inputed tile. Will return the inputed owner
if no adjacent owner is found. Though fronts are drawn against unowned tiles, non unowned tiles are
prioritized by this algorithm.

As a special case, this algorithm will return 0 in such case where the front of x,y is a land front
and is adjacent to a sea tile.
*/
int Battle::findAdjacentEnemy(int x, int y, int owner) {
	int tempOwner = owner;
	int xPos, yPos;

	// Attempts to find any adjacent owner that is not equal to the inputed owner.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// Returns the first adjacent owner that is found.
			if (gbIndex(xPos, yPos, planet).owner != owner) {

				// Returns if the owner is non null.
				if (gbIndex(xPos, yPos, planet).owner) return gbIndex(xPos, yPos, planet).owner;
				// If the owner is null, prepares to return it if no other owner is found.
				else tempOwner = gbIndex(xPos, yPos, planet).owner;

			}
			// Stores 0 if the inputed tile contains a landFront adjacent to sea.
			else if (pIsLand(x, y, planet) && !pIsLand(xPos, yPos, planet)) tempOwner = 0;

		}
	}

	// If there is no adjacent owner, returns either the inputed owner or 0,
	// depending on prior execution.
	return tempOwner;

}

/*
Calculates the strength of all adjacent enemies. This is meant to be used to update a front's
enemyStrength before a new tile is taken. Only the strength of enemy tiles which are not currently
adjacent to tiles belonging to front will be counted.

TODO use all indices
TODO make sure that only unmoving units are counted
*/
int Battle::findAdjacentEnemyStrength(int x, int y, int front) {
	int xPos, yPos;
	int enemyStrength = 0;

	// Sums the strength of all adjacent enemy tiles which are not adjacent to the inputed front.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// If there is an adjacent enemy tile which is not already adjacent to any
			// tile belonging to the inputed front, increases enemyStrength.
			if (comp.comp(fronts[front].owner, gbIndex(xPos, yPos, planet).owner) && !checkFrontAdjacency(xPos, yPos, front) && !gbIndex(xPos, yPos, planet).units[0].moving)
				enemyStrength += groundUnitTileStrength(gbIndex(xPos, yPos, planet));

		}
	}

	// Returns the calculated strength.
	return enemyStrength;

}

/*
Changes the enemyStrength of all adjacent enemy fronts by the inputed strength value.
This is used to enemy strength when units are added or removed from a front.
*/
void Battle::adjustAdjacentEnemyStrengths(int x, int y, int strength) {
	int xPos, yPos;
	int currEdit = 0;
	uint_least16_t edited[8];

	// Changes strength for each adjacent enemy front.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// If the adjacent enemy front has already been adjusted, does not change it further.
			for (int curr = 0; curr < currEdit; ++curr) if (edited[curr] == gbIndex(xPos, yPos, planet).front) goto skip;

			// If an adjacent front is at war with the current front and has not already been adjusted,
			// adjusts the enemyStrength of the front.
			if (comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner)) {
				edited[currEdit] = gbIndex(xPos, yPos, planet).front;
				++currEdit;
				fronts[gbIndex(xPos, yPos, planet).front].enemyStrength += strength;

			}

		// Will not adjust strength if the front has been edited.
		skip:;

		}
	}
}

/*
Checks to see if a tile on a planet is adjacent to a tile belonging to the inputed owner.
*/
int Battle::checkOwnerAdjacency(int x, int y, int owner) {
	int xPos, yPos;

	// Checks to see if any adjacent owner is equal to the inputed owner.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// Returns true if an adjacent tile with the inputed owner is found.
			if (gbIndex(xPos, yPos, planet).owner == owner) return true;

		}
	}
	
	// If an adjacent tile with the inputed owner is not found, returns false.
	return false;

}

/*
Checks to see if a tile on a planet is adjacent to a tile belonging to the inputed enemy.

This function will return false in such case where the inputed enemy is zero and there is a
non zero enemy nearby. This incites creation of new fronts.

This function will return true in such case where an inputed land front's enemy is zero, there
is no adjacent enemy tile, and there is an adjacent sea tile. This creates sea-wall fronts.
Note: It would be optimal to avoid checking for adjacent sea tiles when sea fronts are inputed,
	perhaps the function should be split.
*/
int Battle::checkEnemyAdjacency(int x, int y, GroundFront* front) {
	int special = 0;
	int xPos, yPos;

	// Checks to see if any adjacent owner is equal to the inputed owner.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// Checks to see if an adjacent tile with the inputed enemy is found.
			if (front->enemy && gbIndex(xPos, yPos, planet).owner == front->enemy) return true;
			// If the inputed enemy is unowned and there is an adjacent owned tile, returns false.
			else if (!front->enemy && gbIndex(xPos, yPos, planet).owner && gbIndex(xPos, yPos, planet).owner != front->owner) return false;
			// If the front's enemy is unowned and an unowned tile is found, marks that down.
			else if (!front->enemy && !gbIndex(xPos, yPos, planet).owner) special = 1;
			// If the front is a land front and its enemy is unowned, marks adjacent sea tiles.
			else if (!front->enemy && front->isLand && !pIsLand(xPos, yPos, planet)) special = 1;

		}
	}

	// If an adjacent tile with the inputed owner is not found, returns false.
	if (front->enemy) return false;
	// In the case that the inputed enemy is unowned, return true if no enemy tiles were
	// found and an adjacent zero tile or sea tile was found.
	else if (special) return true;

	// Returns false in the case that the only adjacent tiles belong to the inputed owner.
	return false;

}

/*
Checks to see if the inputed tile is adjacent to any tile containing the inputed front.
*/
int Battle::checkFrontAdjacency(int x, int y, int front) {
	int xPos, yPos;

	// Checks to see if any adjacent owner is equal to the inputed front.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// Returns 1 if a tile containing the inputed front is found.
			if (gbIndex(xPos, yPos, planet).front == front) return 1;

		}
	}

	// Returns 0 if no adjacent tile containing the inputed front is found.
	return 0;

}

/*
Entry point for the recursive extendFront methods. Wraps conditions deciding between the two
for convenience.
*/
inline void Battle::extendFront(int currX, int currY, int front) {
	if (fronts[front].isLand) extendLandFront(currX, currY, front);
	else extendSeaFront(currX, currY, front);

}

/*
Recursive function which extends the given ground front in all directions.
Extends the front to tiles which
	- Are owned by the inputed owner.
	- Are adjacent to the inputed enemy.
	- Do not belong to this front.
*/
void Battle::extendLandFront(int currX, int currY, int front) {
	int xPos, yPos;

	// Returns if the current tile belongs to this front.
	if (gbIndex(currX, currY, planet).front == front) return;

	// Returns if the current tile is not a land tile.
	if (!pIsLand(currX, currY, planet)) return;

	// Returns if the current tile does not belong to the inputed owner.
	if (gbIndex(currX, currY, planet).owner != fronts[front].owner) return;

	// Returns if the current tile is not adjacent to the inputed enemy.
	if (!checkEnemyAdjacency(currX, currY, &fronts[front])) return;

	// Changes the number of tiles which this front and the overwritten front possess.
	// Note: since fronts[0] is unused, underflow from decrementation is meaningless.
	++fronts[front].numTiles;
	--fronts[gbIndex(currX, currY, planet).front].numTiles;

	// Changes the tile's front.
	gbIndex(currX, currY, planet).front = front;

	// Attempts to continue to draw the front in each direction.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = currX + i;
			yPos = currY + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);
			extendLandFront(xPos, yPos, front);

		}
	}
}

/*
Recursive function which extends the given sea front in all directions.
Extends the front to tiles which
	- Are owned by the inputed owner.
	- Are adjacent to the inputed enemy.
	- Do not belong to this front.
*/
void Battle::extendSeaFront(int currX, int currY, int front) {
	int xPos, yPos;

	// Returns if the current tile belongs to this front.
	if (gbIndex(currX, currY, planet).front == front) return;

	// Returns if the current tile is not a sea tile.
	if (pIsLand(currX, currY, planet)) return;

	// Returns if the current tile does not belong to the inputed owner.
	if (gbIndex(currX, currY, planet).owner != fronts[front].owner) return;

	// Returns if the current tile is not adjacent to the inputed enemy.
	if (!checkEnemyAdjacency(currX, currY, &fronts[front])) return;

	// Changes the number of tiles which this front and the overwritten front possess.
	// Note: since fronts[0] is unused, underflow from decrementation is meaningless.
	++fronts[front].numTiles;
	--fronts[gbIndex(currX, currY, planet).front].numTiles;

	// Changes the tile's front.
	gbIndex(currX, currY, planet).front = front;

	// Attempts to continue to draw the front in each direction.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = currX + i;
			yPos = currY + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);
			extendSeaFront(xPos, yPos, front);

		}
	}
}

/*
Extends the front in the given tile. Assumes that this tile has undergone some state change.
This function will assign the inputed owner to be the inputed tile's owner.
*/
void Battle::extendFrontsHelper(int x, int y, int owner) {
	int front;
	int enemy;
	int isLand;
	int xPos, yPos;

	// Assigns the owner of this tile.
	gbIndex(x, y, planet).owner = owner;

	// If this tile belongs to a valid front, does nothing.
	if (fronts[gbIndex(x, y, planet).front].owner == owner && fronts[gbIndex(x, y, planet).front].enemy &&
		checkOwnerAdjacency(x, y, fronts[gbIndex(x, y, planet).front].enemy))
		return;

	// Marks whether the tile is land or sea.
	isLand = pIsLand(x, y, planet);

	// Finds any adjacent fronts belonging to the inputed owner.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// In order to extend a front, there must be a nearby front belonging to the inputed
			// owner which is associated with an adjacent enemy. The front must be on the same
			// terrain type as the tile.
			front = gbIndex(xPos, yPos, planet).front;
			if (front && isLand == pIsLand(xPos, yPos, planet) && gbIndex(xPos, yPos, planet).owner == owner &&
				checkEnemyAdjacency(x, y, &fronts[front])) {
				extendFront(x, y, front);
				return;

			}
		}
	}

	// Creates a new front if no adjacent front is available and there is a nearby enemy.
	if ((enemy = findAdjacentEnemy(x, y, owner)) == owner) return;
	front = getUnusedFront();
	fronts[front].owner = owner;
	fronts[front].enemy = enemy;
	fronts[front].isLand = pIsLand(x, y, planet);
	extendFront(x, y, front);

}

/*
Extends the front in the given tile. Assumes that this tile has just been taken by the inputed owner.
Will also extend fronts and perform front cleanup in the surrounding area.
*/
void Battle::extendFronts(int x, int y, int owner) {
	int xPos, yPos;

	// Extends the fronts for this tile.
	extendFrontsHelper(x, y, owner);

	// Extends fronts for every surrounding tile.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (!i && !j) continue;
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);
			if (gbIndex(xPos, yPos, planet).owner) extendFrontsHelper(xPos, yPos, gbIndex(xPos, yPos, planet).owner);

		}
	}

	// Cleans up nearby unused front tiles.
	for (int i = -3; i < 4; ++i) {
		for (int j = -3; j < 4; ++j) {
			xPos = x + i;
			yPos = y + j;
			wrapAroundPlanet(planet->size, &xPos, &yPos);

			// Zeros the front on a tile if it has no adjacent enemies.
			if (findAdjacentEnemy(xPos, yPos, gbIndex(xPos, yPos, planet).owner) == gbIndex(xPos, yPos, planet).owner) {

				// TODO DEBUG REMOVE keeps front if it is land and has adjacent sea
				//if (checkEnemyAdjacency(xPos, yPos, &fronts[gbIndex(xPos, yPos, planet).front])) continue;
				--fronts[gbIndex(xPos, yPos, planet).front].numTiles;
				gbIndex(xPos, yPos, planet).front = 0;

			}
		}
	}
}

/*
Extends the front in the given tile. Assumes that this tile has just been taken by the inputed colony.
This method is used by external code and maps the inputed colony to an owner.
*/
void Battle::extendFronts(int x, int y, Colony* owner) {
	extendFronts(x, y, getOwner(owner));

}

/*
Brings reinforcements into the Battle. Will attempt to use all given reinforcements, prioritizing outnumbered fronts.
Returns the number of unplaced units.
*/
int Battle::reinforce(void* own, int numReinforcements, int unit) {
	int owner;
	int numUnits;
	int lastFront = -1;
	int remainder = numReinforcements;
	double weights[MAX_GROUND_FRONTS];
	double totalWeight = 0;

	// Finds the owner.
	owner = getOwner(own);

	// Determines the weights of all fronts which belong to the owner and have appropriate
	// Terrain for the given unit.
	memset(weights, 0, sizeof(weights));
	for (int front = 0; front < comp.short1; ++front) {
		if (fronts[front].numTiles && fronts[front].owner == owner &&
			canOccupyTerrain(fronts[front].isLand, owners[owner].unitTable[unit])) {
			weights[front] = pow(2, -((long int)fronts[front].strength - (long int)fronts[front].enemyStrength));
			totalWeight += weights[front];
			lastFront = front;

		}
	}

	// Reinforces all fronts belonging to the owner.
	for (int front = 0; front < lastFront; ++front) {
		if (weights[front]) {
			numUnits = numReinforcements * (weights[front] / totalWeight);
			reinforceFront(front, numUnits, unit);
			remainder -= numUnits;

		}
	}

	// Reinforces the last front. Uses all remaining units.
	if (lastFront >= 0) reinforceFront(lastFront, remainder, unit);

	// Returns the number of remaining units in such case where there are still some remaining.
	return remainder;

}

/*
Reinforces a front.

TODO Could be optimized by reinforcing all fronts at once.
	store queue of reinforcements, place at outset of battle.
*/
void Battle::reinforceFront(int front, int numUnits, int unit) {
	int numTiles = fronts[front].numTiles;
	int num = numUnits / numTiles;

	// Places tiles by algorithm if the number of units is not small.
	if (num) {

		// Reinforces every tile belonging to the front.
		for (int x = 0; x < planet->size; ++x) {
			for (int y = 0; y < planet->size; ++y) {
				if (gbIndex(x, y, planet).front == front) {
					--numTiles;

					// Places remaining units and quits if no tiles are left.
					if (!numTiles) {
						placeUnits(x, y, numUnits - num * (fronts[front].numTiles - 1), unit);
						return;

					}
					// Places an appropriate amount of remaining units if more tiles are left.
					else placeUnits(x, y, num, unit);

				}
			}
		}
	}
	// If the number of units is small, places them at the first available tile.
	else {
		for (int x = 0; x < planet->size; ++x) {
			for (int y = 0; y < planet->size; ++y) {
				if (gbIndex(x, y, planet).front == front) {
					placeUnits(x, y, numUnits, unit);
					return;

				}
			}
		}
	}
}

/*
Places units in the given tile. Note that the index of the unit being created should be attainable externally,
as units in a battle reference the unit roster of the associated government.

If this is used to shuffle units inwards, the units being moved should be made 0 preemptively.

// TODO make this algorithm account for range
*/
inline void Battle::placeUnits(int x, int y, uint_least8_t quantity, uint_least8_t unit) {
	uint_least8_t transferQuantity;

	// Attempts to place all units in the tile, starting from units[0].
	for (int i = 0; quantity && i < NUM_TILE_UNITS; ++i) {
		if (gbIndex(x, y, planet).units[i].unit == unit || !gbIndex(x, y, planet).units[i].unit) {

			// Transfers an appropriate amount of units.
			transferQuantity = 255 - gbIndex(x, y, planet).units[i].quantity;
			transferQuantity = transferQuantity > quantity ? quantity : transferQuantity;
			quantity -= transferQuantity;

			// Moves the units inwards.
			gbIndex(x, y, planet).units[i].unit = unit;
			gbIndex(x, y, planet).units[i].quantity += transferQuantity;

		}
	}
}

/*
Removes units from the given tile. Note that the index of the unit being created should be attainable externally,
as units in a battle reference the unit roster of the associated government.

TODO make a sister function for moving units
TODO when more units than the tile contains are removed, nothing happens. It is imaginable that there may be some
	need to indicate this to the caller.
*/
inline void Battle::removeUnits(int x, int y, uint_least8_t quantity, uint_least8_t unit) {
	uint_least8_t transferQuantity;

	// Attempts to remove quantity of the inputed unit from the tile, starting from the highest index.
	for (int i = NUM_TILE_UNITS; i >= 0; --i) {
		if (gbIndex(x, y, planet).units[i].unit == unit && !gbIndex(x, y, planet).units[i].moving) {

			// Removes the appropriate quantity of units.
			if (gbIndex(x, y, planet).units[i].quantity > quantity) transferQuantity = quantity;
			else transferQuantity = gbIndex(x, y, planet).units[i].quantity;
			gbIndex(x, y, planet).units[i].quantity -= transferQuantity;
			quantity -= transferQuantity;

			// Shifts inwards if a unit is emptied.
			if (!gbIndex(x, y, planet).units[i].quantity) moveUnitsInwards(i, gbIndex(x, y, planet));

		}
	}
}


/*
Moves units inwards.

// TODO do something about moving units and other things
*/
inline void Battle::moveUnitsInwards(int start, GroundBattleTile& tile) {
	for (int i = start; i < NUM_TILE_UNITS - 1; ++i) tile.units[i] = tile.units[i + 1];
	tile.units[NUM_TILE_UNITS - 1] = { 0, 0, 0 };

}

/*
Returns a Direction showing which adjacent tiles contain enemies. Will return
a direction pointing to unowned tiles if there are no adjacent enemies.
Used for renderHabitableArmy.
*/
Direction Battle::enemyDirection(int x, int y) {
	int xPos, yPos;
	Direction enemies;
	Direction unowned;

	// Northwest.
	xPos = x - 1;
	yPos = y - 1;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.nw = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.nw = !gbIndex(xPos, yPos, planet).owner;
	// North.
	xPos = x;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.n = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.n = !gbIndex(xPos, yPos, planet).owner;
	// Northeast.
	xPos = x + 1;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.ne = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.ne = !gbIndex(xPos, yPos, planet).owner;
	// East.
	yPos = y;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.e = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.e = !gbIndex(xPos, yPos, planet).owner;
	// Southeast.
	yPos = y + 1;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.se = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.se = !gbIndex(xPos, yPos, planet).owner;
	// South.
	xPos = x;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.s = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.s = !gbIndex(xPos, yPos, planet).owner;
	// Southwest.
	xPos = x - 1;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.sw = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.sw = !gbIndex(xPos, yPos, planet).owner;
	// West.
	yPos = y;
	wrapAroundPlanet(planet->size, &xPos, &yPos);
	enemies.d.w = comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner);
	unowned.d.w = !gbIndex(xPos, yPos, planet).owner;

	// Returns the direction.
	if (enemies.i) return enemies;
	else return unowned;

}

/*
Creates the initial fronts for a battlefield. Fronts are created along the length of all borders
between owners, including borders between real owners and unnocupied land.
*/
void Battle::initFronts() {
	uint_least8_t owner, enemy;

	// Finds a tile which is not associated with a front.
	for (int i = 0; i < planet->size; ++i) {
		for (int j = 0; j < planet->size; ++j) {

			// If an owned tile which does not possess a front is found, attempts to assign it a front.
			if (gbIndex(i, j, planet).owner && !gbIndex(i, j, planet).front) {

				// Gets the owner and enemy.
				owner = gbIndex(i, j, planet).owner;
				enemy = findAdjacentEnemy(i, j, owner);

				// Will not create a front if no adjacent enemy colony is found.
				if (owner == enemy) continue;

				// Creates a new GroundFront for the new front.
				++comp.short1;
				fronts = (GroundFront*)realloc(fronts, sizeof(GroundFront) * comp.short1);
				fronts[comp.short1 - 1] = {0, 0, 0, pIsLand(i, j, planet), owner, enemy};

				// Draws the new front.
				extendFront(i, j, comp.short1 - 1);

			}
		}
	}
}

/*
Establishes paths for idle units to move to their nearest fronts. The paths will avoid unmoving
units and enemy territory completely. The destination of each path will be the nearest allied front
which can accept the idle unit.

coords[0].x stores the length of the path, coords[0].y stores the current index in the path.
coords[1] stores {unit, quantity}.

TODO fix use of indices (should count down).
*/
void Battle::moveToFronts() {
	bool pathFound;
	uint_least8_t unit;
	uint_least8_t quantity;
	uint_least8_t owner;

	// Creates movement orders to carry every unit to the nearest front.
	for (int x = 0; x < planet->size; ++x) {
		for (int y = 0; y < planet->size; ++y) {

			// Moves units if they are not on a front or already moving.
			for (int i = 0; i < NUM_TILE_UNITS; ++i) {
				if (gbIndex(x, y, planet).units[i].quantity && !gbIndex(x, y, planet).front && !gbIndex(x, y, planet).units[i].moving) {

					// Marks the unit.
					unit = gbIndex(x, y, planet).units[i].unit;
					quantity = gbIndex(x, y, planet).units[i].quantity;
					owner = gbIndex(x, y, planet).owner;

					// Creates a path for the unit.
					pathFound = planet->planetPathfind(x, y, planet->size,

						// endCondition lambda. Ends when an unnocupied tile with a front is found or
						// when an occupied tile containing less than 255 of the given unit is found.
						[unit, quantity, owner](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->bool {
							GroundBattleTile tile = pBattleIndex(currPos->value->coord.x, currPos->value->coord.y, planet);
							return tile.front && tile.owner == owner && // front of correct owner.
								(!tile.units[0].quantity || // empty.
								(tile.units[0].unit == unit && tile.units[0].quantity < 255)); // same unit with valid quantity.

						},

						// action lambda. Creates a path.
						[this, unit, quantity](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos) {
							LinkedListNode<PathCoordinate>* counter = currPos;

							// Does nothing if the gValue is invalid.
							if (currPos->value->gValue <= 0) return;

							// Creates a list of Coords corresponding to the movement.
							// coords[0].x stores the length of the path + 2, coords[0].y stores the current index in the path.
							Coord* coords = new Coord[(int)currPos->value->gValue + 3]();
							coords[0].x = (uint_least8_t)currPos->value->gValue + 2; // Adds 2 for more convenient comparisons with y later.
							coords[0].y = 2; // Starts at 2 since the first 2 indices are used for metadata.

							// Marks down the number of units and the unit type.
							coords[1] = {unit, quantity};

							// Writes down the list of coordinates to be travelled.
							for (; currPos != nullptr; currPos = currPos->previous) {
								coords[(int)currPos->value->gValue + 2].x = currPos->value->coord.x;
								coords[(int)currPos->value->gValue + 2].y = currPos->value->coord.y;

							}

							// Adds the Coords to the LinkedList.
							movements.add(coords);

						},

						// calcG lambda. gValue change is always one for each tile, unless there are other factors.
						[this, unit, quantity, owner](HabitablePlanet* planet, int xBeg, int yBeg, int xDest, int yDest)->int {
							return 256 * (pBattleIndex(xDest, yDest, planet).owner != owner || // Tile not owned by the unit's owner.
								!canOccupyTerrain(xDest, yDest, owners[owner].unitTable[unit]) || // Tile incompatible with current unit.
								(pBattleIndex(xDest, yDest, planet).units[NUM_TILE_UNITS - 1].quantity && !pBattleIndex(xDest, yDest, planet).units[NUM_TILE_UNITS - 1].moving && // Unit in way.
								!(pBattleIndex(xDest, yDest, planet).units[0].unit == unit && pBattleIndex(xDest, yDest, planet).front && pBattleIndex(xDest, yDest, planet).units[0].quantity < 255))) // Does not match endCondition.
								+ 1;

						},

						// calcH lambda. hValue is always zero as this is a breadth first search.
						[](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->double {
							return 0;

						}
						);

					// Marks the unit as moving if pathdinfing was successful.
					// TODO make sure that 2 is not overwritten
					if (pathFound) {
						gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1] = gbIndex(x, y, planet).units[i];
						gbIndex(x, y, planet).units[i] = { 0, 0, 0 };
						gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1].moving = 1;

					}
				}
			}
		}
	}
}

/*
Reallocates units between fronts.

// TODO implement
// TODO use additional indices
*/
void Battle::moveBetweenFronts() {
	//int strengthDiffs[MAX_GROUND_FRONTS];

	//// Parses through each front and calculates the strength difference between it and the enemy front.
	//for (int i = 0; i < comp.short1; ++i) {
	//	if (!fronts[i].numTiles) continue;
	//	strengthDiffs[i] = fronts[i].strength;

	//}

}

/*
Moves units along their designated paths. Units prefer to use higher indices while moving.

coords[0].x stores the length of the path, coords[0].y stores the current index in the path.
coords[1] stores {unit, quantity}.
*/
void Battle::moveUnits() {
	uint_least8_t quantity;
	uint_least8_t unit;
	Coord currCoord;
	Coord nextCoord;
	LinkedListNode<Coord>* currNode = movements.root;
	LinkedListNode<Coord>* tempNode;

	// Moves units along their paths.
	while (currNode != nullptr) {
		currCoord = currNode->value[currNode->value->y];
		nextCoord = currNode->value[currNode->value->y + 1];
		++currNode->value->y;

		// If the owner of nextCoord is not the owner of currCoord, stops pathfinding.
		if (gbIndex(currCoord.x, currCoord.y, planet).owner != gbIndex(nextCoord.x, nextCoord.y, planet).owner) {
			goto stop;

		}
		// If nextCoord is the final coord and contains no unit, places the moving unit into the tile.
		if (currNode->value->y >= currNode->value->x && !gbIndex(nextCoord.x, nextCoord.y, planet).units[0].quantity) {
			
			// Places the units in the next tile.
			placeUnits(nextCoord.x, nextCoord.y, gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity, gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].unit);

			// Removes the units from the previous tile.
			gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1] = { 0, 0, 0 };
			gbIndex(nextCoord.x, nextCoord.y, planet).units[0].moving = 0;

			// Stops pathfinding.
			goto cleanup;

		}
		// If nextCoord is the final coord and contains the correct unit, attempts to merge the armies.
		else if (currNode->value->y >= currNode->value->x &&
			gbIndex(nextCoord.x, nextCoord.y, planet).units[0].unit == gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].unit) {

			// Moves as many troops as possible to the target.
			quantity = 255 - gbIndex(nextCoord.x, nextCoord.y, planet).units[0].quantity;
			quantity = quantity > gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity ? gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity : quantity;
			gbIndex(nextCoord.x, nextCoord.y, planet).units[0].quantity += quantity;
			gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity -= quantity;

			// Ends pathfinding for the unit.
			if (!gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity) gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1] = { 0, 0, 0 };
			else gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].moving = 0;

			// Stops pathfinding. Will fold this unit into a front if it is currently on one.
			// Note that accurate front strengths require goto stop (this is not an appropriate place to check during debugging).
			goto stop;

		}
		// If the army in the next tile cannot be merged into, stops pathfinding.
		else if (currNode->value->y >= currNode->value->x) {
			goto stop;

		}
		// If the army is not merging, will stop pathfinding and recalculate the path if nextCoord is occupied.
		else if (gbIndex(nextCoord.x, nextCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity) {
			goto stop;

		}
		// Moves the unit from the current position to the next position.
		else {

			// Moves the unit.
			gbIndex(nextCoord.x, nextCoord.y, planet).units[NUM_TILE_UNITS - 1] = gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1];
			gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1] = { 0, 0, 0 };

			// Moves on to the next node.
			currNode = currNode->child;

			// Continues pathfinding.
			continue;

		}

	// Stops pathfinding and moves the current unit inward.
	stop:

		// Moves units inwards when pathfinding fails and there are still surplus units available.
		quantity = gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].quantity;
		unit = gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1].unit;
		gbIndex(currCoord.x, currCoord.y, planet).units[NUM_TILE_UNITS - 1] = { 0, 0, 0 };
		placeUnits(currCoord.x, currCoord.y, quantity, unit);

	// Stops pathfinding and cleans up resources.
	cleanup:

		// Deletes the current movement order.
		delete currNode->value;
		tempNode = currNode;
		currNode = currNode->child;
		// TODO more efficient, less lazy deletion
		movements.remove(tempNode->value);
		continue;

	}
}

/*
Diffuses units across fronts. Will attempt to keep the number of units of each tile of each front relatively
similar to the number of units on adjacent tiles. Will only transfer units from one front to another if the
receiving front is in need of reinforcement.

TODO currently transfers to any adjacent available front. Should instead tranfer to only
the current front unless the adjacent front is higher priority
TODO currently only transfers unit[0]. Should transfer all units.
*/
void Battle::diffuseUnits() {
	uint_least8_t numTransferred;
	int maxTransfer;
	int xPos, yPos;
	uint_least8_t* received = requestDummy();
	memset(received, 0, planet->size * planet->size * sizeof(uint_least8_t));

	// Searches every tile on the planet for an available unit to split.
	for (int x = 0; x < planet->size; ++x) {
		for (int y = 0; y < planet->size; ++y) {

			// If a tile with a front and a unit is found, attempts to diffuse units from the tile.
			if (gbIndex(x, y, planet).front && gbIndex(x, y, planet).units[0].quantity) {
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						if (!i && !j) continue;
						xPos = x + i;
						yPos = y + j;
						wrapAroundPlanet(planet->size, &xPos, &yPos);

						// Transfers half of the difference between the two tile's unit counts if an adjacent tile has the same unit type
						// and has less units than the current tile. Tiles cannot supply units that they have just received.
						// Tiles can also supply to empty tiles which have a viable terrain type for the transfering unit.
						if (gbIndex(xPos, yPos, planet).owner == gbIndex(x, y, planet).owner && gbIndex(xPos, yPos, planet).front && // Same owner and front exists.
							!gbIndex(x, y, planet).units[0].moving && !gbIndex(xPos, yPos, planet).units[0].moving && // Neither units are moving
							(gbIndex(xPos, yPos, planet).units[0].unit == gbIndex(x, y, planet).units[0].unit || // Same unit.
								(!gbIndex(xPos, yPos, planet).units[0].unit && canOccupyTerrain(xPos, yPos, gbUnitTemplate(x, y, 0, planet)))) && // Or empty with correct type
							gbIndex(xPos, yPos, planet).units[0].quantity < gbIndex(x, y, planet).units[0].quantity) { // Destination has lower quantity.

							// Maximum number of units which this tile can transfer.
							maxTransfer = gbIndex(x, y, planet).units[0].quantity - received[index(x, y, planet->size)];
							if (maxTransfer <= 0) continue;

							// Transfers units between the fronts.
							numTransferred = (gbIndex(x, y, planet).units[0].quantity - gbIndex(xPos, yPos, planet).units[0].quantity) / 2;
							if (numTransferred > maxTransfer) numTransferred = maxTransfer;
							gbIndex(x, y, planet).units[0].quantity -= numTransferred;
							gbIndex(xPos, yPos, planet).units[0].quantity += numTransferred;
							gbIndex(xPos, yPos, planet).units[0].unit = gbIndex(x, y, planet).units[0].unit;

							// Tracks the number of units this tile has received.
							received[index(xPos, yPos, planet->size)] += numTransferred;

						}
					}
				}
			}
		}
	}

	// Releases the received dummy.
	releaseDummy(received);

}

/*
Causes enemy units to attack each other. Each unit gets only one attack.
If there is no adjacent enemy to attack, attempts to conquer an adjacent tile.
*/
void Battle::attackUnits() {
	uint_least8_t attackerQuantity, defenderQuantity;
	uint_least16_t tempFront;
	int xPos, yPos;
	uint_least8_t* conquered;

	// Uses a dummy to track which tiles have been conquered this far.
	conquered = requestDummy();
	memset(conquered, 0, planet->size * planet->size * sizeof(uint_least8_t));

	// Checks every tile on the planet for a unit.
	for (int x = 0; x < planet->size; ++x) {
		for (int y = 0; y < planet->size; ++y) {

			// Will only attack when there is a unit to attack with.
			if (gbIndex(x, y, planet).units[0].unit && !gbIndex(x, y, planet).units[0].moving) {

				// Attempts to find a unit to launch an attack against.
				for (int i = -1; i < 2; ++i) {
					for (int j = -1; j < 2; ++j) {
						if (!i && !j) continue;
						xPos = x + i;
						yPos = y + j;
						wrapAroundPlanet(planet->size, &xPos, &yPos);

						// When an adjacent enemy tile is found, performs a battle.
						if (comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner) && gbIndex(xPos, yPos, planet).units[0].quantity) {
							attackerQuantity = gbIndex(x, y, planet).units[0].quantity;
							defenderQuantity = gbIndex(xPos, yPos, planet).units[0].quantity;

							// Calculates the casualties of the attackers and defenders.
							determineGroundCasualties(owners[gbIndex(x, y, planet).owner].unitTable[gbIndex(x, y, planet).units[0].unit],
								owners[gbIndex(xPos, yPos, planet).owner].unitTable[gbIndex(xPos, yPos, planet).units[0].unit],
								&gbIndex(x, y, planet).units[0].quantity, &gbIndex(xPos, yPos, planet).units[0].quantity);

							// Clears depleted units.
							if (!gbIndex(x, y, planet).units[0].quantity) moveUnitsInwards(0, gbIndex(x, y, planet));
							if (!gbIndex(xPos, yPos, planet).units[0].quantity) moveUnitsInwards(0, gbIndex(xPos, yPos, planet));

							// Will not attempt to launch more attacks from this tile.
							goto exit;

						}
					}
				}

			// Exit point after an attack is performed.
			exit:;

				// Will perform this action if and only if a unit remains.
				if (gbIndex(x, y, planet).units[0].unit) {

					// Conquers nearby empty enemy tiles which have not already been conquered this turn.
					for (int i = -1; i < 2; ++i) {
						for (int j = -1; j < 2; ++j) {
							if (!i && !j) continue;
							xPos = x + i;
							yPos = y + j;
							wrapAroundPlanet(planet->size, &xPos, &yPos);

							// Conquers nearby unoccupied enemy tiles which the conquering unit can occupy.
							// Note: It is assumed that units[0].unit will be 0 if and only if all enemy units in the tile
							// have been destroyed.
							if (!conquered[index(xPos, yPos, planet->size)] && !gbIndex(xPos, yPos, planet).units[0].unit &&
								/*canOccupyTerrain(xPos, yPos, gbUnitTemplate(x, y, 0, planet)) &&*/
								comp.comp(gbIndex(x, y, planet).owner, gbIndex(xPos, yPos, planet).owner)) {

								// Marks the tile as conquered so it will not flutter.
								conquered[index(xPos, yPos, planet->size)] = 1;

								// Removes control of the tile from its current owner.
								tempFront = gbIndex(xPos, yPos, planet).front;
								--fronts[tempFront].numTiles;
								gbIndex(xPos, yPos, planet).front = 0; // Needs to be done for findAdjacentEnemyStrength and extendFronts to work.

								// Places the tile under control of the attacker.
								extendFronts(xPos, yPos, gbIndex(x, y, planet).owner);

							}
						}
					}
				}
			}
		}
	}

	// Returns conquered.
	releaseDummy(conquered);

}

/*
Plans naval invasions.

TODO make naval invasions more dynamic.
*/
void Battle::navalInvade() {
	uint_least8_t unit;
	uint_least8_t quantity;
	uint_least8_t owner;

	// Finds harbours which contain more than 200 units.
	for (int x = 0; x < planet->size; ++x) {
		for (int y = 0; y < planet->size; ++y) {

			// Plans naval invasions for harbours which contain over 200 units.
			// TODO count all units
			if (pBuilding(x, y, planet) >= Harbour1 && pBuilding(x, y, planet) <= Harbour5 &&
				gbIndex(x, y, planet).units[0].quantity >= 200) {

				// Pathfinds to the nearest unowned harbour.
				// Marks the unit as moving.
				// TODO make sure that 2 is not overwritten
				gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1] = gbIndex(x, y, planet).units[x];
				gbIndex(x, y, planet).units[x] = { 0, 0, 0 };
				gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1].moving = 1;
				unit = gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1].unit;
				quantity = gbIndex(x, y, planet).units[NUM_TILE_UNITS - 1].quantity;
				owner = gbIndex(x, y, planet).owner;

				// Creates a path for the unit.
				planet->planetPathfind(x, y, planet->size / 2,

					// endCondition lambda. Ends when an enemy harbour is found.
					[unit, quantity, owner, this](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->bool {
						return pBuilding(currPos->value->coord.x, currPos->value->coord.y, planet) >= Harbour1 && // harbour.
							pBuilding(currPos->value->coord.x, currPos->value->coord.y, planet) <= Harbour5 &&
							comp.comp(owner, gbIndex(currPos->value->coord.x, currPos->value->coord.y, planet).owner); // at war.

					},

					// action lambda. Creates a path.
					[this, unit, quantity](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos) {
						LinkedListNode<PathCoordinate>* counter = currPos;

						// Does nothing if the gValue is invalid.
						if (currPos->value->gValue <= 0) return;

						// Creates a list of Coords corresponding to the movement.
						// coords[0].x stores the length of the path, coords[0].y stores the current index in the path.
						Coord* coords = new Coord[(int)currPos->value->gValue + 3]();
						coords[0].x = (uint_least8_t)currPos->value->gValue + 2; // Adds 2 for more convenient comparisons with y later.
						coords[0].y = 2; // Starts at 2 since the first 2 indices are used for metadata.

						// Marks down the number of units and the unit type.
						coords[1] = { unit, quantity };

						// Writes down the list of coordinates to be travelled.
						for (; currPos != nullptr; currPos = currPos->previous) {
							coords[(int)currPos->value->gValue + 2].x = currPos->value->coord.x;
							coords[(int)currPos->value->gValue + 2].y = currPos->value->coord.y;

						}

						// Adds the Coords to the LinkedList.
						movements.add(coords);

					},

					// calcG lambda. gValue change is always one for each tile, unless the end criteria are reached.
					[unit, quantity, owner, this](HabitablePlanet* planet, int xBeg, int yBeg, int xDest, int yDest)->int {
						return 256 * (gbIndex(xDest, yDest, planet).owner != owner || // Tile is not owned by this owner.
							!canOccupyTerrain(xDest, yDest, owners[owner].unitTable[unit]) || // Tile incompatible with current unit.
							(gbIndex(xDest, yDest, planet).units[NUM_TILE_UNITS - 1].quantity && !gbIndex(xDest, yDest, planet).units[NUM_TILE_UNITS - 1].moving && // Unit in way.
								!(pBuilding(xDest, yDest, planet) >= Harbour1 && pBuilding(xDest, yDest, planet) <= Harbour5 && comp.comp(owner, gbIndex(xDest, yDest, planet).owner)))) // Does not match endCondition.
							+ 1;

					},

						// calcH lambda. hValue is always zero as this is a breadth first search.
					[](HabitablePlanet* planet, LinkedListNode<PathCoordinate>* currPos)->double {
						return 0;

					}
					);
			}
		}
	}
}

/*
Calculates and assigns front strength.
*/
void Battle::calcFrontStrength() {
	int xPos, yPos;
	uint_least32_t strengths[MAX_GROUND_FRONTS];
	uint_least32_t enemyStrengths[MAX_GROUND_FRONTS];
	memset(strengths, 0, sizeof(strengths[0]) * MAX_GROUND_FRONTS);
	memset(enemyStrengths, 0, sizeof(enemyStrengths[0]) * MAX_GROUND_FRONTS);
	uint_least8_t* dummy;

	// Gets dummy, an array of bitarrays used to track adjacency for fronts.
	dummy = requestLargeDummy();
	memset(dummy, 0, planet->size * planet->size * 64);

	// Finds strengths.
	for (int i = 0; i < planet->size * planet->size; ++i) if (battlefield[i].owner) strengths[battlefield[i].front] += groundUnitTileStrength(battlefield[i]);

	// Checks every tile.
	for (int i = 0; i < planet->size; ++i) {
		for (int j = 0; j < planet->size; ++j) {

			// Gets enemy strengths.
			if (gbIndex(i, j, planet).front) {
				for (int x = -1; x < 2; ++x) {
					for (int y = -1; y < 2; ++y) {
						if (!x && !y) continue;
						xPos = i + x;
						yPos = j + y;
						wrapAroundPlanet(planet->size, &xPos, &yPos);

						// If at war and not already counted, adds enemyStrength.
						if (comp.comp(gbIndex(i, j, planet).owner, gbIndex(xPos, yPos, planet).owner) && !readArrBit(&dummy[index(xPos, yPos, planet->size) * 64], gbIndex(i, j, planet).front)) {
							enemyStrengths[gbIndex(i, j, planet).front] += groundUnitTileStrength(gbIndex(xPos, yPos, planet));
							writeArrBit(&dummy[index(xPos, yPos, planet->size) * 64], gbIndex(i, j, planet).front, 1);

						}
					}
				}
			}
		}
	}

	// Release the dummy.
	releaseLargeDummy(dummy);

	// Assigns the strengths.
	for (int i = 0; i < comp.short1; ++i) {
		fronts[i].strength = strengths[i];
		fronts[i].enemyStrength = enemyStrengths[i];

	}
}

// gbIndex and gbUnitTemplate are not useful outside of Battles.
#undef gbIndex
#undef gbUnitTemplate