#pragma once

#include <memory>
#include <SFML/System/NonCopyable.hpp>

namespace Game {

class DroppingSprite;
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

	/** Resets all dropping texts */
	void reset();
	/** Adds all texts to `group` */
	void subscribe(Game::EntityGroup& group);
	void trigger(Text text);
	bool isPlaying(Text text) const;
};

}
