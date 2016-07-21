#include "Sprite.hpp"
#include "GameCache.hpp"
#include "game.hpp"

using Game::Sprite;

Sprite::Sprite(Game::Entity *const owner, const std::string& texture_name) 
	: Game::Component(owner)
{
	texture = Game::cache.loadTexture(texture_name);
	sprite.setTexture(*texture);
}

Sprite::Sprite(Game::Entity *const owner, const std::string& texture_name,
		const sf::IntRect& division) 
	: Sprite(owner, texture_name)
{
	sprite.setTextureRect(division);
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(sprite, states);
}

void Sprite::update() {
	Game::Component::update();
	sprite.setPosition(owner->getPosition());
}
