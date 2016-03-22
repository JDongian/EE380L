#if !(_Exploration_h)
#define _Exploration_h 1

#include <cmath>
#include "SerialUtils.hpp"

#ifndef M_PI
#	define M_PI ((double) 3.1415926535897932)
#endif

class Exploration {
private:
    double x_min = 0;
    double x_max = 0;
    double y_min = 0;
    double y_max = 0;
public:
    Exploration () { };
    Exploration (std::string serial) {
        auto values = split(serial, ',');
        std::vector<std::string> results;
        for (auto value: values) {
            results.push_back(split(value, '=')[1]);
        }
        x_min = stoi(results[0]);
        x_max = stoi(results[1]);
        y_min = stoi(results[2]);
        y_max = stoi(results[3]);
    }

    void update_explored(const Vector& pos) {
        double x = pos.get_x();
        double y = pos.get_y();

        if (x < x_min) {
            x_min = x;
        } else if (x > x_max) {
            x_max = x;
        }
        if (y < y_min) {
            y_min = y;
        } else if (y > y_max) {
            y_max = y;
        }
    }

    void expand(Vector rel_origin, Exploration exp) {
        Vector top_left(rel_origin);
        top_left += Vector (exp.get_x_min(), exp.get_y_max());

        Vector bottom_right(rel_origin);
        bottom_right += Vector (exp.get_x_max(), exp.get_y_min());

        update_explored(top_left);
        update_explored(bottom_right);
    }

    void reduce(double a, double b) {
        if (x_min < -1 * a) {
            x_min += a;
        }
        if (x_max > a) {
            x_max -= a;
        }
        if (y_min < -1 * a) {
            y_min += a;
        }
        if (y_max > a) {
            y_max -= a;
        }
        x_min *= b;
        x_max *= b;
        y_min *= b;
        y_max *= b;
    }

    void reset() {
        x_min = 0;
        x_max = 0;
        y_min = 0;
        y_max = 0;
    }

    Vector normalized_position(const Vector& rel_pos) {
        // PARAM
        double x = (rel_pos.get_x() - x_min)
            - (x_max - x_min) / 2
            + grid_max / 2;
        double y = (rel_pos.get_y() - y_min)
            - (y_max - y_min) / 2
            + grid_max / 2;
        return Vector(x, y);
    }

    double get_x_min(void) const {
        return x_min;
    }
    double get_x_max(void) const {
        return y_max;
    }
    double get_y_min(void) const {
        return y_min;
    }
    double get_y_max(void) const {
        return y_max;
    }
};

inline std::ostream& operator<<(std::ostream& ost, const Exploration& e) {
    ost << "x_min=" << e.get_x_min() << ","
        << "x_max=" << e.get_x_max() << ","
        << "y_min=" << e.get_y_min() << ","
        << "y_max=" << e.get_y_max();
    return ost;
}
#endif /* !(_Exploration_h) */
