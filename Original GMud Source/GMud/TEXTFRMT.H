/////////////////////////////////////////////////////////////////////////
//
//      TEXTFRMT.H - this module includes the defined for the text wrapping functions.
//      there is only one publicly accessable function
//      created 08/04/93 by Dan Kelly for use in ACE Reporter (c) 1993 DDI
//      actual code is in textfrmt.cpp
//                 

#ifndef _TEXT_H_DEFINED_
#define _TEXT_H_DEFINED_                   

///////////////////
//
//      Function prototypes
//

int Charsinwidth(CDC &mydc,LPCSTR strtocalc,int width);

int CharsInColumns(LPCSTR str, BOOL bWordWrap=FALSE,int columns=80);

int AnsiChars(LPCSTR str,int columns = 80 );

int Ansimemcpy(LPSTR pDest,LPCSTR pSrc,int iFirstChar,int iLength);

int MyAnsiNext(LPCSTR pStr);

#endif //_TEXT_H_DEFINED_        
