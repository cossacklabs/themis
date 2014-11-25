/*
 *  Name:
 *     StarZ

 *  Purpose:
 *     File Browser/Getter for World Wide Web Browsers.

 *  Language:
 *     C

 *  Description:
 *     This is a Common Gateway Interface program which when accessed
 *     with an HTTP GET as SCRIPT_URL (see #defines below) will return
 *     a directory listing of files available on the host machine.
 *
 *     The directory listing is in the shape of an HTML form with a
 *     "checkbox" button for each directory entry.  The size of each file
 *     or an indication that it is a directory are given.
 *
 *     A button for "mode select" is also given.
 *
 *     The default mode is File Browse.
 *     A selected file is displayed, or a selected directory generates a
 *     new HTML form for that directory.
 *
 *     The other mode is File Get.
 *     Selected files and/or directories are put in a tar archive and
 *     compressed back to the Web browser program.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     30-DEC-1994 (MJC):
 *       Added a direct "change directory to" text entry field.
 *       Changed note relating to Netscape to reflect change in Company name.
 *     12-APR-1995 (MJC):
 *       Limited access to directories under /star tree.
 *     {enter_further_changes_here}

 *  Problems:
 *     Nothing checks that a file for browse is a text file.
 *     Default file protections are used, so a remote user can access
 *     any unprotected files on the system hosting this program.
 *     Some large HTML forms are slowly processed by the common
 *     Web browser Mosaic, this is best overcome by using a more advanced
 *     browser such as Netscape Communications' Netscape but remains a general
 *     problem.
 *     The code may well be OSF/1 specific.
 *     {note_further_problems_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-
 */

/*Standard Include files:
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <time.h>

/*Local Macros.
 */
#define MAX_ENTRIES     (10240)  /* Maximum number of lines in cgi request.   */
#define LIST_ENTRIES    (10240)  /* Maximum number of files in a Get request. */
#define TEXT_BUFSIZE    (1024)   /* Internal buffer size.                     */
#define ENT_WID         (64)     /* Maximum length of a file name.            */
#define MODE_FLAG       "spoingmode" /*Internal flag for Browse/Get mode.     */

#define DEFAULT_DIR     "/star"   /* First directory to be listed.            */

/*Where the program will be placed.
 */
#define SCRIPT_URL      "http://www.star.ucl.ac.uk/mjc-cgi/S.tar.Z"

/*Maintenance details.
 */
#define AUTHOR_NAME "Martin Clayton"
#define AUTHOR_URL  "<A HREF=\"mailto:mjc@starlink.ucl.ac.uk\">mjc@starlink.ucl.ac.uk</A>"

/*Structure to hold a cgi request list entry.
 */
typedef struct {
    char *name;
    char *val;
} entry;

/*Function prototypes, local.
 */
void html_file( char *a, char *b, int nf );
void html_directory( char *a, int nf );
void dir_dot_parse( char *s, char *d );
void tar_zed( entry *e, int m );

/*
 *Function prototypes, external.
 */
char *makeword( char *line, char stop );
char *fmakeword( FILE *f, char stop, int *len );
void unescape_url( char *url );
void plustospace (char *str );

/*
 *Function prototyping for lclint checking
 */
/*@nullterminated@*/char *getenv(/*@nullterminated@*/char *);

int atoi(/*@nullterminated@*/char *);

/*@nullterminated@*/char *strcpy(char *, /*@nullterminated@*/char *);

int strcmp(/*@nullterminated@*/ const char *s1, /*@nullterminated@*/ const char *s2);

/*@nullterminated@*/char *strcat(/*@nullterminated@*/char *dst, /*@nullterminated@*/const char *src);

int stat(/*@nullterminated@*/const char *path, struct stat *buf);

int strncmp(const char *s1, const char *s2, size_t n);

DIR *opendir(/*@nullterminated@*/const char *dirname);

char *strncpy(char *dst, const char *src, size_t n);

void qsort(void *base, size_t nel, size_t width,
          int (*compar) (const void *, const void *));

