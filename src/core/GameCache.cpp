#include "GameCache.hpp"
#include "Options.hpp"
#include "core.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>

using lif::GameCache;

GameCache::GameCache() {}

void GameCache::setMaxParallelSounds(std::size_t n) {
	maxParallelSounds = n;
}

sf::Texture* GameCache::loadTexture(const std::string& textureName) {
	// Check if image is already in cache
	const auto nameSid = lif::sid(textureName);
	auto it = textures.find(nameSid);
	if (it != textures.end())
		return &it->second;

	// Not in cache: load from file
	auto& txt = textures[nameSid];
	if (!txt.loadFromFile(textureName)) {
		std::cerr << "[GameCache] Error: couldn't load texture " << textureName << " from file!\r\n";
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << textureName << std::endl;
	}
#endif
	return &txt;
}

sf::SoundBuffer* GameCache::loadSoundBuffer(const std::string &soundName) {
	// Check if sound buffer is already in cache
	const auto nameSid = lif::sid(soundName);
	auto it = soundBuffers.find(nameSid);
	if (it != soundBuffers.end()) {
		return &it->second;
	}
	// Not in cache: load from file
	auto& buf = soundBuffers[nameSid];
	if (!buf.loadFromFile(soundName)) {
		std::cerr << "[GameCache] Error: couldn't load sound " << soundName << " from file!\r\n";
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << soundName << std::endl;
	}
#endif
	return &buf;
}

bool GameCache::loadSound(sf::Sound& sound, const std::string& soundName) {
	auto buf = loadSoundBuffer(soundName);
	if (buf)
		sound.setBuffer(*buf);
	return !!buf;
}

void GameCache::playSound(const std::string& soundName) {
	if (lif::options.soundsMute) return;

	// Find a free slot to put this sound into, or discard oldest sound
	auto it = sounds.begin();
	unsigned idx = 0;
	while (it != sounds.end() && idx < maxParallelSounds) {
		if (it->getStatus() != sf::Sound::Status::Playing) {
			sounds.erase(it);
			break;
		}
		++it, ++idx;
	}

	// no room
	if (idx == maxParallelSounds) {
		sounds.pop_front();
	}
	sounds.push_back(sf::Sound());
	auto& sound = sounds.back();
	if (!loadSound(sound, soundName))
		return;

	sound.setVolume(lif::options.soundsVolume);
	sound.play();
}

sf::Font* GameCache::loadFont(const std::string& fontName) {
	const auto nameSid = lif::sid(fontName);
	auto it = fonts.find(nameSid);
	if (it != fonts.end())
		return &it->second;

	// Load from file and update the cache
	auto& font = fonts[nameSid];
	if (!font.loadFromFile(fontName)) {
		std::cerr << "[GameCache.cpp] Error: couldn't load font " << fontName << " from file!\r\n";
	}
#ifndef RELEASE
	else {
		std::cerr << "[GameCache] Loaded " << fontName << std::endl;
	}
#endif
	return &font;
}

void GameCache::finalize() {
	textures.clear();
	sounds.clear();
	soundBuffers.clear();
	fonts.clear();
}

void GameCache::preloadAllAssets() {
	static const char *const gfxList[] = {
		"alien_boss.png",
		"aliensprite.png",
		"bg1.png",
		"bg2.png",
		"bg3.png",
		"bg4.png",
		"bg5.png",
		"bg6.png",
		"bg7.png",
		"bg8.png",
		"bg9.png",
		"bg.png",
		"big_alien_boss.png",
		"bomb.png",
		"bonuses.png",
		"bonus_icons.png",
		"boom_logo.png",
		"BOOMsprite.png",
		"border1.png",
		"border2.png",
		"border3.png",
		"border4.png",
		"border5.png",
		"border6.png",
		"border7.png",
		"border8.png",
		"border.png",
		"bossbullet.png",
		"breakable.png",
		"bullets.png",
		"coin.png",
		"de.png",
		"egg.png",
		"enemy10.png",
		"enemy1.png",
		"enemy2.png",
		"enemy3.png",
		"enemy4.png",
		"enemy5.png",
		"enemy6.png",
		"enemy7.png",
		"enemy8.png",
		"enemy9.png",
		"energy_bar_empty.png",
		"energy_bar.png",
		"es.png",
		"explosionC.png",
		"explosionH.png",
		"explosion.png",
		"explosionV.png",
		"extragame.png",
		"extra_icons.png",
		"extra_letters.png",
		"fireball.png",
		"fixed.png",
		"flame.png",
		"flash.png",
		"fr.png",
		"gameover.png",
		"health.png",
		"hurryup.png",
		"icon.png",
		"it.png",
		"level1.png",
		"lightbolt.png",
		"magma.png",
		"mg_shot.png",
		"panel.png",
		"plasma.png",
		"player1.png",
		"player2.png",
		"playerheads.png",
		"screenbg1.png",
		"screenbg2.png",
		"screenbg3.png",
		"shot.png",
		"speaker.png",
		"teleport.png",
		"uk.png"
	};

	static const char *const soundList[] = {
		"alienboss_death.ogg",
		"alienboss_hurt.ogg",
		"alien_death.ogg",
		"big_alien_boss_death.ogg",
		"big_alien_boss_hurt.ogg",
		"bonus_grab.ogg",
		"bullet101_hit.ogg",
		"bullet101_shot.ogg",
		"bullet1_hit.ogg",
		"bullet1_shot.ogg",
		"bullet2_hit.ogg",
		"bullet2_shot.ogg",
		"bullet3_hit.ogg",
		"bullet3_shot.ogg",
		"bullet4_hit.ogg",
		"bullet4_shot.ogg",
		"bullet5_hit.ogg",
		"bullet5_shot.ogg",
		"bullet6_hit.ogg",
		"bullet6_shot.ogg",
		"bullet7_hit.ogg",
		"bullet7_shot.ogg",
		"coin.ogg",
		"egg_crack.ogg",
		"egg_spawn.ogg",
		"enemy10_death.ogg",
		"enemy10_yell.ogg",
		"enemy1_death.ogg",
		"enemy1_yell.ogg",
		"enemy2_death.ogg",
		"enemy2_yell.ogg",
		"enemy3_death.ogg",
		"enemy3_yell.ogg",
		"enemy4_attack.ogg",
		"enemy4_death.ogg",
		"enemy5_death.ogg",
		"enemy5_yell.ogg",
		"enemy6_death.ogg",
		"enemy6_yell.ogg",
		"enemy7_attack.ogg",
		"enemy7_death.ogg",
		"enemy8_death.ogg",
		"enemy8_yell.ogg",
		"enemy9_death.ogg",
		"enemy9_yell.ogg",
		"explosion.ogg",
		"explosion_orig.ogg",
		"extragame.ogg",
		"extralife.ogg",
		"foo.ogg",
		"fuse.ogg",
		"gameover.ogg",
		"hurryup.ogg",
		"letter_grab.ogg",
		"levelclear.ogg",
		"player1_death.ogg",
		"player1_hurt.ogg",
		"player1_win.ogg",
		"player2_death.ogg",
		"player2_hurt.ogg",
		"player2_win.ogg",
		"teleport.ogg",
		"timebonus.ogg",
		"wall_break.ogg",
	};
	
	for (auto asset : gfxList) {
		loadTexture(lif::getAsset("graphics", asset));
	}
	for (auto asset : soundList) {
		loadSoundBuffer(lif::getAsset("sounds", asset));
	}
}
