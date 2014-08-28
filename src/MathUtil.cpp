#include <uRobotics/mathematics/MathUtil.h>
#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions/round.hpp>

using namespace Robotics::Mathematics;

const double MathUtil::E =			2.7182818284590452353602874713527;
const double MathUtil::Log10E =		0.43429448190325182765112891891661;
const double MathUtil::Log2E =		1.4426950408889634073599246810019;
const double MathUtil::Pi =			3.1415926535897932384626433832795;
const double MathUtil::PiOver2 =	1.5707963267948966192313216916398;
const double MathUtil::PiOver4 =	0.78539816339744830961566084581988;
const double MathUtil::PiOver6 =	0.52359877559829887307710723054658;
const double MathUtil::TwoPi =		6.283185307179586476925286766559;

double MathUtil::abs(double value) {
	return value < 0 ? -value : value;
}

boost::int16_t MathUtil::abs(boost::int16_t value) {
	return value < 0 ? (short)-value : value;
}

boost::int32_t MathUtil::abs(boost::int32_t value) {
	return value < 0 ? -value : value;
}

boost::int64_t MathUtil::abs(boost::int64_t value) {
	return value < 0 ? -value : value;
}

boost::int8_t MathUtil::abs(boost::int8_t value) {
	return value < 0 ? -value : value;
}

float MathUtil::abs(float value) {
	return value < 0 ? -value : value;
}

double MathUtil::clamp(double value, double min, double max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

double MathUtil::distance(double value1, double value2) {
	return abs(value1 - value2);
}

double MathUtil::lerp(double value1, double value2, double amount) {
	return value1 + (value2 - value1) * amount;
}

double MathUtil::acos(double d) {
	return ACOS[(int)((1 + d) * 5000)];
}

double MathUtil::asin(double d) {
	return ASIN[(int)((1 + d) * 5000)];
}

double MathUtil::cos(double aRad) {
	if(aRad != aRad) return 0;
	aRad = correctAngle(aRad + PiOver2);
	return SIN[(int)boost::math::round(aRad * 10000)];
	return 0;
}

double MathUtil::sin(double aRad) {
	if(aRad != aRad) return 0;
	aRad = correctAngle(aRad);
	return SIN[(int) boost::math::round(aRad * 10000)];
	return 0;
}

double MathUtil::tan(double aRad) {
	if(aRad != aRad) return 0;
	aRad = correctAngle(aRad);
	double s = sin(aRad);
	double c = cos(aRad);
	if (c == 0) return std::numeric_limits<double>::infinity();
	return s / c;
}

float MathUtil::correctAngle(const float& aRad){
	int quotient;
	float corrected;
	float pi = boost::math::constants::pi<float>();
	if(aRad != aRad) return 0;
	quotient = (int)(aRad / pi);
	corrected = aRad - quotient * pi;
	return corrected;
}

double MathUtil::correctAngle(const double& aRad) {
	int quotient;
	double corrected;
	double pi = boost::math::constants::pi<double>();
	if(aRad != aRad) return 0;
	quotient = (int)(aRad / pi);
	corrected = aRad - quotient * pi;
	return corrected;
}

double MathUtil::toDegrees(const double& aRad) {
	if(aRad != aRad) return 0;
	return correctAngle(aRad) * 180.0 / boost::math::constants::pi<double>();
}

double MathUtil::toRadians(const double& aDeg) {
	if(aDeg != aDeg) return 0;
	return correctAngle(aDeg * boost::math::constants::pi<double>() / 180.0);
}

double MathUtil::round(const double& value){
	return boost::math::round(value);
}