struct tm *localtime(const time_t *clock);

void *memcpy(void *s1, const void *s2, size_t n);

int sprintf(char *s, /*@nullterminated@*/const char *format, /* args */ ...);

int system(const char *string);



/*****************************************************************************/
main()
{

/*
 *  Local Variables:
 */
entry entries[MAX_ENTRIES];
int x;
int m;           /*number of lines in request*/
int cl;
char wdir[TEXT_BUFSIZE];
/*@nullterminated@*/ char adir[TEXT_BUFSIZE];
struct stat statbuf;
/*.
 */

/*No request given.
 */
    if ( !getenv( "CONTENT_LENGTH" ) ) {
        m = -1;

/*Move request lines into "entry" array.
 */
    } else {
        cl = atoi( getenv( "CONTENT_LENGTH" ) );
        m = 0;
        for ( x = 0; cl && ( !feof( stdin ) ); x++ ) {
            m = x;
            entries[x].val = fmakeword( stdin, '&', &cl );
            plustospace( entries[x].val );
            unescape_url( entries[x].val );
            entries[x].name = makeword( entries[x].val, '=' );
        }
    }

/*No request made.  Initial inquiry from remote machine.
 */
    if ( ( m == -1 ) || ( !entries[0].name ) ) {
        html_directory( "", 0 );

/*Decode the request.
 */
    } else {

/*    Request for compressed archive data.
 */
        if ( !strcmp( entries[0].name, MODE_FLAG ) ) {
            tar_zed( &entries[1], m - 1 );

/*    Browse request.
 */
        } else if ( !strcmp( entries[0].name, "Explicit" ) ) {

/*        Check for explicit directory or file name.
 */
            if ( strcmp( entries[0].val, "" ) ) {
                strcpy( adir, entries[0].val );

            } else if ( m > 0 ) {

/*            Build required file name, first directory part.
 */
                strcpy( adir, entries[1].val );

/*            Add in a / if not root directory.
 */
                if ( strcmp( adir, "/" ) ) {
                    strcat( adir, "/" );
                }

/*            Append the actual file name.
 */
                strcat( adir, entries[1].name );

            } else {
                strcpy( adir, DEFAULT_DIR );
            }

/*        Modify the file name if it is . or ..
 */
            dir_dot_parse( adir, wdir );

/*        Get file information.
 */
            stat( wdir, &statbuf );

/*        Build a directory listing.
 */
            if ( statbuf.st_mode & S_IFDIR) {
                html_directory( wdir, m - 1 );

/*        Display a file's contents.
 */
            } else {
                html_file( wdir, entries[1].name, m - 1 );
            }
        }
    }

/*Done.
 */
    exit ( EXIT_SUCCESS );
}


