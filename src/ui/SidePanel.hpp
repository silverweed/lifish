#pragma once

#include "conf/bonus.hpp"
#include "conf/player.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <array>

namespace lif {

class LevelManager;
class Player;

/**
 * The side panel with all game information:
 * - Number of remaining lives
 * - Remaining health
 * - EXTRA letters
 * - Bonuses
 * - Score
 * - Time left
 * A side panel always contains info about max 2 players,
 * independently from lif::MAX_PLAYERS.
 */
class SidePanel final : public sf::Drawable, private sf::NonCopyable {
	// Elements' coordinates (in pixel)
	constexpr static unsigned PLAYER_HEAD_WIDTH = 32,
	                          PLAYER_HEAD_HEIGHT = 23,
	                          PLAYER1_HEAD_X = 16,
	                          PLAYER1_HEAD_Y = 59,
	                          PLAYER2_HEAD_Y = 268,
	                          N_LIVES_X = 55,
	                          N_LIVES_Y_1 = 60,
	                          N_LIVES_Y_2 = 268,
	                          HEALTH_SYM_WIDTH = 17,
	                          HEALTH_SYM_HEIGHT = 18,
	                          HEALTH_SYM_POS_X = 15,
	                          HEALTH_SYM_POS_Y_1 = 91,
	                          HEALTH_SYM_POS_Y_2 = 299,
	                          GAME_OVER_POS_X = 16,
	                          EXTRA_LETTERS_WIDTH = 14,
	                          EXTRA_LETTERS_HEIGHT = 15,
	                          EXTRA_LETTERS_POS_X = 12,
	                          EXTRA_LETTERS_POS_Y_1 = 131,
	                          EXTRA_LETTERS_POS_Y_2 = 339,
	                          BONUS_ICON_WIDTH = 14,
	                          BONUS_ICON_HEIGHT = 13,
	                          BONUS_ICON_POS_X = 12,
	                          BONUS_ICON_POS_Y_1 = 150,
	                          BONUS_ICON_POS_Y_2 = 358,
	                          SCORE_POS_X = 12,
	                          SCORE_POS_Y_1 = 195,
	                          SCORE_POS_Y_2 = 403;
	const sf::Color DISABLED_COLOR = sf::Color(100, 100, 100, 255);
	const sf::Vector2f TIME_POS = sf::Vector2f(21, 232);

	/** The LevelManager this panel is connected with */
	const lif::LevelManager& lm;

	/** The background image */
	sf::Sprite backgroundSprite;
	sf::Texture *bgTexture;

	/** The players' heads sprites */
	std::array<sf::Sprite, 2> playerHeadsSprite;
	sf::Texture *playerHeadsTexture;

	/** The health symbols' sprites: empty, half, full */
	enum {
		HEALTH_EMPTY = 0,
		HEALTH_HALF  = 1,
		HEALTH_FULL  = 2
	};
	std::array<sf::Sprite, 3> healthSprite;
	sf::Texture *healthTexture;

	/** The EXTRA letters icons: (empty, E, X, T, R, A) */
	std::array<sf::Sprite, lif::conf::player::N_EXTRA_LETTERS + 1> extraLettersSprite;
	sf::Texture *extraLettersTexture;

	/** The Bonus icons */
	Matrix<sf::Sprite, lif::MAX_PLAYERS, lif::conf::bonus::N_PERMANENT_BONUS_TYPES> bonusesSprite;

	void _drawHealthSprites(sf::RenderTarget& window, sf::RenderStates states,
			const lif::Player& player) const;
	void _drawExtraLetters(sf::RenderTarget& window, sf::RenderStates states,
			const lif::Player& player) const;
	/** Draws the time remaining in format MM:SS */
	void _drawTime(sf::RenderTarget& window, sf::RenderStates states) const;
public:
	explicit SidePanel(const lif::LevelManager& lm);

	void update();
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
};

}
