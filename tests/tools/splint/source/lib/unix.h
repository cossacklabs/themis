/*
** unix.h
*/

/*@-nextlinemacros@*/

/*
** sys/types.h
**
** evans - 2001-08-27: from http://www.opengroup.org/onlinepubs/007908799/xsh/systypes.h.html
*/

typedef /*@integraltype@*/ blkcnt_t;
typedef /*@integraltype@*/ blksize_t;

/*@-redef@*/ /* These are also defined by ansi.h: */
typedef /*@integraltype@*/ clock_t;
typedef /*@integraltype@*/ dev_t;
typedef /*@integraltype@*/ gid_t;
typedef /*@unsignedintegraltype@*/ ino_t;
typedef /*@integraltype@*/ mode_t;
typedef /*@integraltype@*/ nlink_t;
typedef /*@integraltype@*/ off_t;
typedef /*@integraltype@*/ pid_t;
typedef /*@integraltype@*/ time_t;
typedef /*@integraltype@*/ uid_t;

/*@=redef@*/

typedef /*@integraltype@*/ clockid_t;
typedef /*@unsignedintegraltype@*/ fsblkcnt_t;
typedef /*@unsignedintegraltype@*/ fsfilcnt_t;
typedef /*@integraltype@*/ id_t;

typedef /*@integraltype@*/ key_t;
typedef /*@integraltype@*/ pthread_attr_t;
typedef /*@integraltype@*/ pthread_cond_t;
typedef /*@integraltype@*/ pthread_condattr_t;
typedef /*@integraltype@*/ pthread_key_t;
typedef /*@integraltype@*/ pthread_mutex_t;
typedef /*@integraltype@*/ pthread_mutexattr_t;
typedef /*@integraltype@*/ pthread_once_t;
typedef /*@integraltype@*/ pthread_rwlock_t;
typedef /*@integraltype@*/ pthread_rwlockattr_t;
typedef /*@integraltype@*/ pthread_t;
typedef /*@signedintegraltype@*/ suseconds_t;
typedef /*@integraltype@*/ timer_t;
typedef /*@unsignedintegraltype@*/ useconds_t;

/*
** Extra stuff in some unixen, not in posix.
*/

extern /*@unchecked@*/ int signgam;

/*@-redef@*/ /* Defined by ansi: */
typedef /*@integraltype@*/ clockid_t;
/*@=redef@*/

extern void bcopy (char *b1, /*@out@*/ char *b2, int length) 
   /*@modifies *b2@*/ ;  /* Yes, the second parameter is the out param! */

extern int /*@alt _Bool@*/ bcmp (char *b1, char *b2, int length) /*@*/ ;
   /* Return value is NOT like strcmp! */

extern void bzero (/*@out@*/ char *b1, int length) /*@modifies *b1@*/ ;
extern int ffs (int i) /*@*/ ;
extern int symlink (char *name1, char *name2) /*@modifies fileSystem@*/ ;

extern int 
  setvbuf_unlocked (FILE *stream, /*@null@*/ /*@exposed@*/ char *buf, 
		    int mode, size_t size)
  /*@modifies internalState@*/ ;

extern void 
  setbuffer (FILE *stream, /*@null@*/ /*@exposed@*/ char *buf, int size)
  /*@modifies internalState@*/ ;

extern void setlinebuf (FILE *stream) /*@modifies internalState@*/ ;

extern int strerror_r (int errnum, /*@out@*/ char *strerrbuf, int buflen)
  /*@modifies strerrbuf@*/ ;

extern size_t 
  fread_unlocked (/*@out@*/ void *ptr, size_t size, size_t nitems, 
		  FILE *stream) 
  /*@modifies *stream, *ptr;@*/ ;

extern size_t 
  fwrite_unlocked (void *pointer, size_t size, size_t num_items, FILE *stream)
  /*@modifies *stream;@*/ ;

extern void /*@alt void * @*/ 
  memccpy (/*@returned@*/ /*@out@*/ void *s1, 
	   /*@unique@*/ void *s2, int c, size_t n) 
  /*@modifies *s1@*/ ;

extern int strcasecmp (char *s1, char *s2) /*@*/ ;
extern int strncasecmp (char *s1, char *s2, int n) /*@*/ ;
extern /*@null@*/ /*@only@*/ char *strdup (char *s) /*@*/ ;

extern /*@null@*/ /*@dependent@*/ char *
  index (/*@returned@*/ char *s, char c) /*@*/ ;

extern /*@null@*/ /*@dependent@*/ char *
  rindex (/*@returned@*/ char *s, char c) /*@*/ ;

# if 0
These are in ISO C99.  Moved to standard.h:
   extern double cbrt (double x) /*@modifies errno@*/ ;
   extern double rint (double x) /*@*/ ;
   extern double trunc (double x) /*@*/ ; 
# endif


/*@constant int ENOTBLK@*/
/*@constant int ETXTBSY@*/
/*@constant int EWOULDBLOCK@*/
/*@constant int EINPROGRESS@*/
/*@constant int EALREADY@*/
/*@constant int ENOTSOCK@*/
/*@constant int EDESTADDRREQ@*/
/*@constant int EMSGSIZE@*/
/*@constant int EPROTOTYPE@*/
/*@constant int ENOPROTOOPT@*/
/*@constant int EPROTONOSUPPORT@*/
/*@constant int ESOCKTNOSUPPORT@*/
/*@constant int EOPNOTSUPP@*/
/*@constant int EPFNOSUPPORT@*/
/*@constant int EAFNOSUPPORT@*/
/*@constant int EADDRINUSE@*/
/*@constant int EADDRNOTAVAIL@*/
/*@constant int ENETDOWN@*/
/*@constant int ENETUNREACH@*/
/*@constant int ENETRESET@*/
/*@constant int ECONNABORTED@*/
/*@constant int ECONNRESET@*/
/*@constant int ENOBUFS@*/
/*@constant int EISCONN@*/
/*@constant int ENOTCONN@*/
/*@constant int ESHUTDOWN@*/
/*@constant int ETOOMANYREFS@*/
/*@constant int ETIMEDOUT@*/
/*@constant int ECONNREFUSED@*/
/*@constant int ENAMETOOLONG@*/
/*@constant int EHOSTDOWN@*/
/*@constant int EHOSTUNREACH@*/
/*@constant int ENOTEMPTY@*/
/*@constant int EPROCLIM@*/
/*@constant int EUSERS@*/
/*@constant int EDQUOT@*/
/*@constant int ESTALE@*/
/*@constant int EREMOTE@*/
/*@constant int ENOMSG@*/
/*@constant int EIDRM@*/
/*@constant int EALIGN@*/
/*@constant int EDEADLK@*/
/*@constant int ENOLCK@*/
/*@constant int ENOSYS@*/
/*@constant int EACTIVE@*/
/*@constant int ENOACTIVE@*/
/*@constant int ENORESOURCES@*/
/*@constant int ENOSYSTEM@*/
/*@constant int ENODUST@*/
/*@constant int EDUPNOCONN@*/
/*@constant int EDUPNODISCONN@*/
/*@constant int EDUPNOTCNTD@*/
/*@constant int EDUPNOTIDLE@*/
/*@constant int EDUPNOTWAIT@*/
/*@constant int EDUPNOTRUN@*/
/*@constant int EDUPBADOPCODE@*/
/*@constant int EDUPINTRANSIT@*/
/*@constant int EDUPTOOMANYCPUS@*/
/*@constant int ELOOP@*/

/*@constant int LOCK_MAX@*/
/*@constant int FCHR_MAX@*/
/*@constant int USI_MAX@*/
/*@constant int WORD_BIT@*/
/*@constant int LONG_BIT@*/

/*@-incondefs@*/ /* some constant are also declared in posix.h*/

/*@constant long NAME_MAX@*/

/*@constant long NGROUPS_MAX@*/

/*@constant long MAX_CANON@*/
/*@constant int MAX_CHAR@*/
/*@constant long OPEN_MAX@*/
/*@constant int PASS_MAX@*/

/*@constant int PID_MAX@*/
/*@constant int SYSPID_MAX@*/
/*@constant long PIPE_BUF@*/
/*@=incondefs@*/
/*@constant int PIPE_MAX@*/
/*@constant int PROC_MAX@*/
/*@constant int STD_BLK@*/
/*@constant int SYS_NMLN@*/
/*@constant int SYS_OPEN@*/
/*@constant int NZERO@*/
/*@constant int UID_MAX@*/
/*@constant long MB_LEN_MAX@*/
/*@constant int NL_ARGMAX@*/
/*@constant int NL_MSGMAX@*/
/*@constant int NL_NMAX@*/
/*@constant int NL_SETMAX@*/
/*@constant int NL_TEXTMAX@*/
/*@constant int NL_LBLMAX@*/
/*@constant int NL_LANGMAX @*/

/*@constant double M_E@*/
/*@constant double M_LOG2E@*/
/*@constant double M_LOG10E@*/
/*@constant double M_LN2@*/
/*@constant double M_LN10@*/
/*@constant double M_PI@*/
/*@constant double M_PI_2@*/
/*@constant double M_PI_4@*/
/*@constant double M_1_PI@*/
/*@constant double M_2_PI@*/
/*@constant double M_2_SQRTPI@*/
/*@constant double M_SQRT2@*/
/*@constant double M_SQRT1_2@*/ 

/*@constant double MAXFLOAT@*/
/*@constant double HUGE@*/
 
/*@constant int DOMAIN@*/
/*@constant int SING@*/
/*@constant int OVERFLOW@*/
/*@constant int UNDERFLOW@*/
/*@constant int TLOSS@*/
/*@constant int PLOSS@*/

extern /*@unchecked@*/ int daylight;
extern /*@unchecked@*/ long timezone;
extern /*@unchecked@*/ char *tzname[];

/*@-incondefs@*/
extern void tzset(void) /*@modifies daylight, timezone, tzname@*/ ;
/*@=incondefs@*/

