# Simple Makefile.

# Main compiler
CC := i686-w64-mingw32-g++

SRCDIR := src
BUILDDIR := build
EXTERNALDIR := external
TARGETDIR := bin
TARGET := $(TARGETDIR)/meeting-countdown.exe

EXT_OBJECTS := $(BUILDDIR)/SDL2_gfx/SDL2_gfxPrimitives.o \
	$(BUILDDIR)/SDL2_gfx/SDL2_rotozoom.o

SRCEXT := cc
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o)) \
	$(EXT_OBJECTS)

CFLAGS := -w -Wall
LIB := -L/usr/lib\
	-L/usr/bin\
	-static\
       	-lmingw32\
       	-lSDL2main\
       	-lSDL2\
	-lSDL2_ttf\
	-luser32\
	-lgdi32\
	-lwinmm\
	-limm32\
	-lwinspool\
	-lshell32\
	-lole32\
	-loleaut32\
	-lversion\
	-luuid\
	-lcomdlg32\
	-lsetupapi\
	-lhid\
	-lfreetype\
	-static-libgcc

INC := -I/usr/include \
	-I/usr/i686-w64-ming32/include \
	-I$(EXTERNALDIR)

$(TARGET): $(OBJECTS)
	@echo "===== Linking..."
	@mkdir -p $(TARGETDIR)
	@echo "$(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Compile SDL2_gfx external library
$(BUILDDIR)/SDL2_gfx/%.o: $(EXTERNALDIR)/SDL2_gfx/%.c
	@mkdir -p $(BUILDDIR)/SDL2_gfx
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "===== Cleaning..."
	@echo "$(RM) -r $(BUILDDIR) $(TARGETDIR)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
