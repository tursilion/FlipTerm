------------------------------------------------
                    MUCKMATE
------------------------------------------------
1. Installation problems.
2. Using the program.
3. Adding MUCKmate support to your MUCK.
------------------------------------------------

1: Installation problems.

The only problem I can think of that you may encounter
is that after installing this program and you try to run it,
it may generate an error saying you are missing some files.

If you have this problem, it's because your system isn't up
to date with the Visual Basic runtime files. It's very easy
to remedy this, simply go to the folder where you installed
MUCKmate and look for the 'dependencies' folder. In there
you'll see a file called setup.exe. Double click it, and it
will install everything you're missing. 

If you're still having problems mail me at Karna_Roo@hotmail.com

------------------------------------------------

2: Using MUCKmate

Using MUCKmate is really easy. Double click the executable
file MUCKmate.exe or click the link in your Start Menu or
Desktop, then simply select 'Server->Start'.

Connect to a MUCK that is MUCKmate-compatable. Any MUCK
that has SOCKET prims can benefit from MUCKmate. If you're
on a ProtoMUCK or FB6 MUCK just ask the admins to add in
MUCKmate support.

To set a default image for yourself, an object, or a room
type this:

@set <object>=/_muckmate/image:<server> <filepath> <port>

<server> is the name of the HTTP server where the image
is stored, ie: www.flamehouse.org

<filepath> is the path where the image is stored on the
server, ie: /user/image.jpg NOTE: The image MUST be in
JPEG format.

<port> is the port that the server is running it's web
server on. This will almost ALWAYS be 80, but in some
cases (like with FlameHouse.org) the port may be different,
such as 81.

An example of a finished IMAGE string would be:

@set <object>=/_muckmate/image:www.flamehouse.org /images/karna.jpg 80

Make sure there is a space between the <server> and <filepath>.

Images may be in JPEG, GIF or PNG format. The built-in image
viewer does not support the viewing of PNG images, however.

To set background music do the same thing except replace
/_muckmate/image with: /_muckmate/music

Setting Flash and Video files is also the same, except
change the prop string to either:

'/_muckmate/flash' or '/_muckmate/video'.

Music files may be in MP3 or MIDI format.
Video files may be in either AVI or MPG format, but
only MPG files can stream.
Flash files must be in .SWF format.

The commands are:

@image <target>
@video <target>
@music <target>
@flash <target>

You can also set a homepage for yourself, an object
or a location that can be viewed with the @web command.

The syntax for this is different. The prop is '/_muckmate/web'
and all you need to do is set the URL as you normally would.

To see a website, type '@web <target>

------------------------------------------------

3: Adding MUCKmate support to your MUCK.

Follow these steps to add MUCKmate support
to your ProtoMUCK or FB6 MUCK.

All MUF programs needed can be found in the 'muf'
directory where you installed MUCKmate.

Step 1: Install lib-standard.txt 
Step 2: Install lib-muckmate.txt
Step 3: Install mm-basic.txt
Step 4: Type '@act @image;@music=#0' on your MUCK.
Step 5: Type '@link @image=mm-basic.karna' on your MUCK.

You're all set.

