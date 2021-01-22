CC ?= gcc

dgemm_test:
	$(CC) -o $@ $(CFLAGS) $@.c $(LDFLAGS)

run: dgemm_test
	./dgemm_test