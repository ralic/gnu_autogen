
/*
 *  columns.c
 *  $Id: columns.c,v 4.14 2007/11/01 05:24:24 bkorb Exp $
 *  Time-stamp:        "2007-10-28 15:55:26 bkorb"
 *
 *  Columns copyright (c) 1992-2007 Bruce Korb - all rights reserved
 *  Columns is free software.
 *  This file is part of AutoGen.
 *
 *  AutoGen copyright (c) 1992-2007 Bruce Korb - all rights reserved
 *
 *  AutoGen is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  AutoGen is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NUL
# define NUL '\0'
#endif

struct print_list {
    char**     papz;
    int        index;
};

typedef struct print_list tPrintList, *tpPrintList;

int maxEntryWidth = 0;
int fillColumnCt  = 0;

#define LINE_LIMIT 4096
char zLine[ LINE_LIMIT ];
char zFmtLine[ LINE_LIMIT ];

char**  papzLines  = (char**)NULL;
tCC*    pzLinePfx  = NULL;
tCC*    pzFirstPfx = NULL;
size_t  allocCt    = 0;
size_t  usedCt     = 0;
size_t  columnCt   = 0;
size_t  columnSz   = 0;
size_t  indentSize = 0;

/* = = = START-STATIC-FORWARD = = = */
/* static forward declarations maintained by :mkfwd */
static uint32_t
handleIndent( tCC* pzIndentArg );

static void
readLines( void );

static void
writeColumns( void );

static void
writeRows( void );

static void
writeFill( void );

static int
compProc( const void* p1, const void* p2 );
/* = = = END-STATIC-FORWARD = = = */

int
main( int    argc,
      char** argv )
{
    if (optionProcess( &columnsOptions, argc, argv ) != argc) {
        fputs( "Error:  this program takes no arguments\n", stderr );
        USAGE( EXIT_FAILURE );
    }

    if (HAVE_OPT( INDENT )) {
        indentSize = handleIndent( OPT_ARG( INDENT ));
        OPT_VALUE_WIDTH -= indentSize;

        if (! HAVE_OPT( FIRST_INDENT ))
            pzFirstPfx = pzLinePfx;
        else {

            /*
             *  The first line has a special indentation/prefix.
             *  Compute it, but do not let it be larger than
             *  the indentation value.
             */
            tCC* pzSave = pzLinePfx;
            size_t firstSize = handleIndent( OPT_ARG( FIRST_INDENT ));
            pzFirstPfx = pzLinePfx;
            pzLinePfx  = pzSave;

            /*
             *  Now force the first line prefix to have the same size
             *  as the indentSize
             */
            if (firstSize > indentSize) {
                static char const zSepLine[] =
                    "Note: prefix `%s' has been put on a separate line\n";
                fputs(pzFirstPfx, stdout);
                putc('\n', stdout);
                fprintf( stderr, zSepLine, pzFirstPfx );
                pzFirstPfx = pzLinePfx;

            } else if (firstSize < indentSize) {
                char* tmp = malloc( indentSize + 1 );
                char  z[10];
                snprintf( z, sizeof(z), "%%-%ds", (int)indentSize );
                snprintf( tmp, indentSize + 1, z, pzFirstPfx );
                pzFirstPfx = tmp;
            }
        }
    }

    if (HAVE_OPT( LINE_SEPARATION ))
        OPT_VALUE_WIDTH -= strlen( OPT_ARG( LINE_SEPARATION ));

    if (HAVE_OPT( COL_WIDTH ))
        columnSz = OPT_VALUE_COL_WIDTH;

    if (HAVE_OPT( COLUMNS ))
        columnCt = OPT_VALUE_COLUMNS;

    if (OPT_VALUE_WIDTH <= 16)
        OPT_VALUE_WIDTH = 16;

    readLines();

    if (HAVE_OPT( SORT ))
        qsort( (void*)papzLines, usedCt, sizeof( char* ), &compProc );

    if (HAVE_OPT( BY_COLUMNS ))
         writeColumns();
    else if (HAVE_OPT(FILL))
        writeFill();
    else writeRows();

    return EXIT_SUCCESS;
}


