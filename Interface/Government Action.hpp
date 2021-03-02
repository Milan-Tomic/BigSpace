#pragma once

/*
Performs the government actions for all governments.
*/
void governmentBehaviours() {
	int page = requestGovernmentPage();

	// Returns if there is no available page.
	if (page < 0) return;

	// Performs each behaviour for each government.
	for (int i = 0; i < governmentPages[page]->arrCurrGovernment; ++i) {
		for (int index = 0; index < NUM_GOVERNMENT_BEHAVIOURS && governmentPages[page]->governments[i].behaviours[index]; ++index)
			governmentPages[page]->governments[i].behaviours[index](&governmentPages[page]->governments[i]);

	}

	// Continues until there are no remaining GovernmentPages.
	governmentBehaviours();

}

/*
Manages government action for all active governments.
*/
void governmentAction() {
	governmentBehaviours();

}