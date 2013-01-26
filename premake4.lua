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

(function()
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

(function()
	project "Lua"
	targetname "lua-ng"
	targetdir "bin"
	if os.is("windows") then
		defines { "_CRT_SECURE_NO_WARNINGS" }
	end
	kind "StaticLib"
	language "C"
	includedirs { "extern/lua/src" }
	files { "extern/lua/src/*.c" }
	excludes { "extern/lua/src/lua.c", "extern/lua/src/luac.c" }
end)();

function ng_stuff()
	links {
		"glfw3", "Xrandr", "X11", "GLXW", "Lua",
		"openal", "physfs", "ConvertUTF"
	}
	if os.is("windows") then
		links { "opengl32" }
	elseif os.is("macosx") then
		links { "OpenGL.framework" }
	else
		links { "GL" }
	end
	libdirs { "extern/glfw3/src" }
	includedirs {
		"src",
		"extern/lua/src",
		"extern/glm",
		"extern/glfw3/include",
		"extern/glxw/include",
		"extern/simpleini",
		"extern/simpleopt",
		"extern/iqm"
	}
end

-- nepgear project
(function()
	project "Nepgear"
	targetname "nepgear"
	targetdir  "bin"
	kind "StaticLib"
	language "C++"
	files { "src/**.cpp" }
	excludes { "src/main.cpp", "src/screens/**" }
	ng_stuff()

	configuration { "linux", "gmake" }
	buildoptions { "-Wall", "-pedantic", "-ggdb" }
end)();

-- main project
(function()
	project "OpenGrind"
	targetname "opengrind"
	targetdir  "bin"
	kind "WindowedApp"
	language "C++"
	files { "src/main.cpp", "src/screens/**" }
	links { "Nepgear" }

	-- awesomium stuff
	includedirs { "/home/colby/src/awesomium_v1.6.5_sdk_linux64/include" }
	libdirs { "./bin" }
	links { "jpeg-ng", "awesomium-1.6.5" }

	ng_stuff()

	configuration { "linux", "gmake" }
	buildoptions { "-Wall", "-pedantic", "-ggdb" }
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
	buildoptions { "-Wall", "-pedantic", "-ggdb" }
end

test("ShaderTest")
test("VFS")
test("ModelLoader")
