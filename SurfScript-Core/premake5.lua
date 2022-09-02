project "SurfScript-Core"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("../EngineEditor/res/Scripts")
	objdir ("../EngineEditor/res/Scripts/int")

	files 
	{
		"Source/**.cs",
		"Properties/**.cs"
	}
	
	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"