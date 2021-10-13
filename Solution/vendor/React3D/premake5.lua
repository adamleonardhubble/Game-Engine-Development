project "ReactPhysics3d"
    kind "StaticLib"
    language "C++"
    staticruntime "off"
	systemversion "latest"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("build/" .. outputdir .. "/%{prj.name}")

	files
	{
        "src/**.h",
		"src/**.cpp"
    }
	
	includedirs
	{
		"src",
		"src/body",
		"src/collision",
		"src/constraint",
		"src/containers",
		"src/engine",
		"src./mathematics",
		"src/memory",
		"src/utils"
	} 
    
	filter "system:windows"
		defines 
		{ 
			"WIN32",
			"_WINDOWS"
		}
		
	 filter { "system:windows", "configurations:Debug" }
		
		defines "DEBUG"
		runtime "Debug"
		symbols "On"
		
    filter { "system:windows", "configurations:Release" }
		defines "NDEBUG"
		runtime "Release"
		optimize "On"

