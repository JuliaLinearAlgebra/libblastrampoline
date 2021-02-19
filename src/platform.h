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

// We need to tell the compiler to generate "naked" functions for some things
#define LBT_NAKED     __attribute__ ((naked))

// Borrow definitions from `julia.h`
#if defined(__GNUC__)
#  define LBT_CONST_FUNC __attribute__((const))
#elif defined(_COMPILER_MICROSOFT_)
#  define LBT_CONST_FUNC __declspec(noalias)
#else
#  define LBT_CONST_FUNC
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