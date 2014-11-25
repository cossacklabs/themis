/*
** Splint - annotation-assisted static program checker
** Copyright (C) 1994-2003 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on splint: info@splint.org
** To report a bug: splint-bug@splint.org
** For more information: http://www.splint.org
*/
/*
** help.c
*/

# include "splintMacros.nf"
# include "basic.h"
# include "help.h"
# include "osd.h"
# include "llmain.h"
# include "version.h"

static void
describeVars (void)
{
  cstring eval;
  cstring def;

  eval = context_getLarchPath ();
  def = osd_getEnvironmentVariable (LARCH_PATH);

  if (cstring_isDefined (def) || 
      !cstring_equal (eval, cstring_fromChars (DEFAULT_LARCHPATH)))
    {
      llmsg (message ("LARCH_PATH = %s", eval));
    }
  else
    {
      llmsg (message ("LARCH_PATH = <not set> (default = %s)",
		      cstring_fromChars (DEFAULT_LARCHPATH)));
    }
  
  llmsglit ("   --- path used to find larch initialization files and LSL traits");

  eval = context_getLCLImportDir ();
  def = osd_getEnvironmentVariable (cstring_makeLiteralTemp (LCLIMPORTDIR));

  if (cstring_isDefined (def) ||
      !cstring_equal (eval, cstring_fromChars (DEFAULT_LCLIMPORTDIR)))
    {
      llmsg (message ("%q = %s", cstring_makeLiteral (LCLIMPORTDIR), eval));
    }
  else
    {
      llmsg (message ("%s = <not set, default: %s>", cstring_makeLiteralTemp (LCLIMPORTDIR), 
		      cstring_makeLiteralTemp (DEFAULT_LCLIMPORTDIR))); 
    }
  
  llmsglit ("   --- directory containing lcl standard library files "
	    "(import with < ... >)");;

  llmsg (message 
	 ("include path = %q (set by environment variable %s and -I flags)",
	  cppReader_getIncludePath (), INCLUDEPATH_VAR));

  llmsglit ("   --- path used to find #include'd files");

  llmsg (message 
	 ("systemdirs = %s (set by -systemdirs or environment variable %s)", 
	  context_getString (FLG_SYSTEMDIRS),
	  INCLUDEPATH_VAR));

  llmsglit ("   --- if file is found on this path, it is treated as a system file for error reporting");
}

