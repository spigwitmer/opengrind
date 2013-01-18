solution "OpenGrind"
configurations { "Debug", "Release" }
defines { "DEBUG" }
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

function ng_stuff()
	links { "glfw3", "Xrandr", "X11", "lua5.1", "GLXW", "GL", "openal", "physfs" }
	libdirs { "extern/glfw3/src" }
	includedirs { "src", "/usr/include/lua5.1", "extern/glfw3/include", "extern/glxw/include" }
end

-- nepgear project
(function(self)
	project "Nepgear"
	targetname "nepgear"
	targetdir  "bin"
	kind "StaticLib"
	language "C++"
	files { "src/**.cpp" }
	excludes { "src/main.cpp", "src/screens/**" }
	ng_stuff()

	configuration { "linux", "gmake" }
	buildoptions { "-std=c++0x", "-Wall", "-pedantic", "-ggdb" }
end)();

-- main project
(function(self)
	project "OpenGrind"
	targetname "opengrind"
	targetdir  "bin"
	kind "WindowedApp"
	language "C++"
	files { "src/main.cpp", "src/screens/**" }
	links { "Nepgear" }
	ng_stuff()

	configuration { "linux", "gmake" }
	buildoptions { "-std=c++0x", "-Wall", "-pedantic", "-ggdb" }
end)();

-- tests
function test(name)
	local file = name:lower()
	project("OpenGrind-"..name)
	targetname(file)
	targetdir "bin"
	kind "WindowedApp"
	language "C++"
	files { "tests/"..file..".cpp" }
	links { "Nepgear" }
	ng_stuff()

	configuration { "linux", "gmake" }
	buildoptions { "-std=c++0x", "-Wall", "-pedantic", "-ggdb" }
end

test("ShaderTest")
test("VFS")
