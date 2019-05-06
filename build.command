echo This is the radical OSX build script

if ! -a ./build ; then
    mkdir build
fi


pushd ./build

cp -R ../res/ ./

cmake  -G "Unix Makefiles" ..
cmake --build . --config Release

popd
