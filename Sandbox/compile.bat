REM Set the path to the glslangValidator executable
SET GLSLANG_VALIDATOR_PATH="C:/VulkanSDK/1.3.239.0/Bin/glslc.exe"

REM Set the folder path containing the GLSL shaders
SET SHADERS_FOLDER="shaders"

REM Set the output folder for the compiled SPIR-V binaries
SET OUTPUT_FOLDER="shaders"

REM Compile all shaders in the folder
for %%f in (%SHADERS_FOLDER%\*.vert %SHADERS_FOLDER%\*.frag %SHADERS_FOLDER%\*.comp) do (
    %GLSLANG_VALIDATOR_PATH%  "%%f" -o "%OUTPUT_FOLDER%\%%~nxf.spv"
)

REM Check if any shaders were compiled
if exist "%OUTPUT_FOLDER%\*.spv" (
    echo Shaders compiled successfully.
) else (
    echo No shaders found in the specified folder.
)
pause