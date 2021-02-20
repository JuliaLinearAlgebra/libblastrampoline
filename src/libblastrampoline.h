#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// This shamelessly stolen from https://github.com/JuliaLang/julia/blob/master/src/support/platform.h
#if defined(__FreeBSD__)
#define _OS_FREEBSD_
#elif defined(__linux__)
#define _OS_LINUX_
#elif defined(_WIN32) || defined(_WIN64)
#define _OS_WINDOWS_
#elif defined(__APPLE__) && defined(__MACH__)
#define _OS_DARWIN_
#elif defined(__EMSCRIPTEN__)
#define _OS_EMSCRIPTEN_
#endif

// Borrow definition from `support/dtypes.h`
#ifdef _OS_WINDOWS_
# ifdef LIBRARY_EXPORTS
#  define LBT_DLLEXPORT __declspec(dllexport)
# else
#  define LBT_DLLEXPORT __declspec(dllimport)
# endif
# define LBT_HIDDEN
#else
# if defined(LIBRARY_EXPORTS) && defined(_OS_LINUX)
#  define LBT_DLLEXPORT __attribute__ ((visibility("protected")))
# else
#  define LBT_DLLEXPORT __attribute__ ((visibility("default")))
# endif
# define LBT_HIDDEN    __attribute__ ((visibility("hidden")))
#endif

// The metadata stored on each loaded library
typedef struct {
    // The library name as passed to `lbt_forward()`.
    // To get the absolute path to the library, use `dlpath()` or similar on `handle`.
    char * libname;
    void * handle;
    // The suffix used within this library as autodetected by `lbt_forward`.
    // Common values are `""` or `"64_"`.
    const char * suffix;
    // The interface type  as autodetected by `lbt_forward`, see `LBT_INTERFACE_XXX` below
    int32_t interface;
    // The `f2c` status  as autodetected by `lbt_forward`, see `LBT_F2C_XXX` below
    int32_t f2c;
} lbt_library_info_t;

// Possible values for `interface` in `lbt_library_info_t`
#define LBT_INTERFACE_LP64              32
#define LBT_INTERFACE_ILP64             64
#define LBT_INTERFACE_UNKNOWN          -1

// Possible values for `f2c` in `lbt_library_info_t`
#define LBT_F2C_PLAIN                   0
#define LBT_F2C_REQUIRED                1
#define LBT_F2C_UNKNOWN                -1

// The config type you get back from `lbt_get_config()`
typedef struct {
    // The NULL-terminated list of libraries loaded via `lbt_forward()`.
    // This list is emptied if `clear` is set to `1` in a future `lbt_forward()` call.
    lbt_library_info_t ** loaded_libs;
    // Flags that describe this `libblastrampoline`'s build configuration.
    // See `LBT_BUILDFLAGS_XXX` below.
    uint32_t build_flags;
} lbt_config_t;

// Possible values for `build_flags` in `lbt_config_t`
#define LBT_BUILDFLAGS_DEEPBINDLESS     0x01
#define LBT_BUILDFLAGS_F2C_CAPABLE      0x02

/*
 * Load the given `libname`, lookup all registered symbols within our configured list of exported
 * symbols and `dlsym()` the symbols to load the addresses for forwarding into that library.
 *
 * If `clear` is set to a non-zero value, all symbol addresses will be reset to a pre-set value
 * before they are looked up in `libname`.  If `clear` is set to zero, symbols that do not exist in
 * `libname` will keep their previous value, which allows for loading a base library, then overriding
 * some symbols with a second shim library, integrating separate BLAS and LAPACK libraries, merging an
 * LP64 and ILP64 library into one, or all three use cases at the same time.  See the docstring for
 * `lbt_set_default_func` for how to control what `clear` sets.
 *
 * Note that on certain platforms (currently musl linux and freebsd) you cannot load a non-suffixed
 * ILP64 and an LP64 BLAS at the same time.  Read the note in the README about `RTLD_DEEPBIND`
 * support in the system libc for more details.
 *
 * If `verbose` is set to a non-zero value, it will print out debugging information.
 */
int lbt_forward(const char * libname, int clear, int verbose);

/*
 * Returns a structure describing the currently-loaded libraries as well as the build configuration
 * of this `libblastrampoline` instance.  See the definition of `lbt_config_t` in this header file
 * for more details.
 */
const lbt_config_t * lbt_get_config();

/*
 * Returns the number of threads configured by the underlying BLAS library.  In the event that
 * multiple libraries are loaded, returns the maximum over all returned values.  The functions
 * it calls to determine the number of threads are configurable at runtime, see the docstring
 * for the `lbt_register_thread_interface()` function, although many common functions (such as
 * those for `OpenBLAS`, `MKL` and `BLIS`) are already registered by default.
 */
int32_t lbt_get_num_threads();

/*
 * Sets the number of threads in the underlying BLAS library.  In the event that multiple
 * libraries are loaded, sets them all to the same value.  The functions it calls to actually
 * set the number of threads are configurable at runtime, see the docstring for the
 * `lbt_register_thread_interface()` function, although many common functions (such as those
 * for `OpenBLAS`, `MKL` and `BLIS`) are already registered by default.
 */
void lbt_set_num_threads(int32_t num_threads);

/*
 * Register a new `get_num_threads()`/`set_num_threads()` pair.  These functions are assumed to be
 * callable via the function prototypes `int32_t getter()` and `void setter(int32_t num_threads)`.
 * Note that due to register zero-extension on `x86_64` it is permissible that the setter actually
 * expects an `int64_t`, and the getter may return an `int64_t` as long as the value itself is not
 * larger than the maximum permissable `int64_t`.
 *
 * While `libblastrampoline` has built-in knowledge of some BLAS libraries' getter/setter
 * functions (such as those for `OpenBLAS`, `MKL` and `BLIS`) and will call them from
 * `lbt_{get,set}_num_threads()`, if the user loads some exotic BLAS that uses a different symbol
 * name for this functionality, they must register those getter/setter functions here to have them
 * automatically called whenever `lbt_{get,set}_num_threads()` is called.
 */
void lbt_register_thread_interface(const char * getter, const char * setter);

#ifdef __cplusplus
} // extern "C"
#endif