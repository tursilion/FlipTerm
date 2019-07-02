#include "StdAFX.h"
#include "Script.h"
#include "GMud32VW.h"
#include "World.h"

#define SCRIPT_START_STRING "<SCRIPT>"

enum FUNC_IDS{F_NULL=0,F_IF,F_TESTEQUAL,F_SETEQUAL};
enum tokentypes{invalid=0,beginscope=1,endscope=2, function, uservariable};


struct token_struct{
  LPCSTR szToken;
  int iTokenID;
}
g_Functions[]=
{
  {"NULL",F_NULL},
  {"if",F_IF},
  {"==",F_TESTEQUAL},
  {"=",F_SETEQUAL},
  {"NULL",F_NULL},
};


void DescendScope(LPCSTR szScript,int &iCharIndex);
CString GetNextStatement(LPCSTR szScript,int &iCharIndex);
bool FindFunction(CString sStatement,CString &sFunction,CString &sArguments);
CString GetNextToken(CString &sInput);
tokentypes GetTokenType(CString sToken);
void SkipScope(LPCSTR szScript,int &iCharIndex);
bool DoIf(CString sStatement);
int GetFuncID(CString sFunc);
CString GetStringValue(CString sStatement);

CMapStringToString  *g_pUserVariables;
CMudView            *g_pView;
CWorld              *g_pWorld;

void RunScript(CString sScript,CMudView &rView,CWorld &rWorld)
{
  sScript.TrimLeft();
  if(strnicmp(SCRIPT_START_STRING,sScript,lstrlen(SCRIPT_START_STRING))!=0)
  {
    rView.OnUserInput(0,(long)(LPCSTR)sScript);
    return;
  }
  try
  {
    g_pUserVariables = rWorld.GetUserVariables();
    g_pView = &rView;
    g_pWorld = &rWorld;
    sScript=sScript.Mid(lstrlen(SCRIPT_START_STRING));
    sScript.TrimLeft();
    sScript.TrimRight();
    int iIndex= 0;
    TRACE("\n%%%%%% Started Parsing...\n");
    DescendScope(sScript,iIndex);
    TRACE("%%%%%% Completed Parsing.\n");
  }
  catch(CString sError)
  {
    TRACE("%%%%%% Error parsing script, error follows\n");
    TRACE("%s\n",(LPCSTR)sError);
    TRACE("%%%%%% End of Errors\n");
  }
  return;
}


void DescendScope(LPCSTR szScript,int &iCharIndex)
{
  TRACE("%%%%%% Descending\n");

  do
  {
    CString sStatement=GetNextStatement(szScript,iCharIndex);
    TRACE("%%%%%% Parsing %s\n",(LPCSTR)sStatement);
    if(sStatement == '{')
      DescendScope(szScript,iCharIndex);
    else if(sStatement == '}')
      break;
    else
    {
      CString sToken=GetNextToken(sStatement);
      tokentypes type = GetTokenType(sToken);
      if(type == uservariable) // assignment
      {
        CString sOp = GetNextToken(sStatement);
        int iOpType = GetTokenType(sOp);
        if(iOpType!=function)
          throw "function expected! (assignment )";
        int iOp = GetFuncID(sOp);
        if(iOp == F_SETEQUAL)
        {
          CString sValue = GetStringValue(sStatement);
          g_pUserVariables->SetAt(sToken,sValue);
        }
        else
          throw "assignment expected!";
      }
      else if(type == function)
      {
        int function = GetFuncID(sToken);
        switch(function)
        {
          case F_IF:
            if(DoIf(sStatement)== false)
            {
              SkipScope(szScript,iCharIndex);
            }
            break;
          default:
          case F_NULL:
            CString s;
            s.Format("%%%%%% During script, error no function: %s\n",sToken);
            throw s;
        }
      }
      else
      {
        CString s;
        s.Format("%%%%%% During script, Dont know how to handle: %s\n",sToken);
        throw s;
      }
    }
  }while(szScript[iCharIndex]);

  TRACE("%%%%%% Ascending\n");
  return;
}

