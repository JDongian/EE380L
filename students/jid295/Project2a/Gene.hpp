#if !(_Gene_h)
#define _Gene_h 1

#include <ostream>
#include "Params.h"

class Gene {
    private:
        template <typename T>
        T bound(const T& x, const T& min, const T& max) {
            assert (min < max);
            if (x > max) { return max; }
            if (x < min) { return min; }
            return x;
        }

    public:
        double min_speed = 0.5;
        double min_margin = 2;
        double max_margin = grid_max / 3;

        double SPEED_RESTING;
        double RADIUS_DEFAULT;
        double MARGIN_WIDTH;
        double TURN_ENABLED;

        //std::function <const double& (const double&)>& ACTIVITY = [=](const double& speed) ->
        //    const double& {
        //        return grid_max / 8 / speed;
        //    };

        // Good initial values
        Gene() {
            SPEED_RESTING = 7;
            RADIUS_DEFAULT = 100;
            MARGIN_WIDTH = grid_max / 15;

            TURN_ENABLED = false;
        }
        // asexual randomization
        //Gene(const Gene& gene) {
        //    SPEED_RESTING = bound(gene.SPEED_RESTING, min_speed, max_speed);
        //    RADIUS_DEFAULT = bound(gene.RADIUS_DEFAULT, min_perceive_range, max_perceive_range);
        //    MARGIN_WIDTH = bound(gene.MARGIN_WIDTH, min_margin, max_margin);
        //}
        Gene(double resting_speed, double default_radius, double margin_width) {
            SPEED_RESTING = resting_speed;
            RADIUS_DEFAULT = default_radius;
            MARGIN_WIDTH = margin_width;
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
