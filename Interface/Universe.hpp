#pragma once

/*
This header contains information on closures as well as functions for modifying universe tiles.
*/

// The universe used for gameplay.
GalaxyTile* universe;

// Stores all systems used in the universe.
System* allSystemSpace;

// Mutex mediating access to the universe.
std::shared_mutex universeMutex;

// Removes the current owner of the inputed GalaxyTile.
void removeUniverseOwner(int xPos, int yPos);

// Gives the inputed tile to the inputed Government.
// Can use a Coordinate instead of two coordinates.
void changeUniverseOwner(int xPos, int yPos, Government * owner);

// Manages consolidation and splitting of closures for the inputed page.
void manageClosures(int page);

/*
Returns true if the inputed xPos and yPos are within the universe bounds,
otherwise false.
*/
inline bool universeInBounds(int xPos, int yPos) {
	return xPos < universeWidth && xPos >= 0 && yPos < universeHeight && yPos >= 0;

}

/*
Closures represent supply areas associated with squadrons. They are accessible
via the 'closure' element of GalaxyTiles. Each SupplyClosure stores an int
representing the number of tiles associated with it, allowing closures to be
reformatted when GalaxyTiles change hands.
*/
struct SpaceClosure {

	// Government owner of this closure.
	Government* owner;

	// Starting coordinate associated with this closure.
	Coordinate start;

	// Number of tiles associated with this closure.
	uint_least32_t numTiles : 30;

	// Marks that the closure may have a split.
	uint_least32_t split : 1;

	// Marks that the closure may need reconciliation.
	uint_least32_t rec : 1;

};

// The number of SpaceClosures in a ClosurePage.
#define CLOSURE_PAGE_SIZE 1024

// Default number of closure pages. This is also the number used when expanding the roster of
// closure pages.
#define NUM_CLOSURE_PAGES 256

// Page of closures.
struct ClosurePage {
	SpaceClosure closures[CLOSURE_PAGE_SIZE];
	uint16_t arrCurrClosure; // Should store the lowest empty closure, update upon change
	uint16_t numOpen;
	bool active : 1;
	bool dirty : 1;

	// X bytes of padding?

};

// Stores all closures.
int numClosurePages;
ClosurePage** closurePages;

/*
Initializes allClosures. Will round to the next multiple of NUM_CLOSURES.
*/
void initClosures(int numClosures) {

	// Frees closurePages for reuse.
	if (numClosurePages) {
		for (int i = 0; i < numClosurePages; ++i) {
			if (closurePages[i]) delete closurePages[i];
			closurePages[i] = nullptr;

		}
		delete[] closurePages;

	}

	// Initializes closurePages.
	numClosurePages = numClosures / CLOSURE_PAGE_SIZE + 1;
	closurePages = (ClosurePage**)calloc(numClosurePages + NUM_CLOSURE_PAGES - (numClosurePages % NUM_CLOSURE_PAGES), sizeof(ClosurePage*));
	for (int i = 0; i < numClosurePages; ++i) {
		closurePages[i] = (ClosurePage*)calloc(1, sizeof(ClosurePage));
		closurePages[i]->numOpen = CLOSURE_PAGE_SIZE;

	}

	// Ensures that the first closure of the first page is reserved for empty tiles.
	closurePages[0]->arrCurrClosure = 1;

}

/*
Places a closure. Note that 0 is reserved for unowned GalaxyTiles.

Note: Access should be mutexed outside of this function.
*/
int placeClosure() {
	int closure = 0;

	// Attempts to place the closure within one of the current pages.
	for (int page = 0; page < numClosurePages; ++page) {
		if (closurePages[page]->numOpen) {

			// Occupies the first empty closure.
			closure = page * CLOSURE_PAGE_SIZE + closurePages[page]->arrCurrClosure;
			--closurePages[page]->numOpen;

			// Finds and stores the next empty closure on the page.
			if (closurePages[page]->numOpen) while (closurePages[page]->closures[++closurePages[page]->arrCurrClosure].owner != nullptr);

			// Returns the closure upon success.
			return closure;

		}
	}

	// If placement failed, creates a new page and places the closure there.
	// Will only place when all threads are at this function or returned.
	if (!closure) {

		// If the page table is not large enough, reallocates it.
		if (!(numClosurePages + 1 % NUM_CLOSURE_PAGES))
			closurePages = (ClosurePage**)realloc(closurePages, (numClosurePages + NUM_CLOSURE_PAGES) * sizeof(ClosurePage*));

		// Creates a new ClosurePage.
		closurePages[numClosurePages] = (ClosurePage*)calloc(1, sizeof(ClosurePage));
		closurePages[numClosurePages]->numOpen = CLOSURE_PAGE_SIZE;

		// Places the closure in the first index of the new page.
		closure = (numClosurePages + 1) * CLOSURE_PAGE_SIZE;
		++closurePages[numClosurePages]->arrCurrClosure;

		// Moves on to the next page.
		++numClosurePages;

	}

	// Returns the current closure.
	return closure;

}

