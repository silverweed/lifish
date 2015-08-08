#include "SidePanel.hpp"
#include "GameCache.hpp"
#include "LevelRenderer.hpp"

using Game::SidePanel;

SidePanel::SidePanel(const Game::LevelRenderer *const _lr) : lr(_lr) {
	// Load background
	Game::cache.loadTexture(bgTexture, Game::getAsset("test", "panel.png"));
	backgroundSprite.setTexture(bgTexture);
	backgroundSprite.setTextureRect(sf::IntRect(0, 0, Game::SIDE_PANEL_WIDTH, Game::SIDE_PANEL_HEIGHT));	
	backgroundSprite.setPosition(0, 0);

	// Load player heads
	Game::cache.loadTexture(playerHeadsTexture, Game::getAsset("test", "playerheads.png"));
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		playerHeadsSprite[i].setTexture(playerHeadsTexture);
		playerHeadsSprite[i].setTextureRect(sf::IntRect(PLAYER_HEAD_WIDTH * i, 0,
					PLAYER_HEAD_WIDTH, PLAYER_HEAD_HEIGHT));
	}
	playerHeadsSprite[0].setPosition(16, 59);
	playerHeadsSprite[1].setPosition(16, 268);

	// Load health symbols
	Game::cache.loadTexture(healthTexture, Game::getAsset("test", "health.png"));
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
				Game::getAsset("fonts", Game::Fonts::SIDE_PANEL),
				ss.str(),
				sf::Vector2f(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2));
		nLivesText.setCharacterSize(14);
		nLivesText.draw(window);


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
			gameOverText.draw(window);
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
		}
	}

	// Draw the time remaining in format MM:SS
	const short seconds = lr->getLevelTime();
	const short minutes = seconds < 0 ? 0 : seconds / 60;
	ss.str("");
	if (minutes < 10)
		ss << "0";
	ss << minutes << ":";
	if (seconds > 9)
		ss << (seconds - minutes * 60);
	else {
		ss << "0";
		if (seconds >= 0)
			ss << seconds;
		else
			ss << "0";
	}

	Game::ShadedText timeText(
			Game::getAsset("fonts", Game::Fonts::SIDE_PANEL),
			ss.str(), TIME_POS);
	timeText.setCharacterSize(14);
	if (seconds <= 30)
		timeText.setColor(sf::Color(220, 0, 0, 255), sf::Color::Black);
	timeText.draw(window);
}

void SidePanel::_drawWithShadow(sf::RenderTarget& window, sf::Sprite& sprite) {
	sprite.setColor(sf::Color(0, 0, 0, 200));
	auto pos = sprite.getPosition();
	sprite.setPosition(pos + sf::Vector2f(3, 2));
	window.draw(sprite);
	sprite.setColor(sf::Color::White);
	sprite.setPosition(pos);
	window.draw(sprite);
}