static uint32_t
handleIndent( tCC* pzIndentArg )
{
    char* pz;
    uint32_t colCt = strtoul( pzIndentArg, &pz, 0 );

    /*
     *  IF the indent argument is a number
     */
    if ((*pz == '\0') && (colCt > 0) && (colCt < OPT_VALUE_WIDTH)) {
        char* p;

        /*
         *  Allocate a string to hold the line prefix
         */
        pzLinePfx = p = malloc( (size_t)colCt + 1 );
        if (pzLinePfx == NULL) {
            fprintf( stderr, "Cannot malloc %d bytes\n", colCt + 1 );
            exit( EXIT_FAILURE );
        }

        /*
         *  Set it to a NUL terminated string of spaces
         */
        memset(p, ' ', (size_t)colCt);
        p[ colCt ] = '\0';

    } else {
        tCC* p;
        /*
         *  Otherwise, set the line prefix to whatever the string is.
         *  It will not be the empty string because that is handled
         *  as an indent count of zero and is ignored.
         */
        pzLinePfx = pzIndentArg;
        p = pzIndentArg;
        colCt =  0;
        for (;;) {
            /*
             *  Figure out how much of the line is taken up by
             *  the prefix.  We might consider restricted format
             *  strings some time in the future, but not now.
             */
            switch (*p++) {
            case '\0':
                goto colsCounted;

            case '\t':
                colCt += OPT_VALUE_TAB_WIDTH;
                colCt -= (colCt % OPT_VALUE_TAB_WIDTH);
                break;

            case '\n':
            case '\f':
            case '\r':
                colCt = 0;
                break;

            default:
                colCt++;
                break;

            case '\a':
                break;
            }
        } colsCounted:;
    }

    return colCt;
}


static void
readLines(void)
{
    int   sepLen;

    if (HAVE_OPT( SEPARATION ))
         sepLen = strlen( OPT_ARG( SEPARATION ));
    else sepLen = 0;


    /*
     *  Read the input text, stripping trailing white space
     */
    for (;;) {
        char*   pzL;
        char*   pzText = fgets(zLine, (int)sizeof( zLine ), stdin);
        size_t  len;

        if (pzText == NULL)
            break;

        /*
         *  Trim off trailing white space.
         */
        len = strlen( pzText );
        pzText += len;
        while (isspace(pzText[-1])) {
            if (--pzText == zLine) {
                if (HAVE_OPT(FILL))
                    break;
                goto next_line;
            }
            len--;
        }

        *pzText = '\0';

        /*
         *  IF the input lines are to be reformatted,
         *  THEN the length is the result of the sprintf
         *  Else, compute the length.
         */
        if (HAVE_OPT(FORMAT)) {
            pzText = zFmtLine;
            len = snprintf(zFmtLine, sizeof(zFmtLine), OPT_ARG(FORMAT), zLine);
        } else {
            pzText = zLine;
        }

        /*
         *  Allocate a string and space in the pointer array.
         */
        len += sepLen + 1;
        pzL = (char*)malloc( len );
        if (++usedCt > allocCt) {
            allocCt += 128;
            papzLines = (char**)realloc( (void*)papzLines,
                                         sizeof( char* ) * allocCt );
        }
        papzLines[ usedCt-1 ] = pzL;

        /*
         *  Copy the text and append the separation character
         */
        strcpy( pzL, pzText );

        /*
         *  Initially, all strings have the separator,
         *  the entries may get reordered.
         */
        if (sepLen > 0)
            strcat( pzL, OPT_ARG(SEPARATION));

        if (len > maxEntryWidth)
            maxEntryWidth = len;
    next_line:;
    }

    if (maxEntryWidth == 0) {
        fputs( "Warning:  no input text was read\n", stderr );
        exit( EXIT_SUCCESS );
    }

    /*
     *  Set the line width to the amount of space we have to play with.
     */
    if ((OPT_VALUE_WIDTH < maxEntryWidth) && (! HAVE_OPT(FILL)))
        OPT_VALUE_WIDTH = maxEntryWidth;

    /*
     *  If we do not have a column size set,
     *  then figure out what it must be.
     */
    if (columnSz == 0) {
        /*
         *  IF the column count has not been set,
         *  THEN compute it.
         */
        if (columnCt == 0)
            columnCt = OPT_VALUE_WIDTH / maxEntryWidth;

        /*
         *  IF there are to be multiple columns, ...
         */
        if (columnCt > 1) {
            int  spreadwidth = OPT_VALUE_WIDTH - maxEntryWidth;
            int  sz = spreadwidth / (columnCt-1);

            /*
             *  Either there is room for added space,
             *  or we must (possibly) reduce the number of columns
             */
            if (sz >= maxEntryWidth)
                columnSz = sz;
            else {
                columnCt = (spreadwidth / maxEntryWidth) + 1;
                if (columnCt > 1)
                     columnSz = spreadwidth / (columnCt - 1);
                else columnSz = OPT_VALUE_WIDTH;
            }
        }
    }

    /*
     *  Otherwise, the column size has been set.  Ensure it is sane.
     */
    else {
        /*
         *  Increase the column size to the width of the widest entry
         */
        if (maxEntryWidth > columnSz)
            columnSz = maxEntryWidth;

        /*
         *  IF     we have not been provided a column count
         *    *OR* we are set to overfill the output line,
         *  THEN compute the number of columns.
         */
        if (  (columnCt == 0)
           || (((columnSz * (columnCt-1)) + maxEntryWidth) > OPT_VALUE_WIDTH))
            columnCt = ((OPT_VALUE_WIDTH - maxEntryWidth) / columnSz) + 1;
    }

    /*
     *  Ensure that any "spread" we added to the column size
     *  does not exceed the parameterized limit.
     */
    if (   HAVE_OPT( SPREAD )
        && ((maxEntryWidth + OPT_VALUE_SPREAD - 1) < columnSz))
        columnSz = maxEntryWidth + OPT_VALUE_SPREAD - 1;
}


