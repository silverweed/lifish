#include "Entity.hpp"
#include "GameCache.hpp"
#include "core.hpp"
#include "Component.hpp"
#include "utils.hpp"
#include <sstream>

using Game::Entity;

Entity::Entity() {}

Entity::Entity(const sf::Vector2f& pos)
	: position(pos)
{}

Entity::~Entity() {}

void Entity::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);
	for (auto& c : components)
		c->setOrigin(origin);
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return int(position.x) % Game::TILE_SIZE == 0;
		case 'y': return int(position.y) % Game::TILE_SIZE == 0;
	}
	return int(position.x) % Game::TILE_SIZE == 0 
		&& int(position.y) % Game::TILE_SIZE == 0;
}

Game::Entity* Entity::init() {
	if (_initialized) return this;

	for (auto& c : components)
		c->init();
	
	_initialized = true;
	return this;
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
	put_indent(indent) << "[Entity @ " << position << " / " << Game::tile(position) 
		<< " ~ aligned = " << isAligned() << "]";
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

sf::Vector2f Entity::getPosition() const { 
	return position;
} 

void Entity::setPosition(const sf::Vector2f& p) { 
	position = p;
}

void Entity::translate(const sf::Vector2f& offset) {
	setPosition(getPosition() + offset);
}

std::string Entity::toString() const {
	return _toString(0);
}
