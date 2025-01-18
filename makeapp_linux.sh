#!/usr/bin/env bash

set -xe

system_arch="$(uname -m)"

sw_version="1.8.2"

# These are rolling versions, it shouldn't be assumed that a subsequent build will use the same tool and runtime code
appimagetool_version="continuous"
appimagetool_filename="appimagetool-$system_arch.AppImage"
appimagetool_baseurl="https://github.com/AppImage/appimagetool/releases/download/$appimagetool_version"

runtime_version="continuous"
runtime_filename="runtime-$system_arch"
runtime_baseurl="https://github.com/AppImage/type2-runtime/releases/download/$runtime_version"

# Shared libraries as of Ubuntu 22.04
required_libs=(
	libsfml-audio.so.2.5
	libsfml-graphics.so.2.5
	libsfml-system.so.2.5
	libsfml-window.so.2.5

	libbsd.so.0
	libFLAC.so.8
	libfreetype.so.6
	libmd.so.0
	libogg.so.0
	libopenal.so.1
	libsndio.so.7
	libvorbis.so.0
	libvorbisenc.so.2
	libvorbisfile.so.3
)

cmake -S . -B build -DRELEASE=true
cmake --build build -j "$(nproc)"

mkdir -p lifish.AppDir/usr/{bin,lib,share/applications}

cp -a linux/* lifish.AppDir/
cp -a linux/*.desktop lifish.AppDir/usr/share/applications/
cp -a assets/ lifish.AppDir/
cp -a build/lifish lifish.AppDir/usr/bin/

for lib in "${required_libs[@]}"; do
	cp -a -L "/lib/$system_arch-linux-gnu/$lib" lifish.AppDir/usr/lib/
done

if [ ! -f "$appimagetool_filename" ]; then
	curl -sSf -L -O "$appimagetool_baseurl/$appimagetool_filename"
	chmod +x "$appimagetool_filename"
fi

if [ ! -f "$runtime_filename" ]; then
	curl -sSf -L -O "$runtime_baseurl/$runtime_filename"
	chmod +x "$runtime_filename"
fi

./"$appimagetool_filename" --no-appstream --runtime-file "runtime-$system_arch" lifish.AppDir "BOOM-Remake-$sw_version-linux-$system_arch.AppImage"

rm -r lifish.AppDir
