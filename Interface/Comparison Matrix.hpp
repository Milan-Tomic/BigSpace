#pragma once

/*
This class represents a comparison matrix. The comparison matrix uses a one dimensional array
to store a two dimensional stepped (no reflexive entries) comparison matrix.

This class takes 16 bytes.
*/
class ComparisonMatrix {
private:

	// Helper function to find the index at the head of the inputed row.
	int rowGetter(int row);

public:

	// Contains the comparison matrix.
	uint_least8_t* matrix;

	// Number of rows in the comparison matrix.
	int numRows;

	// These four bytes would be wasted if not used here.
	// short1 is used for numFronts, byte1 is used for numOwners.
	uint_least16_t short1;
	uint_least8_t byte1;
	uint_least8_t byte2;

	// Constructor for the comparison matrix.
	void init(int numValues);

	// Deconstructor for the comparison matrix.
	void clear();

	// Returns the value comparing the inputed indices.
	bool comp(int o, int i);

	// Assigns the index corresponding to the inputed items to the inputed value.
	void assign(int o, int i, uint_least8_t val);

	// Helper function to get the number of elements in matrix.
	int size();

	// Prints the matrix.
	// TODO DEBUG REMOVE
	void printComp();

};

/*
Initializes numRows and the matrix.
*/
void ComparisonMatrix::init(int numValues) {
	numRows = numValues - 1;
	matrix = (uint_least8_t*)calloc(rowGetter(numRows), sizeof(uint_least8_t));

}

/*
Clears the matrix.
*/
void ComparisonMatrix::clear() {
	free(matrix);

}

/*
Returns the value located within the matrix at the given point.
Note that the two functions are mirrors. Also note that in the case of equivalence
0 is returned as reflexive entries are not included.
*/
bool ComparisonMatrix::comp(int o, int i) {
	if (i == o) return 0;
	else if (o > i) return matrix[rowGetter(i) + o - i - 1];
	else return matrix[rowGetter(o) + i - o];

}

/*
Assigns the index corresponding to the inputed items to the inputed value.
*/
void ComparisonMatrix::assign(int o, int i, uint_least8_t val) {
	if (o > i) matrix[rowGetter(i) + o - i - 1] = val;
	else matrix[rowGetter(o) + i - o] = val;

}

/*
Returns the first index of the row corresponding the the inputed index.
Note: returns matrix size when row is equal to numRows.
*/
int ComparisonMatrix::rowGetter(int row) {
	return (int)(pow(row, 2) + row) / 2 + (numRows - row) * row;

}

/*
Returns the size of the matrix.
*/
int ComparisonMatrix::size() {
	return rowGetter(numRows);

}

/*
Prints the contents of the comparison matrix.
TODO DEBUG REMOVE
*/
void ComparisonMatrix::printComp() {
	int currLetter = 65;
	for (int i = 0; i < numRows + 1; ++i) printf("%*c", 2, currLetter + i);
	printf("\n");
	for (int i = 0; i < numRows; ++i) {
		printf("%*s", 2 * i + 3, "");
		for (int j = 0; j < numRows - i; ++j) {
			if (!matrix[rowGetter(i) + j]) printf("0 ");
			else printf("%c ", matrix[rowGetter(i) + j]);

		}
		printf("%*c\n", 2, currLetter++);
	}
	printf("%*c\n\n", numRows * 2 + 5, currLetter++);

}