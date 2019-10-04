# Simple Makefile.

# Main compiler
CC := i686-w64-mingw32-g++
WINDRES := i686-w64-mingw32-windres

SRCDIR := src
BUILDDIR := build
EXTERNALDIR := external
TARGETDIR := bin
TARGET := $(TARGETDIR)/meeting-countdown.exe

EXT_OBJECTS := $(BUILDDIR)/SDL2_gfx/SDL2_gfxPrimitives.o \
	$(BUILDDIR)/SDL2_gfx/SDL2_rotozoom.o \
	$(BUILDDIR)/AHEasing/easing.o

SRCEXT := cc
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

# .res file is for windows exe meta information, like icon
RESSRC := windres/winapp.rc
RESOBJECT := windres/winapp.res

OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o)) \
	$(EXT_OBJECTS) $(RESOBJECT)

CFLAGS := -w -Wall

# Read version from the latest git tag
GIT_VERSION := "$(shell git describe --tags --always)"

# Provide version with -D flag
DFLAGS := -DVERSION=\"$(GIT_VERSION)\"

# Note: ordering is important!
LIB := -L/usr/local/lib\
	-L/usr/local/bin\
	-L/usr/i686-w64-mingw32/lib\
	-static\
       	-lmingw32\
       	-lSDL2main\
       	-lSDL2\
	-lSDL2_ttf\
	-lfreetype\
	-lpng\
	-lz\
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
	-static-libgcc

INC := -I/usr/i686-w64-mingw32/include \
	-I/usr/local/include \
	-I/usr/include \
	-I$(EXTERNALDIR)

$(TARGET): $(OBJECTS)
	@echo "===== Linking..."
	@mkdir -p $(TARGETDIR)
	@echo "$(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CC) $(CFLAGS) $(INC) $(DFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) $(DFLAGS) -c -o $@ $<

# Compile SDL2_gfx external library
$(BUILDDIR)/SDL2_gfx/%.o: $(EXTERNALDIR)/SDL2_gfx/%.c
	@mkdir -p $(BUILDDIR)/SDL2_gfx
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Compile easing
$(BUILDDIR)/AHEasing/%.o: $(EXTERNALDIR)/AHEasing/%.c
	@mkdir -p $(BUILDDIR)/AHEasing
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Use windres to compile Windows resource file with icon
$(RESOBJECT): $(RESSRC)
	@echo "$(WINDRES) $< -O coff -o $@"; $(WINDRES) $< -O coff -o $@

clean:
	@echo "===== Cleaning..."
	@echo "$(RM) -r $(BUILDDIR) $(TARGETDIR)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
