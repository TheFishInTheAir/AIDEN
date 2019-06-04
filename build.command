echo This is the radical OSX build script

if ! -a ./build ; then
    mkdir build
fi


pushd ./build

python ../libs/gl3w_gen.py


cp -R ../res/ ./

cmake  -G "Unix Makefiles" ..
cmake --build . --config Release

popd
