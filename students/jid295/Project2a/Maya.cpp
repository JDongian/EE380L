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
    /* DEBUG */
    //return "Maya+" + serialize();
    return "jid295*";
}

String Maya::species_name(void) const {
    return "Maya:" + serialize();
}

void Maya::startup(void) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    set_mspeed(SPEED_RESTING);
    set_direction(Angle(drand48() * 360, Angle::DEGREE));

    locked_on = false;

    last_update = Event::now();
    recurring(UPDATE_INTERVAL, [=](void) {
        self->update_position();
        self->avert_edge();
    });

    const double A = 1.4;
    // PARAM
    auto turn_timeout = [=](void) { return grid_max / A / get_speed(); };
    recurring(turn_timeout, [=](void) {
        if (!locked_on) {
            self->turn(Angle(90, Angle::DEGREE));

            action_event->cancel();
            action_event = new Event(0, [=](void) { self->action(0); });
        }
    });

    // GLOBAL
    auto reset_timeout = [=](void) {
        return RESET_INTERVAL - fmod(Event::now(), 100);
    };
    recurring(reset_timeout, [=](void) {
        if (fmod(Event::now(), RESET_INTERVAL ) < 1) {
            reset_position();

            /* DEBUG */
            //std::cout << "RESET" << std::endl;
        }
    });


    // PARAM
    recurring(min_reproduce_time / 2, [=](void) {
        // TODO: parameterize with surroundings
        if (health() >= 3.0) {
            spawn();
        }
    });

    action_event = new Event(0, [=](void) { self->action(RADIUS_DEFAULT); });

    /* DEBUG */
    //std::cout << "STARTUP: "
    //          << species_name()
    //          << std::endl;
}

