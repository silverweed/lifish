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

LevelEffects::LevelEffects(const sf::Vector2u& windowSize) {
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
		halo.setPosition(ppos.x - TILE_SIZE, ppos.y - TILE_SIZE);
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

	// Flip the view to adjust coordinates
	sf::View view(sf::FloatRect(
			TILE_SIZE,
			(lif::LEVEL_HEIGHT + 1) * TILE_SIZE,
			lif::LEVEL_WIDTH * TILE_SIZE,
			-lif::LEVEL_HEIGHT * TILE_SIZE));
	darknessRenderTex.setView(view);
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
	vrect.setPosition(pos.x, pos.y - nearest[lif::UP]);
	// horizontal rectangle
	sf::RectangleShape hrect(sf::Vector2f(
				TILE_SIZE + nearest[lif::LEFT] + nearest[lif::RIGHT],
				TILE_SIZE));
	hrect.setPosition(pos.x - nearest[lif::LEFT], pos.y);

	return std::make_pair(hrect, vrect);
}

std::vector<sf::FloatRect> LevelEffects::_getRadialRectangles(const sf::Vector2f& center, unsigned radius) const {
	std::vector<sf::FloatRect> rects;
	float px = center.x,
	      py = center.y - radius * TILE_SIZE,
	      width = 1,
	      height = 2 * radius + 1;
	do {
		rects.push_back(sf::FloatRect(px, py, TILE_SIZE * width, TILE_SIZE * height));
		height -= 2;
		width += 2;
		px -= TILE_SIZE;
		py += TILE_SIZE;
	} while (height >= 1);

	return rects;
}
