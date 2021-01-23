# ARCH detection in Make, used to pick out which trampoline assembly syntax we're gonna use
ARCH := $(shell $(CC) -dumpmachine | sed "s/\([^-]*\).*$$/\1/")

# Normalize ppc64le to powerpc64le
ifeq ($(ARCH),ppc64le)
  ARCH := powerpc64le
endif
# We map amd64 to x86_64 for compatibility with systems that identify 64-bit systems as such
ifeq ($(ARCH),amd64)
  ARCH := x86_64
endif
# We map arm64 (Apple spelling) to aarch64 to avoid having to deal with both spellings everywhere
ifeq ($(ARCH),arm64)
  ARCH := aarch64
endif

# Figure out OS and architecture (used to set `shlib_ext`)
OS := $(shell uname)
ifneq (,$(findstring MINGW,$(OS))$(findstring MSYS,$(OS)))
  OS := WINNT
endif

ifeq ($(OS), WINNT)
  SHLIB_EXT := dll
else ifeq ($(OS), Darwin)
  SHLIB_EXT := dylib
else
  SHLIB_EXT := so
endif

# Objects we'll build
OBJS := main.o trampolines/trampolines_$(ARCH).o

# Include win_utils.c on windows
ifeq ($(OS),WINNT)
OBJS += win_utils.o
endif

CFLAGS =-O2 -fPIC -DLIBRARY_EXPORTS
LDFLAGS = -shared

# On linux, we need to link `libdl` to get `dlopen`
ifeq ($(OS),Linux)
LDFLAGS += -ldl
endif

# Default installation location; we expect you to be overriding this
prefix ?= build


all: libblastrampoline.$(SHLIB_EXT)
%.o: %.c
	$(CC)  -o $@ $(CFLAGS) -c $^

libblastrampoline.$(SHLIB_EXT): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

install: libblastrampoline.$(SHLIB_EXT)
	mkdir -p $(prefix)/include
	cp -vd cblas.h $(prefix)/include
	mkdir -p $(prefix)/lib
	cp -vd $< $(prefix)/lib

clean:
	rm -f $(OBJS)
	rm -f libblastrampoline.$(SHLIB_EXT)
