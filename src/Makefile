LBT_ROOT := $(dir $(abspath $(dir $(lastword $(MAKEFILE_LIST)))))
include $(LBT_ROOT)/src/Make.inc

# Default target
all: $(builddir)/libblastrampoline.$(SHLIB_EXT)

# Objects we'll build
MAIN_OBJS := libblastrampoline.o dl_utils.o config.o \
			 autodetection.o \
			 threading.o deepbindless.o trampolines/trampolines_$(ARCH).o

# Include win_utils.c on windws
ifeq ($(OS),WINNT)
MAIN_OBJS += win_utils.o
endif

# If we're on an architecture that supports f2c autodetection, compile that in!
ifeq ($(F2C_AUTODETECTION),1)
MAIN_OBJS += f2c_adapters.o
endif

ifeq ($(CBLAS_DIVERGENCE_AUTODETECTION),1)
MAIN_OBJS += cblas_adapters.o
endif

ifeq ($(COMPLEX_RETSTYLE_AUTODETECTION),1)
MAIN_OBJS += complex_return_style_adapters.o
endif

# Place the `.o` files into `$(builddir)`
MAIN_OBJS := $(addprefix $(builddir)/,$(MAIN_OBJS))

$(builddir) $(builddir)/trampolines:
	@mkdir -p $@

$(builddir)/%.o: %.c | $(builddir) $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(LBT_CFLAGS) -c $^)
$(builddir)/trampolines/%.o: trampolines/%.S exported_funcs.inc | $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(LBT_CFLAGS) -c $<)

dump-trampolines: trampolines/trampolines_$(ARCH).S
	$(CC) $< -S | sed -E 's/ ((%%)|;) /\n/g' | sed -E 's/.global/\n.global/g'


$(builddir)/$(LIB_FULL_VERSION): $(MAIN_OBJS)
	@$(call PRINT_CC,$(CC) -o "$@" $(call IMPLIB_FLAGS,$@) $(LBT_CFLAGS) $(SONAME_FLAG) $^ -shared $(LBT_LDFLAGS))

$(builddir)/$(LIB_MAJOR_VERSION): | $(builddir)/$(LIB_FULL_VERSION)
	ln -sf "$(LIB_FULL_VERSION)" "$@"

$(builddir)/libblastrampoline.$(SHLIB_EXT): | $(builddir)/$(LIB_MAJOR_VERSION)
	ln -sf "$(LIB_MAJOR_VERSION)" "$@"

# Install both libraries and our headers
install: $(builddir)/libblastrampoline.$(SHLIB_EXT)
	@mkdir -p $(prefix)/include/libblastrampoline
	-@cp -Ra $(LBT_ROOT)/include/* $(prefix)/include/libblastrampoline
	@cp -a $(LBT_ROOT)/src/libblastrampoline.h $(prefix)/include/
	@mkdir -p $(prefix)/$(binlib)
	@cp -a $(builddir)/libblastrampoline*$(SHLIB_EXT)* $(prefix)/$(binlib)/
ifeq ($(OS),WINNT)
	@mkdir -p $(prefix)/lib
	@cp -a $(builddir)/$(LIB_FULL_VERSION).a $(prefix)/lib
endif

clean:
	@rm -rf $(builddir)
