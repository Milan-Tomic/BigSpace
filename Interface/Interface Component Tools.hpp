#pragma once

// Quantity to change button colour by when clicked.
#define BUTTON_RECOLOUR 40

// Pointer to the Button that is currently being clicked.
CodeButton* clickedButton;

// Deallocates an array of Buttons.
void deallocateButtons(Button* buttons, int numButtons);

// Deallocates an array of CodeButtons.
void deallocateButtons(CodeButton* buttons, int numButtons);

// Deallocates an array of TextBoxes.
void deallocateBoxes(TextBox* boxes, int numBoxes);

// Returns a reference to a clicked CodeButton in an array of CodeButtons.
CodeButton* getClickedButton(CodeButton* buttons[], int numButtons, int xPos, int yPos);

// Returns the ButtonCode of the inputed CodeButton that the xPos and yPos area in bounds of.
ButtonCode getCode(CodeButton* buttons[], int numButtons, int xPos, int yPos);

// Returns a reference to a clicked TextBox in an array of TextBoxes.
TextBox* getClickedTextBox(TextBox* boxes[], int numBoxes, int xPos, int yPos);

/*
Deallocates an array of Buttons.
*/
void deallocateButtons(Button* buttons[], int numButtons) {
	for (int i = 0; i < numButtons; ++i) delete buttons[i];

}

/*
Deallocates an array of CodeButtons.
*/
void deallocateButtons(CodeButton* buttons[], int numButtons) {
	for (int i = 0; i < numButtons; ++i) delete buttons[i];

}

/*
Deallocates an array of TextBoxes.
*/
void deallocateBoxes(TextBox* boxes[], int numBoxes) {
	for (int i = 0; i < numBoxes; ++i) delete boxes[i];

}

/*
Returns a reference to a clicked CodeButton in an array of CodeButtons.
Returns nullptr if no CodeButton was clicked.
*/
CodeButton* getClickedButton(CodeButton* buttons[], int numButtons, int xPos, int yPos) {
	for (int i = 0; i < numButtons; ++i) {
		if (withinBounds(xPos, yPos, buttons[i]->area)) return buttons[i];

	}
	return nullptr;

}

/*
Returns the ButtonCode of the inputed CodeButton that the xPos and yPos area is in bounds of.
Returns NULL_CODE when no suitable CodeButton was found.
*/
ButtonCode getCode(CodeButton* buttons[], int numButtons, int xPos, int yPos) {
	for (int i = 0; i < numButtons; ++i) {
		if (withinBounds(xPos, yPos, buttons[i]->area)) return buttons[i]->code;

	}
	return NULL_CODE;

}

/*
Returns a reference to a clicked TextBox in an array of TextBoxes.
Returns nullptr if no TextBox was clicked.
*/
TextBox* getClickedTextBox(TextBox* boxes[], int numBoxes, int xPos, int yPos) {
	for (int i = 0; i < numBoxes; ++i) {
		if (withinBounds(xPos, yPos, boxes[i]->area)) return boxes[i];

	}
	return nullptr;

}