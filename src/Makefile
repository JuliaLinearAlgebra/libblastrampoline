LBT_ROOT := $(dir $(abspath $(dir $(lastword $(MAKEFILE_LIST)))))
include $(LBT_ROOT)/src/Make.inc

# Default target
all: $(builddir)/libblastrampoline.$(SHLIB_EXT)

# Objects we'll build
MAIN_OBJS := libblastrampoline.o dl_utils.o config.o autodetection.o surrogates.o trampolines/trampolines_$(ARCH).o

# Include win_utils.c on windws
ifeq ($(OS),WINNT)
MAIN_OBJS += win_utils.o
endif

# If we're on an architecture that supports f2c autodetection, compile that in!
ifeq ($(F2C_AUTODETECTION),1)
MAIN_OBJS += f2c_adapters.o
endif

# Place the `.o` files into `$(builddir)`
MAIN_OBJS := $(addprefix $(builddir)/,$(MAIN_OBJS))

$(builddir) $(builddir)/trampolines:
	@mkdir -p $@

$(builddir)/%.o: %.c | $(builddir) $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(CFLAGS) -c $^)
$(builddir)/trampolines/%.o: trampolines/%.S exported_funcs.inc | $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(CFLAGS) -c $<)

dump-trampolines: trampolines/trampolines_$(ARCH).S
	$(CC) $< -S | sed -E 's/ ((%%)|;) /\n/g' | sed -E 's/.global/\n.global/g'

$(builddir)/libblastrampoline.$(SHLIB_EXT): $(MAIN_OBJS)
	@$(call PRINT_CC,$(CC) -o $@ $(call IMPLIB_FLAGS,$@) $(CFLAGS) $^ $(LDFLAGS))

# Install both libraries and our headers
install: $(builddir)/libblastrampoline.$(SHLIB_EXT)
	@mkdir -p $(prefix)/include/libblastrampoline
	-@cp -Ra $(LBT_ROOT)/include/* $(prefix)/include/libblastrampoline
	@mkdir -p $(prefix)/$(binlib)
	@cp -a $(builddir)/libblastrampoline.$(SHLIB_EXT) $(prefix)/$(binlib)
ifeq ($(OS),WINNT)
	@mkdir -p $(prefix)/lib
	@cp -a $(builddir)/libblastrampoline.$(SHLIB_EXT).a $(prefix)/lib
endif

clean:
	@rm -rf $(builddir)
