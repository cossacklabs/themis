/*
** linux.h
*/

/*
** Based on Linux Standard Base Specification 1.0.0 
** http://www.linuxbase.org/spec/gLSB/gLSB/libc-ddefs.html
*/

/*

Data Definitions for libc

dirent.h
*/

typedef struct __dirstream DIR;

/* Not in LSB: based on RedHat 7.0: */

struct dirent {
#ifndef __USE_FILE_OFFSET64
    __ino_t d_ino;
    __off_t d_off;
#else
    __ino64_t d_ino;
    __off64_t d_off;
#endif
  unsigned short int d_reclen;
  unsigned char d_type;
  char d_name[256];		/* We must not include limits.h! */
} ; 

/*@i43 struct dirent not in LSB spec
       why doesn't alphasort64 use struct dirent64's?
*/

int acct (/*@null@*/ const char *filename) 
   /*:statusreturn@*/
   /*@modifies systemState@*/ ;

int alphasort (const struct dirent **a, const struct dirent **b) /*@*/ ;
int alphasort64(const struct dirent **a, const struct dirent **b) /*@*/ ; 

/*
ftw.h
*/

typedef int (*__ftw_func_t) (char *__filename, struct stat * __status,
			     int __flag);

typedef int (*__ftw64_func_t) (char *__filename, struct stat64 * __status,
			       int __flag);

typedef int (*__nftw_func_t) (char *__filename, struct stat * __status,
			      int __flag, struct FTW * __info);

typedef int (*__nftw64_func_t) (char *__filename, struct stat64 * __status,
				int __flag, struct FTW * __info);

/* 
glob.h
*/

typedef struct
{
  __size_t gl_pathc;
  char **gl_pathv;
  __size_t gl_offs;
  int gl_flags;
  void (*gl_closedir) ();
  struct dirent64 *(*gl_readdir) ();
  void *(*gl_opendir) ();
  int (*gl_lstat) ();
  int (*gl_stat) ();
}
glob_t;

typedef struct
{
  __size_t gl_pathc;
  char **gl_pathv;
  __size_t gl_offs;
  int gl_flags;
  void (*gl_closedir) ();
  struct dirent64 *(*gl_readdir) ();
  void *(*gl_opendir) ();
  int (*gl_lstat) ();
  int (*gl_stat) ();
}
glob64_t;
 
/*
grp.h
*/

struct group
{
  char *gr_name;
  char *gr_passwd;
  __gid_t gr_gid;
  char **gr_mem;
}
;
 
/*
iconv.h
*/

typedef void *iconv_t;
 
/*
inttypes.h
*/

typedef lldiv_t imaxdiv_t;

typedef long long intmax_t;

typedef unsigned long long uintmax_t;
 
/*
locale.h
*/

struct lconv
{
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
}
;
 
/*
nl_types.h
*/

typedef void *nl_catd;

typedef int nl_item;
 
/*
pwd.h
*/

struct passwd
{
  char *pw_name;
  char *pw_passwd;
  __uid_t pw_uid;
  __gid_t pw_gid;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
}
;
 
/*
regex.h
*/

typedef unsigned long reg_syntax_t;

typedef struct re_pattern_buffer
{
  unsigned char *buffer;
  unsigned long allocated;
  unsigned long used;
  reg_syntax_t syntax;
  char *fastmap;
  char *translate;
  size_t re_nsub;
  unsigned int can_be_null;
  unsigned int regs_allocated;
  unsigned int fastmap_accurate;
  unsigned int no_sub;
  unsigned int not_bol;
  unsigned int not_eol;
  unsigned int newline_anchor;
}
regex_t;

typedef int regoff_t;

typedef struct
{
  regoff_t rm_so;
  regoff_t rm_eo;
}
regmatch_t;
 
/*
search.h
*/

typedef enum
{
  FIND,
  ENTER
}
ACTION;

typedef enum
{
  preorder,
  postorder,
  endorder,
  leaf
}
VISIT;

typedef struct entry
{
  char *key;
  void *data;
}
ENTRY;

typedef void (*__action_fn_t) (void *__nodep, VISIT __value, int __level);
 
/*
setjmp.h
*/

typedef int __jmp_buf;

typedef struct __jmp_buf_tag
{
  __jmp_buf __jmpbuf;
  int __mask_was_saved;
  __sigset_t __saved_mask;
}
jmp_buf;

typedef jmp_buf sigjmp_buf;
 
/*
signal.h
*/

typedef union sigval
{
  int sival_int;
  void *sival_ptr;
}
sigval_t;

typedef void (*__sighandler_t) ();

struct sigevent
{
  sigval_t sigev_value;
  int sigev_signo;
  int sigev_notify;
}
;

typedef struct
{
  unsigned long __val[1];
}
__sigset_t;

typedef __sigset_t sigset_t;

struct sigaction;

typedef struct
{
  int si_band;
  int si_fd;
  struct _sifields;
  struct _sigpoll;
}
siginfo_t;

struct sigaltstack
{
  void *ss_sp;
  int ss_flags;
  size_t ss_size;
}
;

struct sigstack
{
  void *ss_sp;
  int ss_onstack;
}
;
 
/*
stdio.h
*/

