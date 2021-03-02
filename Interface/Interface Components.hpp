#pragma once

/*
A Sprite containing a Texture and an SDL_Rect corresponding to that texture's area.
*/
class Sprite {
public:
	// The area of this sprite.
	SDL_Rect area;

	// The texture of this sprite.
	SDL_Texture* texture;

	// Default constructor for Sprite.
	Sprite();

	// Standard constructor for Sprite. Initializes the Sprite's area and texture.
	Sprite(int x, int y, int w, int h, SDL_Texture* texture);

	// Constructor for a Sprite containing text. Initializes the Sprite's area and texture.
	Sprite(int x, int y, int w, int h, std::string text);

	// Constructor for a Sprite containing text. Will attempt to make the sprite a certain width.
	Sprite(int x, int y, int w, int h, std::string text, int charWidth);

	// Deconstructor for a Sprite. Deletes texture.
	~Sprite();

	// Renders this Sprite.
	void render(int x, int y) {
		SDL_Rect tempArea = { area.x + x, area.y + y, area.w, area.h };
		SDL_RenderCopy(renderer, texture, NULL, &tempArea);

	}
};

/*
Default constructor. Does nothing.
*/
Sprite::Sprite() {
	area = {0, 0, 0, 0};
	texture = nullptr;

}

/*
Standard constructor for a Sprite. Initializes the sprites area and texture.
*/
Sprite::Sprite(int x, int y, int w, int h, SDL_Texture* texture) {
	this->area = {x, y, w, h};
	this->texture = texture;

}

/*
Constructor for a Sprite containing text. Initializes the Sprite's area and texture.
*/
Sprite::Sprite(int x, int y, int w, int h, std::string text) {
	this->area = { x, y, w, h };
	SDL_Surface* nameSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	texture = SDL_CreateTextureFromSurface(renderer, nameSurface);
	SDL_FreeSurface(nameSurface);

}

/*
Constructor for a Sprite containing text. Initializes the Sprite's area and texture.
Attempts to square the text with the inputed charwidth. May override w.
*/
Sprite::Sprite(int x, int y, int w, int h, std::string text, int charWidth) {
	w = w < text.length() * charWidth ? w : text.length() * charWidth;
	this->area = { x, y, w, h };
	SDL_Surface* nameSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	texture = SDL_CreateTextureFromSurface(renderer, nameSurface);
	SDL_FreeSurface(nameSurface);

}

/*
Deconstructor for a Sprite. Deletes texture.
*/
Sprite::~Sprite() {
	SDL_DestroyTexture(texture);
	texture = NULL;

}

/*
A button that performs an action or returns a button code when clicked.
*/
class Button {
public:
	// Default constructor for Button.
	Button();

	// Initializes the button's SDL_Rect.
	Button(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, std::string text);

	// Deconstructor for Button.
	~Button();

	// Sets the colour of this button.
	void setColour(Uint8 r, Uint8 g, Uint8 b);

	// Changes the colour of this Button by the specified amount.
	void changeColour(int change);

	// Resizes this button.
	void resize(int w, int h, int textLen);

	// Resizes this button and moves it.
	void resize(int x, int y, int w, int h, int textLen);

	// Renders this Button with no offsets.
	void render();

	// Renders this Button.
	void render(int x, int y);

	// Current colour values of this button.
	SDL_Colour buttonColour;

	// Location and area of this button.
	SDL_Rect area;

	// Location and area of the text of this button.
	SDL_Rect textArea;

	// Texture of text to be displayed by this button.
	SDL_Texture* buttonText;

};

/*
Default constructor for Button.
*/
Button::Button() {
	Button::buttonColour = {0, 0, 0, 0};
	Button::area = { 0, 0, 0, 0 };
	textArea = {0, 0, 0, 0};
	SDL_Surface* textSurface = TTF_RenderText_Blended(defaultFont, "", {0, 0, 0, 0xFF});
	Button::buttonText = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);

}

/*
Constructor for any given button. Initializes location, size, and colour.
*/
Button::Button(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, std::string text) {
	Button::area = {x, y, w, h};
	buttonColour = {r, g, b, 255};
	SDL_Surface* textSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	Button::buttonText = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	w = w < text.length() * w / 20 ? w : text.length() * w / 20;
	textArea = { (int)(1.01 * x), (int)(1.01 * area.y) + 2, (int)w, (int)(0.98 * area.h) };

}

