#include "Screen.hpp"
#include "ScreenBuilder.hpp"
#include "Interactable.hpp"
#include "JoystickManager.hpp"
#include "core.hpp"
#include "GameCache.hpp"

using lif::ui::Screen;

Screen::Screen(const sf::RenderWindow& window, const sf::Vector2u& size)
	: window(window)
	, size(size)
{}

Screen::Screen(const std::string& layoutFileName, const sf::RenderWindow& window, const sf::Vector2u& size)
	: window(window)
	, size(size)
{
	lif::ui::ScreenBuilder builder;
	builder.build(*this, layoutFileName);
}

void Screen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bgSprite, states);
	for (const auto& elem : nonInteractables)
		target.draw(*elem, states);
	for (const auto& inter : interactables)
		target.draw(*inter.second, states);
}

void Screen::update() {
	if (lif::joystick::JoystickManager::getInstance().isAnyEvtMoved())
		usingJoystick = true;

	if (usingJoystick)
		_updateSelectedJoystick();
	else
		_updateSelectedMouse();
}

bool Screen::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
	case sf::Event::JoystickButtonReleased:
		usingJoystick = true;
		return false;
	case sf::Event::MouseMoved:
		if (_getMouseShift(event.mouseMove.x, event.mouseMove.y) > 300) {
			_saveMousePos(event.mouseMove.x, event.mouseMove.y);
			usingJoystick = false;
			return false;
		}
		break;
	default:
		break;
	}
	return false;
}

std::string Screen::getSelected() const {
	return selected.first;
}

void Screen::setOrigin(const sf::Vector2f& pos) {
	lif::WithOrigin::setOrigin(pos);
	bgSprite.setOrigin(pos);
	for (auto& e : nonInteractables) {
		if (auto t = dynamic_cast<lif::ShadedText*>(e.get()))
			t->setOrigin(pos);
		else
			static_cast<sf::Sprite*>(e.get())->setOrigin(pos);
	}
	for (auto& e : interactables)
		e.second->setOrigin(pos);
}

bool Screen::hasCallback(const std::string& name) const {
	return callbacks.find(name) != callbacks.end();
}

lif::ui::Action Screen::fireCallback(const std::string& name) {
	return callbacks[name]();
}

void Screen::_loadBGSprite(const std::string& bgSpritePath) {
	auto texture = lif::cache.loadTexture(bgSpritePath);
	texture->setRepeated(true);
	texture->setSmooth(true);
	bgSprite.setTexture(*texture);
	bgSprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
}

void Screen::_updateSelectedMouse() {
	const auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (selected.second != nullptr) {
		// Check if we're still selecting this element; in that case, don't bother
		// looping through all others elements.
		if (selected.second->getGlobalBounds().contains(mousePos.x, mousePos.y))
			return;
		else {
			selected.second->setColor(sf::Color::White);
			selected.first = "";
			selected.second = nullptr;
		}
	}
	// We may be selecting a new interactable: in that case, find out which.
	for (auto& pair : interactables) {
		auto& inter = pair.second;
		if (inter->getGlobalBounds().contains(mousePos.x, mousePos.y)) {
			selected = std::make_pair(pair.first, inter.get());
			break;
		}
	}
	if (selected.second != nullptr)
		selected.second->setColor(sf::Color::Red);
}

void Screen::_updateSelectedJoystick() {
	// FIXME! should iterate in order
	if (lif::joystick::JoystickManager::getInstance().isAnyEvtMoved()) {
		if (selected.second == nullptr) {
select_begin:
			const auto& pair = *(interactables.begin());
			selected = std::make_pair(pair.first, pair.second.get());
		} else {
			selected.second->setColor(sf::Color::White);
			for (auto it = interactables.begin(); it != interactables.end(); ++it) {
				if (it->second.get() == selected.second) {
					auto nxt = it;
					++nxt;
					if (nxt == interactables.end())
						goto select_begin;
					else {
						selected = std::make_pair(nxt->first, nxt->second.get());
						break;
					}
				}
			}
		}
	}
	if (selected.second != nullptr)
		selected.second->setColor(sf::Color::Red);
}

int Screen::_getMouseShift(int x, int y) const {
	return (latestMouseX - x) * (latestMouseX - x) + (latestMouseY - y) * (latestMouseY - y);
}

void Screen::_saveMousePos(int x, int y) {
	latestMouseX = x;
	latestMouseY = y;
}
