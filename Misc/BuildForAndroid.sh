# Script expects the CMake build configuration to be passed as an argument,
# i.e. "Debug" or "Release".

ndk=28.0.13004108
build_dir=Build/Android/$1

echo Building with Android NDK: $ndk
echo Directory: $build_dir
echo Configuration type: $1

cmake -B $build_dir \
      -DCMAKE_TOOLCHAIN_FILE=$ANDROID_SDK/ndk/$ndk/build/cmake/android.toolchain.cmake \
      -G Ninja \
      -DCPM_SOURCE_CACHE=Build \
      -DCMAKE_BUILD_TYPE=$1 \
      -DPOLLY_BUILD_APPS=OFF

cmake --build $build_dir --parallel --target Polly --config $1

