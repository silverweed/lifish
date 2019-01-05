#include "CutscenePlayer.hpp"
#include "BaseEventHandler.hpp"
#include "Sprite.hpp"
#include "FreeMoving.hpp"
#include "utils.hpp"
#include "game.hpp"
#include <algorithm>

using lif::CutscenePlayer;

static const sf::Time FADE_TIME = sf::seconds(1);

CutscenePlayer::CutscenePlayer()
	: lif::WindowContext()
	, curSubtitle(lif::getAsset("fonts", lif::fonts::CUTSCENES), "")
{
	_addHandler<lif::BaseEventHandler>();
}

void CutscenePlayer::reset() {
	cutscenes.clear();
	curCutsceneIdx = -1;
	_resetCutscene();
}

void CutscenePlayer::play() {
	_startFadeIn();
	_nextCutscene();
}

void CutscenePlayer::update() {
	if (_cutsceneEnded()) {
		if (static_cast<unsigned>(curCutsceneIdx) == cutscenes.size() - 1) {
			// Fade out before terminating
			if (fadingIn)
				_startFadeOut();
			if (fadeClock.getElapsedTime() < FADE_TIME) {
				// Still fading out, wait
				return;
			}
		}
		_nextCutscene();
		return;
	}
	entities.updateAll();

	if (curSubtitleIdx > -1 && subtitleClock.getElapsedTime() > curSubtitleTime)
		_nextSubtitle();
}

void CutscenePlayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	entities.apply([&target, states] (const lif::Entity& e) {
		target.draw(*e.get<lif::Sprite>(), states);
	});
	target.draw(curSubtitle, states);
	_applyFade(target, states);
}

void CutscenePlayer::_resetCutscene() {
	entities.clear();
	curSubtitleIdx = -1;
}

void CutscenePlayer::_nextCutscene() {
	if (static_cast<unsigned>(++curCutsceneIdx) == cutscenes.size()) {
		end();
		return;
	}
	_resetCutscene();
	curCutscene = &cutscenes[curCutsceneIdx];
	_initCutscene();
	fadeSprite.setSize(sf::Vector2f(curCutscene->windowSize));
}

void CutscenePlayer::_initCutscene() {
	if (curCutscene == nullptr)
		throw std::out_of_range("Called _initCutscene on null cutscene!");

	std::vector<lif::Cutscene::Layer> layers(curCutscene->layers);
	// sort layers by inverse distance (farthest first)
	std::sort(layers.begin(), layers.end(), [] (const auto& a, const auto& b) {
		return std::get<1>(a) > std::get<1>(b);
	});

	// Fill EntityGroup with Sprites
	const sf::Vector2f left(-1, 0);
	const auto calc_speed = [] (unsigned distance) { return distance > 0 ? 200. / distance : 0; };
	for (const auto& layer : layers) {
		auto ent = new lif::Entity(sf::Vector2f(0, 0));
		auto sprite = std::make_shared<lif::Sprite>(*ent, std::get<0>(layer), sf::IntRect(
					0, 0,
					static_cast<unsigned>(curCutscene->windowSize.x * 100),
					curCutscene->windowSize.y));
		sprite->getTexture()->setRepeated(true);
		ent->addComponent(sprite);
		ent->addComponent<lif::FreeMoving>(*ent, calc_speed(std::get<1>(layer)), left);
		entities.add(ent);
	}

	curSubtitle.setCharacterSize(15);
	if (curCutscene->subtitles.size() > 0)
		_nextSubtitle();
}

void CutscenePlayer::_nextSubtitle() {
	if (curCutscene == nullptr)
		throw std::out_of_range("Called _nextSubtitle on null cutscene!");

	if (static_cast<unsigned>(++curSubtitleIdx) == curCutscene->subtitles.size()) {
		curSubtitle.setString("");
		curSubtitleIdx = -1;
		return;
	}
	const auto& nxtsub = curCutscene->subtitles[curSubtitleIdx];
	curSubtitle.setString(nxtsub.first);
	const auto bounds = curSubtitle.getGlobalBounds();
	curSubtitle.setPosition(sf::Vector2f(
		lif::centerX(bounds, sf::IntRect(0, 0, curCutscene->windowSize.x, curCutscene->windowSize.y)),
		curCutscene->windowSize.y - bounds.height - 15));
	curSubtitleTime = nxtsub.second;
	subtitleClock.restart();
}

void CutscenePlayer::_startFadeIn() {
	fadeClock.restart();
	fadingIn = true;
}

void CutscenePlayer::_startFadeOut() {
	fadeClock.restart();
	fadingIn = false;
}

void CutscenePlayer::_applyFade(sf::RenderTarget& target, sf::RenderStates states) const {
	// Apply fade-in / fade-out
	const auto alpha = std::min(255.f, 255 * (fadingIn
		? (1.f - fadeClock.getElapsedTime() / FADE_TIME)
		: fadeClock.getElapsedTime() / FADE_TIME));
	if (alpha > 0) {
		fadeSprite.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(alpha)));
		target.draw(fadeSprite, states);
	}
}

bool CutscenePlayer::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		end();
		return true;
	default:
		break;
	}
	return false;
}

void CutscenePlayer::end() {
	newContext = _newContext;
}
