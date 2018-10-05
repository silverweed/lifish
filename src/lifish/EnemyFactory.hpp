#pragma once

#include "Enemy.hpp"
#include <SFML/System.hpp>
#include <memory>

namespace lif {

class LevelManager;

class EnemyFactory final : private sf::NonCopyable {
public:
	static std::unique_ptr<lif::Enemy> create(const lif::LevelManager& ls, unsigned id, const sf::Vector2f& pos);
};

}
