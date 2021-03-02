#include "stdio.h"
#include "testClass.hpp"
#include "BST.hpp"

void BSTMain();
bool BSTAddRoot();
bool BSTAddLeft();
bool BSTAddRight();
bool BSTAddDuplicate();
bool BSTGetHeight();
bool BSTRemoveSingleRoot();
bool BSTRemoveLeaves();
bool BSTRemoveLeftMid();
bool BSTRemoveRightMid();
bool BSTRemovePredecessor();

// Hub for running tests. Comment out undesired tests.
int main() {

	// Runs tests on BST.
	BSTMain();

}

void BSTMain() {
	int numFails = 0;

	// Checks that the root of the BST can be assigned.
	if (!BSTAddRoot()) {
		printf("BSTAddRoot failed.\n");
		++numFails;

	}

	// Checks that the BST can add left children.
	if (!BSTAddLeft()) {
		printf("BSTAddLeft failed.\n");
		++numFails;

	}

	// Checks that the BST can add right children.
	if (!BSTAddRight()) {
		printf("BSTAddRight failed.\n");
		++numFails;

	}

	// Checks that the BST does not add duplicate items.
	if (!BSTAddDuplicate()) {
		printf("BSTAddDuplicate failed.\n");
		++numFails;

	}

	// Checks that getHeight returns the correct value.
	if (!BSTGetHeight()) {
		printf("BSTGetHeight failed.\n");
		++numFails;

	}

	// Checks that a childless root can be removed from the BST.
	if (!BSTRemoveSingleRoot()) {
		printf("BSTRemoveSingleRoot failed.\n");
		++numFails;

	}

	// Checks that childless leaves can be removed from the BST.
	if (!BSTRemoveLeaves()) {
		printf("BSTRemoveLeaves failed.\n");
		++numFails;

	}

	// Checks that left nexus nodes can be removed from the BST.
	if (!BSTRemoveLeftMid()) {
		printf("BSTRemoveLeftMid failed.\n");
		++numFails;

	}

	// Checks that right nexus nodes can be removed from the BST.
	if (!BSTRemoveRightMid()) {
		printf("BSTRemoveRightMid failed.\n");
		++numFails;

	}

	// Checks that in-order predecessor replacement is possible.
	if (!BSTRemovePredecessor()) {
		printf("BSTRemovePredecessor failed.\n");
		++numFails;

	}

	// Prints number of failures if numFails is not 0.
	if (numFails) {
		printf("Failed %d tests in BSTMain.\n\n", numFails);

	}
	// Prints success message if numFails is 0.
	else {
		printf("BSTMain ran with no errors.\n\n");

	}
}

// Checks that the root of the BST can be assigned.
bool BSTAddRoot() {
	BST<testClass> *testBST = new BST<testClass>();

	testClass *t1 = new testClass(450);
	testBST->addItem(t1);

	// If root is assigned or retrieved incorrectly, return false.
	if (testBST->getRoot()->getID() != 450) {
		delete t1;
		delete testBST;
		return false;

	}
	// If root is assigned and retrieved correctly, return true.
	else {
		return true;

	}
}

// Checks that the BST can add left children.
bool BSTAddLeft() {

	BST<testClass> *testBST = new BST<testClass>();

	testClass *t1 = new testClass(35);
	testClass *t2 = new testClass(1);
	testClass *t3 = new testClass(-2);
	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 3) {
		return false;

	}

	// If height is correct, returns true.
	return true;

}

// Checks that the BST can add right children.
bool BSTAddRight() {

	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(35);
	testClass* t2 = new testClass(40);
	testClass* t3 = new testClass(41);
	testClass* t4 = new testClass(50);
	testClass* t5 = new testClass(10000);
	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 5) {
		return false;

	}

	// If height is correct, returns true.
	return true;

}

