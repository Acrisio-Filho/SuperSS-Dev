# DEFAULT GOAL TARGET
.DEFAULT_GOAL := all

# Compiler and linker
CXX := g++-10

# Debug
DEBUG_CXXFLAGS := -Og -D _DEBUG -D SHOW_DEBUG_PACKET -ggdb
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
CXXFLAGS := $(CURRENT_CXXFLAGS) -fpermissive -fsigned-char -std=c++20 `pkg-config --cflags glib-2.0`

# /usr/lib/x86_64-linux-gnu/libglib-2.0.so.0.6400.6 /usr/lib/libmsodbcsql-17.so
CXXLINK := -pthread -lg -ldl `pkg-config --libs libzip` `pkg-config --libs mysqlclient` -lodbc -lcrypto `pkg-config --libs glib-2.0`

# set library static owner to check modify and recompile program
#LIBSTATIC := testlib.a

# Program name
PROGRAM := login

# Directory Output program App
DIRSAVEPRROGRAM := ./Login\ Server

# The build folder, for all generated output. This should normally be included
# in a .gitignore rule
BUILD_FOLDER := output-ls-$(CURRENT_OBJDIR_PREFIX)

# my addiction
CFILES := \
	../Login|Server/Login|Server/Login|Server.cpp \
	../Projeto|IOCP/UTIL/exception.cpp \
	../Projeto|IOCP/UTIL/message_pool.cpp \
	../Projeto|IOCP/UTIL/message.cpp \
	../Projeto|IOCP/TIMER/queue_timer.cpp \
	../Projeto|IOCP/UTIL/event.cpp \
	../Projeto|IOCP/THREAD|POOL/thread.cpp \
	../Projeto|IOCP/UTIL/WinPort.cpp \
	../Projeto|IOCP/UTIL/reader_ini.cpp \
	../Projeto|IOCP/UTIL/util_time.cpp \
	../Projeto|IOCP/THREAD|POOL/job_pool.cpp \
	../Projeto|IOCP/THREAD|POOL/job.cpp \
	../Projeto|IOCP/TIMER/timer.cpp \
	../Projeto|IOCP/TIMER/timer_manager.cpp \
	../Projeto|IOCP/UTIL/hex_util.cpp \
	../Projeto|IOCP/THREAD|POOL/threadpool.cpp \
	../Projeto|IOCP/IOCP/epoll.cpp \
	../Projeto|IOCP/UTIL/buffer.cpp \
	../Projeto|IOCP/SOCKET/session.cpp \
	../Projeto|IOCP/PACKET/packet.cpp \
	../Projeto|IOCP/COMPRESS/compress.cpp \
	../Projeto|IOCP/COMPRESS/minilzo.c \
	../Projeto|IOCP/CRYPT/crypt.cpp \
	../Projeto|IOCP/Server/server.cpp \
	../Projeto|IOCP/THREAD|POOL/threadpl_server.cpp \
	../Projeto|IOCP/DATABASE/normal_manager_db.cpp \
	../Projeto|IOCP/DATABASE/normal_db.cpp \
	../Projeto|IOCP/DATABASE/response.cpp \
	../Projeto|IOCP/DATABASE/result_set.cpp \
	../Projeto|IOCP/SOCKET/socketserver.cpp \
	../Projeto|IOCP/SOCKET/socket.cpp \
	../Projeto|IOCP/PANGYA_DB/pangya_db.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_register_server.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_server_list.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_list_ip_ban.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_list_mac_ban.cpp \
	../Projeto|IOCP/SOCKET/session_manager.cpp \
	../Projeto|IOCP/PACKET/packet_func.cpp \
	../Projeto|IOCP/UTIL/func_arr.cpp \
	../Projeto|IOCP/Smart|Calculator/Smart|Calculator.cpp \
	../Projeto|IOCP/DATABASE/exec_query.cpp \
	../Projeto|IOCP/DATABASE/database.cpp \
	../Projeto|IOCP/UNIT/unit_auth_server_connect.cpp \
	../Projeto|IOCP/UNIT/unit_connect.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_new_auth_server_key.cpp \
	../Projeto|IOCP/DATABASE/mssql.cpp \
	../Projeto|IOCP/DATABASE/postgresql.cpp \
	../Projeto|IOCP/DATABASE/mysql_db.cpp \
	../Login|Server/Login|Server/login_server.cpp \
	../Projeto|IOCP/UTIL/md5.cpp \
	../Projeto|IOCP/UTIL/iff.cpp \
	../Login|Server/SESSION/player_manager.cpp \
	../Login|Server/PACKET/packet_func_ls.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_register_logon.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_verify_id.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_verify_pass.cpp \
	../Login|Server/PANGYA_DB/cmd_player_info.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_logon_check.cpp \
	../Login|Server/PANGYA_DB/cmd_first_set_check.cpp \
	../Login|Server/PANGYA_DB/cmd_verify_ip.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_insert_block_ip.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_insert_block_mac.cpp \
	../Login|Server/PANGYA_DB/cmd_first_login_check.cpp \
	../Login|Server/PANGYA_DB/cmd_create_user.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_auth_key_login_info.cpp \
	../Login|Server/PANGYA_DB/cmd_register_logon_server.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_save_nick.cpp \
	../Login|Server/PANGYA_DB/cmd_add_first_login.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_auth_key_game.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_verify_nick.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_add_character.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_auth_key_login.cpp \
	../Login|Server/PANGYA_DB/cmd_add_first_set.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_update_character_equiped.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_chat_macro_user.cpp \
	../Login|Server/PANGYA_DB/cmd_register_player_login.cpp \
	../Projeto|IOCP/PANGYA_DB/cmd_add_item_base.cpp \
	../Login|Server/SESSION/player.cpp \
	../Login|Server/TYPE/player_info.cpp

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
	cp $(BUILD_FOLDER)/$(PROGRAM) $(DIRSAVEPRROGRAM)/$(PROGRAM)

# Remove debug information for a smaller executable. An embedded project might
# instead using [arm-none-eabi-]objcopy to convert the ELF file to a raw binary
# suitable to be written to an embedded device
STRIPPED_OUTPUT := $(BUILD_FOLDER)/$(PROGRAM)-stripped

$(STRIPPED_OUTPUT): $(BUILD_FOLDER)/$(PROGRAM)
	@echo Stripping $(notdir $@)
	objcopy --strip-debug $^ $@
	cp $@ $(DIRSAVEPRROGRAM)/$(PROGRAM)

# make build folder
$(BUILD_FOLDER):
	@mkdir -p $@

# Since all our generated output is placed into the build folder, our clean rule
# is simple. Prefix the recipe line with '-' to not error if the build folder
# doesn't exist (the -f flag for rm also has this effect)
.PHONY: clean
clean:
	- find $(BUILD_FOLDER)/ -name "*.[od]" -exec rm -rf {} \;