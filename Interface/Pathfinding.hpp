#pragma once

// Structure containing x and y coordinates.
struct Coordinate {
	int x;
	int y;

};

// Small structure containing x and y coordinates.
struct Coord {
	uint_least8_t x;
	uint_least8_t y;

};

// Union containing 8 directions within 8 bits.
struct Dir {
	uint_least8_t nw : 1;
	uint_least8_t n : 1;
	uint_least8_t ne : 1;
	uint_least8_t e : 1;
	uint_least8_t se : 1;
	uint_least8_t s : 1;
	uint_least8_t sw : 1;
	uint_least8_t w : 1;

};

// Union containing 8 directions within 8 bits.
union Direction {
	Dir d;
	uint_least8_t i;

};

// Equivalence operator for a Coordinate. Compares x and y.
bool operator==(const Coordinate& coord, const Coordinate& other) {
	return coord.x == other.x && coord.y == other.y;

}

// Non-equivalence operator for a Coordinate. Compares x and y.
bool operator!=(const Coordinate& coord, const Coordinate& other) {
	return coord.x != other.x || coord.y != other.y;

}

// Not operator for a Coordinate. True if x and y are 0.
bool operator!(const Coordinate& coord) {
	return !coord.x && !coord.y;

}

// Equivalence operator for a Coord. Compares x and y.
bool operator==(const Coord& coord, const Coord& other) {
	return coord.x == other.x && coord.y == other.y;

}

// Non-equivalence operator for a Coord. Compares x and y.
bool operator!=(const Coord& coord, const Coord& other) {
	return coord.x != other.x || coord.y != other.y;

}

// Not operator for a Coord. True if x and y are 0.
bool operator!(const Coord& coord) {
	return !coord.x && !coord.y;

}

// Structure containing a path coordinate.
struct PathCoordinate {
	Coordinate coord;
	float gValue;
	float hValue;
	Direction dir; // Prev direction.
	bool info; // used in planetPathfind to note pole wrap.
	// Spare byte 1.
	// Spare byte 2.

};

// Equivalence operator for a PathCoordinate. Compares the coord, ignores gValue.
bool operator==(const PathCoordinate& coord, const PathCoordinate& other) {
	return coord.coord == other.coord;

}

// Non-equivalence operator for a PathCoordinate. Compares the coord, ignores gValue.
bool operator!=(const PathCoordinate& coord, const PathCoordinate& other) {
	return coord.coord != other.coord;

}

// Comparison operators for a PathCoordinate. Compares the gValue and hValue.
bool operator>(const PathCoordinate& coord, const PathCoordinate& other) {
	return (coord.gValue + coord.hValue) > (other.gValue + other.hValue);

}
bool operator<(const PathCoordinate& coord, const PathCoordinate& other) {
	return (coord.gValue + coord.hValue) < (other.gValue + other.hValue);

}
bool operator>=(const PathCoordinate& coord, const PathCoordinate& other) {
	return (coord.gValue + coord.hValue) > (other.gValue + other.hValue) ||
		coord.gValue + coord.hValue == other.gValue + other.hValue;

}
bool operator<=(const PathCoordinate& coord, const PathCoordinate& other) {
	return (coord.gValue + coord.hValue) < (other.gValue + other.hValue) ||
		coord.gValue + coord.hValue == other.gValue + other.hValue;

}

/*
Finds the complement of the inputed Direction.
i.e. inputing a Direction with nw and w will return a Direction with se and e.
*/
Direction getOppositeDir(Direction dir) {
	Direction opp;
	opp.i = 0;

	// This works because of the way Direction is structured.
	// Opposite directions are 4 bits away.
	opp.i += dir.i >> 4;
	opp.i += dir.i << 4;

	// Returns the direction.
	return opp;

}

