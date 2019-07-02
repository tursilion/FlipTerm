( lib-MCP - Created by Tursi for FlipSide Muck and FlipTerm )
( 8 June 02 )
( This library implements the basics of MCP 2.1 and can be  )
( used to provide MCP functionality in mucks that do not    )
( natively support it. It supports a subset of MCP 2.1,     )
( including negotiation, with the primary limitation that   )
( it can not run until after the user has logged in, as it  )
( stores it's runtime data on the user.                     )
( Run it with an argument of '#setup' to set up the system. )
( To my knowledge, only Trebuchet and FlipTerm support MCP  )
( at this time.                                             )

( In practice, this is a little less useful because most    )
( Mucks won't permit the '#' to begin an action. I've made  )
( an exception in predicates.c [FuzzBall] for names that    )
( start with #$# explicitly, checking the first character   )
( AFTER that sequence, and that's working well enough.      )

( 11 Aug 02 - Added MCPIsSupported, fix for MCPGetArg       )
( Added support for the "AfterEnd" propdir to list progs    )
( to run after the client sends negotiate-end               )

( 18 Aug 02 - Added user MCP switch to disable MCP          )

( 29 Mar 03 - Added user switch to debug and show the string)
(             /_MCPdebug                                    )

( -- Exported function -- )
( MCPSendLine - pass a string consisting of the MCP command )
( you wish to send, excluding the #$#, and using %S for the )
( session key. )
: MCPSendLine ( s -- )
  me @ "/@MCP/key" getpropstr "%S" subst
  "#$#" swap strcat 
  me @ "_MCPDebug" getpropstr "y" stringpfx if dup ">>" swap strcat .tell then
  .tell
;

( MCPGetArg - pass the source string and the name of the )
( argument you want - returns the argument as a string   )
: MCPGetArg ( s s -- s' )
  ":" strcat
  over over instring dup 0 = if pop pop pop "" exit then
  swap strlen + 1 -
  strcut swap pop strip
  dup " " instr dup 0 = if
    pop exit
  else
    strcut pop strip
  then
;

( MCPGetMinVer - returns the min version from the string )
( as an integer times 10 [so 2.1 would be 21]            )
: MCPGetMinVer ( s -- i )
  "min-version" MCPGetArg
  dup "" strcmp not if pop 0 exit then
  "" "." subst atoi
;

( MCPGetMaxVer - returns the max version from the string )
( as an integer times 10 [so 2.1 would be 21]            )
: MCPGetMaxVer ( s -- i )
  "max-version" MCPGetArg
  dup "" strcmp not if pop 0 exit then
  "" "." subst atoi
;

( MCPIsSupported - returns true if the package version is )
( valid and negotiated. Send versions times 10 [1.0=10]   )
( First string is the package name, then min, then max ver)
: MCPIsSupported ( s s s -- f )
  me @ "_MCPEnable" getpropstr 1 strcut pop tolower "n" strcmp not if
    pop pop pop 0 exit
  then
  " max-version:" swap strcat strcat
  " min-version:" swap strcat strcat
  "Package:" swap strcat
  dup "package" MCPGetArg
  dup "" strcmp not if pop pop 0 exit then  ( no package )
  "/@MCP/Packages/" swap strcat me @ swap getpropstr
  dup "" strcmp not if pop pop 0 exit then  ( not registered )
  ( stack: inputstr, registeredstr )
  ( compare versions for overlap )
  dup MCPGetMinVer
  3 pick MCPGetMaxVer
  > if pop pop 0 exit then
  dup MCPGetMaxVer
  3 pick MCPGetMinVer
  < if pop pop 0 exit then
  pop pop 1
;

( -- Internal functions -- )
( doSetup - this function creates the actions, sets up the )
( props, and registers the library for use                 )
: doSetup
  pop

  me @ owner "W" flag? not if "+ Nice try." .tell exit then

  "* Setting negotiate actions..." .tell
  #0 "#$#mcp-negotiate-can" newexit
  prog setlink
  #0 "#$#mcp-negotiate-end" newexit
  prog setlink
  #0 "#$#mcp" newexit
  prog setlink

  "* Setting up library..." .tell
  prog "/_defs/MCPSendLine" "\"$lib/mcp\" match \"MCPSendLine\" call" 0 addprop
  prog "/_defs/MCPGetArg" "\"$lib/mcp\" match \"MCPGetArg\" call" 0 addprop
  prog "/_defs/MCPMinVer" "\"$lib/mcp\" match \"MCPMinVer\" call" 0 addprop
  prog "/_defs/MCPMaxVer" "\"$lib/mcp\" match \"MCPMaxVer\" call" 0 addprop
  prog "/_defs/MCPIsSupported" "\"$lib/mcp\" match \"MCPIsSupported\" call" 0 addprop
  #0 "/_reg/lib/mcp" prog setprop

  "* Setting _connect on #0..." .tell
  #0 "/_Connect/00_MCP" prog setprop

  "* Done *" .tell
;

( doList - this lists the currently set packages on the program )
: doList
  pop

  me @ owner "W" flag? not if "+ Nice try." .tell exit then

  "/Packages/"
  begin
    prog swap nextprop dup "" strcmp while
	dup prog swap getpropstr .tell
  repeat
;

( doAdd - adds a new package to the known list )
( expects to get an argument of 'package minver maxver' )
: doAdd ( s -- )

  me @ owner "W" flag? not if "+ Nice try." .tell exit then

" " explode
4 = not if
  "* Please use: mcp #add <packagename> <minver> <maxver>" .tell
  exit
then
pop
dup "/Packages/" swap strcat -4 rotate                 ( package max min package )
"package: " swap strcat
" min-version: " strcat swap strcat
" max-version: " strcat swap strcat  ( package string )
prog -3 rotate 0 addprop
"* Added." .tell
;

( doRemove - removes a package from the list )
( expects to get the package name as an argument )
: doRemove ( s -- )

  me @ owner "W" flag? not if "+ Nice try." .tell exit then

  " " explode 2 = not if
    "* Please use: mcp #remove <packagename>" .tell
	exit
  then pop
  "/Packages/" swap strcat
  dup prog swap getpropstr "" strcmp not if
    "* Package was not found - try #list." .tell
	exit
  then
  prog swap remove_prop
  "* Removed." .tell
;

( doHelp - summarize the commands )
: doHelp
  pop

  me @ owner "W" flag? not if "+ Nice try." .tell exit then

  "mcp #setup - Setup the MCP lib and negotiation actions" .tell
  "mcp #list  - list the packages configured on the program" .tell
  "mcp #add   - add a new package: <packagename> <minver> <maxver>" .tell
  "mcp #remove- remove a package: <packagename>" .tell
  "mcp #help  - this text!" .tell
  exit
;

( doConnect - prepare a new connection - clear the props )
: doConnect
  pop

  trig #-1 dbcmp not if "+ Nice try." .tell exit then

  ( clear known packages and key )
  me @ "/@MCP" remove_prop

  ( check if disabled )
  me @ "_MCPEnable" getpropstr 1 strcut pop tolower "n" strcmp not if
    exit
  then

  ( tell the client we speak MCP )
  "#$#mcp version: 2.1 to: 2.1" .tell  ( fixed for now )
  ( that's all - we can't do more till we get a key )
  exit
;

( doMCP - this is expected to be an MCP authentication key )
: doMCP ( s -- )

  "" "\"" subst   ( remove quotes )
  "" "." subst    ( remove . )

  dup "authentication-key" MCPGetArg
  dup "" strcmp not if pop exit then
  
  me @ "/@MCP/key" getpropstr "" strcmp if
    "* Duplicate MCP session key attempt" .tell
	pop
	exit
  then
  me @ "/@MCP/key" rot 0 addprop

  dup "version" MCPGetArg
  dup "" strcmp not if 
    "* MCP Version ID not found." .tell
    me @ "/@MCP" remove_prop
    pop exit 
  then
  atoi 21 > if
    "* MCP Version 2.1 not supported by client" .tell
    me @ "/@MCP" remove_prop
	pop exit
  then

  dup "to" MCPGetArg
  dup "" strcmp if ( optional )
    atoi 21 <= not if 
	  "* MCP Version 2.1 not supported by client" .tell
      me @ "/@MCP" remove_prop
	  pop exit
	then
  then

  ( so now we spew out everything WE support )
  ( This is always first - though we don't check the return from the client [todo?] )  
  "mcp-negotiate-can %S package: mcp-negotiate min-version: 2.0 max-version: 2.0" MCPSendLine

  "/Packages/"
  begin
    prog swap nextprop dup "" strcmp while
	dup prog swap getpropstr "mcp-negotiate-can %S " swap strcat MCPSendLine
  repeat
  "mcp-negotiate-end %S" MCPSendLine
;

( CheckSupport - helper function to see if the package is registered )
: CheckSupport ( s -- f )
  dup "package" MCPGetArg
  dup "" strcmp not if pop pop 0 exit then  ( no package )
  "/Packages/" swap strcat prog swap getpropstr
  dup "" strcmp not if pop pop 0 exit then  ( not registered )
  ( stack: inputstr, registeredstr )
  ( compare versions for overlap )
  dup MCPGetMinVer
  3 pick MCPGetMaxVer
  > if pop pop 0 exit then
  dup MCPGetMaxVer
  3 pick MCPGetMinVer
  < if pop pop 0 exit then
  pop pop 1
;

( doNegotiate - handle an MCP-Negotiate-Can by saving it's data )
( Ideally, this function should parse the data and store it in  )
( a useful manner. TODO. )
: doNegotiate ( s -- )
  me @ "/@MCP/Done" getpropstr "" strcmp if
    "* Received MCP-Negotiate-Can after MCP-Negotiate-End!" .tell
	exit
  then
  
  "" "\"" subst
  "" "." subst
  strip
  dup "package:" instring dup 0 = if pop exit then
  1 - strcut swap pop
  dup "package" MCPGetArg
  dup "" strcmp not if pop pop exit then
  ( stack: fullstring, packagename )
  ( see if the package is supported )
  over CheckSupport not if
    pop pop exit
  then

  ( It is, so save it )
  "/@MCP/Packages/" swap strcat swap
  me @ -3 rotate 0 addprop
;

( doNegotiateEnd - flag that the client is done negotiation )
( Then run any programs that have been waiting in our       )
( /AfterEnd/ dir                                            )
: doNegotiateEnd
  pop
  me @ "/@MCP/Done" "Y" 0 addprop  (any character will do)

  "/AfterEnd/"

  begin prog swap nextprop dup "" strcmp while
    dup prog swap getpropstr atoi dbref dup program? if
	  call
	else
	  pop
	then
  repeat
;

( Main function - this one handles the input cases )
: main ( s -- )
dup "#setup" stringcmp not if
  doSetup exit
then

dup "#list" stringcmp not if
  doList exit
then

dup "#add " instring if
  doAdd exit
then

dup "#remove " instring if
  doRemove exit
then

dup "#help" stringcmp not if
  doHelp exit
then

dup "Connect" strcmp not if
  doConnect exit
then

Command @ "#$#mcp" stringcmp not if
  doMCP exit
then

Command @ "#$#mcp-negotiate-can" stringcmp not if
  doNegotiate exit
then

Command @ "#$#mcp-negotiate-end" stringcmp not if
  doNegotiateEnd exit
then

( fail silently )
pop
;

public MCPSendLine
public MCPGetArg
public MCPGetMinVer
public MCPGetMaxVer
public MCPIsSupported
