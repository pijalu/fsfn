bin_PROGRAMS = fsfn
fsfn_SOURCES = fsfn.c alsasound.c acpihandler.c basicmsg.c osd.c readconfig.c autodev.c

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
fsfn_LDFLAGS = $(all_libraries) 
noinst_HEADERS = alsasound.h acpihandler.h osd.h generics.h basicmsg.h readconfig.h autodev.h
AM_CFLAGS = -Wall

# man things
PFLAGS  = -c "Sony Vaio Hotkey Support" -r fsfn
# man defs
man_MANS = fsfn.1 fsfn.5
EXTRA_DIST= fsfn.pod
# clean generated man files
MOSTLYCLEANFILES=$(man_MANS)

# man suffixes
SUFFIXES: .c .1 .pod .5

.c.1:
	@ echo generating manual page $@...
	@ $(POD2MAN) $(PFLAGS) -s1 -n$* $< >$@

.pod.5:
	@ echo generating manual page $@...
	@ $(POD2MAN) $(PFLAGS) -s5 -n$* $< >$@