/*@-redef@*/ /* Defined by ansi: */
typedef /*@integraltype@*/ key_t;
/*@-incondefs@*/ typedef long timer_t; /*@=incondefs@*/
/*@=redef@*/

typedef	unsigned char uchar_t;
typedef	unsigned short ushort_t;
typedef	unsigned int uint_t;
typedef unsigned long ulong_t;
typedef	volatile unsigned char vuchar_t;
typedef	volatile unsigned short	vushort_t;
typedef	volatile unsigned int vuint_t;
typedef volatile unsigned long vulong_t;
typedef long label_t;
typedef int level_t;
typedef	/*@integraltype@*/ daddr_t;
typedef	char *caddr_t;	
typedef long *qaddr_t; 
typedef char *addr_t;
typedef long physadr_t;
typedef short cnt_t;
typedef	int chan_t;	
typedef	int paddr_t;
typedef	void *mid_t;
typedef char slab_t[12];	
typedef ulong_t	shmatt_t;	
typedef ulong_t	msgqnum_t;	
typedef ulong_t	msglen_t;
typedef	uchar_t uchar;
typedef	ushort_t ushort;
typedef	uint_t uint;
typedef ulong_t	ulong;
typedef	uchar_t	u_char;
typedef	ushort_t u_short;
typedef	uint_t u_int;
typedef	ulong_t	u_long;
typedef	vuchar_t vu_char;
typedef	vushort_t vu_short;
typedef	vuint_t	vu_int;
typedef	vulong_t vu_long;
typedef	long swblk_t;
typedef u_long fixpt_t;
typedef long segsz_t;
typedef /*@abstract@*/ fd_set;

int ioctl (int d, int /*@alt long@*/ request, /*@out@*/ void *arg) 
   /*@modifies *arg, errno@*/ ;  /* depends on request! */

pid_t vfork (void) /*@modifies fileSystem@*/ ;

/*
** sys/uio.h
*/

struct iovec {
  /*@dependent@*/ void *iov_base;
  size_t iov_len; /*: maxSet(iov_base) = iov_len */
};

/* from limits.h */
/*@constant int UIO_MAXIOV@*/   /* BSD */
/*@constant int IOV_MAX@*/      /* supposedly SVR4 */

ssize_t readv (int fd, const struct iovec *iov, int iovcnt)
     /*@modifies iov->iov_base, fileSystem, errno@*/;

ssize_t writev (int fd, const struct iovec *iov, int iovcnt)
     /*@modifies errno@*/;

/*________________________________________________________________________
 * poll.h
 */

 struct poll {
	int fd;
	short events;
	short revents;
};

/*@constant short POLLIN@*/
/*@constant short POLLRDNORM@*/
/*@constant short POLLRDBAND@*/
/*@constant short POLLPRI@*/
/*@constant short POLLOUT@*/
/*@constant short POLLWRNORM@*/
/*@constant short POLLWRBAND@*/
/*@constant short POLLERR@*/
/*@constant short POLLHUP@*/
/*@constant short POLLNVAL@*/

extern int poll (struct poll pollfd[], unsigned long nfds, int timeout)
  /*@modifies pollfd[].revents, errno@*/ ;

/*
** free does not take null
*/

/*@-incondefs@*/
extern void free (/*@notnull@*/ /*@out@*/ /*@only@*/ void *p) /*@modifies *p@*/ ;
/*@=incondefs@*/

/*________________________________________________________________________
 * sys/socket.h
 */




/*@constant int SOCK_RDM@*/







/*@constant int SO_USELOOPBACK@*/


/*@constant int SO_REUSEPORT@*/










/*@constant int AF_LOCAL@*/


/*@constant int AF_IMPLINK@*/
/*@constant int AF_PUP@*/
/*@constant int AF_CHAOS@*/
/*@constant int AF_NS@*/
/*@constant int AF_ISO@*/
/*@constant int AF_OSI@*/
/*@constant int AF_ECMA@*/
/*@constant int AF_DATAKIT@*/
/*@constant int AF_CCITT@*/
/*@constant int AF_SNA@*/
/*@constant int AF_DECnet@*/
/*@constant int AF_DLI@*/
/*@constant int AF_LAT@*/
/*@constant int AF_HYLINK@*/
/*@constant int AF_APPLETALK@*/
/*@constant int AF_ROUTE@*/
/*@constant int AF_LINK@*/
/*@constant int pseudo_AF_XTP@*/
/*@constant int AF_COIP@*/
/*@constant int AF_CNT@*/
/*@constant int pseudo_AF_RTIP@*/
/*@constant int AF_IPX@*/
/*@constant int AF_SIP@*/
/*@constant int pseudo_AF_PIP@*/
/*@constant int AF_ISDN@*/
/*@constant int AF_E164@*/
/*@constant int AF_MAX@*/






/*@constant int MSG_DONTWAIT@*/
/*@constant int MSG_EOF@*/
/*@constant int MSG_COMPAT@*/
/*@constant int PF_UNSPEC@*/
/*@constant int PF_LOCAL@*/
/*@constant int PF_UNIX@*/
/*@constant int PF_INET@*/
/*@constant int PF_IMPLINK@*/
/*@constant int PF_PUP@*/
/*@constant int PF_CHAOS@*/
/*@constant int PF_NS@*/
/*@constant int PF_ISO@*/
/*@constant int PF_OSI@*/
/*@constant int PF_ECMA@*/
/*@constant int PF_DATAKIT@*/
/*@constant int PF_CCITT@*/
/*@constant int PF_SNA@*/
/*@constant int PF_DECnet@*/
/*@constant int PF_DLI@*/
/*@constant int PF_LAT@*/
/*@constant int PF_HYLINK@*/
/*@constant int PF_APPLETALK@*/
/*@constant int PF_ROUTE@*/
/*@constant int PF_LINK@*/
/*@constant int PF_XTP@*/
/*@constant int PF_COIP@*/
/*@constant int PF_CNT@*/
/*@constant int PF_SIP@*/
/*@constant int PF_IPX@*/
/*@constant int PF_RTIP@*/
/*@constant int PF_PIP@*/
/*@constant int PF_ISDN@*/
/*@constant int PF_MAX@*/
/*@constant int NET_MAXID@*/
/*@constant int NET_RT_DUMP@*/
/*@constant int NET_RT_FLAGS@*/
/*@constant int NET_RT_IFLIST@*/
/*@constant int NET_RT_MAXID@*/

/*moved this to before socket.h to get splint to parse the header*/
typedef /*@unsignedintegraltype@*/ sa_family_t;

/*
** sys/socket.h
** (updated 26 May 2002)
*/

typedef /*@unsignedintegraltype@*/ __socklen_t; /* not in USB, but needed by linux */
typedef /*@unsignedintegraltype@*/ socklen_t;

struct sockaddr {
  sa_family_t	sa_family;		/* address family */
  char          sa_data[];		/* variable length */
};

struct sockaddr_storage {
  sa_family_t ss_family;
} ;

struct msghdr {
  /*@dependent@*/ void *msg_name;		
  socklen_t msg_namelen;	/*: maxSet (msg_name) >= msg_namelen */
  /*@dependent@*/ struct iovec *msg_iov;	/* scatter/gather array */
  int msg_iovlen;		/* # elements in msg_iov */ /*: maxSet (msg_iov) >= msg_iovlen */
  /*@dependent@*/ void *msg_control;		/* ancillary data, see below */
  socklen_t msg_controllen;     /*: maxSet (msg_control) >= msg_controllen */
  int msg_flags;		/* flags on received message */
} ;

struct cmsghdr {
  socklen_t cmsg_len;		/* data byte count, including hdr */
  int cmsg_level;		/* originating protocol */
  int cmsg_type;		/* protocol-specific type */
} ;

/*@constant int SCM_RIGHTS@*/

/*@exposed@*/ unsigned char *CMSG_DATA (/*@sef@*/ struct cmsghdr *) /*@*/ ;
/*@null@*/ /*@exposed@*/ struct cmsghdr *CMSG_NXTHDR (struct msghdr *, struct cmsghdr *) /*@*/ ;
/*@null@*/ /*@exposed@*/ struct cmsghdr *CMSG_FIRSTHDR (struct msghdr *) /*@*/ ;

struct linger {
  int l_onoff;	
  int l_linger;	
};

/*@constant int SOCK_DGRAM@*/
/*@constant int SOCK_RAW@*/
/*@constant int SOCK_SEQPACKET@*/
/*@constant int SOCK_STREAM@*/

/*@constant int SOL_SOCKET@*/

/*@constant int SO_ACCEPTCONN@*/
/*@constant int SO_BROADCAST@*/
/*@constant int SO_DEBUG@*/
/*@constant int SO_DONTROUTE@*/
/*@constant int SO_ERROR@*/
/*@constant int SO_KEEPALIVE@*/
/*@constant int SO_LINGER@*/
/*@constant int SO_OOBINLINE@*/
/*@constant int SO_RCVBUF@*/
/*@constant int SO_RCVLOWAT@*/
/*@constant int SO_RCVTIMEO@*/
/*@constant int SO_REUSEADDR@*/
/*@constant int SO_SNDBUF@*/
/*@constant int SO_SNDLOWAT@*/
/*@constant int SO_SNDTIMEO@*/
/*@constant int SO_TYPE@*/

/*@constant int SOMAXCONN@*/

/*@constant int MSG_CTRUNC@*/
/*@constant int MSG_DONTROUTE@*/
/*@constant int MSG_EOR@*/
/*@constant int MSG_OOB@*/
/*@constant int MSG_PEEK@*/
/*@constant int MSG_TRUNC@*/
/*@constant int MSG_WAITALL@*/

/*@constant int AF_INET@*/
/*@constant int AF_INET6@*/
/*@constant int AF_UNIX@*/
/*@constant int AF_UNSPEC@*/

/*@constant int SHUT_RD@*/
/*@constant int SHUT_RDWR@*/
/*@constant int SHUT_WR@*/

# if 0
/*
** These were in the old unix.h spec, but are not in SUS6
*/

struct sockproto {
  u_short	sp_family;		/* address family */
  u_short	sp_protocol;		/* protocol */
};