/*
Adds a direction to the inputed Direction* based on two inputed coordinates.
TODO UNUSED REMOVE.
*/
void planetCoordDirection(int x1, int y1, int x2, int y2, Direction* dir) {
	int xDir = x2 - x1;
	int yDir = y2 - y1;

	// Gets direction.
	if (xDir == -1 && yDir == -1) dir->d.nw = true;
	else if (!xDir && yDir == -1) dir->d.n = true;
	else if (xDir == 1 && yDir == -1) dir->d.ne = true;
	else if (xDir == 1 && !yDir) dir->d.e = true;
	else if (xDir == 1 && yDir == 1) dir->d.se = true;
	else if (!xDir && yDir == 1) dir->d.s = true;
	else if (xDir == -1 && yDir == 1) dir->d.sw = true;
	else if (xDir == -1 && !yDir) dir->d.w = true;

}

/*
Adds a direction to the inputed Direction* based on two inputed Coordinates.
TODO UNUSED REMOVE.
*/
void planetCoordDirection(Coordinate start, Coordinate dest, Direction* dir) {
	int xDir = dest.x - start.x;
	int yDir = dest.y - start.y;

	// Gets direction.
	if (xDir == -1 && yDir == -1) dir->d.nw = true;
	else if (!xDir && yDir == -1) dir->d.n = true;
	else if (xDir == 1 && yDir == -1) dir->d.ne = true;
	else if (xDir == 1 && !yDir) dir->d.e = true;
	else if (xDir == 1 && yDir == 1) dir->d.se = true;
	else if (!xDir && yDir == 1) dir->d.s = true;
	else if (xDir == -1 && yDir == 1) dir->d.sw = true;
	else if (xDir == -1 && !yDir) dir->d.w = true;

}

// Global lists of pathfinding coordinates. Reused to avoid unecessary allocating/deallocating 
// of coordinates during pathfinding.
// There will be one list of coords per thread.
LinkedList<PathCoordinate>** coordLists;
bool* coordAccess;

/*
Initializes the coordinate list to contain at least the given number of coordinates.
*/
void initCoordLists(int numItems) {
	int size = 0;
	int needed;
	int numLists = numThreads;

	// Creates the coordLists if none have been created.
	if (coordLists == nullptr) {
		coordLists = new LinkedList<PathCoordinate>* [numLists];
		coordAccess = new bool[numLists];
		for (int i = 0; i < numLists; ++i) {
			coordLists[i] = new LinkedList<PathCoordinate>();
			coordAccess[i] = true;

		}
	}

	// Calculates the total amount of coordinates needed.
	for (int i = 0; i < numLists; ++i) size += numItems - coordLists[i]->size();

	// Creates enough LinkedListNodes to fill the gap.
	LinkedListNode<PathCoordinate>* nodes = (LinkedListNode<PathCoordinate>*)malloc(sizeof(LinkedListNode<PathCoordinate>) * size);
	PathCoordinate* coords = (PathCoordinate*)malloc(sizeof(PathCoordinate) * size);

	// Places the coords into the nodes.
	for (int i = 0; i < size; ++i) nodes[i].value = coords++;

	// Initializes each coordList to contain at least numItems PathCoordinates.
	int currNode = 0;
	for (int i = 0; i < numLists; ++i) {
		needed = numItems - coordLists[i]->size();
		for (int j = 0; j < needed; ++j) {
			coordLists[i]->addc(nodes++);

		}
	}
}

/*
Initializes the coordinate list to contain at least 10000 coords.
TODO make amount of coords dynamic, like dummy size
*/
void initCoordLists() {

	// Calls initCoordLists with 10000 as the argument.
	initCoordLists(10000);

}

/*
Allows threads to request access to a coordList. Should always return a value.
*/
LinkedList<PathCoordinate>* requestCoordList() {
	static std::mutex coordListMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(coordListMutex);

	// Finds the first available coordList.
	// It is assumed that at least one will be available at any time.
	int i;
	for (i = 0;; ++i) {

		// If i is accessible, breaks.
		if (coordAccess[i]) {
			coordAccess[i] = false;
			break;

		}
	}

	// Returns the coordList.
	return coordLists[i];

}

/*
Allows threads to release access rights to a coordList.
*/
void releaseCoordList(LinkedList<PathCoordinate>* inputList) {
	int numLists = numThreads;

	// Finds the coordList in the array and marks it as available.
	for (int i = 0; i < numLists; ++i)
		if (coordLists[i] == inputList)
			coordAccess[i] = true;

}