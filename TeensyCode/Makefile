
################################################################
#  Makefile for use during testing. Compiles C++ code for X86  #
################################################################

CC := clang++

SOURCE_DIRS := . nodes
ADDITIONAL_INCLUDE_DIRS := generated

EXE := DebugTeensy.exe

######## YOU PROBABLY DON'T NEED TO CHANGE ANYTHING BELOW THIS LINE ########

# Directory for storing intermediate files
BUILD_DIR := .$(CC)

# Colors for console output
ANSI_GREEN := \033[1;92m
ANSI_DEFAULT_COLOR:= \033[m

# Specify default goal
.DEFAULT_GOAL := all

#Find source files
CXX_SOURCES := $(foreach dir, $(SOURCE_DIRS), $(wildcard $(dir)/*.cpp))
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CXX_SOURCES))
INCLUDE_DIRS := $(SOURCE_DIRS) $(ADDITIONAL_INCLUDE_DIRS)

# Compiler flags
DEBUG := 1

ifeq ($(DEBUG), 1)
OPT_FLAGS := -O0 -g
else
OPT_FLAGS := -O3 
endif

INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))

CXX_FLAGS := $(OPT_FLAGS) -Wall $(INCLUDE_FLAGS)

# Compile source files
$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)/%.d
	@printf "%b" "$(ANSI_GREEN)Compiling source file $<...$(ANSI_DEFAULT_COLOR)\n"
	@mkdir -p $(dir $@)
	$(CC) $< -c -std=c++14 -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.d $(CXX_FLAGS) -o $@

# Dependancy files are built at the same time as object files. Empty target for
# dependancy files used to trigger object files to be rebuilt if dependancy file is
# missing
%.d: ;

# Ensure dependancy files are not deleted as intermediate files
.PRECIOUS: %.d

.PHONY: all clean

$(EXE): $(OBJ_FILES)
	@printf "%b" "$(ANSI_GREEN)Linking executable $@...$(ANSI_DEFAULT_COLOR)\n"
	$(CC) $^ -o $@

all: $(EXE)
	@printf "%b" "$(ANSI_GREEN)*** Success building Teensy debugging code ***$(ANSI_DEFAULT_COLOR)\n"

clean:
	rm -rf $(BUILD_DIR) $(EXE)

# Include auto-generated dependancy files
DEPENDANCY_FILES := $(shell find . -name "*.d" 2> /dev/null)
include $(DEPENDANCY_FILES)