# endif

int accept (int s, struct sockaddr *addr, int *addrlen)
  /*@modifies *addrlen, errno@*/;

int bind (int s, const struct sockaddr *name, int namelen)
  /*@modifies errno, fileSystem@*/;

int connect (int s, const struct sockaddr *name, int namelen)
  /*@modifies errno, internalState@*/;

int getpeername (int s, /*@out@*/ struct sockaddr */*restrict*/ name, socklen_t */*restrict*/ namelen)
  /*drl splint doesn't handle restrict yet*/
   /*@modifies *name, *namelen, errno@*/;
	
#ifdef STRICT

int getsockname (int s, /*@out@*/ struct sockaddr *address, socklen_t *address_len)
     /*@i556@*/  /*: can't do this? requires maxSet(address) >= (*address_len) @*/ 
  /*@modifies *address, *address_len, errno@*/;

#else  
int getsockname (int s, /*@out@*/ struct sockaddr *address, socklen_t  /*@alt size_t@*/ *address_len)
  /*@i556@*/  /*: can't do this? requires maxSet(address) >= (*address_len) @*/ 
  /*@modifies *address, *address_len, errno@*/;

#endif
  
int getsockopt (int s, int level, int optname, /*@out@*/ void *optval, size_t *optlen)
	/*@modifies *optval, *optlen, errno@*/;

	extern int
listen (int s, int backlog)
	/*@modifies errno, internalState@*/;

	extern ssize_t
recv (int s, /*@out@*/ void *buf, size_t len, int flags)
	/*@modifies *buf, errno@*/;

	extern ssize_t
recvfrom (int s, void *buf, size_t len, int flags, /*@null@*/ struct sockaddr *from, int *fromlen)
	/*@modifies *buf, *from, *fromlen, errno@*/;

	extern ssize_t
recvmsg (int s, struct msghdr *msg, int flags)
	/*@modifies msg->msg_iov->iov_base[], errno@*/;

	extern ssize_t
send (int s, const void *msg, size_t len, int flags)
	/*@modifies errno@*/;

	extern ssize_t
sendto (int s, const void *msg, size_t len, int flags, const struct sockaddr *to, int tolen)
	/*@modifies errno@*/;

	extern ssize_t
sendmsg (int s, const struct msghdr *msg, int flags)
	/*@modifies errno@*/;

	extern int
setsockopt (int s, int level, int optname, const void *optval, int optlen)
	/*@modifies internalState, errno@*/;

	extern int
shutdown (int s, int how)
	/*@modifies errno@*/;

	extern int
socket (int domain, int type, int protocol)
	/*@modifies errno@*/;

	extern int
socketpair (int d, int type, int protocol, /*@out@*/ int *sv)
	/*@modifies errno@*/;

/*@constant int BADSIG@*/
/*@constant int SA_ONSTACK@*/
/*@constant int SA_RESTART@*/
/*@constant int SA_DISABLE@*/
/*@constant int SIGBUS@*/
/*@constant int SIGEMT@*/
/*@constant int SIGINFO@*/
/*@constant int SIGIO@*/
/*@constant int SIGIOT@*/
/*@constant int SIGPOLL@*/
/*@constant int SIGPROF@*/
/*@constant int SIGPWR@*/
/*@constant int SIGSYS@*/
/*@constant int SIGTRAP@*/
/*@constant int SIGURG@*/
/*@constant int SIGVTALRM@*/
/*@constant int SIGWINCH@*/
/*@constant int SIGXCPU@*/
/*@constant int SIGXFSZ@*/

extern void psignal (int sig, const char *msg)
   /*@modifies fileSystem@*/;

	extern int
setenv (const char *name, const char *value, int overwrite)
	/*@globals environ@*/
	/*@modifies *environ, errno@*/;

	extern void
unsetenv (const char *name)
	/*@globals environ@*/
	/*@modifies *environ@*/;

/*________________________________________________________________________
 * sys/wait.h
 */

	extern int
WCOREDUMP (int x)
	/*@*/;

	extern int
W_EXITCODE (int ret, int sig)
	/*@*/;

	extern int
W_STOPCODE (int sig)
	/*@*/;

/*@constant int WAIT_ANY@*/
/*@constant int WAIT_MYPGRP@*/
/*@constant int WSTOPPED@*/

	extern pid_t
wait3 (int *statloc, int options, /*@null@*/ /*@out@*/ struct rusage *rusage)
	/*@modifies *statloc, *rusage, errno@*/;

	extern pid_t
wait4 (pid_t p, int *statloc, int opt, /*@null@*/ /*@out@*/ struct rusage *r)
	/*@modifies *statloc, *r, errno@*/;

struct timeval {
  long	tv_sec;
  long	tv_usec;
} ;

struct timespec {
  long	ts_sec;
  long	ts_nsec;
} ;

struct timezone {
  int	tz_minuteswest;
  int	tz_dsttime;
} ;

/*@constant int DST_NONE@*/
/*@constant int DST_USA@*/
/*@constant int DST_AUST@*/
/*@constant int DST_WET@*/
/*@constant int DST_MET@*/
/*@constant int DST_EET@*/
/*@constant int DST_CAN@*/

/*@constant int ITIMER_PROF@*/
/*@constant int ITIMER_REAL@*/
/*@constant int ITIMER_VIRTUAL@*/

 struct itimerval {
	struct	timeval it_interval;
	struct	timeval it_value;
};

 struct clockinfo {
	int	hz;
	int	tick;
	int	stathz;
	int	profhz;
};

	extern int
adjtime (const struct timeval *delta, /*@null@*/ /*@out@*/ struct timeval *olddelta)
	/*@modifies internalState, *olddelta, errno@*/;

	extern int
getitimer (int which, /*@out@*/ struct itimerval *value)
	/*@modifies errno, *value*/;

	extern int
gettimeofday (/*@null@*/ /*@out@*/ struct timeval *tp, /*@null@*/ /*@out@*/ struct timezone *tzp)
	/*@modifies *tp, *tzp, errno@*/;

	extern int
setitimer (int which, struct itimerval *val, /*@null@*/ /*@out@*/ struct itimerval *oval)
	/*@modifies *oval, errno, internalState*/;

	extern int
settimeofday (const struct timeval *t, const struct timezone *z)
	/*@modifies internalState, errno@*/;

	extern int
utimes (const char *file, /*@null@*/ const struct timeval *times)
	/*@modifies fileSystem, errno*/;

/*________________________________________________________________________
 * sys/mman.h
 */

/*@constant int PROT_READ@*/
/*@constant int PROT_WRITE@*/
/*@constant int PROT_EXEC@*/
/*@constant int MAP_SHARED@*/
/*@constant int MAP_PRIVATE@*/
/*@constant int MAP_COPY@*/
/*@constant int MAP_FIXED@*/
/*@constant int MAP_RENAME@*/
/*@constant int MAP_NORESERVE@*/
/*@constant int MAP_INHERIT@*/
/*@constant int MAP_NOEXTEND@*/
/*@constant int MAP_HASSEMAPHORE@*/
/*@constant int MS_ASYNC@*/
/*@constant int MS_INVALIDATE@*/
/*@constant int MAP_FILE@*/
/*@constant int MAP_ANON@*/
/*@constant int MADV_NORMAL@*/
/*@constant int MADV_RANDOM@*/
/*@constant int MADV_SEQUENTIAL@*/
/*@constant int MADV_WILLNEED@*/
/*@constant int MADV_DONTNEED@*/

	extern caddr_t
mmap (/*@null@*/ /*@returned@*/ caddr_t addr, size_t len, int prot, int flags, int fd, off_t offset)
	/*@modifies addr@*/;

	extern int
madvise (caddr_t addr, int len, int behav)
	/*@*/;

	extern int
mprotect (caddr_t addr, int len, int prot)
	/*@*/;

	extern int
	int munmap (/*@only@*/ caddr_t addr, size_t len)
     /*@modifies fileSystem, *addr, errno @*/;
    
	extern int
msync (caddr_t addr, int len, int flags)
	/*@*/;

	extern int
mlock (caddr_t addr, size_t len)
	/*@*/;

	extern int
munlock (caddr_t addr, size_t len)
	/*@*/;


/*________________________________________________________________________
 * sys/ioctl.h
 */

 struct winsize {
	unsigned short	ws_row;
	unsigned short	ws_col;
	unsigned short	ws_xpixel;
	unsigned short	ws_ypixel;
};

/*@constant int TIOCMODG@*/
/*@constant int TIOCMODS@*/
/*@constant int TIOCM_LE@*/
/*@constant int TIOCM_DTR@*/
/*@constant int TIOCM_RTS@*/
/*@constant int TIOCM_ST@*/
/*@constant int TIOCM_SR@*/
/*@constant int TIOCM_CTS@*/
/*@constant int TIOCM_CAR@*/
/*@constant int TIOCM_CD@*/
/*@constant int TIOCM_RNG@*/
/*@constant int TIOCM_RI@*/
/*@constant int TIOCM_DSR@*/
/*@constant int TIOCEXCL@*/
/*@constant int TIOCNXCL@*/
/*@constant int TIOCFLUSH@*/
/*@constant int TIOCGETA@*/
/*@constant int TIOCSETA@*/
/*@constant int TIOCSETAW@*/
/*@constant int TIOCSETAF@*/
/*@constant int TIOCGETD@*/
/*@constant int TIOCSETD@*/
/*@constant int TIOCSBRK@*/
/*@constant int TIOCCBRK@*/
/*@constant int TIOCSDTR@*/
/*@constant int TIOCCDTR@*/
/*@constant int TIOCGPGRP@*/
/*@constant int TIOCSPGRP@*/
/*@constant int TIOCOUTQ@*/
/*@constant int TIOCSTI@*/
/*@constant int TIOCNOTTY@*/
/*@constant int TIOCPKT@*/
/*@constant int TIOCPKT_DATA@*/
/*@constant int TIOCPKT_FLUSHREAD@*/
/*@constant int TIOCPKT_FLUSHWRITE@*/
/*@constant int TIOCPKT_STOP@*/
/*@constant int TIOCPKT_START@*/
/*@constant int TIOCPKT_NOSTOP@*/
/*@constant int TIOCPKT_DOSTOP@*/
/*@constant int TIOCPKT_IOCTL@*/
/*@constant int TIOCSTOP@*/
/*@constant int TIOCSTART@*/
/*@constant int TIOCMSET@*/
/*@constant int TIOCMBIS@*/
/*@constant int TIOCMBIC@*/
/*@constant int TIOCMGET@*/
/*@constant int TIOCREMOTE@*/
/*@constant int TIOCGWINSZ@*/
/*@constant int TIOCSWINSZ@*/
/*@constant int TIOCUCNTL@*/
/*@constant int TIOCSTAT@*/
/*@constant int TIOCCONS@*/
/*@constant int TIOCSCTTY@*/
/*@constant int TIOCEXT@*/
/*@constant int TIOCSIG@*/
/*@constant int TIOCDRAIN@*/
/*@constant int TIOCMSDTRWAIT@*/
/*@constant int TIOCMGDTRWAIT@*/
/*@constant int TIOCTIMESTAMP@*/
/*@constant int TIOCSDRAINWAIT@*/
/*@constant int TIOCGDRAINWAIT@*/
/*@constant int TTYDISC@*/
/*@constant int TABLDISC@*/
/*@constant int SLIPDISC@*/
/*@constant int PPPDISC@*/