/*
Marks a closure as available by assigning all of its values to 0 and incrementing
numOpen in its page.

NOTE: This has not been debugged.
NOTE: This should never be called for closure 0.
*/
void freeClosure(int clos) {
	SpaceClosure* closure = &closurePages[clos / CLOSURE_PAGE_SIZE]->closures[clos % CLOSURE_PAGE_SIZE];

	// Frees the closure.
	memset(closure, 0, sizeof(SpaceClosure));

	// Increments numOpen.
	++closurePages[clos / CLOSURE_PAGE_SIZE]->numOpen;

}

/*
Requests a ClosurePage. Returns the index of the next available page in closurePages, otherwise -1.
*/
int requestDirtyClosurePage() {
	static std::shared_mutex requestClosurePageMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(requestClosurePageMutex);

	// Parses through each galaxy and returns the next available one.
	for (int i = 0; i < numClosurePages; ++i) {
		if (!closurePages[i]->active && closurePages[i]->dirty) {
			closurePages[i]->active = true;
			closurePages[i]->dirty = false;
			return i;

		}
	}

	// Returns -1 if no ClosurePage was found.
	return -1;

}

/*
Returns a ClosurePage.
*/
void releaseClosurePage(int index) {
	closurePages[index]->active = false;

}

/*
Returns all ClosurePages.
*/
void releaseAllClosurePages() {
	for (int i = 0; i < numClosurePages; ++i) closurePages[i]->active = false;

}

/*
Pathfinds from the inputed centroid to the nearest path associated with the
inputed closure. Pathfinding is done here to simplify assignClosureStart.

Note: It could be more efficient to replace this function with one that
	iterates over every universe tile and calculates the one with the minimum
	distance to the centroid.
*/
void assignClosureStart(int xPos, int yPos, SpaceClosure* closure, int nClosure) {
	LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();
	LinkedList<PathCoordinate>* coords = requestCoordList();
	uint_least8_t* explored = requestDummy();
	LinkedListNode<PathCoordinate>* currTile;

	// Resets explored.
	memset(explored, 0, universeWidth * universeHeight * sizeof(uint_least8_t));

	// Starts from the calculated centroid.
	// Note: xPos and yPos are reused later.
	currTile = coords->popc();
	currTile->value->coord = {xPos, yPos};
	explored[index(xPos, yPos, universeWidth)] = true;
	frontier.addc(currTile);

	// Explores until a suitable start is found.
	while (!frontier.isEmpty()) {

		// Explores the first tile in the frontier.
		currTile = frontier.popc();
		xPos = currTile->value->coord.x;
		yPos = currTile->value->coord.y;

		// If the current tile is associated with the closure, ceases exploration.
		if (uIndex(xPos, yPos).closure == nClosure) break;

		// Queues all unexplored tiles which are adjacent to the current tile.
		actionDirection(xPos, yPos,
			[&frontier, &coords, &explored](int x, int y)->void {
				LinkedListNode<PathCoordinate>* tile;

				// Does not go out of bounds of the universe.
				if (!universeInBounds(x, y)) return;

				// Adds unexplored adjacent tiles to the frontier.
				if (!explored[index(x, y, universeWidth)]) {
					explored[index(x, y, universeWidth)] = true;
					tile = coords->popc();
					tile->value->coord.x = x;
					tile->value->coord.y = y;
					frontier.addc(tile);

				}
			}
		);

		// Returns the used coord to the coordlist.
		coords->addc(currTile);

	}

	// Assigns start to the appropriate tile.
	closure->start = currTile->value->coord;

	// Returns all tiles to the coordlist.
	while (!frontier.isEmpty()) coords->addc(frontier.popc());
	coords->addc(currTile);

	// Releases held data.
	releaseCoordList(coords);
	releaseDummy(explored);

}

