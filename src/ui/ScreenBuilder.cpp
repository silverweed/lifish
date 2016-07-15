#include "ScreenBuilder.hpp"
#include "ScreenStyle.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "utils.hpp"
#include <iostream>
#include <exception>
#include <fstream>
#include <unordered_map>

using Game::UI::ScreenBuilder;
using json = nlohmann::json;

static sf::Vector2f pos;

static void add_style_property(Game::UI::ScreenStyle& style, const std::string& key, const json& value) {
	if (key == "spacing")
		style.spacing = value.get<int>();
	else if (key == "interactable")
		style.interactable = value.get<bool>();
	else if (key == "font")
		style.font = value.get<std::string>();
	else if (key == "character-size")
		style.characterSize = value.get<unsigned int>();
	else if (key == "v-align")
		style.vAlign = value.get<std::string>();
	else if (key == "h-align")
		style.hAlign = value.get<std::string>();
	else
		std::cerr << "Invalid style property: " << key << std::endl;
}

void ScreenBuilder::_parseStyles(Game::UI::Screen& screen, const json& stylesJSON) {
	for (auto it = stylesJSON.begin(); it != stylesJSON.end(); ++it) {
		auto style = it.value();
		for (auto sit = style.begin(); sit != style.end(); ++sit)
			add_style_property(screen.styles[it.key()], sit.key(), sit.value());
	}
}

void ScreenBuilder::_addText(Game::UI::Screen& screen, const json& text) {
	bool interactable = false, consecutive = false;
	sf::Vector2f manualPosition(-1, -1);
	std::string style_name = "default";
	{
		auto it = text.find("interactable");
		interactable = it != text.end() && it->get<bool>();
		it = text.find("consecutive");
		consecutive = it != text.end() && it->get<bool>();
		it = text.find("style");
		if (it != text.end())
			style_name = it->get<std::string>();
		it = text.find("position");
		if (it != text.end()) {
			manualPosition.x = (*it)[0].get<int>();
			manualPosition.y = (*it)[1].get<int>();
		}
	}
	const auto name = text["name"].get<std::string>();
	auto newtxt = new Game::ShadedText;
	const auto style = screen.styles[style_name];
	
	// set shadow 
	newtxt->setShadowSpacing(3.5, 3);
	newtxt->setBGColor(sf::Color::Black);
	// set char size
	newtxt->setCharacterSize(style.characterSize);
	// set font
	newtxt->setFont(Game::getAsset("fonts", style.font));
	// set string
	newtxt->setString(text["string"].get<std::string>());

	// set position
	if (manualPosition.x >= 0 && manualPosition.y >= 0)
		newtxt->setPosition(manualPosition);
	else {
		const auto bounds = newtxt->getGlobalBounds();
		if (consecutive)
			pos.x += bounds.width + style.spacing;
		else
			pos.y += bounds.height + style.spacing;
		std::cerr << "positioned text at " << pos << " :spacing is " << style.spacing << std::endl;
		newtxt->setPosition(pos);
	}

	if (interactable)
		screen.texts[name] = std::unique_ptr<Game::ShadedText>(newtxt);
	else
		screen.nonInteractables.push_back(std::unique_ptr<sf::Drawable>(newtxt));
}

void ScreenBuilder::_addImage(Game::UI::Screen& screen, const json& image) {
	bool interactable = false, consecutive = false;
	sf::Vector2f manualPosition(-1, -1);
	std::string style_name = "default";
	{
		auto it = image.find("interactable");
		interactable = it != image.end() && it->get<bool>();
		it = image.find("consecutive");
		consecutive = it != image.end() && it->get<bool>();
		it = image.find("style");
		if (it != image.end())
			style_name = it->get<std::string>();
		it = image.find("position");
		if (it != image.end()) {
			manualPosition.x = (*it)[0].get<int>();
			manualPosition.y = (*it)[1].get<int>();
		}
	}
	const auto name = image["name"].get<std::string>();
	auto newimg = new sf::Sprite;
	const auto style = screen.styles[style_name];
	
	// set string
	newimg->setTexture(*Game::cache.loadTexture(image["src"].get<std::string>()));
	const auto size = image["size"];
	newimg->setTextureRect(sf::IntRect(0, 0, size[0].get<int>(), size[1].get<int>()));

	// set position
	if (manualPosition.x >= 0 && manualPosition.y >= 0)
		newimg->setPosition(manualPosition);
	else {
		if (consecutive)
			pos.x += style.spacing;
		else
			pos.y += style.spacing;
		newimg->setPosition(pos);
	}

	if (interactable)
		screen.images[name] = std::unique_ptr<sf::Sprite>(newimg);
	else
		screen.nonInteractables.push_back(std::unique_ptr<sf::Drawable>(newimg));
}

void ScreenBuilder::_addElement(Game::UI::Screen& screen, const json& element) {
	const auto type = element["type"].get<std::string>();
	if (type == "text")
		_addText(screen, element);
	else if (type == "image")
		_addImage(screen, element);
	else
		std::cerr << "Invalid type '" << type << "' for element" << std::endl;
}

void ScreenBuilder::build(Game::UI::Screen& screen, const std::string& layoutFileName) {
	if (screen.wasBuilt())
		throw std::logic_error("screen passed to ScreenBuilder has already been built!");

	// See assets/screens/README for the layout format
	const auto absname = Game::getAsset("screens", layoutFileName);
	std::cerr << "Loading screen " << absname << std::endl;
	json screenJSON = json::parse(std::ifstream(absname.c_str()));
	std::cerr << "Loaded." << std::endl;
	
	// top-level properties
	screen.name = screenJSON["name"].get<std::string>();
	auto parent = screenJSON["parent"];
	if (!parent.is_null())
		screen.parent = parent.get<std::string>();
	const auto bgSpritePath = Game::getAsset("graphics", screenJSON["bg"].get<std::string>());
	auto layoutJSON = screenJSON["layout"];

	pos.x = 0, pos.y = 0;

	// styles
	_parseStyles(screen, layoutJSON["styles"]);

	// elements
	for (const auto& element : layoutJSON["elements"])
		_addElement(screen, element);

	auto texture = Game::cache.loadTexture(bgSpritePath);
	texture->setRepeated(true);
	texture->setSmooth(true);
	screen.bgSprite.setTexture(*texture);
	screen.bgSprite.setTextureRect(sf::IntRect(0, 0, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT));
	screen.built = true;

	std::cerr << "Screen loaded. Has " << screen.texts.size() << " texts, " << screen.images.size() 
		<< " images and " << screen.nonInteractables.size() << " non-interactables." << std::endl;
}
