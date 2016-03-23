#if !(_Vector_h)
#define _Vector_h 1

#include <cmath>
#include <math.h>
#include <limits>
#include <ostream>
#include "Angle.hpp"
#include "SerialUtils.h"

#ifndef M_PI
#	define M_PI ((double) 3.1415926535897932)
#endif 

class Vector
{
    private:
        Angle angle;
        double magnitude;

    public:
        Vector() {
            angle = Angle(0, Angle::RADIAN);
            magnitude = 0;
        }
        Vector(Angle a, double len) {
            angle = a;
            magnitude = len;
        }
        Vector(double x, double y) {
            angle = Angle(atan2(y, x), Angle::RADIAN);
            magnitude = pow(x * x + y * y, 0.5);
        }
        Vector(Vector const& a) {
            angle = a.get_angle();
            magnitude = a.get_magnitude();
        }
        Vector (std::string serial) {
            auto results = split(serial, ',');
            new (this) Vector(stoi(results[0]), stoi(results[1])); // wtf
        }
        ~Vector(void) { };

        /*Vector& operator=(Vector const& rhs) {
            degrees = rhs.degrees;
            radians = rhs.radians;
            return *this;
        }

        Vector(Vector&& tmp) {
            degrees = tmp.degrees;
            radians = tmp.radians;
        }

        Vector& operator=(Vector&& rhs) {
            degrees = rhs.degrees;
            radians = rhs.radians;
            return *this;
        }*/

        Vector& operator+=(Vector const& rhs) {
            double x = get_x() + rhs.get_x();
            double y = get_y() + rhs.get_y();
            Vector result{ x, y }; // help
            angle = result.angle;
            magnitude = result.magnitude;

            return *this;
        }

        Vector operator+(Vector const& summand) const {
            Vector result{ *this };
            result += summand;
            return result;
        }

        Vector& operator*=(double const& scalar) {
            magnitude *= scalar;
            return *this;
        }

        Vector operator*(double const& multiplier) const {
            Vector result{ *this };
            result *= multiplier;
            return result;
        }

        Angle get_angle(void) const {
            return angle;
        }

        double get_magnitude(void) const {
            return magnitude;
        }

        double get_x(void) const {
            if (magnitude == 0 || std::isnan(magnitude)) return 0;
            return magnitude * cos(angle.rad());
        }

        double get_y(void) const {
            if (magnitude == 0 || std::isnan(magnitude)) return 0;
            return magnitude * sin(angle.rad());
        }
};

inline std::ostream& operator<<(std::ostream& ost, const Vector& a) {
    ost << a.get_x() << "," << a.get_y();
    return ost;
}
#endif /* !(_Vector_h) */
