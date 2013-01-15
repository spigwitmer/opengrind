solution "OpenGrind"
configurations { "Debug", "Release" }

project "GLXW"
targetname "glxw"
targetdir "bin"
kind "StaticLib"
language "C"
includedirs { "extern/glxw/include" }
files { "extern/glxw/src/glxw.c" }
links { "GL" }

; -- oh lua, you so silly
(function()
	project "OpenGrind"
	targetname "opengrind"
	targetdir  "bin"
	kind "WindowedApp"
	language "C++"
	files { "src/**.cpp" }
	defines { "DEBUG" }
	libdirs { "extern/glfw3/src" }
	links { "glfw3", "Xrandr", "X11", "lua5.1", "GLXW", "GL", "openal", "physfs" }
	includedirs { "src", "/usr/include/lua5.1", "extern/glfw3/include", "extern/glxw/include" }

	configuration { "linux", "gmake" }
	buildoptions { "-std=c++0x", "-Wall", "-pedantic" }
end)()
