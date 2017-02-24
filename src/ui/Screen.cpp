#include "Screen.hpp"
#include "ScreenBuilder.hpp"
#include "Interactable.hpp"
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
	if (usingJoystick)
		_updateSelectedJoystick();
	else
		_updateSelectedMouse();
}

bool Screen::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		switch (event.joystickButton.button) {
			// TODO
			usingJoystick = true;
			return true;
		}
	case sf::Event::MouseMoved:
		usingJoystick = false;
		return false;
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
	
}
