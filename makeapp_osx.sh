#!/usr/bin/env bash

set -xe

sfml_version="2.6.1"
sfml_filename="SFML-$sfml_version-sources.zip"
sfml_url="https://www.sfml-dev.org/files/$sfml_filename"

if [ ! -f "$sfml_filename" ]; then
	curl -sSf -L -O "$sfml_url"
	echo "5bf19e5c303516987f7f54d4ff1b208a0f9352ffa1cd55f992527016de0e8cb7  $sfml_filename" | shasum -a 256 -c
	unzip "$sfml_filename"
	mv "SFML-$sfml_version" SFML
fi

pushd SFML
# These arguments to patch avoid prompting to apply the changes repeatedly, making the script somewhat idempotent
patch -N -r- -p 1 < ../osx/skip_obtaining_hid_devices.patch || true
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=install -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DSFML_BUILD_FRAMEWORKS=TRUE
cmake --build build --config Release --target install -j "$(sysctl -n hw.logicalcpu)"
popd

cmake -S . -B build -DRELEASE=true
cmake --build build -j "$(sysctl -n hw.logicalcpu)"

sw_version="1.6"

mkdir -p "BOOM Remake.app"/Contents/{MacOS,Resources,Frameworks}

cp -a osx/Info.plist "BOOM Remake.app/Contents/"
cp -a build/lifish "BOOM Remake.app/Contents/MacOS/"
cp -a osx/BOOM.icns "BOOM Remake.app/Contents/Resources/"
cp -a assets/* "BOOM Remake.app/Contents/Resources/"
cp -a levels.json "BOOM Remake.app/Contents/Resources/"
cp -a SFML/install/sfml-{audio,graphics,system,window}.framework "BOOM Remake.app/Contents/Frameworks/"
cp -a SFML/install/lib/{FLAC,OpenAL,freetype,ogg,vorbis,vorbisenc,vorbisfile}.framework "BOOM Remake.app/Contents/Frameworks/"

sed -i '' "s/CHANGEME_SW_VERSION/$sw_version/" "BOOM Remake.app/Contents/Info.plist"

echo -n "APPL????" > "BOOM Remake.app/Contents/PkgInfo"

hdiutil create -fs HFS+ -srcfolder "BOOM Remake.app" -volname "BOOM Remake $sw_version" "BOOM-Remake-$sw_version-mac.dmg"

rm -r "BOOM Remake.app"
