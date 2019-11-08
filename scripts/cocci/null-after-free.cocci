// Read a list of 'free' type function names from stdin, if stdin is a pipe.
@initialize:python@
@@
import sys
functions = set(['free'])
if not sys.stdin.isatty():
    for line in sys.stdin:
        f = line.rstrip('\n')
        functions.add(f)

// Match all function names.
@a@
identifier F;
@@
    F(...)

// Match all function names in the set.
@script:python b depends on a@
F << a.F;
f;
@@
if F in functions:
    coccinelle.f = F
    cocci.include_match(True)

// Remove casts.
@@
identifier b.f;
type t;
expression x;
@@
-   f((t *)(x))
+   f(x)

// Add NULL assignment after free, if absent.
@@
identifier b.f;
expression x;
@@
(
    f(x);
    x = NULL;
|
    f(x);
+   x = NULL;
)

// Remove NULL assignments to local variables.
@@
local idexpression x;
@@
-   x = NULL;
    ... when != x

// Remove NULL assignments followed later by another assignment.
@@
idexpression x;
identifier y;
@@
(
-   x = NULL;
    ... when != x
    x = ...;
|
-   x->y = NULL;
    ... when != x->y
    x->y = ...;
)

// Remove NULL assignments followed immediately by another assignment.
@@
expression z;
@@
-   z = NULL;
    z = ...;

// Remove NULL assignments to structure members before the structure is freed.
@@
identifier b.f;
identifier x, y;
@@
-   x->y = NULL;
    ... when != x
    f(x);
