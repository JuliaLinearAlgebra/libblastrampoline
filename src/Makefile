LBT_ROOT := $(dir $(abspath $(dir $(lastword $(MAKEFILE_LIST)))))
include $(LBT_ROOT)/src/Make.inc

ifeq ($(OS),WINNT)
# On Windows only build the library with the major soversion, all other copies
# are useless and error prone.
TARGET_LIBRARIES = $(builddir)/$(LIB_MAJOR_VERSION)
else
TARGET_LIBRARIES = $(builddir)/$(LIB_MAJOR_VERSION) $(builddir)/$(LIB_FULL_VERSION) $(builddir)/libblastrampoline.$(SHLIB_EXT)
endif

maintarget=$(word 1,$(TARGET_LIBRARIES))

# Default target
all: $(maintarget)

# Objects we'll build
MAIN_OBJS := libblastrampoline.o dl_utils.o env_utils.o config.o \
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

ifeq ($(OS),WINNT)
# On Windows only build the library with the major soversion, all other copies
# are useless and error prone.
TARGET_LIBRARIES = $(builddir)/$(LIB_MAJOR_VERSION)
else
TARGET_LIBRARIES = $(builddir)/$(LIB_MAJOR_VERSION) $(builddir)/$(LIB_FULL_VERSION) $(builddir)/libblastrampoline.$(SHLIB_EXT)
endif

$(builddir) $(builddir)/trampolines:
	@mkdir -p $@

$(builddir)/%.o: %.c | $(builddir) $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(LBT_CFLAGS) -c $^)
$(builddir)/trampolines/%.o: trampolines/%.S exported_funcs.inc | $(builddir)/trampolines
	@$(call PRINT_CC,$(CC) -o $@ $(LBT_CFLAGS) -c $<)

dump-trampolines: trampolines/trampolines_$(ARCH).S
	$(CC) $< -S | sed -E 's/ ((%%)|;) /\n/g' | sed -E 's/.global/\n.global/g'


$(builddir)/$(LIB_MAJOR_VERSION): $(MAIN_OBJS)
	@$(call PRINT_CC,$(CC) -o "$@" $(call IMPLIB_FLAGS,$@) $(LBT_CFLAGS) $(SONAME_FLAG) $^ -shared $(LBT_LDFLAGS))

ifneq ($(OS),WINNT)
$(builddir)/$(LIB_FULL_VERSION): | $(builddir)/$(LIB_MAJOR_VERSION)
	ln -sf "$(LIB_MAJOR_VERSION)" "$@"

$(builddir)/libblastrampoline.$(SHLIB_EXT): | $(builddir)/$(LIB_MAJOR_VERSION)
	ln -sf "$(LIB_MAJOR_VERSION)" "$@"
endif

# Install both libraries and our headers
install: $(TARGET_LIBRARIES)
	@mkdir -p $(DESTDIR)$(includedir)/libblastrampoline
	-@cp -Ra $(LBT_ROOT)/include/* $(DESTDIR)$(includedir)/libblastrampoline
	@cp -a $(LBT_ROOT)/src/libblastrampoline.h $(DESTDIR)$(includedir)/
	@mkdir -p $(DESTDIR)$(libdir)
	@for lib in $(TARGET_LIBRARIES); do \
		cp -a $${lib} $(DESTDIR)$(libdir)/; \
	done
ifeq ($(OS),WINNT)
	@mkdir -p $(DESTDIR)$(prefix)/lib
	@cp -a $(builddir)/$(LIB_MAJOR_VERSION).a $(DESTDIR)$(prefix)/lib
endif

clean:
	@rm -rf $(builddir)
