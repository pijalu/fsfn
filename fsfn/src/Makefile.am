bin_PROGRAMS = fsfn
fsfn_SOURCES = fsfn.c alsasound.c acpihandler.c basicmsg.c osd.c

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
fsfn_LDFLAGS = $(all_libraries) 
noinst_HEADERS = alsasound.h acpihandler.h osd.h generics.h basicmsg.h
AM_CFLAGS = -Wall