typedef struct _IO_FILE FILE;

typedef __off_t fpos_t;

typedef __off64_t fpos64_t;
 
/*
stdlib.h
*/

typedef struct
{
  long long quot;
  long long rem;
}
lldiv_t;

typedef struct
{
  int quot;
  int rem;
}
div_t;

typedef struct
{
  long quot;
  long rem;
}
ldiv_t;

typedef int (*__compar_fn_t) ();
 
/*
sys/sem.h
*/

struct sembuf
{
  short sem_num;
  short sem_op;
  short sem_flg;
}
;
 
/*
sys/socket.h
*/

typedef unsigned short sa_family_t;

typedef unsigned int socklen_t;

struct sockaddr
{
  sa_family_t sa_family;
  char sa_data[1];
}
;

struct msghdr
{
  void *msg_name;
  socklen_t msg_namelen;
  struct iovec *msg_iov;
  size_t msg_iovlen;
  void *msg_control;
  size_t msg_controllen;
  int msg_flags;
}
;
 
/*
sys/times.h
*/

struct tms
{
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
}
;
 
/*
sys/utsname.h
*/

struct utsname
{
  char sysname[1];
  char nodename[1];
  char release[1];
  char version[1];
  char machine[1];
  char __domainname[1];
}
;
 
/*
sys/wait.h
*/

typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
}
idtype_t;
 
/*
time.h
*/

typedef long __clock_t;

struct timespec
{
  long tv_sec;
  long tv_nsec;
}
;

struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long __tm_gmtoff;
  char *__tm_zone;
}
;

typedef __clock_t clock_t;

typedef __time_t time_t;
 
int adjtime (/*@notnull@*/ const struct timeval *delta, /*@null@*/ struct timeval *olddelta)
     /*@warn superuser "Only super-user processes may call adjtime."@*/
     /*:statusreturn@*/ 
     /*@modifies systemState@*/ ;

/*
unistd.h
*/

typedef __ssize_t ssize_t;

typedef __pid_t pid_t;

typedef __off_t off_t;

extern /*@observer@*/ /*@nullterminated@*/ /*@null@*/ 
char *bindtextdomain (/*@null@*/ /*@nullterminated@*/ const char *domainname,
		      /*@null@*/ /*@nullterminated@*/ const char *dirname) /*@*/ ;
/* LSB doesn't specify: taken from
   http://www.hgmp.mrc.ac.uk/cgi-bin/man.cgi?section=3C&topic=bindtextdomain
*/
 

/*
utime.h
*/

struct utimbuf
{
  __time_t actime;
  __time_t modtime;
}
;
 
/*
wchar.h
*/

typedef long wchar_t;

typedef unsigned int wint_t;

typedef struct
{
  int count;
  wint_t value;
}
mbstate_t;
 
/*
wctype.h
*/

typedef unsigned long wctype_t;

typedef __int32_t *wctrans_t;
 
/*
wordexp.h
*/

typedef struct
{
  int we_wordc;
  char **we_wordv;
  int we_offs;
}
wordexp_t;
 
/*
timex.h
*/

struct timex {
  int modes; /* mode selector */
  long offset; /* time offset (usec) */
  long freq; /* frequency offset (scaled ppm) */
  long maxerror; /* maximum error (usec) */
  long esterror; /* estimated error (usec) */
  int status; /* clock command/status */
  long constant; /* pll time constant */
  long precision; /* clock precision (usec) (read only) */
  long tolerance; /* clock frequency tolerance (ppm) (read only) */
  struct timeval time; /* current time (read only) */
  long tick; /* usecs between clock ticks */

};

/*@constant int ADJ_OFFSET@*/
/*@constant int ADJ_FREQUENCY@*/
/*@constant int ADJ_MAXERROR@*/
/*@constant int ADJ_ESTERROR@*/
/*@constant int ADJ_STATUS@*/
/*@constant int ADJ_TIMECONST@*/
/*@constant int ADJ_TICK@*/
/*@constant int ADJ_OFFSET_SINGLESHOT@*/

int adjtimex (/*@notnull@*/ struct timex *buf)
     /*@warn superuser "Only super-user processes may call adjtimex unless the modes field is 0."@*/
     /*:statusreturn@*/
     /*@modifies systemState@*/ ;

/*
termios.h
*/

typedef unsigned char	cc_t;
typedef unsigned int	speed_t;
typedef unsigned int	tcflag_t;

struct termios
{
  tcflag_t c_iflag;		/* input mode flags */
  tcflag_t c_oflag;		/* output mode flags */
  tcflag_t c_cflag;		/* control mode flags */
  tcflag_t c_lflag;		/* local mode flags */
  cc_t c_cc[NCCS];		/* control characters */
};

int cfmakeraw (/*@out@*/ struct termios *termios_p)
     /*:statusreturn@*/ ; 

int cfsetspeed(struct termios *t, speedt speed)
     /*:statusreturn@*/ ;
   
/*
** Built in
*/

/*@exits@*/ void __assert_fail (/*@notnull@*/ const char *assertion,
				/*@notnull@*/ const char *file,
				unsigned int line,
				/*@null@*/ const char *function) 
     /*@modifies stderr@*/;
