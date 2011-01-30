CC=gcc -c -g
CCLD=gcc -lglut -lGLU

OBJECTS+=main.o
OBJECTS+=objload.o
OBJECTS+=xmalloc.o

modelviewer: $(OBJECTS)
	@echo "    CCLD $*"
	@$(CCLD) $(OBJECTS) -o $@

main.o: main.c objload.h xmalloc.h
objload.o: objload.c objload.h xmalloc.h
xmalloc.o: xmalloc.h

%.o: %.c
	@echo "    CC $<"
	@$(CC) $<
