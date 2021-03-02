#include <stddef.h>		// For NULL.
#include <vector>		// For returning ordered list.

template <class C>
struct BSTNode {
	C *value;
	BSTNode *left;
	BSTNode *right;

	BSTNode(C *input) {
		this->value = input;
		this->left = NULL;
		this->right = NULL;

	}

	BSTNode(C *input, BSTNode<C> *left, BSTNode<C> *right) {
		this->value = input;
		this->left = left;
		this->right = right;

	}
};

template <class C>
class BST {
private :
	BSTNode<C> *root;

public:
	BST() {};
	//BST(const BST&) = default;

	/*
	Adds an item of Class C to the BST. Does so by parsing down the BST, left if the input value's
	getID is smaller than the node being examined, right if the input value's getID is larger than
	the value being examined. This BST does not allow for redundant items, and will not add a value
	that is already present.
	*/
	void addItem(C *addObj) {

		// Assigns addObj to root and returns if root is null.
		if (root == NULL) {
			root = new BSTNode<C>(addObj);
			return;

		}

		// Parses to the bottom of the BST to add addObj.
		int addID = addObj->getID();
		BSTNode<C> *currNode = root;
		while (true) {

			// If addObj has a smaller ID than the BSTNode being examined, parses through its left subtree.
			while (addID < currNode->value->getID()) {

				// If the bottom of the BST is found, adds addObj there and returns.
				if (currNode->left == NULL) {
					currNode->left = new BSTNode<C>(addObj);
					return;

				}
				// If the bottom of the BST is not found, continues parsing.
				else {
					currNode = currNode->left;

				}
			}
			// If addObj has a larger ID than the BSTNode being examined,paarses through its right subtree.
			while (addID > currNode->value->getID()) {

				// If the bottom of the BST is found, adds addObj there and returns.
				if (currNode->right == NULL) {
					currNode->right = new BSTNode<C>(addObj);
					return;

				}
				// If the bottom of the BST is not found, continues parsing.
				else {
					currNode = currNode->right;

				}
			}
			// If the BSTNode being examined has the same ID as addObj, returns.
			if (addID == currNode->value->getID()) {
				return;

			}
		}
	}

	/*
	Finds the in-order predecessor to the inputed BSTNode.
	*/
	BSTNode<C>* findReplacement(BSTNode<C> *currNode) {

		// Checks to see if currNode has a NULL left child.
		if (currNode->left == NULL) {
			// If the left child is NULL and the right child is not, returns the right child.
			if (currNode->right != NULL) {
				return currNode->right;

			}
			// If both children are NULL, returns NULL.
			else {
				return NULL;

			}
		}

		// If the left child is not NULL, finds currNode's in-order predecessor.
		currNode = currNode->left;
		while (currNode->right != NULL) {
			currNode = currNode->right;

		}

		// Returns the in-order predecessor.
		return currNode;

	}

	/*
	Removes the Object containing an inputed ID from the BST.
	*/
	void removeItem(int removeID) {
		BSTNode<C>* currNode = root;
		BSTNode<C>* parNode = NULL;

		// If the tree is empty, returns.
		if (root == NULL) {
			return;

		}

		// Parses through the BST to find removeObj.
		while (true) {

			// If addObj has a smaller ID than the BSTNode being examined, parses through its left subtree.
			while ((removeID < currNode->value->getID()) && (removeID != currNode->value->getID())) {

				// If the bottom of the BST is found, returns.
				if (currNode->left == NULL) {
					return;

				}
				// If the bottom of the BST is not found, continues parsing.
				else {
					parNode = currNode;
					currNode = currNode->left;

				}
			}
			// If addObj has a larger ID than the BSTNode being examined, parses through its right subtree.
			while (removeID > currNode->value->getID()) {

				// If the bottom of the BST is found, returns.
				if (currNode->right == NULL) {
					return;

				}
				// If the bottom of the BST is not found, continues parsing.
				else {
					parNode = currNode;
					currNode = currNode->right;

				}
			}
			// If the BSTNode being examined has the same ID as removeObj, calls findReplacement and replaces currNode.
			if (removeID == currNode->value->getID()) {
				BSTNode<C>* repNode = findReplacement(currNode);

				// Replaces currNode if curNode is root.
				if (parNode == NULL) {

					// If root has no children, assigns root to NULL and returns.
					if (repNode == NULL) {
						delete root;
						root = NULL;
						return;

					}
					// If root has only a right or left child, assigns root to that child and returns.
					else if (repNode == root->right || (repNode == root->left && root->right == NULL)) {
						delete root;
						root = repNode;
						return;

					}
					// If root has an in order predecessor, replaces root with it, removes it, and returns.
					else {
						C *repObj = repNode->value;
						removeItem(repNode->value->getID());
						BSTNode<C> *remNode = root;
						root = new BSTNode<C>(repObj, root->left, root->right);
						delete remNode;
						return;

					}
				}
				// Replaces currNode if currNode is not the root.
				else {
					bool isLeft = false;

					// Assigns isLeft.
					if (currNode == parNode->left) {
						isLeft = true;

					}

					// If currNode has no children, replaces currNode with NULL and returns.
					if (repNode == NULL) {
						delete currNode;

						// Replaces currNode based on whether it is a right or left child.
						if (isLeft) {
							parNode->left = NULL;

						}
						else {
							parNode->right = NULL;

						}
						return;

					}
					// If currNode has only a right or left child, replaces currNode with it and returns.
					else if (repNode == currNode->right || (repNode == currNode->left && currNode->right == NULL)) {
						delete currNode;

						// Replaces currNode based on whether it is a right or left child.
						if (isLeft) {
							parNode->left = repNode;

						}
						else {
							parNode->right = repNode;

						}
						return;

					}
					// If currNode has an in-order predecessor, replaces currNode with it, removes it, and returns.
					else {
						C* repObj = repNode->value;
						removeItem(repNode->value->getID());
						BSTNode<C>* remNode = currNode;

						// Replaces currNode based on whether it is a right or left child.
						if (isLeft) {
							parNode->left = new BSTNode<C>(repObj, currNode->left, currNode->right);

						}
						else {
							parNode->right = new BSTNode<C>(repObj, currNode->left, currNode->right);

						}
						delete remNode;
						return;

					}
				}
			}
		}
	}

	/*
	Removes an Object from the BST. Does so by calling removeItem with the int ID of the Object.
	*/
	void removeItem(C *removeObj) {
		removeItem(removeObj->getID());

	}

	/*
	Returns the root value of the BST. Used for debugging.
	*/
	C* getRoot() {
		if (root == NULL) {
			return NULL;

		}
		return root->value;

	}

	/*
	Helps get the height of the BST. Does so by recursively parsing through each BSTNode's 
	left and right subtrees and returning the greater of the two.
	*/
	int getHeightHelper(BSTNode<C> *currNode) {

		// Base case. Returns 0 if a NULL BSTNode has been reached.
		if (currNode == NULL) {
			return 0;

		}

		// Gets the heights of the left and right subtrees of currNode.
		int left = getHeightHelper(currNode->left);
		int right = getHeightHelper(currNode->right);

		// Returns the greater height of currNode's left and right subtrees, incremented by 1.
		return 1 + ((left > right) ? left : right);

	}

	/*
	Gets the height of the BST. Used for debugging.
	*/
	int getHeight() {
		return getHeightHelper(root);

	}

	/*
	Creates an ordered list of Classes in the BST stored as a vector.
	*/
	void getInOrderTraversal() {

	}
};