/*****************************************************************************/
void html_directory(
/*+
 *  Name:
 *     html_directory

 *  Purpose:
 *     Build directory listing in HTML.

 *  Language:
 *     C

 *  Invocation:
 *     html_directory( dirname, nf )

 *  Arguments:
 *     dirname = char * (Given)
 *        Pointer to name of the directory to be listed.
 *     nf = int (Given)
 *        Total number of files in request.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     20-DEC-1994 (MJC):
 *       Added support for file last modification time to be displayed.
 *     21-DEC-1994 (MJC):
 *       Removed spurious <P> from second control button pair.
 *     21-JAN-1995 (MJC):
 *       Added gopher images to directory listings.
 *     12-APR-1995 (MJC):
 *       Limited access to directories under /star tree.
 *     {enter_further_changes_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-

 *  Arguments Given:
 */
char *dirname,                   /* Name of directory to be displayed.        */

int nf                           /* Number of files in request.               */

)
{

/*
 *  Local Variables:
 */
DIR *the_directory;

struct dirent *the_file;
struct stat statbuf;
struct tm ttime;

char tbuf[128];
char full_path[TEXT_BUFSIZE];
char adir[TEXT_BUFSIZE];
char ent_tab[LIST_ENTRIES][ENT_WID];

int entries;
int notvalid = 0;
int i;

/*.
 */

/*When not an empty string, use supplied file name.
 */
    if ( *dirname != '\0' ) {
        strcpy( full_path, dirname );

/*Otherwise set to default directory.
 */
    } else {
        strcpy( full_path, DEFAULT_DIR );
    }

/*Make sure request is in /star tree, if not go to default directory.
 */
    if ( strncmp( full_path, DEFAULT_DIR, 5 ) ) {
        notvalid = 1;
        strcpy( full_path, DEFAULT_DIR );
    }

/*Try to open the directory.
 */
    the_directory = opendir( full_path );

/*Start response output.
 */
    www_begin( T_HTML );

/*HTML title.
 */
    www_head( "StarZ: File browser-getter." );

    hline( "<BODY>" );

/*Heading.
 */
    hrule( );
    hheading( "StarZ: File browser-getter", 1 );
    hrule( );

/*Print error message if unable to open directory.
 */
    if ( !the_directory ) {
        printf( "Could not open %s directory.%c", full_path, NEW_LINE );


/*Otherwise start to build listing.
 */
    } else {

/*    Currently, only one directory can be selected for listing.
 *    The first chosen is displayed, others are ignored except that
 *    the following message is displayed.
 */
        if ( nf > 0 ) {
            printf( "<B>Only first entry in list will be displayed.</B>%c",
                    NEW_LINE );
        }

/*    Print warning that request is not in /star tree.
 */
        if ( notvalid ) {
            printf( "<B>Your request has been denied, default %s directory selected instead.</B>%c",
                    DEFAULT_DIR, NEW_LINE );
        }

/*    Start HTML FORM.
 *    URL of where script will reside.
 */
        printf( "<FORM action=\"%s\" method=\"POST\">%c",
                SCRIPT_URL, NEW_LINE );

/*    Submit and reset buttons.  These are duplicated later.
 */
        printf( "<INPUT type=submit value=\" Submit Request \">%c", NEW_LINE );
        printf( "<INPUT type=reset value=\" Reset \"><P>%c", NEW_LINE );

/*    Browse or Get mode-select button.
 */
        printf( "<INPUT TYPE=\"checkbox\" NAME=\"%s\" ", MODE_FLAG );
        printf( "VALUE=\"%s\"> <B>Get or Browse (default) mode</B><BR>%c",
                full_path, NEW_LINE);

/*    Text input field for entry of explicit file or directory name.
 */
        printf( "<INPUT SIZE=32 TYPE=TEXT NAME=\"Explicit\"> <B>Change Directory</B><BR>%c", NEW_LINE );

/*    List heading.
 */
        printf( "<H2>Directory of %s</H2>%c", full_path, NEW_LINE );

/*    Build directory listing.
 */
        entries = 0;
        while ( ( the_file = readdir( the_directory ) ) &&
               ( entries < LIST_ENTRIES ) ) {

/*        Don't include . and .. in list for sort.
 */
            if ( !strcmp( the_file->d_name, "." ) ) {
                continue;

            } else if ( !strcmp( the_file->d_name, ".." ) ) {
                continue;

/*        Other file names added to list.
 */
            } else {
                strncpy( ent_tab[entries], the_file->d_name, ENT_WID );
                ent_tab[entries][ENT_WID - 1] = '\0';
                entries++;
            }
        }

/*    Close the directory.
 */
        closedir( the_directory );

/*    Sort the directory list.
 */
        qsort( (void *)(ent_tab), entries + 1, ENT_WID, strcmp );

/*    Start output of the listing.
 */
        printf( "<PRE WIDTH=64>%c", NEW_LINE );

/*    Current directory button.
 */
        printf( "<INPUT TYPE=\"checkbox\" NAME=\".\" VALUE=\"%s\">",
                full_path);
        printf( " &lt;-current directory-&gt;%c", NEW_LINE );

/*    Parent directory button.
 */
        if ( strcmp( full_path, "/" ) ) {
            printf( "<INPUT TYPE=\"checkbox\" NAME=\"..\" VALUE=\"%s\">",
                    full_path);
            printf( " &lt;-parent directory%c", NEW_LINE );
        }

/*    Files themselves.
 */
        for ( i = 1; i <= entries; i++ ) {

/*       Start with button.
 */
           printf( "<INPUT TYPE=\"checkbox\" NAME=\"%s\" VALUE=\"%s\">",
                   ent_tab[i], full_path );

/*       Get file information.
 */
           strcpy( adir, full_path );
           if ( strcmp( adir, "/" ) ) {
               strcat( adir, "/" );
           }
           strcat( adir, ent_tab[i] );
           stat( adir, &statbuf );

/*       Mark as a directory if that is the case.
 */
           if ( statbuf.st_mode & S_IFDIR) {
               printf( " <IMG ALIGN=absbottom BORDER=0 SRC=\"internal-gopher-menu\">" );
               printf( " %-32s", ent_tab[i] );
               printf( "%9s", "directory" );

/*       Otherwise print the size of the file.
 */
           } else {
               if ( extension( ent_tab[i], "html" ) ) {
                   printf( " <IMG ALIGN=absbottom BORDER=0 SRC=\"internal-gopher-text\">" );

               } else {
                   printf( " <IMG ALIGN=absbottom BORDER=0 SRC=\"internal-gopher-unknown\">" );
               }
               printf( " %-32s", ent_tab[i] );
               printf( "%9d", (int)( statbuf.st_size ) );
           }

/*       Last modification date/time information.
 */
           (void)(memcpy( &ttime, localtime(&statbuf.st_mtime),
                          sizeof(struct tm)) );
           (void)(strncpy( tbuf, asctime( &ttime ), 26 ) );
           *( tbuf + 24 ) = '\0';
           printf( "  %s%c", tbuf, NEW_LINE );

        }

/*    End of the directory listing.
 */
        printf( "</PRE>%c", NEW_LINE );

/*    Output a second set of control buttons.
 */
        printf( "<INPUT type=submit value=\" Submit Request \">%c",
                NEW_LINE );
        printf( "<INPUT type=reset value=\" Reset \">%c", NEW_LINE );

/*    End of form.
 */
        printf( "</FORM>%c", NEW_LINE );
    }

/*End main part of response
 */
    hrule( );


/*HTML Address field.
 */
    www_address( AUTHOR_NAME, AUTHOR_URL, NULL );

/*End response.
 */
    hline( "</BODY>" );
    www_end( );

/*Done.
 */
return;
}


