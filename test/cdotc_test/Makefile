include ../../src/Make.inc

all: $(prefix)/cdotc_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/cdotc_test$(EXE): cdotc_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/cdotc_test$(EXE)

run: $(prefix)/cdotc_test$(EXE)
	@$(prefix)/cdotc_test$(EXE)
