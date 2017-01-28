#include "LevelEffects.hpp"
#include "Entity.hpp"
#include "LevelManager.hpp"
#include "AxisSighted.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "Fog.hpp"
#include "game.hpp"
#include "LightSource.hpp"

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
		_blendDarkness(lm, window, origin);
	}
}

void LevelEffects::_blendDarkness(const lif::LevelManager& lm, sf::RenderTarget& window,
		const sf::Vector2f& origin) const
{
	darknessRenderTex.clear(sf::Color::Black);
	const auto dy = darknessRenderTex.getSize().y;
	// Calculate visibility rectangles for players
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if (player == nullptr) continue;
		const auto rects = _getVisionRectangles(*player);
		darknessRenderTex.draw(rects.first);
		darknessRenderTex.draw(rects.second);
	}
	
	// Calculate visibility circles for light sources
	lm.getEntities().apply([this, dy, origin] (const lif::Entity *e) {
		const auto source = e->get<lif::LightSource>();
		if (source == nullptr) return;
		const float radius = source->getRadius();
		auto rects = _getRadialRectangles(e->getPosition(), radius/TILE_SIZE);

		auto h = std::get<0>(rects);
		h.setFillColor(source->getColor());
		darknessRenderTex.draw(h);

		auto v = std::get<1>(rects);
		v.setFillColor(source->getColor());
		darknessRenderTex.draw(v);

		if (radius > TILE_SIZE) {
			auto c = std::get<2>(rects);
			c.setFillColor(source->getColor());
			darknessRenderTex.draw(c);
		}
	});

	sf::Sprite darkSprite(darknessRenderTex.getTexture());
	darkSprite.setPosition(lif::TILE_SIZE, lif::TILE_SIZE);
	darkSprite.setOrigin(origin);
	window.draw(darkSprite, sf::BlendMultiply);
}

auto LevelEffects::_getVisionRectangles(const lif::Entity& e) const
		-> std::pair<sf::RectangleShape, sf::RectangleShape>
{
	const auto sighted = e.get<lif::AxisSighted>();
	if (sighted == nullptr)
		throw std::invalid_argument("Entity given to _getVisionRectangles() has no AxisSighted!");

	const auto pos = e.getPosition();
	const auto dy = darknessRenderTex.getSize().y;

	std::array<float, static_cast<std::size_t>(lif::Direction::NONE)> nearest = {{
		pos.y - TILE_SIZE, // up
		pos.x - TILE_SIZE, // left
		LEVEL_HEIGHT * TILE_SIZE - pos.y, // down
		LEVEL_WIDTH * TILE_SIZE - pos.x,  // right
	}};
	// calculate vision in all directions
	for (unsigned i = 0; i < nearest.size(); ++i) {
		const float vision = sighted->getVision(static_cast<lif::Direction>(i));
		if (vision >= 0)
			nearest[i] = vision;
	}
	// vertical rectangle
	sf::RectangleShape vrect(sf::Vector2f(
				TILE_SIZE,
				TILE_SIZE + nearest[lif::UP] + nearest[lif::DOWN]));
	vrect.setPosition(pos.x - TILE_SIZE, dy - pos.y - nearest[lif::DOWN]);
	// horizontal rectangle
	sf::RectangleShape hrect(sf::Vector2f(
				TILE_SIZE + nearest[lif::LEFT] + nearest[lif::RIGHT],
				TILE_SIZE));
	hrect.setPosition(pos.x - (TILE_SIZE + nearest[lif::LEFT]), dy - pos.y);

	return std::make_pair(hrect, vrect);
}

auto LevelEffects::_getRadialRectangles(const sf::Vector2f& center, unsigned radius) const
		-> std::tuple<sf::RectangleShape, sf::RectangleShape, sf::RectangleShape>
{
	const auto dy = darknessRenderTex.getSize().y;
	sf::RectangleShape v(static_cast<float>(TILE_SIZE) * sf::Vector2f(1, 2 * radius + 1)),
	                   h(static_cast<float>(TILE_SIZE) * sf::Vector2f(2 * radius + 1, 1)),
	                   c(static_cast<float>(TILE_SIZE) * sf::Vector2f(2 * radius - 1, 2 * radius - 1));

	v.setPosition(center.x - TILE_SIZE, dy - center.y - radius * TILE_SIZE);
	h.setPosition(center.x - (radius + 1) * TILE_SIZE, dy - center.y);
	c.setPosition(center.x - radius * TILE_SIZE, dy - center.y - (radius - 1) * TILE_SIZE);

	return std::make_tuple(h, v, c);
}
