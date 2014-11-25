

# 1 "driverstub.c"
# 1 "/usr/include/sys/types.h" 1 3
 

















 






# 1 "/usr/include/features.h" 1 3
 




















 



























































 



















 





 



 







 
# 138 "/usr/include/features.h" 3


 









 





 



























# 196 "/usr/include/features.h" 3


































 



 








 




 

# 1 "/usr/include/sys/cdefs.h" 1 3
 




















 




 





 








 




# 71 "/usr/include/sys/cdefs.h" 3


 







 



# 103 "/usr/include/sys/cdefs.h" 3



 








 















 








 








 









 







# 250 "/usr/include/features.h" 2 3


 








 





 

 








# 1 "/usr/include/gnu/stubs.h" 1 3
 






































# 278 "/usr/include/features.h" 2 3




# 26 "/usr/include/sys/types.h" 2 3


 

# 1 "/usr/include/bits/types.h" 1 3
 

















 









# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


































typedef unsigned int size_t;






















 




 

# 271 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 29 "/usr/include/bits/types.h" 2 3


 
typedef unsigned char __u_char;
typedef unsigned short __u_short;
typedef unsigned int __u_int;
typedef unsigned long __u_long;

__extension__ typedef unsigned long long int __u_quad_t;
__extension__ typedef long long int __quad_t;
# 48 "/usr/include/bits/types.h" 3

typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;

typedef __quad_t *__qaddr_t;

typedef __u_quad_t __dev_t;		 
typedef __u_int __uid_t;		 
typedef __u_int __gid_t;		 
typedef __u_long __ino_t;		 
typedef __u_int __mode_t;		 
typedef __u_int __nlink_t; 		 
typedef long int __off_t;		 
typedef __quad_t __loff_t;		 
typedef int __pid_t;			 
typedef int __ssize_t;			 
typedef long int __rlim_t;		 
typedef __quad_t __rlim64_t;		 
typedef __u_int __id_t;			 

typedef struct
  {
    int __val[2];
  } __fsid_t;				 

 
typedef int __daddr_t;			 
typedef char *__caddr_t;
typedef long int __time_t;
typedef long int __swblk_t;		 

typedef long int __clock_t;

 
typedef unsigned long int __fd_mask;

 


 




 
typedef struct
  {
     





    __fd_mask __fds_bits[1024  / (8 * sizeof (__fd_mask)) ];


  } __fd_set;


typedef int __key_t;

 
typedef unsigned short int __ipc_pid_t;


 

 
typedef long int __blkcnt_t;
typedef __quad_t __blkcnt64_t;

 
typedef __u_long __fsblkcnt_t;
typedef __u_quad_t __fsblkcnt64_t;

 
typedef __u_long __fsfilcnt_t;
typedef __u_quad_t __fsfilcnt64_t;

 
typedef __u_long __ino64_t;

 
typedef __loff_t __off64_t;

 
typedef long int __t_scalar_t;
typedef unsigned long int __t_uscalar_t;

 
typedef int __intptr_t;


 





# 30 "/usr/include/sys/types.h" 2 3



typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;


typedef __loff_t loff_t;



typedef __ino_t ino_t;











typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;











typedef __pid_t pid_t;




typedef __id_t id_t;



typedef __ssize_t ssize_t;




typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;



typedef __key_t key_t;






# 1 "/usr/include/time.h" 1 3
 

















 














# 51 "/usr/include/time.h" 3



# 62 "/usr/include/time.h" 3








 
typedef __time_t time_t;





# 89 "/usr/include/time.h" 3




# 279 "/usr/include/time.h" 3



# 122 "/usr/include/sys/types.h" 2 3



# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


# 188 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3





 




 

# 271 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 125 "/usr/include/sys/types.h" 2 3



 
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;


 

# 159 "/usr/include/sys/types.h" 3


 







typedef int int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef int int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef int int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef int int64_t __attribute__ ((__mode__ (  __DI__ ))) ;


typedef unsigned int u_int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef unsigned int u_int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef unsigned int u_int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef unsigned int u_int64_t __attribute__ ((__mode__ (  __DI__ ))) ;

typedef int register_t __attribute__ ((__mode__ (__word__)));


 






 
# 1 "/usr/include/endian.h" 1 3
 






















 









 
# 1 "/usr/include/bits/endian.h" 1 3
 






# 35 "/usr/include/endian.h" 2 3


 













# 191 "/usr/include/sys/types.h" 2 3


 
# 1 "/usr/include/sys/select.h" 1 3
 


















 






 


 
# 1 "/usr/include/bits/select.h" 1 3
 

























# 36 "/usr/include/bits/select.h" 3












# 56 "/usr/include/bits/select.h" 3

# 72 "/usr/include/bits/select.h" 3

# 31 "/usr/include/sys/select.h" 2 3


 
# 1 "/usr/include/bits/sigset.h" 1 3
 





















typedef int __sig_atomic_t;

 


typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int))) ];
  } __sigset_t;




 





# 125 "/usr/include/bits/sigset.h" 3

# 34 "/usr/include/sys/select.h" 2 3


 

# 1 "/usr/include/time.h" 1 3
 

















 














# 51 "/usr/include/time.h" 3



# 62 "/usr/include/time.h" 3



# 73 "/usr/include/time.h" 3








 

struct timespec
  {
    long int tv_sec;		 
    long int tv_nsec;		 
  };





# 279 "/usr/include/time.h" 3



# 38 "/usr/include/sys/select.h" 2 3


 

 



struct timeval;

typedef __fd_mask fd_mask;

 
typedef __fd_set fd_set;

 



 




 






 




extern int __select  (int __nfds, __fd_set *__readfds,
			  __fd_set *__writefds, __fd_set *__exceptfds,
			  struct timeval *__timeout)    ;
extern int select  (int __nfds, __fd_set *__readfds,
			__fd_set *__writefds, __fd_set *__exceptfds,
			struct timeval *__timeout)    ;

# 91 "/usr/include/sys/select.h" 3


 


# 194 "/usr/include/sys/types.h" 2 3


 
# 1 "/usr/include/sys/sysmacros.h" 1 3
 





















 








# 47 "/usr/include/sys/sysmacros.h" 3



# 197 "/usr/include/sys/types.h" 2 3




 

typedef __blkcnt_t blkcnt_t;	  
typedef __fsblkcnt_t fsblkcnt_t;  
typedef __fsfilcnt_t fsfilcnt_t;  












 


# 1 "driverstub.c" 2

