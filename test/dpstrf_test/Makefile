include ../../src/Make.inc

all: $(prefix)/dpstrf_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/dpstrf_test$(EXE): dpstrf_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/dpstrf_test$(EXE)

run: $(prefix)/dpstrf_test$(EXE)
	@$(prefix)/dpstrf_test$(EXE)
