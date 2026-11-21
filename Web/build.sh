cd ../
source emsdk/emsdk_env.sh
cd OpenSiv3D/Web/App
emcmake cmake -S . -B Build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build Build -j4
