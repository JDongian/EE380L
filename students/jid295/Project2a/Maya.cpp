#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "Maya.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"
#include "base64.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

template <typename T>
T bound(T& x, const T& min, const T& max) {
    assert (min < max);
    if (x > max) { x = max; }
    if (x < min) { x = min; }
    return x;
}

/* boilerplate */
Initializer<Maya> __Maya_initializer;

void Maya::initialize(void) {
    LifeForm::add_creator(Maya::create, "Maya");
}

SmartPointer<LifeForm> Maya::create(void) {
    return new Maya;
}

Maya::Maya() {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);
    new Event(0, [=](void) { self->startup(); });
}

Maya::~Maya() {}
/* end of boilerplate */

Color Maya::my_color(void) const {
    return BLUE;
}

String Maya::player_name(void) const {
    return "jid295'";
}

String Maya::species_name(void) const {
    return "Maya:" + serialize();
}

void Maya::startup(void) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    id = lrand48();
    
    // TODO: parameterize
    set_speed(SPEED_RESTING);
    set_direction(Angle(0, Angle::DEGREE));

    locked_on = false;

    // // Not sure why we need to wait twice, but okay.
    // new Event(update_interval, [=](void) {
    //     recurring(update_interval, [=](void) { self->update_position(); });
    // });

    // TODO: parameterize
    double STAGE_MARGIN = grid_max / 8 / 1.414;
    //auto turn_timeout = [=](void) { return STAGE_MARGIN / get_speed(); };
    auto turn_timeout = [=](void) { return STAGE_MARGIN * 2 / 1.414 / get_speed(); };
    recurring(turn_timeout,
    [=](void) {
        if (!locked_on) {
            self->turn(Angle(90, Angle::DEGREE));
        }
    });

    // TODO: parameterize
    recurring(5, [=](void) {
        // TODO: parameterize
        if (health() >= 3.0) {
            spawn();
        }
    });

    hunt_event = new Event(0, [=](void) { self->hunt(RADIUS_DEFAULT); });

    /* DEBUG */
    //std::cout << "STARTUP: "
    //          << species_name()
    //          << std::endl;
}

