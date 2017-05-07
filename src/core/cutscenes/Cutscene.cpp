#include "Cutscene.hpp"
#include "BaseEventHandler.hpp"
#include "Sprite.hpp"
#include "FreeMoving.hpp"
#include "utils.hpp"
#include <algorithm>
#include "game.hpp"

using lif::Cutscene;

Cutscene::Cutscene(const sf::Vector2u& windowSize, std::initializer_list<Layer> _layers,
		std::initializer_list<Subtitle> subtitles)
	: lif::WindowContext()
	, windowSize(windowSize)
	, subtitles(subtitles)
	, curSubtitle(lif::getAsset("fonts", lif::fonts::CUTSCENES), "")
{
	_addHandler<lif::BaseEventHandler>();

	std::vector<Layer> layers(_layers);
	// sort layers by inverse distance (farthest first)
	std::sort(layers.begin(), layers.end(), [] (const Layer& a, const Layer& b) {
		return std::get<1>(a) > std::get<1>(b);
	});
	const sf::Vector2f left(-1, 0);
	const auto calc_speed = [] (unsigned distance) { return distance > 0 ? 200. / distance : 0; };
	for (const auto& layer : layers) {
		auto ent = new lif::Entity(sf::Vector2f(0, 0));
		auto sprite = std::make_shared<lif::Sprite>(*ent, std::get<0>(layer), sf::IntRect(
					0, 0, static_cast<unsigned>(windowSize.x * 100), windowSize.y));
		sprite->getTexture()->setRepeated(true);
		ent->addComponent(sprite);
		ent->addComponent<lif::FreeMoving>(*ent, calc_speed(std::get<1>(layer)), left);
		entities.add(ent);
	}

	curSubtitle.setCharacterSize(15);
	if (subtitles.size() > 0)
		_nextSubtitle();
}

void Cutscene::update() {
	entities.updateAll();

	if (curSubtitleIdx > -1 && subtitleClock.getElapsedTime() > curSubtitleTime)
		_nextSubtitle();
}

void Cutscene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	entities.apply([&target, states] (const lif::Entity *e) {
		target.draw(*e->get<lif::Sprite>(), states);
	});
	target.draw(curSubtitle, states);
}

void Cutscene::_nextSubtitle() {
	if (++curSubtitleIdx == static_cast<signed>(subtitles.size())) {
		curSubtitle.setString("");
		curSubtitleIdx = -1;
		return;
	}
	const auto& nxtsub = subtitles[curSubtitleIdx];
	curSubtitle.setString(nxtsub.first);
	const auto bounds = curSubtitle.getGlobalBounds();
	curSubtitle.setPosition(sf::Vector2f(
				lif::centerX(bounds, sf::IntRect(0, 0, windowSize.x, windowSize.y)),
				windowSize.y - bounds.height - 15));
	curSubtitleTime = nxtsub.second;
	subtitleClock.restart();
}
