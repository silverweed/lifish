#include "SidePanel.hpp"
#include "GameCache.hpp"
#include "LevelRenderer.hpp"
#include <iomanip>

using Game::SidePanel;

SidePanel::SidePanel(const Game::LevelRenderer *const _lr) : lr(_lr) {
	// Load background
	Game::cache.loadTexture(bgTexture, Game::getAsset("test", "panel.png"));
	backgroundSprite.setTexture(bgTexture);
	backgroundSprite.setTextureRect(sf::IntRect(0, 0, Game::SIDE_PANEL_WIDTH, Game::SIDE_PANEL_HEIGHT));	
	backgroundSprite.setPosition(0, 0);

	// Load player heads
	Game::cache.loadTexture(playerHeadsTexture, Game::getAsset("graphics", "playerheads.png"));
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		playerHeadsSprite[i].setTexture(playerHeadsTexture);
		playerHeadsSprite[i].setTextureRect(sf::IntRect(PLAYER_HEAD_WIDTH * i, 0,
					PLAYER_HEAD_WIDTH, PLAYER_HEAD_HEIGHT));
	}
	playerHeadsSprite[0].setPosition(16, 59);
	playerHeadsSprite[1].setPosition(16, 268);

	// Load health symbols
	Game::cache.loadTexture(healthTexture, Game::getAsset("graphics", "health.png"));
	healthTexture.setSmooth(true);
	for (unsigned short i = 0; i < healthSprite.size(); ++i) {
		healthSprite[i].setTexture(healthTexture);
		healthSprite[i].setTextureRect(sf::IntRect(HEALTH_SYM_WIDTH * i, 0,
					HEALTH_SYM_WIDTH, HEALTH_SYM_HEIGHT));
	}

	// Load EXTRA letters
	Game::cache.loadTexture(extraLettersTexture, Game::getAsset("test", "extra_icons.png"));
	extraLettersTexture.setSmooth(true);
	for (unsigned short i = 0; i < extraLettersSprite.size(); ++i) {
		extraLettersSprite[i].setTexture(extraLettersTexture);
		extraLettersSprite[i].setTextureRect(sf::IntRect(EXTRA_LETTERS_WIDTH * i, 0,
					EXTRA_LETTERS_WIDTH, EXTRA_LETTERS_HEIGHT));
	}

	// Load bonuses
	Game::cache.loadTexture(bonusesTexture, Game::getAsset("graphics", "bonuses.png"));
	for (unsigned short i = 0; i < bonusesSprite.size(); ++i) {
		bonusesSprite[i].setTexture(bonusesTexture);
		bonusesSprite[i].setTextureRect(sf::IntRect(TILE_SIZE * i, (i / 10) * TILE_SIZE,
					TILE_SIZE, TILE_SIZE));
		bonusesSprite[i].setScale(float(BONUS_ICON_WIDTH) / TILE_SIZE, float(BONUS_ICON_HEIGHT) / TILE_SIZE);
	}
}

SidePanel::~SidePanel() {}