# 1 "/usr/include/sys/ioctl.h" 1 3
 






















 

 
# 1 "/usr/include/bits/ioctls.h" 1 3
 





















 
# 1 "/usr/include/asm/ioctls.h" 1 3



# 1 "/usr/include/asm/ioctl.h" 1 3
 







 









 






















 












 





 





 








# 4 "/usr/include/asm/ioctls.h" 2 3


 
































































 











# 24 "/usr/include/bits/ioctls.h" 2 3


 




 










































 
		     




 




 




 




 

 






 




# 27 "/usr/include/sys/ioctl.h" 2 3


 
# 1 "/usr/include/bits/ioctl-types.h" 1 3
 






















 



struct winsize
  {
    unsigned short int ws_row;
    unsigned short int ws_col;
    unsigned short int ws_xpixel;
    unsigned short int ws_ypixel;
  };


struct termio
  {
    unsigned short int c_iflag;		 
    unsigned short int c_oflag;		 
    unsigned short int c_cflag;		 
    unsigned short int c_lflag;		 
    unsigned char c_line;		 
    unsigned char c_cc[8 ];		 
};

 












 

 















# 30 "/usr/include/sys/ioctl.h" 2 3


 




# 1 "/usr/include/sys/ttydefaults.h" 1 3
 







































 





 








 





























 




 


 










# 37 "/usr/include/sys/ioctl.h" 2 3


 


extern int ioctl  (int __fd, unsigned long int __request, ...)    ;

 


# 2 "driverstub.c" 2

# 1 "/usr/include/stdlib.h" 1 3
 

















 







 





# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


# 188 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3





 




 





























 


























typedef long int wchar_t;

















# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 33 "/usr/include/stdlib.h" 2 3


 




 
typedef struct
  {
    int quot;			 
    int rem;			 
  } div_t;

 

typedef struct
  {
    long int quot;		 
    long int rem;		 
  } ldiv_t;



# 65 "/usr/include/stdlib.h" 3



 



 





 

extern size_t __ctype_get_mb_cur_max  (void)    ;


 
extern double atof  (__const char *__nptr)    ;
 
extern int atoi  (__const char *__nptr)    ;
 
extern long int atol  (__const char *__nptr)    ;


 
__extension__ extern long long int atoll  (__const char *__nptr)    ;


 
extern double strtod  (__const char *   __nptr,
			   char **   __endptr)    ;










 
extern long int strtol  (__const char *   __nptr,
			     char **   __endptr, int __base)    ;
 
extern unsigned long int strtoul  (__const char *   __nptr,
				       char **   __endptr,
				       int __base)    ;


 
__extension__
extern long long int strtoq  (__const char *   __nptr,
				  char **   __endptr, int __base)    ;
 
__extension__
extern unsigned long long int strtouq  (__const char *   __nptr,
					    char **   __endptr,
					    int __base)    ;



 

 
__extension__
extern long long int strtoll  (__const char *   __nptr,
				   char **   __endptr, int __base)    ;
 
__extension__
extern unsigned long long int strtoull  (__const char *   __nptr,
					     char **   __endptr,
					     int __base)    ;



# 190 "/usr/include/stdlib.h" 3



 


extern double __strtod_internal  (__const char *   __nptr,
				      char **   __endptr,
				      int __group)    ;
extern float __strtof_internal  (__const char *   __nptr,
				     char **   __endptr, int __group)    ;
extern long double  __strtold_internal  (__const char *
						  __nptr,
						char **   __endptr,
						int __group)    ;

extern long int __strtol_internal  (__const char *   __nptr,
					char **   __endptr,
					int __base, int __group)    ;



extern unsigned long int __strtoul_internal  (__const char *
						    __nptr,
						  char **   __endptr,
						  int __base, int __group)    ;




__extension__
extern long long int __strtoll_internal  (__const char *   __nptr,
					      char **   __endptr,
					      int __base, int __group)    ;



__extension__
extern unsigned long long int __strtoull_internal  (__const char *
							  __nptr,
							char **
							  __endptr,
							int __base,
							int __group)    ;




# 326 "/usr/include/stdlib.h" 3




 


extern char *l64a  (long int __n)    ;

 
extern long int a64l  (__const char *__s)    ;




 



 
extern int32_t random  (void)    ;

 
extern void srandom  (unsigned int __seed)    ;

 



extern void *  initstate  (unsigned int __seed, void *  __statebuf,
			       size_t __statelen)    ;

 

extern void *  setstate  (void *  __statebuf)    ;



 



struct random_data
  {
    int32_t *fptr;		 
    int32_t *rptr;		 
    int32_t *state;		 
    int rand_type;		 
    int rand_deg;		 
    int rand_sep;		 
    int32_t *end_ptr;		 
  };

extern int random_r  (struct random_data *   __buf,
			  int32_t *   __result)    ;

extern int srandom_r  (unsigned int __seed, struct random_data *__buf)    ;

extern int initstate_r  (unsigned int __seed,
			     void *    __statebuf,
			     size_t __statelen,
			     struct random_data *   __buf)    ;

extern int setstate_r  (void *    __statebuf,
			    struct random_data *   __buf)    ;




 
extern int rand  (void)    ;
 
extern void srand  (unsigned int __seed)    ;


 
extern int rand_r  (unsigned int *__seed)    ;




 

 
extern double drand48  (void)    ;
extern double erand48  (unsigned short int __xsubi[3])    ;

 
extern long int lrand48  (void)    ;
extern long int nrand48  (unsigned short int __xsubi[3])    ;

 
extern long int mrand48  (void)    ;
extern long int jrand48  (unsigned short int __xsubi[3])    ;

 
extern void srand48  (long int __seedval)    ;
extern unsigned short int *seed48  (unsigned short int __seed16v[3])    ;
extern void lcong48  (unsigned short int __param[7])    ;

 
struct drand48_data
  {
    unsigned short int x[3];	 
    unsigned short int a[3];	 
    unsigned short int c;	 
    unsigned short int old_x[3];  
    int init;			 
  };


 
extern int drand48_r  (struct drand48_data *   __buffer,
			   double *   __result)    ;
extern int erand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *   __buffer,
			   double *   __result)    ;

 
extern int lrand48_r  (struct drand48_data *   __buffer,
			   long int *   __result)    ;
extern int nrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *   __buffer,
			   long int *   __result)    ;

 
extern int mrand48_r  (struct drand48_data *   __buffer,
			   long int *   __result)    ;
extern int jrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *   __buffer,
			   long int *   __result)    ;

 
extern int srand48_r  (long int __seedval, struct drand48_data *__buffer)    ;

