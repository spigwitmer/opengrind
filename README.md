About
=====
I felt like a bulldozer

Trying to catch a butterfly

Requirements
============
* cmake (for glfw)
* python 2.7+ (for GLXW)
* xorg-dev (Xrandr, X11) (linux only)
* libgl1-mesa-dev (GL) (linux only)
* libopenal-dev (openal)
* libphysfs-dev (physfs)
* Awesomium 1.6.x (http://awesomium.com/download/) - Note: free for non-commercial and indie use (<$100k/yr)

If you'd like to use an IDE instead of makefiles, just generate the files as needed with premake (e.g. <code>$ premake4 codelite</code>). Premake 4.4 is required due to use of os.is64bit().

Tested and working on Linux (Ubuntu 12.04, 12.10) and Windows (W7, MSVC 2008). Roadmap can be found [here](https://github.com/shakesoda/opengrind/wiki/Roadmap).
