LIB=mkt

# dependencies
L_KUT = ../lib/libkut
I_KUT = ../lib/libkut/include

# compile bin
CFILES = $(wildcard src/*.c)
HFILES = $(wildcard include/*.h)
FILES = $(CFILES) $(HFILES)

CFLAGS = -Wall -rdynamic
bin/tests : $(FILES) ../lib/lib$(LIB).a
	gcc $(CFLAGS) $(CFILES) -o bin/tests \
		-Iinclude -I../include -I$(I_KUT) \
		-L../lib -lmkt \
		-L$(L_KUT) -lkut \
		-lgc -lm

# compile lib
CLIB_FILES = $(wildcard ../src/$(LIB)/*.c)
HLIB_FILES = $(wildcard ../include/$(LIB)/*.h)
LIB_FILES = $(CLIB_FILES) $(HLIB_FILES)

OFILES = $(subst src/$(LIB),obj,$(patsubst %.c,%.o,$(wildcard ../src/$(LIB)/*.c)))

.ONESHELL:
../lib/lib$(LIB).a : $(LIB_FILES)
	cd ..
	make
