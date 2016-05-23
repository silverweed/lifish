#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>

void playSound(sf::Sound&);

sf::SoundBuffer buf;

int main() {
	buf.loadFromFile("../assets/sounds/fuse.ogg");
	sf::Sound sound;
	playSound(sound);
	std::this_thread::sleep_for(std::chrono::seconds(3));
}

void playSound(sf::Sound& sound) {
	sound.setBuffer(buf);
	sound.play();	
}
