#pragma once

/*
Node within the linked list.
*/
template <class C>
struct LinkedListNode {
	C* value;
	LinkedListNode* child;
	LinkedListNode* previous; // used only for pathfinding.

};

/*
Simple class representing a singly linked list.
Since it is used for pathfinding, there are certain pathfinding specific methods.
	i.e. addcAsc.
To save cycles, there are several global lists of PathCoordinates corresponding to the amount of
threads available to the computer. These lists can be requested via the requestcoordlist function,
and reduce redundancy by reducing creation/destruction of LinkedListNodes.
Methods which are useful for getting coords generally contain a c. i.e. addcAsc, popc, etc.

This class takes 16 bytes.
*/
template <class C>
class LinkedList {
public:

	// Roota and tail of this LinkedList.
	LinkedListNode<C>* root;
	LinkedListNode<C>* tail;

	// Unused default constructor for LinkedList.
	LinkedList();

	// Default deconstructor for LinkedList.
	~LinkedList();

	// Checks if the linked list is empty.
	bool isEmpty();

	// Checks if the linked list contains the specific inputed item.
	bool contains(C* value);

	// Returns the size of this LinkedList.
	int size();

	// Adds an item to the LinkedList.
	void add(C* value);

	// Adds a LinkedListNode to the LinkedList.
	void addc(LinkedListNode<C>* node);

	// Adds a LinkedListNote to the LinkedList. Sorts by ascending value.
	void addcAsc(LinkedListNode<C>* node);

	// Removes an item from the LinkedList.
	void remove(C* value);

	// Pops the first item from the LinkedList. The item will be removed.
	C* pop();

	// Pops the first LinkedListNode from the LinkedList. Will create a new one if empty.
	LinkedListNode<C>* popc();

	// Polls an item from the LinkedList. The item will not be removed.
	C* poll();

	// Clears the LinkedList. Deletes values.
	void clear();
	
	// Empties the LinkedList. Does not delete values.
	void empty();

};

/*
Unused default constructor for LinkedList. Does nothing.
*/
template<class C>
LinkedList<C>::LinkedList() {
	root = nullptr;

}

// Default deconstructor for LinkedList. Deletes all nodes without deleting their contents.
template<class C>
LinkedList<C>::~LinkedList() {
	empty();

}

/*
Returns true if the list is empty, otherwise false.
*/
template<class C>
bool LinkedList<C>::isEmpty() {
	return root == nullptr;

}

/*
Returns true if the item is contained within this list.
*/
template<class C>
bool LinkedList<C>::contains(C* value) {

	// Checks to see if the list contains the inputed value.
	LinkedListNode<C>* currNode = root;
	int i = 0;
	while (currNode != nullptr && *(currNode->value) != *value) currNode = currNode->child;
	return currNode != nullptr;

}

/*
Returns the size of this LinkedList.
*/
template<class C>
int LinkedList<C>::size() {
	int size = 0;
	LinkedListNode<C>* currNode = root;

	// Finds the size by parsing through all items.
	while (currNode != nullptr) {
		++size;
		currNode = currNode->child;

	}

	// Returns the counted size.
	return size;

}

/*
Adds an item to the LinkedList.
*/
template <class C>
void LinkedList<C>::add(C* value) {
	LinkedListNode<C>* newNode = new LinkedListNode<C>;
	newNode->value = value;
	newNode->child = nullptr;

	// If the root is null, assigns value to it.
	if (root == nullptr) {
		root = newNode;
		tail = newNode;
		return;

	}

	// If the root is not null, finds the end of the list to put value in.
	tail->child = newNode;
	tail = newNode;

}

/*
Adds a LinkedListNode to the LinkedList.
*/
template<class C>
void LinkedList<C>::addc(LinkedListNode<C>* node) {

	// If the root is null, assigns node to it.
	if (root == nullptr) {
		root = node;
		tail = node;
		return;

	}

	// If the root is not null, finds the end of the list and places the node there.
	tail->child = node;
	tail = node;

}

/*
Adds a LinkedListNote to the LinkedList. Sorts by ascending value.
*/
template <class C>
void LinkedList<C>::addcAsc(LinkedListNode<C>* node) {

	// If the root is null, assigns node to it.
	if (root == nullptr) {
		root = node;
		tail = node;
		return;

	}

	// If the node is less than the root, places it there and returns.
	if (*(node->value) < *(root->value)) {
		node->child = root;
		root = node;
		return;

	}

	// If the node is greater than the tail, places it there and returns.
	if (*(node->value) >= *(tail->value)) {
		tail->child = node;
		tail = node;
		return;

	}

	// If the root is not null, finds a place for the node and places it there.
	LinkedListNode<C>* currNode = root;
	while (*(currNode->child->value) < *(node->value)) currNode = currNode->child;
	node->child = currNode->child;
	currNode->child = node;

}

/*
Removes an item from the LinkedList.
Destroys nodes.
*/
template <class C>
void LinkedList<C>::remove(C* value) {
	LinkedListNode<C>* toDelete;

	// If the node to be deleted is the root, deletes and replaces it.
	if (root->value == value) {
		toDelete = root;
		root = root->child;
		delete toDelete;
		return;

	}

	// Parses through the LinkedList until value is found.
	LinkedListNode<C>* currNode = root;
	while (currNode->child->value != value) {
		currNode = currNode->child;
		if (currNode->child == nullptr) return;

	}

	// If the tail is being deleted, reassigns the tail.
	if (currNode->child == tail) tail = currNode;

	// Deletes the child.
	toDelete = currNode->child;
	currNode->child = currNode->child->child;
	delete toDelete;

}

/*
Pops the first item from the LinkedList.
*/
template <class C>
C* LinkedList<C>::pop() {

	// Returns nothing if the LinkedList is empty.
	if (root == nullptr) return nullptr;

	// Deletes root and returns its value.
	C* value = root->value;
	LinkedListNode<C>* toDelete = root;
	root = root->child;
	delete toDelete;
	if (root == nullptr) tail = nullptr;
	return value;

}

/*
Pops the first LinkedListNode from the LinkedList. Will create a new item if none are present.
*/
template <class C>
LinkedListNode<C>* LinkedList<C>::popc() {

	// Deletes root and returns its value.
	LinkedListNode<C>* value = root;
	root = root->child;
	value->child = nullptr;
	return value;

}

/*
Polls the first item from the LinkedList.
*/
template <class C>
C* LinkedList<C>::poll() {

	// Returns the root.
	return root->value;

}

/*
Clears the LinkedList. Deletes values.
*/
template <class C>
void LinkedList<C>::clear() {

	// Clears every root and its value.
	while (!isEmpty()) {
		LinkedListNode<C>* toDelete = root;
		root = root->child;
		delete toDelete->value;
		delete toDelete;

	}
}

/*
Empties the LinkedList. Does not delete values.
*/
template <class C>
void LinkedList<C>::empty() { while (!isEmpty()) pop(); }
