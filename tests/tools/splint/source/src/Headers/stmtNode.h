/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

struct s_stmtNode {
  ltoken lhs;
  ltoken operator;
  termNodeList args;
} ;

extern /*@unused@*/ /*@notnull@*/ /*@only@*/ cstring stmtNode_unparse (stmtNode p_x);
