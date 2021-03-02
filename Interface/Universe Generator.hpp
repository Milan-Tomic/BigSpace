#pragma once

/*
Enum containing instructions for galaxy generation.
*/
enum GalaxyType {
	RANDOM,
	RING,
	SPIRAL

};

// Generates a new universe.
void generateUniverse(int uniWidth, int uniHeight, int numGalaxies, int minDistance, struct GalaxyInstruction galInstr[]);

// Copies a galaxy to the universe.
void placeGalaxy(int galSize, int uniWidth, int uniHeight, int minDistance, Galaxy* galaxySpace, int** galaxy, int** universe);

// Deallocates the universe.
void deallocateUniverse(int width, int height);

// Initializes a RING GalaxyInstruction.
void initRingGalaxy(GalaxyInstruction* instr, int uniWidth, int uniHeight);

// Generates a ring shaped galaxy.
int** generateRingGalaxy(int width, int height, double coreSizeFraction);

// Initializes a SPIRAL GalaxyInstruction.
void initSpiralGalaxy(GalaxyInstruction* instr, int uniWidth, int uniHeight);

// Generates a spiral shaped galaxy.
int** generateSpiralGalaxy(int width, int numArms, double armSizeFraction, double radFraction);

// Clears a canvas(sets all tiles to ThinSpaceTiles).
void clearUniverseCanvas(int** canvas, int size);

// Copies a galaxies tileID's to a canvas.
void copyUniverseCanvas(int** sourceCanvas, int** copyCanvas, int size);

// Copies a dummy galaxy to a galaxy.
void universeToSpace(int** dummyGalaxy, int width, int height);

// Creates arms for a galaxy.
void createArms(int** dummyGalaxy, int** canvas, int width, int center, int armSize, int numArms, double slope);

// Creates a galactic core.
void createCore(int** coreless, int width, int center);

// Randomly rotates a galaxy.
void rotateGalaxy(int width, int center, int** toRotate, int** canvas);

// Spins a galaxy.
void spinGalaxy(int** toSpin, int** canvas, int width, int center, double radFraction);

// Find the starting parameter for galaxy spinning.
double findStartingParameter(int i, int j, int wCenter, int hCenter);

// Fill gaps in a galaxy.
void fillGaps(int** gapGalaxy, int** canvas, int width, int height, int threshold, int tileID);

// Fills holes in a galaxy.
void fillHoles(int** gapGalaxy, int** canvas, int width, int height);

// Finds system locations within the universe.
void placeSystems(int** universeDummy, Galaxy* galaxies, GalaxyInstruction* galInstrs);

// Generates SystemSpaceTile's and places them in the universe.
void beginGenerateSystems(Galaxy* galaxies, GalaxyInstruction* galInstrs);

