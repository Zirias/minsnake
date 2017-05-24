include zimk/zimk.mk

minsnake_MODULES:= minsnake
minsnake_win32_STATICLIBS:= pdcurses
minsnake_posix_LIBS:= ncurses
$(call binrules,minsnake)

ifeq ($(PLATFORM),win32)
minw32snake_MODULES:= minw32snake
$(call binrules,minw32snake)
endif
