#pragma once

// Flattens two-dimensional indices for a one-dimensional array.
int index(int i, int j, int rowSize);

// Unflattens a one-dimensional index for a two dimensional-array.
inline void deIndex(int& i, int& j, int f, int rowSize);

// Calculates the distance between two points.
inline int calcTaxiDistance(int x1, int y1, int x2, int y2);

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
Flattens two-dimensional indices for a one-dimensional array.
*/
int inline index(int i, int j, int rowSize) {
	return i * rowSize +  j;

}

/*
Unflattens a one-dimensional index for a two dimensional-array.
*/
void inline deIndex(int& x, int& y, int i, int rowSize) {
	x = i / rowSize;
	y = i % rowSize;

}

/*
Calculates the distance between two points.
*/
inline int calcTaxiDistance(int x1, int y1, int x2, int y2) {
	return abs(x2 - x1) + abs(y2 - y1);

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
	//printf("\n");

}