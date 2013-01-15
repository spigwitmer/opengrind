solution "OpenGrind"
configurations { "Debug", "Release" }
;
-- libs

(function(self)
	project "GLXW"
	targetname "glxw"
	targetdir "bin"
	kind "StaticLib"
	language "C"
	includedirs { "extern/glxw/include" }
	files { "extern/glxw/src/glxw.c" }
	links { "GL" }
end)();

-- main project
(function(self)
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
	buildoptions { "-std=c++0x", "-Wall", "-pedantic", "-ggdb" }
end)();

-- tests
(function(self)
	project "OpenGrind-ShaderTest"
	targetname "shadertest"
	targetdir "bin"
	kind "WindowedApp"
	language "C++"
	files { "tests/shadertest.cpp" }
	libdirs { "extern/glfw3/src" }
	links { "glfw3", "Xrandr", "X11", "GLXW", "GL", "physfs" }
	includedirs { "src", "extern/glfw3/include", "extern/glxw/include" }

	configuration { "linux", "gmake" }
	buildoptions { "-std=c++0x", "-Wall", "-pedantic", "-ggdb" }
end)();
