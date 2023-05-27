-- Dependecies.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["VMA"] = "%{wks.location}/Alalba_VK/vendor/VMA/include"
IncludeDir["glm"] = "%{wks.location}/Alalba_VK/vendor/glm"
IncludeDir["GLFW"] = "%{wks.location}/Alalba_VK/vendor/GLFW/include"
IncludeDir["spdlog"] = "%{wks.location}/Alalba_VK/vendor/spdlog/include"
IncludeDir["Assimp"] = "%{wks.location}/Alalba_VK/vendor/assimp/include"
IncludeDir["tinyobj"] = "%{wks.location}/Alalba_VK/vendor/tiny_obj_loader"
IncludeDir["tinygltf"] = "%{wks.location}/Alalba_VK/vendor/tinygltf"
IncludeDir["stb"] = "%{wks.location}/Alalba_VK/vendor/stb/include"
IncludeDir["ImGui"] = "%{wks.location}/Alalba_VK/vendor/imgui-docking"
IncludeDir["entt"] = "%{wks.location}/Alalba_VK/vendor/entt/include"



LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["Assimp_Debug"] = "%{wks.location}/Alalba_VK/vendor/assimp/bin/Debug/assimp-vc142-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/Alalba_VK/vendor/assimp/bin/Release/assimp-vc142-mt.lib"


-- Shared Lib
Binaries = {}
Binaries["Assimp_Debug"] = "%{wks.location}/Alalba_VK/vendor/assimp/bin/Debug/assimp-vc142-mtd.dll"
Binaries["Assimp_Release"] = "%{wks.location}/Alalba_VK/vendor/assimp/bin/Release/assimp-vc142-mt.dll"
