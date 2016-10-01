#pragma once

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include "DroppingSprite.hpp"

namespace Game {

class EntityGroup;

class DroppingTextManager final : private sf::NonCopyable {
	std::shared_ptr<Game::DroppingSprite> gameOverText,
	                                      extraGameText,
	                                      hurryUpText;

public:
	enum class Text {
		GAME_OVER,
		EXTRA_GAME,
		HURRY_UP
	};

	DroppingTextManager();

	/** Adds all texts to `group` */
	void subscribe(Game::EntityGroup& group);
	void trigger(Text text);
};

}
