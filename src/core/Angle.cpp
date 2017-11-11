#include "Angle.hpp"

namespace lif {

const Angle Angle::Zero;

Angle operator +(Angle left, Angle right) {
	return radians(left.asRadians() + right.asRadians());
}

Angle& operator +=(Angle& left, Angle right) {
	return left = left + right;
}

Angle operator -(Angle left, Angle right) {
	return radians(left.asRadians() - right.asRadians());
}

Angle& operator -=(Angle& left, Angle right) {
	return left = left - right;
}

Angle operator -(Angle right) {
	return radians(-right.asRadians());
}

Angle operator *(Angle left, double right) {
	return radians(left.asRadians() * right);
}

Angle& operator *=(Angle& left, double right) {
	return left = left * right;
}

Angle operator /(Angle left, double right) {
	return radians(left.asRadians() / right);
}

Angle& operator /=(Angle& left, double right) {
	return left = left / right;
}

bool operator ==(Angle left, Angle right) {
	return left.asRadians() == right.asRadians();
}

bool operator !=(Angle left, Angle right) {
	return left.asRadians() != right.asRadians();
}

bool operator >=(Angle left, Angle right) {
	return left.asRadians() >= right.asRadians();
}

bool operator >(Angle left, Angle right) {
	return left.asRadians() > right.asRadians();
}

bool operator <=(Angle left, Angle right) {
	return left.asRadians() <= right.asRadians();
}

bool operator <(Angle left, Angle right) {
	return left.asRadians() < right.asRadians();
}

double operator /(Angle left, Angle right) {
	return left.asRadians() / right.asRadians();
}

}