/*
Creates the universe and fills it with galaxies. Does so by parsing through the inputed lists of
galaxy instructions and generating galaxies appropriate to them, or generating random galaxies
for each randGalaxy index. If a GalaxyInstruction has an uninitialized component, it will be initialized
randomly here.

Note: Galaxies of dimensions greater than the universe should be rejected earlier.
*/
void generateUniverse(int uniWidth, int uniHeight, int numGalaxies, int minDistance, struct GalaxyInstruction galInstr[]) {
	int size;
	int** galaxy;
	int** universeDummy = createCanvas(uniWidth, uniHeight);

	// Initializes the universeDummy to contain only -1s.
	for (int i = 0; i < universeWidth; ++i)
		for (int j = 0; j < universeHeight; ++j)
			universeDummy[i][j] = -1;

	// Initializes the array of Galaxies.
	galaxies = new Galaxy[numGalaxies];

	// Generates all galaxies for the universe.
	for (int i = 0; i < numGalaxies; ++i) {

		// If the global numGals is less than the local numGalaxies (a galaxy failed to generate),
		// reduces numGalaxies, left shifts all elements in galInstr[], and reassigns galaxies.
		if (numGals < numGalaxies) {
			--i;
			--numGalaxies;
			for (int j = i; j < numGalaxies; ++j) galInstr[j] = galInstr[j + 1];

			// Reassigns galaxies.
			Galaxy* newGalaxies = new Galaxy[numGalaxies];
			for (int g = 0; g < numGalaxies; ++g) newGalaxies[g] = galaxies[g];
			delete[] galaxies;
			galaxies = newGalaxies;

		}

		// Initializes any uninitialized values for the galaxies, then generates and places them.
		switch (galInstr[i].type) {

		// Creates and places a random galaxy.
		case RANDOM:

			// Assigns the random galaxy a type and then creates it.
			galInstr[i].type = rand() % 2 + 1;
			--i;
			break;

		// Creates and places a ring galaxy.
		case RING:

			// Initializes the parameters of the RING galaxy.
			initRingGalaxy(&galInstr[i], uniWidth, uniHeight);

			// Creates and places a ring galaxy.
			galaxy = generateRingGalaxy(galInstr[i].width, galInstr[i].height, galInstr[i].coreSizeFraction * 0.00499999);
			size = galInstr[i].width >= galInstr[i].height ? galInstr[i].width : galInstr[i].height;
			placeGalaxy(size, uniWidth, uniHeight, minDistance, &galaxies[i], galaxy, universeDummy);
			deallocateCanvas(galaxy, size);
			break;

		// Creates and places a spiral galaxy.
		case SPIRAL:

			// Initializes the parameters of the SPIRAL galaxy.
			initSpiralGalaxy(&galInstr[i], uniWidth, uniHeight);

			// Creates and places a spiral galaxy.
			galaxy = generateSpiralGalaxy(galInstr[i].width, galInstr[i].numArms, galInstr[i].armSizeFraction * 0.002 + 0.1, (100.0 / galInstr[i].radFraction) * 0.0004 + 0.02);
			placeGalaxy(galInstr[i].width, uniWidth, uniHeight, minDistance, &galaxies[i], galaxy, universeDummy);
			deallocateCanvas(galaxy, galInstr[i].width);
			break;

		}
	}

	// Reassigns the galaxyArray and galaxyInstructions to remove unplaced galaxies.
	Galaxy* tempGalaxies = new Galaxy[numGals];
	GalaxyInstruction* tempInstrs = new GalaxyInstruction[numGals];
	int galIndex = 0;
	for (int i = 0; i < numGals; ++i) {
		while (!galaxies[galIndex].area.w && !galaxies[galIndex].area.h) ++galIndex;
		tempGalaxies[i] = galaxies[galIndex];
		tempInstrs[i] = galInstr[galIndex];
		++galIndex;

	}
	delete[] galaxies;
	galaxies = tempGalaxies;

	// Creates the global dummies in Multithreading.hpp to be used later. Dummies are treated as being square.
	// TODO allow dummySize to account for barrens.
	int dummySize = universeWidth >= universeHeight ? universeWidth + 2 : universeHeight + 2;
	for (int i = 0; i < numGals; ++i) if (dummySize < tempInstrs[i].planetSize + 2) dummySize = tempInstrs[i].planetSize + 2;
	initDummies(dummySize * dummySize);

	// Creates the global large dummies in Multithreading.hpp to be used later. Dummies are treated as being square.
	// TODO allow dummySize to account for barrens.
	dummySize = 0;
	for (int i = 0; i < numGals; ++i) if (dummySize < tempInstrs[i].planetSize + 2) dummySize = tempInstrs[i].planetSize + 2;
	initLargeDummies(dummySize * dummySize);

	// Sets all Galaxies as available.
	releaseAllGalaxies();

	// Sets unowned for all Galaxies.
	for (int i = 0; i < numGals; ++i) {

		// Counts the unowned tiles in each Galaxy.
		galaxies[i].unowned = 0;
		for (int x = galaxies[i].area.x; x < galaxies[i].area.x + galaxies[i].area.w; ++x) {
			for (int y = galaxies[i].area.y; y < galaxies[i].area.y + galaxies[i].area.h; ++y) {
				if (universeDummy[x][y] != THIN_SPACE_TILE) ++galaxies[i].unowned;

			}
		}
	}

	// Places systems within the universeDummy.
	placeSystems(universeDummy, galaxies, tempInstrs);

	// Makes the universe from the universeDummy.
	universeToSpace(universeDummy, universeWidth, universeHeight);

	// Generates the systems of the universe.
	beginGenerateSystems(galaxies, tempInstrs);

	// Generates the empires of the universe.
	generateEmpiresUniverse(galaxies, tempInstrs, numGals);

	// Deallocates the universeDummy.
	deallocateCanvas(universeDummy, universeWidth, universeHeight);

	// Deletes the tempInstrs.
	delete tempInstrs;

}

