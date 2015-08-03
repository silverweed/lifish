#pragma once
/**
 * The side panel with all game information:
 * - Number of remaining lives
 * - Remaining health
 * - EXTRA letters
 * - Bonuses
 * - Score
 * - Time left
 */

#include <SFML/Graphics.hpp>
#include "Drawable.hpp"

namespace Game {

class LevelRenderer;

class SidePanel final : public Game::Drawable, private sf::NonCopyable {
	constexpr static unsigned short PLAYER_HEAD_WIDTH = 32;
	constexpr static unsigned short PLAYER_HEAD_HEIGHT = 21;
	constexpr static unsigned short HEALTH_SYM_X = 17;
	constexpr static unsigned short HEALTH_SYM_Y = 18;
	constexpr static unsigned short HEALTH_SYM_POS_X = 15;
	constexpr static unsigned short HEALTH_SYM_POS_Y_1 = 91;
	constexpr static unsigned short HEALTH_SYM_POS_Y_2 = 299;
	const sf::Vector2f TIME_POS = sf::Vector2f(21, 233);

	/** The LevelRenderer this panel is connected with */
	const Game::LevelRenderer *const lr;

	/** The background image */
	sf::Sprite background;
	sf::Texture bgTexture;

	/** The players' heads sprites */
	std::array<sf::Sprite, 2> playerHeads;
	sf::Texture playerHeadsTexture;

	/** The health symbols' sprites: empty, half, full */
	enum : unsigned short {
		HEALTH_EMPTY = 0,
		HEALTH_HALF  = 1,
		HEALTH_FULL  = 2
	};
	std::array<sf::Sprite, 3> healthSprite;
	sf::Texture healthTexture;


	void _drawWithShadow(sf::RenderTarget& window, sf::Sprite& sprite);
public:
	constexpr static unsigned short WIDTH = 96;
	constexpr static unsigned short HEIGHT = 480;

	SidePanel(const Game::LevelRenderer *const _lr);
	~SidePanel();

	void draw(sf::RenderTarget& window) override;
};

}
