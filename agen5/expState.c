
/*
 *  expState.c
 *  $Id: expState.c,v 1.10 2000/03/12 20:40:41 bruce Exp $
 *  This module implements expression functions that
 *  query and get state information from AutoGen data.
 */

/*
 *  AutoGen copyright 1992-1999 Bruce Korb
 *
 *  AutoGen is free software.
 *  You may redistribute it and/or modify it under the terms of the
 *  GNU General Public License, as published by the Free Software
 *  Foundation; either version 2, or (at your option) any later version.
 *
 *  AutoGen is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with AutoGen.  See the file "COPYING".  If not,
 *  write to:  The Free Software Foundation, Inc.,
 *             59 Temple Place - Suite 330,
 *             Boston,  MA  02111-1307, USA.
 */

#include <string.h>

#include <guile/gh.h>
#include "autogen.h"
#include "expr.h"

#ifndef HAVE_STRFTIME
#  include "compat/strftime.c"
#endif

STATIC int     entry_length(   char* pzName, tDefEntry* pCurDef );
STATIC int     count_entries(  char* pzName, tDefEntry* pCurDef );
STATIC ag_bool find_any_entry( char* pzName, tDefEntry* pCurDef );


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  EXPRESSION EVALUATION SUPPORT ROUTINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    STATIC int
entry_length( char* pzName, tDefEntry* pCurDef )
{
    ag_bool     isIndexed;
    tDefEntry*  pE;

    char* pzField = strchr( pzName, '.' );

    if (pzField != (char*)NULL)
        *(pzField++) = NUL;

    pE = findDefEntry( pzName, pCurDef, &isIndexed );

    if (pzField != (char*)NULL)
        pzField[-1] = '.';

    /*
     *  No such entry?  return zero
     */
    if (pE == (tDefEntry*)NULL)
        return 0;

    /*
     *  IF the macro declaration is text type,
     *  THEN ...
     */
    if (pE->valType == VALTYP_TEXT) {
        int resVal = 0;

        /*
         *  IF there are subfields specified
         *  THEN the result is zero (there are none for text macros)
         */
        if (pzField != (char*)NULL)
            return 0;

        /*
         *  IF we found a specific index
         *  THEN the result is the string length
         */
        if (isIndexed)
            return strlen( pE->pzValue );

        /*
         *  ELSE find the string lengths of each member
         */
        do  {
            resVal += strlen( pE->pzValue );
            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);
        return resVal;
    }

    /*
     *  The macro is a block type macro.
     *  IF there are no subfields specified,
     *  THEN the result is the twin count
     */
    if (pzField == (char*)NULL) {
        int resVal = 0;
        if (isIndexed)
            resVal = 1;

        else do  {
            resVal++;
            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);

        return resVal;
    }

    if (isIndexed)
        return entry_length( pzField, (tDefEntry*)(void*)(pE->pzValue) );

    {
        int resVal = 0;

        /*
         *  FOR each entry of the type we found, ...
         */
        do  {
            resVal += entry_length( pzField,
                                      (tDefEntry*)(void*)(pE->pzValue) );

            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);

        return resVal;
    }
}


    STATIC int
count_entries( char* pzName, tDefEntry* pCurDef )
{
    ag_bool     isIndexed;
    tDefEntry*  pE;

    char* pzField = strchr( pzName, '.' );

    if (pzField != (char*)NULL)
        *(pzField++) = NUL;

    pE = findDefEntry( pzName, pCurDef, &isIndexed );

    if (pzField != (char*)NULL)
        pzField[-1] = '.';

    /*
     *  No such entry?  return zero
     */
    if (pE == (tDefEntry*)NULL)
        return 0;

    /*
     *  IF the macro declaration is text type,
     *  THEN ...
     */
    if (pE->valType == VALTYP_TEXT) {
        int resVal = 0;

        /*
         *  IF there are subfields specified
         *  THEN the result is zero (there are none for text macros)
         */
        if (pzField != (char*)NULL)
            return 0;

        /*
         *  IF we found a specific index
         *  THEN it is just this one.
         */
        if (isIndexed)
            return 1;

        /*
         *  ELSE find the string lengths of each member
         */
        do  {
            resVal++;
            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);
        return resVal;
    }

    /*
     *  The macro is a block type macro.
     *  IF there are no subfields specified,
     *  THEN the result is the twin count
     */
    if (pzField == (char*)NULL) {
        int resVal = 0;
        if (isIndexed)
            resVal = 1;

        else do  {
            resVal++;
            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);

        return resVal;
    }

    /*
     *  IF this is indexed, then we only count entries for this entry.
     */
    if (isIndexed)
        return count_entries( pzField, (tDefEntry*)(void*)(pE->pzValue) );

    {
        int resVal = 0;

        /*
         *  FOR each entry of the type we found, ...
         */
        do  {
            resVal += count_entries( pzField,
                                     (tDefEntry*)(void*)(pE->pzValue) );

            pE = pE->pTwin;
        } while (pE != (tDefEntry*)NULL);

        return resVal;
    }
}


    STATIC ag_bool
