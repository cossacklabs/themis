/*
** Copyright (C) University of Virginia, Massachusetts Institue of Technology 1994-2003.
** See ../LICENSE for license information.
**
*/
/*
** lslinit.h
*/

enum e_LSLInitRuleCode {
  /* initFile:				    */
  INITFILE1,		/*    initLines EOF */
  /* initLines: */
  INITLINES1,         /*		*/
  INITLINES2,         /*     initLine  */
  INITLINES3,         /*     initLines initLine */
  
  /* initLine: */
  INITLINE1,		/*     EOL   */
  INITLINE2,          /*     classification EOL */
  
  /* classification: */
  CLASSIFICATION1,    /*     charClass */
  CLASSIFICATION2,    /*     tokenClass */
  CLASSIFICATION3,    /*     synClass */
  
  /* charClass: */
  CHARCLASS1,         /*     ENDCOMMENTCHAR endCommentChars */
  CHARCLASS2,         /*     IDCHAR idChars */
  CHARCLASS3,         /*     OPCHAR opChars */
  CHARCLASS4,         /*     CHC_EXTENSION extensionChar */
  CHARCLASS5,         /*     SINGLECHAR singChars */
  CHARCLASS6,         /*     WHITECHAR whiteChars */

  /* endCommentChars: */
  LRC_ENDCOMMENT1,	/*     endCommentChar */
  LRC_ENDCOMMENT2,	/*     endCommentChars endCommentChar */
  
  /* idChars: */
  IDCHARS1,		/*     idChar */
  IDCHARS2,		/*     idChars idChar */
  
  /* opChars */
  OPCHARS1,		/*    opChar */
  OPCHARS2,		/*    opChars opChar */
  
  /* ExtensionChar */
  LRC_EXTENSIONCHAR1,	/*     singleChar */
  
  /* singChars */
  SINGCHARS1,		/*     singChar */
  SINGCHARS2,		/*     singChars singChar */
  
  /* whiteChars */
  WHITECHARS1,	/*     whiteChar */
  WHITECHARS2,	/*     whiteChars whiteChar */
  
  /* endCommentChar */
  LRC_ENDCOMMENTCHAR1,	/*     commentChar */
  
  /* idChar */
  IDCHAR1,		/*     singleChar */
  
  /* opChar */
  OPCHAR1,		/*     singleChar */
  
  /* singChar */
  SINGCHAR1,		/*     singleChar */
  
  /* whiteChar */
  WHITECHAR1,		/*     singleChar */
  
  /* tokenClass: */
  TOKENCLASS1,        /*     QUANTIFIERSYM quantifierSymToks */
  TOKENCLASS2,        /*     LOGICALOP logicalOpToks */
  TOKENCLASS3,        /*     EQOP eqOpToks */
  TOKENCLASS4,        /*     EQUATIONSYM equationSymToks */
  TOKENCLASS5,        /*     EQSEPSYM eqSepSymToks */
  TOKENCLASS6,        /*     SELECTSYM selectSymToks */
  TOKENCLASS7,        /*     OPENSYM openSymToks */
  TOKENCLASS8,        /*     SEPSYM sepSymToks */
  TOKENCLASS9,        /*     CLOSESYM closeSymToks */
  TOKENCLASS10,       /*     SIMPLEID simpleIdToks */
  TOKENCLASS11,       /*     MAPSYM mapSymToks */
  TOKENCLASS12,       /*     MARKERSYM markerSymToks */
  TOKENCLASS13,       /*     COMMENTSYM commentSymToks */
  
  /* quantifierSymToks */
  QUANTIFIERSYMTOKS1,/*     quantifierSymTok */
  QUANTIFIERSYMTOKS2,/*     quantifierSymToks quantifierSymTok */
  
  /* logicalOpToks */
  LOGICALOPTOKS1,	/*     logicalOpTok */
  LOGICALOPTOKS2,	/*     logicalOpToks logicalOpTok */
  
  /* eqOpToks */
  LRC_EQOPTOKS1,		/*     eqOpTok */
  LRC_EQOPTOKS2,		/*     eqOpToks eqOpTok */
  
  /* equationSymToks */
  LRC_EQUATIONSYMTOKS1, /*     equationSymTok */
  LRC_EQUATIONSYMTOKS2,	/*     equationSymToks equationSymTok */
  
  /* eqSepSymToks */
  LRC_EQSEPSYMTOKS1,	/*     eqSepSymTok */
  LRC_EQSEPSYMTOKS2,	/*     eqSepSymToks eqSepSymTok */
  
  /* selectSymToks */
  SELECTSYMTOKS1,	/*     selectSymTok */
  SELECTSYMTOKS2,	/*     selectSymToks selectSymTok */
  
  /* openSymToks */
  OPENSYMTOKS1,	/*     openSymTok */
  OPENSYMTOKS2,	/*     openSymToks openSymTok */
  
  /* sepSymToks */
  SEPSYMTOKS1,	/*     sepSymTok */
  SEPSYMTOKS2,	/*     sepSymToks sepSymTok */
  
  /* closeSymToks */
  CLOSESYMTOKS1,	/*     closeSymTok */
  CLOSESYMTOKS2,	/*     closeSymToks closeSymTok */
  
  /* simpleIdToks */
  SIMPLEIDTOKS1,	/*     simpleIdTok */
  SIMPLEIDTOKS2,	/*     simpleIdToks simpleIdTok */
  
  /* mapSymToks */
  MAPSYMTOKS1,		/*     mapSymTok */
  MAPSYMTOKS2,		/*     mapSymToks mapSymTok */
  
  /* markerSymToks */
  MARKERSYMTOKS1,	/*     markerSymTok */
  MARKERSYMTOKS2,	/*     markerSymToks markerSymTok */
  
  /* commentSymToks */
  COMMENTSYMTOKS1,	/*     commentSymTok */
  COMMENTSYMTOKS2,	/*     commentSymToks commentSymTok */
  
  /* quantifierSymTok */
  QUANTIFIERSYMTOK1,	/*     token */
  
  /* logicalOpTok */
  LOGICALOPTOK1,	/*     token */
  
  /* eqOpTok */
  LRC_EQOPTOK1,		/*     token */
  
  /* equationSymTok */
  LRC_EQUATIONSYMTOK1,	/*     token */
  
  /* eqSepSymTok */
  LRC_EQSEPSYMTOK1,	/*     token */
  
  /* selectSymTok */
  SELECTSYMTOK1,	/*     token */
  
  /* openSymTok */
  OPENSYMTOK1,	/*     token */
  
  /* sepSymTok */
  SEPSYMTOK1,		/*     token */
  
  /* closeSymTok */
  CLOSESYMTOK1,	/*     token */
  
  /* simpleIdTok */
  SIMPLEIDTOK1,	/*     token */
  
  /* mapSymTok */
  MAPSYMTOK1,		/*     token */
  
  /* markerSymTok */
  MARKERSYMTOK1,	/*     token */
  
  /* commentSymTok */
  COMMENTSYMTOK1,	/*     token */
  
  /* synClass: */
  SYNCLASS1,          /*     SYNONYM oldToken newToken */
  
  /* oldToken: */
  OLDTOKEN1,          /*     token */
  
  /* newToken: */
  NEWTOKEN1           /*     token */
} ;

extern void lslinit_setInitFile (/*@only@*/ inputStream p_s)
     /*@modifies internalState@*/ ;

extern void lslinit_process (void)     
     /*@globals undef g_symtab; @*/
     /*@modifies g_symtab, internalState, fileSystem; @*/ ;
