workspace "SurfEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "SurfEngine/vendor/glfw/include"
IncludeDir["GLAD"] = "SurfEngine/vendor/glad/include"
IncludeDir["IMGUI"] = "SurfEngine/vendor/imgui"
IncludeDir["glm"] = "SurfEngine/vendor/glm"
IncludeDir["stb_image"] = "SurfEngine/vendor/stb_image"
IncludeDir["entt"] = "SurfEngine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "SurfEngine/vendor/yaml-cpp/include"
IncludeDir["mono"] = "SurfEngine/vendor/mono"

include "SurfEngine/vendor/glfw"
include "SurfEngine/vendor/glad"
include "SurfEngine/vendor/imgui"
include "SurfEngine/vendor/yaml-cpp"

project "SurfEngine"
	location "SurfEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "sepch.h"
	pchsource "SurfEngine/src/sepch.cpp"

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
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.mono}"
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
			"SE_PLATFORM_WINDOWS",
			"SE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations.Debug"
		defines "SE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations.Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations.Dist"
		defines "SE_DIST"
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

		defines{
			"_CRT_SECURE_NO_WARNINGS"
		}

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"SurfEngine/vendor/spdlog/include",
			"SurfEngine/src",
			"SurfEngine/vendor",
			"%{IncludeDir.glm}",
			"%{IncludeDir.entt}",
			"%{IncludeDir.mono}"
		}

		links{
			"SurfEngine"
		}

		filter "system:windows"
			systemversion "latest"

			defines{
				"SE_PLATFORM_WINDOWS"
			}

		filter "configurations.Debug"
			defines "SE_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations.Release"
			defines "SE_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations.Dist"
			defines "SE_DIST"
			runtime "Release"
			optimize "on"
