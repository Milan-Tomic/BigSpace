#pragma once

// Generates a new system.
void generateSystem(System* system, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas);

// Generates a randomized Star.
void generateStars(System* system);

// Places the stars of a SystemSpaceTile into the system.
void placeStars(System* system, SDL_Rect* starAreas);

// Generates planets in the system based on the numberand position of stars.
void generatePlanets(System* system, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas);

// Generates habitable planets around the center of a system.
void generateHabitablePlanets(System* system,
	int** dummyPlanet, int** moistureDummy, int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas,
	int planetDistances[200]);

// Generates uninhabitable planets around the center of a system.
void generateBarrenPlanets(System* system, int planetDistances[200]);

// Finds the luminosity of a system.
double findSystemLuminosity(System* system);

// Finds the minimum and maximum distances between a planet and a star.
void findDistanceBounds(System* system, int* minDistance, int* maxDistance);

// Finds the heatMultiple of a planet with the inputed distance to a star of the inputed luminosity.
double findHeatMultiple(double distance, double luminosity);

// Finds the distance required to get a planet of the given heat.
int findDistanceHeat(System* system, int desiredHeat, double albedo);

// Stellar wind.
// https://www.pppl.gov/news/2017/11/blowing-stellar-wind-scientists-reduce-chances-life-exoplanets-so-called-habitable

// Binary stars
// https://en.wikipedia.org/wiki/Binary_star
// https://en.wikipedia.org/wiki/Habitability_of_binary_star_systems

// Ratio of pixels to meters.
#define PIXEL_METERS 125000000

/*
Generates a system. Systems can contain up to four SystemStarTiles and up to TODO SystemPlanetTiles.

NOTE: The inputed array references are included to reduce processor usage by the new operator in 
	  the Planet Generator.
*/
void generateSystem(System* system, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas) {

	// Initializes tradeLanes for the system.
	// TODO change max tradeLane length.
	system->initTradeLanes(7);

	// Generates 1-4 stars.
	int numStars = randB(7);
	if (numStars <= 103) numStars = 1; // 80%
	else if (numStars <= 122) numStars = 2; // 95%
	else if (numStars <= 126) numStars = 3; // 99%
	else numStars = 4;

	// Initializes the system's stars array and generates the appropriate SystemStarTiles.
	system->initStars(numStars);
	generateStars(system);

	// Generates the SystemHabitableTiles for the Star.
	generatePlanets(system, dummyPlanet, moistureDummy, heatDummy, riverDummy, buildingDummy, canvas);

	// Chosen probabilities
	// tot 90
	// Suparmassive red/blue should either A. not exist or B. have probabilities later.
	// 2,400-30,000

}

// Proportions of various stars.
// https://en.wikipedia.org/wiki/Stellar_classification#Harvard_spectral_classification

// Star information.
// https://www.enchantedlearning.com/subjects/astronomy/stars/startypes.shtml

// Luminosity
// https://astro.unl.edu/naap/hr/hr_background2.html

/*
Generates numStars random SystemStarTiles and places them into a SystemSpaceTile's list of stars.

Normal star probabilities :
M: Red normal 40%
K: Orange normal 20%
G: Sun normal 35%
F: White normal 2%
A: Blue normal 3%
*/
void generateStars(System* system) {
	StarType type;
	int heat;
	float radius;
	double luminosity;

	// Adds numStars SystemStarTiles to system.
	for (int i = 0; i < system->numStars; ++i) {

		// Decides which type of star to generate.
		int starDecider = randB(7);

		// Generates a red star 40% of the time.
		if (starDecider <= 51) {
			heat = randB(11) % 1301 + 2400;
			radius = (float)(randB(5) + 40) / 100;
			// NOTE: Originally %61 + 10, incresed min for visibility.
			type = MStar;

		}
		// Generates an orange star 20% of the time.
		else if (starDecider <= 77) {
			heat = randB(11) % 1501 + 3700;
			radius = (float)(randB(5) % 27 + 70) / 100;
			type = KStar;

		}
		// Generates a yellow (sun-like) star 35% of the time.
		else if (starDecider <= 122) {
			heat = randB(10) % 801 + 5200;
			radius = (float)(randB(5) % 20 + 96) / 100;
			type = GStar;

		}
		// Generates a white star 2% of the time.
		else if (starDecider <= 124) {
			heat = randB(11) % 1501 + 6000;
			radius = (float)(randB(5) % 26 + 115) / 100;
			type = FStar;

		}
		// Generates a blue star 3% of the time.
		else {
			heat = randB(12) % 2501 + 7500;
			radius = (float)(randB(6) % 41 + 140) / 100;
			type = AStar;

		}

		// Calculates the luminosity of the star.
		// The equations is 4pi * mu * radius(meters)^2 * heat(kelvin)^2.
		// This particular equation uses the solar radius, thus radius * R_solar.
		luminosity = 4 * M_PI * 0.00000005670373 * (pow(radius * 695510000, 2)) * pow(heat, 4);

		// Assigns a new Star to system matching the above criteria.
		system->stars[i] = Star(type, radius, luminosity);

	}
}