extern int seed48_r  (unsigned short int __seed16v[3],
			  struct drand48_data *__buffer)    ;

extern int lcong48_r  (unsigned short int __param[7],
			   struct drand48_data *__buffer)    ;







 
extern void *  malloc  (size_t __size)    ;
 
extern void *  calloc  (size_t __nmemb, size_t __size)    ;



 

extern void *  realloc  (void *  __ptr, size_t __size)    ;
 
extern void free  (void *  __ptr)    ;


 
extern void cfree  (void *  __ptr)    ;



# 1 "/usr/include/alloca.h" 1 3
 























# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


# 188 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3





 




 

# 271 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 25 "/usr/include/alloca.h" 2 3


 

 


 
extern void *  alloca  (size_t __size)    ;





 


# 492 "/usr/include/stdlib.h" 2 3




 
extern void *  valloc  (size_t __size)    ;



 
extern void abort  (void)     __attribute__ ((__noreturn__));


 
extern int atexit  (void (*__func) (void))    ;


 

extern int __on_exit  (void (*__func) (int __status, void *  __arg),
			   void *  __arg)    ;
extern int on_exit  (void (*__func) (int __status, void *  __arg),
			 void *  __arg)    ;


 


extern void exit  (int __status)     __attribute__ ((__noreturn__));








 
extern char *getenv  (__const char *__name)    ;

 

extern char *__secure_getenv  (__const char *__name)    ;


 
 

extern int putenv  (char *__string)    ;



 

extern int setenv  (__const char *__name, __const char *__value,
			int __replace)    ;

 
extern void unsetenv  (__const char *__name)    ;



 


extern int clearenv  (void)    ;




 



extern char *mktemp  (char *__template)    ;

 




extern int mkstemp  (char *__template)    ;



 
extern int system  (__const char *__command)    ;










 





extern char *realpath  (__const char *   __name,
			    char *   __resolved)    ;



 


typedef int (*__compar_fn_t)  (__const void * , __const void * )  ;






 

extern void *  bsearch  (__const void *  __key, __const void *  __base,
			       size_t __nmemb, size_t __size,
			       __compar_fn_t __compar)  ;

 

extern void qsort  (void *  __base, size_t __nmemb, size_t __size,
			  __compar_fn_t __compar)  ;


 
extern int abs  (int __x)     __attribute__ ((__const__));
extern long int labs  (long int __x)     __attribute__ ((__const__));






 

 
extern div_t div  (int __numer, int __denom)     __attribute__ ((__const__));
extern ldiv_t ldiv  (long int __numer, long int __denom)    
     __attribute__ ((__const__));








 


 


extern char *ecvt  (double __value, int __ndigit, int *   __decpt,
			int *   __sign)    ;

 


extern char *fcvt  (double __value, int __ndigit, int *   __decpt,
			int *   __sign)    ;

 


extern char *gcvt  (double __value, int __ndigit, char *__buf)    ;

 
extern char *qecvt  (long double  __value, int __ndigit,
			 int *   __decpt, int *   __sign)    ;
extern char *qfcvt  (long double  __value, int __ndigit,
			 int *   __decpt, int *   __sign)    ;
extern char *qgcvt  (long double  __value, int __ndigit, char *__buf)    ;



 

extern int ecvt_r  (double __value, int __ndigit, int *   __decpt,
			int *   __sign, char *   __buf,
			size_t __len)    ;
extern int fcvt_r  (double __value, int __ndigit, int *   __decpt,
			int *   __sign, char *   __buf,
			size_t __len)    ;

extern int qecvt_r  (long double  __value, int __ndigit,
			 int *   __decpt, int *   __sign,
			 char *   __buf, size_t __len)    ;
extern int qfcvt_r  (long double  __value, int __ndigit,
			 int *   __decpt, int *   __sign,
			 char *   __buf, size_t __len)    ;




 

extern int mblen  (__const char *__s, size_t __n)    ;
 

extern int mbtowc  (wchar_t *   __pwc,
			__const char *   __s, size_t __n)    ;
 

extern int wctomb  (char *__s, wchar_t __wchar)    ;


 
extern size_t mbstowcs  (wchar_t *    __pwcs,
			     __const char *   __s, size_t __n)    ;
 
extern size_t wcstombs  (char *   __s,
			     __const wchar_t *   __pwcs, size_t __n)    ;



 



extern int rpmatch  (__const char *__response)    ;



# 732 "/usr/include/stdlib.h" 3



# 756 "/usr/include/stdlib.h" 3


# 766 "/usr/include/stdlib.h" 3





 


# 3 "driverstub.c" 2

# 1 "/usr/include/string.h" 1 3
 

















 








 

 


# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


# 188 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3





 




 

# 271 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 33 "/usr/include/string.h" 2 3



 
extern void *  memcpy  (void *    __dest,
			    __const void *    __src, size_t __n)    ;
 

extern void *  memmove  (void *  __dest, __const void *  __src,
			     size_t __n)    ;

 



extern void *  memccpy  (void *  __dest, __const void *  __src,
			     int __c, size_t __n)    ;



 
extern void *  memset  (void *  __s, int __c, size_t __n)    ;

 
extern int memcmp  (__const void *  __s1, __const void *  __s2,
			size_t __n)    ;

 
extern void *  memchr  (__const void *  __s, int __c, size_t __n)    ;








 
extern char *strcpy  (char *   __dest,
			  __const char *   __src)    ;
 
extern char *strncpy  (char *   __dest,
			   __const char *   __src, size_t __n)    ;

 
extern char *strcat  (char *   __dest,
			  __const char *   __src)    ;
 
extern char *strncat  (char *   __dest,
			   __const char *   __src, size_t __n)    ;

 
extern int strcmp  (__const char *__s1, __const char *__s2)    ;
 
extern int strncmp  (__const char *__s1, __const char *__s2, size_t __n)    ;

 
extern int strcoll  (__const char *__s1, __const char *__s2)    ;
 
extern size_t strxfrm  (char *   __dest,
			    __const char *   __src, size_t __n)    ;

# 107 "/usr/include/string.h" 3



 
extern char *__strdup  (__const char *__s)    ;
extern char *strdup  (__const char *__s)    ;


 






# 143 "/usr/include/string.h" 3


 
extern char *strchr  (__const char *__s, int __c)    ;
 
extern char *strrchr  (__const char *__s, int __c)    ;







 

extern size_t strcspn  (__const char *__s, __const char *__reject)    ;
 

extern size_t strspn  (__const char *__s, __const char *__accept)    ;
 
extern char *strpbrk  (__const char *__s, __const char *__accept)    ;
 