static void
writeColumns( void )
{
    char zFmt[ 12 ];
    int  colCt, rowCt, col, row;
    tpPrintList pPL;

    colCt = columnCt;
    snprintf(zFmt, sizeof(zFmt), "%%-%ds", (int)columnSz);

    if (colCt == 1) {
        writeRows();
        return;
    }

    pPL   = (tpPrintList)malloc( colCt * sizeof( tPrintList ));

    /*
     *  This "loop" is normally executed half way through and exited.
     *  IF, however, we would  produce an empty final column,
     *  we will reduce our column count and line width and then
     *  try the top-of-column pointer computation again.
     *
     *  The problem solved here is that sometimes, when the
     *  number of entries in a row is greater than the number of rows,
     *  it is possible that all the entries that would have been
     *  in the last column are, instead, essentially put on the
     *  last row.  That will leave the final column empty.
     *  We could regroup at that point and spread the columns some more,
     *  but, if done, is an exercise for later.
     */
    for (;;) {
        int  rem;
        int  fsz;

        rowCt = (usedCt/colCt) + ((usedCt % colCt) ? 1 : 0);

        /*
         *  FOR each column, compute the address of the pointer to
         *  the string at the top of the column, and the index of
         *  that entry.
         */
        for (col = 0; col < colCt ; col++) {
            pPL[col].papz  = papzLines + (col * rowCt);
            pPL[col].index = col * rowCt;
        }

        /*
         *  IF the final column is not empty,
         *  THEN break out and start printing.
         */
        if (pPL[colCt-1].index < usedCt)
            break;

        /*
         *  The last column is blank, so we reduce our column count,
         *  even if the user specified a count!!
         */
        colCt--;
        rem   = OPT_VALUE_WIDTH - (colCt * maxEntryWidth);

        if ((rem == 0) || (colCt < 2) || (columnSz > 0))
            fsz = maxEntryWidth;
        else
            fsz = maxEntryWidth + (rem / (colCt-1));
        snprintf( zFmt, sizeof(zFmt), "%%-%ds", fsz );
    }

    /*
     *  Now, actually print each row...
     */
    for ( row = 0 ;; ) {
        char*  pzL;
        char*  pzE;

        if (pzLinePfx != NULL)
            fputs( pzLinePfx, stdout );

        /*
         *  Increment the index of the current entry in the last column.
         *  IF it goes beyond the end of the entries in use,
         *  THEN reduce our column count.
         */
        if ((pPL[colCt-1].index)++ >= usedCt)
            colCt--;

        /*
         *  Get the address of the string in the last column.
         */
        pzE = *(pPL[colCt-1].papz++);

        col = 0;

        /*
         *  FOR every column except the last,
         *     print the entry with the width format
         *
         *  No need to worry about referring to a non-existent
         *  entry.  Only the last column might have that problem,
         *  and we addressed it above where the column count got
         *  decremented.
         */
        while (++col < colCt) {
            pzL = *(pPL[col-1].papz++);
            fprintf( stdout, zFmt, pzL );
            free( (void*)pzL );
        }

        /*
         *  See if we are on the last row.  If so, then
         *  this is the last entry.  Strip any separation
         *  characters, emit the entry and break out.
         */
        if (++row == rowCt) {
            /*
             *  IF we have a separator,
             *  THEN remove it from the last entry.
             */
            if (HAVE_OPT( SEPARATION )) {
                char* pz = pzE + strlen( pzE )
                         - strlen( OPT_ARG( SEPARATION ));
                *pz = '\0';
            }
            fputs( pzE, stdout );
            putc( '\n', stdout );
            break;
        }

        /*
         *  Print the last entry on the line, without the width format.
         *  If we have line separation (which does not apply to the last
         *  line), then emit those characters, too.
         */
        fputs( pzE, stdout );
        if (HAVE_OPT( LINE_SEPARATION ))
            fputs( OPT_ARG( LINE_SEPARATION ), stdout );

        putc( '\n', stdout );
        free( (void*)pzE );
    }
}