/*
Deconstructor for Button. Deletes buttonText and buttonColour.
*/
Button::~Button() {
	SDL_DestroyTexture(buttonText);
	buttonText = NULL;

}

/*
Assigns the buttons colour.
*/
void Button::setColour(Uint8 r, Uint8 g, Uint8 b) {
	buttonColour.r = r;
	buttonColour.g = g;
	buttonColour.b = b;

}

/*
Changes the buttons colour by adding the inputed change to r, g, and b.
*/
void Button::changeColour(int change) {
	buttonColour.r += change;
	buttonColour.g += change;
	buttonColour.b += change;

}

/*
Resizes the button.
*/
void Button::resize(int w, int h, int textLen) {
	area.w = w;
	area.h = h;
	w = w < (textLen * w) / 20 ? w : (textLen * w) / 20;
	textArea = { area.x + (area.w - w) / 2, area.y + h / 100, w, (int)(0.98 * area.h) };

}

/*
Resizes the button and moves it.
*/
void Button::resize(int x, int y, int w, int h, int textLen) {
	area = {x, y, w, h};
	w = w < (textLen * w) / 20 ? w : (textLen * w) / 20;
	textArea = { area.x + (area.w - w) / 2, area.y + h / 100, w, (int)(0.98 * area.h) };

}

/*
Renders the button with no offsets.
*/
void Button::render() {

	// Renders the Button.
	SDL_SetRenderDrawColor(renderer, buttonColour.r, buttonColour.g, buttonColour.b, buttonColour.a);
	SDL_Rect tempArea = {area.x, area.y, area.w, area.h};
	SDL_RenderFillRect(renderer, &tempArea);

	// Renders the Button's text.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_Rect tempTextArea = {textArea.x, textArea.y, textArea.w, textArea.h };
	SDL_RenderCopy(renderer, buttonText, NULL, &tempTextArea);

}

/*
Renders the Button. x and y are offsets.
*/
void Button::render(int x, int y) {

	// Renders the Button.
	SDL_SetRenderDrawColor(renderer, buttonColour.r, buttonColour.g, buttonColour.b, buttonColour.a);
	SDL_Rect tempArea = { x + area.x, y + area.y, area.w, area.h };
	SDL_RenderFillRect(renderer, &tempArea);

	// Renders the Button's text.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_Rect tempTextArea = {x + textArea.x, y + textArea.y, textArea.w, textArea.h};
	SDL_RenderCopy(renderer, buttonText, NULL, &tempTextArea);

}

// enum representing the possible buttons.
enum ButtonCode {
	NULL_CODE = -1, // Symbolizes absence of a meaningful ButtonCode
	OPENING_SCREEN, // Opening Screen
	OPENING_NEW_GAME,
	OPENING_LOAD_GAME,
	OPENING_OPTIONS,
	OPENING_QUIT,
	OPENING_END,
	NEW_GAME_GALAXIES, // New Game Screen
	NEW_GAME_ADVANCED,
	NEW_GAME_GENERATE,
	NEW_GAME_PLAY,
	NEW_GAME_END,
	UNI_CUSTOM_QUIT, // Universe Customization Screen
	UNI_CUSTOM_CLEAR,
	UNI_CUSTOM_DELETE,
	UNI_CUSTOM_END,
	GAL_DESIGN, // Galaxy Design Screens
	GAL_DESIGN_FINISH,
	SYST_VIEW_CHANGE // System View.

};

/*
A generic button that returns a ButtonCode.
*/
class CodeButton : public Button {
public:

	// Default constructor for a CodeButton.
	CodeButton();

	// Initializes the Button's SDL_Rect, colour, and ButtonCode.
	CodeButton(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, ButtonCode code, std::string text);

	// Initializes the Button's SDL_Rect and ButtonCode. Colour is grey automatically.
	CodeButton(int x, int y, int w, int h, ButtonCode code, std::string text);

	// Reinitializes the Button's SDL_Rect and ButtonCode. Colour is grey automatically.
	void reinit(int x, int y, int w, int h, ButtonCode code, std::string text);

	// The ButtonCode for this CodeButton.
	ButtonCode code;

};

// Default constructor for a CodeButton. Does nothing.
CodeButton::CodeButton() {

}

