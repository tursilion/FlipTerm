LPSTR skipspace(LPSTR);
LPCSTR skipspace(LPCSTR str);

LPSTR skiptoken(LPSTR);
LPCSTR skiptoken(LPCSTR str);

int tokenlen(LPCSTR str);
BOOL GetParam(CString &instring,CString &outstr,int paramnum);
StripUserName(LPCSTR instr, CString &username);
LPCSTR GetArgPtr(LPCSTR instr, int argnum);
LPCSTR StripCR(CString &str);
LPSTR StripCR(LPSTR str);
LPCSTR GetTime();