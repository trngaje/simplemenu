
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../headers/scripts.h"

static const char *simplemenu_default_text[] =
{
	/* setupHelpScreen */
	"Confirm",
	"Back/Function key",
	"Mark favorite",
	"Show/Hide favorites",
	"Fullscreen mode",
	"Select",
	"Game options",
	"Scroll",
	"Previous/Next letter",
	"Quick switch",
	"Random select",
	"Delete game",
	"PAGE 1/2 - PRESS B TO RETURN",
	"PAGE 2/2 - PRESS B TO RETURN",
	
	/* setupSettingsScreen */
	"Session ",
	"A TO CONFIRM - LEFT/RIGHT TO CHOOSE",
	"Theme ",
	"LAUNCHER THEME",
	"Default launcher ",
	"LAUNCH AFTER BOOTING",
	"Appearance ",
	"APPEARANCE OPTIONS",
	"System ",
	"SYSTEM OPTIONS",
	"Help ",
	"HOW TO USE THIS MENU",
	"shutdown",
	"reboot",
	"quit",
	"Language",

	/* updateScreen */
	"SETTINGS",
	"HELP",
	"APPEARANCE",
	"SYSTEM",

	/* add_menu_zone */
	"VOLUME",
	"BRIGHTNESS",
	"SET THEME",
	"SET LAUNCHER",
	"SET SYSTEM:",
	"EXIT",
	"POWERDOWN",
	
	/* menu_screen_refresh */
	"Saving...",
	"Are you sure?",
	"Loading...",
	"EJECT USB",
	"MOUNT USB",
	"in progress ...",
	"Shutting down...",
	"READ-ONLY",
	"READ-WRITE",

	/* showRomPreferences */
	"Overclock: ",
	"yes",
	"no",
	"not available",
	"Autostart: ",
	"Emulator: ",
	
	/* setupAppearanceSettings */
	"Tidy rom names ",
	"enabled",
	"disabled",
	"CUT DETAILS OUT OF ROM NAMES",
	"Fullscreen rom names ",
	"DISPLAY THE CURRENT ROM NAME",
	"Fullscreen menu ",
	"DISPLAY A TRANSLUCENT MENU",
	
	/* setupSystemSettings */
	"Brightness ",
	"ADJUST BRIGHTNESS LEVEL",
	"Screen timeout ",
	"SECONDS UNTIL THE SCREEN TURNS OFF",
	"always on",
	NULL
};

static const char **trans_text;

int uistring_init(FILE *fp)
{
	int i, j, str;
	int string_count;

	/* count the total amount of strings */
	string_count = 0;
	for (i = 0; simplemenu_default_text[i]; i++)
	{
		string_count++;
	}

	/* allocate the translated text array, and set defaults */
	trans_text = (const char **)malloc(sizeof(const char *) * string_count);

	/* initialize all of the strings */
	str = 0;
	for (i = 0; simplemenu_default_text[i]; i++)
	{
		trans_text[str++] = NULL;
	}

	/* if no language file, exit */
	if (!fp)
		return 0;

	char * line = NULL;
	size_t len = 0;
	
	while (getline(&line, &len, fp) != -1) {
		char *p;
		p = strchr(line, '\n');
		if (p) {
			*p = '\0';
		}

		char *menuname = strtok(line, "=");
		char *aliasname = strtok(NULL, "=");

		/* Find a matching default string */
		str = 0;
		for (i = 0; simplemenu_default_text[i]; i++)
		{
			if (strcmp(menuname, simplemenu_default_text[i]) == 0) {
				/* Allocate storage and copy the string */
				trans_text[str] = strdup(aliasname);
				if (!trans_text[str])
					return 1;	
			}
			str++;
		}
	}
	if (line)
		free(line);

	/* indicate success */
	return 0;
}



const char * ui_getstring(int string_num)
{	if (trans_text[string_num] == NULL)
		return simplemenu_default_text[string_num];
	else
		return trans_text[string_num];
}


void uistring_free(void)
{
	int i;
	
	for (i = 0; simplemenu_default_text[i]; i++)
	{
		if (trans_text[i] != NULL)
			free(trans_text[i]);
	}
	free(trans_text);
}