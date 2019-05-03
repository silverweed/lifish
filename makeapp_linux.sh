#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Usage: $0 <version>"
	exit 1
fi

DIR="lifish_BOOM_${1}_linux_x64"

mkdir -p "$DIR"
if [[ -n $(ls "$DIR") ]]; then
	echo "Directory $DIR not empty!"
	exit 2
fi

[[ -d assets ]] || {
	echo Missing assets directory!
	exit 3
}
[[ -r levels.json ]] || {
	echo Missing levels.json!
	exit 3
}
[[ -x lifish ]] || {
	echo Missing lifish executable! Did you compile the game?
	exit 3
}

mkdir "$DIR/assets"
cp -r assets/{fonts,graphics,music,screens,sounds} "$DIR/assets"
cp levels.json $DIR
cp lifish $DIR

pushd "$DIR"

cat > run.sh <<'EOF'
#!/bin/bash

root="$(dirname $(readlink -f $0))"
[[ -x "$root/lifish" ]] || chmod +x "$root/lifish"
LD_LIBRARY_PATH="$root/libs:$LD_LIBRARY_PATH" "$root/lifish"
EOF

chmod +x run.sh

cat > README.txt <<'EOF'
------------------------------------
BOOM Remake by silverweed
https://github.com/silverweed/lifish
------------------------------------

To run the game:

- open a terminal
- ./run.sh
EOF

mkdir libs
ldd lifish | awk 'NF > 2 { print $3 }' | while read LIB; do
	cp $LIB libs/
done

popd

echo "Created app in $DIR."
echo Compressing...

rm -f "$DIR.tar.xz" 2>/dev/null
tar cf "$DIR.tar" "$DIR"
xz "$DIR.tar"

rm -rf "$DIR"
echo Done.
