# DEFAULT GOAL TARGET
.DEFAULT_GOAL := all

# Compiler and linker
CXX := g++

# Debug
DEBUG_CXXFLAGS := -Og -D _DEBUG -ggdb
DEBUG_OBJDIR_PREFIX := debug

# Release
RELEASE_CXXFLAGS := -O3
RELEASE_OBJDIR_PREFIX := release

CURRENT_CXXFLAGS := $(DEBUG_CXXFLAGS)
CURRENT_OBJDIR_PREFIX := $(DEBUG_OBJDIR_PREFIX)
BUILD_TYPE_INFO := ****** Build Type=Debug ******

ifdef __build_type
	ifeq ($(__build_type), release)
		CURRENT_CXXFLAGS = $(RELEASE_CXXFLAGS)
		CURRENT_OBJDIR_PREFIX = $(RELEASE_OBJDIR_PREFIX)
		BUILD_TYPE_INFO = ****** Build Type=Release ******
	endif
endif

# -fpermissive -Wall -Wextra
CXXFLAGS := $(CURRENT_CXXFLAGS) -fpermissive -fsigned-char -D GGAUTH701_EXPORTS -fPIC -std=c++20 -fvisibility=hidden

# /usr/lib/x86_64-linux-gnu/libglib-2.0.so.0.6400.6 /usr/lib/libmsodbcsql-17.so
CXXLINK := -Wl,--export-dynamic -Wl,--no-undefined -pthread -ggdb -std=c++20 -shared

# set library static owner to check modify and recompile program
#LIBSTATIC := testlib.a

# Program name
PROGRAM := ggauth701.so

# Directory Output program App
DIRSAVEPRROGRAM := ./ggauth70-1

# Directory to Copy Lib
GAMESERVERDIR 		:= ./Game\ Server

# The build folder, for all generated output. This should normally be included
# in a .gitignore rule
BUILD_FOLDER := output-ggauth70-$(CURRENT_OBJDIR_PREFIX)

# my addiction
CFILES := \
	../ggauth70-1/ggauth70-1/dllmain.cpp \
	../ggauth70-1/ggauth70-1/ggauth70-1.cpp \
	../ggauth70-1/ggauth70-1/Protocol70.cpp \
	../Projeto|IOCP/UTIL/exception.cpp \
	../Projeto|IOCP/UTIL/message_pool.cpp \
	../Projeto|IOCP/UTIL/message.cpp \
	../Projeto|IOCP/TIMER/queue_timer.cpp \
	../Projeto|IOCP/UTIL/event.cpp \
	../Projeto|IOCP/THREAD|POOL/thread.cpp \
	../Projeto|IOCP/UTIL/WinPort.cpp \
	../Projeto|IOCP/THREAD|POOL/job_pool.cpp \
	../Projeto|IOCP/THREAD|POOL/job.cpp \
	../Projeto|IOCP/TIMER/timer.cpp \
	../Projeto|IOCP/TIMER/timer_manager.cpp \
	../Projeto|IOCP/UTIL/util_time.cpp \
	../Projeto|IOCP/UTIL/hex_util.cpp

target = $(BUILD_FOLDER)/$(subst |,\ ,$(filter %.o,$(patsubst %.cpp,%.o,${1}) $(patsubst %.c,%.o,${1})))
target_dep = $(BUILD_FOLDER)/$(subst |,\ ,$(filter %.o.d,$(patsubst %.cpp,%.o.d,${1}) $(patsubst %.c,%.o.d,${1})))
obj.cpp :=
obj.c :=
define obj
 $(call target,$(notdir ${1})): $(subst |,\ ,${1}) | ${BUILD_FOLDER}
 obj$(suffix ${1}) += $(call target,$(notdir ${1}))
endef

define SOURCES
 $(foreach src,${1},$(eval $(call obj,${src})))
endef

$(eval $(call SOURCES,${CFILES}))

