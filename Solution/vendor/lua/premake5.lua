

	configurations
	{
		"Debug",
		"Release"
	}
	project "Lua"
	location "vendor/lua"
	language "C"
	systemversion "latest"
	kind "StaticLib"
	staticruntime "off"
	

	targetdir ("../../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../../build/" .. outputdir .. "/%{prj.name}")


	includedirs 
	{
		"vendor/lua/"
	}

	files 
	{ 
		"*.h",
		"*.c"
	}
	
		filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"