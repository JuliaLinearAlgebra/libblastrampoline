include Make.inc

all: libblastrampoline.$(SHLIB_EXT) libblastrampoline64_.$(SHLIB_EXT)
%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $^

libblastrampoline.$(SHLIB_EXT):
	$(CC) -DEXPORTED_FUNCS_INC=\"$(abspath .)/jl_exported_funcs.inc\" -o $@ $(CFLAGS) main.c trampolines/trampolines_$(ARCH).S $^ $(LDFLAGS)

libblastrampoline64_.$(SHLIB_EXT):
	$(CC) -DEXPORTED_FUNCS_INC=\"$(abspath .)/jl_exported_funcs64_.inc\" -o $@ $(CFLAGS) main.c trampolines/trampolines_$(ARCH).S $(LDFLAGS)

install: libblastrampoline.$(SHLIB_EXT)
	mkdir -p $(prefix)/include
	cp -vd cblas.h $(prefix)/include
	mkdir -p $(prefix)/lib
	cp -vd $< $(prefix)/lib

clean:
	rm -f $(OBJS)
	rm -f *.$(SHLIB_EXT)
