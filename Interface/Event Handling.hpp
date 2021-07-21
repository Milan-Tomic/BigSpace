#pragma once

/*
Handles C_NULL_EVENT. Does nothing.
*/
void cNullEvent(Event& event, Colony* colony) {

}

/*
TODO REMOVE
*/
void cThreatEvent(Event& event, Colony* colony) {

}

// Jump table for event handling.
void (*colonyEventHandler[])(Event& event, Colony* colony) = {
	cNullEvent,
	cThreatEvent

};

/*
Begins tracking a threat to this Colony.
*/
inline void Colony::addThreat(Threat threat) {
	threats.push_back(threat);

}

/*
Begins tracking a Squadron that threatens this Colony.

TODO calculate time.
*/
inline void Colony::addThreat(Squadron* squadron) {
	Threat threat;

	// Adds the Threat if it is not redundant.
	if (std::find_if(threats.begin(), threats.end(),
		[squadron](Threat threat) { return threat.squadron == squadron; }) == threats.end()) {
		threat = { squadron, {0, 0}, 1, squadron->strength };
		addThreat(threat);

	}
}

/*
Stops tracking a Threat to this Colony.
*/
inline void Colony::removeThreat(Threat threat) {
	removeThreat(threat.squadron);

}

/*
Stops tracking a Squadron that threatens this Colony.
*/
inline void Colony::removeThreat(Squadron* squadron) {
	if (!threats.size()) return;
	threats.erase(std::remove_if(threats.begin(), threats.end(),
		[squadron](const Threat& threat) { return threat.squadron == squadron; }),
		threats.end());

}

/*
Handles tracking all Threats to this Colony.
*/
inline void Colony::handleThreats() {
	if (!threats.size()) return;

	// Tracks the threats.
	for (Threat& threat : threats) trackThreat(threat);

	// Removes threats from tracking if they are finished.
	threats.erase(std::remove_if(threats.begin(), threats.end(),
		[](const Threat& threat) { return !threat.squadron || !threat.squadron->strength; }),
		threats.end());

}

/*
Begins tracking a Threat to this Squadron.
*/
inline void Squadron::addThreat(Threat threat) {
	threats.push_back(threat);

}

/*
Begins tracking a Squadron that threatens this Squadron.

TODO calculate time.
*/
inline void Squadron::addThreat(Squadron* squadron) {
	Threat threat;

	// Adds the Threat if it is not redundant.
	if (std::find_if(threats.begin(), threats.end(),
		[squadron](Threat threat) { return threat.squadron == squadron; }) == threats.end()) {
		threat = { squadron, {0, 0}, 1, squadron->strength };
		addThreat(threat);

	}
}

/*
Stops tracking a Threat to this Squadron.
*/
inline void Squadron::removeThreat(Threat threat) {
	removeThreat(threat.squadron);

}

/*
Stops tracking a Squadron that threatens this Squadron.
*/
inline void Squadron::removeThreat(Squadron* squadron) {
	if (!threats.size()) return;
	threats.erase(std::remove_if(threats.begin(), threats.end(),
		[squadron](const Threat& threat) { return threat.squadron == squadron; }),
		threats.end());

}

/*
Handles tracking all Threats to this Squadron.
*/
inline void Squadron::handleThreats() {
	if (!threats.size()) return;

	// Tracks the threats.
	for (Threat& threat : threats) trackThreat(threat);

	// Removes threats from tracking if they are finished.
	threats.erase(std::remove_if(threats.begin(), threats.end(),
		[](const Threat& threat) { return !threat.squadron || !threat.squadron->strength; }),
		threats.end());

}

/*
Handles threat tracking.

This event indicates to an entity that there is a nearby enemy Squadron.
When an enemy Squadron is nearby, time will count down until the Squadron is
revealed to the entity.
When the enemy Squadron leaves, time will count up until it is assumed that
the Squadron has left, at which point the Threat will be removed.

TODO enemy flight speed, nation flight speed, and communication should be
	factored into replacing tempTime/tempRange
*/
void Squadron::trackThreat(Threat& threat) {
	int distance = calcTaxiDistance(loc.x, loc.y,
		threat.squadron->loc.x, threat.squadron->loc.y);

	// TODO tempTime is temporarily the time taken to discover the squadron.
	const int tempTime = 5;

	// TODO tempRange is temporarily the maximum tracking range of the threatened entity.
	const int tempRange = 8;

	// Reveals the Squadron if it is within scanning distance.
	if (scan(threat.squadron->loc, distance)) {
		threat.time = 0;
		threat.coord = threat.squadron->loc;
		threat.strength = threat.squadron->strength;

	}
	// Counts down time if the Squadron's current location has not yet been revealed.
	else if (threat.time) {
		--threat.time;

		// Reveals the threat if tracking time is finished.
		if (!threat.time) {

			// Reveals the location and strength of the threat.
			threat.coord = threat.squadron->loc;
			threat.strength = threat.squadron->strength;

			// Marks the threat for removal from tracking if it has moved
			// outside of tracking range.
			if (distance > tempRange) threat.squadron = nullptr;

		}
	}
	// Increases time if the Squadron has been discovered and has since moved.
	else if (threat.coord.x != threat.squadron->loc.x || threat.coord.y != threat.squadron->loc.y) threat.time = tempTime;

}

/*
Handles threat tracking for a Colony. This is identical to threat handling for
a Squadron.

TODO enemy flight speed, nation flight speed, and communication should be
	factored into replacing tempTime/tempRange
*/
void Colony::trackThreat(Threat& threat) {
	int distance = calcTaxiDistance(loc().x, loc().y,
		threat.squadron->loc.x, threat.squadron->loc.y);

	// TODO tempTime is temporarily the time taken to discover the squadron.
	const int tempTime = 5;

	// TODO tempRange is temporarily the maximum tracking range of the threatened entity.
	const int tempRange = 8;

	// Reveals the Squadron if it is within scanning distance.
	if (scan(threat.squadron->loc, distance)) {
		threat.time = 0;
		threat.coord = threat.squadron->loc;
		threat.strength = threat.squadron->strength;

	}
	// Counts down time if the Squadron's current location has not yet been revealed.
	else if (threat.time) {
		--threat.time;

		// Reveals the threat if tracking time is finished.
		if (!threat.time) {

			// Reveals the location and strength of the threat.
			threat.coord = threat.squadron->loc;
			threat.strength = threat.squadron->strength;

			// Marks the threat for removal from tracking if it has moved
			// outside of tracking range.
			if (distance > tempRange) threat.squadron = nullptr;

		}
	}
	// Increases time if the Squadron has been discovered and has since moved.
	else if (threat.coord.x != threat.squadron->loc.x || threat.coord.y != threat.squadron->loc.y) threat.time = tempTime;

}