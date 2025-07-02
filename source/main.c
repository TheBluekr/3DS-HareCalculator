#include <3ds.h>
#include <stdio.h>

#define TURNCOUNT 0
#define HARECOUNT 1
#define TOKENCOUNT 2
#define NEWHARECOUNT 3
#define TOKENMULTIPLIER 4
#define TOKENMANATAP 5
#define UNTAPPEDTOKENEXISTING 6
#define UNTAPPEDTOKENSICKNESS 7
#define TAPPEDTOKENS 8

#define MAXTURNS 1024

int rowMin = TURNCOUNT;
int rowMax = TAPPEDTOKENS;

typedef struct {
	int index;
	int startHareCount;
	int totalTokenCount;
	int summonHareCount;
	int additionalTokenMultiplier;
	bool tokenManaTap;
	int tokenCountExisting;
	int tokenCountSickness;
	int tappedTokens;
} TurnState;

/*struct TurnList {
	int index;
	struct TurnState turnState;
	struct TurnList* previous;
	struct TurnList* next;
}*/

/*typedef struct TurnList {
	int turnCount;
	TurnState *TurnState;
} TurnList;*/

TurnState turnList[MAXTURNS];

void calculateTokens(int* currentTokens, int* currentHares, int* newHares, int* tokenMultiplier) {
	if(*newHares == 0)
		return;
	
	// Increment by the amount of existing hares
	*currentTokens += *currentHares * *tokenMultiplier;


	// And start the recalc
	*currentHares += 1;
	*newHares -= 1;
	
	return calculateTokens(currentTokens, currentHares, newHares, tokenMultiplier);
}

/*struct TurnList *expandTurnList(int index, struct TurnState turnState) {
	struct TurnList* turnListNew = (struct TurnList*) malloc(sizeof(struct TurnList));
	turnListNew.index = index;
	turnListNew.turnState = *turnState;
	turnListNew.next = NULL;

	return turnListNew;
}*/

