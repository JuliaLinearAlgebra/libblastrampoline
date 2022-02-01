include ../../src/Make.inc

all: $(prefix)/zdotc_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/zdotc_test$(EXE): zdotc_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/zdotc_test$(EXE)

run: $(prefix)/zdotc_test$(EXE)
	@$(prefix)/zdotc_test$(EXE)
