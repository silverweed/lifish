#pragma once

#include "Action.hpp"
#include "ScreenStyle.hpp"
#include "TransitionGraph.hpp"
#include "WindowContext.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace lif {

class ShadedText;

namespace ui {

class ScreenBuilder;
class Interactable;

class Screen : public lif::WindowContext {
public:
	using Callback = std::function<lif::ui::Action()>;

	enum class SelectMethod {
		NONE,
		MOUSE,
		KEYBOARD,
		JOYSTICK
	};

private:
	friend class lif::ui::ScreenBuilder;

	sf::Vector2i latestMousePos;
	bool wasActionTriggered = false;
	lif::ui::Action actionTriggered = lif::ui::Action::DO_NOTHING;
	float itrScale = 0;
	sf::Clock itrScaleClock;
	bool itrScaleMid = false;
	// Note: these texts are owned by either `interactables` or `nonInteractables`
	std::unordered_map<std::string, lif::ShadedText*> dynamicTexts;

	void _saveMousePos(int x, int y);
	void _setItrScale(float x);
	void _updateItrScale();
	void _select(lif::ui::Interactable& elem);
	void _deselect();

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

	std::string builtWithLayout = "";

	SelectMethod lastSelectionMethod = SelectMethod::NONE;

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

	/** The elements transition graph, used when navigating via joystick */
	lif::ui::TransitionGraph transitions;


	Screen(const sf::RenderWindow& window, const sf::Vector2u& size);
	void _loadBGSprite(const std::string& bgSpritePath);
	void _updateSelectedMouse();
	void _updateSelectedKeyboard(sf::Event::KeyEvent key);
	void _updateSelectedJoystick();
	/** Manually triggers an Action, as if an interactable was clicked. Useful for
	 *  raising actions upon keyboard shortcuts and similar.
	 */
	void _triggerAction(lif::ui::Action action);

	virtual void clear();

public:
	explicit Screen(const std::string& layoutFileName, const sf::RenderWindow& window, 
	                const sf::Vector2u& size, ScreenBuilder& builder);

	bool wasBuilt() const { return builtWithLayout.length() > 0; }
	const std::string& getBuildLayout() const { return builtWithLayout; }
	virtual void rebuild(ScreenBuilder& builder);

	void setParent(const std::string& name) { parent = name; }
	const std::string& getParent() const { return parent; }
	const std::string& getName() const { return name; }

	/** @return The name of the selected element, if any (else "") */
	std::string getSelected() const;

	void updateDynamicText(const std::string& name, const std::string& value);

	SelectMethod getLastSelectMethod() const { return lastSelectionMethod; }

	bool hasCallback(const std::string& name) const;
	lif::ui::Action fireCallback(const std::string& name);

	/** @return Whether we have triggered an action or not. Upon this query, the action trigger resets. */
	bool pollTriggeredAction() { bool t = wasActionTriggered; return wasActionTriggered = false, t; }
	lif::ui::Action getTriggeredAction() const { return actionTriggered; }

	/** This may be used by child classes to do specific logic;
	 *  @return true if signal was caught and should be ignored by UI's event loop.
	 */
	virtual bool handleEvent(sf::Window&, sf::Event) override;
	virtual void update() override;
	/** Called whenever the screen is displayed */
	virtual void onLoad();
	virtual void onUnload() {}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setOrigin(const sf::Vector2f& pos) override;
};

}

}
