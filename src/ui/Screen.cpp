#include "Screen.hpp"
#include "GameCache.hpp"
#include "ShadedText.hpp"
#include "Interactable.hpp"
#include "JoystickManager.hpp"
#include "ScreenBuilder.hpp"
#include "core.hpp"
#include "utils.hpp"
#include <cassert>

using lif::ui::Screen;

Screen::Screen(const sf::RenderWindow& window, const sf::Vector2u& size)
	: window(window)
	, size(size)
{}

Screen::Screen(const std::string& layoutFileName, const sf::RenderWindow& window, 
               const sf::Vector2u& size, ScreenBuilder& builder)
	: window(window)
	, size(size)
{
	builder.build(*this, layoutFileName);
}

void Screen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(bgSprite, states);
	for (const auto& elem : nonInteractables) {
		assert(elem);
		target.draw(*elem, states);
	}
	for (const auto& inter : interactables) {
		assert(inter.second);
		target.draw(*inter.second, states);
	}
}

void Screen::update() {
	if (lif::joystick::JoystickManager::getInstance().isAnyEvtMoved() >= 0)
		lastSelectionMethod = SelectMethod::JOYSTICK;

	_updateItrScale();

	if (lastSelectionMethod == SelectMethod::JOYSTICK)
		_updateSelectedJoystick();
	else if (lastSelectionMethod == SelectMethod::MOUSE)
		_updateSelectedMouse();
}

bool Screen::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
	case sf::Event::JoystickButtonReleased:
		lastSelectionMethod = SelectMethod::JOYSTICK;
		return false;
	case sf::Event::MouseMoved:
		if (lif::sqrDistance(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), latestMousePos) > 300) {
			_saveMousePos(event.mouseMove.x, event.mouseMove.y);
			lastSelectionMethod = SelectMethod::MOUSE;
			return false;
		}
		break;
	case sf::Event::KeyPressed:
		_updateSelectedKeyboard(event.key);
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
			_deselect();
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
		_select(*selected.second);
}

void Screen::_updateSelectedKeyboard(sf::Event::KeyEvent key) {
	lif::Direction dir = lif::Direction::NONE;
	switch (key.code) {
	case sf::Keyboard::Key::Up:
		dir = lif::Direction::UP;
		break;
	case sf::Keyboard::Key::Down:
		dir = lif::Direction::DOWN;
		break;
	case sf::Keyboard::Key::Left:
		dir = lif::Direction::LEFT;
		break;
	case sf::Keyboard::Key::Right:
		dir = lif::Direction::RIGHT;
		break;
	default: break;
	}

	if (dir == lif::Direction::NONE)
		return;

	lastSelectionMethod = SelectMethod::KEYBOARD;
	
	if (selected.second == nullptr) {
		const auto name = transitions.first();
		selected = std::make_pair(name, interactables[name].get());
	} else {
		const auto nxt = transitions.get(selected.first, dir);
		_deselect();
		if (nxt != "") {
			selected = std::make_pair(nxt, interactables[nxt].get());
		}
	}
	if (selected.second != nullptr)
		_select(*selected.second);
}

static lif::Direction axis2dir(const lif::joystick::JoystickListener::Axis axis) {
	using A = lif::joystick::JoystickListener::Axis;
	using D = lif::Direction;
	switch (axis) {
	case A::L_LEFT: return D::LEFT;
	case A::L_RIGHT: return D::RIGHT;
	case A::L_DOWN: return D::DOWN;
	case A::L_UP: return D::UP;
	default: break;
	}
	return D::NONE;
}

void Screen::_updateSelectedJoystick() {
	auto& jm = lif::joystick::JoystickManager::getInstance();
	int n = -1;
	if ((n = jm.isAnyEvtMoved()) >= 0) {
		if (selected.second == nullptr) {
			const auto name = transitions.first();
			selected = std::make_pair(name, interactables[name].get());
		} else {
			const auto nxt = transitions.get(selected.first, axis2dir(jm.getListener(n).getLatestEvt()));
			_deselect();
			if (nxt != "") {
				selected = std::make_pair(nxt, interactables[nxt].get());
			}
		}
		if (selected.second != nullptr)
			_select(*selected.second);
	}
}

void Screen::_select(lif::ui::Interactable& elem) {
	elem.setColor(sf::Color::Red);
	elem.setScale(1.1f, 1.1f);
}

void Screen::_deselect() {
	selected.second->setColor(sf::Color::White);
	selected.second->setScale(1.f, 1.f);
	selected.first = "";
	selected.second = nullptr;
}

void Screen::_saveMousePos(int x, int y) {
	latestMousePos.x = x;
	latestMousePos.y = y;
}

void Screen::_triggerAction(lif::ui::Action action) {
	actionTriggered = action;
	wasActionTriggered = true;
}

void Screen::onLoad() {
	_setItrScale(0);
	itrScaleClock.restart();
	itrScaleMid = false;
}

void Screen::_setItrScale(float x) {
	itrScale = x;
	for (auto& pair : interactables) {
		auto& itr = pair.second;
		itr->setScale(1, x);
	}
}

void Screen::_updateItrScale() {
	static constexpr auto SCALE_SPEED = 25;
	if (itrScaleMid) {
		if (itrScale > 1)
			_setItrScale(std::max(1.f, itrScale - itrScaleClock.restart().asSeconds() * SCALE_SPEED));
	} else if (itrScale < 1.3f) {
		_setItrScale(std::min(1.3f, itrScale + itrScaleClock.restart().asSeconds() * SCALE_SPEED));
		if (itrScale >= 1.3f) itrScaleMid = true;
	}
}

void Screen::updateDynamicText(const std::string& name, const std::string& value) {
	auto it = dynamicTexts.find(name);
	if (it != dynamicTexts.end()) {
		assert(it->second != nullptr && "Null dynamic text entry?");
		it->second->setString(value);
	}
}

void Screen::rebuild(ScreenBuilder& builder) {
	builder.rebuild(*this);
}

void Screen::clear() {
	builtWithLayout = "";
	selected.first = "";
	selected.second = nullptr;
	nonInteractables.clear();
	transitions.clear();
	dynamicTexts.clear();
}