/*@constant int MAXHOSTNAMELEN@*/

extern void FD_CLR (/*@sef@*/ int n, /*@sef@*/ fd_set *p) /*@modifies *p@*/ ;
extern void FD_COPY (/*@sef@*/ fd_set *f, /*@out@*/ fd_set *t) /*@modifies *t@*/ ;
extern int /*@alt _Bool@*/ FD_ISSET (/*@sef@*/ int n, /*@sef@*/ fd_set *p) /*@*/ ;
extern void FD_SET (/*@sef@*/ int n, /*@sef@*/ fd_set *p) /*@modifies *p@*/ ;
extern void FD_ZERO (/*@sef@*/ fd_set /*@out@*/ *p) /*@modifies *p@*/;

extern int fchdir (int fd) /*@modifies internalState, errno@*/;
extern int fchown (int fd, uid_t owner, gid_t group) /*@modifies errno, fileSystem@*/;
extern int fsync (int fd) /*@modifies errno, fileSystem@*/;

extern int ftruncate (int fd, off_t length) /*@modifies errno, fileSystem@*/;

int gethostname (/*@out@*/ char *address, size_t address_len) 
   /*:errorstatus@*/
   /*@modifies address@*/ ;

int initgroups (const char *name, int basegid)
   /*@modifies internalState@*/;

int lchown (const char *path, uid_t owner, gid_t group)
     /*@modifies errno, fileSystem@*/;
     
int select (int mfd, fd_set /*@null@*/ *r, fd_set /*@null@*/ *w, 
	    fd_set /*@null@*/ *e, /*@null@*/ struct timeval *t)
  /*@modifies *r, *w, *e, *t, errno@*/;
  /* evans - 2002-05-26: added null for t, bug reported by Enrico Scholz */

int setegid (gid_t egid)
  /*@modifies errno, internalState@*/;

int seteuid (uid_t euid)
   /*@modifies errno, internalState@*/;
     
int setgroups (int ngroups, const gid_t *gidset)
   /*@modifies errno, internalState@*/;
     
int setregid (gid_t rgid, gid_t egid)
   /*@modifies errno, internalState@*/;
     
int setreuid (gid_t ruid, gid_t euid)
   /*@modifies errno, internalState@*/;
     
void sync (void)
   /*@modifies fileSystem@*/;
     
int symlink (const char *path, const char *path2)
   /*@modifies fileSystem@*/;
     
int truncate (const char *name, off_t length)
   /*@modifies errno, fileSystem@*/;
     
/*@constant int EBADRPC@*/
/*@constant int ERPCMISMATCH@*/
/*@constant int EPROGUNAVAIL@*/
/*@constant int EPROGMISMATCH@*/
/*@constant int EPROCUNAVAIL@*/
/*@constant int EFTYPE@*/
/*@constant int EAUTH@*/
/*@constant int ENEEDAUTH@*/
/*@constant int ELAST@*/

/*
** tar.h
*/

/*@unchecked@*/ extern char *TMAGIC;
/*@constant int TMAGLEN@*/
/*@unchecked@*/ extern char *TVERSION;
/*@constant int TVERSLEN@*/

/*@constant int REGTYPE@*/
/*@constant int AREGTYPE@*/
/*@constant int LNKTYPE@*/
/*@constant int SYMTYPE@*/
/*@constant int CHRTYPE@*/
/*@constant int BLKTYPE@*/
/*@constant int DIRTYPE@*/
/*@constant int FIFOTYPE@*/
/*@constant int CONTTYPE@*/

/*@constant int TSUID@*/
/*@constant int TSGID@*/
/*@constant int TSVTX@*/

/*@constant int TUREAD@*/
/*@constant int TUWRITE@*/
/*@constant int TUEXEC@*/
/*@constant int TGREAD@*/
/*@constant int TGWRITE@*/
/*@constant int TGEXEC@*/
/*@constant int TOREAD@*/
/*@constant int TOWRITE@*/
/*@constant int TOEXEC@*/

struct ipc_perm {
  uid_t	uid;	/* user id */
  gid_t	gid;	/* group id */
  uid_t	cuid;	/* creator user id */
  gid_t	cgid;	/* creator group id */
  mode_t	mode;	/* r/w permission */
  ulong	seq;	/* slot usage sequence number */
  key_t	key;	/* user specified msg/sem/shm key */
} ;

/*@constant int	IPC_R@*/
/*@constant int	IPC_W@*/
/*@constant int	IPC_M@*/
/*@constant int	IPC_CREAT@*/
/*@constant int	IPC_EXCL@*/
/*@constant int	IPC_NOWAIT@*/
/*@constant key_t IPC_PRIVATE@*/
/*@constant int IPC_RMID@*/
/*@constant int IPC_SET@*/
/*@constant int IPC_STAT@*/

/*
** sys/msg.h
*/

 struct msqid_ds {
	struct	ipc_perm msg_perm;	/* msg queue permission bits */
	struct	msg *msg_first;	/* first message in the queue */
	struct	msg *msg_last;	/* last message in the queue */
	u_long	msg_cbytes;	/* number of bytes in use on the queue */
	u_long	msg_qnum;	/* number of msgs in the queue */
	u_long	msg_qbytes;	/* max # of bytes on the queue */
	pid_t	msg_lspid;	/* pid of last msgsnd() */
	pid_t	msg_lrpid;	/* pid of last msgrcv() */
	time_t	msg_stime;	/* time of last msgsnd() */
	time_t	msg_rtime;	/* time of last msgrcv() */
	time_t	msg_ctime;	/* time of last msgctl() */
};

 struct mymesg {
	long	mtype;		/* message type (+ve integer) */
	char	mtext[];	/* message body */
};

/*@constant int MSG_NOERROR@*/
/*@constant int MSGMAX@*/
/*@constant int MSGMNB@*/
/*@constant int MSGMNI@*/
/*@constant int MSGTQL@*/

	extern int
msgctl (int id , int cmd, /*@out@*/ struct msqid_ds *buf)
	/*@modifies errno, *buf@*/;

	extern int
msgget (key_t key, int flag)
	/*@modifies errno@*/;

	extern int
msgrcv (int id, /*@out@*/ void *ptr, size_t nbytes, long type, int flags)
	/*@modifies errno, *ptr@*/;

	extern int
msgsnd (int id, const void *ptr, size_t nbytes, int flag)
	/*@modifies errno@*/;

/*
** sys/sem.h
*/

 struct semid_ds {
	struct ipc_perm sem_perm;
	struct sem     *sem_base;
	ushort          sem_nsems;
	time_t          sem_otime;
	time_t          sem_ctime;
};

 struct sem {
	ushort semval;
	pid_t  sempid;
	ushort semncnt;
	ushort semzcnt;
};

 union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

 struct sembuf {
	ushort sem_num;
	short sem_op;
	short sem_flg;
};

/*@constant int SEM_A@*/
/*@constant int SEMAEM@*/
/*@constant int SEMMNI@*/
/*@constant int SEMMNS@*/
/*@constant int SEMMNU@*/
/*@constant int SEMMSL@*/
/*@constant int SEMOPN@*/
/*@constant int SEM_R@*/
/*@constant int SEMUME@*/
/*@constant int SEM_UNDO@*/
/*@constant int SEMVMX@*/
/*@constant int GETVAL@*/
/*@constant int SETVAL@*/
/*@constant int GETPID@*/
/*@constant int GETNCNT@*/
/*@constant int GETZCNT@*/
/*@constant int GETALL@*/
/*@constant int SETALL@*/

/*@constant int ERMID@*/

	extern int
semctl (int id, int semnum, int cmd, union semun arg)
	/*@modifies errno@*/;

	extern int
semget (key_t key, int nsems, int flag)
	/*@modifies errno@*/;

	extern int
semop (int id, struct sembuf *semoparray, size_t nops)
	/*@modifies errno@*/;

/*
** sys/shm.h
*/

 struct shmid_ds {
	struct ipc_perm shm_perm;
	int shm_segsz;
	ushort shm_lkcnt;
	pid_t shm_lpid;
	pid_t shm_cpid;
	ulong shm_nattch;
	ulong shm_cnattch;
	time_t shm_atime;
	time_t shm_dtime;
	time_t shm_ctime;
};

/*@constant int SHMLBA@*/
/*@constant int SHM_LOCK@*/
/*@constant int SHMMAX@*/
/*@constant int SHMMIN@*/
/*@constant int SHMMNI@*/
/*@constant int SHM_R@*/
/*@constant int SHM_RDONLY@*/
/*@constant int SHM_RND@*/
/*@constant int SHMSEG@*/
/*@constant int SHM_W@*/
/*@constant int SHM_UNLOCK@*/

