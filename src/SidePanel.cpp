#include "SidePanel.hpp"
#include "GameCache.hpp"
#include "LevelManager.hpp"
#include "Bonusable.hpp"
#include <iomanip>

using Game::SidePanel;

SidePanel::SidePanel(const Game::LevelManager& lm)
	: lm(lm)
{
	nLivesText.fill(Game::ShadedText(Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO), ""));
	gameOverText.fill(Game::ShadedText(Game::getAsset("fonts", Game::Fonts::SIDE_PANEL), ""));
	maxBombsText.fill(Game::ShadedText(Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO) ""));
	bombRadiusText.fill(Game::ShadedText(Game::getAsset("fonts", Game::Fonts::SIDE_PANEL_MONO), ""));
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		nLivesText[i].setPosition(sf::Vector2f(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2));
		nLivesText[i].setCharacterSize(20);
		nLivesText[i].setStyle(sf::Text::Bold);
		nLivesText[i].setShadowSpacing(2, 2);

		gameOverText[i].setPosition(sf::Vector2f(HEALTH_SYM_POS_X, i == 0 
					? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2));
		gameOverText[i].setCharacterSize(HEALTH_SYM_HEIGHT);
		maxBombsText[i].setCharacterSize(11);
		bombRadiusText[i].setCharacterSize(11);
		maxBombsText[i].setShadowSpacing(1, 1);
		bombRadiusText[i].setShadowSpacing(1, 1);
	}

	// Load background
	bgTexture = Game::cache.loadTexture(Game::getAsset("graphics", "panel.png"));
	backgroundSprite.setTexture(*bgTexture);
	backgroundSprite.setTextureRect(sf::IntRect(0, 0, Game::SIDE_PANEL_WIDTH, Game::SIDE_PANEL_HEIGHT));	
	backgroundSprite.setPosition(0, 0);

	// Load player heads
	playerHeadsTexture = Game::cache.loadTexture(Game::getAsset("graphics", "playerheads.png"));
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		playerHeadsSprite[i].setTexture(*playerHeadsTexture);
		playerHeadsSprite[i].setTextureRect(sf::IntRect(PLAYER_HEAD_WIDTH * i, 0,
					PLAYER_HEAD_WIDTH, PLAYER_HEAD_HEIGHT));
	}
	playerHeadsSprite[0].setPosition(16, 59);
	playerHeadsSprite[1].setPosition(16, 268);

	// Load health symbols
	healthTexture = Game::cache.loadTexture(Game::getAsset("graphics", "health.png"));
	healthTexture->setSmooth(true);
	for (unsigned short i = 0; i < healthSprite.size(); ++i) {
		healthSprite[i].setTexture(*healthTexture);
		healthSprite[i].setTextureRect(sf::IntRect(HEALTH_SYM_WIDTH * i, 0,
					HEALTH_SYM_WIDTH, HEALTH_SYM_HEIGHT));
	}

	// Load EXTRA letters
	extraLettersTexture = Game::cache.loadTexture(Game::getAsset("test", "extra_icons.png"));
	extraLettersTexture->setSmooth(true);
	for (unsigned short i = 0; i < extraLettersSprite.size(); ++i) {
		extraLettersSprite[i].setTexture(*extraLettersTexture);
		extraLettersSprite[i].setTextureRect(sf::IntRect(EXTRA_LETTERS_WIDTH * i, 0,
					EXTRA_LETTERS_WIDTH, EXTRA_LETTERS_HEIGHT));
	}

	// Load bonuses
	bonusesTexture = Game::cache.loadTexture(Game::getAsset("graphics", "bonuses.png"));
	for (unsigned short i = 0; i < bonusesSprite.size(); ++i) {
		sf::Vector2f pos(BONUS_ICON_POS_X, i == 0 ? BONUS_ICON_POS_Y_1 : BONUS_ICON_POS_Y_2);
		for (unsigned short j = 0; j < bonusesSprite[i].size(); ++j) {
			bonusesSprite[i][j].setTexture(*bonusesTexture);
			bonusesSprite[i][j].setTextureRect(sf::IntRect(TILE_SIZE * j, (j / 10) * TILE_SIZE,
						TILE_SIZE, TILE_SIZE));
			bonusesSprite[i][j].setScale(float(BONUS_ICON_WIDTH) / TILE_SIZE,
					float(BONUS_ICON_HEIGHT) / TILE_SIZE);
			bonusesSprite[i][j].setPosition(pos);
			bonusesSprite[i][j].setColor(DISABLED_COLOR);
			pos.x += BONUS_ICON_WIDTH;
			if (j != 0 && j % 10 == 0) 
				pos.y += BONUS_ICON_HEIGHT;
		}
	}
}

SidePanel::~SidePanel() {}

static void _drawWithShadow(sf::RenderTarget& window, sf::RenderStates states, const sf::Sprite& sprite) {
	sf::Sprite shadow(sprite.getTexture(), sprite.getTextureRect());
	shadow.setColor(sf::Color(0, 0, 0, 200));
	shadow.setPosition(sprite.getPosition() + sf::Vector2f(3, 2));
	window.draw(shadow, states);
	window.draw(sprite, states);
}

