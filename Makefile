LIB=mkt

# dependencies
L_KUT = lib/libkut
I_KUT = lib/libkut/include


# compile
CFILES = src/$(LIB)/%.c
HFILES = include/$(LIB)/%.h
FILES = $(CFILES) $(HFILES)

#obj/%.o : $(FILES)
#	if [ ! -e obj ];then mkdir obj;fi
#	gcc $(CFLAGS) -c $< -o $@ \
#	  -Iinclude -I$(I_KUT) \
#		-L$(L_KUT) -lkut \
#		-lgc -lm


# archive
OFILES = $(subst src/$(LIB),obj,$(patsubst %.c,%.o,$(wildcard src/$(LIB)/*.c)))
lib/lib$(LIB).a : $(OFILES)
	if [ ! -e lib ];then mkdir lib;fi
	ar rcs lib/lib$(LIB).a $(OFILES)

# compile
CFLAGS = -Wall -rdynamic
obj/%.o : $(FILES)
	if [ ! -e obj ];then mkdir obj;fi
	gcc $(CFLAGS) -c $< -o $@ \
	  -Iinclude -I$(I_KUT) \
		-L$(L_KUT) -lkut
