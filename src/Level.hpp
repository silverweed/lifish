#pragma once
/**
 * The Level class contains the template of a level, with all the
 * static information about it.
 */
#include <string>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "LoopingMusic.hpp"
#include "Drawable.hpp"
#include "Game.hpp"
#include "Track.hpp"
#include "EntityType.hpp"
#include "LevelSet.hpp"
#include "ShadedText.hpp"
#include "utils.hpp"

namespace Game {

class LevelSet;

class Level : public Game::Drawable {
	/** Font for the level num text */
	constexpr static const char* LEVELNUM_FONT = "FreeMono.ttf";

	/** Types of bg texture tiles */
	enum : unsigned short {
		TILE_REGULAR     = 0,
		TILE_UPPER_LEFT  = 1,
		TILE_UPPER_RIGHT = 2,
		TILE_LOWER_LEFT  = 3,
		TILE_LOWER_RIGHT = 4,
		TILE_UPPER       = 5,
		TILE_LOWER       = 6,
		TILE_LEFT        = 7,
		TILE_RIGHT       = 8,
	};

	/** The background texture */
	sf::Texture bgTexture;

	/** The borders' texture */
	sf::Texture borderTexture;

	/** The sprites for the background tiles */
	std::array<sf::Sprite, 9> bgTiles;

	/** The input sound file for the BGM */
	sf::InputSoundFile musicInput;

	/** The music for this level */
	LoopingMusic *music = nullptr;

	/** The music track data */
	Track track;

	/** Number of the level */
	unsigned short levelnum = 0;

	/** The text containing the level number */
	Game::ShadedText *levelnumtext = nullptr;

	/** Time before "Hurry Up" */
	unsigned int time;

	/** This level's static (initial) tilemap */
	Matrix<EntityType, LEVEL_HEIGHT, LEVEL_WIDTH> tiles;

	/** (Optional) LevelSet this level belongs to */
	const LevelSet *const levelSet;


	/** Loads the background/border textures from the file (or from memory, if
	 *  texture has already been cached)
	 */
	bool _loadTexture();

	/** Loads the music from the file <music_name> */
	bool _loadMusic(const std::string& music_name);

	/** Loads the content of bgTiles (bgTexture must already be set) */
	void _loadTiles();

	/** Whether this level has been initialized or not */
	bool initialized = false;
public:
	/** This is public for convenience */
	struct {
		unsigned short bg        = 1,
			       border    = 1,
			       fixed     = 1,
			       breakable = 1;
	} tileIDs;


	/** Constructs a level without a specified time and tileset. init() must
	 *  be called before using this level. Optionally, can specify a LevelSet
	 *  this Level belongs to.
	 */
	Level(const LevelSet *const levelSet = nullptr);
	virtual ~Level();

	/** Loads the appropriate bgTexture, fills the bgTiles and makes this level
	 *  usable. Must be called either after constructing it without parameters or 
	 * after changing them. Returns false if there were errors, true otherwise.
	 */
	bool init();

	bool isInitialized() const { return initialized; }

	unsigned int getTime() const { return time; }
	void setTime(const unsigned int _time) { time = _time; }

	/** Given a string representation of the level, sets its static tilemap
	 *  by filling the <entities> vector.
	 */
	EntityType getTile(unsigned short left, unsigned short top) const;
	bool setTilemap(const std::string& tilemap);

	const Track& getTrack() const { return track; }
	void setTrack(const Track& _track) { track = _track; }

	LoopingMusic* getMusic() const { return music; }

	unsigned short getLevelNum() const { return levelnum; }
	void setLevelNum(const unsigned short num) { levelnum = num; }

	void printInfo() const;
	void printTilemap() const;

	/** Draws this level's background in the target window */
	void draw(sf::RenderTarget& window) override;
};

}