/*
Initializes a CodeButton.
*/
CodeButton::CodeButton(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, ButtonCode code, std::string text) {
	area = { x, y, w, h };
	buttonColour = {r, g, b, 255};
	this->code = code;
	SDL_Surface* textSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	Button::buttonText = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	w = w < (text.length() * w) / 20 ? w : (text.length() * w) / 20;
	textArea = { area.x + (area.w - w) / 2, area.y + h / 100, w, (int)(0.98 * area.h) };

}

/*
Initializes the Button's SDL_Rect and ButtonCode. Colour is grey automatically.
*/
CodeButton::CodeButton(int x, int y, int w, int h, ButtonCode code, std::string text) {
	area = {x, y, w, h};
	buttonColour = {80, 80, 80, 255};
	this->code = code;
	SDL_Surface* textSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	Button::buttonText = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	w = w < (text.length() * w) / 20 ? w : (text.length() * w) / 20;
	textArea = { area.x + (area.w - w) / 2, area.y + h / 100, w, (int)(0.98 * area.h) };

}

/*
Reinitializes the Button's SDL_Rect and ButtonCode. Colour is grey automatically.
*/
void CodeButton::reinit(int x, int y, int w, int h, ButtonCode code, std::string text) {
	area = { x, y, w, h };
	buttonColour = { 80, 80, 80, 255 };
	this->code = code;
	SDL_Surface* textSurface = TTF_RenderText_Blended(defaultFont, text.c_str(), { 0, 0, 0, 0xFF });
	Button::buttonText = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	w = w < (text.length() * w) / 20 ? w : (text.length() * w) / 20;
	textArea = { area.x + (area.w - w) / 2, area.y + h / 100, w, (int)(0.98 * area.h) };

}

/*
Box that the user can type in when clicked on.
*/
class TextBox {
public:

	// Minimum and maximum values for this TextBox's int. min != max symbolizes being purely numerical.
	int min;
	int max;

	// String containing the TextBox's text.
	std::string textBoxScript;

	// Location and area of the TextBox.
	SDL_Rect area;

	// Location and area of the TextBox's name.
	SDL_Rect nameArea;

	// Location and area of the TextBox's text.
	SDL_Rect textArea;

	// Texture of the name to be displayed above this TextBox.
	SDL_Texture* nameTexture;

	// Texture of text to be displayed by this TextBox.
	SDL_Texture* textTexture;

	// Colour of any TextBox.
	static SDL_Colour textBoxColour;

	// Default constructor for a TextBox.
	TextBox();

	// Constructor for a TextBox.
	TextBox(int x, int y, int w, int h, int min, int max, std::string initText, std::string name);

	// Destuctor for a TextBox.
	~TextBox();

	// Manages text input.
	SDL_Event textInput();

	// Returns an integer form of the TextBox's text.
	int toInt();

	// Changes the max of this TextBox.
	void changeMax(int newMax);

	// Renders the TextBox's contents.
	void render(int x, int y);

	// Renders a TextBox's box and text.
	void renderBox(int x, int y);

};

// Default constructor for a TextBox.
TextBox::TextBox() {
	area = { 0, 0, 0, 0 };
	nameArea = { 0, 0, 0, 0 };
	textArea = { 0, 0, 0, 0 };
	nameTexture = NULL;
	textBoxScript = "";
	textTexture = nullptr;
	min = 0;
	max = 0;

}

/*
Standard constructor for a TextBox.
The inputed dimensions are for both the TextBox and its name. Thus, both are half of the inputed
height.

Note: height > 4, width > 12
*/
TextBox::TextBox(int x, int y, int w, int h, int min, int max, std::string initText, std::string name) {

	// Initializes the area of the TextBox, its name, and its text.
	area = { x, y + h / 2, w, h / 2 };
	nameArea = {x, y, w, h / 2};
	textArea = { x + 6, y + h / 2 + 2, 6, h / 2 - 2 };

	// Initializes min and max.
	this->min = min;
	this->max = max;

	// Initializes the TextBox's name.
	SDL_Surface* textBoxSurface = TTF_RenderText_Blended(defaultFont, name.c_str(), { 0, 0, 0, 0xFF });
	nameTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
	SDL_FreeSurface(textBoxSurface);

	// Initializes the default text.
	textBoxScript = initText;
	textArea.w = 6 * strlen(textBoxScript.c_str());
	textBoxSurface = TTF_RenderText_Blended(defaultFont, textBoxScript.c_str(), { 0, 0, 0, 0xFF });
	textTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
	SDL_FreeSurface(textBoxSurface);

}

