#pragma once

////////////////////////////////////////////////////////////////////
// TODO this has not been added to Government or used in any way. //
////////////////////////////////////////////////////////////////////

// Informs the compiler about Governments.
extern class Government;

/*
Stores a relationship one Government and another.

Relationships should always be bidirectional, i.e. if nation A has a relationship
with nation B, nation B should have one with nation A.

16 bytes.
*/
struct Relationship {

	// The Government that this Government has a relationship with.
	Government* target; // 8 bytes.

	// The favourability of this Government towards the target Government.
	// Note: Needs at least 11 bits to store -1000 to 1000.
	uint_least16_t favourability : 11; // 2 bytes.

	// Whether or not this Government is at war with the target Government.
	uint_least16_t war : 1;

	// 4 spare bits.

	// 6 bytes padding.

};