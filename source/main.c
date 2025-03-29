#include <3ds.h>
#include <stdio.h>

int rowMin = 0;
int rowMax = 6;

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

	// Initialize some variables to store the state of hares
	int startHareCount = 0;
	int startTokenCount = 0;
	int summonHareCount = 0;
	int additionalTokenMultiplier = 1;
	int oldTokenCount = 0;
	bool doubleTokens = false;

	// Token states
	int untappedTokens = 0;
	int tappedTokens = 0;

	// Row pointer
	int rowPointer = 0;

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
				case 0:
					startHareCount = startHareCount > 0 ? startHareCount-1 : 0;
					break;
				case 1:
					startTokenCount = startTokenCount > 0 ? startTokenCount-1 : 0;
					break;
				case 2:
					summonHareCount = summonHareCount > 0 ? summonHareCount-1 : 0;
					break;
				case 3:
					additionalTokenMultiplier = additionalTokenMultiplier > 1 ? additionalTokenMultiplier-1 : 1;
					break;
				case 4:
					doubleTokens = false;
					break;
				case 5:
					tappedTokens = startTokenCount-tappedTokens > 0 ? tappedTokens+1 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = tappedTokens > 0 ? tappedTokens-1 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		if(kDown & KEY_L) {
			switch(rowPointer) {
				case 0:
					startHareCount = startHareCount >= 10 ? startHareCount-10 : 0;
					break;
				case 1:
					startTokenCount = startTokenCount >= 10 ? startTokenCount-10 : 0;
					break;
				case 2:
					summonHareCount = summonHareCount >= 10 ? summonHareCount-10 : 0;
					break;
				case 3:
					additionalTokenMultiplier = additionalTokenMultiplier >= 11 ? additionalTokenMultiplier-10 : 1;
					break;
				case 5:
					tappedTokens = startTokenCount-tappedTokens >= 10 ? tappedTokens+10 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = tappedTokens >= 10 ? tappedTokens-10 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		if(kDown & KEY_ZL) {
			switch(rowPointer) {
				case 0:
					startHareCount = startHareCount >= 100 ? startHareCount-100 : 0;
					break;
				case 1:
					startTokenCount = startTokenCount >= 100 ? startTokenCount-100 : 0;
					break;
				case 2:
					summonHareCount = summonHareCount >= 100 ? summonHareCount-100 : 0;
					break;
				case 3:
					additionalTokenMultiplier = additionalTokenMultiplier >= 101 ? additionalTokenMultiplier-100 : 1;
					break;
				case 5:
					tappedTokens = startTokenCount-tappedTokens >= 100 ? tappedTokens+100 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = tappedTokens >= 100 ? tappedTokens-100 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		if(kDown & KEY_DRIGHT) {
			switch(rowPointer) {
				case 0:
					startHareCount += 1;
					break;
				case 1:
					startTokenCount += 1;
					break;
				case 2:
					summonHareCount += 1;
					break;
				case 3:
					additionalTokenMultiplier += 1;
					break;
				case 4:
					doubleTokens = true;
					break;
				case 5:
					tappedTokens = tappedTokens > 0 ? tappedTokens-1 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = startTokenCount-tappedTokens > 0 ? tappedTokens+1 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		if(kDown & KEY_R) {
			switch(rowPointer) {
				case 0:
					startHareCount += 10;
					break;
				case 1:
					startTokenCount += 10;
					break;
				case 2:
					summonHareCount += 10;
					break;
				case 3:
					additionalTokenMultiplier += 10;
					break;
				case 5:
					tappedTokens = tappedTokens >= 10 ? tappedTokens-10 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = startTokenCount-tappedTokens >= 10 ? tappedTokens+10 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		if(kDown & KEY_ZR) {
			switch(rowPointer) {
				case 0:
					startHareCount += 100;
					break;
				case 1:
					startTokenCount += 100;
					break;
				case 2:
					summonHareCount += 100;
					break;
				case 3:
					additionalTokenMultiplier += 100;
					break;
				case 5:
					tappedTokens = tappedTokens >= 100 ? tappedTokens-100 : 0;
					untappedTokens = startTokenCount-tappedTokens;
					break;
				case 6:
					tappedTokens = startTokenCount-tappedTokens >= 100 ? tappedTokens+100 : tappedTokens;
					untappedTokens = startTokenCount-tappedTokens;
					break;
			}
		}
		
		if(kDown & KEY_SELECT) {
			switch(rowPointer) {
				case 0:
					startHareCount = 0;
					break;
				case 1:
					startTokenCount = 0;
					tappedTokens = 0;
					untappedTokens = 0;
					break;
				case 2:
					summonHareCount = 0;
					break;
				case 3:
					additionalTokenMultiplier = 1;
					break;
				case 4:
					doubleTokens = false;
				case 5:
					tappedTokens = startTokenCount;
					untappedTokens = 0;
					break;
				case 6:
					tappedTokens = 0;
					untappedTokens = startTokenCount;
			}
		}

		if(kDown & KEY_A) {
			oldTokenCount = startTokenCount;
			if(doubleTokens)
				startTokenCount *= 2;
			doubleTokens = false;
			calculateTokens(&startTokenCount, &startHareCount, &summonHareCount, &additionalTokenMultiplier);
			untappedTokens = startTokenCount-tappedTokens;
		}

		printf("\x1b[3;1H");

		printf("Current hare count: %s%i\x1b[0m               \n", rowPointer == 0 ? "\x1b[31m" : "\x1b[0m", startHareCount);
		printf("Current token count: %s%i\x1b[0m               \n", rowPointer == 1 ? "\x1b[31m" : "\x1b[0m", startTokenCount);
		printf("New summoned hares: %s%i\x1b[0m               \n", rowPointer == 2 ? "\x1b[31m" : "\x1b[0m", summonHareCount);
		printf("Token multiplier (additive): %s%i\x1b[0m               \n", rowPointer == 3 ? "\x1b[31m" : "\x1b[0m", additionalTokenMultiplier);
		printf("Double token count: %s%s\x1b[0m               \n", rowPointer == 4 ? "\x1b[31m" : "\x1b[0m", doubleTokens == true ? "true" : "false");
		printf("\n");
		printf("Old token count: %i               \n", oldTokenCount);
		printf("\n");
		printf("Untapped tokens: %s%i\x1b[0m               \n", rowPointer == 5 ? "\x1b[31m" : "\x1b[0m", untappedTokens);
		printf("Untapped tokens: %s%i\x1b[0m               \n", rowPointer == 6 ? "\x1b[31m" : "\x1b[0m", tappedTokens);

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