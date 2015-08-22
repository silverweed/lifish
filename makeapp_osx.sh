#!/bin/bash
# Create a Mac OSX app bundle.
# MUST be launched from the lifish directory
# The lifish executable MUST already exist.

APPNAME=Lifish
MACOS=(lifish assets levels.json)
FRAMEWORK_PATH=/Library/Frameworks

for i in ${MACOS[@]}; do
	[[ -e $i ]] || {
		echo "$i not found in this directory." >&2
		exit 1
	}
done

[[ -d "$FRAMEWORK_PATH/SFML.framework" ]] || {
	echo "SFML.framework not found in $FRAMEWORK_PATH". >&2
	exit 1
}

set -x

rm -rf "$APPNAME.app"
mkdir -p "$APPNAME.app"/Contents/{MacOS,Resources}
cp -R "$FRAMEWORK_PATH/SFML.framework" "$APPNAME.app"/Contents/Resources/.

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
        <string>Lifish</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
    </dict>
</plist>
EOF

cp -r ${MACOS[@]} "$APPNAME.app"/Contents/MacOS/.
