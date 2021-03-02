#pragma once

// Turns take a half second.
#define TURN_TIME (std::chrono::nanoseconds) 500000000

// The amount of turns in a cycle.
#define NUM_TURNS 3

// Current turn.
int currTurn;

// Speed of the game.
int gameSpeed;

// Locks for the gameplay loop.
std::shared_mutex gameplayMutex1;
std::shared_mutex gameplayMutex2;

/*
Performs actions depending on the turn.
*/
void gameTurn() {
	while (true) {

		// Performs climate change on 0 turns.
		gameplayMutex1.lock_shared();
		climateChange();
		gameplayMutex1.unlock_shared();
		threadingSemaphore.post();
		gameplayMutex2.lock_shared();
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
		threadingSemaphore.post();
		gameplayMutex2.unlock_shared();

		// Closes the thread if the exitFlag is raised.
		if (exitFlag) return;

	}
}

/*
Allows the threads in gameTurn to move.
*/
inline void beginGameTurn() {

	// Prevents child threads from continuing to the next cycle before the current cycle is over.
	gameplayMutex2.lock();

	// Allows child threads to begin this cycle.
	gameplayMutex1.unlock();

	// Waits until the current turn has been completed.
	threadingSemaphore.wait();

	// Unlocks all habitable pages.
	// TODO create switch containing cleanup behaviours.
	returnAllHabitablePages();
	returnAllGovernmentPages();

	// Prevents worker threads from moving to the next cycle before the current one is completed.
	gameplayMutex1.lock();

	// Allows worker threads to move to the next cycle start.
	gameplayMutex2.unlock();

	// Waits until all threads are ready for the next cycle to continue.
	threadingSemaphore.wait();

	// Unlocks all habitable pages.
	// TODO create switch containing cleanup behaviours.
	returnAllHabitablePages();
	returnAllGovernmentPages();

	// Moves on to the next turn.
	++currTurn;

}

/*
Main loop for the game.
*/
void gameLoop() {

	// Waits for the interface to be ready before beginning.
	threadingSemaphore.wait();

	// Initializes the current turn to 0.
	currTurn = 0;

	// Initializes locks.
	threadingSemaphore.crit = numThreads;
	threadingSemaphore.count = 0;
	gameplayMutex1.lock();

	// Time when gameplay started.
	auto turnStart = std::chrono::high_resolution_clock::now();

	// Current time.
	auto currTime = std::chrono::high_resolution_clock::now();

	// Time when next turn begins (in nanoseconds).
	std::chrono::nanoseconds turnEnd = turnStart.time_since_epoch() + (std::chrono::nanoseconds) TURN_TIME;

	// Creates several threads for the gameTurn loop.
	for (int i = 0; i < numThreads; ++i) threads[demandThread()] = new std::thread(gameTurn);

	// Loops until a buttonCode has been assigned.
	while (true) {
		while ((currTime.time_since_epoch() - turnStart.time_since_epoch()) * gameSpeed < TURN_TIME && !exitFlag) {

			// Adjusts the time.
			currTime = std::chrono::high_resolution_clock::now();
			std::this_thread::yield();

		}

		// TODO DEBUG REMOVE
		// Prints the time since the last turn started.
		printf("time : %ld\n", (currTime.time_since_epoch() - turnStart.time_since_epoch()));

		// Updates the turnStart time.
		turnStart = std::chrono::high_resolution_clock::now();

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