/*
Changes the locations of each Star in the inputed SystemSpaceTile so that they all orbit
around the barycenter. Heavier stars are nearer to the center.
*/
void placeStars(System* system, SDL_Rect* areas) {
	int systemMass = 0;

	// Loads all star areas at the center of the system.
	for (int i = 0; i < system->numStars; ++i)
		areas[i] = { -system->stars[i].size / 2, -system->stars[i].size / 2,
			system->stars[i].size, system->stars[i].size };

	// If there is only one star in the system, returns.
	if (system->numStars == 1) return;

	// Calculates the total 'mass' of all stars within the system.
	// area.w is a stand in for radius and mass, which are not stored to this point.
	for (int i = 0; i < system->numStars; ++i) systemMass += areas[i].w;

	// Places each star around the system's center.
	int distance;
	double rad = 0;
	for (int i = 0; i < system->numStars; ++i) {
		int xOffset;
		int yOffset;

		// Places each star at a distance from the center determined by its mass relative to the total system mass.
		distance = 25 * ((float)systemMass / areas[i].w);

		// Places each star rad degrees from 0.
		xOffset = cos(rad) * distance;
		yOffset = sin(rad) * distance;

		// Places each star.
		areas[i].x += xOffset;
		areas[i].y += yOffset;

		// Increments rad.
		rad += (2 * M_PI) / system->numStars;

	}
}

/*
Generates planets in the system based on the number and position of stars.

	// NOTE: 1 px = 100 million meters
*/
void generatePlanets(System* system, int** dummyPlanet, int** moistureDummy,
	int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas) {

	// Tracks the distances of existing planets to a star so as to minimize orbital overlap.
	//TODO: change planetDistances to be dependent upon user inputed maximum planets.
	int planetDistances[200];

	// Generates habitable planets for the system.
	generateHabitablePlanets(system, dummyPlanet, moistureDummy,
		heatDummy, riverDummy, buildingDummy, canvas, planetDistances);

	// Generates barren planets for the system.
	generateBarrenPlanets(system, planetDistances);

}

/*
Generates planets around the center of a system.
*/
void generateHabitablePlanets(System* system,
	int** dummyPlanet, int** moistureDummy, int** heatDummy, int** riverDummy, int** buildingDummy, int** canvas,
	int planetDistances[200]) {
	int distance;
	int lowerBound = 0;
	int upperBound = 0;
	bool foundSpot;

	// Finds minDistance and maxDistance.
	// The inputed heats are lower (-30) than habitable heats to account for atmospheric heating.
	int minDistance = findDistanceHeat(system, 273 + 100 - 30, 0.3);
	int maxDistance = findDistanceHeat(system, 273 - 50 - 30, 0.3);

	// Makes sure that maxDistance and minDistance are not too close to or too far from the system center.
	findDistanceBounds(system, &lowerBound, &upperBound);
	if (minDistance < lowerBound) minDistance += lowerBound;
	maxDistance += lowerBound;
	if (maxDistance > upperBound) maxDistance = upperBound;

	// Places the planets.
	for (int i = 0; i < system->numHabitable; ++i) {
		foundSpot = false;

		// Places each planet.
		for (int numAttempts = 0; numAttempts < 32 && !foundSpot; ++numAttempts) {
			foundSpot = true;
			distance = randB(13) % (maxDistance - minDistance + 1) + minDistance;

			// Checks that there is no orbital overlap.
			for (int j = 0; j < i; ++j) {
				if (distance < planetDistances[j] + MAX_PLANET_WIDTH &&
					distance > planetDistances[j] - MAX_PLANET_WIDTH) {
					foundSpot = false;

				}
			}
		}

		// Generates the new planet.
		system->planets[i]->heatMultiple = findHeatMultiple((double)distance * PIXEL_METERS, findSystemLuminosity(system));
		planetGenerator(system->planets[i]->size, system->planets[i], dummyPlanet, moistureDummy,
			heatDummy, riverDummy, buildingDummy, canvas);

		// When a planet is placed, adds its distance to planetDistances.
		planetDistances[i] = distance;
		system->planets[i]->distance = distance;

	}
}