extern char *strstr  (__const char *__haystack, __const char *__needle)    ;









 
extern char *strtok  (char *   __s,
			  __const char *   __delim)    ;

 

extern char *__strtok_r  (char *   __s,
			      __const char *   __delim,
			      char **   __save_ptr)    ;

extern char *strtok_r  (char *   __s,
			    __const char *   __delim,
			    char **   __save_ptr)    ;


# 203 "/usr/include/string.h" 3



 
extern size_t strlen  (__const char *__s)    ;








 
extern char *strerror  (int __errnum)    ;

 

extern char *__strerror_r  (int __errnum, char *__buf, size_t __buflen)    ;
extern char *strerror_r  (int __errnum, char *__buf, size_t __buflen)    ;


 

extern void __bzero  (void *  __s, size_t __n)    ;


 
extern void bcopy  (__const void *  __src, void *  __dest, size_t __n)    ;

 
extern void bzero  (void *  __s, size_t __n)    ;

 
extern int bcmp  (__const void *  __s1, __const void *  __s2, size_t __n)    ;

 
extern char *index  (__const char *__s, int __c)    ;

 
extern char *rindex  (__const char *__s, int __c)    ;

 

extern int __ffs  (int __i)     __attribute__ ((const));
extern int ffs  (int __i)     __attribute__ ((const));

 









 
extern int __strcasecmp  (__const char *__s1, __const char *__s2)    ;
extern int strcasecmp  (__const char *__s1, __const char *__s2)    ;

 
extern int strncasecmp  (__const char *__s1, __const char *__s2,
			     size_t __n)    ;


# 277 "/usr/include/string.h" 3



 

extern char *strsep  (char **   __stringp,
			  __const char *   __delim)    ;


# 319 "/usr/include/string.h" 3




# 347 "/usr/include/string.h" 3



 


# 4 "driverstub.c" 2

# 1 "/usr/include/fcntl.h" 1 3
 

















 








 
 

 

# 1 "/usr/include/bits/fcntl.h" 1 3
 

























 






















 











 









 














 


 




 




 







struct flock
  {
    short int l_type;	 
    short int l_whence;	 

    __off_t l_start;	 
    __off_t l_len;	 




    __pid_t l_pid;	 
  };

# 131 "/usr/include/bits/fcntl.h" 3


 








# 33 "/usr/include/fcntl.h" 2 3


 






 








 






 

extern int __fcntl  (int __fd, int __cmd, ...)    ;
extern int fcntl  (int __fd, int __cmd, ...)    ;

 


extern int __open  (__const char *__file, int __oflag, ...)    ;

extern int open  (__const char *__file, int __oflag, ...)    ;












 



extern int creat  (__const char *__file, __mode_t __mode)    ;














 



 









extern int lockf  (int __fd, int __cmd, __off_t __len)    ;













 


# 5 "driverstub.c" 2

# 1 "/usr/include/assert.h" 1 3
 

















 



# 32 "/usr/include/assert.h" 3





 




# 56 "/usr/include/assert.h" 3


 

 
extern void __assert_fail  (__const char *__assertion,
				__const char *__file,
				unsigned int __line,
				__const char *__function)    
     __attribute__ ((__noreturn__));

 
extern void __assert_perror_fail  (int __errnum,
				       __const char *__file,
				       unsigned int __line,
				       __const char *__function)    
     __attribute__ ((__noreturn__));

 













 





















# 6 "driverstub.c" 2

# 1 "/usr/include/ctype.h" 1 3
 

















 









 


 














enum
{
  _ISupper = (( 0 ) < 8 ? ((1 << ( 0 )) << 8) : ((1 << ( 0 )) >> 8)) ,	 
  _ISlower = (( 1 ) < 8 ? ((1 << ( 1 )) << 8) : ((1 << ( 1 )) >> 8)) ,	 
  _ISalpha = (( 2 ) < 8 ? ((1 << ( 2 )) << 8) : ((1 << ( 2 )) >> 8)) ,	 
  _ISdigit = (( 3 ) < 8 ? ((1 << ( 3 )) << 8) : ((1 << ( 3 )) >> 8)) ,	 
  _ISxdigit = (( 4 ) < 8 ? ((1 << ( 4 )) << 8) : ((1 << ( 4 )) >> 8)) ,	 
  _ISspace = (( 5 ) < 8 ? ((1 << ( 5 )) << 8) : ((1 << ( 5 )) >> 8)) ,	 
  _ISprint = (( 6 ) < 8 ? ((1 << ( 6 )) << 8) : ((1 << ( 6 )) >> 8)) ,	 
  _ISgraph = (( 7 ) < 8 ? ((1 << ( 7 )) << 8) : ((1 << ( 7 )) >> 8)) ,	 
  _ISblank = (( 8 ) < 8 ? ((1 << ( 8 )) << 8) : ((1 << ( 8 )) >> 8)) ,	 
  _IScntrl = (( 9 ) < 8 ? ((1 << ( 9 )) << 8) : ((1 << ( 9 )) >> 8)) ,	 
  _ISpunct = (( 10 ) < 8 ? ((1 << ( 10 )) << 8) : ((1 << ( 10 )) >> 8)) ,	 
  _ISalnum = (( 11 ) < 8 ? ((1 << ( 11 )) << 8) : ((1 << ( 11 )) >> 8)) 	 
};


 










extern __const unsigned short int *__ctype_b;	 
extern __const __int32_t *__ctype_tolower;  
extern __const __int32_t *__ctype_toupper;  









 



extern int  isalnum   (int)     ;
extern int  isalpha   (int)     ;
extern int  iscntrl   (int)     ;
extern int  isdigit   (int)     ;
extern int  islower   (int)     ;
extern int  isgraph   (int)     ;
extern int  isprint   (int)     ;
extern int  ispunct   (int)     ;
extern int  isspace   (int)     ;
extern int  isupper   (int)     ;
extern int  isxdigit   (int)     ;






 
extern int tolower  (int __c)    ;

 
extern int toupper  (int __c)    ;




 

extern int isascii  (int __c)    ;

 

extern int toascii  (int __c)    ;




 

extern int  _toupper   (int)     ;
extern int  _tolower   (int)     ;



















# 164 "/usr/include/ctype.h" 3


# 186 "/usr/include/ctype.h" 3













# 273 "/usr/include/ctype.h" 3


 


# 7 "driverstub.c" 2

# 1 "/usr/include/getopt.h" 1 3
 




























 





extern char *optarg;

 











extern int optind;

 


extern int opterr;

 

extern int optopt;


 




















struct option
{

  const char *name;



   