/*
Finds a place to copy the galaxy into the universe, making sure that each galaxy is at least minDistance away from
and other galaxy. Then, calls copyGalaxy to copy in the galaxy.

Note: The first galaxy is always in the upper left corner of the universe.
Note: It is assumed that all inputed galaxies have already been put into square spaces.
*/
void placeGalaxy(int galSize, int uniWidth, int uniHeight, int minDistance, Galaxy* galaxySpace, int** galaxy, int** universe) {

	// If the universe is empty, copies the first galaxy to the upper left.
	if (universe[0][0] == -1) {

		// Copies the galaxy into the universe.
		for (int i = 0; i < galSize; ++i)
			for (int j = 0; j < galSize; ++j)
				universe[i][j] = galaxy[i][j];

		// Fills all tiles of at most minDistance away from the placed galaxy with ThinSpaceTiles.
		for (int i = 0; (i < galSize + minDistance) && (i < uniWidth); ++i) {
			for (int j = 0; (j < galSize + minDistance) && (j < uniHeight); ++j) {
				if (universe[i][j] == -1) universe[i][j] = THIN_SPACE_TILE;

			}
		}

		// Assigns the inputed Galaxy's area.
		galaxySpace->area = {0, 0, galSize, galSize};

		// Assigns the number of unowned tiles in the Galaxy.
		for (int i = galaxySpace->area.x; i < galSize; ++i) {
			for (int j = galaxySpace->area.y; i < galSize; ++i) {
				galaxySpace->unowned += universe[i][j] != THIN_SPACE_TILE;

			}
		}
	}
	// If the universe is not empty, attempts to find a random tile which is suitable for placing
	// the galaxy.
	else {
		bool foundSpot = false;
		int xOrigin;
		int yOrigin;

		// Tries (universe area) / 4 random locations for placement.
		for (int i = 0; i < (uniWidth * uniHeight) / 4 && !foundSpot; ++i) {
			foundSpot = true; // Assume a spot will be found until proven otherwise.

			// Tries to find an empty usable tile.
			xOrigin = rand() % (uniWidth - galSize - 1);
			yOrigin = rand() % (uniHeight - galSize - 1);

			// If the tile is empty, checks to see that the nearby area is empty.
			if (universe[xOrigin][yOrigin] == -1) {
				int wStart = xOrigin - minDistance > 0 ? xOrigin - minDistance : 0;
				int hStart = yOrigin - minDistance > 0 ? yOrigin - minDistance : 0;
				int wFinish = xOrigin + galSize + minDistance < uniWidth ? xOrigin + galSize + minDistance : uniWidth;
				int hFinish = yOrigin + galSize + minDistance < uniHeight ? yOrigin + galSize + minDistance : uniHeight;

				// Checks the corner tiles within range of the galaxy to see that there are no
				// intersecting galaxies.
				for (int w = wStart; w < wFinish && foundSpot; ++w) {
					if ((universe[w][hStart] != -1 &&
						universe[w][hStart] != THIN_SPACE_TILE) ||
						(universe[w][hFinish - 1] != -1 &&
							universe[w][hFinish - 1] != THIN_SPACE_TILE))
					    foundSpot = false;
					
				}

				// Checks the vertical edge tiles within range of the galaxy to see that there
				// are no intersecting galaxies. This is to save time.
				for (int h = hStart; h < hFinish && foundSpot; ++h) {
					if ((universe[wStart][h] != -1 &&
						universe[wStart][h] != THIN_SPACE_TILE) ||
						universe[wFinish - 1][h] != -1 &&
						universe[wFinish - 1][h] != THIN_SPACE_TILE)
						foundSpot = false;

				}

				// Checks each tile within range of the galaxy to see that there are no
				// intersecting galaxies.
				for (int w = wStart + 1; w < wFinish - 1 && foundSpot; ++w) {
					for (int h = hStart + 1; h < hFinish - 1 && foundSpot; ++h) {

						// Quits searching if another galaxy (a tile that is not NULL or ThinSpace)
						// is found.
						if (universe[w][h] != -1 &&
							universe[w][h] != THIN_SPACE_TILE) {
							foundSpot = false;

						}
					}
				}
			}
			// If the tile is not empty, continues searching.
			else {
				foundSpot = false;
				continue;

			}
		}

		// If a spot is found, copies the galaxy to it and assigns the nearby area to ThinSpaceTiles.
		if (foundSpot) {

			// Assigns the inputed Galaxy's area.
			galaxySpace->area = {xOrigin , yOrigin, galSize, galSize};

			// Assigns the number of unowned tiles in the Galaxy.
			for (int i = galaxySpace->area.x; i < galSize; ++i) {
				for (int j = galaxySpace->area.y; i < galSize; ++i) {
					galaxySpace->unowned += universe[i][j] != THIN_SPACE_TILE;

				}
			}

			// Copies the galaxy into the universe.
			for (int w = 0; w < galSize; ++w) {
				for (int h = 0; h < galSize; ++h) {
					universe[w + xOrigin][h + yOrigin] = galaxy[w][h];

				}
			}

			// Calculates the origin point of a box of ThinSpaceTiles.
			xOrigin >= minDistance ? xOrigin -= minDistance : xOrigin = 0;
			yOrigin >= minDistance ? yOrigin -= minDistance : yOrigin = 0;

			// Draws a box of dimensions (2 * minDistance + galWidth) x (2 * minDistance + galHeight)
			// of ThinSpaceTiles around the new galaxy.
			for (int w = xOrigin; w < xOrigin + galSize + 2 * minDistance && w < uniWidth; ++w) {
				for (int h = yOrigin; h < yOrigin + galSize + 2 * minDistance && h < uniHeight; ++h) {
					if (universe[w][h] == -1) universe[w][h] = THIN_SPACE_TILE;

				}
			}
		}
		// If the loop is exited without a spot being found, decrements numGals and returns false.
		else {
			--numGals;
			galaxySpace->area = {0, 0, 0, 0};

		}
	}
}

/*
Initializes a RING GalaxyInstruction.

The minimum width and height are 50.
The minimum coreSizeFraction is 0.
*/
void initRingGalaxy(GalaxyInstruction* instr, int uniWidth, int uniHeight) {

	// Initializes width such that it will fit within the universe.
	if (instr->width <= 50) instr->width = rand() % (int)(0.2 * uniWidth) + 50;
	// Initializes height such that it will fit within the universe.
	if (instr->height <= 50) instr->height = rand() % (int)(0.2 * uniHeight) + 50;
	// Initializes coreSizeFraction to a number between 0 and 100 if uninitialized.
	if (instr->coreSizeFraction <= 0) {

		// 20% chance of no galactic core.
		if (!(rand() % 5)) instr->coreSizeFraction = 0;
		// 80% chance of decently large galactic core.
		else instr->coreSizeFraction = (rand() % 40) + 50;

	}
	// Initializes starChance to be between 5 and 20.
	if (instr->starChance <= 0) instr->starChance = rand() % 16 + 5;
	// Initializes numHabitable to be two.
	if (instr->numHabitable <= 0) instr->numHabitable = 2;
	// Initializes numBarren to six.
	if (instr->numBarren <= 0) instr->numBarren = 6;
	// Initializes planetSize to 70.
	if (instr->planetSize < 20) instr->planetSize = 70;
	// Initializes empireChance to 10/1000.
	if (instr->empireChance <= 0) instr->empireChance = 10;

}