/*****************************************************************************/
void dir_dot_parse(
/*+
 *  Name:
 *     dir_dot_parse

 *  Purpose:
 *     Finds simplest version of a file specifier with . or .. at end.

 *  Language:
 *     C

 *  Invocation:
 *     dir_dot_parse( indir, outdir )

 *  Arguments:
 *     indir = char * (Given)
 *        Pointer to the file specification to be checked.
 *     outdir = char * (Given and Returned)
 *        Pointer to space for parsed specification, must be big enough
 *        to hold converted file name.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     {enter_further_changes_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-

 *  Arguments Given:
 */
char *indir,
char *outdir

)
{

/*
 *  Local Variables:
 */
char *cptr;
char *eptr;

/*.
 */

/*Mark default return value as a blank string.
 */
    strcpy( outdir, "" );
    if ( !strcmp( indir, "" ) ) {
        return;
    }

/*Find the end of the supplied string.
 */
    cptr = indir;
    while ( *cptr ) {
        cptr++;
    }
    eptr = cptr;

/*Point to last non-null character in string.
 */
    if ( cptr > indir ) {
        cptr--;

    } else {
        return;
    }

/*Does string end in a '.'?
 */
    if ( *cptr == '.' ) {
        cptr--;

/*    Current directory, remove /. part of file name.
 */
        if ( *cptr == '/' ) {
            *cptr++ = '\0';
            *cptr = '\0';

        } else if ( *cptr == '.' ) {
            cptr--;

/*        Parent directory, remove /.. part of file name and
 *        name of the current directory, unless the current
 *        directory is root, in which case just remove the ..
 */
            if ( *cptr == '/' ) {
                while ( cptr > indir ) {
                    cptr--;
                    if ( *cptr == '/' ) {
                        break;
                    }
                }
                if ( cptr > indir) {
                    while ( cptr < eptr ) {
                        *cptr++ = '\0';
                    }

                } else {
                    *(indir + 1) = '\0';
                }
            }
        }
    }

/*Copy the parsed file specifier to output buffer.
 */
    strcpy ( outdir, indir );

/*Done.
 */
return;
}


