#pragma once

#include "LoadScreen.hpp"
#include "WindowContext.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include <unordered_map>

namespace lif {

struct SaveData;

namespace ui {

class Screen;

/** High-level interface to UI. It's a singleton class */
class UI final : public lif::WindowContext {
	std::unordered_map<std::string, std::unique_ptr<lif::ui::Screen>> screens;
	std::unordered_map<std::string, std::string> dynamicTexts;
	lif::ui::Screen *curScreen = nullptr;
	sf::Vector2u size;
	bool quitGame = false;
	bool loadGame = false;
	bool saveGame = false;
	std::string saveName;

	UI();

	void _processAction(lif::ui::Action action);

public:

	static UI& getInstance() {
		static UI instance;
		return instance;
	}

	/** Loads all screens from `scrNames` into `screens`. Will ignore already-loaded screens. */
	void load(const sf::RenderWindow& window, std::initializer_list<std::string> scrNames);
	/** Adds a custom Screen to the screens (used to manage dynamic screens). */
	template<class T, class...Args>
	void add(Args&&... args) {
		if (screens.find(T::SCREEN_NAME) != screens.end()) {
			std::stringstream ss;
			ss << "Added 2 screens of type " << T::SCREEN_NAME << "!";
			throw std::logic_error(ss.str());
		}
		screens[T::SCREEN_NAME] = std::make_unique<T>(std::forward<Args>(args)...);
	}

	/** The size of the UI is used to construct any screen loaded via `load`.
	 *  This method should be called once immediately after loading the screens.
	 */
	void setSize(const sf::Vector2u& sz) { size = sz; }
	sf::Vector2u getSize() const { return size; }

	std::string getCurrent() const;
	void setCurrent(const std::string& name, bool overrideParent = false);
	void setCurrentToParent();
	void fireClick();

	/** Static screens may have text content of the form {{NAME}}: in that case, that text
	 *  is interpreted as a 'dynamic text', and can be changed after the screen was created.
	 *  Calling this method sets all dynamic texts called 'name' to the new string 'value'.
	 *  This can also be done on a per-screen basis via Screen::updateDynamicText.
	 */
	void setDynamicText(const std::string& name, const std::string& value);

	bool mustQuitGame() const { return quitGame; }
	bool mustLoadGame() const { return loadGame; }
	bool mustSaveGame() const { return saveGame; }
	/** This method can only be safely called after a check that `mustLoadGame() == true`. */
	const lif::SaveData& getLoadedData() const {
		return static_cast<lif::ui::LoadScreen*>(curScreen)->getLoadedData();
	}
	std::string getSaveName() const { return saveName; }

	void rebuildScreens();

	/** UI-specific event loop, to be called when UI is active (instead of the main event loop) */
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void update() override;
	void setOrigin(const sf::Vector2f& pos) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

} // end namespace ui

} // end namespace lif