void * shmat (int id, /*@null@*/ void *addr, int flag)
     /*@modifies errno@*/ ;
     
extern int shmctl (int id, int cmd, /*@out@*/ struct shmid_ds *buf)
     /*@modifies errno, *buf@*/ ;

extern int shmdt (void *addr)
     /*@modifies errno@*/ ;

extern int shmget (key_t key, int size, int flag)
     /*@modifies errno@*/ ;


/*
** stdio.h - in separte file stdio.h
*/

/*
** syslog.h
*/

/*@constant int LOG_EMERG@*/
/*@constant int LOG_ALERT@*/
/*@constant int LOG_CRIT@*/
/*@constant int LOG_ERR@*/
/*@constant int LOG_WARNING@*/
/*@constant int LOG_NOTICE@*/
/*@constant int LOG_INFO@*/
/*@constant int LOG_DEBUG@*/

/*@constant int LOG_KERN@*/
/*@constant int LOG_USER@*/
/*@constant int LOG_MAIL@*/
/*@constant int LOG_DAEMON@*/
/*@constant int LOG_AUTH@*/
/*@constant int LOG_SYSLOG@*/
/*@constant int LOG_LPR@*/
/*@constant int LOG_NEWS@*/
/*@constant int LOG_UUCP@*/
/*@constant int LOG_CRON@*/
/*@constant int LOG_AUTHPRIV@*/
/*@constant int LOG_FTP@*/
/*@constant int LOG_LOCAL0@*/
/*@constant int LOG_LOCAL1@*/
/*@constant int LOG_LOCAL2@*/
/*@constant int LOG_LOCAL3@*/
/*@constant int LOG_LOCAL4@*/
/*@constant int LOG_LOCAL5@*/
/*@constant int LOG_LOCAL6@*/
/*@constant int LOG_LOCAL7@*/

/*@constant int LOG_PID@*/
/*@constant int LOG_CONS@*/
/*@constant int LOG_ODELAY@*/
/*@constant int LOG_NDELAY@*/
/*@constant int LOG_NOWAIT@*/
/*@constant int LOG_PERROR@*/

int LOG_MASK (int pri)
     /*@*/;
     
int LOG_UPTO (int pri)
     /*@*/;
     
void closelog (void)
     /*@modifies fileSystem@*/;
     
void openlog (const char *ident, int logopt, int facility)
     /*@modifies fileSystem@*/;
     
int setlogmask (int maskpri)
     /*@modifies internalState@*/;
     
void /*@printflike@*/ syslog (int priority, const char *message, ...)
     /*@modifies fileSystem@*/;
     
void vsyslog (int priority, const char *message, va_list args)
     /*@modifies fileSystem@*/;

/*________________________________________________________________________
 * pwd.h
 */

	extern extern void
endpwent (void)
	/*@modifies internalState@*/;

	extern /*@null@*/ struct passwd *
getpwent (void)
	/*@modifies internalState@*/;

	extern int
setpassent (int stayopen)
	/*@modifies internalState@*/;

	extern int
setpwent (void)
	/*@modifies internalState@*/;

/*
** grp.h
*/

void endgrent (void) /*@modifies internalState@*/;

/*@null@*/ /*@observer@*/ struct group *getgrent (void)
   /*@modifies internalState@*/;

int setgrent (void) /*@modifies internalState@*/;

void setgrfile (const char *name) /*@modifies internalState@*/;

int setgroupent (int stayopen) /*@modifies internalState@*/;

/*
** sys/stat.h
**
** evans 2001-08-26 - updated from http://www.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
*/

/*
** struct stat replaces POSIX version - more required fields in Unix
*/

/*@-redef@*/ /*@-matchfields@*/
struct stat {
  dev_t     st_dev; /* ID of device containing file */
  ino_t     st_ino; /* file serial number */
  mode_t    st_mode; /* mode of file (see below) */
  nlink_t   st_nlink; /* number of links to the file */
  uid_t     st_uid; /* user ID of file */
  gid_t     st_gid; /* group ID of file */
  dev_t     st_rdev; /* device ID (if file is character or block special) */
  off_t     st_size; /* file size in bytes (if file is a regular file) */
  time_t    st_atime; /* time of last access */
  time_t    st_mtime; /* time of last data modification */
  time_t    st_ctime; /* time of last status change */
  blksize_t st_blksize; /* a filesystem-specific preferred I/O block size for
			   this object.  In some filesystem types, this may
			   vary from file to file */
  blkcnt_t  st_blocks; /*  number of blocks allocated for this object */
} ;
/*@=redef@*/ /*@=matchfields@*/

/*@constant mode_t S_IWUSR@*/
/*@constant mode_t S_IXUSR@*/
/*@constant mode_t S_IRWXG@*/
/*@constant mode_t S_IRGRP@*/
/*@constant mode_t S_IWGRP@*/
/*@constant mode_t S_IXGRP@*/
/*@constant mode_t S_IRWXO@*/
/*@constant mode_t S_IROTH@*/
/*@constant mode_t S_IWOTH@*/
/*@constant mode_t S_IXOTH@*/
/*@constant mode_t S_ISUID@*/
/*@constant mode_t S_ISGID@*/
/*@constant mode_t S_ISVTX@*/

# if 0
/*These are the old definitions - they don't appear to be in the Single UNIX Specification */

/*@constant int S_ISTXT@*/
/*@constant int S_IREAD@*/
/*@constant int S_IWRITE@*/
/*@constant int S_IEXEC@*/
/*@constant int S_IFMT@*/
/*@constant int S_IFIFO@*/
/*@constant int S_IFCHR@*/
/*@constant int S_IFDIR@*/
/*@constant int S_IFBLK@*/
/*@constant int S_IFREG@*/
/*@constant int S_IFLNK@*/
/*@constant int S_IFSOCK@*/
/*@constant int S_ISVTX@*/
/*@constant int S_ISVTX@*/
/*@constant int SF_SETTABLE@*/
/*@constant int SF_ARCHIVED@*/
/*@constant int ACCESSPERMS@*/
/*@constant int ALLPERMS@*/
/*@constant int DEFFILEMODE@*/
/*@constant int S_BLKSIZE@*/
/*@constant int SF_IMMUTABLE@*/
/*@constant int SF_APPEND@*/
/*@constant int UF_NODUMP@*/
/*@constant int UF_IMMUTABLE@*/
/*@constant int UF_APPEND@*/
# endif

int /*@alt _Bool@*/ S_ISBLK (/*@sef@*/ __mode_t m) /*@*/;
int /*@alt _Bool@*/ S_ISCHR (/*@sef@*/ __mode_t m) /*@*/;
int /*@alt _Bool@*/ S_ISDIR (/*@sef@*/ __mode_t m) /*@*/;
int /*@alt _Bool@*/ S_ISFIFO (/*@sef@*/ __mode_t m) /*@*/;
int /*@alt _Bool@*/ S_ISREG (/*@sef@*/ __mode_t m) /*@*/;
int /*@alt _Bool@*/ S_ISLNK (/*@sef@*/ __mode_t m) /*@*/;

int /*@alt _Bool@*/ S_TYPEISMQ (/*@sef@*/ struct stat *buf) /*@*/ ;
int /*@alt _Bool@*/ S_TYPEISSEM (/*@sef@*/ struct stat *buf) /*@*/ ;
int /*@alt _Bool@*/ S_TYPEISSHM  (/*@sef@*/ struct stat *buf) /*@*/ ;

/* in POSIX: chmod, fstat, mkdir, mkfifo, stat, umask */

int lstat(const char *, /*@out@*/ struct stat *)
     /*:errorcode -1:*/
     /*@modifies errno@*/ ;
     
int mknod (const char *, mode_t, dev_t)
  /*@warn portability "The only portable use of mknod is to create FIFO-special file. If mode is not S_IFIFO or dev is not 0, the behaviour of mknod() is unspecified."@*/
  /*:errorcode -1:*/
  /*@modifies errno@*/ ;

int chflags (const char *path, u_long flags)
  /*@warn unixstandard "Not in Single UNIX Specification Version 2"@*/
  /*@modifies fileSystem, errno@*/;

int fchflags (int fd, u_long flags)
  /*@warn unixstandard "Not in Single UNIX Specification Version 2"@*/
  /*@modifies fileSystem, errno@*/;

/* evans 2002-03-17: this was missing, reported by Ralf Wildenhues */
int fchmod(int fildes, mode_t mode) 
   /*@modifies fileSystem, errno@*/ ; 
  
/*
** sys/statvfs.h
** from http://www.opengroup.org/onlinepubs/007908799/xsh/sysstatvfs.h.html
*/

struct statvfs {
   unsigned long f_bsize;
   unsigned long f_frsize;
   fsblkcnt_t    f_blocks;
   fsblkcnt_t    f_bfree;
   fsblkcnt_t    f_bavail;
   fsfilcnt_t    f_files;
   fsfilcnt_t    f_ffree;
   fsfilcnt_t    f_favail;                       
   unsigned long f_fsid;
   unsigned long f_flag;
   unsigned long f_namemax; 
} ;

/*@constant unsigned long ST_RDONLY; @*/
/*@constant unsigned long ST_NOSUID; @*/

int fstatvfs (int fildes, /*@out@*/ struct statvfs *buf) 
   /*@modifies buf@*/ ;

int statvfs (const char *path, /*@out@*/ struct statvfs *buf)
    /*@modifies buf@*/ ; 


/*________________________________________________________________________
 * stropts.h
 */

/*@constant int FMNAMESZ@*/
/*@constant int MSG_BAND@*/
/*@constant int MSG_HIPRI@*/
/*@constant int RS_HIPRI@*/
/*@constant int S_INPUT@*/
/*@constant int S_RDNORM@*/
/*@constant int S_RDBAND@*/
/*@constant int S_BANDURG@*/
/*@constant int S_HIPRI@*/
/*@constant int S_OUTPUT@*/
/*@constant int S_WRNORM@*/
/*@constant int S_WRBAND@*/
/*@constant int S_MSG@*/
/*@constant int S_ERROR@*/
/*@constant int S_HANGUP@*/

 struct strbuf {
	int maxlen;
	int len;
	char *buf;
}

 struct str_mlist {
	char l_name[];
}

 struct str_list {
	int sl_nmods;
	struct str_mlist *sl_modlist;
}

	extern int
