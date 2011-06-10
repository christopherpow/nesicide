### NESICIDE generated makefile for 
### 
### <!project-name!>

# Toolchain stuff.
COMPILE  := cc65
ASSEMBLE := ca65
LINK     := ld65

# Environment stuff.
RM       := rm

# Path(s) to additional libraries required for linking the program
# Use only if you don't want to place copies of the libraries in SRCDIR
# Default: none
LIBS    :=

# Get rid of Windows path wonkiness.
LIBS := $(subst \,/,$(LIBS))

# Custom linker configuration file
# Use only if you don't want to place it in SRCDIR
# Default: none
CONFIG  := <!linker-config!>

# Get rid of Windows path wonkiness.
CONFIG := $(subst \,/,$(CONFIG))

# Additional C compiler flags and options.
# Default: none
CFLAGS  = <!compiler-flags!>

# Get rid of Windows path wonkiness.
CFLAGS := $(subst \,/,$(CFLAGS))

# Additional assembler flags and options.
# Default: none
ASFLAGS = -g -v --debug-info <!assembler-flags!>

# Get rid of Windows path wonkiness.
ASFLAGS := $(subst \,/,$(ASFLAGS))

# Additional linker flags and options.
# Default: none
LDFLAGS = -C $(CONFIG) --dbgfile <!debug-file!> <!linker-flags!>

# Get rid of Windows path wonkiness.
LDFLAGS := $(subst \,/,$(LDFLAGS))

# Path to the directory where object files are to be stored (inside respective target subdirectories).
# Default: obj
OBJDIR := <!object-dir!>

# Get rid of Windows path wonkiness.
OBJDIR := $(subst \,/,$(OBJDIR))

# Program ROM file name (game code goes here).
PROGRAM = $(OBJDIR)/<!prg-rom-name!>

# Set SOURCES to something like 'src/foo.c src/bar.s'.
SOURCES := <!clang-sources!>
SOURCES += <!asm-sources!>

# Get rid of Windows path wonkiness.
SOURCES := $(subst \,/,$(SOURCES))

# Set OBJECTS to something like 'obj/foo.o obj/bar.o'.
OBJECTS := $(addsuffix .o,$(basename $(addprefix $(OBJDIR)/,$(notdir $(SOURCES)))))

# Set DEPENDS to something like 'obj/foo.d obj/bar.d'.
DEPENDS := $(OBJECTS:.o=.d)

.SUFFIXES:
.PHONY: all clean

all: $(PROGRAM)

-include $(DEPENDS)

# The remaining targets.
$(OBJDIR):
	mkdir -p $@

vpath %c $(foreach c,$(SOURCES),$(dir $c))

$(OBJDIR)/%.o: %.c
	$(COMPILE) --create-dep $(@:.o=.d) $(CFLAGS) -o $@ $<

vpath %s $(foreach s,$(SOURCES),$(dir $s))

$(OBJDIR)/%.o: %.s | $(OBJDIR)
	$(ASSEMBLE) --create-dep $(@:.o=.d) $(ASFLAGS) -o $@ $<

$(PROGRAM): $(CONFIG) $(OBJECTS) $(LIBS)
	$(LINK) -o $@ $(LDFLAGS) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(REMOVES)
	$(RM) $(PROGRAM)
