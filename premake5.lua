VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "cauldron"
    configurations { "Debug", "Release" }
    platforms { "Linux64", "Win64", "Win32" }
    includedirs { "external/include" }
    libdirs { "external/lib/%{cfg.platform}/%{cfg.buildcfg}" }
    location "bin"

project "imgui_impl"
    kind "StaticLib"
    language "C++"
    location "bin/imgui_impl"
    files "imgui_impl/**.cpp"
    includedirs { "external/include" }
    links { "cimgui.lib" }

    filter "platforms:Win64"
        system "Windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        architecture "x86_64"

    filter "platforms:Win32"
        system "Windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        architecture "x86"

    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "_NDEBUG" }
        symbols "Off"

    postbuildcommands { "{COPY} %{cfg.targetdir}/imgui_impl.lib external/lib/%{cfg.platform}/%{cfg.buildcfg}/imgui_impl.lib" }

project "cauldron"
    kind "ConsoleApp"
    language "C"
    location "bin/cauldron"
    files "src/**.c"    
    links { "vulkan-1.lib", "cimgui.lib", "imgui_impl" }
    cppdialect "C++latest"
    --postbuildcommands { "powershell.exe -File ../../asset_pipeline.ps1 -target %{prj.name} -platform %{cfg.platform} -configuration %{cfg.buildcfg}" }

    filter "platforms:Win64"
        system "Windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        architecture "x86_64"
        includedirs { string.format("%s/Include", VULKAN_SDK) }
        libdirs { string.format("%s/Lib", VULKAN_SDK) }

    filter "platforms:Win32"
        system "Windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        architecture "x86"
        includedirs { string.format("%s/Include", VULKAN_SDK) }
        libdirs { string.format("%s/Lib32", VULKAN_SDK) }

    filter "configurations:Debug"
        links {"SDL2maind"}

    filter "configurations:Release"
        links { "SDL2main" }

    filter "platforms:Linux64"
        system "Linux"
        architecture "x86_64"

    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        kind "WindowedApp"
        defines { "_NDEBUG" }
        optimize "On"