/*
Generates a ring shaped galaxy. Does so by creating several concentric oblongs between the furthest
appropriate radius and the core-size radius.

Note: If coreFraction is 0, there will be no galactic center and the galaxy will be a disc.
	  Furthermore, coreFraction should always be below 0.5.
*/
int** generateRingGalaxy(int width, int height, double coreSizeFraction) {
	int coreSize = coreSizeFraction * width;
	double ratio = (double)height / width;
	int size = ratio <= 1 ? width : height;
	int center = ratio <= 1 ? width / 2 : height / 2;

	// Allocates a dummy galaxy for galaxy generation.
	int** dummyGalaxy = new int*[size];
	for (int i = 0; i < size; ++i) dummyGalaxy[i] = new int[size];

	// Allocates a canvas for galaxy generation.
	int** canvas = new int*[size];
	for (int i = 0; i < size; ++i) canvas[i] = new int[size];

	// Creates ThickSpace tiles from the outer ring to the core.
	for (double r = coreSize; r < width - width / 2 - 1; r += 0.1) {
		
		// Draws the rings.
		for (double rad = 0; rad < 2 * M_PI; rad += 0.005) {
			dummyGalaxy[(int)(r * sin(rad)) + center][(int)(r * ratio * cos(rad)) + center] = THICK_SPACE_TILE;

		}
	}

	// Rotates the galaxy for variability.
	rotateGalaxy(size, center, dummyGalaxy, canvas);

	// Fills any small gaps in the galaxy's ThickSpaceTiles.
	fillGaps(dummyGalaxy, canvas, size, size, 3, THICK_SPACE_TILE);

	// Fills holes in the galaxy.
	fillHoles(dummyGalaxy, canvas, size, size);

	// Deallocates canvas.
	deallocateCanvas(canvas, size);

	// Returns a space copy the completed galaxy.
	return dummyGalaxy;

}

/*
Initializes a Spiral GalaxyInstruction.

The minimum width is 50.
The minimum numArms is 2.
the minimum armSizeFraction and radFraction are 0.
*/
void initSpiralGalaxy(GalaxyInstruction* instr, int uniWidth, int uniHeight) {

	// Initializes width such that it will fit within the universe if uninitialized.
	if (instr->width <= 50) {
		int maxSize = uniWidth < uniHeight ? 0.3 * uniWidth : 0.3 * uniHeight;
		instr->width = rand() % maxSize + 100;

	}
	// Initializes numArms to an even number between 2 and 8.
	if (instr->numArms < 2) {
		instr->numArms = rand() % 7 + 2;
		if (instr->numArms % 2) instr->numArms += 1;

	}
	// Initializes armSizeFraction to a number between 0 and 100.
	if (instr->armSizeFraction <= 0) instr->armSizeFraction = rand() % 101;
	// Initializes radFraction to a number between 0 and 100.
	if (instr->radFraction <= 0) instr->radFraction = rand() % 101;
	// Initializes starChance to be between 5 and 20.
	if (instr->starChance <= 0) instr->starChance = rand() % 16 + 5;
	// Initializes numHabitable to be two.
	if (instr->numHabitable <= 0) instr->numHabitable = 2;
	// Initializes numBarren to six.
	if (instr->numBarren <= 0) instr->numBarren = 6;
	// Initializes planetSize to 70.
	if (instr->planetSize < 20) instr->planetSize = 70;
	// Initializes empireChance to 10/1000.
	if (instr->empireChance <= 0) instr->empireChance = 10;

}

/*
Generates a spiral galaxy with an even number of arms between 2 and 12 (inclusive). Does so by calling
createArms, rotateGalaxy, spinGalaxy, fillGaps, and createCore.

Note: numArms should be an even integer above 2, below 12.
Note: armSizeFraction should be between 0.1 and 0.5 (inclusive).
Note: radFraction should be between 0.02 and 0.06 (inclusive).
*/
int** generateSpiralGalaxy(int width, int numArms, double armSizeFraction, double radFraction) {
	int center = width / 2;
	int armSize = armSizeFraction * width;
	double slope = (double)armSize / center;

	// Allocates a dummy galaxy for galaxy generation.
	int** dummyGalaxy = createCanvas(width);

	// Allacotes a canvas for galaxy generation.
	int** canvas = createCanvas(width);

	// Creates an n-pointed star for the galaxy.
	createArms(dummyGalaxy, canvas, width, center, armSize, numArms, slope);

	// Rotates the n-sided star for variability.
	rotateGalaxy(width, center, dummyGalaxy, canvas);

	// Spins the n-pointed star to emulate galactic arms.
	spinGalaxy(dummyGalaxy, canvas, width, center, radFraction);

	//// Fills any small gaps in the galaxy.
	fillGaps(dummyGalaxy, canvas, width, width, 3, THICK_SPACE_TILE);

	// Cleans any extra pieces off of the new galaxy.
	fillGaps(dummyGalaxy, canvas, width, width, 6, THIN_SPACE_TILE);

	// Creates a galactic core.
	createCore(dummyGalaxy, width, center);

	// Fills holes in the galaxy.
	fillHoles(dummyGalaxy, canvas, width, width);

	// Deallocates the canvas.
	deallocateCanvas(canvas, width);

	// Returns a space copy the completed galaxy.
	return dummyGalaxy;

}

