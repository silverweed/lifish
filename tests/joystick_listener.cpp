#include "JoystickListener.hpp"
#include <iostream>

using lif::joystick::JoystickListener;

int main() {
	JoystickListener jl(0);
	
	sf::Joystick::update();
	std::cout << "connected? " << sf::Joystick::isConnected(0) << std::endl;

	while (true) {
		sf::Joystick::update();
		jl.update();
		if (jl.evtMoved(JoystickListener::Axis::L_UP)) std::cout << "L_UP" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::L_DOWN)) std::cout << "L_DOWN" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::L_LEFT)) std::cout << "L_LEFT" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::L_RIGHT)) std::cout << "L_RIGHT" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::R_UP)) std::cout << "R_UP" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::R_DOWN)) std::cout << "R_DOWN" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::R_LEFT)) std::cout << "R_LEFT" << std::endl;
		if (jl.evtMoved(JoystickListener::Axis::R_RIGHT)) std::cout << "R_RIGHT" << std::endl;
	}
}
