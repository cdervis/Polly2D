# Script expects the CMake build configuration to be passed as an argument,
# i.e. "Debug" or "Release".

android_sdk=$ANDROID_SDK
android_cmake_binary=$android_sdk/cmake/3.22.1/bin/cmake
android_ndk_version=29.0.14033849
android_platform=android-28
android_stl=c++_shared
android_toolchain=$android_sdk/ndk/$android_ndk_version/build/cmake/android.toolchain.cmake

echo Building with Android NDK: $android_ndk_version
echo Android Platform: $android_platform

for android_abi in arm64-v8a armeabi-v7a x86_64
do
    config=Debug

    echo Building for $android_abi-$config
    build_dir=Build/Android/$android_abi-$config
    
    $android_cmake_binary \
        -B $build_dir \
        -G Ninja \
        -D CPM_SOURCE_CACHE=Build \
        -D CMAKE_BUILD_TYPE=$config \
        -D CMAKE_TOOLCHAIN_FILE=$android_toolchain \
        -D ANDROID_ABI=$android_abi \
        -D ANDROID_PLATFORM=$android_platform \
        -D ANDROID_STL=$android_stl \
        -D POLLY_BUILD_APPS=OFF

    $android_cmake_binary --build $build_dir --target Polly --parallel || { echo 'Build failed'; exit 1; }
done