/*
Deallocates the universe.
*/
void deallocateUniverse(int width, int height) {

	// Will not act if there is no universe.
	if (universe == nullptr) return;

	// Duct-tape solution for deleting SystemSpaceTiles.
	// Counts SystemSpaceTiles.
	int numSystems = 0;
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			if (universe[index(i, j, width)].tileID == SYSTEM_TILE)
				++numSystems;

	// Deletes planets and barrens for SystemSpaceTile.
	for (int i = 0; i < numSystems; ++i) {
		delete[] allSystemSpace[i].barrens;
		delete[] allSystemSpace[i].planets;

	}

	// Deletes the galaxy data.
	delete[] galaxies;
	free(allSystemSpace);
	delete[] universe;

}

/*
Clears a canvas (sets all tiles to ThinSpaceTiles).
*/
void clearUniverseCanvas(int** canvas, int size) {

	// Sets all tiles to ThinSpaceTiles.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			canvas[i][j] = THIN_SPACE_TILE;

		}
	}
}

/*
Copies a dummy galaxy's tileID's to a canvas.
*/
void copyUniverseCanvas(int** sourceCanvas, int** copyCanvas, int size) {

	// Copies the tileID at each index of galaxy to the corresponding index of canvas.
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			copyCanvas[i][j] = sourceCanvas[i][j];

		}
	}
}

/*
Copies a dummy universe to the universe.
*/
void universeToSpace(int** dummySpace, int width, int height) {
	int numSystems = 0;
	int currSystem = 0;

	// Counts the number of Systems and converts all empty tiles to ThinSpaceTiles.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			if (dummySpace[i][j] == SYSTEM_TILE) ++numSystems;
			if (dummySpace[i][j] < 0) dummySpace[i][j] = THIN_SPACE_TILE;

		}
	}

	// Initializes the array of Systems.
	allSystemSpace = (System*)calloc(numSystems, sizeof(System));

	// Initializes the universe.
	universe = (GalaxyTile*)calloc(width * height, sizeof(GalaxyTile));

	// Copies each dummySpace tile to the universe.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			uIndex(i, j).tileID = dummySpace[i][j];

			// Adds a system if necessary.
			if (dummySpace[i][j] == SYSTEM_TILE) uSystem(i, j) = &allSystemSpace[currSystem++];

		}
	}
}

/*
Creates an n-armed galaxy. Does so by creating a template arm based on the inputed parameters,
then copying and rotating it 2pi/n radians to create a regular n-armed star.
*/
void createArms(int** dummyGalaxy, int** canvas, int width, int center, int armSize, int numArms, double slope) {
	double radius;
	double startingRad;
	double armRadian = 2 * M_PI / numArms;

	// Clears dummyGalaxy before usage.
	clearUniverseCanvas(dummyGalaxy, width);

	// Creates the vertical galactic arm from ThickSpaceTiles.
	for (int i = 0; i < center; ++i) {

		// Creates a triangle from the end of the upper galactic edge to the galactic center.
		for (int j = i; j > 0 - i; --j) {
			canvas[(int)(slope * j + center)][i] = THICK_SPACE_TILE;

		}
	}

	// Adds numArms arms.
	for (int arms = 0; arms < numArms; ++arms) {

		// Evenly spaces the arms by separating each one by the same amount of radians.
		for (double rad = 0; rad < 2 * M_PI; rad += armRadian) {

			// Rotates the template arm to create the number of desired arms.
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < width; ++j) {
					startingRad = 0;

					// If not on the center, calls findStartingParameter an appropriate starting parameter
					// for spinning the galaxy.
					if (i != center && j != center) {
						startingRad = findStartingParameter(i, j, center, center);

					}

					// If the inputed galaxy has a ThickSpaceTile at a certain index, moves that tile in a
					// manner consonant with the desired rotation.
					if (canvas[i][j] == THICK_SPACE_TILE) {

						// Finds the radius of a circle intersecting the current tile.
						radius = sqrt(pow((double)i - center, 2.0) + pow((double)j - center, 2.0));

						// Rotates the current tile.
						dummyGalaxy[(int)(radius * sin(startingRad + rad)) + center]
								   [(int)(radius * cos(startingRad + rad)) + center]
								   = THICK_SPACE_TILE;

					}
				}
			}
		}
	}
}

/*
Creates a galactic core for a given galaxy. Does so by creating concentric rings of ThinSpaceTiles
from the outermost point of the core to the galactic center.
*/
void createCore(int** coreless, int width, int center) {
	int coreSize = width * 0.07;

	// Creates a galactic core of the desired radius.
	for (double i = 0; i < coreSize; i += 0.5) {
		for (double rad = 0; rad < 2 * M_PI; rad += 0.01) {
			coreless[(int)(i * sin(rad)) + center][(int)(i * cos(rad)) + center] = THIN_SPACE_TILE;

		}
	}
}

