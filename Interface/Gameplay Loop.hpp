#pragma once

// Turns take a half second.
#define TURN_TIME 500

// The amount of turns in a cycle.
#define NUM_TURNS 3

// Current turn.
int currTurn;

// Speed of the game.
int gameSpeed;

// High speed mode flag.
bool highSpeedMode;

// Locks for the gameplay loop.
std::shared_mutex gameplayMutex1;
std::shared_mutex gameplayMutex2;

/*
Performs actions depending on the turn. This function will be managed by multiple threads.
*/
void gameTurn() {
	while (true) {

		// Performs climate change on 0 turns.
		gameplayMutex1.lock_shared();
		climateChange();
		gameplayMutex1.unlock_shared();
		threadingSemaphore.post();
		gameplayMutex2.lock_shared();
		spaceBattles();
		groundBattles();
		threadingSemaphore.post();
		gameplayMutex2.unlock_shared();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

		// Performs production on 1 turns.
		gameplayMutex1.lock_shared();
		planetProduction();
		gameplayMutex1.unlock_shared();
		threadingSemaphore.post();
		gameplayMutex2.lock_shared();
		groundBattles();
		spaceBattles();
		threadingSemaphore.post();
		gameplayMutex2.unlock_shared();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

		// Performs government action on 2 turns.
		gameplayMutex1.lock_shared();
		governmentAction();
		gameplayMutex1.unlock_shared();
		threadingSemaphore.post();
		gameplayMutex2.lock_shared();
		groundBattles();
		spaceBattles();
		threadingSemaphore.post();
		gameplayMutex2.unlock_shared();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

	}
}

/*
Manages the system of locks to ensure that the threads in gameTurn execute
correctly.
*/
inline void beginGameTurn() {

	// Prevents child threads from continuing to the second cycle before the
	// first cycle is over.
	gameplayMutex2.lock();

	// Prevents child threads from entering the second phase of spaceBattles
	// while in the second cycle.
	spaceBattleMutex.lock();

	// Allows child threads to begin the first cycle.
	gameplayMutex1.unlock();

	// Waits until the current turn has been completed.
	threadingSemaphore.wait();

	// Unlocks all habitable pages.
	// TODO create switch containing cleanup behaviours.
	releaseAllHabitablePages();
	releaseAllGovernmentPages();
	releaseAllColonyPages();
	releaseAllClosurePages();
	releaseAllGalaxies();
	resetInt();
	emptyRemovedSquadrons();

	// Prevents worker threads from moving to the first cycle before the second
	// one is completed.
	gameplayMutex1.lock();

	// Allows worker threads to move to the next cycle start.
	gameplayMutex2.unlock();

	// Waits until the first phase of spaceBattles is complete, then allows
	// child threads to continue.
	spaceBattleSemaphore.wait();
	resetInt();
	spaceBattleMutex.unlock();

	// Waits until all threads are ready for the next cycle to continue.
	threadingSemaphore.wait();

	// Unlocks all habitable pages.
	// TODO create switch containing cleanup behaviours.
	releaseAllHabitablePages();
	releaseAllGovernmentPages();
	releaseAllColonyPages();
	releaseAllClosurePages();
	releaseAllGalaxies();
	resetInt();
	emptyRemovedSquadrons();

	// Moves on to the next turn.
	++currTurn;

}

/*
Main loop for the game.
*/
void gameLoop() {
	using::std::chrono::milliseconds;
	using::std::chrono::duration_cast;

	// Waits for the interface to be ready before beginning.
	threadingSemaphore.wait();

	// Initializes the current turn to 0.
	currTurn = 0;

	// Initializes locks.
	threadingSemaphore.crit = numThreads;
	threadingSemaphore.count = 0;
	spaceBattleSemaphore.crit = numThreads;
	spaceBattleSemaphore.count = 0;
	gameplayMutex1.lock();

	// Time when the current turn started.
	auto turnStart = std::chrono::system_clock::now();

	// Current time.
	auto currTime = std::chrono::system_clock::now();

	// Creates several threads for the gameTurn loop.
	for (int i = 0; i < numThreads; ++i) threads[demandThread()] = new std::thread(gameTurn);

	// Loops until the game is exited.
	while (true) {
		while (!highSpeedMode && !exitFlag &&
			(duration_cast<milliseconds>(currTime.time_since_epoch()).count() - duration_cast<milliseconds>(turnStart.time_since_epoch()).count()) * gameSpeed < TURN_TIME) {

			// Adjusts the time.
			currTime = std::chrono::system_clock::now();
			std::this_thread::yield();

		}

		// TODO DEBUG REMOVE
		// Prints the time since the last turn started.
		printf("time : %16lu\n", duration_cast<milliseconds>(currTime.time_since_epoch()).count() - duration_cast<milliseconds>(turnStart.time_since_epoch()).count());

		// Updates the turnStart time.
		turnStart = std::chrono::system_clock::now();

		// Performs the action for this turn.
		beginGameTurn();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

		// Creates a new event to order a new frame to be rendered.
		SDL_Event event = {};
		event.type = SDL_USEREVENT;
		SDL_PushEvent(&event);

	}
}

/*
Main loop for the game when performing speed testing.
*/
void speedTestGameLoop(int numTurns) {
	using::std::chrono::milliseconds;
	using::std::chrono::duration_cast;
	long long int duration = 0;
	long long int med;

	// Waits for the interface to be ready before beginning.
	threadingSemaphore.wait();

	// Initializes the current turn to 0.
	currTurn = 0;

	// Initializes locks.
	threadingSemaphore.crit = numThreads;
	threadingSemaphore.count = 0;
	spaceBattleSemaphore.crit = numThreads;
	spaceBattleSemaphore.count = 0;
	gameplayMutex1.lock();

	// Time when the game started.
	auto gameStart = std::chrono::system_clock::now();

	// Creates several threads for the gameTurn loop.
	for (int i = 0; i < numThreads; ++i) threads[demandThread()] = new std::thread(gameTurn);

	// Loops until the test is over;
	while (currTurn < numTurns) {

		// Performs the action for this turn.
		beginGameTurn();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

		// Creates a new event to order a new frame to be rendered.
		SDL_Event event = {};
		event.type = SDL_USEREVENT;
		SDL_PushEvent(&event);

	}

	// Calculates the duration of the game's running.
	auto currTime = std::chrono::system_clock::now();
	duration = duration_cast<milliseconds>(currTime.time_since_epoch()).count() - duration_cast<milliseconds>(gameStart.time_since_epoch()).count();

	// Prints the number of turns taken during the test.
	printf("numTurns :%12d\n", numTurns);

	// Prints hours taken for the test.
	med = duration / 3600000;
	printf("%013lu:", med);

	// Prints the minutes taken for the test.
	med = duration % 3600000;
	med /= 60000;
	printf("%02lu:", med);

	// Prints the seconds taken for the test.
	med = duration % 60000;
	med /= 1000;
	printf("%02lu", med);

	// Prints the milliseconds taken for the test.
	med = duration % 1000;
	printf(".%03lu\n", med);

	// Prints the total duration of the test in milliseconds.
	printf("time : %14ldms\n", duration);

	// Prints the time taken per turn of the test in turns/millisecond..
	printf("rate : %13ft/s\n", (double)numTurns / (((double)duration) / 1000.0));

}