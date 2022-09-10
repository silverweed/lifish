#!/bin/bash
# Create a Mac OSX app bundle.
# MUST be launched from the lifish directory
# The lifish executable MUST already exist.

if [[ $# < 1 ]]; then
	echo "Usage: $0 <version>"
	exit 1
fi

OUT_ZIP="lifish_BOOM_${1}_osx.zip"
APPNAME="BOOM Remake"
MACOS=(lifish)
RESOURCES=(levels.json)
ASSETS=(assets/fonts assets/graphics assets/sounds assets/screens assets/music assets/l10n)
FRAMEWORK_PATH=/Library/Frameworks
FRAMEWORKS=({FLAC,ogg,freetype,OpenAL,vorbis{,enc,file}}.framework)
# Ugh...
LOCAL_FRAMEWORK_PATH="../SFML-2.5.1-macos-clang/Frameworks"
LOCAL_FRAMEWORKS=(sfml-{window,graphics,system,audio}.framework)

for i in ${MACOS[@]} ${RESOURCES[@]};  do
	[[ -e $i ]] || {
		echo "$i not found in this directory." >&2
		exit 1
	}
done

for i in ${FRAMEWORKS[@]}; do
	[[ -d "$FRAMEWORK_PATH/$i" ]] || {
		echo "$i not found in $FRAMEWORK_PATH". >&2
		exit 1
	}
done

# Lazy workaround to SFML not being *really* installed on my machine...
for i in ${LOCAL_FRAMEWORKS[@]}; do
	[[ -d "$LOCAL_FRAMEWORK_PATH/$i" ]] || {
		echo "$i not found in $LOCAL_FRAMEWORK_PATH". >&2
		exit 1
	}
done

set -x

rm -rf "$APPNAME.app"
mkdir -p "$APPNAME.app"/Contents/{MacOS,Resources,Frameworks}
for i in ${FRAMEWORKS[@]}; do
	cp -R "$FRAMEWORK_PATH/$i" "$APPNAME.app"/Contents/Frameworks/.
done
for i in ${LOCAL_FRAMEWORKS[@]}; do
	cp -R "$LOCAL_FRAMEWORK_PATH/$i" "$APPNAME.app"/Contents/Frameworks/.
done

cat > "$APPNAME.app"/Contents/Info.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
    <dict>
        <key>CFBundleExecutable</key>
        <string>lifish</string>
        <key>CFBundleIdentifier</key>
        <string>github.com/silverweed/lifish</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>0.0</string>
        <key>CFBundleName</key>
        <string>lifish</string>
        <key>CFBundleDisplayName</key>
        <string>BOOM Remake</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
	<key>CFBundleIconFile</key>
	<string>BOOM.icns</string>
    </dict>
</plist>
EOF

cp -r ${MACOS[@]} "$APPNAME.app"/Contents/MacOS/.
cp -r ${RESOURCES[@]} "$APPNAME.app"/Contents/Resources/.
mkdir "$APPNAME.app"/Contents/Resources/{assets,saves}
cp -r ${ASSETS[@]} "$APPNAME.app"/Contents/Resources/assets/
cp osx/BOOM.icns "$APPNAME.app"/Contents/Resources/.

pushd "$APPNAME.app"/Contents/MacOS
ln -s ../Resources/assets
ln -s ../Resources/saves
ln -s ../Resources/levels.json

popd
find "$APPNAME.app" -type f -exec install_name_tool -add_rpath "@executable_path/../Frameworks" {} \; 2>&1 | grep -v "not a Mach-O"

rm -f "$OUT_ZIP" 2>/dev/null
zip -r --symlinks "$OUT_ZIP" "$APPNAME.app"