static void
printAnnotations (void)
{
  llmsglit ("Annotations");
  llmsglit ("-----------");
  llmsglit ("");
  llmsglit ("Annotations are semantic comments that document certain "
	    "assumptions about functions, variables, parameters, and types. ");
  llmsglit ("");
  llmsglit ("They may be used to indicate where the representation of a "
	    "user-defined type is hidden, to limit where a global variable may "
	    "be used or modified, to constrain what a function implementation "
            "may do to its parameters, and to express checked assumptions about "
	    "variables, types, structure fields, function parameters, and "
	    "function results.");
  llmsglit ("");
  llmsglit ("Annotations are introduced by \"/*@\". The role of the @ may be "
	    "played by any printable character, selected using -commentchar <char>.");
  llmsglit ("");
  llmsglit ("Consult the User's Guide for descriptions of checking associated with each annotation.");
  llmsglit ("");
  llmsglit ("Globals: (in function declarations)");
  llmsglit ("   /*@globals <globitem>,+ @*/");
  llmsglit ("      globitem is an identifier, internalState or fileSystem");
  llmsglit ("");
  llmsglit ("Modifies: (in function declarations)");
  llmsglit ("   /*@modifies <moditem>,+ @*/");
  llmsglit ("      moditem is an lvalue");
  llmsglit ("   /*@modifies nothing @*/");
  llmsglit ("   /*@*/   (Abbreviation for no globals and modifies nothing.)");
  llmsglit ("");
  llmsglit ("Iterators:");
  llmsglit ("   /*@iter <identifier> (<parameter-type-list>) @*/ - declare an iterator");
  llmsglit ("");
  llmsglit ("Constants:");
  llmsglit ("   /*@constant <declaration> @*/ - declares a constant");
  llmsglit ("");
  llmsglit ("Alternate Types:");
  llmsglit ("   /*@alt <basic-type>,+ @*/");
  llmsglit ("   (e.g., int /*@alt char@*/ is a type matching either int or char)");
  llmsglit ("");
  llmsglit ("Declarator Annotations");
  llmsglit ("");
  llmsglit ("Type Definitions:");
  llmsglit ("   /*@abstract@*/ - representation is hidden from clients");
  llmsglit ("   /*@concrete@*/ - representation is visible to clients");
  llmsglit ("   /*@immutable@*/ - instances of the type cannot change value");
  llmsglit ("   /*@mutable@*/ - instances of the type can change value");
  llmsglit ("   /*@refcounted@*/ - reference counted type");
  llmsglit ("");
  llmsglit ("Global Variables:");
  llmsglit ("   /*@unchecked@*/ - weakest checking for global use");
  llmsglit ("   /*@checkmod@*/ - check modification by not use of global");
  llmsglit ("   /*@checked@*/ - check use and modification of global");
  llmsglit ("   /*@checkedstrict@*/ - check use of global strictly");
  llmsglit ("");
  llmsglit ("Memory Management:");
  llmsglit ("   /*@dependent@*/ - a reference to externally-owned storage");
  llmsglit ("   /*@keep@*/ - a parameter that is kept by the called function");
  llmsglit ("   /*@killref@*/ - a refcounted parameter, killed by the call");
  llmsglit ("   /*@only@*/ - an unshared reference");
  llmsglit ("   /*@owned@*/ - owner of storage that may be shared by /*@dependent@*/ references");
  llmsglit ("   /*@shared@*/ - shared reference that is never deallocated");
  llmsglit ("   /*@temp@*/ - temporary parameter");
  llmsglit ("");
  llmsglit ("Aliasing:");
  llmsglit ("   /*@unique@*/ - may not be aliased by any other visible reference");
  llmsglit ("   /*@returned@*/ - may be aliased by the return value");
  llmsglit ("");
  llmsglit ("Exposure:");
  llmsglit ("   /*@observer@*/ - reference that cannot be modified");
  llmsglit ("   /*@exposed@*/ - exposed reference to storage in another object");
  llmsglit ("");
  llmsglit ("Definition State:");
  llmsglit ("   /*@out@*/ - storage reachable from reference need not be defined");
  llmsglit ("   /*@in@*/ - all storage reachable from reference must be defined");
  llmsglit ("   /*@partial@*/ - partially defined, may have undefined fields");
  llmsglit ("   /*@reldef@*/ - relax definition checking");
  llmsglit ("");
  llmsglit ("Global State: (for globals lists, no /*@, since list is already in /*@\'s)");
  llmsglit ("   undef - variable is undefined before the call");
  llmsglit ("   killed - variable is undefined after the call");
  llmsglit ("");
  llmsglit ("Null State:");
  llmsglit ("   /*@null@*/ - possibly null pointer");
  llmsglit ("   /*@notnull@*/ - definitely non-null pointer");
  llmsglit ("   /*@relnull@*/ - relax null checking");
  llmsglit ("");
  llmsglit ("Null Predicates:");
  llmsglit ("   /*@nullwhentrue@*/ - if result is TRUE, first parameter is NULL");
  llmsglit ("   /*@falsewhennull@*/ - if result is TRUE, first parameter is not NULL");
  llmsglit ("");
  llmsglit ("Execution:");
  llmsglit ("   /*@noreturn@*/ - function never returns");
  llmsglit ("   /*@maynotreturn@*/ - function may or may not return");
  llmsglit ("   /*@noreturnwhentrue@*/ - function does not return if first parameter is TRUE");
  llmsglit ("   /*@noreturnwhenfalse@*/ - function does not return if first parameter if FALSE");
  llmsglit ("   /*@alwaysreturns@*/ - function always returns");
  llmsglit ("");
  llmsglit ("Side-Effects:");
  llmsglit ("   /*@sef@*/ - corresponding actual parameter has no side effects");
  llmsglit ("");
  llmsglit ("Declaration:");
  llmsglit ("   /*@unused@*/ - need not be used (no unused errors reported)");
  llmsglit ("   /*@external@*/ - defined externally (no undefined error reported)");
  llmsglit ("");
  llmsglit ("Case:");
  llmsglit ("   /*@fallthrough@*/ - fall-through case");
  llmsglit ("");
  llmsglit ("Break:");
  llmsglit ("   /*@innerbreak@*/ - break is breaking an inner loop or switch");
  llmsglit ("   /*@loopbreak@*/ - break is breaking a loop");
  llmsglit ("   /*@switchbreak@*/ - break is breaking a switch");
  llmsglit ("   /*@innercontinue@*/ - continue is continuing an inner loop");
  llmsglit ("");
  llmsglit ("Unreachable Code:");
  llmsglit ("   /*@notreached@*/ - statement may be unreachable.");
  llmsglit ("");
  llmsglit ("Special Functions:");
  llmsglit ("   /*@printflike@*/ - check variable arguments like printf");
  llmsglit ("   /*@scanflike@*/ - check variable arguments like scanf");
}

