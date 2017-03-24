#include "SidePanel.hpp"
#include "GameCache.hpp"
#include "LevelManager.hpp"
#include "Bonusable.hpp"
#include "ShadedText.hpp"
#include "Player.hpp"
#include "Lifed.hpp"
#include <iomanip>

using lif::SidePanel;

SidePanel::SidePanel(const lif::LevelManager& lm)
	: lm(lm)
{
	// Load background
	bgTexture = lif::cache.loadTexture(lif::getAsset("graphics", "panel.png"));
	backgroundSprite.setTexture(*bgTexture);
	backgroundSprite.setTextureRect(sf::IntRect(0, 0, lif::SIDE_PANEL_WIDTH, lif::SIDE_PANEL_HEIGHT));	
	backgroundSprite.setPosition(0, 0);

	// Load player heads
	playerHeadsTexture = lif::cache.loadTexture(lif::getAsset("graphics", "playerheads.png"));
	for (unsigned i = 0; i < playerHeadsSprite.size(); ++i) {
		playerHeadsSprite[i].setTexture(*playerHeadsTexture);
		playerHeadsSprite[i].setTextureRect(sf::IntRect(PLAYER_HEAD_WIDTH * i, 0,
					PLAYER_HEAD_WIDTH, PLAYER_HEAD_HEIGHT));
	}
	playerHeadsSprite[0].setPosition(16, 59);
	playerHeadsSprite[1].setPosition(16, 268);

	// Load health symbols
	healthTexture = lif::cache.loadTexture(lif::getAsset("graphics", "health.png"));
	healthTexture->setSmooth(true);
	for (unsigned i = 0; i < healthSprite.size(); ++i) {
		healthSprite[i].setTexture(*healthTexture);
		healthSprite[i].setTextureRect(sf::IntRect(HEALTH_SYM_WIDTH * i, 0,
					HEALTH_SYM_WIDTH, HEALTH_SYM_HEIGHT));
	}

	// Load EXTRA letters
	extraLettersTexture = lif::cache.loadTexture(lif::getAsset("test", "extra_icons.png"));
	extraLettersTexture->setSmooth(true);
	for (unsigned i = 0; i < extraLettersSprite.size(); ++i) {
		extraLettersSprite[i].setTexture(*extraLettersTexture);
		extraLettersSprite[i].setTextureRect(sf::IntRect(EXTRA_LETTERS_WIDTH * i, 0,
					EXTRA_LETTERS_WIDTH, EXTRA_LETTERS_HEIGHT));
	}

	// Load bonuses
	const auto bonusesTexture = lif::cache.loadTexture(lif::getAsset("graphics", "bonuses.png"));
	for (unsigned i = 0; i < bonusesSprite.size(); ++i) {
		sf::Vector2f pos(BONUS_ICON_POS_X, i == 0 ? BONUS_ICON_POS_Y_1 : BONUS_ICON_POS_Y_2);
		for (unsigned j = 0; j < bonusesSprite[i].size(); ++j) {
			bonusesSprite[i][j].setTexture(*bonusesTexture);
			bonusesSprite[i][j].setTextureRect(sf::IntRect(
						j * TILE_SIZE,
						(j / 10) * TILE_SIZE,
						TILE_SIZE, TILE_SIZE));
			bonusesSprite[i][j].setScale(
					static_cast<float>(BONUS_ICON_WIDTH) / TILE_SIZE,
					static_cast<float>(BONUS_ICON_HEIGHT) / TILE_SIZE);
			bonusesSprite[i][j].setPosition(pos);
			bonusesSprite[i][j].setColor(DISABLED_COLOR);
			pos.x += BONUS_ICON_WIDTH;
			if (j != 0 && j % 10 == 0)
				pos.y += BONUS_ICON_HEIGHT;
		}
		bonusesSprite[i][bonusesSprite[i].size() - 2].setPosition(sf::Vector2f(
					pos.x - 3 * BONUS_ICON_WIDTH, pos.y + 2 * BONUS_ICON_HEIGHT));
		bonusesSprite[i][bonusesSprite[i].size() - 1].setPosition(sf::Vector2f(
					pos.x - 3 * BONUS_ICON_WIDTH, pos.y + BONUS_ICON_HEIGHT));
	}
}

static void _drawWithShadow(sf::RenderTarget& window, sf::RenderStates states, const sf::Sprite& sprite) {
	sf::Sprite shadow(sprite);
	shadow.setColor(sf::Color(0, 0, 0, 200));
	shadow.setPosition(sprite.getPosition() + sf::Vector2f(3, 2));
	window.draw(shadow, states);
	window.draw(sprite, states);
}

void SidePanel::_drawHealthSprites(sf::RenderTarget& window, sf::RenderStates states,
		const lif::Player& player) const
{
	const auto lifed = player.get<lif::Lifed>();
	const unsigned short n_tot = lifed->getMaxLife() / 2;
	const unsigned short n_full = lifed->getLife() / 2;
	const unsigned short n_half = lifed->getLife() % 2;

	sf::Vector2f pos(HEALTH_SYM_POS_X, player.getInfo().id == 1 ? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2);
	for (unsigned j = 0; j < n_tot; ++j) {
		const auto& hs = healthSprite[j < n_full 
						? HEALTH_FULL : j < n_full + n_half
						? HEALTH_HALF : HEALTH_EMPTY];
		sf::Sprite sprite(*healthTexture, hs.getTextureRect());
		if (j < n_tot / 2)
			sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * j, 0));
		else
			sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * (j - n_tot/2),
						HEALTH_SYM_HEIGHT));
		_drawWithShadow(window, states, sprite);
	}
}

