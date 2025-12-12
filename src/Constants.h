#pragma once

//////////////////////////////////////////////////////
//	DEFINES
//////////////////////////////////////////////////////

#define WNDTITLE	"pacman_sdl (" __DATE__ ", " __TIME__ ")"

#define STATE_GAME	0
#define STATE_ENTER_HSCORE 1
#define STATE_VIEW_HSCORE 2
#define STATE_MAP_EDITOR 3
#define STATE_STOPPED 4
#define STATE_EDITOR 5

#define MODE_LEVELS 2
#define MODE_SKINS 1

#define UP	0
#define RIGHT   1
#define DOWN    2
#define LEFT    3
#define ENTER   4
#define SPACE   5
#define CLICK   6

#define MOVEMOD 2000.0f

#define EXTRA_Y_SPACE 50

#define SMALL_DOTS_SCORE 10
#define LARGE_DOTS_SCORE 50

#define SETTINGSFILE "settings.conf"
#define CFGFILE "level.conf"
#define OBJFILE "objmap"
#define MAPFILE "map"

#define FRUITDURATION 10000
#define BOOSTTIME 3000

#define MAXENTRIES 8
