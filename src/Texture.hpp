#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Entity.hpp"

namespace Game {

/**
 * A component holding a texture. Mainly used for composition of sprites and animations.
 */
class Texture : public Game::Component {
protected:
	sf::Texture *texture;
public:
	explicit Texture(Game::Entity *const entity, const std::string& texture_name);

	sf::Texture* getTexture() const { return texture; }
};

}