/*
Assigns the starting position of the closure to be roughly the centroid of the closure.
*/
void assignClosureStart(SpaceClosure* closure, int nClosure) {
	long int xCent = 0;
	long int yCent = 0;
	
	// Sums the x and y values of all coordinates in this closure.
	for (int x = 0; x < universeWidth; ++x) {
		for (int y = 0; y < universeHeight; ++y) {
			if (uIndex(x, y).closure == nClosure) {
				xCent += x;
				yCent += y;

			}
		}
	}

	// Calculates the centroid of this closure.
	xCent /= closure->numTiles;
	yCent /= closure->numTiles;

	// If the calculated centroid is a part of the closure, assigns the centroid
	// and returns.
	if (uIndex(xCent, yCent).closure == nClosure) closure->start = { xCent, yCent };
	// If the calculated centroid is not part of the closure, pathfinds to the
	// nearest tile of the closure and uses that as the centroid.
	else assignClosureStart(xCent, yCent, closure, nClosure);

}

/*
Removes the current owner of the inputed GalaxyTile.

NOTE: This function merely does bookeeping relating to the owner's closure and
does not actually change the closure to 0.

NOTE: Access to this function should require universeMutex to be locked.
*/
void removeUniverseOwner(int xPos, int yPos) {
	SpaceClosure* closure = &uClosure(xPos, yPos);
	int nClosure = uIndex(xPos, yPos).closure;
	Direction dir;

	// Decrements the number of unoccupied tiles in the Galaxy and returns if the
	// tile is unowned.
	if (!nClosure) {
		--findGalaxy(xPos, yPos)->unowned;
		return;

	}

	// Decrements the number of tiles associated with the current closure.
	--closure->numTiles;

	// Releases the closure and returns if it has no associated tiles.
	if (!closure->numTiles) {
		freeClosure(nClosure);
		return;

	}

	// Checks to see if the closure may need a split.
	dir = fillDirection(xPos, yPos, [nClosure](int x, int y)->bool { return uIndex(x, y).closure == nClosure; });

	// Marks the closure for splitting if the tile is adjacent to two or more blocks.
	if (directionNumBlocks(dir) > 1) {
		closure->split = true;
		indexClosurePage(nClosure)->dirty = true;

	}

	// Attempts to find a new starting point for the closure if the start has been lost.
	if (closure->start.x == xPos && closure->start.y == yPos) assignClosureStart(closure, nClosure);

}

/*
Gives the inputed tile to the inputed Government. Will associate the added tile with an
appropriate existing closure or create a new closure.
*/
void changeUniverseOwner(int xPos, int yPos, Government* owner) {
	int closure;

	// Disables concurrent acces to this function.
	universeMutex.lock();

	// Removes the previous owner from the tile.
	removeUniverseOwner(xPos, yPos);

	// If the tile is unowned, assigns the de-jure owner of this tile to be
	// the inputed Government.
	if (!uOwner(xPos, yPos)) uOwner(xPos, yPos) = owner;

	// Finds an adjacent closure belonging to the inputed Government.
	for (int i = -1; i < 2; ++i) {
		for (int j = -1; j < 2; ++j) {
			if (uController(xPos + i, yPos + j) == owner) {
				closure = uIndex(xPos + i, yPos + j).closure;
				goto assignClosure;

			}
		}
	}

	// Places a new closure if no closure is found.
	// Note that all variables should be assigned since the closure may be reused.
	closure = placeClosure();
	indexClosure(closure).start = { xPos, yPos };
	owner->addClosure(closure);

	// Goes here if a valid adjacent closure is found.
	assignClosure:;

	// Assigns the inputed tile to an appropriate closure.
	uIndex(xPos, yPos).closure = closure;
	++indexClosure(closure).numTiles;

	// Unlocks the mutex.
	universeMutex.unlock();

	// If this tile bridges the gap between two closures owned by this government,
	// marks the closure for reconcilation.
	actionDirectionCond(xPos, yPos,
		[owner, closure](int i, int j)->bool {

			// Marks the closure for reconciliation if another owned closure is found.
			if (uController(i, j) == owner && uIndex(i, j).closure != closure) {
				indexClosure(closure).rec = true;
				indexClosurePage(closure)->dirty = true;
				return true;

			}

			// Continues operation if no owned closure is found.
			return false;

		}
	);
}

