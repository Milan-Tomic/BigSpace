#pragma once

// Gets the index for a one-dimensional array.
int index(int i, int j, int rowSize);

// Creates and returns an array of canvases.
int*** createCanvases(int canvasSize, int numCanvases);

// Creates and returns a canvas.
int** createCanvas(int canvasSize);

// Creates and returns a canvas.
int** createCanvas(int width, int height);

// Deallocates an array of canvases.
void deallocateCanvases(int*** canvases, int size, int numCanvases);

// Deallocates a canvas.
void deallocateCanvas(int** canvas, int size);

/*
Gets the index for a one-dimensional array.
*/
int inline index(int i, int j, int rowSize) {
	return i * rowSize +  j;

}

/*
Creates and returns an array of canvases.
*/
int*** createCanvases(int canvasSize, int numCanvases) {
	int*** canvases = new int** [numCanvases];
	for (int i = 0; i < numCanvases; ++i) canvases[i] = createCanvas(canvasSize);

	// Returns a pointer to the new canvases.
	return canvases;

}

/*
Creates and returns a reference to a canvas of size canvasSize.
*/
int** createCanvas(int canvasSize) {

	// Allocates the newCanvas.
	int** newCanvas = new int* [canvasSize];
	for (int i = 0; i < canvasSize; ++i) {
		newCanvas[i] = new int[canvasSize];

	}

	// Returns a pointer to the newCanvas.
	return newCanvas;

}

/*
Creates and returns a canvas with width width and height height.
*/
int** createCanvas(int width, int height) {

	// Allcoates the newCanvas.
	int** newCanvas = new int* [width];
	for (int i = 0; i < width; ++i) {
		newCanvas[i] = new int[height];

	}

	// Returns a pointer to the newCanvas.
	return newCanvas;

}

/*
Assigns all elements of a canvas to the inputed value.
*/
void zeroCanvas(int size, int** canvas, int value) {
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			canvas[i][j] = value;

}

/*
Assigns all elements of a canvas to the inputed value.
*/
void zeroCanvas(int width, int height, int** canvas, int value) {
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			canvas[i][j] = value;

}

/*
Deallocates an array of canvases.
*/
void deallocateCanvases(int*** canvases, int size, int numCanvases) {
	for (int i = 0; i < numCanvases; ++i) deallocateCanvas(canvases[i], size);
	delete[] canvases;

}

/*
Deallocates a canvas.
*/
void deallocateCanvas(int** canvas, int size) {
	for (int i = 0; i < size; ++i) delete[] canvas[i];
	delete[] canvas;

}

/*
Deallocates a canvas.
*/
void deallocateCanvas(int** canvas, int width, int height) {
	for (int i = 0; i < width; ++i) delete[] canvas[i];
	delete[] canvas;

}

/*
Returns a string representing the inputed bit string.
*/
void printBitString(long int str, int n) {
	for (--n; n >= 0; --n) printf("%d", !!(str & (1 << n)));

}