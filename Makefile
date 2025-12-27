NC     = \033[0m
BLUE   = \033[1;34m
CYAN   = \033[1;36m
GREEN  = \033[1;32m
YELLOW = \033[1;33m

CC = clang
LD = clang

CFLAGS =  -std=c11 -Wall -Wextra -Wpedantic -Isrc
CFLAGS += -Ideps/

CFLAGS_DEB = -O0 -g -gdwarf-4 -fsanitize=address
CFLAGS_REL = -O3

LDFLAGS =

LDFLAGS_DEB = -fsanitize=address

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2) $(filter $(subst *, %, $2), $d))

OBJ_DEB_DIR = build/debug/obj
OBJ_REL_DIR = build/release/obj
SRC         = $(call rwildcard, src, *.c)
OBJ_DEB     = $(patsubst src/%.c, $(OBJ_DEB_DIR)/%.o, $(SRC))
OBJ_REL     = $(patsubst src/%.c, $(OBJ_REL_DIR)/%.o, $(SRC))

EXE_REL = build/release/mamoc
EXE_DEB = build/debug/mamoc

.PHONY: run clean

debug: $(EXE_DEB)
release: $(EXE_REL)

run: debug
	@ echo -e "$(CYAN)EXECUTING$(NC) $(EXE_DEB)"
	@ ./$(EXE_DEB)

$(EXE_DEB): $(OBJ_DEB)
	@ echo -e "$(GREEN)LINKING EXECUTABLE$(NC) $(EXE_DEB)"
	@ $(LD) $(OBJ_DEB) -o $(EXE_DEB) $(LDFLAGS) $(LDFLAGS_DEB)

$(EXE_REL): $(OBJ_REL)
	@ echo -e "$(GREEN)LINKING EXECUTABLE$(NC) $(EXE_REL)"
	@ $(LD) $(OBJ_REL) -o $(EXE_REL) $(LDFLAGS)

$(OBJ_REL_DIR)/%.o: src/%.c
	@ mkdir -p $(@D)
	@ echo -e "$(GREEN)COMPILING OBJECT$(NC) $@"
	@ $(CC) $(CFLAGS) $(CFLAGS_REL) -c $< -o $@

$(OBJ_DEB_DIR)/%.o: src/%.c
	@ mkdir -p $(@D)
	@ echo -e "$(GREEN)COMPILING OBJECT$(NC) $@"
	@ $(CC) $(CFLAGS) $(CFLAGS_DEB) -c $< -o $@

clean:
	@ echo -e "$(YELLOW)CLEANING PROJECT$(NC)"
	@ rm -rf build