void SidePanel::draw(sf::RenderTarget& window) {
	window.draw(backgroundSprite);

	std::stringstream ss;
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		// Draw player head
		_drawWithShadow(window, playerHeadsSprite[i]);
	
		const auto player = lr->getPlayer(i + 1);
		
		// Draw remaining lives
		sf::Vector2f pos(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2); 
		ss.str("");
		if (player == nullptr)
			ss << "X0";
		else
			ss << "X" << player->getRemainingLives();

		Game::ShadedText nLivesText(
				Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO),
				ss.str(),
				sf::Vector2f(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2));
		nLivesText.setCharacterSize(20);
		nLivesText.setStyle(sf::Text::Bold);
		nLivesText.setShadowSpacing(2, 2);
		window.draw(nLivesText);


		pos.x = HEALTH_SYM_POS_X;
		pos.y = i == 0 ? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2;

		if (player == nullptr) {
			// Write 'GAME OVER' instead of health
			Game::ShadedText gameOverText(
					Game::getAsset("fonts", Game::Fonts::SIDE_PANEL),
					"GAME\nOVER",
					sf::Vector2f(HEALTH_SYM_POS_X, i == 0 
						? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2));
			gameOverText.setCharacterSize(HEALTH_SYM_HEIGHT);
			window.draw(gameOverText);
		} else {
			// Draw health
			const unsigned short n_tot = player->getMaxLife() / 2;
			const unsigned short n_full = player->getLife() / 2;
			const unsigned short n_half = player->getLife() % 2;

			for (unsigned short j = 0; j < n_tot; ++j) {
				sf::Sprite& sprite = healthSprite[j < n_full 
								? HEALTH_FULL : j < n_full + n_half
								? HEALTH_HALF : HEALTH_EMPTY];
				if (j < n_tot / 2)
					sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * j, 0));
				else
					sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * (j - n_tot/2),
								HEALTH_SYM_HEIGHT));
				_drawWithShadow(window, sprite);
			}

			// Draw EXTRA letters
			pos.x = EXTRA_LETTERS_POS_X;
			pos.y = i == 0 ? EXTRA_LETTERS_POS_Y_1 : EXTRA_LETTERS_POS_Y_2;
			for (unsigned short j = 0; j < player->extra.size(); ++j) {
				if (player->extra[j]) {
					// Has j-th letter
					extraLettersSprite[j + 1].setPosition(pos);
					_drawWithShadow(window, extraLettersSprite[j + 1]);
				} else {
					// Letter not taken yet
					extraLettersSprite[0].setPosition(pos);
					_drawWithShadow(window, extraLettersSprite[0]);
				}
				pos.x += EXTRA_LETTERS_WIDTH;
			}

			// Draw bonuses
			pos.x = BONUS_ICON_POS_X;
			pos.y = i == 0 ? BONUS_ICON_POS_Y_1 : BONUS_ICON_POS_Y_2;
			for (unsigned short j = 0; j < bonusesSprite.size(); ++j) {
				bonusesSprite[j].setPosition(pos);
				sf::Color color = sf::Color::White;
				const sf::Color disabled(100, 100, 100, 255);

				switch (j) {
					using B = Game::Bonus::Type;
				case B::MAX_BOMBS:
					{
						ss.str("");
						ss << "x" << player->powers.maxBombs;
						Game::ShadedText text(
								Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO),
								ss.str(), sf::Vector2f(pos.x, pos.y + BONUS_ICON_HEIGHT + 2));
						text.setCharacterSize(11);
						text.setShadowSpacing(1, 1);
						window.draw(text);
						break;
					}
				case B::MAX_RANGE:
					{
						ss.str("");
						ss << "x" << player->powers.bombRadius;;
						Game::ShadedText text(
								Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO),
								ss.str(), sf::Vector2f(pos.x, pos.y + BONUS_ICON_HEIGHT + 2));
						text.setCharacterSize(11);
						text.setShadowSpacing(1, 1);
						window.draw(text);
						break;
					}
					break;
				case B::QUICK_FUSE:
					if (player->powers.bombFuseTime == Game::Bomb::DEFAULT_FUSE)
						color = disabled;
					break;
				case B::SHIELD:
					if (!player->hasShield())
						color = disabled;
					break;
				case B::SPEEDY:
					if (!player->hasSpeedy())
						color = disabled;
					break;
				default:
					break;
				}
				_drawWithShadow(window, bonusesSprite[j], color);
				pos.x += BONUS_ICON_WIDTH;
				if (j != 0 && j % 10 == 0) 
					pos.y += BONUS_ICON_HEIGHT;
			}

			// Draw score
			ss.str("");
			ss << std::setfill('0') << std::setw(7) << Game::score[i];
			pos.x = SCORE_POS_X;
			pos.y = i == 0 ? SCORE_POS_Y_1 : SCORE_POS_Y_2;
			Game::ShadedText scoreText(
					Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO),
					ss.str(), pos);
			scoreText.setCharacterSize(16);
			scoreText.setShadowSpacing(2, 2);
			window.draw(scoreText);
		}
	}

	// Draw the time remaining in format MM:SS
	short seconds = lr->getLevelTime();
	const short minutes = seconds < 0 ? 0 : seconds / 60;
	ss.str("");
	if (minutes < 10)
		ss << "0";
	ss << minutes << ":";
	seconds -= minutes * 60;
	if (seconds < 10) {
		ss << "0";
		if (seconds >= 0)
			ss << seconds;
		else
			ss << "0";
	} else {
		ss << seconds;
	}

	Game::ShadedText timeText(
			Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO),
			ss.str(), TIME_POS);
	timeText.setCharacterSize(16);
	timeText.setShadowSpacing(2, 2);
	if (minutes < 1 && seconds <= 30) {
		timeText.setColor(sf::Color(220, 0, 0, 255), sf::Color::Black);
		timeText.setStyle(sf::Text::Bold);
	}
	window.draw(timeText);
}

void SidePanel::_drawWithShadow(sf::RenderTarget& window, sf::Sprite& sprite, const sf::Color& color) {
	sprite.setColor(sf::Color(0, 0, 0, 200));
	auto pos = sprite.getPosition();
	sprite.setPosition(pos + sf::Vector2f(3, 2));
	window.draw(sprite);
	sprite.setColor(color);
	sprite.setPosition(pos);
	window.draw(sprite);
}