/*
Rotates the inputed galaxy to give more random character to the universe. Does so by moving each
tile by a radian amount from zero to four.
*/
void rotateGalaxy(int width, int center, int** toRotate, int** canvas) {
	double radius;
	double startingRad;

	// Copies toRotate to canvas and clears it.
	copyUniverseCanvas(toRotate, canvas, width);
	clearUniverseCanvas(toRotate, width);

	// Finds a random amount to rotate by.
	double rad = (rand() % 400) * 0.01;

	// Rotates the galaxy.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < width; ++j) {

			// If not on the center, calls findStartingParameter an appropriate starting parameter
			// for spinning the galaxy.
			if (i != center && j != center) {
				startingRad = findStartingParameter(i, j, center, center);

			}

			// If the inputed galaxy has a ThickSpaceTile at a certain index, moves that tile in a
			// manner consonant with the desired rotation.
			if (canvas[i][j] == THICK_SPACE_TILE) {

				// Finds the radius of a circle intersecting the current tile.
				radius = sqrt(pow((double)i - center, 2.0) + pow((double)j - center, 2.0));

				// Rotates the current tile.
				toRotate[(int)(radius * sin(startingRad + rad)) + center]
					    [(int)(radius * cos(startingRad + rad)) + center]
						= THICK_SPACE_TILE;

			}
		}
	}
}

/*
Spins an inputed galaxy to give the appearance of spiralled arms. Does so by copying each
ThickSpace tile in the inputed galaxy to a new tile determined by a modified parametric circle.

Note: radFraction should be between 0.02 and 0.06.
*/
void spinGalaxy(int** toSpin, int** canvas, int width, int center, double radFraction) {
	double radDivisor = radFraction * width;
	double radius;
	double startingRad;
	double rotateRad = 1.0 / width;

	// Copies toSpin to canvas and clears it.
	copyUniverseCanvas(toSpin, canvas, width);
	clearUniverseCanvas(toSpin, width);

	// Selects a spin direction.
	bool left = randB(1);
	if (left) radDivisor = -radDivisor;

	// Spins the inputed galaxy.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < width; ++j) {

			// If not on the center, calls findStartingParameter an appropriate starting parameter
			// for spinning the galaxy.
			if (i != center && j != center) {
				startingRad = findStartingParameter(i, j, center, center);

			}

			// If the inputed galaxy has a ThickSpaceTile at a certain index, moves that tile in a
			// manner consonant with the desired rotation.
			if (canvas[i][j] == THICK_SPACE_TILE) {

				// Finds the radius of a circle intersecting the current tile.
				radius = sqrt(pow((double)i - center, 2.0) + pow((double)j - center, 2.0));

				// Spins the current tile.
				for (double variance = -0.1; variance < 0.1; variance += rotateRad) {
					toSpin[(int)(radius * sin(startingRad + (M_PI / 4) + ((radius / radDivisor) / M_PI) + variance)) + center]
						  [(int)(radius * cos(startingRad + (M_PI / 4) + ((radius / radDivisor) / M_PI) + variance)) + center]
						  = THICK_SPACE_TILE;

				}
			}
		}
	}
}

/*
Finds the starting parameter for galaxy spinning. Specifically, finds the radian measure from the
upper y axis to the current point for the current galaxy.
*/
double findStartingParameter(int i, int j, int wCenter, int hCenter) {

	// Finds the radian angle measure from the x axis to the current tile.
	double startingRad = atan(((double)j - hCenter) / ((double)i - wCenter));

	// Calculates the starting parameter for quadrants I and II.
	if (i > wCenter) {
		return 1.5708 - startingRad;

	}
	// Calculates the starting parameter for quadrants III and IV.
	else if (i < wCenter) {
		return 4.71239 - startingRad;

	}
	// Calculates the starting parameter on the y-axis.
	else {

		// Calculates the starting parameter for the border between quadrants I and IV.
		if (j > hCenter) {
			return 0;

		}
		// Calculates the starting parameter for the border between quadrants II and III.
		else {
			return M_PI;

		}
	}
}

/*
Fills the gaps in a galaxy. Specifically, assigns any ThinSpace tile with at least two adjacent
ThickSpace tiles to ThickSpace.
*/
void fillGaps(int** gapGalaxy, int** canvas, int width, int height, int threshold, int tileID) {

	// Copies gapGalaxy to canvas.
	copyUniverseCanvas(gapGalaxy, canvas, width);

	// Checks for gaps in the inputed galaxy's tileID body and fills them.
	for (int i = 1; i < width - 1; ++i) {
		for (int j = 1; j < height - 1; ++j) {

			// If the current index has an instance of the desired GalaxyTileID, keeps the existing copy.
			if (canvas[i][j] == tileID);
			// If the current index does not have an instance of the desired GalaxyTileID, counts the
			// number of adjacent tileID GalaxyTileID.
			else {
				int counter = 0;

				// Checks all tiles within a 3x3 area centered around the current tile. Increments counter
				// for each tile that matches the inputed tileID.
				for (int x = i - 1; x < i + 2; ++x) {
					for (int y = j - 1; y < j + 2; ++y) {
						if (canvas[x][y] == tileID) ++counter;

					}
				}

				// If the counter is at or above the threshold, assigns the chosen tile the inputed tileID.
				if (counter >= threshold) gapGalaxy[i][j] = tileID;

			}
		}
	}
}

