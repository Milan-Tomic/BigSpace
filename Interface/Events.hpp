/*
This header contains the Event struct, the Event enums, and references to the Event jump tables.
*/

/*
This struct represents an Event that relates to Squadron movement.

Note: time should be at least 1 when representing C_THREAT_EVENT
*/
struct SquadronEvent {
	uint_least16_t id;
	uint_least16_t time;
	CoordU coord;
	Squadron* squadron;

};

/*
This union stores all possible types of events in the same place.
*/
union EventType {
	SquadronEvent squadron;

};

/*
This struct represents an event. Events can be applied to colonies or Governments,
though they will be read and acted upon differently.

Events are generic and their primary distinction is in how they are read.
*/
union Event {
	uint_least16_t id;
	EventType type;

};

// Enum representing Colony Events.
enum ColonyEvents {
	C_NULL_EVENT,
	C_THREAT_EVENT

};

// Enum representing Government Events.
enum GovernmentEvents {
	G_NULL_EVENT
};

// Lets the compiler know about the Colony Event handler.
extern void (*colonyEventHandler[])(Event& event, Colony* colony);

/*
Contains a threat for tracking.

14 bytes, 2 bytes padding.
sizeof is 16 bytes.
*/
struct Threat {

	// Squadron being tracked.
	Squadron* squadron; // 8 bytes.

	// Last known location of the Squadron.
	CoordU coord; // 4 bytes.

	// Time until Squadron location revealed.
	uint_least16_t time; // 2 bytes.

	// Estimated strength of the Threat.
	uint_least16_t strength; // 2 bytes.

};

/*
Not operator for a Threat. Returns true if the threat is non-threatening or
untraced. Used for Threat tracking.
*/
bool operator!(const Threat& threat) {
	return !threat.squadron || !threat.strength || !threat.coord;

}