static void
printComments (void)
{
  llmsglit ("Control Comments");
  llmsglit ("----------------");
  llmsglit ("");
  llmsglit ("Setting Flags");
  llmsglit ("");
  llmsglit ("Most flags (all except those characterized as \"globally-settable only\") can be set locally using control comments. A control comment can set flags locally to override the command line settings. The original flag settings are restored before processing the next file.");
  llmsglit ("");
  llmsglit ("The syntax for setting flags in control comments is the same as that of the command line, except that flags may also be preceded by = to restore their setting to the original command-line value. For instance,");
  llmsglit ("   /*@+boolint -modifies =showfunc@*/");
  llmsglit ("sets boolint on (this makes bool and int indistinguishable types), sets modifies off (this prevents reporting of modification errors), and sets showfunc to its original setting (this controls  whether or not the name of a function is displayed before a message).");
  llmsglit ("");
  llmsglit ("Error Suppression");
  llmsglit ("");
  llmsglit ("Several comments are provided for suppressing messages. In general, it is usually better to use specific flags to suppress a particular error permanently, but the general error suppression flags may be more convenient for quickly suppressing messages for code that will be corrected or documented later.");
  llmsglit ("");
  llmsglit ("/*@ignore@*/ ... /*@end@*/");
  llgenindentmsgnoloc
    (cstring_makeLiteral 
     ("No errors will be reported in code regions between /*@ignore@*/ and /*@end@*/. These comments can be used to easily suppress an unlimited number of messages."));
  llmsglit ("/*@i@*/");
    llgenindentmsgnoloc
    (cstring_makeLiteral 
     ("No errors will be reported from an /*@i@*/ comment to the end of the line."));
  llmsglit ("/*@i<n>@*/");
  llgenindentmsgnoloc
    (cstring_makeLiteral 
     ("No errors will be reported from an /*@i<n>@*/ (e.g., /*@i3@*/) comment to the end of the line. If there are not exactly n errors suppressed from the comment point to the end of the line, Splint will report an error."));
  llmsglit ("/*@t@*/, /*@t<n>@*/");
  llgenindentmsgnoloc
    (cstring_makeLiteral 
     ("Like i and i<n>, except controlled by +tmpcomments flag. These can be used to temporarily suppress certain errors. Then, -tmpcomments can be set to find them again."));
  llmsglit ("");
  llmsglit ("Type Access");
  llmsglit ("");
  llmsglit ("/*@access <type>@*/"); 
  llmsglit ("   Allows the following code to access the representation of <type>");
  llmsglit ("/*@noaccess <type>@*/");
  llmsglit ("   Hides the representation of <type>");
  llmsglit ("");
  llmsglit ("Macro Expansion");
  llmsglit ("");
  llmsglit ("/*@notfunction@*/");
  llgenindentmsgnoloc 
    (cstring_makeLiteral
     ("Indicates that the next macro definition is not intended to be a "
      "function, and should be expanded in line instead of checked as a "
      "macro function definition."));
}
  
static void
printFlags (void)
{
  llmsglit ("Flag Categories");
  llmsglit ("---------------");
  listAllCategories ();
  llmsglit ("\nTo see the flags in a flag category, do\n   splint -help flags <category>");
  llmsglit ("To see a list of all flags in alphabetical order, do\n   splint -help flags alpha");
  llmsglit ("To see a full description of all flags, do\n   splint -help flags full");
}

static void
printMaintainer (void)
{
  llmsg (message ("Maintainer: %s", cstring_makeLiteralTemp (SPLINT_MAINTAINER)));
  llmsglit (LCL_COMPILE);
}

static void
printMail (void)
{
  llmsglit ("Mailing Lists");
  llmsglit ("-------------");
  llmsglit ("");
  llmsglit ("There are two mailing lists associated with Splint: ");
  llmsglit ("");
  llmsglit ("   splint-announce@cs.virginia.edu");
  llmsglit ("");
  llmsglit ("      Reserved for announcements of new releases and bug fixes.");
  llmsglit ("");
  llmsglit ("   splint-discuss@virginia.edu");
  llmsglit ("");
  llmsglit ("      Informal discussions on the use and development of Splint.");
  llmsglit ("");
  llmsglit ("To subscribe or view archives, visit http://www.splint.org/lists.html");
}

