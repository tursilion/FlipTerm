#include "stdafx.h"
#include "lowlev.h"


LPSTR skipspace(LPSTR str)
{
	while(*str && *str==32)
		str++;
	return str;
}

LPCSTR skipspace(LPCSTR str)
{
	while(*str && *str==32)
		str++;
	return str;
}

LPSTR skiptoken(LPSTR str)
{
	while(*str && *str!=32 && *str!='\t')
		str++;
	return str;
}

LPCSTR skiptoken(LPCSTR str)
{
	while(*str && *str!=32 && *str!='\t')
		str++;
	return str;
}

int tokenlen(LPCSTR str)
{
	int len=0;
	while(*str && *str!=' ' && *str!='\r')
	{
		len++;
		str++;
	}
	return len;
}

BOOL GetParam(CString &instring,CString &outstr,int paramnum)
{
	outstr="";
	LPCSTR instr = instring;
	
	if(!paramnum || instring=="")
		return FALSE;
	
	paramnum--;
	
	while(*instr && paramnum)
	{
		if(*instr==' ')
			paramnum--;
		instr++;
	}
	if(*instr==':')
		instr++;
	char retbuf[1024];
	LPSTR pBuf=retbuf;
	while(*instr && *instr!=' ' && *instr!='\r' && *instr!='\n')
	{
		*pBuf=*instr;
		instr++;
		pBuf++;
	}
	*pBuf=0;
	outstr=retbuf;
	return TRUE;
}

StripUserName(LPCSTR instr, CString &username)
{
	username="";
	while(*instr && *instr!='!')
	{
		username+=*instr;
		instr++;
	}
	return TRUE;
}

LPCSTR GetArgPtr(LPCSTR instr, int paramnum)
{
	if(paramnum ==0 || paramnum==1)
		return instr;
	paramnum--;
	
	while(*instr && paramnum)
	{
		if(*instr==' ')
			paramnum--;
		instr++;
	}
	if(*instr==':')
		instr++;
	return instr;
}

LPSTR StripCR(LPSTR str)
{
	char *at=str;
	do
	{
		if(*at=='\r' || *at == '\n')
		{
			strcpy(at,at+1);
		}
	}
	while(*at);
	
	return at;
}

LPCSTR StripCR(CString &str)
{
	LPCSTR at = str;
	CString retstr;
	while(*at)
	{
		if(*at!='\r' && *at != '\n')
		{
			retstr+=*at;
		}
		at++;
	}
	
	str=retstr;
	return str;
}

LPCSTR GetTime()
{
	static char str[10];
    char am_pm[] = "AM";
	
	time_t ttime;
	time(&ttime);
	
	struct tm *newtime;
	newtime=localtime(&ttime);

    if( newtime->tm_hour > 12 )             /* Set up extension. */
            strcpy( am_pm, "PM" );
    if( newtime->tm_hour > 12 )             /* Convert from 24-hour */
            newtime->tm_hour -= 12;         /*   to 12-hour clock.  */
    if( newtime->tm_hour == 0 )             /*Set hour to 12 if midnight. */
            newtime->tm_hour = 12;
	
	sprintf(str,"%2.2d:%2.2d %s",newtime->tm_hour,newtime->tm_min,am_pm);
	return str;
}   