define obj_link
	$(foreach src,${CFILES},$(call target,$(notdir ${src})))
endef

define dep_link
	$(foreach src,${CFILES},$(call target_dep,$(notdir ${src})))
endef

# scape white space
space := $(subst ,, )

define scape_WS
 $(subst $(space),\ ,${1})
endef

# Default all rule will build the 'example' target, which here is an executable
.PHONY:
all: $(BUILD_FOLDER)/$(PROGRAM)

# Show Info Build Type
$(info $(BUILD_TYPE_INFO))

# Generate a list of .o files from the .c files. Prefix them with the build
# folder to output the files there
OBJ_FILES := $(call obj_link)
#$(addprefix $(BUILD_FOLDER)/,$(SRC_FILES:.cpp=.o))

# Generate a list of depfiles, used to track includes. The file name is the same
# as the object files with the .d extension added
DEP_FILES := $(call dep_link)
#$(addsuffix .d,$(OBJ_FILES))

# Flags to generate the .d dependency-tracking files when we compile.  It's
# named the same as the target file with the .d extension
DEPFLAGS = -MMD -MP -MF $(call scape_WS,$@).d -MT '$(strip $(call scape_WS,$@)) $(strip $(call scape_WS,$@)).d'

# Include the dependency tracking files
-include $(DEP_FILES)

# List of include dirs. These are put into CFLAGS.
#INCLUDE_DIRS = \
#    src/

# Prefix the include dirs with '-I' when passing them to the compiler
#CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

# Set some compiler flags we need. Note that we're appending to the CFLAGS
# variable
#    -Wall \
#    -Werror 
CFLAGS += $(CXXFLAGS)

# Our project requires some linker flags: garbage collect sections, output a
# .map file
#LDFLAGS += 

# Set LDLIBS to specify linking with libm, the math library
LDLIBS += $(CXXLINK)

# The rule for compiling the SRC_FILES into OBJ_FILES
${obj.cpp} ${obj.c}: %.o :
	@echo Compiling $(CXX) $(CFLAGS) -c $(call scape_WS,$<) -o $(call scape_WS,$@)
	@$(CXX) $(CFLAGS) $(DEPFLAGS) -c $(call scape_WS,$<) -o $(call scape_WS,$@)

# Make Objects to link
OUT_OBJECTS = $(subst .a\,.a, $(subst .o\,.o,$(addsuffix \,$^)))

# The rule for building the executable "example", using OBJ_FILES as
# prerequisites. Since we're not relying on an implicit rule, we need to
# explicity list CFLAGS, LDFLAGS, LDLIBS
$(BUILD_FOLDER)/$(PROGRAM): $(OBJ_FILES) $(LIBSTATIC)
	@echo Linking $(notdir $@)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OUT_OBJECTS) $(LDLIBS) -o $@
	cp $(BUILD_FOLDER)/$(PROGRAM) $(GAMESERVERDIR)/$(PROGRAM)

# Remove debug information for a smaller executable. An embedded project might
# instead using [arm-none-eabi-]objcopy to convert the ELF file to a raw binary
# suitable to be written to an embedded device
STRIPPED_OUTPUT := $(BUILD_FOLDER)/$(PROGRAM)-stripped

$(STRIPPED_OUTPUT): $(BUILD_FOLDER)/$(PROGRAM)
	@echo Stripping $(notdir $@)
	objcopy --strip-debug $^ $@
	cp $@ $(GAMESERVERDIR)/$(PROGRAM)
	cp $@ $(MESSAGESERVERDIR)/$(PROGRAM)

# make build folder
$(BUILD_FOLDER):
	@mkdir -p $@

# Since all our generated output is placed into the build folder, our clean rule
# is simple. Prefix the recipe line with '-' to not error if the build folder
# doesn't exist (the -f flag for rm also has this effect)
.PHONY: clean
clean:
	- find $(BUILD_FOLDER)/ -name "*.[od]" -exec rm -rf {} \;
