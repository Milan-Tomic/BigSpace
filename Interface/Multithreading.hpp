#pragma once

// Number of threads that the program will have access to.
int numThreads;

// Array containing threads for this program.
std::thread** threads;

// Main thread for the gameLoop, this thread will manage semaphores and other
// threading tasks between turns in the gameLoop.
std::thread* gameLoopThread;

// Available threads. 1 encodes availability.
bool* currThreads;

// Threads to clean. 1 encodes attention needed.
bool* cleanThreads;

// Stores dummies for use when parsing planets, galaxies, or the universe.
// Availability is at the -1st index of each dummy.
// Dummy size is at the -1st index of threadDummies. It is size (in bytes) + 1.
// threadDummies starts on the -1st index of the first dummy.
uint_least8_t* threadDummies;

// Stores larger dummies for parsing over planets.
// Same as above.
uint_least8_t* threadLargeDummies;

// Informs worker threads that the process is exiting gameplay.
bool exitFlag;

// Special verison of rand_s that returns an int and ignores errors.
int randS();

// Initializes all data needed for threading.
void initThreading();

// Returns the next available thread's index in threads.
int demandThread();

// Waits for all threads to finish execution. Joins and deletes all of them.
void joinAllThreads();

// Joins threads which have finished execution and cleans up their memory.
void joinComplete();

// Creates numThreads dummies of size size. They are either the size of the universe or
// the largest possible planet, whichever's larger.
void initDummies(int size);
uint_least8_t* requestDummy();
void releaseDummy(uint_least8_t* dummy);

// Similar to the above, creates large dummies whose indices have enough space
// to store a bitarray containing 256 elements.
// Should be equal to the size of the largest planet, as it is used only in
// Ground Armies Battle->calcFrontStrength.
void initLargeDummies(int size);
uint_least8_t* requestLargeDummy();
void releaseLargeDummy(uint_least8_t* dummy);

// Reads and writes bits in bitarrays.
inline bool readArrBit(uint_least8_t* arr, int bit);
inline void writeArrBit(uint_least8_t* arr, int bit, bool write);

// Requests a galaxy.
int requestGalaxy();

// Returns a galaxy.
void releaseGalaxy(int index);

// Returns all galaxies.
void releaseAllGalaxies();

/*
A simple 'semaphore' to be used to help the main thread wait for threads in the gameTurn loop.
It is expected that there will only ever be one waiting thread at any given time, having more than one waiting
thread will lead to unexpected behaviour.
*/
class Semaphore {
public:
	std::mutex mtx;
	std::condition_variable cv;
	uint_least8_t count;
	uint_least8_t crit;

	// Constructor.
	Semaphore(uint_least8_t count, uint_least8_t crit) : count(count), crit(crit) { }

	// Releases all threads if count has reached dest.
	inline void post() {
		std::unique_lock<std::mutex> lock(mtx);
		++count;

		// Orders waiting threads to check the condition.
		cv.notify_all();

	}

	// Waits until count reaches the desired value.
	inline void wait() {
		std::unique_lock<std::mutex> lock(mtx);
		while (count != crit) cv.wait(lock);
		count = 0;

	}
};

// Global semaphore used to manage threads.
Semaphore threadingSemaphore(0, 0);

/*
Special verison of rand_s that returns an int and ignores errors.
*/
inline int randS() {
	unsigned int temp;
	rand_s(&temp);
	return temp & 2147483647;

}

/*
Special version of rand_s that returns an unsigned int and ignores errors.
*/
inline unsigned int randU() {
	unsigned int temp;
	rand_s(&temp);
	return temp;

}

/*
Special version of rand_s that returns a handful of bits.
*/
inline unsigned int randB(int n) {
	thread_local static unsigned int input = 0;
	unsigned int output;

	// Reassigns temp if it has run out of bits.
	if (!(input & ~((1 << n) - 1))) rand_s(&input);

	// Assigns the output variable.
	output = input % (1 << n);

	// Moves on to the next n bits.
	input >>= n;

	// Returns the output.
	return output;

}

