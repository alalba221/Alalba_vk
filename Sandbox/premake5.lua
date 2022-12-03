project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	staticruntime "on"
    --staticruntime "off"
	files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "%{wks.location}/Alalba_VK/src",
		"%{wks.location}/Alalba_VK/vendor",
      
      -- except for GLFW, all is header only
      -- Why include GLFW: in the Windwo.h includes glfw. This is needed to be fixed
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.VulkanSDK}", 
        "%{IncludeDir.VMA}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.tinyobj}",

        "%{IncludeDir.GLFW}",
        "%{IncludeDir.stb}",
   }

    links
    {
        "Alalba_VK"
    }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "ALALBA_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "ALALBA_DEBUG" }
      runtime "Debug"
      symbols "On"
      
      links
		{
			--"%{Library.Assimp_Debug}"
		}
        postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"'
		}

   filter "configurations:Release"
      defines { "ALALBA_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"
      links
		{
			--"%{Library.Assimp_Release}"
		}
       postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		}


   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "ALALBA_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"
      links
		{
			--"%{Library.Assimp_Release}"
		}
       postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		}