void SidePanel::_drawHealthSprites(sf::RenderTarget& window, sf::RenderStates states, 
		const Game::Player *player) const 
{
	const unsigned short n_tot = player->getMaxLife() / 2;
	const unsigned short n_full = player->getLife() / 2;
	const unsigned short n_half = player->getLife() % 2;

	sf::Vector2f pos(HEALTH_SYM_POS_X, player->getInfo().id == 1 ? HEALTH_SYM_POS_Y_1 : HEALTH_SYM_POS_Y_2);
	for (unsigned short j = 0; j < n_tot; ++j) {
		sf::Sprite& sprite = healthSprite[j < n_full 
						? HEALTH_FULL : j < n_full + n_half
						? HEALTH_HALF : HEALTH_EMPTY];
		if (j < n_tot / 2)
			sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * j, 0));
		else
			sprite.setPosition(pos + sf::Vector2f((HEALTH_SYM_WIDTH - 1) * (j - n_tot/2),
						HEALTH_SYM_HEIGHT));
		_drawWithShadow(window, states, sprite);
	}
}

void SidePanel::_drawExtraLetters(sf::RenderTarget& window, sf::RenderStates states,
		const Game::Player *player) const
{
	sf::Vector2f(EXTRA_LETTERS_POS_X, player->getInfo().id == 1 ? EXTRA_LETTERS_POS_Y_1 : EXTRA_LETTERS_POS_Y_2);
	for (unsigned short j = 0; j < player->getInfo().extra.size(); ++j) {
		const unsigned short i = player->getInfo().extra[j] ? j + 1 : 0;
		sf::Sprite sprite(extraLettersTexture[i].getTexture(),
				extraLettersTexture[i].getTextureRect());
		sprite.setPosition(pos);
		_drawWithShadow(window, states, sprite);
		pos.x += EXTRA_LETTERS_WIDTH;
	}
}

void SidePanel::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(backgroundSprite, states);
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		_drawWithShadow(window, states, playerHeadsSprite[i]);
		window.draw(nLivesText[i], states);
		window.draw(healthText[i], states);

		// Draw health / game over
		const auto player = lm.getPlayer(i + 1);
		if (player == nullptr)
			window.draw(gameOverText[i], states);
		else {
			_drawHealthSprites(window, states, player);
			_drawExtraLetters(window, states, player);
		}

		window.draw(maxBombsText[i], states);
		window.draw(bombRadiusText[i], states);
		for (const auto& bsprite : bonusesSprite[i])
			_drawWithShadow(window, states, bsprite);
	}
}

void SidePanel::update() {
	std::stringstream ss;
	for (unsigned short i = 0; i < playerHeadsSprite.size(); ++i) {
		const auto player = lm.getPlayer(i + 1);
		
		// Update remaining lives
		sf::Vector2f pos(N_LIVES_X, i == 0 ? N_LIVES_Y_1 : N_LIVES_Y_2); 
		ss.str("");
		if (player == nullptr)
			ss << "X0";
		else
			ss << "X" << player->getRemainingLives();

		nLivesText[i].setString(ss.str());

		if (player != nullptr) {
			// Update bonuses
			const auto powers = player->getInfo().powers;
			const auto bonusable = player->get<Game::Bonusable>();
			for (unsigned short j = 0; j < bonusesSprite[i].size(); ++j) {
				switch (j) {
					using B = Game::Bonus::Type;
				case B::MAX_BOMBS:
					ss.str("");
					ss << "x" << player->powers.maxBombs;
					maxBombsText[i].setString(ss.str());
					break;
				case B::MAX_RANGE:
					ss.str("");
					ss << "x" << player->powers.bombRadius;
					bombRadiusText[i].setString(ss.str());
					break;
				case B::QUICK_FUSE:
					bonusesSprite[i][j].setColor(powers.bombFuseTime == Game::Bomb::DEFAULT_FUSE
							? DISABLED_COLOR : sf::Color::White);
					break;
				case B::SHIELD:
				case B::SPEEDY:
					bonusesSprite[i][j].setColor(bonusable->hasBonus(
								static_cast<Game::Bonus::Type>(j))
							? sf::Color::White : DISABLED_COLOR);
					break;
				default:
					break;
				}
			}

			// TODO

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
	short seconds = short(lm.getLevel()->get<Game::LevelTime>()->getTime());
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

//void SidePanel::_drawWithShadow(sf::RenderTarget& window, sf::Sprite& sprite, const sf::Color& color) {
	//sprite.setColor(sf::Color(0, 0, 0, 200));
	//auto pos = sprite.getPosition();
	//sprite.setPosition(pos + sf::Vector2f(3, 2));
	//window.draw(sprite);
	//sprite.setColor(color);
	//sprite.setPosition(pos);
	//window.draw(sprite);
//}