/*
Deconstructor for a TextBox. Destroys all sprites.
*/
TextBox::~TextBox() {
	SDL_DestroyTexture(nameTexture);
	SDL_DestroyTexture(textTexture);

}

/*
Assesses text input for the TextBox. Does so by handling keyboard events.

Note: Text input is assessed differently based on whether or not the TextBox is coded to int input.
	  If it is (max != min), only positive integer input will be accepted.

*/
SDL_Event TextBox::textInput() {

	// Loops waiting for text input.
	SDL_Event event;
	while (true) {
		while (SDL_PollEvent(&event) != 0) {

			// Removes a character when backspace is pressed.
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_BACKSPACE && textBoxScript.length() > 0) {

					// Removes the last character of the string.
					textBoxScript.pop_back();

					// Adds a 0 if the textBox is empty.
					if (textBoxScript.length() == 0) {
						textBoxScript = "0";

					}

					// Assigns the length of the text.
					textArea.w = 6 * strlen(textBoxScript.c_str());

					// Renders the textTexture.
					SDL_DestroyTexture(textTexture);
					SDL_Surface* textBoxSurface = TTF_RenderText_Blended(defaultFont, textBoxScript.c_str(), { 0, 0, 0, 0xFF });
					textTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
					SDL_FreeSurface(textBoxSurface);

					// Returns the next event.
					SDL_PollEvent(&event);
					return event;

				}
			}
			// Stays in the loop when unimportant events occur.
			else if (event.type != SDL_TEXTINPUT && event.type != SDL_QUIT && event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_MOUSEBUTTONUP);
			// Handles typing.
			else if (event.type == SDL_TEXTINPUT) {

				// Accepts only input that can fit into the TextBox.
				if (textArea.w < area.w - 12) {

					// If the TextBox is coded for int input, accepts only int input that can fit into the TextBox.
					if (min != max && *(event.text.text) >= '0' && *(event.text.text) <= '9') {

						// Adds a character to the textBoxScript.
						textBoxScript += event.text.text;

						// If this TextBox's script is above max, assigns it to max.
						if (this->toInt() > max) {
							textBoxScript = std::to_string(max);

						}

						// Removes leading 0s.
						while (textBoxScript.length() > 1 && textBoxScript.front() == '0') {
							textBoxScript.erase(0, 1);

						}
					}
					// If the TextBox is not coded for int input, accepts any characters that can fit into the TextBox.
					else {

						// Adds a character to the textBoxScript.
						textBoxScript += event.text.text;

					}
				}

				// Assigns the length of the text.
				textArea.w = 6 * strlen(textBoxScript.c_str());

				// Renders the textTexture.
				SDL_DestroyTexture(textTexture);
				SDL_Surface* textBoxSurface = TTF_RenderText_Blended(defaultFont, textBoxScript.c_str(), { 0, 0, 0, 0xFF });
				textTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
				SDL_FreeSurface(textBoxSurface);

				// Returns the next event.
				SDL_PollEvent(&event);
				return event;

			}
			// Returns any events that do not match the accepted ones.
			else {

				// Ensures that textBoxScript does not go below min for numerical TextBoxes.
				// (going above max is handled earlier).
					if (max != min && this->toInt() < min) {
					textBoxScript = std::to_string(min);

					// Assigns the length of the text.
					textArea.w = 6 * strlen(textBoxScript.c_str());

					// Renders the textTexture.
					SDL_DestroyTexture(textTexture);
					SDL_Surface* textBoxSurface = TTF_RenderText_Blended(defaultFont, textBoxScript.c_str(), { 0, 0, 0, 0xFF });
					textTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
					SDL_FreeSurface(textBoxSurface);

				}

				// Returns the event.
				return event;

			}
		}
	}

}

/*
Converts the contents of the textBoxScript string to int if the TextBox is coded to store int data.
Returns 0 if no int is found.
*/
int TextBox::toInt() {
	if (min != max && !textBoxScript.empty()) {
		std::string::size_type strSize;
		std::stoi(textBoxScript, &strSize);

	}
	else return 0;

}

