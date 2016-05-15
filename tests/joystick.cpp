#include <SFML/Window.hpp>
#include <iostream>

#define N 0
#define RED "\033[1;31m"
#define NORMAL "\033[0;0m"

using J = sf::Joystick;

int main() {
	J::update();
	auto id = J::getIdentification(N);
	std::cout << "Joystick: \n"
		<< " [vendor: " << id.vendorId << ", product: " << id.productId << "]" 
		<< "\nhas axis {"
		<< "\n\tX: " << J::hasAxis(N, J::X)
		<< "\n\tY: " << J::hasAxis(N, J::Y)
		<< "\n\tZ: " << J::hasAxis(N, J::Z)
		<< "\n\tR: " << J::hasAxis(N, J::R)
		<< "\n\tU: " << J::hasAxis(N, J::U)
		<< "\n\tV: " << J::hasAxis(N, J::V)
		<< "\n}"
		<< "\n# buttons: " << J::getButtonCount(N)
		<< std::endl;
	while (true) {
		J::update();
		std::cout << "BTN: ";
		for (int i = 0; i < J::getButtonCount(N); ++i) {
			std::cout << i << ":";
			if (J::isButtonPressed(N, i))
				std::cout << RED;
			else
				std::cout << NORMAL;
			std::cout << J::isButtonPressed(N, i) << NORMAL << ", ";
		}
		
		std::cout << "| AXS: ";
		std::cout << "X:" << J::getAxisPosition(N, J::X) << ", ";
		std::cout << "Y:" << J::getAxisPosition(N, J::Y) << ", ";
		std::cout << "Z:" << J::getAxisPosition(N, J::Z) << ", ";
		std::cout << "R:" << J::getAxisPosition(N, J::R) << ", ";
		std::cout << "U:" << J::getAxisPosition(N, J::U) << ", ";
		std::cout << "V:" << J::getAxisPosition(N, J::V);
		std::cout << "\r";	
	}
}