find_any_entry( char* pzName, tDefEntry* pCurDef )
{
    ag_bool     isIndexed;
    tDefEntry*  pE;

    char* pzField = strchr( pzName, '.' );

    if (pzField != (char*)NULL)
        *(pzField++) = NUL;

    pE = findDefEntry( pzName, pCurDef, &isIndexed );

    /*
     *  No such entry?  return zero
     */
    if (pE == (tDefEntry*)NULL)
        return AG_FALSE;

    /*
     *  No subfield?  return one
     */
    if (pzField == (char*)NULL)
        return AG_TRUE;

    /*
     *  a subfield for a text macro?  return zero
     */
    if (pE->valType == VALTYP_TEXT)
        return AG_FALSE;

    pzField[-1] = '.';
    pzName = pzField;

    if (isIndexed) {
        tDefEntry*  pMemberList = (tDefEntry*)(void*)(pE->pzValue);
        if (find_any_entry( pzField, pMemberList ))
            return AG_TRUE;
    }
    else do  {
        tDefEntry*  pMemberList = (tDefEntry*)(void*)(pE->pzValue);
        if (find_any_entry( pzField, pMemberList ))
            return AG_TRUE;
        pE = pE->pTwin;
    } while (pE != (tDefEntry*)NULL);
    return AG_FALSE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  EXPRESSION ROUTINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*=gfunc base_name
 *
 * what:   base output name
 *
 * doc:  Returns a string containing the base name of the output file(s).
 *       Generally, this is also the base name of the definitions file.
=*/
    SCM
ag_scm_base_name( void )
{
    return gh_str02scm( OPT_ARG( BASE_NAME ));
}


/*=gfunc count
 *
 * what:   definition count
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc:  Count the number of entries for a definition.
 *      The input argument must be a string containing the name
 *      of the AutoGen values to be counted.  If there is no
 *      value associated with the name, the result is an SCM
 *      immediate integer value of zero.
=*/
    SCM
ag_scm_count( SCM obj )
{
    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    {
        int len = count_entries( SCM_CHARS( obj ), pDefContext );
        return gh_int2scm( len );
    }
}


/*=gfunc def_file
 *
 * what:   definitions file name
 *
 * doc:  Get the name of the definitions file.
 *       Returns the name of the source file containing the AutoGen
 *       definitions.
=*/
    SCM
ag_scm_def_file( void )
{
    return gh_str02scm( pBaseCtx->pzFileName );
}


/*=gfunc exist_p
 *
 * what:   test for value name
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc:  return SCM_BOOL_T iff a specified name has an AutoGen value.
 *       The name may include indexes and/or member names.
 *       All but the last member name must be an aggregate definition.
 *       For example:
 *       @example
 *       (exist? "foo[3].bar.baz")
 *       @end example
 *       will yield true if all of the following is true:
 *       @*
 *       There is a member value of either group or string type
 *       named @code{baz} for some group value @code{bar} that
 *       is a member of the @code{foo} group with index @code{3}.
 *       There may be multiple entries of @code{bar} within
 *       @code{foo}, only one needs to contain a value for @code{baz}.
=*/
    SCM
ag_scm_exist_p( SCM obj )
{
    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    return (find_any_entry( SCM_CHARS( obj ), pDefContext ))
           ? SCM_BOOL_T
           : SCM_BOOL_F;
}


/*=gfunc get
 *
 * what:   get named value
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc:  get the first string value associated with the name.
=*/
    SCM
ag_scm_get( SCM obj )
{
    tDefEntry*  pE;
    ag_bool     isIndexed;

    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    pE = findDefEntry( SCM_CHARS( obj ), pDefContext, &isIndexed );

    if ((pE == (tDefEntry*)NULL) || (pE->valType != VALTYP_TEXT))
        return gh_str02scm( "" );

    return gh_str02scm( pE->pzValue );
}


/*=gfunc high_lim
 *
 * what:   get highest value index
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc:  Returns the highest index associated with an array of definitions.
=*/
    SCM
ag_scm_high_lim( SCM obj )
{
    tDefEntry*  pE;
    ag_bool     isIndexed;

    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    pE = findDefEntry( SCM_CHARS( obj ), pDefContext, &isIndexed );

    /*
     *  IF we did not find the entry we are looking for
     *  THEN return zero
     *  ELSE search the twin list for the high entry
     */
    if (pE == (tDefEntry*)NULL)
        return gh_int2scm( 0 );

    if (isIndexed)
        return gh_int2scm( pE->index );

    if (pE->pEndTwin != (tDefEntry*)NULL)
        pE = pE->pEndTwin;

    return gh_int2scm( pE->index );
}


/*=gfunc len
 *
 * what:   get count of values
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc: If the named object is a group definition, then "len" is
 *     the same as "count".  Otherwise, if it is one or more text
 *     definitions, then it is the sum of their string lengths.
=*/
    SCM
ag_scm_len( SCM obj )
{
    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    {
        int len = entry_length( SCM_CHARS( obj ), pDefContext );
        return gh_int2scm( len );
    }
}


/*=gfunc low_lim
 *
 * what:   get lowest value index
 *
 * exparg: ag-name, name of AutoGen value
 *
 * doc:  Returns the lowest index associated with an array of definitions.
=*/
    SCM
ag_scm_low_lim( SCM obj )
{
    tDefEntry*  pE;
    ag_bool     isIndexed;

    if (! gh_string_p( obj ))
        return SCM_UNDEFINED;

    pE = findDefEntry( SCM_CHARS( obj ), pDefContext, &isIndexed );

    /*
     *  IF we did not find the entry we are looking for
     *  THEN return zero
     *  ELSE search the twin list for the high entry
     */
    if (pE == (tDefEntry*)NULL)
        return gh_int2scm( 0 );

    if (isIndexed)
        return gh_int2scm( pE->index );

    return gh_int2scm( pE->index );
}


/*=gfunc suffix
 *
 * what:   get the current suffix
 *
 * doc:
 *  Returns the current active suffix.  See @code{generate} in the
 *  @code{Declarations Input} section above.
=*/
    SCM
ag_scm_suffix( void )
{
    return gh_str02scm( (char*)pzCurSfx );
}


/*=gfunc tpl_file
 *
 * what:   get the template file name
 *
 * doc:  Returns the name of the current template file.
=*/
    SCM
ag_scm_tpl_file( void )
{
    return gh_str02scm( pzTemplFileName );
}

#include "expr.ini"
/* end of expState.c */