/*
Pathfinds from the inputed starting xPos and yPos and marks each ThinSpaceTile
that is contiguous with the inputed one.
*/
void fillHoles(int** holeGalaxy, int** canvas, int width, int height, int xPos, int yPos) {
	Coordinate coord;
	std::vector<Coordinate> coordQueue;

	// Places the first coord into the queue.
	coordQueue.push_back({xPos, yPos});

	// Explores every possible tile.
	while (!coordQueue.empty()) {

		// Gets the next coord.
		coord = coordQueue.back();
		coordQueue.pop_back();

		// Will not mark the tile if it is out of bounds.
		if (coord.x >= width || coord.x < 0 || coord.y >= height || coord.y < 0) continue;

		// Will not mark the tile if it has already been explored.
		if (canvas[coord.x][coord.y]) continue;

		// Will not mark the tile if it is not a ThinSpace tile.
		if (holeGalaxy[coord.x][coord.y] != THIN_SPACE_TILE) continue;

		// Marks the tile.
		canvas[coord.x][coord.y] = 1;

		// Adds the adjacent tiles to the queue.
		for (int i = -1; i < 2; ++i) {
			for (int j = -1; j < 2; ++j) {
				if (!i && !j) continue;
				coordQueue.push_back({coord.x + i, coord.y + j});

			}
		}
	}
}

/*
Fills holes in the galaxy. Marks every ThinSpace tile accessible from the center of the galaxy and the upper
leftmost tile of the galaxy as valid, then replaces every remaining ThinSpace tile with ThickSpace tiles.
*/
void fillHoles(int** holeGalaxy, int** canvas, int width, int height) {

	// Resets the canvas.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			canvas[i][j] = 0;

		}
	}

	// Marks tiles starting from the upper left.
	fillHoles(holeGalaxy, canvas, width, height, 0, 0);

	// Marks tiles starting from the center.
	fillHoles(holeGalaxy, canvas, width, height, width / 2, height / 2);

	// All unmarked tiles are converted into ThickSpaceTiles.
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			if (!canvas[i][j]) holeGalaxy[i][j] = THICK_SPACE_TILE;

		}
	}
}

/*
Finds system locations within the universe.
*/
void placeSystems(int** dummyUniverse, Galaxy* galaxies, GalaxyInstruction* galInstrs) {

	// Parses through each Galaxy and places SystemSpaceTiles within it. Replaces ThickSpaceTiles
	// with SystemTiles based on the relevant chance.
	for (int i = 0; i < numGals; ++i)
		for (int w = galaxies[i].area.x; w < galaxies[i].area.x + galaxies[i].area.w; ++w)
			for (int h = galaxies[i].area.y; h < galaxies[i].area.y + galaxies[i].area.h; ++h)
				if (dummyUniverse[w][h] == THICK_SPACE_TILE && rand() % 100 < galInstrs[i].starChance)
					dummyUniverse[w][h] = SYSTEM_TILE;

}

/*
Assigns the number of Habitables and barrens to each system.
*/
void assignSystemPlanetCounts(Galaxy* galaxies, GalaxyInstruction* galInstrs, int& numHab, int& numBarr) {
	numHab = 0;
	numBarr = 0;
	System* currSystem;

	// Assigns the number of habitable and barren planets for each system.
	for (int i = 0; i < numGals; ++i) {

		// Places the Habitables and Barrens within this galaxy.
		for (int w = galaxies[i].area.x; w < galaxies[i].area.x + galaxies[i].area.w; ++w) {
			for (int h = galaxies[i].area.y; h < galaxies[i].area.y + galaxies[i].area.h; ++h) {
				if (uIndex(w, h).tileID == SYSTEM_TILE) {
					currSystem = uSystem(w, h);
					currSystem->initHabitable(randB(8) % galInstrs[i].numHabitable + MIN_HABITABLE);
					currSystem->initBarren(randB(8) % galInstrs[i].numBarren + MIN_BARREN);
					numHab += currSystem->numHabitable;
					numBarr += currSystem->numBarren;

				}
			}
		}
	}
}

/*
Places Habitables and Barrens within the SystemSpaceTiles. Also counts the number of PlanetTiles placed.
Does not do anything with the planets, only places them.
All HabitablePlanets will have a size which is divisible by 2 such that pole wraps have correct adjacency.
*/
void placeAllPlanets(Galaxy* galaxies, GalaxyInstruction* galInstrs) {
	int size;
	int numTiles = 0;
	System* currSystem;

	// Goes through each galaxy (galaxies each have their own planet sizes).
	for (int i = 0; i < numGals; ++i) {

		// Places the Habitables and Barrens within this galaxy.
		for (int w = galaxies[i].area.x; w < galaxies[i].area.x + galaxies[i].area.w; ++w) {
			for (int h = galaxies[i].area.y; h < galaxies[i].area.y + galaxies[i].area.h; ++h) {
				if (uIndex(w, h).tileID == SYSTEM_TILE) {
					currSystem = uSystem(w, h);

					// Places and initializes the size of each planet.
					// TODO make dependent upon system stars.
					for (int plan = 0; plan < currSystem->numHabitable; ++plan) {
						// Should have range of MIN_PLANET_SIZE to galInstrs[i].planetSize and be divisible by 2.
						size = ((randB(8) % (galInstrs[i].planetSize - MIN_PLANET_WIDTH + 1)) & ~1) + MIN_PLANET_WIDTH;
						currSystem->planets[plan] = placeHabitable(size);
						currSystem->planets[plan]->location = { w, h };
						numTiles += currSystem->planets[plan]->size * currSystem->planets[plan]->size;

					}

					// Places and initializes each BarrenPlanet.
					for (int barr = 0; barr < currSystem->numBarren; ++barr) {
						currSystem->barrens[barr] = placeBarren();
						currSystem->barrens[barr]->size = 20;

					}
				}
			}
		}
	}

	// Initializes the array of tiles.
	placePlanetTiles(numTiles + 1, true);

	// Places the PlanetTiles for each planet.
	for (int i = 0; i < numHabitablePages; ++i) {
		for (int j = 0; j < habitablePages[i]->arrCurrPlanet; ++j) {
			habitablePages[i]->planets[j].planet = placePlanetTiles(habitablePages[i]->planets[j].size * 
				habitablePages[i]->planets[j].size, false);

		}
	}
}

