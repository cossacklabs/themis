/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
*/

# ifndef USYMTAB_INTERFACE_H
# define USYMTAB_INTERFACE_H

extern void doDeclareConstant (constDeclarationNode, bool);
extern void doDeclareVar      (varDeclarationNode, bool);
extern void doDeclareType     (typeNode, bool);
extern void doDeclareFcn      (fcnNode p_f, typeId p_tn, bool p_priv, bool p_spec);
extern void declareIter        (iterNode p_iter);

extern void declareConstant (constDeclarationNode);
extern void declareVar      (varDeclarationNode);
extern void declareType     (typeNode);
extern void declareFcn      (fcnNode p_f, typeId p_t);

extern void declarePrivConstant (constDeclarationNode);
extern void declarePrivVar      (varDeclarationNode);
extern void declarePrivType     (typeNode);
extern void declarePrivFcn      (fcnNode p_f, typeId p_t);

# define declareConstant(c)     doDeclareConstant(c, FALSE)
# define declareVar(c)          doDeclareVar(c, FALSE)
# define declareType(c)         doDeclareType(c, FALSE)
# define declareFcn(f, t)       doDeclareFcn(f, t, FALSE, TRUE)

# define declarePrivConstant(c) doDeclareConstant(c, TRUE)
# define declarePrivVar(c)      doDeclareVar(c, TRUE)
# define declarePrivType(c)     doDeclareType(c, TRUE)
# define declarePrivFcn(f, t)   doDeclareFcn(f, t, TRUE, TRUE)

# else
# error "Multiple include"
# endif




