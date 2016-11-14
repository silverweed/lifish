#pragma once

namespace Game {

class Activable {
protected:
	bool active;

public:
	virtual void setActive(bool b) { active = b; }	
	bool isActive() const { return active; }
	/** @return The active state after the change */
	bool toggleActive() { 
		setActive(!active);
		return active;
	}
};

}