/*
Changes the max of this TextBox. Does so by modifying textBoxScript, textArea, and textTexture.
*/
void TextBox::changeMax(int newMax) {

	// Changes the maximum value.
	this->max = newMax;

	// If the current value of the text is greater than the new maximum or max == 0, reassigns
	// textBoxScript and rerenders the textTexture.
	if (toInt() > newMax || !newMax) {

		// Reassign textBoxScript to max.
		textBoxScript = std::to_string(newMax);

		// Assigns the length of the text.
		textArea.w = 6 * strlen(textBoxScript.c_str());

		// Renders the textTexture.
		SDL_DestroyTexture(textTexture);
		SDL_Surface* textBoxSurface = TTF_RenderText_Blended(defaultFont, textBoxScript.c_str(), { 0, 0, 0, 0xFF });
		textTexture = SDL_CreateTextureFromSurface(renderer, textBoxSurface);
		SDL_FreeSurface(textBoxSurface);

	}
}

/*
Renders a textbox. x and y are offsets.

Note: Text is always black and TextBoxes are always white.
*/
void TextBox::render(int x, int y) {

	// Renders the TextBox's name.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_Rect tempTextBoxNameArea = { nameArea.x + x, nameArea.y + y, nameArea.w, nameArea.h };
	SDL_RenderCopy(renderer, nameTexture, NULL, &tempTextBoxNameArea);

	// Renders a TextBox's box and text.
	renderBox(x, y);


}

/*
Renders a TextBox's box and text.

Note: Text is always black and TextBoxes are always white.
*/
void TextBox::renderBox(int x, int y) {

	// Renders the TextBox.
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect tempTextBoxArea = { area.x + x, area.y + y, area.w, area.h };
	SDL_RenderFillRect(renderer, &tempTextBoxArea);

	// Renders the TextBox's text.
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect tempTextBoxTextArea = { textArea.x + x, textArea.y + y, textArea.w, textArea.h };
	SDL_RenderCopy(renderer, textTexture, NULL, &tempTextBoxTextArea);

}

// Sets the colour of TextBoxes to white.
SDL_Colour TextBox::textBoxColour = { 0xFF, 0xFF, 0xFF, 0xFF };

/*
Object representing a Panel that contains TextBoxes and Buttons. Generally, it is expected that
the contents of the TextBoxes will be converted to other data at some point, rather than that
being done within the Panel.

Note: The locations of internal TextBoxes, Buttons, and the name are relative to the location of
	  the Panel.

*/
class Panel {
public:
	// Code of this panel. Expected to store an enum representing this panel's type.
	int code;

	// Number of TextBoxes stored in this Panel.
	int numBoxes;

	// Number of CodeButtons stored in this Panel.
	int numButtons;

	// Number of sprites stored in this Panel.
	int numSprites;

	// Area and location of the galaxy's panel.
	SDL_Rect area;

	// Array of TextBoxes for inputing various attributes of this Panel.
	TextBox** textBoxes;

	// Array of CodeButtons for performing various actions pertinent to this Panel.
	CodeButton** buttons;

	// Array of sprites displaying various data pertinent to this panel.
	Sprite** sprites;

	// Unused default constructor for Panel.
	Panel();

	// Constructor for a Panel.
	Panel(int code, int numBoxes, int numButtons, int numSprites, int x, int y, int w, int h);

	// Deconstrctor for a Panel.
	~Panel();

	// Adds a new TextBox to a Panel.
	// TODO should be removed, as freely adding TextBoxes is superior.
	void addTextBox(int index, int min, int max, std::string name);

	// Adds a new TextBox to the Panel with an inputed initial value.
	// TODO should be removed, as freely adding TextBoxes is superior.
	void addTextBox(int index, int min, int max, std::string init, std::string name);

	// Adds an existing TextBox to the Panel.
	void addTextBox(int index, TextBox* toAdd);

	// Adds a new CodeButton to the Panel.
	void addButton(int index, CodeButton* toAdd);

	// Adds a new sprite to the Panel.
	void addSprite(int index, Sprite* toAdd);

	// Renders the GalaxyPanel.
	void render(int x, int y);

	// Returns any clicked TextBox.
	TextBox* getTextBox(int x, int y, int xPos, int yPos);

	// Returns any clicked CodeButton.
	CodeButton* getCodeButton(int x, int y, int xPos, int yPos);

	// Returns the ButtonCode of any clicked CodeButton.
	ButtonCode getButtonCode(int x, int y, int xPos, int yPos);

};

// Unused default constructor for Panel.
Panel::Panel() {
	textBoxes = nullptr;
	area = {0, 0, 0, 0};
	code = -1;
	numBoxes = 0;

};