/*
Generates barren planets for the system.
*/
void generateBarrenPlanets(System* system, int planetDistances[200]) {
	int distance;
	int minDistance = 0;
	int maxDistance = 0;
	bool foundSpot;

	// Finds minDistance and maxDistance.
	findDistanceBounds(system, &minDistance, &maxDistance);

	// Generates the planets.
	for (int i = 0; i < system->numBarren; ++i) {
		foundSpot = false;
		minDistance += system->barrens[i]->size / 2;

		// Places the planets.
		for (int numAttempts = 0; numAttempts < 32 && !foundSpot; ++numAttempts) {
			foundSpot = true;
			distance = randB(13) % (maxDistance - minDistance + 1) + minDistance;

			// Checks that there is no orbital overlap.
			for (int j = 0; j < system->numHabitable + i; ++j) {
				if (distance < planetDistances[j] + MAX_PLANET_WIDTH &&
					distance > planetDistances[j] - MAX_PLANET_WIDTH) {
					foundSpot = false;

				}
			}
		}

		// When a planet is placed, adds its distance to planetDistances.
		planetDistances[i + system->numHabitable] = distance;
		system->barrens[i]->distance = distance;

		// Restores minDistance.
		minDistance -= system->barrens[i]->size / 2;

		// Assigns the heat multiple of the planets.
		system->barrens[i]->heatMultiple = findHeatMultiple((double)distance * PIXEL_METERS, findSystemLuminosity(system));

		// Assigns the heat of the bodies.
		// TEMP assigns albedo to 0.3 and tau to 1.
		// Should be done in generateNewPlanet().
		system->barrens[i]->temperature = (int)findHeat((double)system->barrens[i]->heatMultiple, 0.3, 1.0);

	}
}

/*
Finds the luminosity of a system. A systems luminosity is the average luminosity of its component stars.
*/
double findSystemLuminosity(System* system) {
	double luminosity = 0;
	for (int i = 0; i < system->numStars; ++i) luminosity += system->stars[i].luminosity;
	luminosity /= system->numStars;
	return luminosity;

}

/*
Finds the minimum and maximum distances between a planet and a star.
*/
void findDistanceBounds(System* system, int* minDistance, int* maxDistance) {

	// If this in a single star system, sets minDistance and maxDistance based on the star's width.
	if (system->numStars == 1) {
		*minDistance = system->stars[0].size / 2 + 20;
		*maxDistance = system->stars[0].size * 20;

	}
	// Otherwise, finds the appropriate minimim distance between a planet and the barycenter based on
	// the distance of the stars from the barycenter.
	else {

		// Finds the star's locations.
		SDL_Rect areas[4];
		placeStars(system, areas);

		// Finds the appropriate minimum distance from a planet to the barycenter.
		for (int i = 0; i < system->numStars; ++i) {

			// Finds the distance between each star and the barycenter.
			int newDistance = sqrt(pow(areas[i].x + areas[i].w / 2, 2) +
				pow(areas[i].y + areas[i].h / 2, 2));

			// If the distance between the star and the barycenter is greater than minDistance, increases it.
			if (newDistance > *minDistance) *minDistance = newDistance + areas[i].w / 2 + 80;

		}

		// Finds the appropriate maximum distance from a planet to the barycenter.
		for (int i = 0; i < system->numStars; ++i) *maxDistance += areas[i].w;
		*maxDistance *= 20;

	}
}

/*
Finds the heat multiple of a planet with the inputed distance to a star of inputed heat.
The resulting equation for radiative equilibrium heat is T^4 = findHeat() * (1-a).

Source:
https://astronomy.stackexchange.com/questions/10113/how-to-calculate-the-expected-surface-temperature-of-a-planet

NOTE: albedo of rock is 0.2.
	  albedo of snow is 0.9.
	  default planet albedo should be 0.3.
*/
double findHeatMultiple(double distance, double luminosity) {

	// Finds the mulitple for (1-a) to calculate T^4.
	return luminosity / (16 * M_PI * distance * distance * 0.00000005670373);

}

/*
Finds the distance in pixels required to get a planet of the given heat.

NOTE : each pixel is 500m meters.
*/
int findDistanceHeat(System* system, int desiredHeat, double albedo) {

	// Finds the aggregate luminosity of the system.
	double luminosity = findSystemLuminosity(system);

	// Calculates and returns distance.
	return (sqrt((luminosity * (1 - albedo)) / (16 * M_PI * pow(desiredHeat, 4) * 0.00000005670373))) / PIXEL_METERS;

}