static void
printReferences (void)
{
  llmsglit ("References");
  llmsglit ("----------");
  llmsglit ("");
  llmsglit ("For more information, see the Splint web site: http://www.splint.org");
}

static void
describePrefixCodes (void)
{
  llmsglit ("Prefix Codes");
  llmsglit ("------------");
  llmsglit ("");
  llmsglit ("These characters have special meaning in name prefixes:");
  llmsglit ("");
  llmsg (message ("   %h  Any uppercase letter [A-Z]", PFX_UPPERCASE));
  llmsg (message ("   %h  Any lowercase letter [a-z]", PFX_LOWERCASE));
  llmsg (message ("   %h  Any character (valid in a C identifier)", PFX_ANY));
  llmsg (message ("   %h  Any digit [0-9]", PFX_DIGIT));
  llmsg (message ("   %h  Any non-uppercase letter [a-z0-9_]", PFX_NOTUPPER));
  llmsg (message ("   %h  Any non-lowercase letter [A-Z0-9_]", PFX_NOTLOWER));
  llmsg (message ("   %h  Any letter [A-Za-z]", PFX_ANYLETTER));
  llmsg (message ("   %h  Any letter or digit [A-Za-z0-9]", PFX_ANYLETTERDIGIT));
  llmsglit ("   *  Zero or more repetitions of the previous character class until the end of the name");
}

void help_showAvailableHelp (void)
{
  showHerald ();
  
  llmsg (message ("Source files are .c, .h and %s files.  If there is no suffix,",
		  LCL_EXTENSION));
  llmsg (message ("   Splint will look for <file>.c and <file>%s.", LCL_EXTENSION));
  llmsglit ("");
  llmsglit ("Use splint -help <topic or flag name> for more information");
  llmsglit ("");
  llmsglit ("Topics:");
  llmsglit ("");
  llmsglit ("   annotations (describes source-code annotations)");
  llmsglit ("   comments (describes control comments)");
  llmsglit ("   flags (describes flag categories)");
  llmsglit ("   flags <category> (describes flags in category)");
  llmsglit ("   flags all (short description of all flags)");
  llmsglit ("   flags alpha (list all flags alphabetically)");
  llmsglit ("   flags full (full description of all flags)");
  llmsglit ("   mail (information on mailing lists)");
  llmsglit ("   modes (show mode settings)");
  llmsglit ("   parseerrors (help on handling parser errors)");
  llmsglit ("   prefixcodes (character codes in namespace prefixes)");
  llmsglit ("   references (sources for more information)");
  llmsglit ("   vars (environment variables)"); 
  llmsglit ("   version (information on compilation, maintainer)");
  llmsglit ("");
}

static bool
specialFlagsHelp (char *next)
{
  if ((next != NULL) && (*next != '-') && (*next != '+'))
    {
      if (mstring_equal (next, "alpha"))
	{
	  printAlphaFlags ();
	  return TRUE;
	}
      else if (mstring_equal (next, "all"))
	{
	  printAllFlags (TRUE, FALSE);
	  return TRUE;
	}
      else if (mstring_equal (next, "categories")
	       || mstring_equal (next, "cats"))
	{
	  listAllCategories ();
	  return TRUE;
	}
      else if (mstring_equal (next, "full"))
	{
	  printAllFlags (FALSE, TRUE);
	  return TRUE;
	}
      else if (mstring_equal (next, "manual"))
	{
	  printFlagManual (FALSE);
	  return TRUE;
	}
      else if (mstring_equal (next, "webmanual"))
	{
	  printFlagManual (TRUE);
	  return TRUE;
	}
      else
	{
	  return FALSE;
	}
    }
  else
    {
      return FALSE;
    }
}