  int has_arg;
  int *flag;
  int val;
};

 







 

























 


extern int getopt (int __argc, char *const *__argv, const char *__shortopts);





extern int getopt_long (int __argc, char *const *__argv, const char *__shortopts,
		        const struct option *__longopts, int *__longind);
extern int getopt_long_only (int __argc, char *const *__argv,
			     const char *__shortopts,
		             const struct option *__longopts, int *__longind);

 
extern int _getopt_internal (int __argc, char *const *__argv,
			     const char *__shortopts,
		             const struct option *__longopts, int *__longind,
			     int __long_only);

# 160 "/usr/include/getopt.h" 3






 



# 8 "driverstub.c" 2

# 1 "/usr/include/netinet/in.h" 1 3
 





















# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 1 3
 


 





 
# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h" 1 3
 





# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 1 3
 


 

# 114 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 3



# 1 "/usr/include/limits.h" 1 3
 

















 









 
# 1 "/usr/include/bits/posix1_lim.h" 1 3
 

















 









 

 


 


 


 


 


 


 


 



 


 


 


 


 



 


 


 


 


 


 


 


 


 


 


 


 



 


 


 


 


 



 
# 1 "/usr/include/bits/local_lim.h" 1 3
 


















 





 
# 1 "/usr/include/linux/limits.h" 1 3



















# 27 "/usr/include/bits/local_lim.h" 2 3


 





 

 


 

 


 

 


 



 

# 126 "/usr/include/bits/posix1_lim.h" 2 3








 







# 30 "/usr/include/limits.h" 2 3




# 1 "/usr/include/bits/posix2_lim.h" 1 3
 

















 







 


 


 


 


 




 




 



 


 



 




 































 



# 34 "/usr/include/limits.h" 2 3








 





 

# 121 "/usr/include/limits.h" 3




  








# 117 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 2 3




# 7 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/syslimits.h" 2 3


# 11 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 2 3





 



 



 




 





 



 












 

 




 



 








 



 













 



# 107 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 3




 









# 23 "/usr/include/netinet/in.h" 2 3

# 1 "/usr/include/stdint.h" 1 3
 

















 








# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 190 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 




 


# 269 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 28 "/usr/include/stdint.h" 2 3

# 1 "/usr/include/bits/wordsize.h" 1 3
 


















# 29 "/usr/include/stdint.h" 2 3


 

 

 
# 47 "/usr/include/stdint.h" 3


 
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;



__extension__
typedef unsigned long long int	uint64_t;



 

 
typedef signed char		int_least8_t;
typedef short int		int_least16_t;
typedef int			int_least32_t;



__extension__
typedef long long int		int_least64_t;


 
typedef unsigned char		uint_least8_t;
typedef unsigned short int	uint_least16_t;
typedef unsigned int		uint_least32_t;



__extension__
typedef unsigned long long int	uint_least64_t;



 

 
typedef signed char		int_fast8_t;





typedef int			int_fast16_t;
typedef int			int_fast32_t;
__extension__
typedef long long int		int_fast64_t;


 
typedef unsigned char		uint_fast8_t;





typedef unsigned int		uint_fast16_t;
typedef unsigned int		uint_fast32_t;
__extension__
typedef unsigned long long int	uint_fast64_t;



 








typedef int			intptr_t;


typedef unsigned int		uintptr_t;



 




__extension__
typedef long long int		intmax_t;
__extension__
typedef unsigned long long int	uintmax_t;



 











 

 




 





 






 




 





 






 









 










 











 











 

 


 



 

 








 



 






 

 




 






 



 









 









 











# 24 "/usr/include/netinet/in.h" 2 3



# 1 "/usr/include/bits/socket.h" 1 3
 



























# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 1 3






 


# 19 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3



 


 





 


# 61 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 





 


















 





 

 

# 131 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 


# 188 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3





 




 

# 271 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


# 283 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3


 

 

# 317 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/stddef.h" 3




 





















# 29 "/usr/include/bits/socket.h" 2 3


# 1 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 1 3
 


 

# 114 "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/limits.h" 3







# 31 "/usr/include/bits/socket.h" 2 3



 
typedef unsigned int socklen_t;

 
enum __socket_type
{
  SOCK_STREAM = 1,		 


  SOCK_DGRAM = 2,		 


  SOCK_RAW = 3,			 

  SOCK_RDM = 4,			 

  SOCK_SEQPACKET = 5,		 


  SOCK_PACKET = 10		 



};

 




























 




























 











 


 
# 1 "/usr/include/bits/sockaddr.h" 1 3
 


















 







 
typedef unsigned short int sa_family_t;

 








 

extern int __libc_sa_len  (sa_family_t __af)    ;


# 133 "/usr/include/bits/socket.h" 2 3


 
struct sockaddr
  {
    sa_family_t  sa_family ;	 
    char sa_data[14];		 
  };


 









struct sockaddr_storage
  {
    sa_family_t  __ss_family ;	 
    __uint32_t  __ss_align;	 
    char __ss_padding[(128  - (2 * sizeof (__uint32_t ))) ];
  };


 
enum
  {
    MSG_OOB		= 0x01,	 

    MSG_PEEK		= 0x02,	 

    MSG_DONTROUTE	= 0x04,	 






    MSG_CTRUNC		= 0x08,	 

    MSG_PROXY		= 0x10,	 

    MSG_TRUNC		= 0x20,

    MSG_DONTWAIT	= 0x40,  

    MSG_EOR		= 0x80,  

    MSG_WAITALL		= 0x100,  

    MSG_FIN		= 0x200,

    MSG_SYN		= 0x400,

    MSG_URG		= 0x800,

    MSG_RST		= 0x1000,

    MSG_ERRQUEUE	= 0x2000,  

    MSG_NOSIGNAL	= 0x4000   

  };


 

struct msghdr
  {
    void *  msg_name;		 
    socklen_t msg_namelen;	 

    struct iovec *msg_iov;	 
    size_t msg_iovlen;		 

    void *  msg_control;	 
    size_t msg_controllen;	 

    int msg_flags;		 
  };

 
struct cmsghdr
  {
    size_t cmsg_len;		 

    int cmsg_level;		 
    int cmsg_type;		 

    unsigned char __cmsg_data[0];  
     

  };

 















extern struct cmsghdr *__cmsg_nxthdr  (struct msghdr *__mhdr,
					   struct cmsghdr *__cmsg)    ;
# 270 "/usr/include/bits/socket.h" 3


 

enum
  {
    SCM_RIGHTS = 0x01,		 


    SCM_CREDENTIALS = 0x02,      


    __SCM_CONNECT = 0x03	 
  };

 