void Maya::reset_position() {
    relative_position = Vector();
    exploration.reset();
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
    
    /* DEBUG */
    //std::cout << "HEALTH: " << health() << std::endl;

    double bound_timeout = timeout();
    bound(bound_timeout, 0.0, 500.0); // in case of infinite timeouts

    new Event(bound_timeout, [=](void) {
        // Helps keep event queue clean
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Maya::avert_edge() {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    Vector position = exploration.normalized_position(relative_position);
    // PARAM
    position += Vector(grid_max / -2, grid_max / -2);
    double x = position.get_x();
    double y = position.get_y();

    // PARAM
    if (grid_max / 2 - abs(x) < MARGIN_WIDTH
     || grid_max / 2 - abs(y) < MARGIN_WIDTH) {
        action_event->cancel();
        action_event = new Event(0.1, [=](void) { self->action(0); });

        /* DEBUG */
        //std::cout << "EDGE AVERT, "
        //          << position
        //          << ", "
        //          << exploration
        //          << std::endl;
    }
}

void Maya::set_direction(const Angle& course) {
    update_position();
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

void Maya::set_mspeed(const double& speed) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);
    new Event(0, [=](void) { self->update_position(); });

    update_position();
    set_speed(speed);

    // PARAM
    //exploration.reduce(0, 0.999);
}

void Maya::update_position(void) {
    double time_delta = Event::now() - last_update;
    relative_position += Vector(direction, time_delta * get_speed());
    exploration.update_explored(relative_position);
    last_update = Event::now();

    /* DEBUG POSITION */
    //std::cout << "POSITION UPDATE: (" << relative_position
    //          << ")\t\tEXPLORATION: " << exploration
    //          << std::endl;
}

ObjList Maya::sense(double radius) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    auto area_info = perceive(radius);
    for (auto info: area_info) {
        Vector delta(Angle(info.bearing, Angle::RADIAN), info.distance);
        if (is_family(info.species)) {
            double id;
            Vector rel_pos;
            Exploration exp;
            deserialize(info.species, id, rel_pos, exp);

            Vector delta_start(relative_position + delta + rel_pos * -1);

            // PARAM
            exploration.expand(delta_start, exp);
            //exploration.expand(delta_start, exp, grid_max + MARGIN_WIDTH * 2);

            /* DEBUG */
            //std::cout << "SHARE: " << id
            //          << " from " << delta_start
            //          << " (e_dx="
            //          << exploration.get_x_max() - exploration.get_x_min()
            //          << ",e_dy="
            //          << exploration.get_y_max() - exploration.get_y_min()
            //          << ")" << std::endl;
        } else {
            exploration.update_explored(relative_position + delta);
        }
    }
    return area_info;
}

Action Maya::encounter(const ObjInfo& target) {
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    locked_on = false;

    if (is_family(target.species)) {
        set_direction(Angle(target.bearing + M_PI / 2, Angle::RADIAN));
        set_mspeed(SPEED_RESTING);

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
        action_event->cancel();
        action_event = new Event(0.1, [=](void) { self->action(RADIUS_DEFAULT); });

        return LIFEFORM_EAT;
    }
}

void Maya::action(double radius) {
    if (health() == 0.0) { return; }
    SmartPointer<Maya> self = SmartPointer<Maya>(this);

    double timeout;
    double new_speed;
    ObjList area_info = sense(radius);
    Vector decision = potential_fields(area_info);

    if (area_info.size() == 0) {
        locked_on = false;
        
        new_speed = SPEED_RESTING;

        // TODO: parameterize
        radius = encounter_distance + radius * 2;
    } else {
        locked_on = true;

        //ObjInfo best_target;
        //best_target.distance = HUGE;

        //for (auto target: area_info) {
        //    if (best_target.distance > target.distance) {
        //        best_target = target;
        //    }
        //}

        double A, B;
        A = 1; B = 20;

        // TODO: figure out speed -- perhaps f(area_info.counts()) ?
        // PARAM
        new_speed = max_speed / (A + B * decision.get_magnitude());


        // TODO: figure out radius for potential fields
        radius *= .5;
    }

    bound(new_speed, SPEED_RESTING, max_speed);
    set_mspeed(new_speed);
    if (decision.get_magnitude() != 0) {
        set_direction(decision.get_angle());
    }
    
    bound(radius, min_perceive_range, max_perceive_range);
    // TODO: parameterize
    timeout = radius / 3 / get_speed();
    // PARAM
    bound(timeout, 0.5, grid_max / 8 / get_speed());

    action_event = new Event(timeout, [=](void) {
        self->action(radius);
    });

    /* DEBUG */
    //std::cout << "LifeForms in radius " << radius
    //          << ": " << area_info.size() << std::endl;
}

double Maya::score_enemy_health(double enemy_health, double my_health) {
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

double Maya::score_algae_health(double health) {
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

Vector Maya::gen_family_force(ObjInfo family) {
    double A;
    A = 7;

    double score_distance = pow(family.distance, -2);

    Vector result (Angle(family.bearing + M_PI, Angle::RADIAN),
            score_distance);
    result *= A; 

    /* DEBUG */
    //std::cout << "ALGAE VECTOR: " << result << std::endl;

    return result;
}

Vector Maya::gen_enemy_force(ObjInfo enemy, double my_health) {
    double A;
    A = 15;

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
Vector Maya::gen_algae_force(ObjInfo algae) { 
    const double A = 10;
    double score_hp = score_algae_health(algae.health);
    double score_distance = pow(algae.distance, -2);

    Vector result (Angle(algae.bearing, Angle::RADIAN),
            score_hp * score_distance);
    result *= A;

    /* DEBUG */
    //std::cout << "ALGAE VECTOR: " << result << std::endl;

    return result;
}

Vector Maya::gen_edge_force(Vector norm_pos, double margin_width) {
    const double A = 8;
    const double B = 1.1;
    double score_distance;
    Vector result;
    // TODO: parameterize
    double margin = B * margin_width;
    // PARAM
    if (norm_pos.get_x() < margin) {
        score_distance = pow(norm_pos.get_x(), -2);
        result += Vector(Angle(0, Angle::DEGREE), score_distance);
    } else if (grid_max - norm_pos.get_x() < margin) {
        score_distance = pow(grid_max - norm_pos.get_x(), -2);
        result += Vector(Angle(180, Angle::DEGREE), score_distance);
    }
    if (norm_pos.get_y() < margin) {
        score_distance = pow(norm_pos.get_y(), -2);
        result += Vector(Angle(90, Angle::DEGREE), score_distance);
    } else if (grid_max - norm_pos.get_y() < margin) {
        score_distance = pow(grid_max - norm_pos.get_y(), -2);
        result += Vector(Angle(270, Angle::DEGREE), score_distance);
    }

    result *= A;

    /* DEBUG */
    //std::cout << "EDGE VECTOR: " << result << std::endl;

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
    Vector position = exploration.normalized_position(relative_position);
    result += gen_edge_force(position, MARGIN_WIDTH);

    /* DEBUG */
    //std::cout << "DECISION VECTOR: " << result
    //          << ", EST. POSITION: " << position
    //          << std::endl;

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
    // times_reproduced++;
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
    sstm << id << ";"
         << relative_position << ";"
         << exploration << ";"
         << direction;

    return sstm.str();
}

void Maya::deserialize(String serial,
        double& id, Vector& rel_pos, Exploration& exp) const {
    auto parts = split(serial, ':');
    auto values = split(parts[1], ';');

    id = stoi(values[0]);
    rel_pos = Vector(values[1]);
    exp = Exploration(values[2]);
}
