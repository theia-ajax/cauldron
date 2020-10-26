VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "cauldron"
    configurations { "Debug", "Release" }
    platforms { "Linux64", "Win64", "Win32" }
    includedirs { "external/include" }
    --libdirs { "external/lib/%{cfg.platform}/%{cfg.buildcfg}" }
    location "bin"

project "cimgui"
    kind "StaticLib"
    language "C++"
    location "bin/cimgui"
    files "src/cimgui/**.cpp"
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

project "cimgui_impl"
    kind "StaticLib"
    language "C++"
    location "bin/cimgui_impl"
    files "src/cimgui_impl/**.cpp"
    includedirs { "external/include" }
    links { "cimgui" }

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

project "cauldron"
    kind "ConsoleApp"
    language "C"
    location "bin/cauldron"
    files "src/cauldron/**.c"    
    links { "cimgui", "cimgui_impl" }
    includedirs { "src/cimgui", "src/cimgui_impl" }
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