getmsg (int fd, /*@out@*/ struct strbuf *c, /*@out@*/ struct strbuf *d, int *f)
	/*@modifies *c, *d, errno@*/;

	extern int
getpmsg (int fd, /*@out@*/ struct strbuf *c, /*@out@*/ struct strbuf *d, int *b, int *f)
	/*@modifies *b, *c, *d, errno@*/;

	extern int
putmsg (int fd, const struct strbuf *c, const struct strbuf *d, int *f)
	/*@modifies internalState, errno@*/;

extern int putpmsg (int fd, const struct strbuf *c, const struct strbuf *d, int b, int *f)
   /*@modifies internalState, errno@*/;

/*
** sys/resource.h
**
** Update 2002-07-09 from
** http://www.opengroup.org/onlinepubs/007904975/basedefs/sys/resource.h.html
*/

/*@constant int PRIO_PROCESS@*/
/*@constant int PRIO_PGRP@*/
/*@constant int PRIO_USER@*/

typedef /*@unsignedintegraltype@*/ rlim_t;

/*@constant rlim_t RLIM_INFINITY@*/
/*@constant rlim_t RLIM_SAVED_MAX@*/
/*@constant rlim_t RLIM_SAVED_CUR@*/

/*@constant int RUSAGE_SELF@*/
/*@constant int RUSAGE_CHILDREN@*/

struct rlimit {
  rlim_t rlim_cur;
  rlim_t rlim_max;
};

struct rusage {
  struct timeval ru_utime;        /* user time used */
  struct timeval ru_stime;        /* system time used */
  /* other members optional */
};

/*@constant int RLIMIT_CORE@*/
/*@constant int RLIMIT_CPU@*/
/*@constant int RLIMIT_DATA@*/
/*@constant int RLIMIT_FSIZE@*/
/*@constant int RLIMIT_NOFILE@*/
/*@constant int RLIMIT_STACK@*/
/*@constant int RLIMIT_AS@*/

int getpriority (int which, id_t who)
   /*@modifies errno@*/;

int getrlimit (int res, /*@out@*/ struct rlimit *rlp)
   /*@modifies *rlp, errno@*/;

int getrusage (int who, /*@out@*/ struct rusage *rusage)
   /*@modifies *rusage, errno@*/;

int setpriority (int which, id_t who, int prio)
   /*@modifies errno, internalState@*/;

int setrlimit (int resource, const struct rlimit *rlp)
   /*@modifies errno, internalState@*/;

/*
** in <netdb.h>
*/

struct servent
{
  /*@observer@*/ char *s_name;			/* Official service name.  */
  /*@observer@*/ char **s_aliases;		/* Alias list.  */
  int s_port;			                /* Port number.  */
  /*@observer@*/ char *s_proto;		        /* Protocol to use.  */
} ;

/*@observer@*/ /*@dependent@*/ /*@null@*/ struct servent *getservbyname (const char *name, /*@null@*/ const char *proto) 
     /*@warn multithreaded "Unsafe in multithreaded applications, use getsrvbyname_r instead"@*/ ;

struct servent *getservbyname_r (const char *name, /*@null@*/ const char *proto, /*@out@*/ /*@returned@*/ struct servent *result, /*@out@*/ char *buffer, int buflen) 
     /*@requires maxSet (buffer) >= buflen@*/ ;


/*@observer@*/ /*@dependent@*/ struct servent *getservbyport (int port, /*@null@*/ const char *proto)
     /*@warn multithreaded "Unsafe in multithreaded applications, use getservbyport_r instead"@*/ ;

struct servent *getservbyport_r (int port, /*@null@*/ const char *proto, /*@out@*/ /*@returned@*/ struct servent *result, /*@out@*/ char *buffer, int buflen)
     /*@requires maxSet (buffer) >= buflen@*/ ;

/*@null@*/ struct servent *getservent (void);

/*@null@*/ struct servent *getservent_r (struct servent *result, char *buffer, int buflen);

int setservent (int stayopen);
int endservent (void);

extern int h_errno;

/*@null@*/ /*@observer@*/ struct hostent *gethostbyname (/*@nullterminated@*/ /*@notnull@*/ const char *name)
     /*@warn multithreaded "Unsafe in multithreaded applications, use gethostbyname_r instead"@*/ 
     /*@modifies h_errno@*/ ;

struct hostent *gethostbyname_r (/*@nullterminated@*/ const char *name, /*@notnull@*/ /*@returned@*/ struct hostent *hent, /*@out@*/ /*@exposed@*/ char *buffer, int bufsize, /*@out@*/ int *h_errnop)
     /*@requires maxSet(buffer) >= bufsize@*/ ;

/*@null@*/ /*@observer@*/ struct hostent *gethostbyaddr (/*@notnull@*/ const void *addr, size_t addrlen, int type) 
     /*@requires maxRead(addr) == addrlen@*/ /*:i534 ??? is this right? */
     /*@warn multithreaded "Unsafe in multithreaded applications, use gethostbyaddr_r instead"@*/ 
     /*@modifies h_errno@*/ ;

struct hostent *gethostbyaddr_r (/*@notnull@*/ const void *addr, size_t addrlen, int type, 
				 /*@returned@*/ /*@out@*/ struct hostent *hent, 
				 /*@exposed@*/ /*@out@*/ char *buffer, int bufsize, /*@out@*/ int *h_errnop)
     /*@requires maxRead(addr) == addrlen /\ maxSet(buffer) >= bufsize@*/
     /*:i534 ??? is this right? */ ;

/*@observer@*/ /*@null@*/ struct hostent *gethostent (void)
    /*@warn multithreaded "Unsafe in multithreaded applications, use gethostent_r instead"@*/ ;

struct hostent *gethostent_r (/*@out@*/ /*@returned@*/ struct hostent *hent, /*@exposed@*/ /*@out@*/ char *buffer, int bufsize)
     /*@requires maxSet(buffer) >= bufsize@*/ ;

/*:i534 need to annotate these: */

struct hostent *fgethostent(FILE *f);
struct hostent *fgethostent_r(FILE*f, struct hostent *hent, char buffer, int bufsize);
void sethostent(int stayopen);
void endhostent(void);
void herror(const char *string);
char *hstrerror(int err);

struct hostent {
  /*@observer@*/ /*@nullterminated@*/ char *h_name;   /* official name of host */
  /*@observer@*/ /*@nullterminated@*/ char * /*:observer@*/ /*:nullterminated@*/ *h_aliases;   /* alias list */
  int  h_addrtype;   /* host address type*/
  int  h_length;   /* length ofaddress*/
  /*@observer@*/ /*@nullterminated@*/ char * /*:observer@*/ /*:nullterminated@*/ *h_addr_list; /* list of addressesfrom name server */
  /*@observer@*/ /*@nullterminated@*/ char *h_addr; /* first address in list (backward compatibility) */
} ;

/*
** unistd.h
** from http://www.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html
*/

/*@constant int _POSIX_VERSION@*/
/*@constant int _POSIX2_VERSION@*/
/*@constant int _POSIX2_C_VERSION@*/
/*@constant int _XOPEN_VERSION@*/
/*@constant int _XOPEN_XCU_VERSION@*/

/* for access: */

/*@constant int R_OK@*/
/*@constant int W_OK@*/
/*@constant int X_OK@*/
/*@constant int F_OK@*/

/* for confstr: */
/*@constant int _CS_PATH@*/
/*@constant int _CS_XBS5_ILP32_OFF32_CFLAGS@*/
/*@constant int _CS_XBS5_ILP32_OFF32_LDFLAGS@*/
/*@constant int _CS_XBS5_ILP32_OFF32_LIBS@*/
/*@constant int _CS_XBS5_ILP32_OFF32_LINTFLAGS@*/
/*@constant int _CS_XBS5_ILP32_OFFBIG_CFLAGS@*/
/*@constant int _CS_XBS5_ILP32_OFFBIG_LDFLAGS@*/
/*@constant int _CS_XBS5_ILP32_OFFBIG_LIBS@*/
/*@constant int _CS_XBS5_ILP32_OFFBIG_LINTFLAGS@*/
/*@constant int _CS_XBS5_LP64_OFF64_CFLAGS@*/
/*@constant int _CS_XBS5_LP64_OFF64_LDFLAGS@*/
/*@constant int _CS_XBS5_LP64_OFF64_LIBS@*/
/*@constant int _CS_XBS5_LP64_OFF64_LINTFLAGS@*/
/*@constant int _CS_XBS5_LPBIG_OFFBIG_CFLAGS@*/
/*@constant int _CS_XBS5_LPBIG_OFFBIG_LDFLAGS@*/
/*@constant int _CS_XBS5_LPBIG_OFFBIG_LIBS@*/
/*@constant int _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS@*/

/* name parameters to sysconf: */

