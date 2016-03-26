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
        double max_reproduce_threshold = 100.0;
        double min_min_activity = 0.7;
        double max_min_activity = grid_max / 4 / max_speed;
        double min_rad_mult = 0.1;
        double max_rad_mult = 100;
        double min_rad_reduc = 0.1;
        double max_rad_reduc = 1.5;

        double speed_resting;
        double radius_default;
        double margin_divisor;
        double turn_enabled;
        double reproduce_a;
        double min_activity;
        double f_family_a;
        double f_algae_a;
        double f_enemy_a;
        double empty_mult;
        double speed_mult;
        double rad_reduc;

    public:
        double SPEED_RESTING;
        double RADIUS_DEFAULT;
        double MARGIN_WIDTH;
        bool TURN_ENABLED;
        double REPRODUCE_THRESHOLD;
        double MIN_ACTIVITY;
        double FORCE_FAMILY_A;
        double FORCE_ALGAE_A;
        double FORCE_ENEMY_A;
        double EMPTY_WORLD_RADIUS_MULTIPLIER;
        double VECTOR_SPEED_MULTIPLIER;
        double LOCK_ON_REDUCTION;

        //std::function <const bool&(const double&&)> DO_REPRODUCE;
        //= []() {
        //    return false;//health > reproduce_a;
        //};
 
        // Good initial values
        Gene() {
            speed_resting = max_speed * 0.40 + uni_rand(0, max_speed);
            radius_default = max_perceive_range * 0.6 + uni_rand(0, max_perceive_range/2);
            margin_divisor = 12.5 + uni_rand(0, 1);
            turn_enabled = uni_rand(0, 0.2);
            reproduce_a = 3.8 + uni_rand(0, 6);
            min_activity = 10 + uni_rand(0, 4);
            f_family_a = 17 + uni_rand(0, 8);
            f_algae_a = 26 + uni_rand(0, 8);
            f_enemy_a = 24 + uni_rand(0, 8);
            empty_mult = 2;
            speed_mult = 2000;
            rad_reduc = 0.5;

            bind();
        }
        Gene(double speed,
             double radius,
             double margin,
             double turn,
             double rep_a,
             double min_act,
             double f_f_a,
             double f_a_a,
             double f_e_a,
             double e_m,
             double s_m,
             double r_r) {
            speed_resting = speed;
            radius_default = radius;
            margin_divisor = margin;
            turn_enabled = turn,
            reproduce_a = rep_a;
            min_act = min_activity;
            f_family_a = f_f_a;
            f_algae_a = f_a_a;
            f_enemy_a = f_e_a;
            empty_mult = e_m;
            speed_mult = s_m;
            rad_reduc = r_r;

            bind();
        }
        Gene(const Gene& other) {
            new (this) Gene(other.speed_resting,
                            other.radius_default,
                            other.margin_divisor,
                            other.turn_enabled,
                            other.reproduce_a,
                            other.min_activity,
                            other.f_family_a,
                            other.f_algae_a,
                            other.f_enemy_a,
                            other.empty_mult,
                            other.speed_mult,
                            other.rad_reduc);
             bind();
        }
        Gene(std::string serial) {
            auto results = split(serial, ',');
            new (this) Gene(stod(results[0]),
                            stod(results[1]),
                            stod(results[2]),
                            stod(results[3]),
                            stod(results[4]),
                            stod(results[5]),
                            stod(results[6]),
                            stod(results[7]),
                            stod(results[8]),
                            stod(results[9]),
                            stod(results[10]),
                            stod(results[11]));
        }

        Gene& operator=(Gene&& other) {
            new (this) Gene(other.speed_resting,
                            other.radius_default,
                            other.margin_divisor,
                            other.turn_enabled,
                            other.reproduce_a,
                            other.min_activity,
                            other.f_family_a,
                            other.f_algae_a,
                            other.f_enemy_a,
                            other.empty_mult,
                            other.speed_mult,
                            other.rad_reduc);
            return *this;
        }

        std::ostream& serialize(std::ostream & ost) const {
            ost << speed_resting << ","
                << radius_default << ","
                << margin_divisor << ","
                << turn_enabled << ","
                << reproduce_a << ","
                << min_activity << ","
                << f_family_a << ","
                << f_algae_a << ","
                << f_enemy_a << ","
                << empty_mult << ","
                << speed_mult << ","
                << rad_reduc;
            return ost;
        }

        void bind(void) {
            SPEED_RESTING = speed_resting;
            bound(SPEED_RESTING, min_speed, max_speed);

            RADIUS_DEFAULT = radius_default;
            bound(RADIUS_DEFAULT, min_perceive_range, max_perceive_range + 3);

            MARGIN_WIDTH = grid_max / margin_divisor;
            bound(MARGIN_WIDTH, min_margin, max_margin);

            bound(turn_enabled, 0.0, 1.0);
            TURN_ENABLED = turn_enabled < 0.5;

            REPRODUCE_THRESHOLD = reproduce_a;
            bound(REPRODUCE_THRESHOLD, min_reproduce_threshold, max_reproduce_threshold);

            MIN_ACTIVITY = min_activity;
            bound(MIN_ACTIVITY, min_min_activity, max_min_activity);

            FORCE_FAMILY_A = f_family_a;
            FORCE_ALGAE_A = f_algae_a;
            FORCE_ENEMY_A = f_enemy_a;

            EMPTY_WORLD_RADIUS_MULTIPLIER = empty_mult;
            bound(EMPTY_WORLD_RADIUS_MULTIPLIER, min_rad_mult, max_rad_mult);

            VECTOR_SPEED_MULTIPLIER = speed_mult;

            LOCK_ON_REDUCTION = rad_reduc;
            bound(LOCK_ON_REDUCTION, min_rad_reduc, max_rad_reduc);
        }

        // asexual randomization
        void randomize(void) {
            speed_resting += uni_rand(0.05, 2);
            radius_default += uni_rand(0.1, 6);
            margin_divisor += uni_rand(0, 1);
            turn_enabled += uni_rand(0, 0.2);
            reproduce_a += uni_rand(0.1, 2);
            min_activity += uni_rand(0, 2);
            f_family_a += uni_rand(0, 2);
            f_algae_a += uni_rand(0, 4);
            f_enemy_a += uni_rand(0, 2);
            empty_mult += uni_rand(0, 0.4);
            speed_mult += uni_rand(0, 20);
            rad_reduc += uni_rand(0, 0.1);

            bind();
        }

        double uni_rand(double origin, double range) {
            return drand48() * range + origin - range / 2;
        }

        // TODO: gaussian
        // TODO: simulated annealing (the cooldown part)
};

inline std::ostream& operator<<(std::ostream& ost, const Gene& g) {
    return g.serialize(ost);
}
#endif /* !(_Gene_h) */
