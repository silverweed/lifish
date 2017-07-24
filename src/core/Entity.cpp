#include "Entity.hpp"
#include "GameCache.hpp"
#include "core.hpp"
#include "Component.hpp"
#include "utils.hpp"
#include <sstream>
#include <iostream>

// Note: in theory, this should check for HAVE_CXA_DEMANGLE.
// The GCC version that I'm using, though, despite being pretty recent (6.2.1),
// does not define that macro, so I just check that we're using GCC and hope
// it's not a prehistoric version.
// In case of compilation problems, just change this directive and give up
// having class names demangled in toString().
#ifdef __GNUG__
#	include <cxxabi.h>
#	include <cstdlib>

static std::string demangle(const char *name) {
	int status;
	char *res = abi::__cxa_demangle(name, NULL, NULL, &status);
	if (status == 0) {
		std::string s(res);
		free(res);
		return s;
	}
	return std::string(name);
}

#	define DEMANGLE(name) demangle(name)
#else
#	define DEMANGLE(name) name
#endif

using lif::Entity;

Entity::Entity() : Entity({ 0, 0 }) {}

Entity::Entity(const sf::Vector2f& pos)
	: position(pos)
{}

Entity::~Entity() {}

void Entity::_addUnique(lif::Component *c) {
	if (std::find(compSet.begin(), compSet.end(), c) == compSet.end()) {
		compSet.emplace_back(c);
	}
}

void Entity::setOrigin(const sf::Vector2f& origin) {
	WithOrigin::setOrigin(origin);
	for (auto c : compSet)
		c->setOrigin(origin);
}

bool Entity::isAligned(const char axis) const {
	switch (axis) {
		case 'x': return static_cast<int>(position.x) % lif::TILE_SIZE == 0;
		case 'y': return static_cast<int>(position.y) % lif::TILE_SIZE == 0;
	}
	return static_cast<int>(position.x) % lif::TILE_SIZE == 0
		&& static_cast<int>(position.y) % lif::TILE_SIZE == 0;
}

lif::Entity* Entity::init() {
	if (_initialized) return this;

	for (auto c : compSet)
		c->init();

	_initialized = true;
	return this;
}

void Entity::update() {
	for (auto c : compSet)
		if (c->isActive())
			c->update();
}

std::string Entity::_toString(int indent) const {
	std::stringstream ss;
	const auto put_indent = [&ss] (int indent) -> std::stringstream& {
		for (int i = 0; i < indent; ++i)
			ss << "    ";
		return ss;
	};
	put_indent(indent) << "[" << DEMANGLE(typeid(*this).name())
		<< " @ " << position << " / " << lif::tile(position)
		<< " ~ aligned = " << isAligned() << "]";
	if (components.size() > 0) {
		ss << "\r\n";
		put_indent(indent) << "{\r\n";
		for (auto c : compSet)
			ss << c->_toString(indent + 1) << "\r\n";
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

std::string Entity::getTypeName() const {
	std::string name = DEMANGLE(typeid(*this).name());
	const auto idx = name.find_last_of(":");
	if (idx != std::string::npos && idx < name.length() - 1)
		name = name.substr(idx + 1);
	return name;
}
