int main()
{
  static void nfunc(void);
  nfunc();
  return 0;
}
static void nfunc(){}

/*
From: "Johnson, Nathan E. (AZ76)" <NEJohnson@space.honeywell.com>
To: "'lclint-bug'" <lclint-bug@salsa.lcs.mit.edu>
Subject: Mishandling of prototypes stated within functions
Date: Wed, 26 May 1999 09:00:24 -0700
Importance: low
X-Priority: 5
MIME-Version: 1.0
X-Mailer: Internet Mail Service (5.5.2448.0)
Content-Type: text/plain

Using flags:
+singleinclude
-warnunixlib
+unixlib

The following program:
----------------------------
int main()
    {
    static void nothing(void);
    nothing();
    return 0;
    }
static void nothing(){}

------------------------------
yields:
------------------------------
LCLint 2.4b --- 18 Apr 98

lbug.c:7:13: File static function nothing declared but not used
  A function is declared but not used. Use @unused@ in front of function
  header to suppress message. (-fcnuse will suppress message)
   lbug.c:7:24: Definition of nothing

Finished LCLint checking --- 1 code error found

*/
