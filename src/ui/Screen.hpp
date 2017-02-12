#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include "ScreenStyle.hpp"
#include "WithOrigin.hpp"
#include "Action.hpp"
#include "WindowContext.hpp"

namespace lif {

namespace ui {

class ScreenBuilder;
class Interactable;

class Screen : public lif::WindowContext {
public:
	using Callback = std::function<lif::ui::Action()>;

private:
	friend class lif::ui::ScreenBuilder;

protected:
	/** The window this screen is rendered in */
	const sf::RenderWindow& window;
	/** The expected size of the rendering target, used when positioning elements in this screen */
	sf::Vector2u size;
	/** The name of this screen */
	std::string name;
	/** The parent screen, if any */
	std::string parent;
	/** The background tile */
	sf::Sprite bgSprite;

	/** Whether this Screen was already built or not */
	bool built = false;
	/** If true, the elements' selection is being done via joystick, else via mouse */
	bool usingJoystick = false;

	/** The styles */
	std::unordered_map<std::string, lif::ui::ScreenStyle> styles;
	/** The interactable texts/images */
	std::unordered_map<std::string, std::unique_ptr<lif::ui::Interactable>> interactables;
	/** The non-interactable texts/images */
	std::vector<std::unique_ptr<sf::Drawable>> nonInteractables;

	/** The internal callbacks, if any. Internal callbacks have priority over external ones. */
	std::unordered_map<std::string, Callback> callbacks;

	/** The currently selected element, if any */
	std::pair<std::string, lif::ui::Interactable*> selected;


	Screen(const sf::RenderWindow& window, const sf::Vector2u& size);
	void _loadBGSprite(const std::string& bgSpritePath);
	void _updateSelectedMouse();
	void _updateSelectedJoystick();

public:
	explicit Screen(const std::string& layoutFileName, const sf::RenderWindow& window, const sf::Vector2u& size);

	bool wasBuilt() const { return built; }
	void setParent(const std::string& name) { parent = name; }
	const std::string& getParent() const { return parent; }
	const std::string& getName() const { return name; }

	/** @return The name of the selected element, if any (else "") */
	std::string getSelected() const;

	bool hasCallback(const std::string& name) const;
	lif::ui::Action fireCallback(const std::string& name);

	/** This may be used by child classes to do specific logic;
	 *  @return true if signal was caught and should be ignored by UI's event loop.
	 */
	virtual bool handleEvent(sf::Window&, sf::Event) override;
	virtual void update() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setOrigin(const sf::Vector2f& pos) override;
};

}

}
