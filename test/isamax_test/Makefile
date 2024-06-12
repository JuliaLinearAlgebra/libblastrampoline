include ../../src/Make.inc

all: $(prefix)/isamax_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/isamax_test$(EXE): isamax_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/isamax_test$(EXE)

run: $(prefix)/isamax_test$(EXE)
	@$(prefix)/isamax_test$(EXE)
