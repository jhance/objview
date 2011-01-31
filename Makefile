CC=gcc -c -g
CCLD=gcc -lglut -lGLU -lGL

OBJECTS+=main.o
OBJECTS+=objload.o
OBJECTS+=xmalloc.o
OBJECTS+=tga.o
OBJECTS+=hash.o

modelviewer: $(OBJECTS)
	@echo "    CCLD $*"
	@$(CCLD) $(OBJECTS) -o $@

main.o: main.c objload.h xmalloc.h
objload.o: objload.c objload.h xmalloc.h tga.h hash.h
xmalloc.o: xmalloc.c xmalloc.h
tga.o: tga.c xmalloc.h
hash.o: hash.c hash.h tga.h xmalloc.h

%.o: %.c
	@echo "    CC $<"
	@$(CC) $<
