#include "SidePanel.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "LevelRenderer.hpp"

using Game::SidePanel;

SidePanel::SidePanel(const Game::LevelRenderer *const _lr) : lr(_lr) {
	// Load background
	Game::cache.loadTexture(bgTexture, Game::getAsset("test", "panel.png"));
	background.setTexture(bgTexture);
	background.setTextureRect(sf::IntRect(0, 0, SidePanel::WIDTH, SidePanel::HEIGHT));	
	background.setPosition(0, 0);

	// Load player heads
	Game::cache.loadTexture(playerHeadsTexture, Game::getAsset("test", "playerheads.png"));
	for (unsigned short i = 0; i < playerHeads.size(); ++i) {
		playerHeads[i].setTexture(playerHeadsTexture);
		playerHeads[i].setTextureRect(sf::IntRect(PLAYER_HEAD_WIDTH * i, 0,
					PLAYER_HEAD_WIDTH, PLAYER_HEAD_HEIGHT));
	}
	playerHeads[0].setPosition(16, 59);
	playerHeads[1].setPosition(16, 268);

	// Load health symbols
	Game::cache.loadTexture(healthTexture, Game::getAsset("test", "health.png"));
	for (unsigned short i = 0; i < healthSprite.size(); ++i) {
		healthSprite[i].setTexture(healthTexture);
		healthSprite[i].setTextureRect(sf::IntRect(HEALTH_SYM_X * i, 0,
					HEALTH_SYM_X, HEALTH_SYM_Y));
	}
}

SidePanel::~SidePanel() {

}

void SidePanel::draw(sf::RenderTarget& window) {
	window.draw(background);

	for (unsigned short i = 0; i < playerHeads.size(); ++i) {
		// Draw player head
		_drawWithShadow(window, playerHeads[i]);
		
		// Draw health
		sf::Vector2f pos(HEALTH_SYM_POS_X, i == 0 ? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2);
		const auto player = lr->getPlayer(i + 1);
		if (player == nullptr) {
			// Write 'GAME OVER' instead of health
			Game::ShadedText gameOverText(
					Game::getAsset("fonts", Game::Fonts::SIDE_PANEL),
					"GAME\nOVER",
					sf::Vector2f(HEALTH_SYM_POS_X, i == 0 
						? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2));
			gameOverText.setCharacterSize(HEALTH_SYM_Y);
			gameOverText.draw(window);
		} else {
			const unsigned short n_tot = player->getMaxLife() / 2;
			const unsigned short n_full = player->getLife() / 2;
			const unsigned short n_half = player->getLife() % 2;

			for (unsigned short j = 0; j < n_tot; ++j) {
				sf::Sprite& sprite = healthSprite[j < n_full 
								? HEALTH_FULL : j < n_full + n_half
								? HEALTH_HALF : HEALTH_EMPTY];
				if (j < n_tot / 2)
					sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_X - 1) * j, 0));
				else
					sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_X - 1) * (j - n_tot/2),
								HEALTH_SYM_Y));
				_drawWithShadow(window, sprite);
			}
		}
	}

	// Draw the time remaining in format MM:SS
	const short seconds = lr->getLevelTime();
	const short minutes = seconds < 0 ? 0 : seconds / 60;
	std::stringstream ss;
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