/*
Constructor for a Panel. Initializes code and area, numBoxes, numButtons, and numSprites.
*/
Panel::Panel(int code, int numButtons, int numBoxes, int numSprites, int x, int y, int w, int h) {
	this->code = code;
	this->area = { x, y, w, h };
	this->numBoxes = numBoxes;
	this->numButtons = numButtons;
	this->numSprites = numSprites;
	textBoxes = new TextBox* [numBoxes];
	buttons = new CodeButton* [numButtons];
	sprites = new Sprite* [numSprites];

}

/*
Deconstructor for a Panel. Deletes all TextBoxes and CodeButtons, and deletes the nameTexture.
*/
Panel::~Panel() {

	// Deletes each TextBox and the textBoxes array.
	for (int i = 0; i < numBoxes; ++i) delete textBoxes[i];
	delete[] textBoxes;

	// Deletes each CodeButton and the buttons array.
	for (int i = 0; i < numButtons; ++i) delete buttons[i];
	delete[] buttons;

	// Deletes each Sprite in the Panel.
	for (int i = 0; i < numSprites; ++i) delete sprites[i];
	delete[] sprites;

}

/*
Adds a TextBox to a Panel in the specified index. Spaces TextBoxes automatically.
It is assumed that TextBoxes will not be assigned in occupied indices.

NumTextBoxes will be initialized to contain "0", while other TextBoxes will be initialized to
contain "".

Note: min != max signifies a NumTextBox.
*/
void Panel::addTextBox(int index, int min, int max, std::string name) {

	// Assigns a new TextBox with the inputed parameters to the inputed index when given a TextBox.
	if (max == min)
		textBoxes[index] = new TextBox(2 + index * (area.w / 4), area.h / 2, area.w / 4 - 4, area.h / 2 - 8, min, max, "", name);
	// Assigns a new TextBox with the inputed parameters to the inputed index when given a NumTextBox.
	else
		textBoxes[index] = new TextBox(2 + index * (area.w / 4), area.h / 2, area.w / 4 - 4, area.h / 2 - 8, min, max, "0", name);

}

/*
Adds a TextBox to a Panel in the specified index. Spaces TextBoxes automatically.
It is assumed that TextBoxes will not be assigned in occupied indices.
*/
void Panel::addTextBox(int index, int min, int max, std::string init, std::string name) {

	// Assigns a new TextBox with the inputed parameters to the inputed index.
	textBoxes[index] = new TextBox(2 + index * (area.w / 4), area.h / 2, area.w / 4 - 4, area.h / 2 - 8, min, max, init, name);

}

/*
Adds a TextBox that has already been initialized.
It is assumed that TextBoxes will not be assigned in occupied indices.
*/
void Panel::addTextBox(int index, TextBox* toAdd) {
	textBoxes[index] = toAdd;

}

/*
Adds a CodeButton that has already been initialized.
It is assumed that CodeButtons will not be assigned in occupied indices.
*/
void Panel::addButton(int index, CodeButton* toAdd) {
	buttons[index] = toAdd;

}

/*
Adds a new sprite to the Panel.
It is assumed that Sprites will not be assigned in occupied indices.
*/
void Panel::addSprite(int index, Sprite* toAdd) {
	sprites[index] = toAdd;

}

/*
Renders a panel. First renders the panel's area, then renders its textBoxes.
*/
void Panel::render(int x, int y) {

	// Renders the Panel's area.
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_Rect panelArea = {area.x + x, area.y + y, area.w, area.h};
	SDL_RenderFillRect(renderer, &panelArea);

	// Renders all TextBoxes.
	for (int i = 0; i < numBoxes; ++i) textBoxes[i]->render(panelArea.x, panelArea.y);

	// Renders all CodeButtons.
	for (int i = 0; i < numButtons; ++i) buttons[i]->render(panelArea.x, panelArea.y);

	// Renders all sprites.
	for (int i = 0; i < numSprites; ++i) sprites[i]->render(panelArea.x, panelArea.y);

}

/*
Returns any clicked TextBox, or nullptr if none were clicked.
*/
TextBox* Panel::getTextBox(int x, int y, int xPos, int yPos) {

	// Parses through the TextBoxes and looks for one within bounds.
	for (int i = 0; i < numBoxes; ++i) {

		// Returns any TextBox within bounds of the mouse.
		if (withinBounds(xPos - area.x - x, yPos - area.y - y, textBoxes[i]->area)) return textBoxes[i];

	}

	// Returns nullptr if no TextBox was found.
	return nullptr;

}

