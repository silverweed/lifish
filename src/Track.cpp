#include "Track.hpp"
#include "Game.hpp"
#include <sstream>

using Game::Track;
using Game::DIRSEP;

Track::Track(const std::string& _name, float start, float length) 
	: name(_name), loopstart(start), looplength(length) {}

Track::Track(unsigned short num, float start, float length) 
	: loopstart(start), looplength(length) 
{
	std::stringstream ss;
	ss << Game::pwd << DIRSEP << "assets" << DIRSEP << "music" << DIRSEP << "music" << num << ".ogg";
	name = ss.str();
}
