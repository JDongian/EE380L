#include "Params.h"

template <typename T>
T bound(const T& x, const T& min, const T& max) {
    assert (min < max);
    if (x > max) { return max; }
    if (x < min) { return min; }
    return x;
}

class Gene {
    public:
        const double min_speed = 0.5;
        const double min_margin = 2;
        const double max_margin = grid_max / 3;

        double SPEED_RESTING;
        double RADIUS_DEFAULT;
        double MARGIN_WIDTH;

        //std::function <const double& (const double&)>& ACTIVITY = [=](const double& speed) ->
        //    const double& {
        //        return grid_max / 8 / speed;
        //    };

        // Good initial values
        Gene() {
<<<<<<< d9b76bf64d7e779250ac7519f70fe776fc129fe3
            SPEED_RESTING = 7;
=======
            SPEED_RESTING = 9;
>>>>>>> there is a bug
            RADIUS_DEFAULT = 100;
            MARGIN_WIDTH = grid_max / 16;
        }
        // asexual randomization
        Gene(const Gene& gene) {
            SPEED_RESTING = bound(gene.SPEED_RESTING, min_speed, max_speed);
            RADIUS_DEFAULT = bound(gene.RADIUS_DEFAULT, min_perceive_range, max_perceive_range);
            MARGIN_WIDTH = bound(gene.MARGIN_WIDTH, min_margin, max_margin);
        }
        //Gene(double resting_speed, double default_radius, double margin_width) {
        //    SPEED_RESTING = resting_speed;
        //    RADIUS_DEFAULT = default_radius;
        //    MARGIN_WIDTH = margin_width;
        //}
}
