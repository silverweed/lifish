#pragma once

#include "ShadedText.hpp"
#include "Time.hpp"
#include "utils.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <deque>

namespace lif {
namespace debug {

class FadeoutTextManager : public sf::Drawable {

	constexpr static int PAD_X = 5;
	constexpr static int PAD_Y = 5;
	constexpr static int ROW_HEIGHT = 20;

	struct FadeoutText {
		mutable lif::ShadedText text;
		sf::Time time;
	};

	const sf::Font& font;
	std::deque<FadeoutText> texts;
	sf::Time fadeoutTime;

public:
	explicit FadeoutTextManager(const sf::Font& font, sf::Time fadeoutTime)
		: font(font)
		, fadeoutTime(fadeoutTime)
	{}

	void update() {
		const auto delta = lif::time.getRealDelta();

		int nToRemove = 0;
		for (auto& text : texts) {
			auto& time = text.time;
			time -= delta;
			nToRemove += (time <= sf::Time::Zero);

			auto& txt = text.text;
			const auto t = time / fadeoutTime;
			const auto alpha = lif::lerp(0, 255, std::pow(t, 0.5));
			auto fc = txt.getFGColor();
			auto bc = txt.getBGColor();
			fc.a = alpha;
			bc.a = alpha;
			txt.setColor(fc, bc);
		}

		for (int i = 0; i < nToRemove; ++i)
			texts.pop_front();
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		int i = 0;
		for (const auto& text : texts) {
			text.text.setPosition(sf::Vector2f(PAD_X, PAD_Y + (i * ROW_HEIGHT)));
			target.draw(text.text, states);
			++i;
		}
	}

	void add(const std::string& text) {
		lif::ShadedText txt(font, text, sf::Vector2f(300, 300));
		txt.setCharacterSize(16);
		texts.push_back({ txt, fadeoutTime });
	}
};
}
}
