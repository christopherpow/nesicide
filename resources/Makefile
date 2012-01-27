### NESICIDE generated makefile for 
### 
### <!project-name!>

# Toolchain stuff.
COMPILE  := cl65
ASSEMBLE := ca65 
LINK     := cl65

# Environment stuff.
RM = rm

ifeq ($(shell echo),)
  MKDIR = mkdir -p $1
  RMDIR = rmdir $1
  RMFILES = $(RM) $1
else
  MKDIR = mkdir $(subst /,\,$1)
  RMDIR = rmdir $(subst /,\,$1)
  RMFILES = $(if $1,del /f $(subst /,\,$1))
endif

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
CFLAGS  = -t nes -g --debug-info <!compiler-flags!>

# Get rid of Windows path wonkiness.
CFLAGS := $(subst \,/,$(CFLAGS))

# Additional assembler flags and options.
# Default: none
ASFLAGS = -t nes -g --debug-info <!assembler-flags!>

# Get rid of Windows path wonkiness.
ASFLAGS := $(subst \,/,$(ASFLAGS))

# Additional linker flags and options.
# Default: none
LDFLAGS = -t nes -C $(CONFIG) <!linker-flags!> -Wl --dbgfile,<!debug-file!>
REMOVES += <!debug-file!>

# Get rid of Windows path wonkiness.
LDFLAGS := $(subst \,/,$(LDFLAGS))

# Path to the directory where object files are to be stored.
OBJDIR := <!object-dir!>

# Get rid of Windows path wonkiness.
OBJDIR := $(subst \,/,$(OBJDIR))

# Path to the directory where PRG files are to be stored.
PRGDIR := <!prg-dir!>

# Get rid of Windows path wonkiness.
PRGDIR := $(subst \,/,$(PRGDIR))

# Program ROM file name (game code goes here).
PROGRAM = $(PRGDIR)/<!prg-rom-name!>

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

# CPTODO: Disabled for now because of Windows crap paths
#-include $(DEPENDS)

# The remaining targets.
$(OBJDIR):
	$(call MKDIR,@)

<!target-rules!>

$(PROGRAM): $(CONFIG) $(OBJECTS) $(LIBS)
	$(LINK) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@ <!linker-dependencies!>

clean:
	-$(call RMFILES,$(OBJECTS))
	-$(call RMFILES,$(DEPENDS))
	-$(call RMFILES,$(REMOVES))
	-$(call RMFILES,$(PROGRAM))
