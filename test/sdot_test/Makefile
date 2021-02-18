include ../../src/Make.inc

all: $(prefix)/sdot_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/sdot_test$(EXE): sdot_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/sdot_test$(EXE)

run: $(prefix)/sdot_test$(EXE)
	@$(prefix)/sdot_test$(EXE)