/*****************************************************************************/
void html_file(
/*+
 *  Name:
 *     html_file

 *  Purpose:
 *     Display a text file in an HTML 'wrapper'.

 *  Language:
 *     C

 *  Invocation:
 *     html_file( fname, sname, nf )

 *  Arguments:
 *     fname = char * (Given)
 *        Pointer to full path of the file to be displayed.
 *     sname = char * (Given)
 *        Pointer to name of the file only.
 *     nf = int (Given)
 *        Total number of files in request.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     21-DEC-1994 (MJC):
 *       Added missing <BODY>.
 *     {enter_further_changes_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-

 *  Arguments Given:
 */
char *fname,                     /* Full path of file.                        */
char *sname,                     /* Name only of file.                        */

int nf                           /* Total number of files in request.         */

)
{

/*
 *  Local Variables:
 */
char thecommand[4 * TEXT_BUFSIZE];
char title[1024];                 /* Workspace for page title.                */
/*.
 */

/*It's a gif wrap in simple HTML page.
 */
    if ( extension( fname, "gif" ) ) {

/*    Start response.
 */
        www_begin( T_HTML );
        sprintf( title, "StarZ: %s", sname );
        www_head( title );

        hline( "<BODY>" );

/*Pointer to image.
 */
        printf( "<IMG SRC=\"%s\"><P>%c", fname, NEW_LINE );

/*    Attach an address field.
 */
        www_address( AUTHOR_NAME, AUTHOR_URL, NULL );

/*    End response.
 */
        hline( "</BODY>" );
        www_end( );


/*If the file appears NOT to be an HTML file, then wrap it in
 *some appropriate HTML.
 */
    } else if ( !extension( fname, "html" ) ) {

/*    Start response.
 */
        www_begin( T_HTML );
        sprintf( title, "StarZ: %s", sname );
        www_head( title );

        hline( "<BODY>" );

/*    Only the first file of a multiple-file display request is piped back.
 */
        if ( nf > 0 ) {
            hline( "<B>Only first entry in list will be displayed.</B>" );
        }

/*    Wrap the file in preformatted HTML style.
 */
        hline( "<PRE>" );

/*    Flush stdout prior to calling cat to display file.
 */
        fflush( stdout );

/*    Build command line to display the file.
 */
        sprintf( thecommand, "cat %s\0x00", fname );

/*    Pipe out file.
 */
        if ( system ( thecommand ) ) {
            printf( "Could not access file %s%c", fname, NEW_LINE );
        }

/*    End of wrap.
 */
        hline( "</PRE>" );

/*    Attach an address field.
 */
        www_address( AUTHOR_NAME, AUTHOR_URL, NULL );

/*    End response.
 */
        hline( "</BODY>" );
        www_end( );


/*Other case is a file we expect to be in HTML format.
 */
    } else if ( extension( fname, "html" ) ) {

/*    Start response.
 */
        printf( "Content-type: text/html%c%c", NEW_LINE, NEW_LINE );

/*    Only the first file of a multiple-file display request is piped back.
 */
        if ( nf > 0 ) {
            printf( "<B>Only first entry in list will be displayed.</B>%c",
                    NEW_LINE );
        }

/*    Flush stdout prior to calling cat
 */
        fflush( stdout );

/*    Pipe out HTML file.
 */
        sprintf( thecommand, "cat %s\0x00", fname );
        if ( system ( thecommand ) ) {
            printf( "Could not access file %s%c", fname, NEW_LINE );
        }
    }

/*Done.
 */
return;
}