static void
writeRows( void )
{
    char zFmt[ 12 ];
    int  colCt;

    colCt = columnCt;
    snprintf(zFmt, sizeof(zFmt), "%%-%ds", (int)columnSz);

    /*
     *  IF we have a separator,
     *  THEN remove it from the last entry.
     */
    if (HAVE_OPT( SEPARATION )) {
        char* pz = papzLines[ usedCt-1 ];
        pz += strlen( pz ) - strlen( OPT_ARG( SEPARATION ));
        *pz = '\0';
    }

    if (pzFirstPfx != NULL) {
        fputs( pzFirstPfx, stdout );
        pzFirstPfx = pzLinePfx;
    }

    {
        char**  ppzLL = papzLines;
        size_t  left  = usedCt;
        int     lnNo  = 0;

        /*
         *  FOR every entry we are to emit, ...
         */
        for (;;) {
            char* pzL = *ppzLL++;

            /*
             *  IF this is the last entry,
             *  THEN emit it and a new line and break out
             */
            if (--left <= 0) {
                fputs( pzL, stdout );
                putc( '\n', stdout );
                free( (void*)pzL );
                break;
            }

            /*
             *  IF the count of entries on this line is still less
             *     than the number of columns,
             *  THEN emit the padded entry
             *  ELSE ...
             */
            if (++lnNo < colCt)
                fprintf( stdout, zFmt, pzL );

            else {
                lnNo = 0;
                /*
                 *  Last entry on the line.  Emit the string without padding.
                 *  IF we have a line separation string, emit that too.
                 */
                fputs( pzL, stdout );
                if (HAVE_OPT( LINE_SEPARATION ))
                    fputs( OPT_ARG( LINE_SEPARATION ), stdout );

                putc( '\n', stdout );

                /*
                 *  Start the next line with any required indentation
                 */
                if (pzFirstPfx != NULL) {
                    fputs( pzFirstPfx, stdout );
                    pzFirstPfx = pzLinePfx;
                }
            }

            free( (void*)pzL );
        }
    }
}

static int
emitWord(char const * word, size_t len, int col)
{
    static int ended_with_period = 0;

    if (col > 0) {
        if (len >= (OPT_VALUE_WIDTH - col)) {
            putc('\n', stdout);
            if (pzLinePfx != NULL)
                fputs(pzLinePfx, stdout);
            col = 0;

        } else {
            if (ended_with_period) {
                putc(' ', stdout);
                col++;
            }
            putc(' ', stdout);
            col++;
        }
    }

    fwrite(word, len, 1, stdout);
    col += len;
    ended_with_period = (word[len - 1] == '.');

    return col;
}


static void
writeFill( void )
{
    char**  ppzLL = papzLines;
    size_t  left  = usedCt;
    int     colNo = 0;

    if (pzFirstPfx != NULL)
        fputs(pzFirstPfx, stdout);

    /*
     *  FOR every entry we are to emit, ...
     */
    while (left-- > 0) {
        char* pzL = *ppzLL;

        while (isspace(*pzL))  pzL++;

        /*
         *  Blank lines are magical and trigger a blank line in output.
         */
        if (*pzL == NUL) {
            if (colNo > 0) /* guard against multiple blank lines */
                putc('\n', stdout);
            putc('\n', stdout);
            colNo = -2;
            free(*(ppzLL++));
            continue;
        }

        /*
         *  We are going to emit some output.  Make sure we're indented.
         */
        if (colNo < 0) {
            if (pzLinePfx != NULL)
                fputs(pzLinePfx, stdout);
            colNo = 0;
        }

        do {
            size_t tknlen;

            for (tknlen = 0; pzL[tknlen] != NUL; tknlen++)
                if (isspace(pzL[tknlen]))
                    break;
            colNo = emitWord(pzL, tknlen, colNo);
            pzL += tknlen;
            while (isspace(*pzL))  pzL++;
        } while (*pzL != NUL);

        free(*(ppzLL++));
    }
    putc( '\n', stdout );
}


/*
 *  Line comparison procedure
 */
static int
compProc( const void* p1, const void* p2 )
{
    char const* pz1 = *(char* const*)p1;
    char const* pz2 = *(char* const*)p2;
    return strcmp( pz1, pz2 );
}
/*
 * Local Variables:
 * mode: C
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * end of columns/columns.c */
