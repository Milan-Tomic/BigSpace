// ID counter for testClass.
static int testClassItemID;

// Class containing linearly increasing itemIDs.
class testClass {
public :
	// ID of the current testClass.
	int currID;

	// Assigns currID to the incremented itemID.
	testClass() {
		currID = ++testClassItemID;

	}

	// Assigns currID to the inputed int.
	testClass(int input) {
		currID = input;

	}

	// Returns currID.
	int getID() {
		return currID;

	}
};

//int testClass::testClassItemID = 0;