#pragma once

// Describes a certain good in a market.
struct Good {
	uint_least32_t quantity;
	uint_least32_t desired;

};

// Describes a certain race's food in a market.
// race == nullptr connotes end of foods list.
struct Food {
	RaceTemplate* race;
	uint_least32_t vegQuantity;
	uint_least32_t vegDesired;
	uint_least32_t meatQuantity;
	uint_least32_t meatDesired;

};

// Tells the compiler about Colonies.
extern class Colony;

/*
Market storing goods. Represents middle-men and wholesalers.
*/
class Market {
public:

	// Goods in this market.
	Good goods[NUM_GOODS];

	// Stores the foods for this market.
	// Should be dynamically allocated.
	// TODO maybe store in RaceInstance.
	Food* foods;

	// Market capital for this market. To be used when multiple Colonies share markets.
	// TODO when cutoff from market capital, lose market etc.
	// TODO marketCapital is never used. Check all references.
	Colony* marketCapital;

	// TODO: Stores the currencies available in this market.
	// TODO: Stores the securities (stocks and bonds) avaialbe in this market.

	// Number of foods in this market.
	uint_least32_t numFoods;

	// Number of Colonies that connect to this Market.
	uint_least32_t numColonies; // Could have reduced size. Currently contemplating universal Market advancement.

	// Deconstructor for the Market.
	~Market();

	// Adds a RaceTemplate to the Market's Foods.
	// Do not use this.
	inline void addMarketFood(RaceTemplate* race);

	// Attempts to add a RaceTemplate to the Market's Foods.
	// Use this for adding foods.
	inline void tryAddMarketFood(RaceTemplate* race);

	// Adds a Colony to this Market (and vice versa).
	// Found in Colonies.
	void addColony(Colony* colony);

	// Removes a Colony from this Market (and vice versa).
	// May delete the Market or change its capital.
	// Found in Colonies.
	void removeColony(Colony* colony);

};

/*
Inventory of goods. Similar to a Market, except it belongs to a particular owner.]
TODO use
*/
struct Inventory {

	// Owner of this inventory. TODO likely not necessary.
	void* owner;

};

// Creates markets for each planet in the universe.
void initMarkets(int numMarkets);

// Returns the next available market.
Market* placeMarket();

// Number of elements in a MarketPage. Should be around 1MB.
#define MARKET_PAGE_SIZE ((1048576 - 8) / sizeof(Market))

// Default number of Market pages. This is also the number used when expanding the roster of
// Market pages.
#define NUM_MARKET_PAGES 256

// Page of Markets.
struct MarketPage {
	Market markets[MARKET_PAGE_SIZE];
	uint16_t arrCurrMarket;
	uint16_t numOpen;
	bool active;

	// 3 bytes of padding?.

};

// Contains all MarketPages. Minimizes memory use from allocations.
int numMarketPages;
MarketPage** marketPages;

/*
Creates a paging table of Markets. Initialized with capacity for numMarkets.
It is expected that this function will be called exactly once per universe and in a single-threaded manner.
*/
void initMarkets(int numMarkets) {

	// Frees marketPages for reuse.
	if (numMarketPages) {
		for (int i = 0; i < numMarketPages; ++i) {
			if (marketPages[i]) delete marketPages[i];
			marketPages[i] = nullptr;

		}
		delete[] marketPages;

	}

	// Initializes marketPages.
	numMarketPages = numMarkets / MARKET_PAGE_SIZE + 1;
	marketPages = (MarketPage**)calloc(numMarketPages + NUM_MARKET_PAGES - (numMarketPages % NUM_MARKET_PAGES), sizeof(MarketPage*));
	for (int i = 0; i < numMarketPages; ++i) marketPages[i] = (MarketPage*)calloc(1, sizeof(MarketPage));

}

/*
Places a Market. Generates a new MarketPage if there are none available.
Resizes marketPages in increments of 256 when the object pool is insufficiently large.
*/
Market* placeMarket() {
	static std::shared_mutex placeMarketMutex;

	// Forbids concurrent access to this function.
	const std::lock_guard<std::shared_mutex> lock(placeMarketMutex);

	// Attempts to place the Market within one of the current pages.
	Market* market = nullptr;
	for (int page = 0; page < numMarketPages; ++page) {
		if (marketPages[page]->arrCurrMarket < MARKET_PAGE_SIZE) {
			market = &marketPages[page]->markets[marketPages[page]->arrCurrMarket];
			++marketPages[page]->arrCurrMarket;
			break;

		}
	}

	// If placement failed, creates a new MarketPage and places the Market there.
	if (market == nullptr) {

		// If the page table is not large enough, reallocates it.
		if (!(numMarketPages + 1 % NUM_MARKET_PAGES))
			marketPages = (MarketPage**)realloc(marketPages, (numMarketPages + NUM_MARKET_PAGES) * sizeof(MarketPage*));

		// Creates a new MarketPage.
		marketPages[numMarketPages] = (MarketPage*)calloc(1, sizeof(MarketPage));

		// Places the Market in the first index of the new page.
		market = &(marketPages[numMarketPages]->markets[0]);
		++marketPages[numMarketPages]->arrCurrMarket;

		// Moves on to the next page.
		++numMarketPages;

	}

	// Returns a pointer to the Market.
	return market;

}

/*
Finds the page of a given Market within the paging table.
*/
inline void findMarketPage(int& page, Market* market) {

	// Finds the page.
	for (page = 0; page < numMarketPages; ++page) {

		// Returns the value if the Market is within bounds.
		if ((char*)market >= (char*)marketPages[page] &&
			(char*)market < (char*)marketPages[page] + sizeof(MarketPage)) {
			return;

		}
	}
}

/*
Finds the page and index of a given Market within the paging table.
*/
inline void findMarketIndex(int& page, int& index, Market* market) {

	// Finds the page.
	findMarketPage(page, market);
	index = ((char*)market - (char*)marketPages[page]) / sizeof(Market);

}

/*
Marks a Market as being available for reuse.
*/
inline void freeMarket(Market* market) {
	int page;

	// Marks that there is a free Market in the page.
	findMarketPage(page, market);
	++marketPages[page]->numOpen;

	// Clears the Market.
	market->~Market();

}

/*
Adds a RaceTemplate to the Market's Foods.
*/
inline void Market::addMarketFood(RaceTemplate* race) {

	// If there are no Foods, initailizes foods.
	if (!numFoods) {
		++numFoods;
		foods = new Food[1];

	}
	// If foods exists, adds a spot for the new Food.
	else {
		++numFoods;
		foods = (Food*)realloc(foods, numFoods * sizeof(Food));

	}
	
	// Assigns the new Food to the inputed Race.
	foods[numFoods - 1].race = race;

}

/*
Attempts to add a RaceTemplate to the Market's Foods.
*/
inline void Market::tryAddMarketFood(RaceTemplate* race) {

	// Returns if the inputed race is found within the Market.
	for (int r = 0; r < numFoods; ++r) if (foods[r].race == race) return;

	// Adds the food to the Market if the inputed race is not found within the Market.
	addMarketFood(race);

}

/*
Deconstructor for the Market. Deletes foods and clears the Market.
*/
Market::~Market() {
	delete foods;
	memset(this, 0, sizeof(this));

}