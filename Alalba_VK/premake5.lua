project "Alalba_VK"
	kind "StaticLib"
	--kind "SharedLib"

	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	--staticruntime "off"
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/tiny_obj_loader/tiny_obj_loader.h",
		"vendor/VMA/include/**.h",
		"vendor/VMA/include/**.cpp",
		"%{prj.location}/src/%{prj.name}/Shaders/**",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_VULKAN",
		"GLFW_INCLUDE_NONE",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_FORCE_RADIANS",
		--"VMA_IMPLEMENTATION",
		""
	}

	includedirs
	{
		"src",
		"src/pch",
		--"%{IncludeDir.Assimp}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.tinyobj}",
		"%{IncludeDir.VMA}",
		"%{IncludeDir.stb}",
	}

	links
	{
		"GLFW",
		"%{Library.Vulkan}",
	}

	--https://datacadamia.com/lang/dos/argument#combinations
	-- ~nx: name and extension, ~n: name, ~x: extension
	prebuildcommands [[for %%i in (src\Alalba_VK\Shaders\*.vert src\Alalba_VK\Shaders\*.frag src\Alalba_VK\Shaders\*.comp ) do (C:\VulkanSDK\1.3.236.0\Bin\glslangvalidator.exe  --target-env vulkan1.3 -o "%%~dpi\%%~ni.spv" %%i )]]
	-- prebuildcommands [[for %%i in (src\Alalba_VK\Shaders\*.vert src\Alalba_VK\Shaders\*.frag src\Alalba_VK\Shaders\*.comp ) do (C:\VulkanSDK\1.3.231.1\Bin\glslc.exe %%i -o "%%~dpi\%%~ni.spv")]]

	-- filter "files: %{prj.location}/src/%{prj.name}/Shaders/*"
    -- buildcommands '"C:/VulkanSDK/1.3.224.0/Bin/glslc.exe" -V -o "%{file.directory}/%{file.name}.spv" "%{file.relpath}"'
    -- buildoutputs "%{file.directory}/bin/%{file.name}.spv"

	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ALALBA_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "ALALBA_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			--"%{Library.ShaderC_Debug}",
			--"%{Library.SPIRV_Cross_Debug}",
			--"%{Library.SPIRV_Cross_GLSL_Debug}"
			--"%{Library.Assimp_Debug}"
		}
		postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"'
		}


	filter "configurations:Release"
		defines "ALALBA_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			--"%{Library.ShaderC_Release}",
			--"%{Library.SPIRV_Cross_Release}",
			--"%{Library.SPIRV_Cross_GLSL_Release}"
			--"%{Library.Assimp_Release}"
		}
		postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
			-- '{COPY} "../Hazel/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}


	filter "configurations:Dist"
		defines "ALALBA_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			--"%{Library.ShaderC_Release}",
			--"%{Library.SPIRV_Cross_Release}",
			--"%{Library.SPIRV_Cross_GLSL_Release}"
			--"%{Library.Assimp_Release}"
		}
		postbuildcommands 
		{
			--'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"'
		--	'{COPY} "../Hazel/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}
