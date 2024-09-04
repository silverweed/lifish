#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Usage: $0 <version>"
	exit 1
fi

DIR="lifish_BOOM_${1}_windows_x64"

mkdir -p "$DIR"
if [[ -n $(ls "$DIR") ]]; then
	echo "Directory $DIR not empty!"
	exit 2
fi

[[ -d assets ]] || {
	echo Missing assets directory!
	exit 3
}
[[ -x Release/lifish ]] || {
	echo Missing lifish executable! Did you compile the game?
	exit 3
}

mkdir "$DIR"/{assets,saves}
cp -r assets/{fonts,graphics,l10n,levels.json,music,screens,sounds} "$DIR/assets"
cp Release/lifish "$DIR"
cp windows/libs/* "$DIR"

pushd "$DIR"

cat > README.txt <<'EOF'
------------------------------------
BOOM Remake by silverweed
https://github.com/silverweed/lifish
------------------------------------

The game will only work on a 64bit Windows.
EOF

popd

echo "Created app in $DIR."
echo Compressing...

rm -f "$DIR.zip" 2>/dev/null
powershell -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('$DIR', '$DIR.zip'); }"

rm -rf "$DIR"
echo Done.
