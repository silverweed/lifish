#pragma once

namespace Game {

class Activable {
protected:
	bool active;

public:
	void setActive(bool b) { active = b; }	
	bool isActive() const { return active; }
	/** @return The active state after the change */
	bool toggleActive() { return active = !active; }
};

}
