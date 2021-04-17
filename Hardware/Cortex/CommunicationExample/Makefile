
HARDWARE_DIR := ../..
TOOLCHAIN_PATH := $(HARDWARE_DIR)/Toolchain

LIB_SOURCE_DIRS := ./src/io ./src/sercom ./src/util ./src/compatability
EXE_SOURCE_DIRS := ./Hardware ./src
PUBLIC_INTERFACE_DIRS := ./src/compatability
ADDITIONAL_INCLUDE_DIRS := ./Hardware/SAMD21/samd21c/include ./Hardware/CMSIS/Core/Include $(HARDWARE_DIR)/Common

BIN_DIR := ./bin
LIB_DIR := ./lib
INC_DIR := ./include

CXX_SOURCE_EXTENSION := cpp
C_SOURCE_EXTENSION := c

LINK_SCRIPT := ./Hardware/samd21e15b_flash.ld

PROJECT_NAME := CortexCommunicationExample

######## YOU PROBABLY DON'T NEED TO CHANGE ANYTHING BELOW THIS LINE ########

SOURCE_DIRS := $(LIB_SOURCE_DIRS) $(EXE_SOURCE_DIRS)

COMPILER_PATH = $(TOOLCHAIN_PATH)/gcc-arm-none-eabi/bin

CXX := $(COMPILER_PATH)/arm-none-eabi-g++
CC := $(COMPILER_PATH)/arm-none-eabi-gcc
AR := $(COMPILER_PATH)/arm-none-eabi-ar
OBJ_COPY := $(COMPILER_PATH)/arm-none-eabi-objcopy
PRINT_SIZE := $(COMPILER_PATH)/arm-none-eabi-size

ELF := $(BIN_DIR)/$(PROJECT_NAME).elf
BIN := $(BIN_DIR)/$(PROJECT_NAME).bin
LIB := $(LIB_DIR)/$(PROJECT_NAME).a

# Directory for storing intermediate files
C_BUILD_DIR := .$(notdir $(CC))
CXX_BUILD_DIR := .$(notdir $(CXX))

# Colors for console output
ANSI_GREEN := \033[1;92m
ANSI_DEFAULT_COLOR:= \033[m

# Specify default goal
.DEFAULT_GOAL := all

#Find source files
LIB_CXX_SOURCES := $(foreach dir, $(LIB_SOURCE_DIRS), $(wildcard $(dir)/*.$(CXX_SOURCE_EXTENSION)))
LIB_CXX_OBJ := $(patsubst %.$(CXX_SOURCE_EXTENSION), $(CXX_BUILD_DIR)/%.o, $(LIB_CXX_SOURCES))

LIB_C_SOURCES := $(foreach dir, $(LIB_SOURCE_DIRS), $(wildcard $(dir)/*.$(C_SOURCE_EXTENSION)))
LIB_C_OBJ := $(patsubst %.$(C_SOURCE_EXTENSION), $(C_BUILD_DIR)/%.o, $(LIB_C_SOURCES))

EXE_CXX_SOURCES := $(foreach dir, $(EXE_SOURCE_DIRS), $(wildcard $(dir)/*.$(CXX_SOURCE_EXTENSION)))
EXE_CXX_OBJ := $(patsubst %.$(CXX_SOURCE_EXTENSION), $(CXX_BUILD_DIR)/%.o, $(EXE_CXX_SOURCES))

EXE_C_SOURCES := $(foreach dir, $(EXE_SOURCE_DIRS), $(wildcard $(dir)/*.$(C_SOURCE_EXTENSION)))
EXE_C_OBJ := $(patsubst %.$(C_SOURCE_EXTENSION), $(C_BUILD_DIR)/%.o, $(EXE_C_SOURCES))

LIB_OBJ := $(LIB_CXX_OBJ) $(LIB_C_OBJ)
EXE_OBJ := $(EXE_CXX_OBJ) $(EXE_C_OBJ)

HEADERS_TO_MAKE_PUBLIC := $(foreach dir, $(PUBLIC_INTERFACE_DIRS), $(wildcard $(dir)/*.h))
PUBLIC_HEADERS := $(addprefix $(INC_DIR)/,$(notdir $(HEADERS_TO_MAKE_PUBLIC)))

ALL_OBJ := $(LIB_OBJ) $(EXE_OBJ)

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

C_FLAGS := $(OPT_FLAGS) -Wall $(INCLUDE_FLAGS) $(DEVICE_FLAGS) -std=gnu99 -ffunction-sections
CXX_FLAGS := $(OPT_FLAGS) -Wall $(INCLUDE_FLAGS) $(DEVICE_FLAGS) -std=gnu++17 -fno-exceptions -fno-rtti -ffunction-sections

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

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(INC_DIR):
	@mkdir $(INC_DIR)

$(PUBLIC_HEADERS) : $(HEADERS_TO_MAKE_PUBLIC) | $(INC_DIR)
	@printf "%b" "$(ANSI_GREEN)Outputting public includes...$(ANSI_DEFAULT_COLOR)\n"
	cp $(HEADERS_TO_MAKE_PUBLIC) $(INC_DIR)


$(LIB): $(LIB_OBJ) $(PUBLIC_HEADERS) | $(LIB_DIR)
	@printf "%b" "$(ANSI_GREEN)Packing static library $@...$(ANSI_DEFAULT_COLOR)\n"
	$(AR) rcs $@ $^

$(ELF): $(EXE_OBJ) $(LIB) | $(BIN_DIR)
	@printf "%b" "$(ANSI_GREEN)Linking ELF file $@...$(ANSI_DEFAULT_COLOR)\n"
	$(CXX) $^ $(LIB) $(LINK_FLAGS) -o $@

$(BIN): $(ELF) | $(BIN_DIR)
	@printf "%b" "$(ANSI_GREEN)Generating binary file $@...$(ANSI_DEFAULT_COLOR)\n"
	$(OBJ_COPY) -O binary $< $@

all: $(BIN)
	@printf "%b" "$(ANSI_GREEN)*** Success building $(PROJECT_NAME) ***$(ANSI_DEFAULT_COLOR)\n"
	@$(PRINT_SIZE) $(ELF)

program: $(BIN)
	@JLinkExe -commanderscript program.jlink

clean:
	rm -rf $(C_BUILD_DIR) $(CXX_BUILD_DIR) $(BIN_DIR) $(LIB_DIR) $(INC_DIR)

# Include auto-generated dependancy files
DEPENDANCY_FILES := $(shell find . -name "*.d" 2> /dev/null)
include $(DEPENDANCY_FILES)