void Maya::recurring(double timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    new Event(timeout, [=](void) {
        // Helps keep event queue clean
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Maya::recurring(const std::function <double (void)>& timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    new Event(timeout(), [=](void) {
        // Helps keep event queue clean
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Maya::set_direction(const Angle& course) {
    direction = course;
    set_course(course.rad());

    /* DEBUG */
    //std::cout << "TURN: +"
    //          << delta.deg()
    //          << "deg"
    //          << std::endl;
}

void Maya::turn(const Angle& delta) {
    set_direction(direction + delta);
}

void Maya::update_position(void) {
    if (health() == 0.0) { return; }
    SmartPointer<Maya> self = SmartPointer<Maya>(this);
}

ObjList Maya::sense(double radius) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    return perceive(radius);
}

Action Maya::encounter(const ObjInfo& target) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    locked_on = false;

    if (is_family(target.species)) {
        set_direction(Angle(target.bearing + M_PI / 2, Angle::RADIAN));
        set_speed(SPEED_RESTING);

        /* DEBUG */
        //std::cout << "FAMILY ENCOUNTER: "
        //    << species_name()
        //    << " encounters "
        //    << target.species
        //    << ", my direction="
        //    << direction
        //    << std::endl;

        return LIFEFORM_IGNORE;
    } else {
        hunt_event->cancel();
        hunt_event = new Event(0.2, [=](void) { self->hunt(RADIUS_DEFAULT); });

        return LIFEFORM_EAT;
    }
}

void Maya::hunt(double radius) {
    if (health() == 0.0) { return; }
    SmartPointer<Maya> self = SmartPointer<Maya>(this);
    // TODO: parameterize
    double timeout;

    ObjList area_info = sense(radius);

    if (area_info.size() == 0) {
        locked_on = false;

        // TODO: improve this (maybe go to the center)
        set_speed(SPEED_RESTING);

        // TODO: parameterize
        radius += encounter_distance;
        radius *= 2;
    } else {
        locked_on = true;

        ObjInfo best_target;
        best_target.distance = HUGE;

        for (auto target: area_info) {
            if (best_target.distance > target.distance) {
                best_target = target;
            }
        }

        double A;
        A = 20;

        //double A;
        //A = 4;

        Vector decision = potential_fields(area_info);
        Angle new_course = decision.get_angle();
    
        if (decision.get_magnitude() > 0) {
            // TODO: figure out speed
            // PARAM
            double new_speed = max_speed
                / (1 + A * decision.get_magnitude());
            //double new_speed = best_target.their_speed + A;
            bound(new_speed, 2.0, 2.0 + best_target.distance);

            set_direction(new_course);
            set_speed(new_speed);

            /* DEBUG */
            //std::cout << "DECISION VECTOR: " << decision
            //          << ", target_speed=" << new_speed
            //          << std::endl;

            // TODO: figure out radius for potential fields
            radius *= .5;
        } else { std::cout << "MATH ERROR"; }
    }
    
    set_speed(10);

    bound(radius, min_perceive_range, max_perceive_range);
    timeout = radius / 3 / get_speed();
    bound(timeout, 1.0, 30.0);

    hunt_event = new Event(timeout, [=](void) {
        self->hunt(radius);
    });
}

double score_enemy_health(double enemy_health, double my_health) {
    // TODO: GA
    double A, B, C;
    double D; // might be unsigned
    A = 0.7; B = 180; C = 2; D = -0.3;

    double enemy_energy = enemy_health * start_energy;
    double my_energy = my_health * start_energy;
    double energy_difference = my_energy - enemy_energy + D * start_energy;
    // PARAM
    // didn't factor in movement cost
    double divisor = A * min_energy  
        + B * (age_penalty / age_frequency)
        + C * encounter_penalty;

    // Hopefully this is a useful metric.
    return energy_difference / divisor;
}

double score_algae_health(double health) {
    // TODO: GA
    double A, B, C;
    A = 1; B = 200; C = 2;

    double real_energy = health * start_energy;
    // PARAM
    // didn't factor in movement cost
    double divisor = A * min_energy  
        + B * (age_penalty / age_frequency)
        + C * encounter_penalty;

    // Hopefully this is a useful metric.
    return real_energy / divisor;
}

Vector gen_family_force(ObjInfo family) {
    double A;
    A = 10;

    double score_distance = pow(family.distance, -2);

    Vector result (Angle(family.bearing + M_PI + 0.1, Angle::RADIAN),
            score_distance);
    result *= A; 

    /* DEBUG */
    //std::cout << "ALGAE VECTOR: " << result << std::endl;

    return result;
}

Vector gen_enemy_force(ObjInfo enemy, double my_health) {
    double A;
    A = 13;

    double score_hp = score_enemy_health(enemy.health, my_health);
    double score_distance = pow(enemy.distance, -2);

    Vector result (Angle(enemy.bearing, Angle::RADIAN),
            score_hp * score_distance);
    result *= A;

    /* DEBUG */
    //std::cout << "ENEMY VECTOR: " << result << std::endl;

    return result;
}

// Parameter control, no scale factor.
Vector gen_algae_force(ObjInfo algae) { 
    double score_hp = score_algae_health(algae.health);
    double score_distance = pow(algae.distance, -2);

    Vector result (Angle(algae.bearing, Angle::RADIAN),
            score_hp * score_distance);

    /* DEBUG */
    //std::cout << "ALGAE VECTOR: " << result << std::endl;

    return result;
}

Vector Maya::potential_fields(ObjList area_info) {
    Vector result{};

    for (auto info: area_info) {
        switch (get_phylum(info.species)) {
        case FAMILY:
            result += gen_family_force(info);
            break;
        case ENEMY:
            result += gen_enemy_force(info, health());
            break;
        case ALGAE:
            result += gen_algae_force(info);
            break;
        }
    }
    return result;
}

Maya::Phylum Maya::get_phylum(String name) {
    if (is_family(name)) {
        return FAMILY;
    } else if (name == "Algae") {
        return ALGAE;
    } else {
        return ENEMY;
    }
}

void Maya::spawn(void) {
    // TODO: sex
    SmartPointer<Maya> child = new Maya;
    reproduce(child);
}

bool Maya::is_family(String name) {
    String base = "Maya";
    bool result = name.compare(0, base.length(), base) == 0;

    /* DEBUG */
    //std::cout << "NAMECHECK: '"
    //          << name
    //          << (result ? "' is family" : "' not family")
    //          << std::endl;

    return result;
}

// TODO: make this faster with std::to_string
String Maya::serialize() const {
    std::stringstream sstm;
    sstm << id << ","
         << speed << ","
         << direction.deg();

    return sstm.str();
}
