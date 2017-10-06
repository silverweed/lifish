#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/NonCopyable.hpp>
#include "Entity.hpp"
#include "Track.hpp"
#include "entity_type.hpp"

namespace lif {

class LevelSet;
class LevelNumText;

/** Holds the data used in Level initialization. */
struct LevelInfo {
	struct {
		int bg        = 1,
		    border    = 1,
		    fixed     = 1,
		    breakable = 1;
	} tileIDs;

	/** This game's track info */
	lif::Track track;

	/** Number of the level */
	int levelnum = 0;

	/** Width in tiles */
	int width;
	/** Height in tiles */
	int height;

	/** Time before "Hurry Up" (in seconds) */
	int time = 0;

	/** String representation of the tilemap */
	std::string tilemap;

	/** Special effects for this level (e.g. Fog) */
	std::unordered_set<std::string> effects;

	/** Optional cutscene for this level (played before level begin). */
	std::string cutscenePre;
	/** Optional cutscene for this level (played after level ends). */
	std::string cutscenePost;
};

/**
 * The Level class contains the template of a level, with all the
 * static information about it. Dynamic informations about the level
 * during the game are managed by lif::LevelManager.
 */
class Level final : public lif::Entity, private sf::NonCopyable {

	friend class lif::LevelSet;

	/** This ought to be set before calling level.init(); */
	lif::LevelInfo info;

	/** The text containing the level number */
	lif::LevelNumText *levelnumtext = nullptr;

	/** This level's static (initial) tilemap */
	std::vector<lif::EntityType> tiles;

	sf::Texture *bgTexture = nullptr;
	sf::Sprite bgSprite;

	sf::Texture *borderTexture = nullptr;
	sf::Sprite borderSprite;

	/** The LevelSet this level belongs to */
	const lif::LevelSet& levelSet;

	/** Whether this level has been initialized or not */
	bool initialized = false;


	/** Loads the background/border textures from the file (or from memory, if
	 *  texture has already been cached)
	 */
	void _loadTextures();

	/** Loads the content of bgTiles (bgTexture must already be set) */
	void _loadTiles();

	/** Given a string representation of the level, sets its static tilemap
	 *  by filling the `entities` vector.
	 */
	bool _setTilemap(const std::string& tilemap);

public:
	/** Constructs a level without a specified time and tileset. init() must
	 *  be called before using this level. You need to specify a LevelSet
	 *  this Level belongs to.
	 */
	explicit Level(const LevelSet& levelSet);

	/** Loads the appropriate bgTexture, fills the bgTiles and makes this level
	 *  usable. Must be called after setting info.
	 *  Returns nullptr if there were errors, self otherwise.
	 */
	lif::Entity* init() override;
	bool isInitialized() const { return initialized; }

	const LevelInfo& getInfo() const { return info; }

	EntityType getTile(int left, int top) const;

	/** Changes the origin of all tiles */
	void setOrigin(const sf::Vector2f& origin) override;

	const lif::LevelSet& getLevelSet() const { return levelSet; }

	std::string toString() const override;
	std::string getTilemap() const;
	std::string getTilemapRaw() const;

	const sf::Drawable& getBackground() const { return bgSprite; }
	const sf::Drawable& getBorder() const { return borderSprite; }

	bool hasEffect(const std::string& effectName) const {
		return info.effects.find(effectName) != info.effects.end();
	}
};

}
