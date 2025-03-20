#include "ScreenBuilder.hpp"
#include "GameCache.hpp"
#include "Interactable.hpp"
#include "Screen.hpp"
#include "ScreenStyle.hpp"
#include "game.hpp"
#include "language.hpp"
#include <iostream>
#include <unordered_map>
#include <tinyjson.h>

using lif::ui::ScreenBuilder;
using namespace tinyjson;

static sf::Color colorFromName(const std::string& name) {
	if (name == "red") return sf::Color::Red;
	else if (name == "orange") return sf::Color(255, 51, 0);
	else if (name == "blue") return sf::Color::Blue;
	else if (name == "white") return sf::Color::White;
	else if (name == "gray") return sf::Color(120, 120, 120);
	else if (name == "yellow") return sf::Color::Yellow;
	else if (name == "black") return sf::Color::Black;
	// TODO return arbitrary color from hex
	return sf::Color::Black;
}

static void addStyleProperty(lif::ui::ScreenStyle& style, const std::string& key, const json& value) {
	if (key == "spacing")
		style.spacing = value.get_integer();
	else if (key == "interactable")
		style.interactable = value.get_bool();
	else if (key == "font")
		style.font = value.get_string();
	else if (key == "character-size")
		style.characterSize = value.get_integer();
	else if (key == "v-align")
		style.vAlign = value.get_string();
	else if (key == "h-align")
		style.hAlign = value.get_string();
	else if (key == "color") {
		style.color = colorFromName(value.get_string());
	} else if (key == "shadow-color") {
		style.bgcolor = colorFromName(value.get_string());
	} else if (key == "shadow-spacing") {
		style.shadowSpacing = value.get_integer();
	} else
		std::cerr << "Invalid style property: " << key << std::endl;
}

sf::String ScreenBuilder::_maybeInsertDynamicText(const std::string& s) const {
	const auto len = s.length();

	if (len > 0 && s[0] == '!') {
		// FIXME: handle non-ASCII text
		// Localized string
		return lif::getLocalized(s.substr(1));
	}

	if (len > 4 && s[0] == '{' && s[1] == '{' && s[len - 1] == '}' && s[len - 2] == '}') {
		const auto name = s.substr(2, len - 4);
		const auto mapped = dynamicTexts.find(name);
		if (mapped != dynamicTexts.end())
			return mapped->second;
	}
	return s;
}

void ScreenBuilder::_parseStyles(lif::ui::Screen& screen, const json& stylesJSON) {
	for (const auto &[styleKey, style] : stylesJSON.get_object()) {
		for (const auto &[key, val] : style.get_object()) {
			addStyleProperty(screen.styles[styleKey], key, val);
		}
	}
}

// Define some useful mixins
#define FILL_STYLE_PROPERTIES(obj) \
	{ \
		auto it = obj.find("interactable"); \
		interactable = it && it->get_bool(); \
		it = obj.find("consecutive"); \
		consecutive = it && it->get_bool(); \
		it = obj.find("style"); \
		if (it) \
			style_name = it->get_string(); \
		it = obj.find("position"); \
		if (it) { \
			manualPosition.x = (*it)[0].get_integer(); \
			if (manualPosition.x < 0) \
				manualPosition.x += lif::WINDOW_WIDTH; \
			manualPosition.y = (*it)[1].get_integer(); \
			if (manualPosition.y < 0) \
				manualPosition.y += lif::WINDOW_HEIGHT; \
		} \
	}

#define COMPUTE_STYLE_OVERRIDE(obj) \
	{ \
		auto it = obj.find("style-override"); \
		if (it) \
			for (const auto &[key, val] : it->get_object()) \
				addStyleProperty(style, key, val); \
	} \

#define COMPUTE_POSITION(obj) \
	if (manualPosition.x >= 0 && manualPosition.y >= 0) \
		obj->setPosition(manualPosition); \
	else { \
		const auto bounds = obj->getGlobalBounds(); \
		const auto align = style.hAlign.length() > 0 ? style.hAlign : "center"; \
		if (consecutive) { \
			pos.x += prevElemBounds.width + style.spacing; \
			rowWidths.back() = prevElemBounds.width + style.spacing + bounds.width; \
			rowAligns.back() = align; \
		} else { \
			pos.x = 0; \
			pos.y += prevElemBounds.height + style.spacing; \
			rowWidths.emplace_back(bounds.width); \
			rowAligns.emplace_back(align); \
			totHeight += bounds.height + style.spacing; \
		} \
		obj->setPosition(pos); \
		toBeAligned.emplace_back(obj, rowWidths.size() - 1); \
		prevElemBounds = bounds; \
	} \

void ScreenBuilder::_addText(lif::ui::Screen& screen, const json& text) {
	bool interactable = false, consecutive = false;
	sf::Vector2f manualPosition(-1, -1);
	std::string style_name = "default";

	FILL_STYLE_PROPERTIES(text)

	auto newtxt = new lif::ShadedText;
	auto style = screen.styles[style_name];

	COMPUTE_STYLE_OVERRIDE(text)

	// set shadow
	newtxt->setShadowSpacing(3.5, 3);
	newtxt->setFGColor(style.color);
	newtxt->setBGColor(style.bgcolor);
	// set char size
	newtxt->setCharacterSize(style.characterSize);
	// set font
	newtxt->setFont(*lif::cache.loadFont(lif::getAsset("fonts", style.font)));
	// set string
	newtxt->setString(_maybeInsertDynamicText(text["string"].get_string()));

	// set position
	COMPUTE_POSITION(newtxt)

	if (interactable) {
		const auto name = text["name"].get_string();
		screen.interactables[name] = std::make_unique<lif::ui::Interactable>(newtxt);
	} else {
		newtxt->setShadowSpacing(style.shadowSpacing, style.shadowSpacing);
		screen.nonInteractables.emplace_back(newtxt);
	}
}

