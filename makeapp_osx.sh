#!/usr/bin/env bash

set -xe

sfml_version="2.6.2"
sfml_filename="$sfml_version.zip"
sfml_url="https://github.com/SFML/SFML/archive/refs/tags/$sfml_filename"

if [ ! -f "$sfml_filename" ]; then
	curl -sSf -L -O "$sfml_url"
	echo "19d6dbd9c901c74441d9888c13cb1399f614fe8993d59062a72cfbceb00fed04  $sfml_filename" | shasum -a 256 -c
	unzip "$sfml_filename"
	mv "SFML-$sfml_version" SFML
fi

pushd SFML
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=install -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DSFML_BUILD_FRAMEWORKS=TRUE
cmake --build build --config Release --target install -j "$(sysctl -n hw.logicalcpu)"
popd

cmake -S . -B build -DRELEASE=true
cmake --build build -j "$(sysctl -n hw.logicalcpu)"

sw_version="1.8.2"

mkdir -p "BOOM Remake.app"/Contents/{MacOS,Resources,Frameworks}

cp -a osx/Info.plist "BOOM Remake.app/Contents/"
cp -a build/lifish "BOOM Remake.app/Contents/MacOS/"
cp -a osx/BOOM.icns "BOOM Remake.app/Contents/Resources/"
cp -a assets/* "BOOM Remake.app/Contents/Resources/"
cp -a SFML/install/sfml-{audio,graphics,system,window}.framework "BOOM Remake.app/Contents/Frameworks/"
cp -a SFML/install/lib/{FLAC,OpenAL,freetype,ogg,vorbis,vorbisenc,vorbisfile}.framework "BOOM Remake.app/Contents/Frameworks/"

sed -i '' "s/CHANGEME_SW_VERSION/$sw_version/" "BOOM Remake.app/Contents/Info.plist"

echo -n "APPL????" > "BOOM Remake.app/Contents/PkgInfo"

hdiutil create -fs HFS+ -srcfolder "BOOM Remake.app" -volname "BOOM Remake $sw_version" "BOOM-Remake-$sw_version-mac.dmg"

rm -r "BOOM Remake.app"
