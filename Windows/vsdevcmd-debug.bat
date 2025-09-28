call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild ""%1".vcxproj"
cd ""%2/App"
"%3(debug).exe"
