# Configuration
PROGRAM    = $(EXAMPLE)
# Directories
SRC_DIRS  += src
SRC_DIRS  += src/examples/$(EXAMPLE)
INC_DIRS  += inc
LIB_DIRS  += bin/libs
BIN_DIR    = bin
# Compiler
CC		   = g++
CFLAGS	  += -O3 -g -std=c++17
DEFINES   +=
LIBRARIES += raylib

# Script (Read Only)
# Lists
SRC_FILES  = $(wildcard $(patsubst %,%/*.cpp,$(SRC_DIRS)))
SRC_TO_OBJ = $(addprefix ./$(BIN_DIR)/,$(addsuffix .o,$(notdir $(basename $1))))
OBJ_FILES  = $(call SRC_TO_OBJ,$(SRC_FILES))
SRC_TO_DEP = $(addprefix ./$(BIN_DIR)/,$(addsuffix .d,$(notdir $(basename $1))))
DEP_FILES  = $(call SRC_TO_DEP,$(SRC_FILES))

# Options
CFLAGS	+= $(foreach def,$(DEFINES),-D$(def))
CFLAGS	+= $(foreach dir,$(INC_DIRS),-I$(dir))
LDFLAGS += $(foreach dir,$(LIB_DIRS),-L$(dir)) -L$(BIN_DIR)
LDFLAGS += $(foreach lib,$(LIBRARIES),-l$(lib))

# Compiler
OUTPUT_COMMAND = $(CC) -o $(PROGRAM) $(OBJ_FILES) $(LDFLAGS)

# Targeter
define TARGETER
$(call SRC_TO_OBJ,$1) : $1 | $(BIN_DIR)
	$(CC) -MD -MP -MT "$(call SRC_TO_DEP,$1) $$@" -c $(CFLAGS) -o $$@ $$<
endef

# Targets  (Read Only)
.PHONY: all clean clean-d clean-o clean-x

all: $(PROGRAM)

clean: clean-d clean-o clean-x

clean-d:
	rm -rf $(DEP_FILES)

clean-o:
	rm -rf $(OBJ_FILES)

clean-x:
	rm -rf $(PROGRAM)

$(PROGRAM): $(SRC_FILES) $(OBJ_FILES)
	$(OUTPUT_COMMAND)

$(foreach src,$(SRC_FILES),$(eval $(call TARGETER,$(src))))

-include $(DEP_FILES)
