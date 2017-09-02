#pragma once

#include <memory>
#include <SFML/System.hpp>
#include "Enemy.hpp"

namespace lif {

class LevelManager;

class EnemyFactory final : private sf::NonCopyable {
public:
	static std::unique_ptr<lif::Enemy> create(lif::LevelManager& ls, unsigned id, const sf::Vector2f& pos);
};

}