static void
printParseErrors (void)
{
  llmsglit ("Parse Errors");
  llmsglit ("------------");
  llmsglit ("");
  llmsglit ("Splint will sometimes encounter a parse error for code that "
	    "can be parsed with a local compiler. There are a few likely "
	    "causes for this and a number of techniques that can be used "
	    "to work around the problem.");
  llmsglit ("");
  llmsglit ("Compiler extensions --- compilers sometimes extend the C "
	    "language with compiler-specific keywords and syntax. While "
	    "it is not advisible to use these, oftentimes one has no choice "
	    "when the system header files use compiler extensions. ");
  llmsglit ("");
  llmsglit ("Splint supports some of the GNU (gcc) compiler extensions, "
	    "if the +gnuextensions flag is set. You may be able to workaround "
	    "other compiler extensions by using a pre-processor define. "
	    "Alternately, you can surround the unparseable code with");
  llmsglit ("");
  llmsglit ("   # ifndef S_SPLINT_S");
  llmsglit ("   ...");
  llmsglit ("   # endif");
  llmsglit ("");
  /* evans 2000-12-21 fixed typo reported by Jeroen Ruigrok/Asmodai */
  llmsglit ("Missing type definitions --- an undefined type name will usually "
	    "lead to a parse error. This often occurs when a standard header "
	    "file defines some type that is not part of the standard library. ");
  llmsglit ("By default, Splint does not process the local files corresponding "
	    "to standard library headers, but uses a library specification "
	    "instead so dependencies on local system headers can be detected. "
	    "If another system header file that does not correspond to a "
	    "standard library header uses one of these superfluous types, "
	    "a parse error will result.");
  llmsglit ("");
  llmsglit ("If the parse error is inside a posix standard header file, the "
	    "first thing to try is +posixlib. This makes Splint use "
	    "the posix library specification instead of reading the posix "
	    "header files.");
  llmsglit ("");
  llmsglit ("Otherwise, you may need to either manually define the problematic "
	    "type (e.g., add -Dmlink_t=int to your .splintrc file) or force "
	    "splint to process the header file that defines it. This is done "
	    "by setting -skipisoheaders or -skipposixheaders before "
	    "the file that defines the type is #include'd.");
  llmsglit ("(See splint -help "
	    "skipisoheaders and splint -help skipposixheaders for a list of "
	    "standard headers.)  For example, if <sys/local.h> uses a type "
	    "defined by posix header <sys/types.h> but not defined by the "
	    "posix library, we might do: ");
  llmsglit ("");
  llmsglit ("   /*@-skipposixheaders@*/");
  llmsglit ("   # include <sys/types.h>");
  llmsglit ("   /*@=skipposixheaders@*/");
  llmsglit ("   # include <sys/local.h>");
  llmsglit ("");
  llmsglit ("to force Splint to process <sys/types.h>.");
  llmsglit ("");
  llmsglit ("At last resort, +trytorecover can be used to make Splint attempt "
	    "to continue after a parse error.  This is usually not successful "
	    "and the author does not consider assertion failures when +trytorecover "
	    "is used to be bugs.");
}

void help_processFlags (int argc, char **argv)
{
  int i;

  showHerald ();

  if (argc == 0)
    {
      help_showAvailableHelp ();
    }

  for (i = 0; i < argc; i++)
    {
      char *thisarg = argv[i];
      
      if (*thisarg == '-' || *thisarg == '+')
	{
	  thisarg++;	/* skip '-' */
	}
      if (mstring_equal (thisarg, "modes"))
	{
	  llmsg (describeModes ());
	}
      else if (mstring_equal (thisarg, "vars")  
	       || mstring_equal (thisarg, "env"))
	{
	  describeVars ();
	}
      else if (mstring_equal (thisarg, "annotations"))
	{
	  printAnnotations ();
	}
      else if (mstring_equal (thisarg, "parseerrors"))
	{
	  printParseErrors ();
	}
      else if (mstring_equal (thisarg, "comments"))
	{
	  printComments ();
	}
      else if (mstring_equal (thisarg, "prefixcodes"))
	{
	  describePrefixCodes ();
	}
      else if (mstring_equal (thisarg, "references") 
	       || mstring_equal (thisarg, "refs"))
	{
	  printReferences ();
	}
      else if (mstring_equal (thisarg, "mail"))
	{
	  printMail ();
	}
      else if (mstring_equal (thisarg, "maintainer")
	       || mstring_equal (thisarg, "version"))
	{
	  printMaintainer ();
	}
      else if (flags_isModeName (cstring_fromChars (thisarg)))
	{
	  llmsg (describeMode (cstring_fromChars (thisarg)));
	}
      else if (mstring_equal (thisarg, "flags"))
	{
	  if (i + 1 < argc)
	    {
	      char *next = argv[i + 1];
	      
	      if (specialFlagsHelp (next))
		{
		  i++;
		}
	      else
		{
		  flagkind k = identifyCategory (cstring_fromChars (next));
		  
		  if (k != FK_NONE)
		    {
		      printCategory (k);
		      i++;
		    }
		}
	    }
	  else
	    {
	      printFlags ();
	    }
	}
      else
	{
	  cstring s = describeFlag (cstring_fromChars (thisarg));
	  
	  if (cstring_isDefined (s))
	    {
	      llmsg (s);
	    }
	}
    }
}
