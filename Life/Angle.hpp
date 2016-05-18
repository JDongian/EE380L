#if !(_Angle_h)
#define _Angle_h 1

#include <math.h>
#include <limits>
#include <ostream>

#ifndef M_PI
#	define M_PI ((double) 3.1415926535897932)
#endif 

class Angle
{
    private:
        double degrees;
        double radians;

    public:
        enum Unit {
            DEGREE = 1,
            RADIAN = 2
        };

        Angle() {
            degrees = std::numeric_limits<double>::quiet_NaN();
            radians = std::numeric_limits<double>::quiet_NaN();
        }
        Angle(double angle, Unit unit) {
            switch (unit) {
                case DEGREE:
                    degrees = fmod(angle, 360);
                    radians = degrees / 180 * M_PI;
                    break;
                case RADIAN:
                    radians = fmod(angle, 2 * M_PI);
                    degrees = radians / M_PI * 180;
                    break;
            }
        }
        Angle(Angle const& a) {
            degrees = a.degrees;
            radians = a.radians;
        }
        ~Angle(void) { };

        /*Angle& operator=(Angle const& rhs) {
            degrees = rhs.degrees;
            radians = rhs.radians;
            return *this;
        }

        Angle(Angle&& tmp) {
            degrees = tmp.degrees;
            radians = tmp.radians;
        }

        Angle& operator=(Angle&& rhs) {
            degrees = rhs.degrees;
            radians = rhs.radians;
            return *this;
        }*/

        Angle& operator+=(Angle const& rhs) {
            degrees = fmod(degrees + rhs.deg(), 360);
            radians = degrees / 180 * M_PI;
            return *this;
        }

        Angle operator+(Angle const& summand) const {
            Angle result{ *this };
            result += summand;
            return result;
        }

        double rad(void) const {
            return radians;
        }

        double deg(void) const {
            return degrees;
        }
};

inline std::ostream& operator<<(std::ostream& ost, const Angle& a) {
    ost << "(" << a.rad() << "rad, " << a.deg() << "deg)";
    return ost;
}
#endif /* !(_Angle_h) */
