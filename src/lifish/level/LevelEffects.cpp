#include "LevelEffects.hpp"
#include "Entity.hpp"
#include "LevelManager.hpp"
#include "AxisSighted.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "Fog.hpp"
#include "game.hpp"

#include <iostream>

using lif::LevelEffects;
using lif::TILE_SIZE;
using lif::LEVEL_WIDTH;
using lif::LEVEL_HEIGHT;

LevelEffects::LevelEffects() {
	darknessRenderTex.create(LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE);
}

std::set<lif::Entity*> LevelEffects::getEffectEntities(const lif::Level& level) {
	std::set<lif::Entity*> entities;

	const auto& effects = level.getInfo().effects;
	if (effects.find("fog") != effects.end()) {
		entities.insert(new lif::Fog);
	}

	return entities;
}

void LevelEffects::blendEffects(const lif::LevelManager& lm, sf::RenderTarget& window,
		const sf::Vector2f& origin) const
{
	if (darknessOn) {
		darknessRenderTex.clear(sf::Color::Black);
		// Calculate visibility rectangles for players
		for (unsigned short i = 0; i < lif::MAX_PLAYERS; ++i) {
			const auto player = lm.getPlayer(i + 1);
			std::cerr << "player " << i << std::endl;
			if (player == nullptr) continue;
			const auto pos = player->getPosition();
			const auto dy = darknessRenderTex.getSize().y;
			const auto sighted = player->get<lif::AxisSighted>();
			if (sighted == nullptr) throw;
			// distance with nearest opaque entity
			std::array<unsigned short, static_cast<std::size_t>(lif::Direction::NONE)> nearest = {
				static_cast<unsigned short>(pos.y / TILE_SIZE - 1), // up
				static_cast<unsigned short>(pos.x / TILE_SIZE - 1), // left
				static_cast<unsigned short>(LEVEL_HEIGHT - nearest[0] - 1), // down
				static_cast<unsigned short>(LEVEL_WIDTH - nearest[1] - 1),  // right
			};
			// calculate vision in all directions
			for (unsigned short i = 0; i < nearest.size(); ++i) {
				const int vision = sighted->getVision(static_cast<lif::Direction>(i));
				std::cerr << sighted << ": vision[" << static_cast<lif::Direction>(i) << "] = " << vision << std::endl;
				if (vision >= 0)
					nearest[i] = vision;
			}
			// vertical rectangle
			sf::RectangleShape rect(sf::Vector2f(
						TILE_SIZE,
						(1 + nearest[lif::UP] + nearest[lif::DOWN]) * TILE_SIZE));
			rect.setPosition(pos.x - TILE_SIZE, dy - pos.y - nearest[lif::UP]);
			darknessRenderTex.draw(rect);
			// horizontal rectangle
			rect = sf::RectangleShape(sf::Vector2f(
						(1 + nearest[lif::LEFT] + nearest[lif::RIGHT]) * TILE_SIZE,
						TILE_SIZE));
			rect.setPosition(pos.x - (1 + nearest[lif::LEFT]) * TILE_SIZE, dy - pos.y);
			darknessRenderTex.draw(rect);
		}

		sf::Sprite darkSprite(darknessRenderTex.getTexture());
		darkSprite.setPosition(lif::TILE_SIZE, lif::TILE_SIZE);
		darkSprite.setOrigin(origin);
		window.draw(darkSprite, sf::BlendMultiply);
	}
}
