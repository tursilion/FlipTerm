( Set3D.MUF - 11 Aug 02 by Tursi for FlipSide )
( Updated 29 Mar 03 to add 'nowrap' flag      )
( Updated  2 Nov 03 to strip spaces before writing props )

: dohelp
  "* Usage: Set3D <filename.jpg> <nowrap> <World name>" .tell 
  "         Set3D #renew (To indicate the picture for this room was updated)" .tell
  "  For 'nowrap', specify '1' to disable wrapping the image 360 degrees" .tell
;

: main ( s -- )
( The first argument is the filename. The rest is treated as the tag. Spaces are changed to _ )
me @ "W" flag? not if
me @ location owner me @ dbcmp not if "* Sorry, you don't own this room." .tell exit then
then
dup "" strcmp not if
  dohelp 
  exit then
dup 1 strcut pop "#" strcmp not if
  dup "#renew" strcmp not if
    me @ location "_Room3d/Updated" systime intostr 0 addprop
    "* Timestamp updated" .tell
    exit
  then
  dohelp
  exit
then
 
dup " " instr strcut
dup " " instr strcut

dup "" strcmp not if dohelp exit then
rot dup "" strcmp not if dohelp exit then
rot dup "" strcmp not if dohelp exit then
rot
 
"_" " " subst
me @ location swap "_Room3d/Tag" swap strip 0 addprop
me @ location swap "_Room3d/NoWrap" swap strip 0 addprop
me @ location swap "_Room3d/File" swap strip 0 addprop
me @ location "_Room3d/Updated" systime intostr 0 addprop
 
"* Prop added." .tell
;