/*
Gives the inputed tile to the inputed Government.
*/
void changeUniverseOwner(CoordU loc, Government* owner) {
	changeUniverseOwner(loc.x, loc.y, owner);

}

/*
Changes the closure of a given tile. Assumes that the inputed tile is owned by
the owner of the given closure.

The replaced closure will be freed if it has no remaining tiles.

Note: Changes here should be replicated in splitClosure where new closures are assigned.
*/
void changeClosure(int xPos, int yPos, int closure) {
	SpaceClosure* tempClosure = &uClosure(xPos, yPos);
	int tempNClosure = uIndex(xPos, yPos).closure;

	// Removes the tile from its original closure.
	--tempClosure->numTiles;

	// Places the tile under control of the inputed closure.
	uIndex(xPos, yPos).closure = closure;
	++indexClosure(closure).numTiles;

	// Frees the replaced closure if it has no remaining tiles.
	if (!tempClosure->numTiles) freeClosure(tempNClosure);
	// Changes the start of the closure if it has been overwritten.
	else if (tempClosure->start.x == xPos && tempClosure->start.y == yPos)
		assignClosureStart(&indexClosure(closure), closure);

}

/*
Attempts to split closures marked for splitting. Does so by counting all
tiles of a given closure which are connected to the closure's start, then
splitting the closure in such case where the number of tiles connected
to the closure's start is not equal to the total number of tiles associated
with the closure. The closure which is split off will have this function
performed on it to make sure that three-way (and larger) splits are possible.

NOTE: If split stored the number of blocks instead of a boolean, the recursion
of this function could be simplified.

Note: This is written here to make manageClosures more comprehensible.
*/
void splitClosure(SpaceClosure* closure, uint_least16_t nClosure, LinkedList<PathCoordinate>* coords, uint_least8_t* explored) {
	LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();
	LinkedListNode<PathCoordinate>* currTile;
	int xPos, yPos;
	int numTiles = 0;
	int newClosure;

	// Resets eplored.
	memset(explored, 0, universeWidth * universeHeight);

	// Starts from the starting point of the closure.
	currTile = coords->popc();
	currTile->value->coord = closure->start;
	explored[index(closure->start.x, closure->start.y, universeWidth)] = true;
	frontier.addc(currTile);

	// Counts and marks down all tiles belonging to this closure which are
	// connected to the closure's start.
	while (!frontier.isEmpty()) {

		// Explores the first tile in the frontier.
		currTile = frontier.popc();
		xPos = currTile->value->coord.x;
		yPos = currTile->value->coord.y;

		// Queues all unexplored tiles in the current closure which are adjacent to the current tile.
		actionDirection(xPos, yPos,
			[&frontier, &coords, &explored, nClosure](int x, int y)->void {
				LinkedListNode<PathCoordinate>* tile;

				// Adds unexplored tiles belonging to this closure to the frontier.
				if (!explored[index(x, y, universeWidth)] && uIndex(x, y).closure == nClosure) {
					explored[index(x, y, universeWidth)] = true;
					tile = coords->popc();
					tile->value->coord.x = x;
					tile->value->coord.y = y;
					frontier.addc(tile);

				}
			}
		);

		// Returns the used coord to the coordlist.
		coords->addc(currTile);
		++numTiles;

	}

	// Splits the closure if the number of connected tiles is less than the
	// total number of tiles.
	// Note: I don't understand why <= works here, but it turns out that numTiles
	// is equal to closure->numTiles when there is a split numTiles - 1 : 1.
	if (numTiles < closure->numTiles) {

		// Adds all non-connected tiles to a new closure.
		newClosure = placeClosure();
		indexClosure(newClosure).owner = closure->owner;
		for (int i = 0; i < universeWidth * universeHeight; ++i) {
			if (universe[i].closure == nClosure && !explored[i]) {
				universe[i].closure = newClosure;
				++indexClosure(newClosure).numTiles;
				--indexClosure(nClosure).numTiles;
				if (!indexClosure(newClosure).start) deIndex(indexClosure(newClosure).start.x, indexClosure(newClosure).start.y, i, universeWidth);

			}
		}

		// If there was a split, checks the split closure to see if another split needs
		// to be performed (as would happen when a three-way intersection is lost).
		splitClosure(&indexClosure(newClosure), newClosure, coords, explored);

	}
}

