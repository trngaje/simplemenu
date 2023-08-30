#include "../headers/globals.h"
#ifndef SCREEN_DEFINED
#define SCREEN_DEFINED

enum {
#if defined(RGNANO) || defined(FUNKEY)
	ID_MENU_SYSTEM_SETTING_BRIGHTNESS_OPTION=0,
	ID_MENU_SYSTEM_SETTING_SCREEN_TIMEOUT_OPTION,
#else
	ID_MENU_SYSTEM_SETTING_VOLUME_OPTION=0
	ID_MENU_SYSTEM_SETTING_BRIGHTNESS_OPTION,
	ID_MENU_SYSTEM_SETTING_SHARPNESS_OPTION,
	ID_MENU_SYSTEM_SETTING_SCREEN_TIMEOUT_OPTION,
	ID_MENU_SYSTEM_SETTING_OC_OPTION,
	ID_MENU_SYSTEM_SETTING_USB_OPTION,
#endif
	NUM_OF_MENU_SYSTEM_SETTING
};

enum {
	ID_MENU_SETTING_SHUTDOWN_OPTION=0,
	ID_MENU_SETTING_THEME_OPTION,
	ID_MENU_SETTING_LANGUAGE_OPTION, // add by trngaje
	ID_MENU_SETTING_DEFAULT_OPTION,
	ID_MENU_SETTING_APPEARANCE_OPTION,
	ID_MENU_SETTING_SYSTEM_OPTION,
	ID_MENU_SETTING_HELP_OPTION,
	NUM_OF_MENU_SETTING
};

void updateScreen(struct Node *node);
void drawHeader();
void drawGameList();
void drawFooter(char *text);
void setupKeys();
void setupDecorations();
void freeResources();
void displayGamePicture();
void displayBackgroundPicture();
void showConsole();
void drawShutDownScreen();
uint32_t hideFullScreenModeMenu();
void resetPicModeHideMenuTimer();
void resetPicModeHideLogoTimer();
void clearPicModeHideMenuTimer();
void clearPicModeHideLogoTimer();
void clearHideHeartTimer();
void resetHideHeartTimer();
void drawSpecialScreen();
void drawHelpScreen(int page);
void clearShutdownTimer();
void resetShutdownTimer();
void startBatteryTimer();
void clearBatteryTimer();
void drawLoadingText();
void drawCopyingText();
void resizeSectionBackground(struct MenuSection *section);
void initializeFonts();
void freeResources();
void freeFonts();
void freeSettingsFonts();
void initializeSettingsFonts();

#endif
