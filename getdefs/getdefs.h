/*  -*- Mode: C -*-
 *
 *  $Id: getdefs.h,v 3.3 2002/03/19 04:11:41 bkorb Exp $
 *
 *    getdefs copyright 1999 Bruce Korb
 *
 *  Author:            Bruce Korb <bkorb@gnu.org>
 *  Maintainer:        Bruce Korb <bkorb@gnu.org>
 *  Created:           Mon Jun 30 15:35:12 1997
 *  Last Modified:     $Date: 2002/03/19 04:11:41 $
 *            by:      Bruce Korb <bkorb@gnu.org>
 */

#ifndef GETDEFS_HEADER
#define GETDEFS_HEADER

#include "config.h"

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <streqv.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

#ifndef HAVE_SNPRINTF
#  include "snprintfv/snprintfv.h"
#endif

#include REGEX_HEADER

#ifdef DEBUG
#  define STATIC
#else
#  define STATIC static
#endif
#define EXPORT

#include "opts.h"

#ifdef DEFINE
#  define MODE
#  define VALUE(v) = v
#  define DEF_STRING(n,s) tCC n[] = s
#else
#  define MODE extern
#  define VALUE(v)
#  define DEF_STRING(n,s) extern tCC n[sizeof(s)]
#endif

#ifndef FOPEN_BINARY_FLAG
#  ifdef USE_FOPEN_BINARY
#    define FOPEN_BINARY_FLAG   "b"
#  else
#    define FOPEN_BINARY_FLAG
#  endif
#endif

#ifndef FOPEN_TEXT_FLAG
#  ifdef USE_FOPEN_TEXT
#    define FOPEN_TEXT_FLAG   "t"
#  else
#    define FOPEN_TEXT_FLAG
#  endif
#endif

#define MAXNAMELEN 256
#ifndef MAXPATHLEN
#  define MAXPATHLEN 4096
#endif

#define NUL            '\0'
#define MAX_SUBMATCH   1
#define COUNT(a)       (sizeof(a)/sizeof(a[0]))

#define MARK_CHAR ':'

#ifndef STR
#  define __STR(s)  #s
#  define STR(s)    __STR(s)
#endif

#define AG_NAME_CHAR(c) (zUserNameCh[(unsigned)(c)] & 2)
#define USER_NAME_CH(c) (zUserNameCh[(unsigned)(c)] & 1)
MODE char zUserNameCh[ 256 ] VALUE( { '\0' } );

/*
 *  Procedure success codes
 *
 *  USAGE:  define procedures to return "tSuccess".  Test their results
 *          with the SUCCEEDED, FAILED and HADGLITCH macros.
 */
#define SUCCESS  ((tSuccess) 0)
#define FAILURE  ((tSuccess)-1)
#define PROBLEM  ((tSuccess) 1)

typedef int tSuccess;

#define SUCCEEDED( p )     ((p) == SUCCESS)
#define SUCCESSFUL( p )    SUCCEEDED( p )
#define FAILED( p )        ((p) <  SUCCESS)
#define HADGLITCH( p )     ((p) >  SUCCESS)

/*
 *  Index database string pointers.
 */
MODE char*    pzIndexText VALUE( (char*)NULL ); /* all the text    */
MODE char*    pzEndIndex  VALUE( (char*)NULL ); /* end of current  */
MODE char*    pzIndexEOF  VALUE( (char*)NULL ); /* end of file     */
MODE size_t   indexAlloc  VALUE( 0 );           /* allocation size */

/*
 *  Name of program to process output (normally ``autogen'')
 */
MODE tCC*     pzAutogen   VALUE( "autogen" );

/*
 *  const global strings
 */
DEF_STRING( zGlobal,     "\n/* GLOBALDEFS */\n" );
DEF_STRING( zLineId,     "\n#line %d \"%s\"\n" );
DEF_STRING( zMallocErr,  "Error:  could not allocate %d bytes for %s\n" );
DEF_STRING( zAttribRe,   "\n[^*\n]*\\*[ \t]*([a-z][a-z0-9_]*):");
DEF_STRING( zNameTag,    " = {\n    name    = '" );
DEF_STRING( zMemberLine, "    member  = " );
DEF_STRING( zNoData,     "error no data for definition in file %s line %d\n" );
DEF_STRING( zAgDef,      "autogen definitions %s;\n");
DEF_STRING( zDne,
            "/*  -*- buffer-read-only: t -*- vi: set ro:\n *\n"
            " *\n *  DO NOT EDIT THIS FILE   (%s)\n *\n"
            " *  It has been extracted by getdefs from the following files:\n"
            " *\n" );

/*
 *  ptr to zero (NUL) terminated definition pattern string.
 *
 *  The pattern we look for starts with the three characters
 *  '/', '*' and '=' and is followed by two names:
 *  the name of a group and the name of the entry within the group.
 *
 *  The patterns we accept for output may specify a particular group,
 *  certain members within certain groups or all members of all groups
 */
MODE char*   pzDefPat   VALUE( (char*)NULL );
MODE regex_t define_re;
MODE regex_t attrib_re;

/*
 *  The NUL-terminated string containing the name of the template
 *  the output definitions are to refer to.
 */
MODE char   zTemplName[ 65 ] VALUE( {0} );

/*
 *  The output file pointer.  It may be "stdout".
 *  It gets closed when we are done.
 */
MODE FILE*  evtFp       VALUE( (FILE*)NULL );

/*
 *  The output file modification time.  Only used if we
 *  have specified a real file for output (not stdout).
 */
MODE time_t modtime     VALUE( 0 );

/*
 *  The array of pointers to the output blocks.
 *  We build them first, then sort them, then print them out.
 */
MODE char**  papzBlocks VALUE( (char**)NULL );
MODE size_t  blkUseCt   VALUE(  0 );
MODE size_t  blkAllocCt VALUE(  0 );

MODE pid_t   agPid      VALUE( -1 );

#include "proto.h"

#endif /* GETDEFS_HEADER */