/*
Initializes all data needed for threading.
*/
void initThreading() {

	// Initializes the thread count and threads.
	numThreads = std::thread::hardware_concurrency();
	if (numThreads <= 1) numThreads = 1;

	// TODO DEBUG REMOVE
	//numThreads = 1;

	// Initializes threads.
	threads = new std::thread*[numThreads];

	// Initializes the array for tracking threads.
	currThreads = new bool[numThreads];
	for (int i = 0; i < numThreads; ++i)
		currThreads[i] = true;

	// Initializes the array for tracking thread cleanup.
	cleanThreads = new bool[numThreads];
	for (int i = 0; i < numThreads; ++i) cleanThreads[i] = false;

}

/*
Returns the next available thread's index in threads. Cleans any threads used.
This should be accessed only by the main thread.
*/
int demandThread() {

	// Parses through the array of threads until an available thread is found.
	while (true) {
		for (int i = 0; i < numThreads; ++i) {
			if (currThreads[i]) {
				currThreads[i] = false;

				// Cleans thread if cleanup is needed.
				if (cleanThreads[i]) {
					threads[i]->join();
					delete threads[i];

				}

				// Returns the index of the available thread.
				return i;

			}
		}
	}
}

/*
Returns the neaxt available thread's index in threads, or -1 if no thread is avaialable. Cleans any threads used.
This may be accessed by any thread.
*/
int getThread() {
	static std::mutex getThreadMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(getThreadMutex);

	// Finds an available thread.
	for (int i = 0; i < numThreads; ++i) {
		if (currThreads[i]) {
			currThreads[i] = false;

			// Cleans thread if cleanup is needed.
			if (cleanThreads[i]) {
				threads[i]->join();
				delete threads[i];

			}

			// Returns the index of the available thread.
			return i;

		}
	}
}

/*
Waits for all threads to finish execution. Joins and deletes all of them.
*/
void joinAllThreads() {

	// Joins each thread.
	for (int i = 0; i < numThreads; ++i) {
		if (!currThreads[i]) {
			threads[i]->join();
			delete threads[i];
			cleanThreads[i] = false;
			currThreads[i] = true;

		}
	}

	// Cleans all remaining threads.
	joinComplete();

}

/*
Joins threads which have finished execution and cleans up their memory.
*/
void joinComplete() {
	for (int i = 0; i < numThreads; ++i) {
		if (cleanThreads[i]) {
			threads[i]->join();
			delete threads[i];
			cleanThreads[i] = false;

		}
	}
}

/*
Returns the number of unused threads.
*/
int unusedThreads() {
	int numUnused = 0;
	for (int i = 0; i < numThreads; ++i) numUnused += currThreads[i];
	return numUnused;

}

/*
Initializes numThreads int dummies. They are either the size of the universe or
the largest possible planet, whichever's larger. This should only be accessed in a
single threaded mannerone one time per game session.
Size is the total length of a dummy - all calculations for size should be done earlier.

This is called in beginGenerateSystems in Universe Generator.hpp
*/
void initDummies(int size) {
	int numDummies = numThreads;
	int* sizeDummy;

	// Creates threadDummies. Includes space to store dummy size and availability.
	threadDummies = new uint_least8_t[numDummies * (size * sizeof(uint_least8_t) + sizeof(uint_least8_t)) + sizeof(int)]();

	// The -4th index of threadDummies stores a 4 byte int containing size + 1.
	sizeDummy = (int*)threadDummies;
	sizeDummy[0] = size * sizeof(uint_least8_t) + sizeof(uint_least8_t);
	threadDummies += sizeof(int);

}

