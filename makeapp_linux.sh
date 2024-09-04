#!/usr/bin/env bash

set -xe

system_arch="$(uname -m)"

sw_version="1.7"

appimagetool_version="13"
appimagetool_filename="appimagetool-$system_arch.AppImage"
appimagetool_url="https://github.com/AppImage/AppImageKit/releases/download/$appimagetool_version/$appimagetool_filename"

declare -A appimagetool_hashes
appimagetool_hashes["aarch64"]="334e77beb67fc1e71856c29d5f3f324ca77b0fde7a840fdd14bd3b88c25c341f"
appimagetool_hashes["x86_64"]="df3baf5ca5facbecfc2f3fa6713c29ab9cefa8fd8c1eac5d283b79cab33e4acb"

# Shared libraries as of Ubuntu 20.04
required_libs=(
	libsfml-audio.so.2.5
	libsfml-graphics.so.2.5
	libsfml-system.so.2.5
	libsfml-window.so.2.5

	libFLAC.so.8
	libfreetype.so.6
	libogg.so.0
	libopenal.so.1
	libsndio.so.7.0
	libvorbis.so.0
	libvorbisenc.so.2
	libvorbisfile.so.3
)

cmake -S . -B build -DRELEASE=true
cmake --build build -j "$(nproc)"

mkdir -p lifish.AppDir/usr/{bin,lib,share/applications}

cp -a linux/* lifish.AppDir/
cp -a assets/ lifish.AppDir/
cp -a build/lifish lifish.AppDir/usr/bin/

for lib in "${required_libs[@]}"; do
	cp -a -L "/lib/$system_arch-linux-gnu/$lib" lifish.AppDir/usr/lib/
done

if [ ! -f "$appimagetool_filename" ]; then
	curl -sSf -L -O "$appimagetool_url"
	echo "${appimagetool_hashes[$system_arch]}  $appimagetool_filename" | shasum -a 256 -c
	chmod +x "$appimagetool_filename"
fi

./"$appimagetool_filename" lifish.AppDir "BOOM-Remake-$sw_version-linux-$system_arch.AppImage"

rm -r lifish.AppDir
