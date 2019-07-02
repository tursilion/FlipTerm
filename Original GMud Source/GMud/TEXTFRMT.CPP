/////////////////////////////////////////////////////////////////////////
//
//

#include "stdafx.h"
#include "textfrmt.h"
#define END_OF_LINE '\n'

// charsinwidth returns the number of characters that will fit within a certain width.              
int Charsinwidth(CDC &mydc,LPCSTR strtocalc,int width )
{                       // width is the width in logical units of the maximum output width
    TEXTMETRIC metric; 	// structure used to get font information
    LPCSTR stemp;		// temp string
    SIZE ssize;			

	if(!*strtocalc)
		return FALSE;    
	mydc.GetTextMetrics( &metric);	    			// get the font information
	ssize.cx=0;
	
	if(*strtocalc==END_OF_LINE)
		return 1;
	stemp=strtocalc;								// Start at first word
	if(!*stemp)
		return FALSE;
	
	// first make an educated guess... 
	{
		int guess = width/metric.tmAveCharWidth; // how many chars we think are in here...
		int loop;
		for(loop=0;loop<guess;loop++)
		{
			if(!*(stemp+loop) || *(stemp+loop)==END_OF_LINE)
				break;
		}
		stemp+=loop;
	}
	
	// keep advancing word by word untill we're at least 1 word too long
	// we could already be much further if the educated guess was way off...
	while(ssize.cx<width && *stemp && *stemp!=END_OF_LINE)
	{ 		
		// while we're shorter than the width that they want, but not at end of string or a C/R
		// keep going to the next word
		while(*stemp!=32 && *stemp && *stemp!=END_OF_LINE)// advance a char at a time
			stemp++;                                  // up to next word break
		ssize=mydc.GetTabbedTextExtent(strtocalc,stemp-strtocalc,0,0);	// now, find out how wide this string is
		if(!*stemp || *stemp==END_OF_LINE)
			break;
		stemp++;// if we're short, we want to skip over this space for next loop iteration :)
	}
	
	// now we're too long, so back up a word at a time untill we are right.
	while(ssize.cx>width && stemp>strtocalc)		// don't back up to before where we started!
	{
	    stemp--;// decrement one more time so next time we aren't at the 
	    		// same stupid space. we can over-run the beginning of the string this
	    		// way, but that won't matter cause we'll catch it in the next 
	    		// section of code.
		while(*stemp!=32 && *stemp!='\t' && stemp>strtocalc)
			stemp--; 								// back up untill we hit a space
		ssize=mydc.GetTabbedTextExtent(strtocalc,stemp-strtocalc,0,0) ;
	}
	// is there only one word on the line?
	if(stemp<=strtocalc)
	{
		stemp=strtocalc+1;	// if so, advance one character at a time.
		while(*stemp!=END_OF_LINE && *stemp)  	// to do this just advance up one character at a time untill
		{								// we dont fit within the margin... then back up one char, and
										// return that as the width.
										// slow and painfull, but there's no other way
			ssize=mydc.GetTabbedTextExtent(strtocalc,stemp-strtocalc,0,0);
			if(ssize.cx>width)
			{
				stemp--;
				break;
			}
			stemp++;
		}
	}
	// add trailing spaces to this thing.
	while(*stemp==' ')
	{
		stemp++;
	}

	if(stemp==strtocalc)    // did we not get rid of any characters?
		stemp++;			// we have to get rid of at least one character in this routine.

	return(stemp-strtocalc);	// return number of characters that will fit
}

int CharsInColumns(LPCSTR str, BOOL bWordWrap,int columns)
{
	int charat=0;
	LPCSTR strstart=str;
	while(*str && *str!=END_OF_LINE && charat < columns)
	{
		str++;
		charat++;
	}

	if(bWordWrap && charat>79) // wrapping by words?
	{
		LPCSTR strstartwrap=str;
		while(*str!=' ' && *str+1!=' ' && str>strstart && *str!=END_OF_LINE && *str)
			str--;
		if(str<=strstart)
			str=strstartwrap;
	}

	// kill the leading space off of the next line.
	if(*str==' ')
		str++;
	return (str-strstart);
}

int MyAnsiNext(LPCSTR pStr)
{
	if(!*pStr)
	{
		TRACE("\nError, calling MyAnsiNext with pointer to null.");
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
	TRACE("\nInvalid ansi sequence.");
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