/*
Returns any clicked CodeButton, or nullptr if none were clicked.
*/
CodeButton* Panel::getCodeButton(int x, int y, int xPos, int yPos) {

	// Parses through the CodeButtons and looks for one within bounds.
	for (int i = 0; i < numButtons; ++i) {

		// Returns the first CodeButton within bounds.
		if (withinBounds(xPos - area.x - x, yPos - area.y - y, buttons[i]->area)) return buttons[i];

	}

	// If no CodeButtons were within bounds, returns nullptr.
	return NULL;

}

/*
Returns the ButtonCode of any clicked button, or NULL if none were clicked.
*/
ButtonCode Panel::getButtonCode(int x, int y, int xPos, int yPos) {

	// Parses through the CodeButtons and looks for one within bounds.
	for (int i = 0; i < numButtons; ++i) {

		// Returns the ButtonCode of any CodeButton within bounds.
		if (withinBounds(xPos - area.x - x, yPos - area.y - y, buttons[i]->area)) return buttons[i]->code;

	}

	// If no CodeButtons were within bounds, returns 0.
	return NULL_CODE;

}

/*
Class representing a generic ScrollBar. Contains a double representing the area covered by the grab
bar and an int representing the distance from the top.

Note: this class is the parent class to all other ScrollBars, and is not used itself.
	  When it is inherited from, it overrides the virtual void method moveBar.
*/
class ScrollBar {
public:
	// Abstract amount of space that the grab bar occupies.
	int barSize;

	// Abstract amount of space that the grab bar can occupy.
	int barSpace;

	// Grab bar's distance from the top of the ScrollBar.
	int distance;

	// Area occupied by the ScrollBar.
	SDL_Rect area;

	// Unused default constructor for ScrollBar.
	ScrollBar();

	// Constructor for the ScrollBar. Assigns area, barSize, barSpace
	ScrollBar(int x, int y, int w, int h, int barSize, int barSpace);

	// Renders the ScrollBar.
	void render(int x, int y);

	// Moves the grab bar while the user holds down left click.
	// Note: may be overloaded in children to allow for additional parameters for performMovement.
	// Note: should be nested in a loop that contains performMovement, a renderer, and a return.
	void moveBar();

	// Performs some action dependent upon the location of the bar. Overloaded to mesh with
	// its assigned task. It is assumed that the movement will be a function of distance.
	//void performMovement();

};

// Unused default constructor for ScrollBar.
ScrollBar::ScrollBar() {
	area = {0, 0, 0, 0};
	barSize = 0;
	barSpace = 0;
	distance = 0;

}

/*
Constructor for the ScrollBar. Assigns area, barSize, and barSpace. Initializes distance to 0.
*/
ScrollBar::ScrollBar(int x, int y, int w, int h, int barSize, int barSpace) {
	area = {x, y, w, h};
	distance = 0;
	ScrollBar::barSize = barSize;
	ScrollBar::barSpace = barSpace;

}

/*
Renders the ScrollBar. Renders the bar and the grab bar.
*/
void ScrollBar::render(int x, int y) {
	double barFrac = (barSpace == 0 ? 1 : (double)barSize / barSpace);

	// Renders the ScrollBar.
	SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
	SDL_Rect tempArea = {x + area.x, y + area.y, area.w, area.h};
	SDL_RenderFillRect(renderer, &tempArea);

	// Renders the grab bar.
	SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
	SDL_Rect tempGrabArea = { x + area.x, (int)(y + area.y + distance * barFrac), area.w, (int)(barFrac * area.h) };
	SDL_RenderFillRect(renderer, &tempGrabArea);

	// Renders the ScrollBar to the screen.
	SDL_RenderPresent(renderer);

}

/*
Moves the grab bar while the user holds down left click. Does so by finding distance as a function
of the mouse's yPos vs the bar's area.

*/
void ScrollBar::moveBar() {
	int xPos, yPos;

	// Gets the mouse position.
	SDL_GetMouseState(&xPos, &yPos);

	// Converts the y position relative to the ScrollBar's size to a distance.
	distance = barSpace * ((double)(yPos - area.y) / area.h) - barSize / 2;

	// Sets distance to 0 if it is below 0 (cursor is above the bar).
	if (distance < 0) {
		distance = 0;

	}
	// Sets distance to its maximum if it is above it.
	else if (distance > barSpace - barSize) {
		distance = barSpace - barSize;

	}
}