void ScreenBuilder::_addImage(lif::ui::Screen& screen, const json& image) {
	bool interactable = false, consecutive = false;
	sf::Vector2f manualPosition(-1, -1);
	std::string style_name = "default";

	FILL_STYLE_PROPERTIES(image)

	auto newimg = new sf::Sprite;
	auto style = screen.styles[style_name];

	COMPUTE_STYLE_OVERRIDE(image)

	// set string
	newimg->setTexture(*lif::cache.loadTexture(lif::getAsset("graphics", image["src"].get_string())));
	const auto size = image["size"];
	newimg->setTextureRect(sf::IntRect(0, 0, size[0].get_integer(), size[1].get_integer()));

	// set position
	COMPUTE_POSITION(newimg)

	if (interactable) {
		const auto name = image["name"].get_string();
		screen.interactables[name] = std::make_unique<lif::ui::Interactable>(newimg);
	} else
		screen.nonInteractables.emplace_back(newimg);
}

void ScreenBuilder::_addElement(lif::ui::Screen& screen, const json& element) {
	const auto type = element["type"].get_string();
	if (type == "text")
		_addText(screen, element);
	else if (type == "image")
		_addImage(screen, element);
	else
		std::cerr << "Invalid type '" << type << "' for element" << std::endl;
}

// center all non-absolute-positioned elements
void ScreenBuilder::_fixAlign(lif::ui::Screen& screen) {
	const float yOffset = vAlign == "top" ? V_PADDING
				: vAlign == "bottom" ? (screen.size.y - totHeight - V_PADDING)
				: (screen.size.y - totHeight) / 2;

	for (auto& pair : toBeAligned) {
		auto& e = pair.first;
		if (!e)
			continue;
		unsigned row = pair.second;
		const float xOffset = rowAligns[row] == "left" ? H_PADDING
					: rowAligns[row] == "right" ? (screen.size.x - rowWidths[row] - H_PADDING)
					: (screen.size.x - rowWidths[row]) / 2;

		if (auto text = dynamic_cast<lif::ShadedText*>(e)) {
			text->setPosition(text->getPosition() + sf::Vector2f(xOffset, yOffset));
		} else {
			auto sprite = static_cast<sf::Sprite*>(e);
			sprite->setPosition(sprite->getPosition() + sf::Vector2f(xOffset, yOffset));
		}
	}
}

void ScreenBuilder::_calcTransitions(lif::ui::Screen& screen) {
	using ElemPair = std::pair<lif::ui::Interactable*, std::string>;
	struct PosCmp {
		bool operator()(const ElemPair& e1, const ElemPair& e2) const {
			return e1.first->getPosition().y < e2.first->getPosition().y;
		}
	};
	std::set<ElemPair, PosCmp> elems;
	// Create a set of interactables ordered by y ascending
	for (const auto& pair : screen.interactables)
		elems.emplace(pair.second.get(), pair.first);

	// Add up/down transitions
	using D = lif::Direction;
	for (auto it = elems.begin(); it != elems.end(); ++it) {
		const auto nxt = std::next(it);
		if (nxt == elems.end())
			screen.transitions.add(it->second, std::make_pair(D::DOWN, elems.begin()->second));
		else
			screen.transitions.add(it->second, std::make_pair(D::DOWN, nxt->second));
	}
	// TODO: left/right
}

void ScreenBuilder::rebuild(lif::ui::Screen& screen) {
	if (!screen.wasBuilt())
		throw std::logic_error("screen passed to ScreenBuilder::rebuild was never built!");
	const auto layout = screen.builtWithLayout;
	screen.clear();
	build(screen, layout);
}

void ScreenBuilder::build(lif::ui::Screen& screen, const std::string& layoutFileName) {
	if (screen.wasBuilt())
		throw std::logic_error("screen passed to ScreenBuilder has already been built!");

	// See assets/screens/README for the layout format
	const auto absname = lif::getAsset("screens", layoutFileName);
	std::string fileContent = lif::readEntireFile(absname);
	json screenJSON = tinyjson::parser::parse(fileContent.c_str());

	// top-level properties
	screen.name = screenJSON["name"].get_string();
	auto parent = screenJSON.find("parent");
	if (parent && parent->type() != tinyjson::json_t::null)
		screen.parent = parent->get_string();
	vAlign = "center";
	{
		auto it = screenJSON.find("v-align");
		if (it)
			vAlign = it->get_string();
	}
	const auto bgSpritePath = lif::getAsset("graphics", screenJSON["bg"].get_string());
	auto layoutJSON = screenJSON["layout"];

	// styles
	_parseStyles(screen, layoutJSON["styles"]);

	// elements
	for (const auto& element : layoutJSON["elements"].get_array())
		_addElement(screen, element);

	// fix elements' align
	_fixAlign(screen);

	// calculate transitions
	_calcTransitions(screen);

	// load bg sprite
	screen._loadBGSprite(bgSpritePath);

	screen.builtWithLayout = layoutFileName;

	//std::cerr << "Screen loaded. Has " << screen.texts.size() << " texts, " << screen.images.size()
		//<< " images and " << screen.nonInteractables.size() << " non-interactables." << std::endl;
}

