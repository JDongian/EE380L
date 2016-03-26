#if !(_Gene_h)
#define _Gene_h 1

#include <ostream>
#include "Params.h"

class Gene {
    private:
        template <typename T>
        T bound(T& x, const T& min, const T& max) {
            assert (min < max);
            if (x > max) { x = max; }
            if (x < min) { x = min; }
            return x;
        }

        double min_speed = 1;
        double min_margin = grid_max / 100;
        double max_margin = grid_max / 4;
        double min_reproduce_threshold = min_energy / start_energy;
        double max_reproduce_threshold = 10.0;

        double speed_resting;
        double radius_default;
        double margin_divisor;
        double turn_enabled;
        double reproduce_a;

    public:
        double SPEED_RESTING;
        double RADIUS_DEFAULT;
        double MARGIN_WIDTH;
        bool TURN_ENABLED;
        double REPRODUCE_THRESHOLD;

        //std::function <const bool&(const double&&)> DO_REPRODUCE;
        //= []() {
        //    return false;//health > reproduce_a;
        //};
 
        // Good initial values
        Gene() {
            speed_resting = max_speed * 0.71 + uni_rand(0, max_speed);
            radius_default = max_perceive_range + uni_rand(0, max_perceive_range/2);
            margin_divisor = 13.8 + uni_rand(0, 1);
            turn_enabled = uni_rand(0, 0.3);
            reproduce_a = 4 + uni_rand(0, 2);

            bind();
        }
        Gene(double speed,
             double radius,
             double margin,
             double turn,
             double rep_a) {
            speed_resting = speed;
            radius_default = radius;
            margin_divisor = margin;
            turn_enabled = turn,
            reproduce_a = rep_a;

            bind();
        }
        Gene(const Gene& other) {
            speed_resting = other.speed_resting;
            radius_default = other.radius_default;
            margin_divisor = other.margin_divisor;
            turn_enabled = other.turn_enabled;
            reproduce_a = other.reproduce_a;

            bind();
        }
        Gene(std::string serial) {
            auto results = split(serial, ',');
            new (this) Gene(stod(results[0]),
                            stod(results[1]),
                            stod(results[2]),
                            stod(results[3]),
                            stod(results[4]));
        }

        Gene& operator=(Gene&& other) {
            speed_resting = other.speed_resting;
            radius_default = other.radius_default;
            margin_divisor = other.margin_divisor;
            turn_enabled = other.turn_enabled;
            reproduce_a = other.reproduce_a;

            bind();

            return *this;
        }

        std::ostream& serialize(std::ostream & ost) const {
            ost << speed_resting << ","
                << radius_default << ","
                << margin_divisor << ","
                << turn_enabled << ","
                << reproduce_a;
            return ost;
        }

        void bind(void) {
            SPEED_RESTING = speed_resting;
            bound(SPEED_RESTING, min_speed, max_speed);

            RADIUS_DEFAULT = radius_default;
            bound(RADIUS_DEFAULT, min_perceive_range, max_perceive_range);

            MARGIN_WIDTH = grid_max / margin_divisor;
            bound(MARGIN_WIDTH, min_margin, max_margin);

            bound(turn_enabled, 0.0, 1.0);
            TURN_ENABLED = turn_enabled < 0.5;

            REPRODUCE_THRESHOLD = reproduce_a;
            bound(REPRODUCE_THRESHOLD, min_reproduce_threshold, max_reproduce_threshold);
        }

        // asexual randomization
        void randomize(void) {
            speed_resting += uni_rand(0, 2);
            radius_default += uni_rand(0, 6);
            margin_divisor += uni_rand(0, 1);
            turn_enabled += uni_rand(0, 0.2);
            reproduce_a += uni_rand(0, 0.4);

            bind();
        }

        double uni_rand(double origin, double range) {
            return drand48() * range + origin - range / 2;
        }

        // TODO: gaussian
};

inline std::ostream& operator<<(std::ostream& ost, const Gene& g) {
    return g.serialize(ost);
}
#endif /* !(_Gene_h) */
