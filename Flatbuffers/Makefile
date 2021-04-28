
##############################################################
#  Makefile to generate Flatbuffers code for GUI and Teensy  #
##############################################################

ANSI_GREEN := \033[1;92m
ANSI_DEFAULT_COLOR:= \033[m

PROJECT_ROOT_DIR := ./..
CPP_OUTPUT_DIR := $(PROJECT_ROOT_DIR)/Hardware/Teensy/src/generated
PYTHON_OUTPUT_DIR := $(PROJECT_ROOT_DIR)/GUI/Python

CPP_FLAGS := --cpp --scoped-enums -o $(CPP_OUTPUT_DIR)
PYTHON_FLAGS := --python -o $(PYTHON_OUTPUT_DIR)

INPUT_FILES := $(wildcard *.fbs)
CPP_OUTPUT_FILES := $(patsubst %.fbs, $(CPP_OUTPUT_DIR)/%_generated.h, $(INPUT_FILES))
PYTHON_OUTPUT_FILES := $(patsubst %.fbs, $(PYTHON_OUTPUT_DIR)/%, $(INPUT_FILES))

all: $(CPP_OUTPUT_FILES) | $(PYTHON_OUTPUT_FILES)
	@printf "%b" "$(ANSI_GREEN)*** Success compiling FlatBuffers schema ***$(ANSI_DEFAULT_COLOR)\n"

clean:
	rm -rf $(CPP_OUTPUT_FILES) $(PYTHON_OUTPUT_FILES)

$(CPP_OUTPUT_DIR)/%_generated.h: %.fbs $(INPUT_FILES) | $(CPP_OUTPUT_DIR)
	./toolchain/flatc $(CPP_FLAGS) $<

$(PYTHON_OUTPUT_DIR)/%: %.fbs $(INPUT_FILES)
	./toolchain/flatc $(PYTHON_FLAGS) $<

$(CPP_OUTPUT_DIR):
	mkdir $@


.PHONY: all clean