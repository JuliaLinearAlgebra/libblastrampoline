include ../../src/Make.inc

all: $(prefix)/dgemm_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/dgemm_test$(EXE): dgemm_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/dgemm_test$(EXE)

run: $(prefix)/dgemm_test$(EXE)
	@$(prefix)/dgemm_test$(EXE)
