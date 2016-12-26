#include "HauntedStatue.hpp"
#include "Sprite.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "core.hpp"
#include "ZIndexed.hpp"
#include "Fixed.hpp"
#include "collision_layers.hpp"
#include "game_values.hpp"

using Game::HauntedStatue;

HauntedStatue::HauntedStatue(const sf::Vector2f& pos) : Game::Entity(pos) {
	addComponent(new Game::Drawable(*this,
				*addComponent(new Game::Sprite(*this, Game::getAsset("graphics", "haunted_statue.png"),
				sf::IntRect(0, 0, Game::TILE_SIZE, 2 * Game::TILE_SIZE)))));
	addComponent(new Game::Collider(*this, Game::Layers::UNBREAKABLES, 
				sf::Vector2i(Game::TILE_SIZE, Game::TILE_SIZE)));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::TALL_ENTITIES));
	addComponent(new Game::Fixed(*this));

	get<Sprite>()->setOrigin(sf::Vector2f(0, 32));
}
