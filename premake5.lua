workspace "MechEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "MechEngine/vendor/glfw/include"
IncludeDir["GLAD"] = "MechEngine/vendor/glad/include"
IncludeDir["IMGUI"] = "MechEngine/vendor/imgui"
IncludeDir["glm"] = "MechEngine/vendor/glm"
IncludeDir["stb_image"] = "MechEngine/vendor/stb_image"
IncludeDir["entt"] = "MechEngine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "MechEngine/vendor/yaml-cpp/include"

include "MechEngine/vendor/glfw"
include "MechEngine/vendor/glad"
include "MechEngine/vendor/imgui"
include "MechEngine/vendor/yaml-cpp"

project "MechEngine"
	location "MechEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "mepch.h"
	pchsource "MechEngine/src/mepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",	
		"%{IncludeDir.yaml_cpp}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"IMGUI",
		"yaml-cpp",
		"opengl32.lib"
	}


	filter "system:windows"
		systemversion "latest"

		defines{
			"IMGUI_IMPL_OPENGL_LOADER_GLAD",
			"ME_PLATFORM_WINDOWS",
			"ME_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations.Debug"
		defines "ME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations.Release"
		defines "ME_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations.Dist"
		defines "ME_DIST"
		runtime "Release"
		optimize "on"

project "EngineEditor"
		kind "ConsoleApp"
		location "EngineEditor"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"MechEngine/vendor/spdlog/include",
			"MechEngine/src",
			"MechEngine/vendor",
			"%{IncludeDir.glm}",
			"%{IncludeDir.entt}"
		}

		links{
			"MechEngine"
		}

		filter "system:windows"
			staticruntime "On"
			systemversion "latest"

			defines{
				"ME_PLATFORM_WINDOWS"
			}

		filter "configurations.Debug"
			defines "ME_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations.Release"
			defines "ME_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations.Dist"
			defines "ME_DIST"
			runtime "Release"
			optimize "on"
