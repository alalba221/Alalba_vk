-- premake5.lua
include "Dependencies.lua"

workspace "Alalba_VK"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   targetdir "build"
 --  startproject "Sandbox"
   startproject "DPA8090"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
include "Alalba_VK/vendor/GLFW"

group ""


include "Alalba_VK"
include "Sandbox"
include "DPA8090"