#include "Spikes.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "ZIndexed.hpp"
#include "Bonusable.hpp"
#include "Player.hpp"
#include "Drawable.hpp"
#include "Lifed.hpp"
#include "Killable.hpp"
#include "collision_layers.hpp"
#include "conf/zindex.hpp"
#include "conf/enemy.hpp"

using lif::Spikes;

constexpr static unsigned MIN_DAMAGING_FRAME = 4;
constexpr static unsigned MAX_DAMAGING_FRAME = 6;

Spikes::Spikes(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "spikes.png"));
	animated->addAnimation("trigger", {
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(1 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(2 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(3 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(4 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(5 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE),
		sf::IntRect(0 * lif::TILE_SIZE, 0, lif::TILE_SIZE, lif::TILE_SIZE)
	}, true);
	animated->getSprite().setLooped(false, false);
	animated->getSprite().stop();

	addComponent<lif::Collider>(*this, [this] (lif::Collider& c) {
		_maybeTrigger(c);
	}, lif::c_layers::TELEPORTS);

	addComponent<lif::Drawable>(*this, *animated);
}

void Spikes::update() {
	lif::Entity::update();

	const auto frame = animated->getSprite().getCurrentFrame();
	damaging = MIN_DAMAGING_FRAME <= frame && frame <= MAX_DAMAGING_FRAME;
}

void Spikes::_maybeTrigger(const lif::Collider& c) {
	using L = lif::c_layers::Layer;
	if (damaging) {
		switch (c.getLayer()) {
		case L::PLAYERS:
			{
				auto& player = static_cast<lif::Player&>(c.getOwnerRW());
				if (player.get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD))
					return;
				player.dealDamage(lif::conf::enemy::spikes::DAMAGE);
				break;
			}
		case L::ENEMIES:
		case L::ENEMIES_IGNORE_BREAKABLES:
		case L::BOSSES:
			{
				auto life = c.getOwner().get<lif::Lifed>();
				if (life == nullptr)
					return;
				if (life->decLife(lif::conf::enemy::spikes::DAMAGE) < 0) {
					auto killable = c.getOwner().get<lif::Killable>();
					if (killable != nullptr)
						killable->kill();
				}
				break;
			}
		default:
			break;
		}
	} else {
		auto& sprite = animated->getSprite();
		if (!sprite.isPlaying() && c.getLayer() == L::PLAYERS) {
			sprite.stop();
			sprite.play();
		}
	}
}
