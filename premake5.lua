workspace "Chippy"
	architecture "x86_64"
	configurations { "Debug", "Release" }

project "Chippy"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++14"
	staticruntime "On"

	targetdir "bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	objdir  "build/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

	files {
		"src/**.cc",
		"src/**.h"
	}

	includedirs {
		"include",
		"src/**"
	}

	links {
		"X11",
		"GL",
		"pthread",
		"png"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"