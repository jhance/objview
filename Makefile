CC=$(CROSS)gcc -c -g -Wall -Werror -ansi -pedantic $(DEFINES)
GLLIBS=-lglut -lGLU -lGL
CCLD=$(CROSS)gcc
BIN=modelviewer

OBJECTS+=main.o
OBJECTS+=objload.o
OBJECTS+=xmalloc.o
OBJECTS+=tga.o
OBJECTS+=hash.o

$(BIN): $(OBJECTS)
	@echo "    CCLD $*"
	$(CCLD) $(OBJECTS) -o $@ $(GLLIBS)

cross:
	make CROSS=i686-pc-mingw32- GLLIBS="-lglu32 -lglut -lopengl32" DEFINES=-DMINGW BIN=modelviewer.exe


main.o: main.c objload.h xmalloc.h
objload.o: objload.c objload.h xmalloc.h tga.h hash.h
xmalloc.o: xmalloc.c xmalloc.h
tga.o: tga.c xmalloc.h
hash.o: hash.c hash.h tga.h xmalloc.h

%.o: %.c
	@echo "    CC $<"
	@$(CC) $<

clean:
	rm -f *.o modelviewer