CString GetNextToken(CString &sInput)
{
  const char *token_delims=" <+>=(){},.[]|*&^%$#@!~";
  for(int loop=0;loop<sInput.GetLength();loop++)
  {
    char ch=sInput[loop];
    if(strchr(token_delims,ch))
    {
      if(loop==0)
        loop++;
      CString sRet = sInput.Left(loop);
      sInput=sInput.Mid(loop);
      return sRet;
    }
  }
  return "";
}
/*
bool FindFunction(CString sStatement,CString &Function,CString &sArguments)
{
  sStatement.TrimLeft();
  for(int loop=0;loop<sStatement.GetLength();loop++)
  {
    if(sStatement[loop]=='(')
      return false; //[*]
  }
  return false;
}
*/
const char *szStatementEnders=";{}";

CString GetNextStatement(LPCSTR szScript,int &iCharIndex)
{
  while(isspace(szScript[iCharIndex]))
    iCharIndex++;
 
  int iStart = iCharIndex;
  CString sStatement;
  while(  szScript[iCharIndex] && !strchr(szStatementEnders,szScript[iCharIndex]))
  {
    sStatement+=szScript[iCharIndex];
    iCharIndex++;
  }
  if(!szScript[iCharIndex])
    throw CString("Error: EOF Found, expected \';\'");

  if(!sStatement.GetLength())
  {
    sStatement=szScript[iCharIndex];
    iCharIndex++;
  }
  else if(szScript[iCharIndex] != '{')
    iCharIndex++;
  sStatement.TrimRight();
  return sStatement;
}

int GetFuncID(CString sFunc)
{
  if(sFunc.IsEmpty())
    return F_NULL;
  token_struct *pToken=NULL;
  int iIndex=1;
  while(1)
  {
    pToken=&g_Functions[iIndex];
    if(pToken->iTokenID==F_NULL)
      break;
    if(sFunc.CompareNoCase(pToken->szToken)==0)
      return pToken->iTokenID;
    iIndex++;
  }
  return F_NULL;
}

tokentypes GetTokenType(CString sToken)
{
  if(GetFuncID(sToken)!=F_NULL)
    return function;

  CString sDummy;
  if(g_pUserVariables->Lookup(sToken,sDummy))
    return uservariable;

  return uservariable;
}

void SkipScope(LPCSTR szScript,int &iCharIndex)
{
  int iBrackets=0;
  // skip leading space;
  while(szScript[iCharIndex] && isspace(szScript[iCharIndex]))
    iCharIndex++;
  if(szScript[iCharIndex]!='{') // single line if
  {
    while(szScript[iCharIndex] && szScript[iCharIndex]!=';')
      iCharIndex++;
  }
  else // bracketed if
  {
    for(iCharIndex;szScript[iCharIndex]!=NULL;iCharIndex++)
    {
      if(szScript[iCharIndex]=='{')
        iBrackets++;
      else if (szScript[iCharIndex] == '}')
        iBrackets--;
    }
  }
}

bool DoIf(CString sStatement)
{
  TRACE("Entering DoIf with statemend of %s\n",(LPCSTR)sStatement);
  if(sStatement[0]!='(')
    throw "expected '('";
  if(sStatement[sStatement.GetLength()-1]!=')')
    throw "expected ')'";
  sStatement=sStatement.Mid(1,sStatement.GetLength()-2);
  sStatement.TrimLeft();
  sStatement.TrimRight();
  if(GetNextToken(sStatement)=="") // testing for existance of var.
  {
    CString sDummy;
    if(g_pUserVariables->Lookup(sStatement,sDummy))
      return true;
    else
      return false;
  }
  return false;
}

CString GetStringValue(CString sStatement)
{
  sStatement.TrimLeft();

  CString sRet;
  bool bInQuote=false;
  for(int loop=0;loop<sStatement.GetLength();loop++)
  {
    if(sStatement[loop] == 34) //'/"')
    {
      if(bInQuote)
        break;
      else
        bInQuote=true;
    }
    else
      sRet+=sStatement[loop];
  }
  return sRet;
}