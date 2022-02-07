project "Lua"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
	"lua.cpp",
	"lua.h",
	"luaconf.h",
	"lualib.h",
	"lauxlib.h",
	"lua.hpp",
	"src/lapi.c",
	"src/lcode.c",
	"src/lctype.c",
	"src/ldebug.c",
	"src/ldo.c",
	"src/ldump.c",
	"src/lfunc.c",
	"src/lgc.c",
	"src/llex.c",
	"src/lmem.c",
	"src/lobject.c",
	"src/lopcodes.c",
	"src/lparser.c",
	"src/lstate.c",
	"src/lstring.c",
	"src/ltable.c",
	"src/ltm.c",
	"src/lundump.c",
	"src/lvm.c",
	"src/lzio.c",
	"src/lauxlib.c",
	"src/lbaselib.c",
	"src/lcorolib.c",
	"src/ldblib.c",
	"src/liolib.c",
	"src/lmathlib.c",
	"src/loadlib.c",
	"src/loslib.c",
	"src/lstrlib.c",
	"src/ltablib.c",
	"src/lutf8lib.c",
	"src/linit.c"
   }

	filter "system:windows"
		systemversion "latest"

	filter "configurations.Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations.Release"
		runtime "Release"
		optimize "on"
