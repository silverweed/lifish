#pragma once

#include <ostream>
#include <SFML/Graphics.hpp>

namespace Game {

class Tile {
public:
	enum Type : unsigned char {
		EMPTY = '0',
		BREAKABLE = '1',
		UNBREAKABLE = '2'
	};
protected:
	Type type;
	sf::Sprite *sprite = nullptr;

public:
	Tile(const Tile::Type type = Type::EMPTY);

	Type getType() const;
	void setType(const Type _type) { type = _type; }

	virtual sf::Sprite* getSprite() { return sprite; }
};

}
