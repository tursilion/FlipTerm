This depot is not guaranteed to build at any given time - it's my bleeding edge repository.

FlipTerm MUCK Client v 1.12.0
=============================

![Screenshot](https://github.com/tursilion/flipterm/raw/master/dist/flipterm.jpg)

I wrote this when GMUD stopped working on my Windows 2000 machine, and used it and updated it for over a decade. FlipTerm has many features and extensions over the original GMud, as requested by a great crew of beta testers, but the main goal is it's stable.

Download the latest release installer here: [https://github.com/tursilion/flipterm/raw/master/dist/InstallFlipTerm.exe](https://github.com/tursilion/flipterm/raw/master/dist/InstallFlipTerm.exe)

1.12.0 removes a few old features that were never fully used. In particular, MCP support and plugins were removed.

Note if you install the new 1.12.0 overtop of an old install, it will leave some tailings behind (old source, old docs, old plugins). You can either delete these manually, or uninstall BEFORE you install the new one. To preserve your settings:

- Use File->Export Settings to save a copy of your settings. Check the exported reg file in Notepad to make sure it's not empty!
- run the uninstaller - it will clean up everything!
- install the new version
- Double-click the reg file and let Windows reimport your settings

If you just install overtop of the old version, it won't hurt anything! So it's up to you.

The new FlipTerm uses a much newer version of MFC. They changed lots in the docking code, so you will have to redock your windows, but everything else SHOULD come across. The basic improvements are better handling of high-DPI monitors when docking, the newer toolbar interface (just better tooltips and docking modes really), and proper wrap-to-window even if you resize the window. ;)

It ALSO means that the oldest supported version is Windows XP. If you need older versions, grab the "old98" version instead, but I can't support it for any fixes. [https://github.com/tursilion/flipterm/raw/master/dist/InstallFlipTerm_old98.exe](https://github.com/tursilion/flipterm/raw/master/dist/InstallFlipTerm_old98.exe)


Want to donate?
---------------

- Click here for [Ko-Fi](https://ko-fi.com/tursilion)...
- Alternately, there's my [Amazon Wishlist](http://www.amazon.com/gp/registry/2AFCOAM5DD1L6/ref=cm_aya_wl/103-5991996-6483001)

