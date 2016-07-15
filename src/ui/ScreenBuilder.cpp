#include "ScreenBuilder.hpp"
#include "ScreenStyle.hpp"
#include "GameCache.hpp"
#include "utils.hpp"
#include <iostream>
#include <exception>
#include <fstream>
#include <unordered_map>
#include <algorithm>

using Game::UI::ScreenBuilder;
using json = nlohmann::json;

sf::Vector2f ScreenBuilder::pos;
sf::FloatRect ScreenBuilder::prevElemBounds;
std::vector<std::pair<sf::Drawable*, unsigned short>> ScreenBuilder::toBeAligned;
std::vector<float> ScreenBuilder::rowWidths; 
float ScreenBuilder::totHeight;

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

// TODO: refactor this and _addImage to DRY
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
	auto newtxt = new Game::ShadedText;
	auto style = screen.styles[style_name];
	{
		auto it = text.find("style-override");
		if (it != text.end())
			for (auto sit = it->begin(); sit != it->end(); ++sit)
				add_style_property(style, sit.key(), sit.value());
	}
	
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
		if (consecutive) {
			pos.x += prevElemBounds.width + style.spacing;
			rowWidths.back() = prevElemBounds.width + style.spacing + bounds.width;
		} else {
			pos.x = 0;
			pos.y += prevElemBounds.height + style.spacing;
			rowWidths.push_back(bounds.width);
			totHeight += bounds.height + style.spacing;
		}
		newtxt->setPosition(pos);
		toBeAligned.push_back(std::make_pair(newtxt, rowWidths.size() - 1));
		prevElemBounds = bounds;
	}

	if (interactable) {
		const auto name = text["name"].get<std::string>();
		screen.texts[name] = std::unique_ptr<Game::ShadedText>(newtxt);
	} else
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
	auto newimg = new sf::Sprite;
	auto style = screen.styles[style_name];
	{
		auto it = image.find("style-override");
		if (it != image.end())
			for (auto sit = it->begin(); sit != it->end(); ++sit)
				add_style_property(style, sit.key(), sit.value());
	}
	
	// set string
	newimg->setTexture(*Game::cache.loadTexture(Game::getAsset("graphics", image["src"].get<std::string>())));
	const auto size = image["size"];
	newimg->setTextureRect(sf::IntRect(0, 0, size[0].get<int>(), size[1].get<int>()));

	// set position
	if (manualPosition.x >= 0 && manualPosition.y >= 0)
		newimg->setPosition(manualPosition);
	else {
		const auto bounds = newimg->getGlobalBounds();
		if (consecutive) {
			pos.x += prevElemBounds.width + style.spacing;
			rowWidths.back() = prevElemBounds.width + style.spacing + bounds.width;
		} else {
			pos.x = 0;
			pos.y += prevElemBounds.height + style.spacing;
			rowWidths.push_back(bounds.width);
			totHeight += bounds.height + style.spacing;
		}
		newimg->setPosition(pos);
		toBeAligned.push_back(std::make_pair(newimg, rowWidths.size() - 1));
		prevElemBounds = bounds;
	}

	if (interactable) {
		const auto name = image["name"].get<std::string>();
		screen.images[name] = std::unique_ptr<sf::Sprite>(newimg);
	} else
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

// center all non-absolute-positioned elements
void ScreenBuilder::_fixAlign(Game::UI::Screen& screen) {
	const float yOffset = (screen.size.y - totHeight) / 2;

	for (auto& pair : toBeAligned) {
		auto& e = pair.first;
		unsigned short row = pair.second;
		const float xOffset = (screen.size.x - rowWidths[row]) / 2;
		auto text = dynamic_cast<Game::ShadedText*>(e);
		if (text != nullptr) {
			text->setPosition(text->getPosition() + sf::Vector2f(xOffset, yOffset));
		} else {
			auto sprite = static_cast<sf::Sprite*>(e);
			sprite->setPosition(sprite->getPosition() + sf::Vector2f(xOffset, yOffset));
		}
	}
}

void ScreenBuilder::build(Game::UI::Screen& screen, const std::string& layoutFileName) {
	if (screen.wasBuilt())
		throw std::logic_error("screen passed to ScreenBuilder has already been built!");

	// See assets/screens/README for the layout format
	const auto absname = Game::getAsset("screens", layoutFileName);
	std::cerr << "Loading screen " << absname << std::endl;
	json screenJSON = json::parse(std::ifstream(absname.c_str()));
	
	// top-level properties
	screen.name = screenJSON["name"].get<std::string>();
	auto parent = screenJSON["parent"];
	if (!parent.is_null())
		screen.parent = parent.get<std::string>();
	const auto bgSpritePath = Game::getAsset("graphics", screenJSON["bg"].get<std::string>());
	auto layoutJSON = screenJSON["layout"];

	pos.x = 0, pos.y = 0;
	toBeAligned.clear();
	rowWidths.clear();
	totHeight = 0;

	// styles
	_parseStyles(screen, layoutJSON["styles"]);

	// elements
	for (const auto& element : layoutJSON["elements"])
		_addElement(screen, element);

	// fix elements' align
	_fixAlign(screen);

	// load bg sprite
	auto texture = Game::cache.loadTexture(bgSpritePath);
	texture->setRepeated(true);
	texture->setSmooth(true);
	screen.bgSprite.setTexture(*texture);
	screen.bgSprite.setTextureRect(sf::IntRect(0, 0, screen.size.x, screen.size.y));
	
	screen.built = true;

	std::cerr << "Screen loaded. Has " << screen.texts.size() << " texts, " << screen.images.size() 
		<< " images and " << screen.nonInteractables.size() << " non-interactables." << std::endl;
}
