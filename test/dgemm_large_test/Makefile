include ../../src/Make.inc

all: $(prefix)/dgemm_large_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/dgemm_large_test$(EXE): dgemm_large_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/dgemm_large_test$(EXE)

run: $(prefix)/dgemm_large_test$(EXE)
	@$(prefix)/dgemm_large_test$(EXE)
