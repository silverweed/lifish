#include "RexFlame.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Sprite.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "conf/boss.hpp"

using lif::RexFlame;

RexFlame::RexFlame(const sf::Vector2f& pos, const sf::Vector2f& size)
	: lif::Pond(pos, size, lif::conf::boss::rex_boss::FLAME_DAMAGE, { lif::c_layers::PLAYERS })
{
	for (unsigned i = 0; i < sprites.size(); ++i) {
		auto sprite = addComponent<lif::Sprite>(*this, lif::getAsset("graphics", "rex_flame.png"),
				sf::IntRect(i * 2 * lif::TILE_SIZE, 0, 2 * lif::TILE_SIZE, 2 * lif::TILE_SIZE));
		sprites[i] = sprite;
	}

	addComponent<lif::Temporary>(*this, [this] () {
		return durationT > lif::conf::boss::rex_boss::FLAME_DAMAGE_TIME;
	});
	addComponent<lif::Drawable>(*this, *this);
}

void RexFlame::update() {
	lif::Pond::update();

	const auto delta = lif::time.getDelta();
	spriteT += delta;
	durationT += delta;

	if (spriteT > sf::milliseconds(100)) {
		spriteT = sf::Time::Zero;
		++spriteOffset;
	}
}

void RexFlame::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (int i = 0; i < size.x / lif::TILE_SIZE - 1; ++i) {
		for (int j = 0; j < size.y / lif::TILE_SIZE - 1; ++j) {
			const auto s = sprites[((i+j) + spriteOffset) % sprites.size()];
			sf::Sprite sprite(*s->getTexture(), s->getSprite().getTextureRect());
			sprite.setPosition(position.x + i * 1 * lif::TILE_SIZE, position.y + j * 1 * lif::TILE_SIZE);
			target.draw(sprite, states);
		}
	}
}
