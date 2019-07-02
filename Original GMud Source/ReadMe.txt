GMud Source code

PLEASE DO NOT EMAIL ME ASKING QUESTIONS ABOUT THIS CODE. I Really dont have time. Email me only with offers of cash :)

Please be sure you read the following sections: Backwards compatibility, Crappy code, and Re-Distribution.

I quit coding on GMud for 4 major reasons:
-----------------------------------------
1) I quit mudding.
2) I was WAY to busy in personal life and at work.
3) I lost the source code. I had the source code on 2 separate hard drives, and lost both of them within a few days. Horrid. I also had a backup on floppy but alas this was only version 1.7b. (see Backwards Compatibility below) I then put the source code in version control on a separate server in a different room in the house. guesss what? I lost it AGAIN!!! 2 hard drives on separate machines in a week! I managed to recover one of the hard drives last week (after about a year of it sitting in my closet) so I figured I better release the code before the demons strike again.
4) Because Microsoft sucked butt and decided to make the new compiler (MSVC 4.2) not generate Win32s binaries, and didnt mention this except deep within the bowels of thier documentation. I didnt know this untill after I had installed 4.2, and had long since lost my copy of MSVC 4.1, and didnt wanna abandon all my faithfull users that had registered and ran on Windows 3.1(1)

Speaking of registered users, I had like 15 of you :) Writing shareware code for a living sux. At the peak I was fielding 100 emails a day, and only 15 registered... that's really sad. It didnt bother me so much except for all the people who said "Add this 1 feature and I'll register"... after the first 20 or 30 or so of them that weren't good for thier word, I just started to ignore them.


Backwards Compatibility
-----------------------
This source code is based off of version 1.7b (see 3) above), I recovered most of the features and bugfixes that were added into 1.9b (and a few more). But I didnt have the file format specs for 1.9b so this code will not load any saved worldlist files from 1.9b! feel free to hack out the format if you can, but I seriously doubt it can be done.

Crappy Code
-----------
This is the worst code I have ever written in my life. Bar none. The reasons? 1) this was based off of a 16bit Windows API (read non-mfc) IRC client that I never released or intended to release, I just wrote it so I could be on IRC in windows back in like 1990.  2) I'd be mudding and be like OOOH I could kick ass if I had feature X... so I'd flip over to the IDE and start jamming real quick, hacking anything that would work together to make the feature work, then flip back and run it till it crashed, stop and fix the bug, and move on to mudding untill the next feature crisis arrived... so the code is full of obviously frenzied coding (take a look at the Function-Key code if you dont believe me).

Re-Distribution
---------------
You can re-distribute the source so long as I'm given credit as the original author, and it's mentioned that it's based off of G-Mud source code, and it's not called G-Mud.
You can re-distribute a binary based off the source so long as it's for free, and I'm given credit as the original author, and it's mentioned that it's based off of G-Mud source code, and it's not called G-Mud, and the source code is included or available for download from the main binary distribution site.

If you wish to do something other than the above (for profit, etc) get ahold of me... I could work something out very cheap I think, and free in alot of cases, just be sure to ask me first okay? (entropy@icubed.com)

If you rip off the display window code, paste it into your application, and dont give me credit, or try to sell that application as shareware, commercial, or whatever, I'll rip your nuts off. Really. I know of several oddities that it would be no prob to prove this is my code. I'm serious about this.

Every single line of this code is mine, NONE of it is ripped off. It really pissed me off to see my main competitor obviously totally rip off the source code from tin-tin, and not give one bit of credit to the original author. This sucks. This same asshole promised to give the spec for the format of his Client-To-Client chat protocol to me so I could make GMUD talk to his clients in exchange for the secrets of how I got my output window to scroll so fast... I gave him the secrets and then the asshole never came through on his end of the bargain. If this asshole in particular rips off my code I'll see him in court. In fact he's the whole reason I'm being so anal about re-distribution. We developers should all be working together and sharing code to make all our clients better, and patting eachother on the back in the progress, not ripping eachother off, and ditching out on obligations.

Notes about the code
--------------------
PLEASE DO NOT EMAIL ME ASKING QUESTIONS ABOUT THIS CODE. I Really dont have time. Email me only with offers of cash :)

MSVC 4.1 to 5.0 required, MFC based.
All windows / connections share a single thread. It uses the window messages from the WsaAsync calls to do this. I wrote it this way cause originally it was a 16bit app.
I try to model my coding style after MFC's, so if you're familiar with thiers you shouldnt have too much trouble.

There was Scripting code added at the last minute, it's not complete but it works somewhat and the code looks SOOOO horrible.

If you're running on a non-ansi mud you could rip out the ansi stuff and make it like 5x faster (assuming you're not already already scroll-speed bound).

There's a bug that alot of you have reported, unfortunatly I'm not running win32s or the obscure version of NT that it occurs on, it manifests itself in the following way: The output to the mud is like 5 or 6 [ENTER]s behind what they are typing. The bug is in the CSmartSocket class in the recieve or send, it has to do with the different way the operating systems behave when you tell it to notify you of incoming data / ready to send outgoing data and it already has incoming data / outgoing data in the queue, I think on the buggy OS it doesnt immediatly return with "data in the incoming queue" or whatever like it should... it's easy to fix tho, I fixed it right before I lost the source, but cant reproduce it now cause I no-longer have a win32s or buggy-NT-version box.


Thanks to Don K. for being so patient for this, I tried to hide the fact that I lost the source code the second time cause I didnt wanna look so silly. I sure am glad that I recovered it, now I dont feel so bad bout admitting this. :)

Thanks to Rita for the help with the documentation (winhelp version).

Love, Dan

