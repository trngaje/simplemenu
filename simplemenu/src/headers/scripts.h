/***************************************************************************

    ui_text.h

    Functions used to retrieve text used by MAME, to aid in
    translation.

    Copyright (c) 1996-2006, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__

#include <stdint.h>

/* Important: this must match the default_text list in ui_text.c! */
enum
{
	UI_Confirm = 0,
	UI_Back_Function_key,
	UI_Mark_favorite,
	UI_Show_Hide_favorites,
	UI_Fullscreen_mode,
	UI_Select,
	UI_Game_options,
	UI_Scroll,
	UI_Previous_Next_letter,
	UI_Quick_switch,
	UI_Random_select,
	UI_Delete_game,
	UI_PAGE_1_2___PRESS_B_TO_RETURN,
	UI_PAGE_2_2___PRESS_B_TO_RETURN,
	
	/* setupSettingsScreen */
	UI_Session_,
	UI_A_TO_CONFIRM___LEFT_RIGHT_TO_CHOOSE,
	UI_Theme_,
	UI_LAUNCHER_THEME,
	UI_Default_launcher_,
	UI_LAUNCH_AFTER_BOOTING,
	UI_Appearance_,
	UI_APPEARANCE_OPTIONS,
	UI_System_,
	UI_SYSTEM_OPTIONS,
	UI_Help_,
	UI_HOW_TO_USE_THIS_MENU,
	UI_shutdown,
	UI_reboot,
	UI_quit,
	UI_Language,
	
	/* updateScreen */
	UI_SETTINGS,
	UI_HELP,
	UI_APPEARANCE,
	UI_SYSTEM,
	
	/* add_menu_zone */
	UI_VOLUME,
	UI_BRIGHTNESS,
	UI_SET_THEME,
	UI_SET_LAUNCHER,
	UI_SET_SYSTEM_,
	UI_EXIT,
	UI_POWERDOWN,
	
	/* menu_screen_refresh */
	UI_Saving___,
	UI_Are_you_sure_,
	UI_Loading___,
	UI_EJECT_USB,
	UI_MOUNT_USB,
	UI_in_progress____,
	UI_Shutting_down___,
	UI_READ_ONLY,
	UI_READ_WRITE,
	
	/* showRomPreferences */
	UI_Overclock__,
	UI_yes,
	UI_no,
	UI_not_available,
	UI_Autostart__,
	UI_Emulator__,
	
	/* setupAppearanceSettings */
	UI_Tidy_rom_names_,
	UI_enabled,
	UI_disabled,
	UI_CUT_DETAILS_OUT_OF_ROM_NAMES,
	UI_Fullscreen_rom_names_,
	UI_DISPLAY_THE_CURRENT_ROM_NAME,
	UI_Fullscreen_menu_,
	UI_DISPLAY_A_TRANSLUCENT_MENU,
	
	/* setupSystemSettings */
	UI_Brightness_,
	UI_ADJUST_BRIGHTNESS_LEVEL,
	UI_Screen_timeout_,
	UI_SECONDS_UNTIL_THE_SCREEN_TURNS_OFF,
	UI_always_on
};

int uistring_init(FILE *fp);
void uistring_free(void);

const char * ui_getstring (int string_num);

#endif /* __UI_TEXT_H__ */

