include ../../src/Make.inc

all: $(prefix)/dgemmt_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/dgemmt_test$(EXE): dgemmt_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/dgemmt_test$(EXE)

run: $(prefix)/dgemmt_test$(EXE)
	@$(prefix)/dgemmt_test$(EXE)
