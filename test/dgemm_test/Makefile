CC ?= gcc

dgemm_test:
	$(CC) -I../../src -o $@ $(CFLAGS) $@.c $(LDFLAGS)

run: dgemm_test
	./dgemm_test
