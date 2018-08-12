Write-Host "Appveyor Helper scrips https://github.com/TheOneRing/appVeyorHelp"

$ErrorActionPreference="Stop"

$script:INSTALL_DIR="$env:APPVEYOR_BUILD_FOLDER\work\install"
$CMAKE_INSTALL_ROOT="`"$INSTALL_DIR`"" -replace "\\", "/"
$env:PATH="$env:PATH;$script:INSTALL_DIR"

if(!$env:CI -eq "true")
{
    function Push-AppveyorArtifact()
    {
        Write-Host "Push-AppveyorArtifact $ARGS"
    }
    function Start-FileDownload([string] $url, [string] $out)
    {
        if(!$out)
        {
            $out = $url.SubString($url.LastIndexOf("/"))
        }
        wget $url -Outfile $out
    }
}

function LogExec()
{
    $OldErrorActionPreference=$ErrorActionPreference
    $ErrorActionPreference="Continue"
    $LastExitCode = 0
    Write-Host $Args[0], $Args[1..(($Args.Count)-1)]
    & $Args[0] $Args[1..(($Args.Count)-1)]
    if(!$LastExitCode -eq 0)
    {
        exit $LastExitCode
    }
    $ErrorActionPreference=$OldErrorActionPreference
}

#Set environment variables for Visual Studio Command Prompt
#http://stackoverflow.com/questions/2124753/how-i-can-use-powershell-with-the-visual-studio-command-prompt
function BAT-CALL([string] $path, [string] $arg)
{
    Write-Host "Calling `"$path`" `"$arg`""
    cmd /c  "$path" "$arg" `&`& set `|`| exit 1|
    foreach {
      if ($_ -match "=") {
        $v = $_.split("=")
        #Write-Host "ENV:\$($v[0])=$($v[1])"
        set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
      }
    }
    if($LastExitCode -eq 1) {
        Write-Error "$path not found."
    }
}

function Get-QtDir()
{
    $ver = 5.5
    if($env:QT_VER)
    {
        $ver = $env:QT_VER
    }
    return "C:\Qt\$ver\$env:COMPILER\"
}

