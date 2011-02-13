CC=$(CROSS)gcc -c -g -Wall -Werror -ansi -pedantic $(DEFINES) -I./include
GLLIBS=-lglut -lGLU -lGL
CCLD=$(CROSS)gcc
BIN=modelviewer
LIB=lib/libobjview.a

OBJECTS+=main.o

LOBJECTS+=lib/objload.o
LOBJECTS+=lib/xmalloc.o
LOBJECTS+=lib/tga.o
LOBJECTS+=lib/hash.o

$(BIN): $(OBJECTS) $(LIB)
	@echo "    CCLD $@"
	@$(CCLD) -L./lib $(OBJECTS) -o $@ $(GLLIBS) -lobjview

$(LIB): $(LOBJECTS)
	@echo "    AR $@"
	@ar -cq $(LIB) $(LOBJECTS)
	@ranlib $(LIB)

cross:
	make CROSS=i686-pc-mingw32- GLLIBS="-lglu32 -lglut -lopengl32" DEFINES=-DMINGW BIN=modelviewer.exe

%.o: %.c .d/%.d
	@echo "    CC $@"
	@$(CC) $<

lib/%.o: lib/%.c .d/lib/%.d
	@echo "    CC $@"
	@$(CC) $< -o $@

.d/%.d: %.c
	@$(CC) -MM $< -MF $@

-include $(OBJECTS:%.o=.d/%.d)
-include $(LOBJECTS:%.o=.d/%.d)

clean:
	rm -f lib/*.a lib/*.o *.o modelviewer