struct ucred
{
  pid_t  pid;			 
  uid_t  uid;			 
  gid_t  gid;			 
};

 
# 1 "/usr/include/asm/socket.h" 1 3



# 1 "/usr/include/asm/sockios.h" 1 3



 








# 4 "/usr/include/asm/socket.h" 2 3


 
















 







 






 



 
# 55 "/usr/include/asm/socket.h" 3



# 295 "/usr/include/bits/socket.h" 2 3



 
struct linger
  {
    int l_onoff;		 
    int l_linger;		 
  };


# 27 "/usr/include/netinet/in.h" 2 3



 

 
enum
  {
    IPPROTO_IP = 0,	    
    IPPROTO_HOPOPTS = 0,    
    IPPROTO_ICMP = 1,	    
    IPPROTO_IGMP = 2,	    
    IPPROTO_IPIP = 4,	    
    IPPROTO_TCP = 6,	    
    IPPROTO_EGP = 8,	    
    IPPROTO_PUP = 12,	    
    IPPROTO_UDP = 17,	    
    IPPROTO_IDP = 22,	    
    IPPROTO_TP = 29,	    
    IPPROTO_IPV6 = 41,      
    IPPROTO_ROUTING = 43,   
    IPPROTO_FRAGMENT = 44,  
    IPPROTO_RSVP = 46,	    
    IPPROTO_GRE = 47,	    
    IPPROTO_ESP = 50,       
    IPPROTO_AH = 51,        
    IPPROTO_ICMPV6 = 58,    
    IPPROTO_NONE = 59,      
    IPPROTO_DSTOPTS = 60,   
    IPPROTO_MTP = 92,	    
    IPPROTO_ENCAP = 98,	    
    IPPROTO_PIM = 103,	    
    IPPROTO_COMP = 108,	    
    IPPROTO_RAW = 255,	    
    IPPROTO_MAX
  };

 
enum
  {
    IPPORT_ECHO = 7,		 
    IPPORT_DISCARD = 9,		 
    IPPORT_SYSTAT = 11,		 
    IPPORT_DAYTIME = 13,	 
    IPPORT_NETSTAT = 15,	 
    IPPORT_FTP = 21,		 
    IPPORT_TELNET = 23,		 
    IPPORT_SMTP = 25,		 
    IPPORT_TIMESERVER = 37,	 
    IPPORT_NAMESERVER = 42,	 
    IPPORT_WHOIS = 43,		 
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,		 
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,		 
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,		 


    IPPORT_EXECSERVER = 512,	 
    IPPORT_LOGINSERVER = 513,	 
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,

     
    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,

     
    IPPORT_RESERVED = 1024,

     
    IPPORT_USERRESERVED = 5000
  };


 
struct in_addr
  {
    uint32_t s_addr;
  };


 



























 

 

 


 

 




 






 
struct in6_addr
  {
    union
      {
	uint8_t		u6_addr8[16];
	uint16_t	u6_addr16[8];
	uint32_t	u6_addr32[4];



      } in6_u;




  };

extern const struct in6_addr in6addr_any;         
extern const struct in6_addr in6addr_loopback;    






 



 
struct sockaddr_in
  {
    sa_family_t  sin_family ;
    uint16_t sin_port;			 
    struct in_addr sin_addr;		 

     
    unsigned char sin_zero[sizeof (struct sockaddr) -
			   (sizeof (unsigned short int))  -
			   sizeof (uint16_t) -
			   sizeof (struct in_addr)];
  };

 
struct sockaddr_in6
  {
    sa_family_t  sin6_family ;
    uint16_t sin6_port;		 
    uint32_t sin6_flowinfo;	 
    struct in6_addr sin6_addr;	 
  };

 
struct ipv6_mreq
  {
     
    struct in6_addr ipv6mr_multiaddr;

     
    unsigned int ipv6mr_interface;
  };

 
# 1 "/usr/include/bits/in.h" 1 3
 

















 





 






















 


 




 






 


struct ip_opts
  {
    struct in_addr ip_dst;	 
    char ip_opts[40];		 
  };

 
struct ip_mreq
  {
    struct in_addr imr_multiaddr;	 
    struct in_addr imr_interface;	 
  };

 
struct ip_mreqn
  {
    struct in_addr imr_multiaddr;	 
    struct in_addr imr_address;		 
    int	imr_ifindex;			 
  };

 
struct in_pktinfo
  {
    int ipi_ifindex;			 
    struct in_addr ipi_spec_dst;	 
    struct in_addr ipi_addr;		 
  };

 

























 




 


# 225 "/usr/include/netinet/in.h" 2 3


 






extern uint32_t ntohl  (uint32_t __netlong)    ;
extern uint16_t ntohs  (uint16_t __netshort)    ;
extern uint32_t htonl  (uint32_t __hostlong)    ;
extern uint16_t htons  (uint16_t __hostshort)    ;



 
# 1 "/usr/include/bits/byteswap.h" 1 3
 






















 





# 40 "/usr/include/bits/byteswap.h" 3






 





 



# 68 "/usr/include/bits/byteswap.h" 3
# 79 "/usr/include/bits/byteswap.h" 3







 









# 242 "/usr/include/netinet/in.h" 2 3
















































 
extern int bindresvport  (int __sockfd, struct sockaddr_in *__sock_in)    ;


















 
struct in6_pktinfo
  {
    struct in6_addr	ipi6_addr;     
    unsigned int	ipi6_ifindex;  
  };

 


# 9 "driverstub.c" 2

# 1 "/usr/include/zlib.h" 1 3
 
































# 1 "/usr/include/zconf.h" 1 3
 




 




 



# 49 "/usr/include/zconf.h" 3














 

























 




 





 








 








 












                         









 





# 155 "/usr/include/zconf.h" 3








 
# 189 "/usr/include/zconf.h" 3

























typedef unsigned char  Byte;   

typedef unsigned int   uInt;   
typedef unsigned long  uLong;  





   typedef Byte    Bytef;

typedef char    charf;
typedef int     intf;
typedef uInt    uIntf;
typedef uLong   uLongf;


   typedef void   *voidpf;
   typedef void     *voidp;



















 
# 277 "/usr/include/zconf.h" 3



# 34 "/usr/include/zlib.h" 2 3








 




















typedef voidpf (*alloc_func)  (voidpf opaque, uInt items, uInt size)  ;
typedef void   (*free_func)   (voidpf opaque, voidpf address)  ;

struct internal_state;

