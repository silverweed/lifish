#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "WindowContext.hpp"
#include "Cutscene.hpp"
#include "ShadedText.hpp"
#include "EntityGroup.hpp"

namespace lif {

/** The CutscenePlayer is a WindowContext which plays a sequence of Cutscenes in a row. */
class CutscenePlayer final : public lif::WindowContext {

	std::vector<lif::Cutscene> cutscenes;
	const lif::Cutscene *curCutscene = nullptr;
	int curCutsceneIdx = 0;

	int _newContext = 0;

	lif::EntityGroup entities;

	int curSubtitleIdx = -1;
	lif::ShadedText curSubtitle;
	sf::Time curSubtitleTime;
	sf::Clock subtitleClock;

	sf::Clock fadeClock;
	mutable sf::RectangleShape fadeSprite;
	bool fadingIn = false;

	void _startFadeIn();
	void _startFadeOut();
	void _applyFade(sf::RenderTarget& target, sf::RenderStates states) const;

	/** Sets the next cutscene as the current one and resets the per-cutscene status.
	 *  If the current cutscene is the last one, switches to `_newContext`.
	 */
	void _nextCutscene();
	void _initCutscene();
	/** Sets the current subtitle to the next one in the current cutscene. If the current
	 *  subtitle is the last one, sets the subtitle index to -1, so _cutsceneEnded will return true.
	 */
	void _nextSubtitle();
	/** Resets the per-cutscene internal status. */
	void _resetCutscene();
	bool _cutsceneEnded() const { return curSubtitleIdx == -1; }

public:
	explicit CutscenePlayer();

	/** Clears the cutscenes vector and resets the internal status. */
	void reset();
	template<class T>
	void addCutscenes(const T& cuts) {
		for (const auto& c : cuts)
			cutscenes.emplace_back(c);
	}
	/** Starts playing the cutscenes by calling _nextCutscene */
	void play();
	/** Quits the cutscenes and goes to the new context */
	void end();

	void update() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	/** Sets which context the CutscenePlayer returns to after playing all cutscenes. */
	void setNewContext(int ctx) { _newContext = ctx; }

	bool handleEvent(sf::Window& window, sf::Event event) override;
};

}