/*
Entry function for recursive closure splitting. Allocates/frees shared memory.
*/
void splitClosure(SpaceClosure* closure, int nClosure) {
	LinkedList<PathCoordinate>* coords = requestCoordList();
	uint_least8_t* explored = requestDummy();

	// Marks that the closure no longer needs to be split.
	closure->split = false;

	// Splits the closure.
	splitClosure(closure, nClosure, coords, explored);

	// Releases held data.
	releaseCoordList(coords);
	releaseDummy(explored);

}

/*
Reconciles closures. Does so by spreading the inputed closure over all connected
owned tiles.

Note: This is written here to make manageClosures more comprehensible.
TODO: add functionality for merging attributes of closures when such 
	  attributes exist.
*/
void reconcileClosure(SpaceClosure* closure, uint_least16_t nClosure) {
	LinkedList<PathCoordinate>* coords;
	LinkedList<PathCoordinate> frontier = LinkedList<PathCoordinate>();
	LinkedListNode<PathCoordinate>* currTile;
	uint_least8_t* explored;
	int xPos, yPos;
	coords = requestCoordList();
	explored = requestDummy();
	memset(explored, 0, universeWidth * universeHeight);

	// Marks that the closure no longer needs to be reconciled.
	closure->rec = false;

	// Starts from the starting point of the closure.
	currTile = coords->popc();
	currTile->value->coord = closure->start;
	explored[index(closure->start.x, closure->start.y, universeWidth)] = true;
	frontier.addc(currTile);

	// Spreads this closure over all connected owned tiles.
	while (!frontier.isEmpty()) {

		// Explores the first tile in the frontier.
		currTile = frontier.popc();
		xPos = currTile->value->coord.x;
		yPos = currTile->value->coord.y;

		// Removes the tile from its closure and gives it to the inputed closure.
		changeClosure(xPos, yPos, nClosure);

		// Queues all owned and unexplored tiles adjacent to the current tile.
		actionDirection(xPos, yPos,
			[&frontier, &coords, explored, closure](int x, int y)->void {
				LinkedListNode<PathCoordinate>* tile;

				// Adds unexplored tiles belonging to this owner to the frontier.
				if (!explored[index(x, y, universeWidth)] && uController(x, y) == closure->owner) {
					explored[index(x, y, universeWidth)] = true;
					tile = coords->popc();
					tile->value->coord.x = x;
					tile->value->coord.y = y;
					frontier.addc(tile);

				}
			}
		);

		// Returns the used coord to the coordlist.
		coords->addc(currTile);

	}

	// Releases held data.
	releaseCoordList(coords);
	releaseDummy(explored);

}

/*
Manages consolidation and splitting of closures for a given page.

TODO implement multithreading for closure management.
	note this may require a mutex for each nation so that no two same-nation closures overwrite one-another.
*/
void manageClosures(int page) {
	SpaceClosure* closure;

	// Splits and reconciles all flagged closures.
	for (int currClosure = 0; currClosure < CLOSURE_PAGE_SIZE; ++currClosure) {
		closure = &closurePages[page]->closures[currClosure];

		// Attempts to split marked closures.
		if (closure->split) splitClosure(closure, page * CLOSURE_PAGE_SIZE + currClosure);
		// Attempts to reconcile marked closures.
		if (closure->rec) reconcileClosure(closure, page * CLOSURE_PAGE_SIZE + currClosure);

	}
}