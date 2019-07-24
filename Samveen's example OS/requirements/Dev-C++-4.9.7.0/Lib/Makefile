# Makefile for win32 import libraries.
#
# If a library must contain some compiled code the name of the
# source file should be named as the corresponding .def file with a
# .c extension.
#
# NEEDS GNU MAKE!

DLLTOOL=dlltool
CC=gcc
AR=ar
AR_FLAGS=rs
RM=rm -f

lib%.a : %.def %.o
	$(DLLTOOL) -k --output-lib $@ -d $*.def
	$(AR) $(AR_FLAGS) $@ $*.o

lib%.a : %.def
	$(DLLTOOL) -k --output-lib $@ --def $<

.c.o:
	$(CC) -I$(INCDIR) -c -o $@ $<

INCDIR = ../include
DEFS := $(wildcard *.def)
IMPLIBS := $(subst .def,.a,$(DEFS))
IMPLIBS := $(addprefix lib,$(IMPLIBS))

EXTRA_LIBS=libuuid.a
EXTRA_OBJS=shell32.o
GUID_OBJS=uuid.o

all: $(IMPLIBS) $(EXTRA_LIBS) $(EXTRA_OBJS)

libuuid.a: $(GUID_OBJS)
	$(AR) $(AR_FLAGS) $@ $(GUID_OBJS)

echo:
	@echo $(IMPLIBS)

clean:
	$(RM) $(IMPLIBS) $(EXTRA_LIBS) *.o *.s
