#include "Entity.hpp"
#include "GameCache.hpp"
#include "Game.hpp"
#include "Component.hpp"
#include "utils.hpp"
#include <type_traits>
#include <sstream>

using Game::Entity;

void Entity::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);

	for (auto& c : components) {
		auto o = dynamic_cast<Game::WithOrigin*>(c.get());
		if (o != nullptr)
			o->setOrigin(origin);
	}
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return (unsigned short)position.x % Game::TILE_SIZE == 0;
		case 'y': return (unsigned short)position.y % Game::TILE_SIZE == 0;
	}
	return (unsigned short)position.x % Game::TILE_SIZE == 0 
		&& (unsigned short)position.y % Game::TILE_SIZE == 0;
}

void Entity::update() {
	for (auto& c : components)
		c->update();
}

std::string Entity::_toString(unsigned short indent) const {
	std::stringstream ss;
	auto put_indent = [&ss] (unsigned short indent) -> std::stringstream& { 
		for (unsigned short i = 0; i < indent; ++i) 
			ss << "    ";
		return ss;
	};
	put_indent(indent) << "[Entity @ " << position << " / " << Game::tile(position) << " ~ aligned = " << isAligned() << "]";
	if (components.size() > 0) {
		ss << "\r\n";
		put_indent(indent) << "{\r\n";
		for (const auto& c : components) {
			ss << c->_toString(indent + 1) << "\r\n";
		}
		put_indent(indent) << "}";
	}
	return ss.str();
}
