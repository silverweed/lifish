#include "AbsorbFX.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "utils.hpp"

using lif::AbsorbFX;

constexpr static float SPEED = 350;

AbsorbFX::AbsorbFX(const sf::Vector2f& pos, std::weak_ptr<const lif::Entity> target)
	: lif::Entity(pos)
	, target(target)
{
	for (unsigned i = 0; i < sprites.size(); ++i) {
		sprites[i] = addComponent<lif::Sprite>(*this,
				lif::getAsset("graphics", "absorb.png"), sf::IntRect(0, 0, 16, 16));
		sprites[i]->getSprite().setScale(1 - i * 0.15, 1 - i * 0.15);
		sprites[i]->getSprite().setColor(sf::Color(255, 255, 255, 180));
		sprites[i]->setPosition(sf::Vector2f(pos.x + (i % 2) * lif::TILE_SIZE / 2.5,
		                                     pos.y + (i / 2) * lif::TILE_SIZE / 2.5));
	}
	addComponent<lif::Drawable>(*this, *this);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::FLASHES);
	addComponent<lif::Temporary>(*this, [this] () { return expired; });
}

void AbsorbFX::update() {
	lif::Entity::update();
	if (target.expired()) {
		expired = true;
		return;
	}
	const auto dt = lif::time.getDelta().asSeconds();
	const auto& trg = target.lock();
	if (lif::sqrDistance(trg->getPosition(), position) < lif::TILE_SIZE / 10.0) {
		expired = true;
		return;
	}
	const auto dir = lif::normalized(trg->getPosition() - position);
	position += dt * SPEED * dir;
	for (unsigned i = 0; i < sprites.size(); ++i) {
		sprites[i]->setPosition(sf::Vector2f(position.x + (i % 2) * lif::TILE_SIZE / 2.5,
		                                     position.y + (i / 2) * lif::TILE_SIZE / 2.5));
	}
}

void AbsorbFX::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (const auto s : sprites)
		target.draw(*s, states);
}
