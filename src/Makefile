include Make.inc

# Objects we'll build
OBJS := main.o trampolines/trampolines_$(ARCH).o

# Include win_utils.c on windows
ifeq ($(OS),WINNT)
OBJS += win_utils.o
endif

all: libblastrampoline.$(SHLIB_EXT)
%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $^

libblastrampoline.$(SHLIB_EXT): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

install: libblastrampoline.$(SHLIB_EXT)
	mkdir -p $(prefix)/include
	cp -vd cblas.h $(prefix)/include
	mkdir -p $(prefix)/lib
	cp -vd $< $(prefix)/lib

clean:
	rm -f $(OBJS)
	rm -f *.$(SHLIB_EXT)