/*
Finds a system for generation by the calling thread. Uses universeX and universeY to store position.
Returns a SystemSpaceTile upon success, otherwise nullptr.
*/
System* requestSystemToGenerate() {
	static std::mutex requestSystemToGenerateMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(requestSystemToGenerateMutex);
	
	// Searches through each tile in the galaxy until the next system is found.
	for (; universeX < universeWidth; ++universeX) {
		if (universeY >= universeHeight) universeY = 0;
		for (;  universeY < universeHeight; ++universeY) {
			if (uIndex(universeX, universeY).tileID == SYSTEM_TILE) {
				++universeY;
				return uSystem(universeX, universeY - 1);

			}
		}
	}

	// If no tile was found, returns nullptr.
	return nullptr;

}

/*
Generates systems in a threaded manner.
*/
void generateSystems(int** dummyPlanet, int** moistureDummy, int** heatDummy, int** riverDummy,
	int** buildingDummy, int** canvas) {
	System* system;

	// Generates systems.
	while (system = requestSystemToGenerate())
		generateSystem(system, dummyPlanet, moistureDummy, heatDummy, riverDummy, buildingDummy, canvas);

}

/*
Generates systems and places them in the universe.

Allocates the planet generation canvases to minimize cpu usage on new.
Uses multiple threads for system generation.
*/
void beginGenerateSystems(Galaxy* galaxies, GalaxyInstruction* galInstrs) {
	int numTiles = 0;
	int neededHab;
	int neededBarr;

	// Allocates the canvases for planet generation.
	int*** dummyPlanets = createCanvases(MAX_PLANET_WIDTH, numThreads);
	int*** moistureDummies = createCanvases(MAX_PLANET_WIDTH, numThreads);
	int*** heatDummies = createCanvases(MAX_PLANET_WIDTH, numThreads);
	int*** riverDummies = createCanvases(MAX_PLANET_WIDTH, numThreads);
	int*** buildingDummies = createCanvases(MAX_PLANET_WIDTH, numThreads);
	int*** canvases = createCanvases(MAX_PLANET_WIDTH + 2, numThreads);

	// Assigns the number of HabitablePlanets and BarrenPlanets into the SystemSpaceTiles of each Galaxy.
	assignSystemPlanetCounts(galaxies, galInstrs, neededHab, neededBarr);

	// Initializes habitablePages and barrenPages.
	initHabitablePlanets(neededHab);
	initBarrenPlanets(neededBarr);

	// Places HabitablePlanets and BarrenPlanets into each SystemSpaceTile in each Galaxy.
	placeAllPlanets(galaxies, galInstrs);

	// Sets universeX and universeY to 0 for system generation.
	universeX = 0;
	universeY = 0;

	// Uses each thread to generate systems.
	for (int i = 0; i < numThreads - 1; ++i) threads[demandThread()] = new std::thread(generateSystems,
		dummyPlanets[i], moistureDummies[i], heatDummies[i], riverDummies[i], buildingDummies[i],
		canvases[i]);
	generateSystems(dummyPlanets[numThreads - 1], moistureDummies[numThreads - 1], heatDummies[numThreads - 1],
		riverDummies[numThreads - 1], buildingDummies[numThreads - 1], canvases[numThreads - 1]);

	// Joins all threads.
	joinAllThreads();

	// Sets universeX and universeY to 0 after system generation.
	universeX = 0;
	universeY = 0;

	// TODO DEBUG REMOVE
	// Shows number of systems.
	int numSystems = 0;
	for (int i = 0; i < universeWidth * universeHeight; ++i)
		if (universe[i].tileID == SYSTEM_TILE) ++numSystems;

	// TODO DEBUG REMOVE
	// Shows num Systems, HabitablePlanets and BarrenPlanets.
	printf("numSystems : %d\n", numSystems);
	//printf("neededHab : %d\n", neededHab);
	printf("numHabitablePlanets : %d\n", numHabitablePlanets);
	printf("numHabitablePages : %d\n", numHabitablePages);
	//printf("neededBarr : %d\n", neededBarr);
	printf("numBarrenPlanets : %d\n", numBarrenPlanets);
	printf("numBarrenPages : %d\n", numBarrenPages);

	// Deallocates the dummyPlanets and canvas.
	deallocateCanvases(dummyPlanets, MAX_PLANET_WIDTH, numThreads);
	deallocateCanvases(moistureDummies, MAX_PLANET_WIDTH, numThreads);
	deallocateCanvases(heatDummies, MAX_PLANET_WIDTH, numThreads);
	deallocateCanvases(riverDummies, MAX_PLANET_WIDTH, numThreads);
	deallocateCanvases(buildingDummies, MAX_PLANET_WIDTH, numThreads);
	deallocateCanvases(canvases, MAX_PLANET_WIDTH + 2, numThreads);

}