/*@constant int _SC_2_C_BIND@*/
/*@constant int _SC_2_C_DEV@*/
/*@constant int _SC_2_C_VERSION@*/
/*@constant int _SC_2_FORT_DEV@*/
/*@constant int _SC_2_FORT_RUN@*/
/*@constant int _SC_2_LOCALEDEF@*/
/*@constant int _SC_2_SW_DEV@*/
/*@constant int _SC_2_UPE@*/
/*@constant int _SC_2_VERSION@*/
/*@constant int _SC_ARG_MAX@*/
/*@constant int _SC_AIO_LISTIO_MAX@*/
/*@constant int _SC_AIO_MAX@*/
/*@constant int _SC_AIO_PRIO_DELTA_MAX@*/
/*@constant int _SC_ASYNCHRONOUS_IO@*/
/*@constant int _SC_ATEXIT_MAX@*/
/*@constant int _SC_BC_BASE_MAX@*/
/*@constant int _SC_BC_DIM_MAX@*/
/*@constant int _SC_BC_SCALE_MAX@*/
/*@constant int _SC_BC_STRING_MAX@*/
/*@constant int _SC_CHILD_MAX@*/
/*@constant int _SC_CLK_TCK@*/
/*@constant int _SC_COLL_WEIGHTS_MAX@*/
/*@constant int _SC_DELAYTIMER_MAX@*/
/*@constant int _SC_EXPR_NEST_MAX@*/
/*@constant int _SC_FSYNC@*/
/*@constant int _SC_GETGR_R_SIZE_MAX@*/
/*@constant int _SC_GETPW_R_SIZE_MAX@*/
/*@constant int _SC_IOV_MAX@*/
/*@constant int _SC_JOB_CONTROL@*/
/*@constant int _SC_LINE_MAX@*/
/*@constant int _SC_LOGIN_NAME_MAX@*/
/*@constant int _SC_MAPPED_FILES@*/
/*@constant int _SC_MEMLOCK@*/
/*@constant int _SC_MEMLOCK_RANGE@*/
/*@constant int _SC_MEMORY_PROTECTION@*/
/*@constant int _SC_MESSAGE_PASSING@*/
/*@constant int _SC_MQ_OPEN_MAX@*/
/*@constant int _SC_MQ_PRIO_MAX@*/
/*@constant int _SC_NGROUPS_MAX@*/
/*@constant int _SC_OPEN_MAX@*/
/*@constant int _SC_PAGESIZE@*/
/*@constant int _SC_PAGE_SIZE@*/
/*@constant int _SC_PASS_MAX@*/
/*@constant int _SC_PRIORITIZED_IO@*/
/*@constant int _SC_PRIORITY_SCHEDULING@*/
/*@constant int _SC_RE_DUP_MAX@*/
/*@constant int _SC_REALTIME_SIGNALS@*/
/*@constant int _SC_RTSIG_MAX@*/
/*@constant int _SC_SAVED_IDS@*/
/*@constant int _SC_SEMAPHORES@*/
/*@constant int _SC_SEM_NSEMS_MAX@*/
/*@constant int _SC_SEM_VALUE_MAX@*/
/*@constant int _SC_SHARED_MEMORY_OBJECTS@*/
/*@constant int _SC_SIGQUEUE_MAX@*/
/*@constant int _SC_STREAM_MAX@*/
/*@constant int _SC_SYNCHRONIZED_IO@*/
/*@constant int _SC_THREADS@*/
/*@constant int _SC_THREAD_ATTR_STACKADDR@*/
/*@constant int _SC_THREAD_ATTR_STACKSIZE@*/
/*@constant int _SC_THREAD_DESTRUCTOR_ITERATIONS@*/
/*@constant int _SC_THREAD_KEYS_MAX@*/
/*@constant int _SC_THREAD_PRIORITY_SCHEDULING@*/
/*@constant int _SC_THREAD_PRIO_INHERIT@*/
/*@constant int _SC_THREAD_PRIO_PROTECT@*/
/*@constant int _SC_THREAD_PROCESS_SHARED@*/
/*@constant int _SC_THREAD_SAFE_FUNCTIONS@*/
/*@constant int _SC_THREAD_STACK_MIN@*/
/*@constant int _SC_THREAD_THREADS_MAX@*/
/*@constant int _SC_TIMERS@*/
/*@constant int _SC_TIMER_MAX@*/
/*@constant int _SC_TTY_NAME_MAX@*/
/*@constant int _SC_TZNAME_MAX@*/
/*@constant int _SC_VERSION@*/
/*@constant int _SC_XOPEN_VERSION@*/
/*@constant int _SC_XOPEN_CRYPT@*/
/*@constant int _SC_XOPEN_ENH_I18N@*/
/*@constant int _SC_XOPEN_SHM@*/
/*@constant int _SC_XOPEN_UNIX@*/
/*@constant int _SC_XOPEN_XCU_VERSION@*/
/*@constant int _SC_XOPEN_LEGACY@*/
/*@constant int _SC_XOPEN_REALTIME@*/
/*@constant int _SC_XOPEN_REALTIME_THREADS@*/
/*@constant int _SC_XBS5_ILP32_OFF32@*/
/*@constant int _SC_XBS5_ILP32_OFFBIG@*/
/*@constant int _SC_XBS5_LP64_OFF64@*/
/*@constant int _SC_XBS5_LPBIG_OFFBIG@*/


int access(const char *, int) /*@modifies errno@*/ /*:errorcode -1:*/ ;
unsigned int alarm (unsigned int) /*@modifies internalState@*/ ;

int brk(void *)
     /*@modifies errno@*/
     /*:errorcode -1:*/
     /*@warn legacy "brk is obsolete"@*/ ;

int chdir (const char *)
     /*@modifies internalState, errno@*/
     /*:errorcode -1:*/ ;

int chown (const char *, uid_t, gid_t)
     /*@modifies internalState, errno@*/
     /*:errorcode -1:*/ ;

int close (int)
     /*@modifies internalState, errno@*/
     /*:errorcode -1:*/ ;

size_t confstr(int, /*@null@*/ char *, size_t)
     /*@globals internalState@*/
     /*@modifies errno@*/
     /*:errorcode 0:*/ ;

/*@dependent@*/ /*@null@*/ char *crypt(const char *, const char *)
     /*@modifies errno, internalState@*/ ;

/*@dependent@*/ /*@null@*/ char *ctermid(/*@returned@*/ /*@null@*/ /*@out@*/ char *s)
     /*@modifies s@*/ ;

/*@null@*/ /*@dependent@*/ char *cuserid (/*@null@*/ /*@returned@*/ char *s)
     /*@warn legacy "cuserid is obsolete"@*/
     /*@modifies s@*/ ;

int dup(int)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int dup2(int, int)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

void encrypt(char p_block[], int)
     /*@requires maxSet(p_block) == 63@*/
     /*@modifies p_block, errno@*/ ;

extern char **environ;

int execl (const char *, const char *, ...)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int execle(const char *, const char *, ...)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int execlp(const char *, const char *, ...)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int execv(const char *, char *const [])
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int execve(const char *, char *const [], char *const [])
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int execvp(const char *, char *const [])
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

/*@exits@*/ void _exit (int);

int fchown (int, uid_t, gid_t)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

int fchdir (int)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

int fdatasync (int)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

pid_t fork (void)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

long int fpathconf(int, int)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int fsync(int)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

int ftruncate(int, off_t)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

/*@null@*/ char *getcwd (/*@returned@*/ char *buf, size_t size)
     /*@requires maxSet(buf) >= size;@*/
     /*@modifies errno@*/ ;

int getdtablesize (void)
     /*@warn legacy "getdtablesize is obsolete"@*/ ;

gid_t getegid (void) /*@globals internalState*/ ;
uid_t geteuid (void) /*@globals internalState*/ ;
gid_t getgid (void) /*@globals internalState*/ ;

int getgroups (int gidsetsize, gid_t grouplist[])
     /*@requires maxSet(grouplist) >= gidsetsize@*/
     /*@modifies errno@*/
     /*:errorcode -1:*/ ;

long gethostid (void) /*@globals internalState@*/ ;

/*@null@*/ /*@dependent@*/ char *getlogin (void)
     /*@modifies errno@*/ ;

int getlogin_r (char *name, size_t namesize)
     /*@requires maxSet(name) >= namesize@*/
     /*:errorcode !0:*/ ;

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt(int, char * const [], const char *)
     /*@modifies optind, opterr, optopt, errno@*/
     /*:errorcode -1:*/ ;

int getpagesize(void)
     /*@warn legacy "getpagesize is obsolete"@*/ ;

/*@dependent@*/ /*@null@*/ char *getpass(/*@nullterminated@*/ const char *)
     /*@warn legacy "getpass is obsolete"@*/ ;

pid_t getpgid(pid_t)
     /*@modifies errno@*/
     /*@globals internalState@*/
     /*:errorcode (pid_t)-1:*/ ;

pid_t getpgrp(void) /*@globals internalState*/ ;

pid_t getpid(void) /*@globals internalState*/ ;
pid_t getppid(void) /*@globals internalState*/ ;

pid_t getsid(pid_t)
     /*@modifies errno@*/
     /*@globals internalState@*/
     /*:errorcode (pid_t)-1:*/ ;

uid_t getuid(void) /*@globals internalState@*/ ;

/*@null@*/ char *getwd (/*@returned@*/ char *path_name)
     /*@modifies path_name@*/ ;

int isatty(int)
     /*@globals internalState@*/
     /*@modifies errno@*/
     /*:errorcode 0:*/ ;

int lchown(const char *, uid_t, gid_t)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

int link(const char *, const char *)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

int lockf(int, int, off_t)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

off_t lseek(int, off_t, int)
     /*@modifies errno, fileSystem@*/
     /*:errorcode (off_t)-1:*/ ;

int nice(int)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

long int pathconf(const char *, int)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int pause(void)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int pipe(int p[])
     /*@requires maxRead(p) == 1@*/
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

ssize_t pread(int, /*@out@*/ void *buf, size_t nbyte, off_t offset)
     /*@modifies errno, fileSystem@*/
     /*@requires maxSet(buf) >= (nbyte - 1) @*/
     /*@ensures maxRead(buf) >= nbyte @*/ 
     /*:errorcode -1:*/ ;
     
int pthread_atfork(void (*)(void), void (*)(void), void(*)(void))
     /*@modifies errno, fileSystem@*/
     /*:errorcode !0:*/ ;

ssize_t pwrite(int, const void *buf, size_t nbyte, off_t)
     /*@requires maxRead(buf) >= nbyte@*/
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

/*     ssize_t      read(int, void *, size_t); in posix.h */

int readlink(const char *, char *buf, size_t bufsize)
     /*@requires maxSet(buf) >= (bufsize - 1)@*/
     /*@modifies errno, fileSystem, *buf@*/
     /*:errorcode -1:*/ ;

