/////////////////////////////////////////////////////////////////////////
//
//

#include "stdafx.h"
#include "textfrmt.h"
#define END_OF_LINE '\n'

int MyAnsiNext(LPCSTR pStr)
{
	if(!*pStr)
	{
		return 0;
	}
	LPCSTR pAbort = pStr;
	if(*pStr!=27)
		return 1;
	pStr++;
	if(strchr("c78ABCDFGHIJKMZ=>12<",*pStr))
		return 2;
	if(*pStr=='#' && *(pStr+1)>'2' && *(pStr+1)<'9')
		return 3;
	if( (*pStr=='(' || *pStr==')')&& strchr("AB012",*(pStr+1)))
		return 3;
	if(*pStr!='[')
		goto abort;
	pStr++;
	if(strchr("KJm",*pStr))
		return 3;
	do{
		if(*pStr==';')
			pStr++; // move off of '[' or ';'

		// get first digit
		if(isdigit(*pStr))
			pStr++;
		else
			goto abort;

		// get second digit
		if(isdigit(*pStr))
			pStr++;
	}
	while(*pStr==';');
	if(strchr("rmABCDRnHfsuJkKycqg",*pStr))
		return (pStr-pAbort)+1;
abort:
	return 1;
	
}

int AnsiChars(LPCSTR str, int columns /* = 80 */)
{
	int charat=0;
	LPCSTR strstart=str;
	while(*str && *str!=END_OF_LINE && charat < columns)
	{
		int num=MyAnsiNext(str);
		str+= num;
		if(num==1)
			charat++;
	}

	// kill the leading space off of the next line.
	if(*str==' ')
		str++;
	return (str-strstart);
}

int Ansimemcpy(LPSTR pDest,LPCSTR pSrc,int iFirstChar,int iLength)
{
	if(iFirstChar)
		pSrc+=AnsiChars(pSrc,iFirstChar);
	int charat=0;
	while(*pSrc && charat < iLength)
	{
		int num=MyAnsiNext(pSrc);
		if(num==1)
		{
			*pDest=*pSrc;
			charat++;
			pDest++;
		}
		pSrc+= num;
	}
	*pDest=0;
	return charat;
}