typedef struct z_stream_s {
    Bytef    *next_in;   
    uInt     avail_in;   
    uLong    total_in;   

    Bytef    *next_out;  
    uInt     avail_out;  
    uLong    total_out;  

    char     *msg;       
    struct internal_state   *state;  

    alloc_func zalloc;   
    free_func  zfree;    
    voidpf     opaque;   

    int     data_type;   
    uLong   adler;       
    uLong   reserved;    
} z_stream;

typedef z_stream   *z_streamp;

 































                         






 










 







 




 




 


 




 

                         

extern  const char *   zlibVersion  (void)  ;
 





 






















extern  int   deflate  (z_streamp strm, int flush)  ;
 












































































extern  int   deflateEnd  (z_streamp strm)  ;
 












 




















extern  int   inflate  (z_streamp strm, int flush)  ;
 



































































extern  int   inflateEnd  (z_streamp strm)  ;
 









                         

 



 










































                            
extern  int   deflateSetDictionary  (z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength)  ;
 
































extern  int   deflateCopy  (z_streamp dest,
                                    z_streamp source)  ;
 















extern  int   deflateReset  (z_streamp strm)  ;
 









extern  int   deflateParams  (z_streamp strm,
				      int level,
				      int strategy)  ;
 

















 






















extern  int   inflateSetDictionary  (z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength)  ;
 















extern  int   inflateSync  (z_streamp strm)  ;
 













extern  int   inflateReset  (z_streamp strm)  ;
 









                         

 







extern  int   compress  (Bytef *dest,   uLongf *destLen,
                                 const Bytef *source, uLong sourceLen)  ;
 












extern  int   compress2  (Bytef *dest,   uLongf *destLen,
                                  const Bytef *source, uLong sourceLen,
                                  int level)  ;
 











extern  int   uncompress  (Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen)  ;
 
















typedef voidp gzFile;

extern  gzFile   gzopen   (const char *path, const char *mode)  ;
 














extern  gzFile   gzdopen   (int fd, const char *mode)  ;
 











extern  int   gzsetparams  (gzFile file, int level, int strategy)  ;
 






extern  int      gzread   (gzFile file, voidp buf, unsigned len)  ;
 






extern  int      gzwrite  (gzFile file, 
				   const voidp buf, unsigned len)  ;
 





extern  int     gzprintf  (gzFile file, const char *format, ...)  ;
 





extern  int   gzputs  (gzFile file, const char *s)  ;
 





extern  char *   gzgets  (gzFile file, char *buf, int len)  ;
 







extern  int      gzputc  (gzFile file, int c)  ;
 




extern  int      gzgetc  (gzFile file)  ;
 




extern  int      gzflush  (gzFile file, int flush)  ;
 








extern  long       gzseek  (gzFile file,
				      long  offset, int whence)  ;
 















extern  int      gzrewind  (gzFile file)  ;
 





extern  long       gztell  (gzFile file)  ;
 







extern  int   gzeof  (gzFile file)  ;
 




extern  int      gzclose  (gzFile file)  ;
 





extern  const char *   gzerror  (gzFile file, int *errnum)  ;
 







                         

 





extern  uLong   adler32  (uLong adler, const Bytef *buf, uInt len)  ;

 














extern  uLong   crc32    (uLong crc, const Bytef *buf, uInt len)  ;
 















                         

 


extern  int   deflateInit_  (z_streamp strm, int level,
                                     const char *version, int stream_size)  ;
extern  int   inflateInit_  (z_streamp strm,
                                     const char *version, int stream_size)  ;
extern  int   deflateInit2_  (z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size)  ;
extern  int   inflateInit2_  (z_streamp strm, int  windowBits,
                                      const char *version, int stream_size)  ;












    struct internal_state {int dummy;};  


extern  const char   *   zError            (int err)  ;
extern  int              inflateSyncPoint  (z_streamp z)  ;
extern  const uLongf *   get_crc_table     (void)  ;






# 10 "driverstub.c" 2

# 1 "/usr/include/signal.h" 1 3
 

















 











 

# 1 "/usr/include/bits/sigset.h" 1 3
 


















# 33 "/usr/include/bits/sigset.h" 3



 












 



 














# 97 "/usr/include/bits/sigset.h" 3



 


extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);

# 122 "/usr/include/bits/sigset.h" 3




# 33 "/usr/include/signal.h" 2 3


 





typedef __sig_atomic_t sig_atomic_t;







typedef __sigset_t sigset_t;






# 1 "/usr/include/bits/signum.h" 1 3
 




















 









 










































 





# 56 "/usr/include/signal.h" 2 3


# 67 "/usr/include/signal.h" 3



 
typedef void (*__sighandler_t)  (int)  ;

 


extern __sighandler_t __sysv_signal  (int __sig,
					  __sighandler_t __handler)    ;




 



extern __sighandler_t signal  (int __sig, __sighandler_t __handler)    ;
# 96 "/usr/include/signal.h" 3








 



extern int kill  (__pid_t __pid, int __sig)    ;



 


extern int killpg  (__pid_t __pgrp, int __sig)    ;


 
extern int raise  (int __sig)    ;


 
extern __sighandler_t ssignal  (int __sig, __sighandler_t __handler)    ;
extern int gsignal  (int __sig)    ;



 
extern void psignal  (int __sig, __const char *__s)    ;



 




extern int __sigpause  (int __sig_or_mask, int __is_sig)    ;


 

extern int sigpause  (int __mask)    ;










 




 


 
extern int sigblock  (int __mask)    ;

 
extern int sigsetmask  (int __mask)    ;

 
extern int siggetmask  (void)    ;











 

typedef __sighandler_t sig_t;





 

# 1 "/usr/include/time.h" 1 3
 

















 














# 51 "/usr/include/time.h" 3



# 62 "/usr/include/time.h" 3



# 73 "/usr/include/time.h" 3




# 89 "/usr/include/time.h" 3




# 279 "/usr/include/time.h" 3



# 191 "/usr/include/signal.h" 2 3


 
# 1 "/usr/include/bits/siginfo.h" 1 3
 


























 
typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;




typedef struct siginfo
  {
    int si_signo;		 
    int si_errno;		 

    int si_code;		 

    union
      {
	int _pad[((128  / sizeof (int)) - 3) ];

	  
	struct
	  {
	    __pid_t si_pid;	 
	    __uid_t si_uid;	 
	  } _kill;

	 
	struct
	  {
	    unsigned int _timer1;
	    unsigned int _timer2;
	  } _timer;

	 
	struct
	  {
	    __pid_t si_pid;	 
	    __uid_t si_uid;	 
	    sigval_t si_sigval;	 
	  } _rt;

	 
	struct
	  {
	    __pid_t si_pid;	 
	    __uid_t si_uid;	 
	    int si_status;	 
	    __clock_t si_utime;
	    __clock_t si_stime;
	  } _sigchld;

	 
	struct
	  {
	    void *si_addr;	 
	  } _sigfault;

	 
	struct
	  {
	    int si_band;	 
	    int si_fd;
	  } _sigpoll;
      } _sifields;
  } siginfo_t;


 













 

