# make help to see all targets
# export CC=clang to change compiler ( default ggc )
# Build with DEBUG=1 for debug mode ( -ggdb3 no opt )
# Build with V=1 to enable verbose mode


# Compiler
CC ?= gcc	# gcc by default
C_STD := -std=gnu17
C_OPT := -O3
C_WARNS :=

C_FLAGS :=

ifeq ($(CC),clang)
	C_WARNS += -Weverything -Wno-padded
else ifneq (, $(filter $(CC), cc gcc))
	C_WARNS += -Wall -Wextra -pedantic -Wcast-align \
			   -Winit-self -Wlogical-op -Wmissing-include-dirs \
			   -Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wundef  \
			   -Wwrite-strings -Wpointer-arith -Wmissing-declarations \
			   -Wuninitialized -Wold-style-definition -Wstrict-prototypes \
			   -Wmissing-prototypes -Wswitch-default -Wbad-function-cast \
			   -Wnested-externs -Wconversion -Wunreachable-code
endif

ifeq ("$(origin DEBUG)", "command line")
	GGDB := -ggdb3
else
	GGDB :=
endif

C_FLAGS += $(C_STD) $(C_OPT) $(GGDB) $(C_WARNS)

# Directories
SDIR := ./src
IDIR := ./include
EDIR := ./example
TDIR := ./test
SCRIPT_DIR := ./scripts

# .c Files
SRC := $(wildcard $(SDIR)/*.c)
ESRC := $(SRC) $(wildcard $(EDIR)/*.c)
TSRC := $(SRC) $(wildcard $(TDIR)/*.c)

# .o Files
LOBJ := $(SRC:%.c=%.o)
EOBJ := $(ESRC:%.c=%.o)
TOBJ := $(TSRC:%.c=%.o)
OBJ := $(EOBJ) $(LOBJ) $(TOBJ)

# Libraries
LIB := pthread
L_INC := $(foreach l, $(LIB), -l$l)

# Binary Files
E_EXEC := example.out
T_EXEC := test.out
LIB_NAME := libmylogger.a

# Verbose mode / Not quiet
ifeq ("$(origin V)", "command line")
	Q :=
else
	Q ?= @
endif


# Installation of MyLogger
INSTALL_PATH :=
ifeq ("$(origin P)", "command line")
	INSTALL_PATH = $(P)
endif

# Shell commands
RM := rm -rf
AR := ar rcs

# Prints
define print_cc
   $(if $(Q), @echo "[CC]        $(1)")
endef

define print_bin
   $(if $(Q), @echo "[BIN]       $(1)")
endef

define print_rm
    $(if $(Q), @echo "[RM]        $(1)")
endef

define print_ar
    $(if $(Q), @echo "[AR]        $(1)")
endef


all: logger examples

logger: $(LIB_NAME)

install: __FORCE
	$(Q)$(SCRIPT_DIR)/install_mylogger.sh $(INSTALL_PATH)


__FORCE:

$(LIB_NAME): $(LOBJ)
	$(call print_ar,$@)
	$(Q)$(AR) $@ $^

examples: $(E_EXEC)

$(E_EXEC): $(EOBJ)
	$(call print_bin,$@)
	$(Q)$(CC) $(C_FLAGS) -I$(IDIR) $(EOBJ) -o $@ $(L_INC)

test: $(T_EXEC)

$(T_EXEC): $(TOBJ)
	$(call print_bin,$@)
	$(Q)$(CC) $(C_FLAGS) -I$(IDIR) $(TOBJ) -o $@ $(L_INC)

%.o:%.c
	$(call print_cc,$<)
	$(Q)$(CC) $(C_FLAGS) -I$(IDIR) -c $< -o $@

clean:
	$(call print_rm,EXEC)
	$(Q)$(RM) $(E_EXEC)
	$(Q)$(RM) $(T_EXEC)
	$(Q)$(RM) $(LIB_NAME)
	$(call print_rm,OBJ)
	$(Q)$(RM) $(OBJ)

help:
	@echo "Targets:"
	@echo "    all               - build MyLogger, examples and tests"
	@echo "    logger            - build only MyLogger"
	@echo "    test              - tests"
	@echo "    examples          - examples"
	@echo "    install[P = Path] - install MyLogger to path P"
	@echo -e
	@echo "When DEBUG=1 no opt applied and compiled with -ggdb3 flag"
	@echo "When V=1 build with verbose mode"
	@echo "Change CC variable for different compiler (export CC=clang)"

