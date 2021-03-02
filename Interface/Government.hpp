#ifndef GOVERNMENT_H
#define GOVERNMENT_H

// Number of behaviours for a given government.
#define NUM_GOVERNMENT_BEHAVIOURS 10

// Notifies the Government about some items.
extern struct GroundUnitTemplate;
extern struct ShipTemplate;

/*
Class representing a government.

NOTES:

Laws:
Each child's laws should have a bool representing whether the government is obligated to take from its parent,
and also each parent's laws should have a bool representing whether its children are obligated to take from it.
It will be difficult to model vassals, special administrative regions, protectorates, puppets, etc.
	Perhaps the senate should send ultimatums based on sub government loyalty to integrate subjects, erode indep.
		Perhaps ultimatums will be sent more with higher centralisation, other ethics.

Governments:


Colonies:
There is a certain number of colonies under any given Government's control. The government should manage these colonies
based on its laws. This should be done through usage of prerogatives based on ethics and laws.
Can be modeled using switches or function pointers.

Note: Government is actually defined in Governments.hpp
*/

/*
Measured size is XXX bytes.
*/
class Government {
public:

	// Behaviour functions for this government. Dictates what the government may do.
	// May be further changed using laws, etc.
	// TODO change num behaviours.
	void (*behaviours[NUM_GOVERNMENT_BEHAVIOURS])(Government*);

	// Parent of this government. nullptr when fully independent.
	Government* parent;

	// TODO child governments?

	// Colonies directly controlled by this government.
	Colony** colonies;

	// List of closures associated with this government.
	uint_least16_t* closures;

	// Table of units used by this Government. This may point to the parent Government's
	// unit table.
	// *((int*)government->unitTable) contains the number of units (hence 0 is always occupied).
	// The unitTable will have numUnits + 1 entries.
	GroundUnitTemplate* unitTable;

	// Table of ships used by this Government.
	ShipTemplate* shipTable;

	// Expansion targets for this Government.
	Coordinate spaceTargets[NUM_TARGETS];

	// TODO laws

	// Colour of this government's flag.
	SDL_Color flagColour;

	// Number of colonies directly held by this Government.
	uint_least16_t numColonies;

	// Number of closures belonging to this Government.
	uint_least16_t numClosures;

	// Index within spaceTargets.
	uint_least8_t spaceTarget;

	// TODO government type?

	// Unused default Constructor for a Government.
	// Initializes everything to null.
	Government();

	// Deconstructor for a Government. Deallocates all arrays and clears memory.
	~Government();

	// Adds a Colony to this Government.
	void addColony(Colony* colony);

	// Removes a Colony from this Government.
	void removeColony(Colony* colony);

	// Adds a closure to this Government.
	void addClosure(uint_least16_t closure);

	// Removes a closure from this Government.
	void removeClosure(uint_least16_t closure);

	// Fills the spaceTargets array.
	void findSpace();

	// Requests a single land tile from the spaceTargets array.
	Coordinate requestSpace();

};

#endif // GOVERNMENT_H
