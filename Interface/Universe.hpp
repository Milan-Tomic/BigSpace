#pragma once

/*
This header contains information on closures as well as functions for modifying universe tiles.
*/

// The universe used for gameplay.
GalaxyTile* universe;

// Stores all systems used in the universe.
System* allSystemSpace;

// Mutex mediating access to the universe.
std::mutex universeMutex;

// Removes the current owner of the inputed GalaxyTile.
void removeUniverseOwner(int xPos, int yPos);

// Gives the inputed tile to the inputed Government.
// Can use a Coordinate instead of two coordinates.
void changeUniverseOwner(int xPos, int yPos, Government * owner);

// Manages consolidation and splitting of closures.
void manageClosures();

/*
Closures represent supply areas associated for fleets. They are accessible
via the 'closure' element of GalaxyTiles. Each SuplyClosure stores an int
representing the number of tiles associated with it, allowing closures to be
reformatted when GalaxyTiles change hands.

New closures are assigned using the clock algorithm.
*/
struct SpaceClosure {

	// Government owner of this closure.
	Government* owner;

	// Starting coordinate associated with this closure.
	// TODO could be the centroid.
	Coordinate start;

	// Marks that the closure has been modified.
	uint_least32_t modified : 1;

	// Number of tiles associated with this closure.
	uint_least32_t numTiles : 31;

};

// Assumes that only 1024 closures will be needed.
#define NUM_CLOSURES 1024

// Stores all closures.
SpaceClosure allClosures[NUM_CLOSURES];

/*
Finds and returns an empty closure. Note that 0 is reserved for unowned GalaxyTiles.

Note: Access should be mutexed outside of this function.

TODO add some activity when the array of closures is full.
*/
int placeClosure() {
	static int curr = 1;

	// Stores the original closure.
	int orig = curr;

	// Finds an available closure. Will loop when NUM_CLOSURES is reached.
	while (allClosures[curr].numTiles) {
		curr = curr + 1 >= NUM_CLOSURES ? 1 : curr + 1;

		// TODO add activity when closures is full.
		if (curr == orig) {
			printf("Insufficient closures\n");
			exit(1);

		}
	}

	// Returns the current closure.
	return curr;

}

/*
Removes the current owner of the inputed GalaxyTile.

Note: Access should be mutexed outside of this function.
*/
void removeUniverseOwner(int xPos, int yPos) {

	// Decrements the number of unoccupied tiles in the Galaxy if the tile is unowned.
	if (!uIndex(xPos, yPos).closure) {
		--findGalaxy(xPos, yPos)->unowned;
		return;

	}

	// Decrements the number of tiles associated with the current closure.
	--uClosure(xPos, yPos).numTiles;

	// Marks the closure as modified.
	uClosure(xPos, yPos).modified = 1;

}

/*
Gives the inputed tile to the inputed Government.
*/
void changeUniverseOwner(int xPos, int yPos, Government* owner) {
	int closure;

	// Disables concurrent acces to this function.
	universeMutex.lock();

	// Removes the previous owner from the tile.
	removeUniverseOwner(xPos, yPos);

	// Finds an adjacent closure belonging to the inputed Government.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (uOwner(xPos + i, yPos + j) == owner) {
				closure = uIndex(xPos + i, yPos + j).closure;
				goto exit;

			}
		}
	}

	// Places a new closure if no closure is found.
	closure = placeClosure();
	allClosures[closure].start = { xPos, yPos };
	++allClosures[closure].numTiles;
	owner->addClosure(closure);

	// Goes here if a valid adjacent closure is found.
	exit:;

	// Assigns the closure of the inputed tile.
	uIndex(xPos, yPos).closure = closure;

	// Increments the number of tiles associated with this closure.
	++allClosures[closure].numTiles;

	// Marks the closure as modified.
	allClosures[closure].modified = 1;

	// TODO if there are multiple closures belonging to the Government, consolidates them.

	// Unlocks the mutex.
	universeMutex.unlock();

}

/*
Gives the inputed tile ot the inputed Government.
*/
void changeUniverseOwner(Coordinate location, Government* owner) {
	changeUniverseOwner(location.x, location.y, owner);

}

/*
Manages consolidation and splitting of closures.

TODO mark each closure as modified from changeUniverseOwner/removeUniverseOwner.
	review each marked closure. consolidate/split closures as necessary.
*/
void manageClosures() {

	// TODO marks all closures as unmodified.
	for (int i = 0; i < NUM_CLOSURES; ++i) allClosures[i].modified = 0;

}