#include "Sprite.hpp"
#include "GameCache.hpp"
#include "Game.hpp"

using Game::Sprite;

Sprite::Sprite(Game::Entity *const owner, const std::string& texture_name) 
	: Game::Component(owner)
{
	texture = addComponent(new Game::Texture(this, texture_name));
	sprite.setTexture(*texture->getTexture());
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
	sprite.setPosition(owner.getPosition());
}