/*
Requests a dummy. Note that the dummy will not be cleared and must be
initialized elsewhere.
*/
uint_least8_t* requestDummy() {
	static std::mutex dummyMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(dummyMutex);

	// Increment to reach the next availability index.
	int dist = *((int*)threadDummies - 1);

	// Bound for parsing.
	uint_least8_t* end = threadDummies + (dist * numThreads);

	// threadDummies starts on the -1st index of the 1st array.
	uint_least8_t* dummyPointer;

	// Continuously searches for an available dummy until one is found.
	// Note: It is incredibly unlikely that this while loop will be necessarry,
	// but it is included for safety.
	while (true) {
		dummyPointer = threadDummies;
		for (; dummyPointer <= end - 1; dummyPointer += dist) {
			if (!dummyPointer[0]) {
				dummyPointer[0] = 1;
				return dummyPointer + 1;

			}
		}
	}
}

/*
Marks a dummy as available.
*/
inline void releaseDummy(uint_least8_t* dummy) {
	dummy[-1] = 0;

}

/*
Initializes numThreads bitarry dummies. They are the size of the largest possible planet.
This should only be accessed in a single threaded manner one time per game session.
Size is the total length of a dummy - all calculations for size should be done earlier.

This is called in beginGenerateSystems in Universe Generator.hpp
*/
void initLargeDummies(int size) {
	int numDummies = numThreads;
	int* sizeDummy;

	// Creates threadLargeDummies. Includes space to store dummy size and availability.
	threadLargeDummies = new uint_least8_t[numDummies * (size * 32 + sizeof(uint_least8_t)) + sizeof(int)]();

	// The -4th index of threadLargeDummies stores a 4 byte int containing size + 1.
	sizeDummy = (int*)threadLargeDummies;
	sizeDummy[0] = size * 32 + sizeof(uint_least8_t);

	// TODO DEBUG REMOVE (don't know why that's written, seems to work)
	threadLargeDummies += sizeof(int);

}

/*
Requests a dummy. Note that the dummy will not be cleared and must be
initialized elsewhere.
*/
uint_least8_t* requestLargeDummy() {
	static std::mutex dummyMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(dummyMutex);

	// Increment to reach the next availability index.
	int dist = *((int*)threadLargeDummies - 1);

	// Bound for parsing.
	uint_least8_t* end = threadLargeDummies + (dist * numThreads);

	// threadLargeDummies starts on the -1st index of the 1st array.
	uint_least8_t* dummyPointer;

	// Continuously searches for an available dummy until one is found.
	// Note: It is incredibly unlikely that this while loop will be necessarry,
	// but it is included for safety.
	while (true) {
		dummyPointer = threadLargeDummies;
		for (; dummyPointer <= end - 1; dummyPointer += dist) {
			if (!dummyPointer[0]) {
				dummyPointer[0] = 1;
				return dummyPointer + 1;

			}
		}
	}
}

/*
Marks a dummy as available.
*/
inline void releaseLargeDummy(uint_least8_t* dummy) {
	((uint_least8_t*)dummy)[-1] = 0;

}

/*
Accesses a given bit in a bitarray. Used for threadLargeDummies.
*/
inline bool readArrBit(uint_least8_t* arr, int bit) {
	return (arr[bit / 8] >> (bit % 8)) & 1;

}

/*
Writes a given bit an a bitarray. Used for threadLargeDummies.
*/
inline void writeArrBit(uint_least8_t* arr, int bit, bool write) {
	arr[bit / 8] ^= (-(uint_least8_t)write ^ arr[bit / 8]) & (1UL << (bit % 8));

}

/*
Requests a galaxy. Returns the index of the next available galaxy in galaxies, otherwise -1.
*/
int requestGalaxy() {
	static std::mutex requestGalaxyMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::mutex> lock(requestGalaxyMutex);

	// Parses through each galaxy and returns the next available one.
	for (int i = 0; i < numGals; ++i) {
		if (!galaxies[i].active) {
			galaxies[i].active = true;
			return i;

		}
	}

	// Returns -1 if no galaxy was found.
	return -1;

}

/*
Returns a galaxy.
*/
void releaseGalaxy(int index) {
	galaxies[index].active = false;

}

// Returns all galaxies.
void releaseAllGalaxies() {
	for (int i = 0; i < numGals; ++i) galaxies[i].active = false;

}