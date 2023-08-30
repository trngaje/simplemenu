#if defined (TARGET_OD) || defined (TARGET_OD_BETA)
#include <shake.h>
#endif
#include <stdlib.h>
#include "../headers/config.h"
#include "../headers/control.h"
#include "../headers/definitions.h"
#include "../headers/globals.h"
#include "../headers/graphics.h"
#include "../headers/logic.h"
#include "../headers/screen.h"
#include "../headers/utils.h"
#include "../headers/system_logic.h"

#if defined(RGNANO) || defined(FUNKEY)
#include "../headers/fk_menu.h"
#endif
int performAction(struct Node *node) {
	struct Rom *rom;
	if (node!=NULL) {
		rom = node->data;
	} else {
		rom = NULL;
	}
	if((!alternateControls&&currentState==SELECTING_SECTION)||(alternateControls&&(currentState==CHOOSING_GROUP||currentState==SELECTING_SECTION))) {
		if (keys[BTN_A]) {
			if (CURRENT_SECTION.backgroundSurface==NULL) {
				logMessage("INFO","performAction","Loading system background");
				CURRENT_SECTION.backgroundSurface = IMG_Load(CURRENT_SECTION.background);
				logMessage("INFO","performAction","Loading system picture");
				CURRENT_SECTION.systemPictureSurface = IMG_Load(CURRENT_SECTION.systemPicture);
			}
			logMessage("INFO","performAction","Loading game list");
			if (currentSectionNumber!=favoritesSectionNumber) {
				loadGameList(0);
#if !defined(TARGET_BITTBOY) && !defined(TARGET_RFW)
			} else {
				loadFavoritesSectionGameList();
			}
#endif
			if(CURRENT_SECTION.gameCount>0) {
				scrollToGame(CURRENT_SECTION.realCurrentGameNumber);
			}
			pushEvent();
			currentState=BROWSING_GAME_LIST;
			return 1;
		}
		if (keys[BTN_START]) {
#if !defined(TARGET_BITTBOY) && !defined(TARGET_RFW)
			chosenSetting=0;
#endif
			previousState=currentState;
			currentState=SETTINGS_SCREEN;
			chosenSetting=SHUTDOWN_OPTION;
			themeChanged = activeTheme;
			langChanged = activeLang;
			selectedShutDownOption=0;
			currRawtime = time(NULL);
			currTime = localtime(&currRawtime);
			lastMin=currTime->tm_min;
			lastChargeLevel = getBatteryLevel();
			return 1;
		}
#if defined(RGNANO) || defined(FUNKEY)		
		if (keys[BTN_POWER]) {
			if (FK_RunMenu(screen) == MENU_RETURN_EXIT) {
				running = 0;
				return 0;
			}
		}
#endif
	}
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
	if (rom!=NULL&&keys[BTN_SELECT]&&keys[BTN_R1]) {
#else
	if (rom!=NULL&&keys[BTN_R2]) {
#endif
		hideFullScreenModeMenu();
		if(currentSectionNumber!=favoritesSectionNumber) {
			logMessage("INFO","performAction","Not Favs, loading game list");
			loadGameList(1);
			return(1);
#if !defined(TARGET_BITTBOY) && !defined(TARGET_RFW)
		} else {
			loadFavoritesSectionGameList();
			return(1);
#endif
		}
	}
	if(itsStoppedBecauseOfAnError&&!keys[BTN_A]) {
		return(0);
	}
	if(keys[BTN_B]&&!(currentState==SELECTING_SECTION)) {
		hotKeyPressed=1;
		if (currentState==BROWSING_GAME_LIST) {
			if (rom!=NULL&&keys[BTN_A]) {
				launchEmulator(rom);
				aKeyComboWasPressed=1;
				return 1;
			}
			if (rom!=NULL&&keys[BTN_X]) {
				if (!isPicModeMenuHidden) {
					resetPicModeHideMenuTimer();
				}
				callDeleteGame(rom);
				aKeyComboWasPressed=1;
				return 1;
			}

			if (rom!=NULL&&keys[BTN_SELECT]) {
				int flag = 0;
				const int GAME_FPS=60;
				const int FRAME_DURATION_IN_MILLISECONDS = 1000/GAME_FPS;
				Uint32 start_time;
				for(int i=0;i<25;i++) {
					selectRandomGame();
					if (fullscreenMode==0) {
						fullscreenMode=1;
						flag = 1;
					}
					updateScreen(CURRENT_SECTION.currentGameNode);
					refreshScreen();
					int timeSpent = SDL_GetTicks()-start_time;
					if(timeSpent < FRAME_DURATION_IN_MILLISECONDS) {
						//Wait the remaining time until one frame completes
						SDL_Delay(FRAME_DURATION_IN_MILLISECONDS-timeSpent);
					}
				}
				saveFavorites();
				if (flag == 1) {
					fullscreenMode=0;
				}
				launchGame(CURRENT_SECTION.currentGameNode->data);
			}
			if (rom!=NULL&&keys[BTN_RIGHT]) {
				hotKeyPressed=1;
				CURRENT_SECTION.alphabeticalPaging=1;
				advancePage(rom);
				if(fullscreenMode) {
					resetPicModeHideMenuTimer();
				}
				aKeyComboWasPressed=1;
				return 0;
			}
			if (rom!=NULL&&keys[BTN_LEFT]) {
				hotKeyPressed=1;
				CURRENT_SECTION.alphabeticalPaging=1;
				rewindPage(rom);
				if(fullscreenMode) {
					resetPicModeHideMenuTimer();
				}
				aKeyComboWasPressed=1;
				return 0;
			}
		}
		if (currentState == BROWSING_GAME_LIST) {
			if(keys[BTN_DOWN]) {
				hotKeyPressed=0;
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
				int advanced = advanceSection(0);
#else
				int advanced = advanceSection();
#endif
				if(advanced) {
					if (CURRENT_SECTION.backgroundSurface == NULL) {
						logMessage("INFO","performAction","Loading system background");
						CURRENT_SECTION.backgroundSurface = IMG_Load(CURRENT_SECTION.background);
						resizeSectionBackground(&CURRENT_SECTION);
						CURRENT_SECTION.systemPictureSurface = IMG_Load(CURRENT_SECTION.systemPicture);
					}
					logMessage("INFO","performAction","Advanced, loading game list");
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
					loadGameList(0);
#else
					if(currentSectionNumber==favoritesSectionNumber) {
						loadFavoritesSectionGameList();
					} else {
						loadGameList(0);
					}
#endif
				}
				if(CURRENT_SECTION.gameCount>0) {
					scrollToGame(CURRENT_SECTION.realCurrentGameNumber);
				}
				aKeyComboWasPressed=1;
				return 0;
			}
			if(keys[BTN_UP]) {
				hotKeyPressed=0;
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
				int rewinded = rewindSection(0);
#else
				int rewinded = rewindSection();
#endif
				if(rewinded) {
					if (CURRENT_SECTION.backgroundSurface == NULL) {
						logMessage("INFO","performAction","Loading system background");
						CURRENT_SECTION.backgroundSurface = IMG_Load(CURRENT_SECTION.background);
						logMessage("INFO","performAction","Loading system picture");
						CURRENT_SECTION.systemPictureSurface = IMG_Load(CURRENT_SECTION.systemPicture);
					}
					logMessage("INFO","performAction","Rewinded, loading game list");
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
					loadGameList(0);
#else
					if(currentSectionNumber==favoritesSectionNumber) {
						loadFavoritesSectionGameList();
					} else {
						loadGameList(0);
					}
#endif
				}
				if(CURRENT_SECTION.gameCount>0) {
					scrollToGame(CURRENT_SECTION.realCurrentGameNumber);
				}
				aKeyComboWasPressed=1;
				return 0;
			}
		}
	}
#if defined(TARGET_BITTBOY)
	if (keys[BTN_SELECT]&&!isFavoritesSectionSelected()&&!(currentState==SELECTING_SECTION)) {
#elif defined(TARGET_RFW)
	if (keys[BTN_SELECT]&&!favoritesSectionSelected&&!(currentState==SELECTING_SECTION)) {
#else
	if (keys[BTN_SELECT]&&currentSectionNumber!=favoritesSectionNumber&&!(currentState==SELECTING_SECTION)) {
#endif
		currentState=SELECTING_EMULATOR;
		chosenChoosingOption=0;
		launchAtBoot=isLaunchAtBoot(CURRENT_SECTION.currentGameNode->data->name);
		drawTransparentRectangleToScreen(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, (int[]) {0,0,0},180);
		loadRomPreferences(CURRENT_SECTION.currentGameNode->data);
		return 0;
	}
	if(!alternateControls) {
		if((currentState==SELECTING_SECTION&&(keys[BTN_UP]))) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
			if (currentSectionNumber!=favoritesSectionNumber && menuSectionCounter>1) {
#else
			if (menuSectionCounter>1) {
				if (currentSectionNumber!=favoritesSectionNumber) {
					returnTo=currentSectionNumber;
				}
#endif
				currentState=SELECTING_SECTION;
				hotKeyPressed=0;
				rewindSection();
			}
		}
	} else {
		if(keys[BTN_L1]) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
			if (currentSectionNumber!=favoritesSectionNumber && menuSectionCounter>1) {
#else
			if (menuSectionCounter>1) {
				if (currentSectionNumber!=favoritesSectionNumber) {
					returnTo=currentSectionNumber;
				}
#endif
				currentState=SELECTING_SECTION;
				hotKeyPressed=0;
				rewindSection();
			}
		}
	}

	if(!alternateControls) {
		if((currentState==SELECTING_SECTION&&(keys[BTN_DOWN]))) {
			if (currentSectionNumber!=favoritesSectionNumber) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
				currentState=SELECTING_SECTION;
				hotKeyPressed=0;
				advanceSection(1);
			}
#else
				returnTo=currentSectionNumber;
			}
			currentState=SELECTING_SECTION;
			hotKeyPressed=0;
			advanceSection();
#endif
			return 0;
		}
	} else {
		if(keys[BTN_R1]) {
			if (currentSectionNumber!=favoritesSectionNumber) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
				currentState=SELECTING_SECTION;
				hotKeyPressed=0;
				advanceSection(1);
#else
				returnTo=currentSectionNumber;
			}
			currentState=SELECTING_SECTION;
			hotKeyPressed=0;
			advanceSection();
#endif
			return 0;
		}
	}

	if (currentState!=SELECTING_EMULATOR&&!hotKeyPressed) {
		if (keys[BTN_Y]) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
			showOrHideFavorites();
#else
			if(!isFavoritesSectionSelected()) {
				if(FAVORITES_SECTION.gameCount>0) {
					showOrHideFavorites();
				}
			} else {
				currentSectionNumber=returnTo;
				if (CURRENT_SECTION.gameCount==0) {
					CURRENT_SECTION.gameCount=theSectionHasGames(&CURRENT_SECTION);
				}
				currentState=SELECTING_SECTION;
				if (CURRENT_SECTION.systemLogoSurface == NULL) {
					CURRENT_SECTION.systemLogoSurface = IMG_Load(CURRENT_SECTION.systemLogo);
				}
			}
#endif
			return 0;
		}
	}

	if (currentState!=SELECTING_EMULATOR&&!hotKeyPressed&&!(currentState==SELECTING_SECTION)) {

		if (rom!=NULL&&keys[BTN_X]) {
			if(!isPicModeMenuHidden) {
				resetPicModeHideMenuTimer();
			}
#if defined(TARGET_BITTBOY)
			if (!isFavoritesSectionSelected()) {
#elif defined(TARGET_RFW)
			if (!favoritesSectionSelected) {
#else
			if (currentSectionNumber!=favoritesSectionNumber) {
#endif
				markAsFavorite(rom);
			} else {
				removeFavorite();
				if(favoritesSize==0) {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
					showOrHideFavorites();
#else
					currentState = SELECTING_SECTION;
					currentSectionNumber=returnTo;
					if(CURRENT_SECTION.gameCount==0) {
						int advanced = advanceSection();
						if (!advanced) {
							currentSectionNumber=0;
						}
					}
#endif
				}
			}
			return 0;
		}
		if (keys[BTN_START]) {
			chosenSetting=SHUTDOWN_OPTION;
			selectedShutDownOption=0;
			previousState=currentState;
			currentState=SETTINGS_SCREEN;
			themeChanged=activeTheme;
			langChanged = activeLang;
			lastChargeLevel = getBatteryLevel();
			return 0;
		}

#if defined(RGNANO) || defined(FUNKEY)		
		if (keys[BTN_POWER]) {
			if (FK_RunMenu(screen) == MENU_RETURN_EXIT) {
				running = 0;
				return 0;
			}
		}
#endif
		
		if (rom!=NULL&&keys[BTN_A]) {
			if(itsStoppedBecauseOfAnError) {
				if(thereIsACriticalError) {
					#ifndef TARGET_PC
					running=0;
					#else
					freeResources();
					saveLastState();
					saveFavorites();
					exit(0);
					#endif
				}
				itsStoppedBecauseOfAnError=0;
				return 0;
			}
			if (countGamesInPage()>0) {
				saveFavorites();
				launchGame(rom);
			}
			return 0;
		}
		if(!alternateControls) {
#if defined(TARGET_BITTBOY)
			if (keys[BTN_R]) {
#else
			if (keys[BTN_R1]) {
#endif
				int number = CURRENT_GAME_NUMBER;
				if (fullscreenMode) {
					fullscreenMode=0;
					ITEMS_PER_PAGE=MENU_ITEMS_PER_PAGE;
				} else {
					fullscreenMode=1;
					ITEMS_PER_PAGE=FULLSCREEN_ITEMS_PER_PAGE;
				}
				if (CURRENT_SECTION.gameCount>0) {
					scrollToGame(number);
				}
			}
		} else {
#if defined(TARGET_BITTBOY) || defined(TARGET_RFW)
			if (keys[BTN_R]) {
#else
			if (keys[BTN_L2]) {
#endif
				int number = CURRENT_GAME_NUMBER;
				if (fullscreenMode) {
					fullscreenMode=0;
					ITEMS_PER_PAGE=MENU_ITEMS_PER_PAGE;
				} else {
					fullscreenMode=1;
					ITEMS_PER_PAGE=FULLSCREEN_ITEMS_PER_PAGE;
				}
				if (CURRENT_SECTION.gameCount>0) {
					scrollToGame(number);
				}
			}
		}
		if (rom!=NULL&&keys[BTN_DOWN]) {
			if(fullscreenMode) {
				resetPicModeHideMenuTimer();
			}
			scrollDown();
			return 1;
		}
		if(rom!=NULL&&keys[BTN_UP]) {
			if(fullscreenMode) {
				resetPicModeHideMenuTimer();
			}
			scrollUp();
			return 1;
		}
		if(rom!=NULL&&keys[BTN_RIGHT]) {
			if(fullscreenMode) {
				resetPicModeHideMenuTimer();
			}
			advancePage(rom);
			return 1;
		}
		if(rom!=NULL&&keys[BTN_LEFT]) {
			if(fullscreenMode) {
				resetPicModeHideMenuTimer();
			}
			rewindPage(rom);
			return 1;
		}
	}
	return 0;
}
