$ErrorActionPreference = 'Stop'

$compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $compiler) {
    throw 'cl.exe is not available. Run this script from a Visual Studio Developer PowerShell.'
}

$outputDirectory = Join-Path $env:TEMP 'cities-box-connectable-tests'
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null
$outputFile = Join-Path $outputDirectory 'connectable_behavior_tests.exe'

& $compiler.Source /nologo /std:c++17 /utf-8 /EHsc /W4 /WX /Fe:$outputFile "$PSScriptRoot\connectable_behavior_tests.cpp"
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $outputFile
exit $LASTEXITCODE
