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

(function(self)
	-- Note: This lib isn't provided directly from Unicode.org anymore.
	-- Apparently, it's unmaintained and a bit buggy? Either way, it's handy
	-- and SimpleINI wants it.
	project "ConvertUTF"
	targetname "convertutf"
	targetdir "bin"
	kind "StaticLib"
	language "C"
	includedirs { "extern/simpleini" }
	files { "extern/simpleini/ConvertUTF.c" }
end)();

function ng_stuff()
	links {
		"glfw3", "Xrandr", "X11", "lua5.1", "GLXW",
		"GL", "openal", "physfs", "ConvertUTF",
	}
	libdirs { "extern/glfw3/src" }
	includedirs {
		"src",
		"/usr/include/lua5.1",
		"extern/glfw3/include",
		"extern/glxw/include",
		"extern/simpleini",
		"extern/simpleopt"
	}
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
