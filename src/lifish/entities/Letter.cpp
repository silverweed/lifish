#include "Letter.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Grabbable.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "conf/player.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <random>

using lif::Letter;
using lif::TILE_SIZE;
using lif::conf::player::N_EXTRA_LETTERS;

const sf::Time Letter::TRANSITION_DELAY = sf::milliseconds(3000);

static std::uniform_int_distribution<unsigned short> dist(0, N_EXTRA_LETTERS - 1);

unsigned short Letter::randomId() {
	return dist(lif::rng);
}

Letter::Letter(const sf::Vector2f& pos, unsigned short _id)
	: lif::Entity(pos)
	, id(_id)
{
	addComponent<lif::Scored>(*this, 100);
	addComponent<lif::Sounded>(*this,
		lif::sid("grab"), lif::getAsset("sounds", "letter_grab.ogg")
	);
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "extra_letters.png"));
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::Killable>(*this);
	addComponent<lif::Collider>(*this, [this] (lif::Collider& coll) {
		if (coll.getLayer() != lif::c_layers::PLAYERS || grabbable->isGrabbed())
			return;
		get<lif::Killable>()->kill();
		grabbable->grab();
		get<lif::Scored>()->setTarget(static_cast<const lif::Player&>(coll.getOwner()).getInfo().id);
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("grab"));

		// Give letter to player
		auto& player = static_cast<lif::Player&>(coll.getOwnerRW());
		player.setExtra(id, true);
	});
	grabbable = addComponent<lif::Grabbable>(*this);

	// Letters are indexed 0 to N_EXTRA_LETTERS - 1.
	if (id > N_EXTRA_LETTERS - 1)
		id = N_EXTRA_LETTERS - 1;

	auto& animatedSprite = animated->getSprite();

	for (unsigned i = 0; i < N_EXTRA_LETTERS; ++i) {
		auto& anim = animated->addAnimation(static_cast<StringId>(i));
		// Total different frames are 4 * N_EXTRA_LETTERS
		// (full letter + 3 transitions to next, cyclic).
		// Here, animations[i] is _5_ frames long, because it contains:
		//   (initial letter) + (3-frames transition) + (final letter)
		// where (i-th final letter) and ((i+1)-th initial letter) are the same
		// frame. This way we can tell when the letter has ended its transition:
		// that is when animations[i].isPlaying() == false.
		for (unsigned j = 0; j < 5; ++j) {
			const unsigned short idx = i * 4 + j;
			anim.addFrame(sf::IntRect(
					(idx % 10) * TILE_SIZE,
					((idx % (N_EXTRA_LETTERS * 4)) / 10)  * TILE_SIZE,
					TILE_SIZE, TILE_SIZE));
		}
		if (i == id)
			animatedSprite.setAnimation(anim);
	}

	animatedSprite.setLooped(false, false);
	animatedSprite.setFrameTime(sf::seconds(0.1));
	animatedSprite.pause();
}

void Letter::update() {
	lif::Entity::update();
	transitionT += lif::time.getDelta();

	auto& animatedSprite = animated->getSprite();
	if (!animatedSprite.isPlaying() && transitioning) {
		transitioning = false;
		id = (id + 1) % N_EXTRA_LETTERS;
		animatedSprite.setAnimation(*get<lif::Animated>()->getAnimation(static_cast<StringId>(id)));
		animatedSprite.pause();
	} else if (transitionT >= TRANSITION_DELAY) {
		transitionT = sf::Time::Zero;
		transitioning = true;
		animatedSprite.play();
	}
}
