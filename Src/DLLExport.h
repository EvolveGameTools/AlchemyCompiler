/* Some compilers use a special export keyword */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define TE_DECLSPEC __declspec(dllexport)
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define TE_DECLSPEC __attribute__ ((visibility("default")))
#  elif defined(__GNUC__) && __GNUC__ >= 2
#   define TE_DECLSPEC __declspec(dllexport)
#  else
#   define TE_DECLSPEC
#  endif
# endif

#define DLLEXPORT TE_DECLSPEC