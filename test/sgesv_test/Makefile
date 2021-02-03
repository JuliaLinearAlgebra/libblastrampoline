include ../../src/Make.inc

all: $(prefix)/sgesv_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/sgesv_test$(EXE): sgesv_test.c | $(prefix)
	@$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	@rm -f $(prefix)/sgesv_test$(EXE)

run: $(prefix)/sgesv_test$(EXE)
	@$(prefix)/sgesv_test$(EXE)