/*****************************************************************************/
int extension(
/*+
 *  Name:
 *     extension

 *  Purpose:
 *     Compare the last part of a file specifier with a supplied extension.

 *  Language:
 *     C

 *  Invocation:
 *     extension( fname, fext )

 *  Arguments:
 *     fname = char * (Given)
 *        Pointer to name of the file to be checked.
 *     fext = char * (Given)
 *        Pointer to the extension to be tested for.

 *  Returned:
 *     Returns the result of a strcmp( ) call comparing the relevant
 *     part of the supplied text with the supplied extension.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     {enter_further_changes_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-

 *  Arguments Given:
 */
char *fname,                     /* The file specification.                   */
char *fext                       /* A file extension, without ".".            */

)
{

/*
 *  Local Variables:
 */
char *cptr;

/*.
 */

/*Find end of file name.
 */
    cptr = fname;
    while ( *cptr ) {
        cptr++;
    }
    if ( cptr > fname ) {
        cptr--;
    }

/*Find last . in  file name and point to character after it.
 */
    while ( ( *cptr != '.' ) && ( cptr > fname ) ) {
        cptr--;
    }
    if ( cptr > fname ) {
        cptr++;
    }

/*Compare supplied extension with part of file name.
 */
    return ( !strcmp( cptr, fext) );
}


/*****************************************************************************/
void tar_zed(
/*+
 *  Name:
 *     tar_zed

 *  Purpose:
 *     Pipe a compressed tar archive of the requested files to stdout.

 *  Language:
 *     C

 *  Invocation:
 *     tar_zed( ent, n )

 *  Arguments:
 *     ent = entry * (Given)
 *        Pointer to first item in a list of "entry" structures.
 *     n = int (Given)
 *        Total number of items in list.

 *  Authors:
 *     MJC: Martin Clayton (Starlink)
 *     {enter_new_authors_here}

 *  History:
 *     14-DEC-1994 (MJC):
 *       Original Version.
 *     {enter_further_changes_here}

 *  Bugs:
 *     {note_any_bugs_here}

 *-

 *  Arguments Given:
 */
entry *ent,                      /* Pointer to start of list.                 */

int n                            /* Number of items in list.                  */

)
{

/*
 *  Local Variables:
 */
char request_text[65536];        /* Buffer for command line.                  */

int i;                           /* Loop index.                               */

/*.
 */

/*A null request was received, return message.
 */
    if ( n < 1 ) {
        printf( "Content-type: text/html%c%c", NEW_LINE, NEW_LINE );
        printf( "<B>Compressed tar file down-load request detected.</B><P>%c",
                NEW_LINE );
        printf( "<B>Your request was empty!</B>%c", NEW_LINE );

/*Otherwise build command line to compress tar file to stdout.
 */
    } else {

/*    ent[].val is the same foe all entries, the base directory for
 *    the archive build.  Mark a "change directory" command for this.
 */
        strcpy( request_text, "cd " );
        strcat( request_text, ent[1].val );

/*    Add tar command with flags for pipe to stdout and follow soft links.
 */
        strcat( request_text, " ; tar -cfh - " );

/*    Now add all the list entries.
 */
        for ( i = 1; i <= n; i++ ) {
            strcat( request_text, ent[i].name );
            strcat( request_text, " " );
        }

/*    Finally, the pipe to compress with flag to send output to stdout.
 */
        strcat( request_text, " | compress -c" );

/*    Start transfer.
 */
        printf( "Content-Encoding: x-compress%c%c", NEW_LINE, NEW_LINE );

/*    Flush output.
 */
        fflush ( stdout );

/*    Execute compress archive command line.
 */
        if ( system( request_text ) ) {
            printf( "Content-type: text/html%c%c", NEW_LINE, NEW_LINE );
            printf( "<B>Transfer failed.</B><P>%c", NEW_LINE );
        }
    }

/*Done.
 */
return;
}

/*End-of-file.
 */