enum
{
  SI_SIGIO = -5,		 

  SI_ASYNCIO,			 

  SI_MESGQ,			 

  SI_TIMER,			 

  SI_QUEUE,			 

  SI_USER			 

};


 
enum
{
  ILL_ILLOPC = 1,		 

  ILL_ILLOPN,			 

  ILL_ILLADR,			 

  ILL_ILLTRP,			 

  ILL_PRVOPC,			 

  ILL_PRVREG,			 

  ILL_COPROC,			 

  ILL_BADSTK			 

};

 
enum
{
  FPE_INTDIV = 1,		 

  FPE_INTOVF,			 

  FPE_FLTDIV,			 

  FPE_FLTOVF,			 

  FPE_FLTUND,			 

  FPE_FLTRES,			 

  FPE_FLTINV,			 

  FPE_FLTSUB			 

};

 
enum
{
  SEGV_MAPERR = 1,		 

  SEGV_ACCERR			 

};

 
enum
{
  BUS_ADRALN = 1,		 

  BUS_ADRERR,			 

  BUS_OBJERR			 

};

 
enum
{
  TRAP_BRKPT = 1,		 

  TRAP_TRACE			 

};

 
enum
{
  CLD_EXITED = 1,		 

  CLD_KILLED,			 

  CLD_DUMPED,			 

  CLD_TRAPPED,			 

  CLD_STOPPED,			 

  CLD_CONTINUED			 

};

 
enum
{
  POLL_IN = 1,			 

  POLL_OUT,			 

  POLL_MSG,			 

  POLL_ERR,			 

  POLL_PRI,			 

  POLL_HUP			 

};








 



typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;

    union
      {
	int _pad[((64  / sizeof (int)) - 3) ];

	struct
	  {
	    void (*_function)  (sigval_t)  ;  
	    void *_attribute;			   
	  } _sigev_thread;
      } _sigev_un;
  } sigevent_t;

 



 
enum
{
  SIGEV_SIGNAL = 0,		 

  SIGEV_NONE,			 

  SIGEV_THREAD			 

};


# 194 "/usr/include/signal.h" 2 3



 
extern int sigemptyset  (sigset_t *__set)    ;

 
extern int sigfillset  (sigset_t *__set)    ;

 
extern int sigaddset  (sigset_t *__set, int __signo)    ;

 
extern int sigdelset  (sigset_t *__set, int __signo)    ;

 
extern int sigismember  (__const sigset_t *__set, int __signo)    ;

# 223 "/usr/include/signal.h" 3


 

# 1 "/usr/include/bits/sigaction.h" 1 3
 






















 
struct sigaction
  {
     

    union
      {
	 
	__sighandler_t sa_handler;
	 
	void (*sa_sigaction)  (int, siginfo_t *, void *)  ;
      }
    __sigaction_handler;






     
    __sigset_t sa_mask;

     
    int sa_flags;

     
    void (*sa_restorer)  (void)  ;
  };

 














 





 



# 227 "/usr/include/signal.h" 2 3


 
extern int sigprocmask  (int __how,
			     __const sigset_t *__set, sigset_t *__oset)    ;

 

extern int sigsuspend  (__const sigset_t *__set)    ;

 
extern int __sigaction  (int __sig, __const struct sigaction *__act,
			     struct sigaction *__oact)    ;
extern int sigaction  (int __sig, __const struct sigaction *__act,
			   struct sigaction *__oact)    ;

 
extern int sigpending  (sigset_t *__set)    ;


 
extern int sigwait  (__const sigset_t *__set, int *__sig)    ;


 
extern int sigwaitinfo  (__const sigset_t *__set, siginfo_t *__info)    ;

 

extern int sigtimedwait  (__const sigset_t *__set, siginfo_t *__info,
			      __const struct timespec *__timeout)    ;

 

extern int sigqueue  (__pid_t __pid, int __sig,
			  __const union sigval __val)    ;






 

extern __const char *__const _sys_siglist[64 ];
extern __const char *__const sys_siglist[64 ];

 
struct sigvec
  {
    __sighandler_t sv_handler;	 
    int sv_mask;		 

    int sv_flags;		 

  };

 





 




extern int sigvec  (int __sig, __const struct sigvec *__vec,
			struct sigvec *__ovec)    ;


 
# 1 "/usr/include/bits/sigcontext.h" 1 3
 






















 



# 1 "/usr/include/asm/sigcontext.h" 1 3



 







struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpstate {
	unsigned long 	cw,
			sw,
			tag,
			ipoff,
			cssel,
			dataoff,
			datasel;
	struct _fpreg	_st[8];
	unsigned long	status;
};

struct sigcontext {
	unsigned short gs, __gsh;
	unsigned short fs, __fsh;
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long trapno;
	unsigned long err;
	unsigned long eip;
	unsigned short cs, __csh;
	unsigned long eflags;
	unsigned long esp_at_signal;
	unsigned short ss, __ssh;
	struct _fpstate * fpstate;
	unsigned long oldmask;
	unsigned long cr2;
};



# 28 "/usr/include/bits/sigcontext.h" 2 3


# 300 "/usr/include/signal.h" 2 3


 
extern int sigreturn  (struct sigcontext *__scp)    ;






 


extern int siginterrupt  (int __sig, int __interrupt)    ;

# 1 "/usr/include/bits/sigstack.h" 1 3
 























 
struct sigstack
  {
    void *  ss_sp;		 
    int ss_onstack;		 
  };


 
enum
{
  SS_ONSTACK = 1,

  SS_DISABLE

};

 


 



 
typedef struct sigaltstack
  {
    void *  ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
# 315 "/usr/include/signal.h" 2 3





 


extern int sigstack  (struct sigstack *__ss, struct sigstack *__oss)    ;

 

extern int sigaltstack  (__const struct sigaltstack *__ss,
			     struct sigaltstack *__oss)    ;



# 350 "/usr/include/signal.h" 3


 


 
extern int __libc_current_sigrtmin  (void)    ;
 
extern int __libc_current_sigrtmax  (void)    ;



 


# 11 "driverstub.c" 2
