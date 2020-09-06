param(
    [Parameter(Mandatory)]
    [string] $target,

    [ValidateSet("Win32", "Win64", "All")]
    [Parameter(Mandatory)]
    [string] $platform,
    
    [ValidateSet("Debug", "Release", "All")] 
    [Parameter(Mandatory)]
    [string] $configuration,
    
    [string] $asset_dir = "assets",
    [switch] $clean = $false
)

$platform_groups = @{
    "Win32" = @("Win32")
    "Win64" = @("Win64")
    "All"   = @("Win32", "Win64")
}

$configuration_groups = @{
    "Debug"   = @("Debug")
    "Release" = @("Release")
    "All"     = @("Debug", "Release")
}

function LinkExists {
    return Test-Path $link_path -PathType any
}

function Invoke-CreateAssetSymlink() {
    param (
        [string] $platform,
        [string] $configuration
    );

    $link_dir = "$PSScriptRoot\bin\$target\bin\$platform\$configuration"
    $link_path = "$link_dir\$asset_dir"
    $link_target = "$PSScriptRoot\$asset_dir"

    Write-Host "**************************************************"
    Write-Host "**              ASSET PIPELINE"
    Write-Host "**  Target:         $target"
    Write-Host "**  Platform:       $platform"
    Write-Host "**  Configuration:  $configuration"
    Write-Host "**  Asset Path:     $asset_dir"
    Write-Host "**  Link Path:      $link_path"
    Write-Host "**  Link Target:    $link_target"
    Write-Host "**  Clean:          $clean"
    Write-Host "**************************************************"

    if ($clean -eq $true) {
        Write-Host "Cleaning..."

        if (LinkExists) {
            Write-Host "Removing asset directory symbolic link."
            (Get-Item $link_path).Delete()
        }
        return
    }

    # Create symbolic link to the assets directory next to the binary if it does not already exist.
    Write-Host "Creating symbolic link to assets path..."

    if (LinkExists) {
        Write-Host "Skipping creation of symbolic link to asset directory because it already exists."
    }
    else {
        try {
            New-Item -ItemType SymbolicLink -Path $link_path -Target $link_target -ErrorAction Stop | Out-Null
        }
        catch [System.UnauthorizedAccessException] {
            Write-Host "The asset pipeline failed to create a symbolic link. Creating a symbolic link in Win10 requires admin access or developer mode to be turned on. Please turn on developer mode and restart."
        }
    }
}

function Invoke-CompileShaders() {
    # Compile shaders
    # All shaders matching the pattern ./assets/shaders/*.glsl will be compiled into ./assets/shaders/__generated__/*.spv
    $shader_dir = "$PSScriptRoot\$assetDir\shaders"
    $shader_generated_dir = "$shader_dir\__generated__"

    if ($clean -eq $true) {
        Write-Host "Removing compiled shaders..."
        Remove-Item $shader_generated_dir -Recurse -Force -ErrorAction Ignore | Out-Null
        return
    }

    Write-Host "Compilng shaders..."

    New-Item -ItemType Directory $shader_generated_dir -ErrorAction Ignore | Out-Null

    Get-ChildItem $shader_dir\* -Include *.glsl, *.vert, *.frag, *.geom, *.tesc, *.tese, *.comp |
    ForEach-Object {
        $shader_path = Get-Item $_ | Resolve-Path -Relative
        $shader_name = $_.Name
        $output_file = "$shader_generated_dir\$shader_name.spv"
        glslangValidator.exe -V $shader_path -o $output_file
    }
}

$platform_group = $platform_groups[$platform]
$configuration_group = $configuration_groups[$configuration]

foreach ($platform in $platform_group) {
    foreach ($configuration in $configuration_group) {
        Invoke-CreateAssetSymlink -platform $platform -configuration $configuration
    }
}
Invoke-CompileShaders
