m4_define([AH_LCLINT_TEMPLATE],
[AH_VERBATIM([$1],
             m4_text_wrap([$2 */], [   ], [/* ])
             m4_text_wrap([$3@*/], [   ], [/*@])[
#undef $1])])

m4_define([AC_DEFINE_LCLINT],
[AC_DEFINE_TRACE([$1])dnl
AH_LCLINT_TEMPLATE([$1], [$3], [$4])dnl
cat >>confdefs.h <<\_ACEOF
[@%:@define] $1 [$2]
_ACEOF
])
