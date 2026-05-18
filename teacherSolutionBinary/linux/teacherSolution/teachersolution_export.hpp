
#ifndef TEACHERSOLUTION_EXPORT_H
#define TEACHERSOLUTION_EXPORT_H

#ifdef TEACHERSOLUTION_STATIC_DEFINE
#  define TEACHERSOLUTION_EXPORT
#  define TEACHERSOLUTION_NO_EXPORT
#else
#  ifndef TEACHERSOLUTION_EXPORT
#    ifdef teacherSolution_EXPORTS
        /* We are building this library */
#      define TEACHERSOLUTION_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define TEACHERSOLUTION_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef TEACHERSOLUTION_NO_EXPORT
#    define TEACHERSOLUTION_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef TEACHERSOLUTION_DEPRECATED
#  define TEACHERSOLUTION_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef TEACHERSOLUTION_DEPRECATED_EXPORT
#  define TEACHERSOLUTION_DEPRECATED_EXPORT TEACHERSOLUTION_EXPORT TEACHERSOLUTION_DEPRECATED
#endif

#ifndef TEACHERSOLUTION_DEPRECATED_NO_EXPORT
#  define TEACHERSOLUTION_DEPRECATED_NO_EXPORT TEACHERSOLUTION_NO_EXPORT TEACHERSOLUTION_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TEACHERSOLUTION_NO_DEPRECATED
#    define TEACHERSOLUTION_NO_DEPRECATED
#  endif
#endif

#endif /* TEACHERSOLUTION_EXPORT_H */