int main(int argc, char **argv) {
	/*char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};*/

	// Initialize services
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_TOP, NULL);

	// Row pointer
	int rowPointer = 0;
	
	// Keep track of current turn
	int turnCount = 0;
	int maxTurnCount = 0;

	TurnState turnState;
	turnState.index = 0;
	turnState.startHareCount = 0;
	turnState.totalTokenCount = 0;
	turnState.summonHareCount = 0;
	turnState.additionalTokenMultiplier = 1;
	turnState.tokenManaTap = false;
	turnState.tokenCountExisting = 0;
	turnState.tokenCountSickness = 0;
	turnState.tappedTokens = 0;

	/*struct TurnList* turnListCurrent = (struct TurnList*)malloc(sizeof(struct TurnList));
	turnListCurrent.index = turnCount;
	turnListCurrent.turnState = *turnState;
	turnListCurrent.previous = NULL;
	turnListCurrent.next = NULL;*/

	turnList[turnCount] = turnState;

	//To move the cursor you have to print "\x1b[r;cH", where r and c are respectively
	//the row and column where you want your cursor to move
	//The top screen has 30 rows and 50 columns
	//The bottom screen has 30 rows and 40 columns
	printf("\x1b[1;13H\x1b[47;30mHare Apparent calculator\x1b[0m");

	//Print a REALLY crappy poeam with colored text
	//\x1b[cm set a SGR (Select Graphic Rendition) parameter, where c is the parameter that you want to set
	//Please refer to http://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes to see all the possible SGR parameters
	//As of now libctru support only these parameters:
	//Reset (0), Half bright colors (2), Reverse (7), Text color (30-37) and Background color (40-47)

	printf("\x1b[27;16HPress X to delete turn(s)");
	printf("\x1b[28;16HPress A to calculate");
	printf("\x1b[29;16HPress Select to reset");
	printf("\x1b[30;16HPress Start to exit");
	// Main loop
	while(aptMainLoop()) {
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if(kDown & KEY_START) break; // break in order to return to hbmenu
		
		// Row selection
		if(kDown & KEY_DUP && rowPointer > rowMin)
			rowPointer -= 1;
		if(kDown & KEY_DDOWN && rowPointer < rowMax)
			rowPointer += 1;

		// Incrementing counts
		if(kDown & KEY_DLEFT) {
			switch(rowPointer) {
				case TURNCOUNT: // Current turn count
					turnCount = turnCount > 0 ? turnCount-1 : 0;
					break;
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount = turnList[turnCount].startHareCount > 0 ? turnList[turnCount].startHareCount-1 : 0;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount = turnList[turnCount].totalTokenCount > 0 ? turnList[turnCount].totalTokenCount-1 : 0;
					break;
				case NEWHARECOUNT: // Newly summoned hare count
					turnList[turnCount].summonHareCount = turnList[turnCount].summonHareCount > 0 ? turnList[turnCount].summonHareCount-1 : 0;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier = turnList[turnCount].additionalTokenMultiplier > 1 ? turnList[turnCount].additionalTokenMultiplier-1 : 1;
					break;
				case TOKENMANATAP: // Tap tokens for hare cost
					turnList[turnCount].tokenManaTap = false;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					if(turnList[turnCount].tokenCountExisting > 0) {
						turnList[turnCount].tokenCountExisting -= 1;
						turnList[turnCount].totalTokenCount -= 1;
					}
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					if(turnList[turnCount].tokenCountSickness > 0) {
						turnList[turnCount].tokenCountSickness -= 1;
						turnList[turnCount].totalTokenCount -= 1;
					}
					break;
				case TAPPEDTOKENS: // Tapped tokens
					// Make sure we have any tapped tokens to add to summoning sickness tokens
					turnList[turnCount].tappedTokens = turnList[turnCount].tappedTokens > 0 ? turnList[turnCount].tappedTokens-1 : 0;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					break;
			}
		}
		if(kDown & KEY_L) {
			switch(rowPointer) {
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount = turnList[turnCount].startHareCount >= 10 ? turnList[turnCount].startHareCount-10 : 0;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount = turnList[turnCount].totalTokenCount >= 10 ? turnList[turnCount].totalTokenCount-10 : 0;
					break;
				case NEWHARECOUNT: // Newly summoned hare count
					turnList[turnCount].summonHareCount = turnList[turnCount].summonHareCount >= 10 ? turnList[turnCount].summonHareCount-10 : 0;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier = turnList[turnCount].additionalTokenMultiplier >= 11 ? turnList[turnCount].additionalTokenMultiplier-10 : 1;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					if(turnList[turnCount].tokenCountExisting >= 10) {
						turnList[turnCount].tokenCountExisting -= 10;
						turnList[turnCount].totalTokenCount -= 10;
					}
					else {
						turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountExisting;
						turnList[turnCount].tokenCountExisting = 0;
					}
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					if(turnList[turnCount].tokenCountSickness >= 10) {
						turnList[turnCount].tokenCountSickness -= 10;
						turnList[turnCount].totalTokenCount -= 10;
					}
					else {
						turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountSickness;
						turnList[turnCount].tokenCountSickness = 0;
					}
					break;
				case TAPPEDTOKENS: // Tapped tokens
 					turnList[turnCount].tappedTokens = turnList[turnCount].tappedTokens >= 10 ? turnList[turnCount].tappedTokens-10 : 0;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					break;
			}
		}
		if(kDown & KEY_ZL) {
			switch(rowPointer) {
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount = turnList[turnCount].startHareCount >= 100 ? turnList[turnCount].startHareCount-100 : 0;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount = turnList[turnCount].totalTokenCount >= 100 ? turnList[turnCount].totalTokenCount-100 : 0;
					break;
				case NEWHARECOUNT: // Newly summoned hare count
					turnList[turnCount].summonHareCount = turnList[turnCount].summonHareCount >= 100 ? turnList[turnCount].summonHareCount-100 : 0;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier = turnList[turnCount].additionalTokenMultiplier >= 101 ? turnList[turnCount].additionalTokenMultiplier-100 : 1;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					if(turnList[turnCount].tokenCountExisting >= 100) {
						turnList[turnCount].totalTokenCount -= 100;
						turnList[turnCount].tokenCountExisting -= 100;
					}
					else {
						turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountSickness;
						turnList[turnCount].tokenCountExisting = 0;
					}
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					if(turnList[turnCount].tokenCountSickness >= 100) {
						turnList[turnCount].totalTokenCount -= 100;
						turnList[turnCount].tokenCountSickness -= 100;
					}
					else {
						turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountSickness;
						turnList[turnCount].tokenCountSickness = 0;
					}
					break;
				case TAPPEDTOKENS: // Tapped tokens
					turnList[turnCount].tappedTokens = turnList[turnCount].tappedTokens >= 100 ? turnList[turnCount].tappedTokens-100 : 0;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					break;
			}
		}
		if(kDown & KEY_DRIGHT) {
			switch(rowPointer) {
				case TURNCOUNT: // Current turn count
					turnCount += 1;
					// Check if our current index is outside array bounds (index starts at 0) or we flagged it for overwrite
					//if(sizeof(turnList) / sizeof(turnList[0]) >= turnCount || turnList[turnCount].index == -1) {

					// This is bad, but arrays force my hand
					if(turnCount > maxTurnCount || turnList[turnCount].index == -1) {
						turnList[turnCount] = turnList[turnCount-1];
						turnList[turnCount].index = turnCount;
						turnList[turnCount].tappedTokens = 0;
						turnList[turnCount].tokenCountSickness = 0;
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount;
						maxTurnCount = turnCount;
					}
					
					break;
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount += 1;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount += 1;
					break;
				case NEWHARECOUNT: // Newly summoned hare count
					if(!turnList[turnCount].tokenManaTap)
						turnList[turnCount].summonHareCount += 1;
					else
						turnList[turnCount].summonHareCount = 1;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier += 1;
					break;
				case TOKENMANATAP: // Tap tokens for hare cost
					turnList[turnCount].tokenManaTap = true;
					if(turnList[turnCount].summonHareCount > 1)
						turnList[turnCount].summonHareCount = 1;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					turnList[turnCount].totalTokenCount += 1;
					turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tokenCountSickness-turnList[turnCount].tappedTokens;
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					turnList[turnCount].totalTokenCount += 1;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tokenCountExisting-turnList[turnCount].tappedTokens;
					break;
				case TAPPEDTOKENS: // Tapped tokens
					turnList[turnCount].tappedTokens = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens > 0 ? turnList[turnCount].tappedTokens+1 : turnList[turnCount].tappedTokens;
					if(turnList[turnCount].tokenCountSickness > 0) // Make sure there's any ss tokens to tap first
						turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					else if(turnList[turnCount].tokenCountExisting > 0) // If we can't find any ss tokens let's tap non-ss tokens
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					break;
			}
		}
		if(kDown & KEY_R) {
			switch(rowPointer) {
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount += 10;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount += 10;
					break;
				case NEWHARECOUNT: // Newly summoned hare count
					if(!turnList[turnCount].tokenManaTap)
						turnList[turnCount].summonHareCount += 10;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier += 10;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					turnList[turnCount].totalTokenCount += 10;
					turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					turnList[turnCount].totalTokenCount += 10;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					break;
				case TAPPEDTOKENS: // Tapped tokens
					turnList[turnCount].tappedTokens = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens >= 10 ? turnList[turnCount].tappedTokens+10 : turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens;
					if(turnList[turnCount].tokenCountSickness >= 10) // Make sure there's any ss tokens to tap first
						turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					else if(turnList[turnCount].tokenCountExisting >= 10) // If we can't find any ss tokens let's tap non-ss tokens
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					else {
						// Start splitting token taps
						turnList[turnCount].tokenCountSickness = 0;
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					}
					break;
			}
		}
		if(kDown & KEY_ZR) {
			switch(rowPointer) {
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount += 100;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount += 100;
					break;
				case NEWHARECOUNT: // New summoned hare count
					if(!turnList[turnCount].tokenManaTap)
						turnList[turnCount].summonHareCount += 100;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier += 100;
					break;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					turnList[turnCount].totalTokenCount += 100;
					turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					turnList[turnCount].totalTokenCount += 100;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					break;
				case TAPPEDTOKENS: // Tapped tokens
					turnList[turnCount].tappedTokens = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens >= 100 ? turnList[turnCount].tappedTokens+100 : turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens;
					if(turnList[turnCount].tokenCountSickness >= 100) // Make sure there's any ss tokens to tap first
						turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
					else if(turnList[turnCount].tokenCountExisting >= 100) // If we can't find any ss tokens let's tap non-ss tokens
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					else {
						// Start splitting token taps
						turnList[turnCount].tokenCountSickness = 0;
						turnList[turnCount].tokenCountExisting = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountSickness;
					}
					break;
			}
		}
		
		if(kDown & KEY_SELECT) {
			switch(rowPointer) {
				case HARECOUNT: // Current hare count
					turnList[turnCount].startHareCount = 0;
					break;
				case TOKENCOUNT: // Current token count
					turnList[turnCount].totalTokenCount = 0;
					turnList[turnCount].tappedTokens = 0;
					turnList[turnCount].tokenCountExisting = 0;
					turnList[turnCount].tokenCountSickness = 0;
					break;
				case NEWHARECOUNT: // New summoned hare count
					turnList[turnCount].summonHareCount = 0;
					break;
				case TOKENMULTIPLIER: // Token multiplier
					turnList[turnCount].additionalTokenMultiplier = 1;
					break;
				case TOKENMANATAP: // Tap tokens for hare cost
					turnList[turnCount].tokenManaTap = false;
				case UNTAPPEDTOKENEXISTING: // Untapped tokens without summoning sickness
					turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountExisting;
					turnList[turnCount].tokenCountExisting = 0;
					break;
				case UNTAPPEDTOKENSICKNESS: // Untapped tokens with summoning sickness
					turnList[turnCount].totalTokenCount -= turnList[turnCount].tokenCountSickness;
					turnList[turnCount].tokenCountSickness = 0;
					break;
				case TAPPEDTOKENS: // Tapped tokens
					turnList[turnCount].tappedTokens = 0;
					turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tokenCountExisting;
					break;
			}
		}

		if(kDown & KEY_X) {
			// Flag anything starting from current turn for overwrite
			if(turnCount > 0) {
				turnCount -= 1;
				for(int i = sizeof(turnList) / sizeof(turnList[0]); i > turnCount ; i--) {
					turnList[i].index = -1;
				}
				maxTurnCount = turnCount;
			}
		}

		if(kDown & KEY_A) {
			//int hareCountBuffer = turnList[turnCount].summonHareCount;
			//int tokenCountBuffer = turnList[turnCount].totalTokenCount;
			
			calculateTokens(&turnList[turnCount].totalTokenCount, &turnList[turnCount].startHareCount, &turnList[turnCount].summonHareCount, &turnList[turnCount].additionalTokenMultiplier);
			// Update new token sickness count
			turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tokenCountExisting;
			
			// Check if we have atleast 4 tokens to tap if we want to tap tokens for mana (we'll enforce 1 hare when tapping for mana)
			if(turnList[turnCount].tokenManaTap && turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens >= 4) {
				turnList[turnCount].tokenCountSickness -= 4;
				turnList[turnCount].tappedTokens += 4;
			}
			turnList[turnCount].tokenCountSickness = turnList[turnCount].totalTokenCount-turnList[turnCount].tappedTokens-turnList[turnCount].tokenCountExisting;
		}

		printf("\x1b[3;1H");

		printf("Current turn count: %s%i\x1b[0m               \n", rowPointer == TURNCOUNT ? "\x1b[31m" : "\x1b[0m", turnCount+1);
		printf("\n");
		printf("Current hare count: %s%i\x1b[0m               \n", rowPointer == HARECOUNT ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].startHareCount);
		printf("Current token count: %s%i\x1b[0m               \n", rowPointer == TOKENCOUNT ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].totalTokenCount);
		printf("\n");
		printf("New summoned hares: %s%i\x1b[0m               \n", rowPointer == NEWHARECOUNT ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].summonHareCount);
		printf("Token multiplier (additive): %s%i\x1b[0m               \n", rowPointer == TOKENMULTIPLIER ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].additionalTokenMultiplier);
		printf("Tap tokens for Hare mana: %s%s\x1b[0m               \n", rowPointer == TOKENMANATAP ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].tokenManaTap == true ? "true" : "false");
		printf("\n");
		printf("Untapped tokens (without SS): %s%i\x1b[0m               \n", rowPointer == UNTAPPEDTOKENEXISTING ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].tokenCountExisting);
		printf("Untapped tokens (with SS): %s%i\x1b[0m               \n", rowPointer == UNTAPPEDTOKENSICKNESS ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].tokenCountSickness);
		printf("Tapped tokens: %s%i\x1b[0m               \n", rowPointer == TAPPEDTOKENS ? "\x1b[31m" : "\x1b[0m", turnList[turnCount].tappedTokens);
		printf("%s\x1b[0m                     \n", turnList[turnCount].totalTokenCount != (turnList[turnCount].tokenCountExisting + turnList[turnCount].tokenCountSickness + turnList[turnCount].tappedTokens) ? "\x1b[31mWarning! Token count is off!": "");

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit services
	gfxExit();

	return 0;
}