// Checks that the BST does not add items with the same ID.
bool BSTAddDuplicate() {

	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(0);
	testClass* t2 = new testClass(0);

	testBST->addItem(t1);
	testBST->addItem(t1);
	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t2);

	// Returns false if the height i s incorrect.
	if (testBST->getHeight() != 1) {
		return false;

	}

	// If the height is correct, returns true.
	return true;

}

// Checks that getHeight returns the correct height.
bool BSTGetHeight() {

	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(50);
	testClass* t2 = new testClass(25);
	testClass* t3 = new testClass(75);
	testClass* t4 = new testClass(0);
	testClass* t5 = new testClass(100);
	testClass* t6 = new testClass(30);
	testClass* t7 = new testClass(70);
	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);
	testBST->addItem(t6);
	testBST->addItem(t7);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 3) {
		return false;

	}

	// If height is correct, returns true.
	return true;

}

// Checks that a single root can be removed from the BST.
bool BSTRemoveSingleRoot() {

	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(50);

	testBST->addItem(t1);
	testBST->removeItem(t1);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 0) {
		return false;

	}

	// Returns false if the root is not NULL.
	if (testBST->getRoot() != NULL) {
		return false;

	}

	// If the height is correct, returns true.
	return true;

}

// Removes several leaves of the BST.
bool BSTRemoveLeaves() {
	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(50);
	testClass* t2 = new testClass(40);
	testClass* t3 = new testClass(60);
	testClass* t4 = new testClass(35);
	testClass* t5 = new testClass(45);
	testClass* t6 = new testClass(55);
	testClass* t7 = new testClass(65);

	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);
	testBST->addItem(t6);
	testBST->addItem(t7);
	testBST->removeItem(t4);
	testBST->removeItem(t5);
	testBST->removeItem(t6);
	testBST->removeItem(t7);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 2) {
		return false;

	}

	// If the height is correct, returns true.
	return true;

}

// Removes a left midpoint of the BST.
bool BSTRemoveLeftMid() {
	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(50);
	testClass* t2 = new testClass(40);
	testClass* t3 = new testClass(60);
	testClass* t4 = new testClass(35);
	testClass* t5 = new testClass(45);
	testClass* t6 = new testClass(55);
	testClass* t7 = new testClass(65);

	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);
	testBST->addItem(t6);
	testBST->addItem(t7);
	testBST->removeItem(t2);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 3) {
		return false;

	}

	// If the height is correct, returns true.
	return true;

}

// Removes a right midpoint of the BST.
bool BSTRemoveRightMid() {
	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(50);
	testClass* t2 = new testClass(40);
	testClass* t3 = new testClass(60);
	testClass* t4 = new testClass(35);
	testClass* t5 = new testClass(45);
	testClass* t6 = new testClass(55);
	testClass* t7 = new testClass(65);

	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);
	testBST->addItem(t6);
	testBST->addItem(t7);
	testBST->removeItem(t3);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 3) {
		return false;

	}

	// If the height is correct, returns true.
	return true;

}

// Checks that in-order predecessor replacement is possible.
bool BSTRemovePredecessor() {
	BST<testClass>* testBST = new BST<testClass>();

	testClass* t1 = new testClass(0);
	testClass* t2 = new testClass(50);
	testClass* t3 = new testClass(25);
	testClass* t4 = new testClass(30);
	testClass* t5 = new testClass(40);
	testClass* t6 = new testClass(45);
	testClass* t7 = new testClass(42);
	testClass* t8 = new testClass(41);

	testBST->addItem(t1);
	testBST->addItem(t2);
	testBST->addItem(t3);
	testBST->addItem(t4);
	testBST->addItem(t5);
	testBST->addItem(t6);
	testBST->addItem(t7);
	testBST->addItem(t8);
	testBST->removeItem(t2);

	// Returns false if the height is incorrect.
	if (testBST->getHeight() != 7) {
		return false;

	}

	// If the height is correct, returns true.
	return true;
}