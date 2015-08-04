#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"
#include "Game.hpp"

namespace Game {

class LevelRenderer;

/**
 * The side panel with all game information:
 * - Number of remaining lives
 * - Remaining health
 * - EXTRA letters
 * - Bonuses
 * - Score
 * - Time left
 * A side panel always contains info about max 2 players,
 * independently from Game::MAX_PLAYERS.
 */
class SidePanel final : public Game::Drawable, private sf::NonCopyable {
	constexpr static unsigned short PLAYER_HEAD_WIDTH = 32;
	constexpr static unsigned short PLAYER_HEAD_HEIGHT = 21;
	constexpr static unsigned short N_LIVES_X = 55;
	constexpr static unsigned short N_LIVES_Y_1 = 63;
	constexpr static unsigned short N_LIVES_Y_2 = 271;
	constexpr static unsigned short HEALTH_SYM_WIDTH = 17;
	constexpr static unsigned short HEALTH_SYM_HEIGHT = 18;
	constexpr static unsigned short HEALTH_SYM_POS_X = 15;
	constexpr static unsigned short HEALTH_SYM_POS_Y_1 = 91;
	constexpr static unsigned short HEALTH_SYM_POS_Y_2 = 299;
	constexpr static unsigned short EXTRA_LETTERS_WIDTH = 14;
	constexpr static unsigned short EXTRA_LETTERS_HEIGHT = 15;
	constexpr static unsigned short EXTRA_LETTERS_POS_X = 12;
	constexpr static unsigned short EXTRA_LETTERS_POS_Y_1 = 131;
	constexpr static unsigned short EXTRA_LETTERS_POS_Y_2 = 339;
	const sf::Vector2f TIME_POS = sf::Vector2f(21, 233);

	/** The LevelRenderer this panel is connected with */
	const Game::LevelRenderer *const lr;

	/** The background image */
	sf::Sprite backgroundSprite;
	sf::Texture bgTexture;

	/** The players' heads sprites */
	std::array<sf::Sprite, 2> playerHeadsSprite;
	sf::Texture playerHeadsTexture;

	/** The health symbols' sprites: empty, half, full */
	enum : unsigned short {
		HEALTH_EMPTY = 0,
		HEALTH_HALF  = 1,
		HEALTH_FULL  = 2
	};
	std::array<sf::Sprite, 3> healthSprite;
	sf::Texture healthTexture;

	/** The EXTRA letters icons: (empty, E, X, T, R, A) */
	std::array<sf::Sprite, Game::N_EXTRA_LETTERS + 1> extraLettersSprite;
	sf::Texture extraLettersTexture;


	void _drawWithShadow(sf::RenderTarget& window, sf::Sprite& sprite);
public:
	SidePanel(const Game::LevelRenderer *const _lr);
	~SidePanel();

	void draw(sf::RenderTarget& window) override;
};

}
