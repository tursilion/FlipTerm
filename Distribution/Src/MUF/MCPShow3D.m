( MCPShow3D.muf )
( 11 Aug 02 - by Tursi for FlipSide Muck )
( This should be the last _arrive prop on a room )
( Updated 29 Mar 03 for the nowrap flag )

$include $lib/mcp

( Set this to your subfolder for the 3D image cache )
( remember the trailing slashes                     )
$def MUCKFOLDER "flipside\\"
$def URL "http://flipsidemuck.org/images/"

: main
  (should always check first that we're negotiated )
  "dns-com-tursisoft-3d" "10" "10" MCPIsSupported
  not if exit then

  me @ location "/_Room3d/file" getpropstr
  dup "" strcmp if
    "dns-com-tursisoft-3d-newimage %S Filename:"  MUCKFOLDER strcat swap strcat
    me @ location "_Room3d/NoWrap" getpropstr
	"1" strcmp not if
	  " NoWrap:1" strcat
	then
    me @ location "_Room3d/Tag" getpropstr
    dup "" strcmp if
      "_" " " subst
      " Tag:" swap strcat strcat
    else 
      pop
    then
    " URL:" strcat
    URL strcat
    me @ location "_Room3d/Updated" getpropstr 
	dup "" strcmp if
	  " TimeStamp:" swap strcat strcat
	else
	  pop
	then
    MCPSendLine
  else
    pop
	( clear the image )
	"dns-com-tursisoft-3d-newimage %S Filename:" MCPSendLine
  then
;
