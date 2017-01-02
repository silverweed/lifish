#include "Angle.hpp"
#include <iostream>

using Game::Angle;
using std::cout;
using std::endl;

int main() {
	Angle a;

	cout << "angle is " << a.asRadians() << " rad.\n";

	a += Game::radians(1);
	cout << "angle is " << a.asRadians() << " rad.\n";
}
