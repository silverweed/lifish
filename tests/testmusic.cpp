#include <SFML/Audio.hpp>
#include "../src/LoopingMusic.hpp"
#include <unistd.h>
#include <climits>

int main() {
	sf::InputSoundFile input;
	input.openFromFile("../assets/music/music1.ogg");
	LoopingMusic music(input);
	music.setLoopPoints(sf::seconds(7.836), sf::seconds(51.5));
	music.setLoop(true);
	music.play();
	usleep(INT_MAX);
}