function SETUP-QT()
{
    [string] $compiler=$env:COMPILER
    $qtDir = Get-QtDir
    $script:QT_BINARY_DIRS = @($qtDir)

    BAT-CALL  "$qtDir\bin\qtenv2.bat"
    if ($compiler.StartsWith("mingw49"))
    {
        #remove sh.exe from path
        $env:PATH=$env:PATH -replace "C:\\Program Files \(x86\)\\Git\\bin", ""
        $script:MAKE="mingw32-make"
        $script:CMAKE_GENERATOR="MinGW Makefiles"
        $script:STRIP=@("strip", "-s")
        $script:QT_BINARY_DIRS += (Resolve-Path "$qtDir\..\..\Tools\mingw492_32\opt\")
    }
    elseif ($compiler.StartsWith("msvc"))
    {
        $arch = "x86"
        if($compiler.EndsWith("64"))
        {
            $arch = "amd64"
        }
        $compilerDirs = @{
                "msvc2010" = "VS100COMNTOOLS";
                "msvc2012" = "VS110COMNTOOLS";
                "msvc2013" = "VS120COMNTOOLS";
                "msvc2015" = "VS140COMNTOOLS"
            }

        $compilerVar = $compilerDirs[$compiler.Split("_")[0]]
        $compilerDir = (get-item -path "env:\$($compilerVar)").Value
        BAT-CALL "$compilerDir\..\..\VC\vcvarsall.bat" $arch
        $script:MAKE="nmake"
        $script:CMAKE_GENERATOR="NMake Makefiles"
        if($arch -eq "x86")
        {
            $script:QT_BINARY_DIRS += ("C:\OpenSSL-Win32")
        }
        else
        {
            $script:QT_BINARY_DIRS += ("C:\OpenSSL-Win64")
        }
    }
}

function Install-ChocolatelyModule([string] $module, [string[]] $myargs)
{
    Write-Host "Install chocolately package $module"
    LogExec cinst $module @myargs -y
}

function Install-CmakeGitModule([string] $url, [hashtable] $arguments)
{
    $module = $url.SubString($url.LastIndexOf("/")+1)
    $module = $module.Substring(0,$module.Length - 4)
    if(!$arguments.Contains("branch"))
    {
        $arguments["branch"] = "master"
    }
    if(!$arguments.Contains("buildType"))
    {
        $arguments["buildType"] = "Release"
    }
    mkdir -Force $env:APPVEYOR_BUILD_FOLDER\work\build\$module
    pushd $env:APPVEYOR_BUILD_FOLDER\work\git
    LogExec git clone -q --depth 1 --branch ([string]$arguments["branch"]) $url $module
    popd
    pushd  $env:APPVEYOR_BUILD_FOLDER\work\build\$module
    LogExec cmake -G $script:CMAKE_GENERATOR  ("-DCMAKE_BUILD_TYPE=`"{0}`"" -f [string]$arguments["buildType"]) $env:APPVEYOR_BUILD_FOLDER\work\git\$module -DCMAKE_INSTALL_PREFIX="$CMAKE_INSTALL_ROOT" $arguments["options"]
    LogExec  $script:MAKE install
    popd
}

function Init([string[]] $chocoDeps, [System.Collections.Specialized.OrderedDictionary] $cmakeModules)
{
    $script:MAKE=""
    $script:CMAKE_GENERATOR=""
    $script:STRIP=$null

    mkdir -Force $env:APPVEYOR_BUILD_FOLDER\work\image | Out-Null
    mkdir -Force $env:APPVEYOR_BUILD_FOLDER\work\build | Out-Null

    SETUP-QT

    if($chocoDeps -contains "ninja") {
        $script:CMAKE_GENERATOR="Ninja"
        $script:MAKE="ninja"
    }

    if ( !(Test-Path "$env:APPVEYOR_BUILD_FOLDER\work\install" ) )
    {
        mkdir -Force $env:APPVEYOR_BUILD_FOLDER\work\install | Out-Null
        mkdir -Force $env:APPVEYOR_BUILD_FOLDER\work\git | Out-Null

        foreach($module in $chocoDeps) {
            if($module -eq "nsis")
            {
                Install-ChocolatelyModule "nsis.portable" @("-pre")
                continue
            }
            Install-ChocolatelyModule $module
        }

        foreach($key in $cmakeModules.Keys) {
            Install-CmakeGitModule $key $cmakeModules[$key]
        }

        [string] $compiler=$env:COMPILER
        if($compiler.StartsWith("msvc"))
        {
            Write-Host "Downloading vcredist.exe"
            if ($compiler.StartsWith("msvc2015"))
            {
                if($compiler.EndsWith("64"))
                {
                    Start-FileDownload https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x64.exe $env:APPVEYOR_BUILD_FOLDER\work\install\vcredist.exe
                }
                else
                {
                    Start-FileDownload https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe $env:APPVEYOR_BUILD_FOLDER\work\install\vcredist.exe
                }
            }
            else
            {
                if($compiler.EndsWith("64"))
                {
                    Start-FileDownload http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x64.exe $env:APPVEYOR_BUILD_FOLDER\work\install\vcredist.exe
                }
                else
                {
                    Start-FileDownload http://download.microsoft.com/download/2/E/6/2E61CFA4-993B-4DD4-91DA-3737CD5CD6E3/vcredist_x86.exe $env:APPVEYOR_BUILD_FOLDER\work\install\vcredist.exe
                }
            }
        }
    }
}

function relativePath([string] $root, [string] $path)
{
    pushd $root
    $out = Resolve-Path -Relative $path
    popd
    return $out
}

function StripFile([string] $name)
{
    if($script:STRIP) {
        if( $name.EndsWith(".dll") -or $name.EndsWith(".exe"))
        {
            Write-Host "strip file $name"
            LogExec @script:STRIP $name
        }
    }
}

function Get-DeployImageName()
{
    $version = Get-Version
    if($env:APPVEYOR_REPO_TAG -eq "true") {
        return "$env:APPVEYOR_PROJECT_NAME`_$version`_Qt$env:QT_VER`_$env:COMPILER"
    }else{
        return "$env:APPVEYOR_PROJECT_NAME`_$env:APPVEYOR_REPO_BRANCH`_$version`_Qt$env:QT_VER`_$env:COMPILER"
    }
}

function Get-Version()
{
    if($env:APPVEYOR_REPO_TAG -eq "true") {
        return $env:APPVEYOR_REPO_TAG_NAME
    }else{
        $commit = ([string]$env:APPVEYOR_REPO_COMMIT).SubString(0,6)
        return $commit
    }
}

function CmakeImageInstall()
{
    $imageName = Get-DeployImageName
    $destDir = "$env:APPVEYOR_BUILD_FOLDER\work\cmakeDeployImage\$imageName"
    $env:DESTDIR = $destDir
    LogExec $script:MAKE install
    $env:DESTDIR = $null
    if(!$LastExitCode -eq 0)
    {
        Write-Error "Build Failed"
    }
    $env:DESTDIR=$null
    $prefix=$script:INSTALL_DIR
    if( $prefix.substring(1,1) -eq ":")
    {
        $prefix=$prefix.substring(3)
    }
    Write-Host "move $destDir\$prefix to $destDir"
    mv -Force "$destDir\$prefix\*" "$destDir"
    $rootLeftOver = $prefix.substring(0, $prefix.indexOf("\"))
    rm -Recurse "$destDir\$rootLeftOver"
}

function CreateDeployImage([string[]] $whiteList, [string[]] $blackList)
{
    $imageName = Get-DeployImageName
    $deployPath = "$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName"

    function copyWithWhitelist([string] $root)
    {
        $files = ls $root -Recurse
        foreach($fileName in $files.FullName)
        {
            $relPath = (relativePath $root $fileName).SubString(2)
            if($whiteList | Where {$relPath -match $_})
            {
                if($blackList | Where {$relPath -match $_})
                {
                    continue
                }
                if(!(Test-Path $deployPath\$relPath))
                {
                    Write-Host "copy $fileName to $deployPath\$relPath"
                    mkdir -Force (Split-Path -Parent $deployPath\$relPath) | Out-Null
                    cp -Force $fileName $deployPath\$relPath
                    StripFile $deployPath\$relPath
                }
            }
        }
    }
    Write-Host "CreateDeployImage $imageName"
    mkdir $deployPath | Out-Null

    copyWithWhitelist "$env:APPVEYOR_BUILD_FOLDER\work\cmakeDeployImage\$imageName"
    copyWithWhitelist "$env:APPVEYOR_BUILD_FOLDER\work\install\"
    foreach($folder in $script:QT_BINARY_DIRS)
    {
        copyWithWhitelist $folder
    }
    Write-Host "Deploy path $deployPath"
    return $deployPath
}

function 7ZipDeployImage()
{
    $imageName = Get-DeployImageName
    LogExec 7za a "$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName.7z" "$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName"
    Push-AppveyorArtifact "$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName.7z"
}

function NsisDeployImage([string] $scriptName)
{
    $imageName = Get-DeployImageName
    $installerName = "$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName.exe"
    $version = Get-Version
    if(([string]$env:COMPILER).StartsWith("msvc"))
    {
        $redist = "$env:APPVEYOR_BUILD_FOLDER\work\install\vcredist.exe"
    }else{
        $redist = "none"
    }
    if($env:COMPILER.EndsWith("64"))
    {
        $defaultinstdir = "`$PROGRAMFILES64"
    }else{
        $defaultinstdir = "`$PROGRAMFILES"
    }
    LogExec makensis.exe /DgitDir=$env:APPVEYOR_BUILD_FOLDER /Dsetupname=$installerName /Dcaption=$imageName /Dversion=$version /Dcompiler=$env:COMPILER /Dvcredist=$redist /Ddefaultinstdir=$defaultinstdir /Dsrcdir=$env:APPVEYOR_BUILD_FOLDER\work\deployImage\$imageName $scriptName
    Push-AppveyorArtifact $installerName
}

# based on http://thesurlyadmin.com/2013/01/07/remove-empty-directories-recursively/
function DeleteEmptyFodlers([string] $root)
{
    $Folders = @()
    foreach($Folder in (Get-ChildItem -Path $root -Recurse -Directory))
       {
            $Folders += New-Object PSObject -Property @{
                Object = $Folder
                Depth = ($Folder.FullName.Split("\")).Count
            }
    }
    $Folders = $Folders | Sort Depth -Descending

    foreach($Folder in $Folders)
    {
       If ($Folder.Object.GetFileSystemInfos().Count -eq 0)
       {
            Write-Host "Delete empty dir:" $Folder.Object.FullName
            Remove-Item -Path $Folder.Object.FullName -Force
       }
    }

}

Write-Host "CMAKE_INSTALL_ROOT: $CMAKE_INSTALL_ROOT"
Write-Host "Image-Name: ", (Get-DeployImageName)

Export-ModuleMember -Function @("Init","CmakeImageInstall", "CreateDeployImage", "LogExec", "7ZipDeployImage", "NsisDeployImage", "DeleteEmptyFodlers") -Variable @("CMAKE_INSTALL_ROOT")
