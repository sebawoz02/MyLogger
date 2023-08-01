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
C_TEST_FLAGS ?=

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
LOGS := $(wildcard log*.txt)

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

# Other files
COV_FILES := *.html *.css $(TDIR)/*.gcda $(TDIR)/*.gcno $(SDIR)/*.gcda $(SDIR)/*.gcno

# Programs
T_COVR := gcovr

T_COVR_FLAGS := -r . --html-details --output=coverage_report.html

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

coverage:
	$(Q)$(MAKE) test C_TEST_FLAGS='-fprofile-arcs -ftest-coverage'
	$(Q)./$(T_EXEC)
	$(Q)$(RM) $(TDIR)/*.gcda $(TDIR)/*.gcno
	$(Q)$(T_COVR) $(T_COVR_FLAGS)

test: $(T_EXEC)

$(T_EXEC): $(TOBJ)
	$(call print_bin,$@)
	$(Q)$(CC) $(C_FLAGS) -I$(IDIR) $(TOBJ) -o $@ $(L_INC) $(C_TEST_FLAGS)

%.o:%.c
	$(call print_cc,$<)
	$(Q)$(CC) $(C_FLAGS) -I$(IDIR) -c $< -o $@ $(C_TEST_FLAGS)

clean:
	$(call print_rm,EXEC)
	$(Q)$(RM) $(E_EXEC)
	$(Q)$(RM) $(T_EXEC)
	$(Q)$(RM) $(LIB_NAME)
	$(call print_rm,OBJ)
	$(Q)$(RM) $(OBJ)
	$(Q)$(RM) $(LOGS)
	$(Q)$(RM) $(COV_FILES)

help:
	@echo "Targets:"
	@echo "    all               - build MyLogger, examples and tests"
	@echo "    logger            - build only MyLogger"
	@echo "    test              - tests"
	@echo "    coverage          - create html report about test coverage"
	@echo "    examples          - examples"
	@echo "    install[P = Path] - install MyLogger to path P"
	@echo "    clean             - remove every .c .o .out file and log files in current dir"
	@echo -e
	@echo "When DEBUG=1 no opt applied and compiled with -ggdb3 flag"
	@echo "When V=1 build with verbose mode"
	@echo "Change CC variable for different compiler (export CC=clang)"

