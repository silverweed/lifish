#pragma once

#define DEG2RAD 0.017453292519943295
#define RAD2DEG 57.29577951308232

namespace Game {

/**
 * Generic class for representing angles. Does NOT autowrap values, and allows negative angles.
 */
class Angle {
	/** Angle is stored in radians */
	double angle;

public:
	enum Type {
		RADIANS,
		DEGREES
	};

	constexpr Angle() : angle(0) {}
	constexpr Angle(double a, Type type) : angle(a * (type == Angle::RADIANS ? 1.0 : DEG2RAD)) {}

	constexpr double asRadians() const { return angle; }
	constexpr double asDegrees() const { return angle * RAD2DEG; }
};

#undef RAD2DEG
#undef DEG2RAD

constexpr Angle radians(double a) { return Angle(a, Angle::Type::RADIANS); }
constexpr Angle degrees(double a) { return Angle(a, Angle::Type::DEGREES); }

Angle operator +(Angle left, Angle right);
Angle& operator +=(Angle& left, Angle right);
Angle operator -(Angle left, Angle right);
Angle& operator -=(Angle& left, Angle right);
Angle operator -(Angle right);
Angle operator *(Angle left, double right);
Angle& operator *=(Angle& left, double right);
Angle operator /(Angle left, double right);
Angle& operator /=(Angle& left, double right);
bool operator ==(Angle left, Angle right);
bool operator !=(Angle left, Angle right);
bool operator <=(Angle left, Angle right);
bool operator >=(Angle left, Angle right);
bool operator <(Angle left, Angle right);
bool operator >(Angle left, Angle right);

}