void SidePanel::_drawExtraLetters(sf::RenderTarget& window, sf::RenderStates states,
		const lif::Player& player) const
{
	sf::Vector2f pos(EXTRA_LETTERS_POS_X, player.getInfo().id == 1
			? EXTRA_LETTERS_POS_Y_1 : EXTRA_LETTERS_POS_Y_2);
	for (unsigned j = 0; j < player.getInfo().extra.size(); ++j) {
		const unsigned short i = player.getInfo().extra[j] ? j + 1 : 0;
		sf::Sprite sprite(extraLettersTexture[i], extraLettersSprite[i].getTextureRect());
		sprite.setPosition(pos + sf::Vector2f(j * EXTRA_LETTERS_WIDTH, 0));
		_drawWithShadow(window, states, sprite);
	}
}

void SidePanel::_drawTime(sf::RenderTarget& window, sf::RenderStates states) const {
	short seconds = static_cast<short>(lm.getLevelTime().getRemainingTime().asSeconds());
	const short minutes = seconds < 0 ? 0 : seconds / 60;
	std::stringstream ss;
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

	lif::ShadedText timeText(
			lif::getAsset("fonts", lif::fonts::SIDE_PANEL_MONO),
			ss.str(), TIME_POS);
	timeText.setCharacterSize(16);
	timeText.setShadowSpacing(2, 2);
	if (minutes < 1 && seconds <= 30) {
		timeText.setColor(sf::Color(220, 0, 0, 255), sf::Color::Black);
		timeText.setStyle(sf::Text::Bold);
	}
	window.draw(timeText, states);
}

void SidePanel::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(backgroundSprite, states);
	for (unsigned i = 0; i < playerHeadsSprite.size(); ++i) {
		_drawWithShadow(window, states, playerHeadsSprite[i]);
		
		// Draw remaining lives
		sf::Vector2f pos(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2); 
		std::stringstream ss;
		const auto player = lm.getPlayer(i + 1);
		if (player == nullptr)
			ss << "X0";
		else {
			_drawExtraLetters(window, states, *player);
			ss << "X" << player->getInfo().remainingLives;
		}

		lif::ShadedText text(lif::getAsset("fonts", lif::fonts::SIDE_PANEL_MONO), ss.str(), pos);
		text.setCharacterSize(20);
		text.setStyle(sf::Text::Bold);
		text.setShadowSpacing(2, 2);
		window.draw(text, states);


		// Draw health / game over
		if (player == nullptr) {
			text.setPosition(sf::Vector2f(HEALTH_SYM_POS_X, i == 0 
						? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2));
			text.setCharacterSize(HEALTH_SYM_HEIGHT);
			text.setString("GAME\nOVER");
			window.draw(text, states);
		} else {
			_drawHealthSprites(window, states, *player);

			// Draw max bombs
			const auto powers = player->getInfo().powers;
			pos.x = BONUS_ICON_POS_X;
			pos.y = i == 0 ? BONUS_ICON_POS_Y_1 : BONUS_ICON_POS_Y_2;
			text.setStyle(sf::Text::Regular);
			text.setPosition(sf::Vector2f(pos.x, pos.y + BONUS_ICON_HEIGHT + 2));
			text.setCharacterSize(11);
			text.setShadowSpacing(1, 1);
			text.setString("x" + lif::to_string(powers.maxBombs));
			window.draw(text, states);

			// Draw bomb radius
			text.setPosition(sf::Vector2f(pos.x + 2 * BONUS_ICON_WIDTH, pos.y + BONUS_ICON_HEIGHT + 2));
			text.setString("x" + lif::to_string(powers.bombRadius));
			window.draw(text, states);

			// Draw bonuses
			for (const auto& bsprite : bonusesSprite[i])
				_drawWithShadow(window, states, bsprite);

			// Draw score
			ss.str("");
			ss << std::setfill('0') << std::setw(7) << lm.getScore(i + 1);
			pos.x = SCORE_POS_X;
			pos.y = i == 0 ? SCORE_POS_Y_1 : SCORE_POS_Y_2;
			lif::ShadedText scoreText(
					lif::getAsset("fonts", lif::fonts::SIDE_PANEL_MONO),
					ss.str(), pos);
			scoreText.setCharacterSize(16);
			scoreText.setShadowSpacing(2, 2);
			window.draw(scoreText, states);
		}
	}

	_drawTime(window, states);
}

void SidePanel::update() {
	std::stringstream ss;
	for (unsigned i = 0; i < playerHeadsSprite.size(); ++i) {
		const auto player = lm.getPlayer(i + 1);

		if (player != nullptr) {
			// Update bonuses
			const auto powers = player->getInfo().powers;
			const auto bonusable = player->get<lif::Bonusable>();
			for (unsigned j = 0; j < bonusesSprite[i].size(); ++j) {
				switch (lif::BonusType(j)) {
					using B = lif::BonusType;
				case B::QUICK_FUSE:
					bonusesSprite[i][j].setColor(
							powers.bombFuseTime == lif::conf::bomb::DEFAULT_FUSE
							? DISABLED_COLOR : sf::Color::White);
					break;
				case B::SHIELD:
				case B::SPEEDY:
					bonusesSprite[i][j].setColor(bonusable->hasBonus(
								static_cast<lif::BonusType>(j))
							? sf::Color::White : DISABLED_COLOR);
					break;
				default:
					break;
				}
			}
		}
	}
}
