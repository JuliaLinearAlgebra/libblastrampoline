# Shared build rules for the standalone C test programs under `test/<name>_test/`.
# Each test directory's Makefile simply does `include ../test.mk`; the test name is
# derived from the containing directory, so there is no per-test boilerplate to keep
# in sync.  Invoked by `test/common.jl` as `make -sC test/<name> prefix=... CFLAGS=...`.
include ../../src/Make.inc

# e.g. `dgemm_test` for `test/dgemm_test/`
TEST_NAME := $(notdir $(CURDIR))

all: $(prefix)/$(TEST_NAME)$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/$(TEST_NAME)$(EXE): $(TEST_NAME).c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/$(TEST_NAME)$(EXE)

run: $(prefix)/$(TEST_NAME)$(EXE)
	@$(prefix)/$(TEST_NAME)$(EXE)
