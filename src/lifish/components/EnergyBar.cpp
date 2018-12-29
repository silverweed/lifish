#include "EnergyBar.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::EnergyBar;

EnergyBar::EnergyBar(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<lif::EnergyBar>();

	bar = addComponent<lif::Sprite>(*this,
			lif::getAsset("graphics", "energy_bar.png"), sf::IntRect(0, 0, 285, 20));
	emptyBar = addComponent<lif::Sprite>(*this,
			lif::getAsset("graphics", "energy_bar_empty.png"), sf::IntRect(0, 0, 285, 20));
	addComponent<lif::Drawable>(*this, *this);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::UI);
}

void EnergyBar::setEnergy(int amt) {
	bar->getSprite().setTextureRect(sf::IntRect(0, 0, 4 + amt * 7, 20));
}

void EnergyBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*emptyBar, states);
	target.draw(*bar, states);
}

void EnergyBar::update() {
	lif::Component::update();

	if (!moving) return;

	setPosition(getPosition() - sf::Vector2f(0, SPEED) * lif::time.getDelta().asSeconds());
}

void EnergyBar::moveOut() {
	moving = true;
}
