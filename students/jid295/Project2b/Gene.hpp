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

    public:
        double min_speed = 0.5;
        double min_margin = grid_max / 100;
        double max_margin = grid_max / 4;

        double turn_enabled;
        double margin_width;

        double SPEED_RESTING;
        double RADIUS_DEFAULT;
        double MARGIN_WIDTH;
        bool TURN_ENABLED;

        //std::function <const double& (const double&)>& ACTIVITY = [=](const double& speed) ->
        //    const double& {
        //        return grid_max / 8 / speed;
        //    };

        // Good initial values
        Gene() {
            SPEED_RESTING = 9;
            SPEED_RESTING += uni_rand(0, 6);
            RADIUS_DEFAULT = 100;
            RADIUS_DEFAULT += uni_rand(0, 20);
            margin_width = 14;
            margin_width += uni_rand(0, 1);

            turn_enabled = 0;

            bind();
        }
        Gene(double resting_speed, double default_radius, double margin_width) {
            SPEED_RESTING = resting_speed;
            RADIUS_DEFAULT = default_radius;
            MARGIN_WIDTH = margin_width;

            bind();
        }
       
        // asexual randomization
        void randomize(void) {
            SPEED_RESTING += uni_rand(0, 2);
            RADIUS_DEFAULT += uni_rand(0, 6);
            MARGIN_WIDTH += uni_rand(0, 12);

            bind();
        }

        void bind(void) {
            bound(SPEED_RESTING, min_speed, max_speed);
            bound(RADIUS_DEFAULT, min_perceive_range, max_perceive_range);
            MARGIN_WIDTH = grid_max / margin_width;
            bound(MARGIN_WIDTH, min_margin, max_margin);
            TURN_ENABLED = turn_enabled < 0.5;
        }

        double uni_rand(double origin, double range) {
            return drand48() * range + origin - range / 2;
        }

        Gene(std::string serial) {
            auto results = split(serial, ',');
            new (this) Gene(stoi(results[0]),
                            stoi(results[1]),
                            stoi(results[2]));
        }
};

inline std::ostream& operator<<(std::ostream& ost, const Gene& g) {
    ost << g.SPEED_RESTING << "," << g.RADIUS_DEFAULT << "," << g.MARGIN_WIDTH;
    return ost;
}
#endif /* !(_Gene_h) */