/* int          rmdir(const char *); in posix.h */

void *sbrk(intptr_t)
     /*@modifies errno@*/
     /*:errorcode (void *)-1:*/
     /*@warn legacy "sbrk is obsolete"@*/ ;

     /*     int          setgid(gid_t);
	    int          setpgid(pid_t, pid_t);
     */

pid_t setpgrp(void) /*@modifies internalState@*/ ;

int setregid(gid_t, gid_t)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

int setreuid(uid_t, uid_t)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

pid_t setsid(void)
     /*@modifies errno, internalState@*/
     /*:errorcode (pid_t) -1:*/ ;

int setuid(uid_t)
     /*@modifies errno, internalState@*/
     /*:errorcode -1:*/ ;

unsigned int sleep(unsigned int)
     /*@modifies systemState@*/ ;

void swab(/*@unique@*/ const void *src, /*@unique@*/ void *dest, ssize_t nbytes)
     /*@requires maxSet(dest) >= (nbytes - 1)@*/ ;

int symlink(const char *, const char *)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

void sync(void) /*@modifies systemState@*/ ;

long int sysconf(int)
     /*@modifies errno, systemState@*/
     /*:errorcode -1:*/ ;

pid_t tcgetpgrp(int)
     /*@globals systemState@*/
     /*@modifies errno@*/
     /*:errorcode -1:*/ ;

int tcsetpgrp(int, pid_t)
     /*@modifies errno, systemState@*/
     /*:errorcode -1:*/ ;

int truncate(const char *, off_t)
     /*@modifies errno, fileSystem@*/
     /*:errorcode -1:*/ ;

/*@dependent@*/ /*@null@*/ char *ttyname(int)
     /*@globals systemState@*/
     /*@modifies errno@*/
     /*:errorcode -1:*/ ;

int ttyname_r(int, char *name, size_t namesize)
     /*@requires maxSet(name) >= (namesize - 1)@*/ ;
     /*:errorcode !0:*/ ;

useconds_t ualarm(useconds_t, useconds_t)
     /*@modifies systemState@*/ ;

int unlink(const char *)
     /*@modifies fileSystem, errno@*/
     /*:errorcode -1:*/ ;
     
int usleep(useconds_t)
     /*@modifies fileSystem, errno@*/
     /*:errorcode -1:*/ ;

pid_t vfork(void)
     /*@modifies fileSystem, errno@*/
     /*:errorcode -1:*/ ;

     /* in posix.h ssize_t write(int, const void *, size_t); */


int chroot (/*@notnull@*/ /*@nullterminated@*/ const char *path)
     /*@modifies internalState, errno@*/
     /*:errorcode -1:*/ 
     /*@warn superuser "Only super-user processes may call chroot."@*/ ;

int fchroot (int fildes)
   /*:statusreturn@*/
   /*@warn superuser "Only super-user processes may call fchroot."@*/ ;


/*
** ctype.h 
**
** evans 2001-08-26 - added from http://www.opengroup.org/onlinepubs/007908799/xsh/ctype.h.html
*/

# ifdef STRICT
_Bool isascii(int) /*@*/ ;
_Bool toascii(int) /*@*/ ;
char _toupper(/*@sef@*/ int) /*@*/ ;
char  _tolower(/*@sef@*/ int) /*@*/ ;
# else
_Bool /*@alt int@*/ isascii(int /*@alt unsigned char@*/) /*@*/ ;
_Bool /*@alt int@*/ toascii(int /*@alt unsigned char@*/);
char  /*@alt int@*/ _toupper(/*@sef@*/ int /*@alt unsigned char@*/);
char /*@alt int@*/ _tolower(/*@sef@*/ int /*@alt unsigned char@*/);
# endif

/* other ctype.h functions in ansi.h */

/*
** stdlib.h
**
** evans 2001-08-27 - added from http://www.opengroup.org/onlinepubs/007908799/xsh/drand48.html
*/

double drand48 (void) /*@modifies internalState@*/ ; 
double erand48 (unsigned short int /*@-fixedformalarray@*/ xsubi[3] /*@=fixedformalarray@*/ ) 
   /*@modifies internalState@*/ ; 

void srand48 (long int seedval) /*@modifies internalState@*/ ; 

/*
** netinet/in.h
**
** evans 2001-08-27 - added from http://www.opengroup.org/onlinepubs/007908799/xns/netinetin.h.html
*/

typedef /*@unsignedintegraltype@*/ in_port_t; /* An unsigned integral type of exactly 16 bits. */
typedef /*@unsignedintegraltype@*/ in_addr_t; /* An unsigned integral type of exactly 32 bits. */

/* sa_family_t moved earlier */

struct in_addr {
  in_addr_t      s_addr;
} ;

struct sockaddr_in {
  sa_family_t    sin_family;
  in_port_t      sin_port;
  struct in_addr sin_addr;
  unsigned char  sin_zero[8];
} ;


/* The <netinet/in.h> header defines the following macros for use as values of the level argument of 
   getsockopt() and setsockopt(): 
 */

/*@constant int IPPROTO_IP@*/
/*@constant int IPPROTO_ICMP@*/
/*@constant int IPPROTO_TCP@*/
/*@constant int IPPROTO_UDP@*/

/* The <netinet/in.h> header defines the following macros for use as destination addresses for connect(), sendmsg() and sendto(): 
 */

/*@constant in_addr_t INADDR_ANY@*/
/*@constant in_addr_t INADDR_BROADCAST@*/

/*
** arpa/inet.h
*/

in_addr_t htonl (in_addr_t hostlong) /*@*/ ;
in_port_t htons (in_port_t hostshort) /*@*/ ;
in_addr_t ntohl (in_addr_t netlong) /*@*/ ;
in_port_t ntohs (in_port_t netshort) /*@*/ ;

/*
** dirent.h
**
** evans 2001-08-27 - added from http://www.opengroup.org/onlinepubs/007908799/xsh/dirent.h.html
*/

/*@-redef@*/ /*@-matchfields@*/ /* Has d_ino field, not in posix (?) */

struct dirent
{
  ino_t  d_ino;
  char   d_name[];    
} ;

/*@=redef@*/ /*@=matchfields@*/

typedef /*@abstract@*/ DIR;

/*:i32 need to check annotations on these */

int closedir (DIR *) /*:errorcode -1*/ ; 
/*@null@*/ /*@dependent@*/ DIR *opendir(const char *)  /*@modifies errno, fileSystem@*/ ;

/* in posix.h: struct dirent *readdir(DIR *); */

int readdir_r (DIR *, struct dirent *, /*@out@*/ struct dirent **result)
     /*@modifies *result@*/
     /*:errorcode !0:*/ ;

void rewinddir(DIR *);
void seekdir(DIR *, long int);
long int telldir(DIR *);

/*drl added these functions
  stpcpy and stpncpy are found on linux but
  don't seem to be present on other unixes

  thanks to Jeff Johnson for pointing out that
  these functions were in the unix library
*/

/* this function is like strcpy but it reutrns a pointer to the null terminated character in dest instead of the beginning of dest */

extern char * stpcpy(/*@out@*/ /*@returned@*/ char * dest, const char * src)
             /*@modifies *dest @*/
     /*@requires maxSet(dest) >= maxRead(src) @*/
     /*@ensures MaxRead(dest) == MaxRead (src) /\ MaxRead(result) == 0 /\ MaxSet(result) == ( maxSet(dest) - MaxRead(src) ); @*/;


extern char * stpncpy(/*@out@*/ /*@returned@*/ char * dest,
		      const char * src, size_t n)
           /*@modifies *dest @*/
   /*@requires MaxSet(dest) >= ( n - 1 ); @*/ /*@ensures MaxRead (src) >= MaxRead(dest) /\ MaxRead (dest) <= n; @*/
  ; 

  /* drl added 09-25-001
     Alexander Ma pointed out these were missing 
  */
  
int usleep (useconds_t useconds) /*@modifies systemState, errno@*/
     /*error -1 sucess 0 */
     /* warn opengroup unix specification recommends using setitimer(), timer_create(), timer_delete(), timer_getoverrun(), timer_gettime() or
     timer_settime() instead of this interface. 
     */
     ;


     /* drl added 10-27-001 */
         /*not sure what the exact size of this is
       also can IPv6 use this function?
      */
     /*I'm going to assume that the address had the format:
       "###.###.###.###" which is 16 bytes*/
     
       /*@kept@*/ char *inet_ntoa(struct in_addr in)
     /*@ensures maxSet(result) <= 15 /\ maxRead(result) <= 15 @*/
     ;


     extern  double hypot(double x, double y) /*@modifies errno@*/ /*error errno only*/;


     extern double j0(double x) /*@modifies errno @*/ /*error 0 or NaN */;
 extern      double j1(double x) /*@modifies errno @*/ /*error 0 or NaN */;
 extern      double jn(int n, double x) /*@modifies errno @*/ /*error 0 or NaN */;

     extern double y0(double x) /*@modifies errno @*/  /*error NaN -HUGE_VAL 0.0 */     ;
     extern      double y1 (double x) /*@modifies errno @*/  /*error NaN -HUGE_VAL 0.0 */;
     extern      double yn (int n, double x)  /*@modifies errno @*/  /*error NaN -HUGE_VAL 0.0 */;

     extern       double acosh(double x)  /*@modifies errno @*/ /*error errno and implementation-dependent(NaN if present) */ /*error NaN and may errno*/;
     extern       double asinh(double x) /*@modifies errno @*/  /*error NaN and may errno */;
     
  extern        double atanh(double x) /*@modifies errno @*/ /*error errno and implementation-dependent(NaN if present) */ /*error NaN and may errno */ ;

     extern         double lgamma(double x)  /*@modifies errno @*/  /*error NaN or HUGE_VAL may set errno */;
     
     extern int signgam ;
     
      extern      double erf(double x)  /*@modifies errno @*/  /*error NaN or 0 may set errno */;

   extern      double erfc (double x) /*@modifies errno @*/  /*error NaN or 0
					may set errno */;

     

     
     

