/*  -*- Mode: C -*-  */

/* --- fake the preprocessor into handlng portability */

/*
 * Author:           Gary V Vaughan <gvaughan@oranda.demon.co.uk>
 * Created:          Mon Jun 30 15:54:46 1997
 * Last Modified:     Thu May  6 18:21:59 1999
 *            by:     Bruce Korb <korb@datadesign.com>
 *
 * $Id: compat.h,v 2.4 1999/06/03 19:43:28 bkorb Exp $
 */
#ifndef COMPAT_H
#define COMPAT_H 1

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#undef STATIC

#ifdef DEBUG
#  define STATIC
#else
#  define STATIC static
#endif

#include <config.h>

#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#else
#  error NEED <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#else
#  error NEED <sys/stat.h>
#endif

#ifdef STDC_HEADERS
#  ifndef HAVE_STRING_H
#     define HAVE_STRING_H	1
#  endif
#  ifndef HAVE_STDLIB_H
#     define HAVE_STDLIB_H	1
#  endif
#endif

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#else
#  error NEED <stdlib.h>
#endif

# ifdef HAVE_UNISTD_H
#   include <unistd.h>
# endif

# ifdef HAVE_ERRNO_H
#   include <errno.h>
# endif

/* some systems #def errno! and others do not declare it!! */
#ifndef errno
   extern       int     errno;
#endif

#include <stdio.h>
#include <ctype.h>

/* Some machines forget this! */

# ifndef EXIT_FAILURE
#   define EXIT_SUCCESS 0
#   define EXIT_FAILURE 1
# endif

#ifndef NUL
#  define NUL '\0'
#endif

#ifndef FOPEN_BINARY_FLAG
#  ifdef USE_FOPEN_BINARY
#    define FOPEN_BINARY_FLAG	"b"
#  else
#    define FOPEN_BINARY_FLAG
#  endif
#endif

#ifndef FOPEN_TEXT_FLAG
#  ifdef USE_TEXT_BINARY
#    define FOPEN_TEXT_FLAG	"t"
#  else
#    define FOPEN_TEXT_FLAG
#  endif
#endif

#ifdef HAVE_LIMITS_H
#  include <limits.h>
#elif HAVE_SYS_LIMITS_H
#  include <sys/limits.h>
#endif

#if !defined (MAXPATHLEN) && defined (HAVE_SYS_PARAM_H)
#  include <sys/param.h>
#endif /* !MAXPATHLEN && HAVE_SYS_PARAM_H */

#if !defined (MAXPATHLEN) && defined (PATH_MAX)
#  define MAXPATHLEN PATH_MAX
#endif /* !MAXPATHLEN && PATH_MAX */

#if !defined (MAXPATHLEN)
#  define MAXPATHLEN 1024
#endif /* MAXPATHLEN */

# ifndef LONG_MAX
#   define LONG_MAX	~(1L << (8*sizeof(long) -1))
#   define INT_MAX	~(1 << (8*sizeof(int) -1))
#   define SHORT_MAX	~(1 << (8*sizeof(short) -1))
# endif

# ifndef ULONG_MAX
#   define ULONG_MAX	~(OUL)
#   define UINT_MAX	~(OU)
#   define USHORT_MAX	~(OUS)
# endif

/* redefine these for BSD style string libraries */
#ifndef HAVE_STRCHR
#  define strchr		index
#  define strrchr		rindex
#endif

#if HAVE_STRING_H
#  include <string.h>
#elif HAVE_STRINGS_H
#  include <strings.h>
#else
   extern char *strchr(), *strrchr();
#endif

#ifdef HAVE_SETJMP_H
#  include <setjmp.h>
#else
#  error NEED <setjmp.h>
#endif

/* ##### Pointer sized word ##### */

/* FIXME:  the MAX stuff in here is broken! */
#if SIZEOF_CHARP > SIZEOF_INT
   typedef long t_word;
   #define WORD_MAX  LONG_MAX
   #define WORD_MIN  LONG_MIN
#else /* SIZEOF_CHARP <= SIZEOF_INT */
   typedef int t_word;
   #define WORD_MAX  INT_MAX
   #define WORD_MIN  INT_MIN
#endif

#ifndef HAVE_PATHFIND
  EXTERN char *pathfind (/* const char *, const char *, const char * */);
#endif

# if defined (HAVE_DIRENT_H)
#   include <dirent.h>
#   define D_NAMLEN(dirent) strlen((dirent)->d_name)
# else /* !HAVE_DIRENT_H */
#   define dirent direct
#   define D_NAMLEN(dirent) (dirent)->d_namlen
#   if defined (HAVE_SYS_NDIR_H)
#     include <sys/ndir.h>
#   endif /* HAVE_SYS_NDIR_H */
#   if defined (HAVE_SYS_DIR_H)
#     include <sys/dir.h>
#   endif /* HAVE_SYS_DIR_H */
#   if defined (HAVE_NDIR_H)
#     include <ndir.h>
#   endif /* HAVE_NDIR_H */
#   if !defined (HAVE_SYS_NDIR_H) && !defined (HAVE_SYS_DIR_H) && !defined (HAVE_NDIR_H)
#     include "ndir.h"
#   endif /* !HAVE_SYS_NDIR_H && !HAVE_SYS_DIR_H && !HAVE_NDIR_H */
# endif /* !HAVE_DIRENT_H */

# if defined (_POSIX_SOURCE)
/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
#    define REAL_DIR_ENTRY(dp) 1
# else /* !_POSIX_SOURCE */
#    define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
# endif /* !_POSIX_SOURCE */

#ifdef HAVE_LIBGEN_H
#  include <libgen.h>
#endif

#define UNUSED_COMPAT
#ifdef UNUSED_COMPAT

# ifdef HAVE_MEMORY_H
#   include <memory.h>
# endif

/* use the gnu fast memcpy if we have access to it */
# if __GNUC__ > 1
#     define memcpy(d,s,n)	__builtin_memcpy ((d),(s),(n))
# endif

/* otherwise redefine the mem* functions only if they do not exist */
# ifdef HAVE_MEMCPY_LATER
#   if __GNUC__ <= 1
#     define memcpy(d,s,n)	bcopy((s),(d),(n))
#   endif
#   define memmove(d,s,n)	bcopy((s),(d),(n))
#   define memcmp(d,s,n)	bcmp ((s),(d),(n))
# endif


/* variable argument parsing - use VA_START(ap, last) in source files */
# ifdef HAVE_VARARGS_H
#   include <varargs.h>
#   define VA_START(ap, last)	va_start(ap)
# elif HAVE_STDARG_H
#   include <stdarg.h>
#   define VA_START		va_start
#   define va_alist		...
# endif

#endif /* UNUSED_COMPAT */
#endif /* COMPAT_H */

/* compat.h ends here */
