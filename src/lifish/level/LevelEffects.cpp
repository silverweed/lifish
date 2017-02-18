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

LevelEffects::LevelEffects(const sf::Vector2u& windowSize) : dy(windowSize.y) {
	darknessRenderTex.create(windowSize.x, windowSize.y);
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
	// Calculate visibility rectangles for players
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if (player == nullptr) continue;
		auto rects = _getVisionRectangles(*player);
		rects.first.setFillColor(sf::Color(255, 255, 255, 120));
		rects.second.setFillColor(sf::Color(255, 255, 255, 120));
		darknessRenderTex.draw(rects.first);
		darknessRenderTex.draw(rects.second);
		sf::RectangleShape halo(sf::Vector2f(3 * TILE_SIZE, 3 * TILE_SIZE));
		const auto ppos = player->getPosition();
		halo.setPosition(ppos.x - 2 * TILE_SIZE, _yw2s(ppos.y + TILE_SIZE));
		halo.setFillColor(sf::Color(255, 255, 255, 200));
		darknessRenderTex.draw(halo);
	}
	
	// Calculate visibility circles for light sources
	lm.getEntities().apply([this, origin] (const lif::Entity *e) {
		const auto source = e->get<lif::LightSource>();
		if (source == nullptr) return;
		const float radius = source->getRadius();
		auto rects = _getRadialRectangles(e->getPosition(), radius/TILE_SIZE);
		
		for (const auto& fr : rects) {
			sf::RectangleShape rect(sf::Vector2f(fr.width, fr.height));
			rect.setPosition(fr.left, fr.top);
			rect.setFillColor(source->getColor());
			darknessRenderTex.draw(rect);
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

	std::array<float, static_cast<std::size_t>(lif::Direction::NONE)> nearest = {{
		pos.y - TILE_SIZE, // up
		pos.x - TILE_SIZE,             // left
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
	vrect.setPosition(pos.x - TILE_SIZE, _yw2s(pos.y + nearest[lif::DOWN]));
	// horizontal rectangle
	sf::RectangleShape hrect(sf::Vector2f(
				TILE_SIZE + nearest[lif::LEFT] + nearest[lif::RIGHT],
				TILE_SIZE));
	hrect.setPosition(pos.x - TILE_SIZE - nearest[lif::LEFT], _yw2s(pos.y));

	return std::make_pair(hrect, vrect);
}

std::vector<sf::FloatRect> LevelEffects::_getRadialRectangles(const sf::Vector2f& center, unsigned radius) const {
	std::vector<sf::FloatRect> rects;
	float px = center.x - TILE_SIZE,
	      py = _yw2s(center.y - TILE_SIZE),
	      width = 1,
	      height = 2 * radius + 1;
	do {
		rects.push_back(sf::FloatRect(px, py, TILE_SIZE * width, TILE_SIZE * height));
		height += 2;
		width -= 2;
		px -= TILE_SIZE;
		py += TILE_SIZE;
	} while (height >= 1);

	return rects;
}
