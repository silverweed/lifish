#include "ScreenHandler.hpp"
#include "Options.hpp"
#include "utils.hpp"
#include <unordered_set>
#include <stdexcept>

using Game::ScreenHandler;

Game::Action ScreenHandler::handleScreenEvents(sf::RenderWindow& window, ScreenType rootScreen, int enabledScreens) {
	// The currently displayed screen
	Game::Screen *cur_screen = nullptr;
	
	switch (rootScreen) {
	case HOME_SCREEN:
		cur_screen = &screens.home;
		break;
	case PREFERENCES_SCREEN: 
		cur_screen = &screens.preferences;
		break;
	case CONTROLS_SCREEN: 
		cur_screen = &screens.controls;
		break;
	case ABOUT_SCREEN:
		cur_screen = &screens.about;
		break;
	case PAUSE_SCREEN:
		cur_screen = &screens.pause;
		break;
	default:
		throw std::invalid_argument("Called handleScreenEvents with non-existing screen!");
	}

	// The enabled screens
	std::unordered_set<Game::Screen*> enabled_screens;
	if (enabledScreens & HOME_SCREEN) 
		enabled_screens.insert(&screens.home);
	if (enabledScreens & PREFERENCES_SCREEN)
		enabled_screens.insert(&screens.preferences);
	if (enabledScreens & CONTROLS_SCREEN) 
		enabled_screens.insert(&screens.controls);
	if (enabledScreens & ABOUT_SCREEN)
		enabled_screens.insert(&screens.about);
	if (enabledScreens & PAUSE_SCREEN)
		enabled_screens.insert(&screens.pause);

	window.clear();
	cur_screen->draw(window);

	auto find_parent = [&enabled_screens] (Game::Screen *screen) -> Game::Screen* {
		for (const auto& parent : screen->getParents()) {
			if (enabled_screens.find(parent) != enabled_screens.end())
				return parent;
		}
		return nullptr;
	};

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(Game::keep_ratio(event.size, SCREEN_SIZE));
				break;
			case sf::Event::MouseMoved:
				cur_screen->triggerMouseOver(
						window.mapPixelToCoords(sf::Mouse::getPosition(window)));
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Key::Escape) {
					const auto parent = find_parent(cur_screen);
					if (parent != nullptr)
						cur_screen = parent;
					else
						return Game::Action::DO_NOTHING;
				} else if (event.key.code == sf::Keyboard::Key::F) {
					Game::options.showFPS = !Game::options.showFPS;
				}
				break;
			case sf::Event::MouseButtonReleased:
				{
					const auto clicked = cur_screen->triggerMouseClick(
							window.mapPixelToCoords(sf::Mouse::getPosition(window)));
					if (clicked == "start") {
						return Game::Action::START_GAME;
					} else if (clicked == "load") {
						return Game::Action::LOAD_GAME;
					} else if (clicked == "save") {
						return Game::Action::SAVE_GAME;
					} else if (clicked == "preferences") {
						if (enabledScreens & PREFERENCES_SCREEN)
							cur_screen = &screens.preferences;
						break;
					} else if (clicked == "preferences::music_volume_down") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::LOWER);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::music_volume_up") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::RAISE);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::music_mute_toggle") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::MUSIC,
								Game::PreferencesScreen::VolumeAction::MUTE_TOGGLE);
						Game::testMusic();
						break;
					} else if (clicked == "preferences::sounds_volume_down") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::LOWER);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::sounds_volume_up") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::RAISE);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::sounds_mute_toggle") {
						screens.preferences.changeVolume(
								Game::PreferencesScreen::VolumeType::SOUND,
								Game::PreferencesScreen::VolumeAction::MUTE_TOGGLE);
						Game::cache.playSound(Game::getAsset("sounds", Game::TIME_BONUS_SOUND));
						break;
					} else if (clicked == "preferences::controls") {
						if (enabledScreens & CONTROLS_SCREEN)
							cur_screen = &screens.controls;
						break;
					} else if (clicked == "controls::p1") {
						screens.controls.selectPlayer(1);
						break;
					} else if (clicked == "controls::p2") {
						screens.controls.selectPlayer(2);
						break;
					} else if (Game::startsWith(clicked, "controls::change_")) {
						screens.controls.changeControl(window, clicked);
						break;
					} else if (clicked == "controls::joystick_toggle") {
						screens.controls.toggleJoystick();
						break;
					} else if (clicked == "about") {
						if (enabledScreens & ABOUT_SCREEN)
							cur_screen = &screens.about;
						break;
					} else if (clicked == "exit") {
						const auto parent = find_parent(cur_screen);
						if (parent != nullptr)
							cur_screen = parent;
						else
							return Game::Action::EXIT;
					}
					break;
				}
			default:
				break;
			}
		}
		window.clear();
		cur_screen->draw(window);
		Game::maybeShowFPS(window);
		window.display();
	}
	return Game::Action::DO_NOTHING;
}
