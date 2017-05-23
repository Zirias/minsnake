include zimk/zimk.mk

minsnake_MODULES:= minsnake
minsnake_win32_STATICLIBS:= pdcurses
minsnake_posix_LIBS:= ncurses

$(call binrules,minsnake)

