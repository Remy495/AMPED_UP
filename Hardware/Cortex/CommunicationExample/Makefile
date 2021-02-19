
CXX := arm-none-eabi-g++
CC := arm-none-eabi-gcc
OBJ_COPY := arm-none-eabi-objcopy
PRINT_SIZE := arm-none-eabi-size

SOURCE_DIRS := ./Hardware ./src ./src/io ./src/sercom ./src/util
ADDITIONAL_INCLUDE_DIRS := ./Hardware/SAMD21/samd21c/include ./Hardware/CMSIS/Core/Include

BIN_DIR := ./bin

CXX_SOURCE_EXTENSION := cpp
C_SOURCE_EXTENSION := c

LINK_SCRIPT := ./Hardware/samd21e15b_flash.ld

PROJECT_NAME := CortexCommunicationExample

######## YOU PROBABLY DON'T NEED TO CHANGE ANYTHING BELOW THIS LINE ########

ELF := $(BIN_DIR)/$(PROJECT_NAME).elf
BIN := $(BIN_DIR)/$(PROJECT_NAME).bin

# Directory for storing intermediate files
C_BUILD_DIR := .$(CC)
CXX_BUILD_DIR := .$(CXX)

# Colors for console output
ANSI_GREEN := \033[1;92m
ANSI_DEFAULT_COLOR:= \033[m

# Specify default goal
.DEFAULT_GOAL := all

#Find source files
CXX_SOURCES := $(foreach dir, $(SOURCE_DIRS), $(wildcard $(dir)/*.$(CXX_SOURCE_EXTENSION)))
CXX_OBJ := $(patsubst %.$(CXX_SOURCE_EXTENSION), $(CXX_BUILD_DIR)/%.o, $(CXX_SOURCES))

C_SOURCES := $(foreach dir, $(SOURCE_DIRS), $(wildcard $(dir)/*.$(C_SOURCE_EXTENSION)))
C_OBJ := $(patsubst %.$(C_SOURCE_EXTENSION), $(C_BUILD_DIR)/%.o, $(C_SOURCES))

ALL_OBJ := $(CXX_OBJ) $(C_OBJ)
INCLUDE_DIRS := $(SOURCE_DIRS) $(ADDITIONAL_INCLUDE_DIRS)

# Compiler flags
DEBUG := 0

ifeq ($(DEBUG), 1)
OPT_FLAGS := -O0 -g
else
OPT_FLAGS := -O3
endif

INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))
DEVICE_FLAGS := -mthumb -D__SAMD21E15B__ -mcpu=cortex-m0plus

C_FLAGS := $(OPT_FLAGS) -Wall $(INCLUDE_FLAGS) $(DEVICE_FLAGS) -std=gnu99
CXX_FLAGS := $(OPT_FLAGS) -Wall $(INCLUDE_FLAGS) $(DEVICE_FLAGS) -std=gnu++17

LINK_SCRIPT_DIR := $(dir $(LINK_SCRIPT))
LINK_SCRIPT_NAME := $(notdir $(LINK_SCRIPT))
LINK_FLAGS := -Wl,--start-group -lm -Wl,--end-group -L$(LINK_SCRIPT_DIR) -Wl,--gc-sections -mthumb -mcpu=cortex-m0plus -T$(LINK_SCRIPT_NAME) -specs=nano.specs -specs=nosys.specs

# Compile C++ source files
$(CXX_BUILD_DIR)/%.o: %.cpp $(CXX_BUILD_DIR)/%.d
	@printf "%b" "$(ANSI_GREEN)Compiling source file $<...$(ANSI_DEFAULT_COLOR)\n"
	@mkdir -p $(dir $@)
	$(CXX) $< -c -MT $@ -MMD -MP -MF $(CXX_BUILD_DIR)/$*.d $(CXX_FLAGS) -o $@

# Compile C source files
$(C_BUILD_DIR)/%.o: %.c $(C_BUILD_DIR)/%.d
	@printf "%b" "$(ANSI_GREEN)Compiling source file $<...$(ANSI_DEFAULT_COLOR)\n"
	@mkdir -p $(dir $@)
	$(CC) $< -c -MT $@ -MMD -MP -MF $(C_BUILD_DIR)/$*.d $(C_FLAGS) -o $@

# Dependancy files are built at the same time as object files. Empty target for
# dependancy files used to trigger object files to be rebuilt if dependancy file is
# missing
%.d: ;

# Ensure dependancy files are not deleted as intermediate files
.PRECIOUS: %.d

.PHONY: all clean program

$(BIN_DIR):
	@mkdir $(BIN_DIR)

$(ELF): $(ALL_OBJ) | $(BIN_DIR)
	@printf "%b" "$(ANSI_GREEN)Linking ELF file $@...$(ANSI_DEFAULT_COLOR)\n"
	$(CXX) $^ $(LINK_FLAGS) -o $@

$(BIN): $(ELF) | $(BIN_DIR)
	@printf "%b" "$(ANSI_GREEN)Generating binary file $@...$(ANSI_DEFAULT_COLOR)\n"
	$(OBJ_COPY) -O binary $< $@

all: $(BIN)
	@printf "%b" "$(ANSI_GREEN)*** Success building Teensy debugging code ***$(ANSI_DEFAULT_COLOR)\n"
	@$(PRINT_SIZE) $(ELF)

program: $(BIN)
	@JLinkExe -commanderscript program.jlink

clean:
	rm -rf $(C_BUILD_DIR) $(CXX_BUILD_DIR) $(BIN_DIR)

# Include auto-generated dependancy files
DEPENDANCY_FILES := $(shell find . -name "*.d" 2> /dev/null)
include $(DEPENDANCY_FILES)