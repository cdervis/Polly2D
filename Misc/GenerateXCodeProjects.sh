build_dir=Build/Xcode
macos_build_dir=$build_dir/macOS
ios_build_dir=$build_dir/iOS

echo Generating Xcode projects in: $build_dir

cmake -B $macos_build_dir -G Xcode -DCPM_SOURCE_CACHE=Build
cmake -B $ios_build_dir -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCPM_SOURCE_CACHE=Build


