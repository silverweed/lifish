#pragma once

namespace Game {

class Tile {
public:
	enum Type {
		EMPTY,
		BREAKABLE,
		UNBREAKABLE
	};
protected:
	Type type;

public:
	Tile(const Tile::Type type = EMPTY);

	Type getType() const;
};

}
