#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "Inca.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

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

void pause(void) {
    std::cout << "press Enter to continue...\n";
    system("read");
}

Initializer<Inca> __Inca_initializer;

void Inca::initialize(void) {
    LifeForm::add_creator(Inca::create, "Inca");
}

SmartPointer<LifeForm> Inca::create(void) {
    return new Inca;
}

Inca::Inca() {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);
    gene = new Gene();
    new Event(0, [=](void) { self->startup(); });
}

Inca::Inca(Gene* birth_gene) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);
    gene = birth_gene;
    new Event(0, [=](void) { self->startup(); });
}

Inca::~Inca() {
    delete gene;
}

Color Inca::my_color(void) const {
    return CYAN;
}

String Inca::player_name(void) const {
    /* DEBUG */
    //return "Inca+" + serialize();
    return "jid295**";
}

String Inca::species_name(void) const {
    return "Inca:" + serialize();
}

void Inca::startup(void) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    set_mspeed(gene->SPEED_RESTING);
    //set_direction(Angle(drand48() * 360, Angle::DEGREE));
    set_direction(Angle(0, Angle::DEGREE));

    locked_on = false;

    last_update = Event::now();
    recurring(UPDATE_INTERVAL, [=](void) {
        self->update_position();
        self->avert_edge();
    });

    const double A = 1.4;
    // PARAM
    auto turn_timeout = [=](void) { return grid_max / A / get_speed(); };
    if (gene->TURN_ENABLED) {
        recurring(turn_timeout, [=](void) {
            if (!locked_on) {
                self->turn(Angle(120, Angle::DEGREE));

                action_event->cancel();
                action_event = new Event(0, [=](void) { self->action(0); });
            }
        });
    }

    // GLOBAL
    //auto reset_timeout = [=](void) {
    //    return RESET_INTERVAL - fmod(Event::now(), RESET_INTERVAL);
    //};
    //recurring(reset_timeout, [=](void) {
    //    if (fmod(Event::now(), RESET_INTERVAL ) < 1) {
    //        reset_position();
    //    }
    //});


    // PARAM
    recurring(min_reproduce_time / 2, [=](void) {
        // TODO: parameterize with surroundings
        if (health() >= 3.0) {
            spawn();
        }
    });

    action_event = new Event(0, [=](void) { self->action(gene->RADIUS_DEFAULT); });

    /* DEBUG */
    //std::cout << "STARTUP: "
    //          << species_name()
    //          << std::endl;
}

void Inca::reset_position() {
    relative_position = Vector();
    exploration.reset();

    /* DEBUG */
    //std::cout << "RESET: " << exploration << "(w="
    //          << exploration.get_width() << ", h="
    //          << exploration.get_height() << ")" << std::endl;
    //pause();
}

void Inca::recurring(double timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    new Event(timeout, [=](void) {
        // Helps keep event queue clean
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Inca::recurring(const std::function <double (void)>& timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);
    
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

void Inca::avert_edge() {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    Vector position = exploration.normalized_position(relative_position);
    // PARAM
    position += Vector(grid_max / -2, grid_max / -2);
    double x = position.get_x();
    double y = position.get_y();

    // PARAM
    if (grid_max / 2 - abs(x) < gene->MARGIN_WIDTH
     || grid_max / 2 - abs(y) < gene->MARGIN_WIDTH) {
        action_event->cancel();
        action_event = new Event(0.1, [=](void) { self->action(0); });

        /* DEBUG */
        std::cout << "EDGE OVERSHOOT: "
                  << position
                  << ", "
                  << exploration
                  << std::endl;
    }
}

void Inca::set_direction(const Angle& course) {
    update_position();
    direction = course;
    set_course(course.rad());
}

void Inca::turn(const Angle& delta) {
    set_direction(direction + delta);
}

void Inca::set_mspeed(const double& speed) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);
    new Event(0, [=](void) { self->update_position(); });

    update_position();
    set_speed(speed);

    // PARAM
    //exploration.reduce(0, 0.999);
}

void Inca::update_position(void) {
    double time_delta = Event::now() - last_update;
    relative_position += Vector(direction, time_delta * get_speed());
    exploration.update_explored(relative_position);
    last_update = Event::now();

    /* DEBUG POSITION */
    //std::cout << "POSITION UPDATE: (" << relative_position
    //          << ")\t\tEXPLORATION: " << exploration
    //          << std::endl;
}

ObjList Inca::sense(double radius) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    auto area_info = perceive(radius);
    for (auto info: area_info) {
        Vector delta(Angle(info.bearing, Angle::RADIAN), info.distance);
        if (is_family(info)) {
            double id;
            Vector rel_pos;
            Exploration exp;
            double course;
            double speed;
            Gene genes;
            deserialize(info.species, id, rel_pos, exp, course, speed, genes);

            Vector delta_start(relative_position + delta + rel_pos * -1);

            if (exp.get_width() < grid_max &&
                exp.get_height() < grid_max) {
                exploration.expand(delta_start, exp);

                /* DEBUG */
                //std::cout << "SHARE: " << id
                //          << " from (" << delta.get_x() + get_POSITION().xpos << ", "
                //          << delta.get_y() + get_POSITION().ypos << ")"
                //          << " (e_dx="
                //          << exploration.get_x_max() - exploration.get_x_min()
                //          << ",e_dy="
                //          << exploration.get_y_max() - exploration.get_y_min()
                //          << ") to me "
                //          << get_POSITION()
                //          << std::endl;

                if (exploration.get_width() > grid_max + gene->MARGIN_WIDTH ||
                    exploration.get_height() > grid_max + gene->MARGIN_WIDTH) {
                    reset_position();
                }
            }
       } else {
            exploration.update_explored(relative_position + delta);
            // TODO: add algae + margin vector
        }
    }
    return area_info;
}

Action Inca::encounter(const ObjInfo& target) {
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    locked_on = false;

    if (is_family(target)) {
        set_direction(Angle(target.bearing + M_PI / 2, Angle::RADIAN));
        set_mspeed(gene->SPEED_RESTING);

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
        action_event = new Event(0.1, [=](void) { self->action(gene->RADIUS_DEFAULT); });

        return LIFEFORM_EAT;
    }
}

void Inca::action(double radius) {
    if (health() == 0.0) { return; }
    SmartPointer<Inca> self = SmartPointer<Inca>(this);

    double timeout;
    double new_speed;
    ObjList area_info = sense(radius);
    Vector decision = potential_fields(area_info);

    if (area_info.size() == 0) {
        locked_on = false;
        
        new_speed = gene->SPEED_RESTING;

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

    bound(new_speed, gene->SPEED_RESTING, max_speed + 0.1);
    set_mspeed(new_speed);
    if (decision.get_magnitude() != 0) {
        set_direction(decision.get_angle());
    }
    
    bound(radius, min_perceive_range, max_perceive_range);
    // TODO: parameterize
    timeout = radius / 3 / get_speed();
    // PARAM
    bound(timeout, 0.5, grid_max / 8 / new_speed);
    bound(timeout, 0.5, 8.88);

    action_event = new Event(timeout, [=](void) {
        self->action(radius);
    });

    /* DEBUG */
    //std::cout << "LifeForms in radius " << radius
    //          << ": " << area_info.size() << std::endl;
}

double Inca::score_enemy_health(double enemy_health, double my_health) {
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

double Inca::score_algae_health(double health) {
    // TODO: GA
    double A, B, C;
    A = 1; B = 100; C = 2;

    double real_energy = health * start_energy;
    // PARAM
    // didn't factor in movement cost
    double divisor = A * min_energy  
        + B * (age_penalty / age_frequency)
        + C * encounter_penalty;

    // Hopefully this is a useful metric.
    return real_energy / divisor;
}

Vector Inca::gen_family_force(ObjInfo family) {
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

Vector Inca::gen_enemy_force(ObjInfo enemy, double my_health) {
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
Vector Inca::gen_algae_force(ObjInfo algae) { 
    const double A = 20;
    double score_hp = score_algae_health(algae.health);
    double score_distance = pow(algae.distance, -2);

    Vector result (Angle(algae.bearing, Angle::RADIAN),
            score_hp * score_distance);
    result *= A;

    /* DEBUG */
    //std::cout << "ALGAE VECTOR: " << result << std::endl;

    return result;
}

Vector Inca::gen_edge_force(Vector norm_pos, double margin_width) {
    const double A = 12.22;
    const double B = 2;
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
    //if (result.get_magnitude() != 0) {
    //    std::cout << "EDGE FORCE: " << result << std::endl; }

    return result;
}

Vector Inca::potential_fields(ObjList area_info) {
    Vector result{};

    for (auto info: area_info) {
        switch (get_phylum(info)) {
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
    result += gen_edge_force(position, gene->MARGIN_WIDTH);

    /* DEBUG */
    //std::cout << "DECISION VECTOR: " << result
    //          << ", EST. POSITION: " << position
    //          << std::endl;

    return result;
}

Inca::Phylum Inca::get_phylum(const ObjInfo& info) {
    if (is_family(info)) {
        return FAMILY;
    } else if (info.species == "Algae") {
        return ALGAE;
    } else {
        return ENEMY;
    }
}

void Inca::spawn(void) {
    // TODO: rand
    Gene* child_gene = new Gene(*gene);
    child_gene->randomize();
    SmartPointer<Inca> child = new Inca(child_gene);
    reproduce(child);
    // times_reproduced++;
}

bool Inca::is_family(const ObjInfo& info) {
    String base = "Inca";
    bool result = false;

    if (info.species.compare(0, base.length(), base) == 0) {
        double id;
        Vector rel_pos;
        Exploration exp;
        double course;
        double speed;
        Gene genes;

        // Validate valid string
        if (deserialize(info.species, id, rel_pos, exp, course, speed, genes)) {
            result = true;
        } else {
            result = false;
            std::cout << "WARNING: bad string format \""
                      << info.species << "\""
                      << std::endl;
            pause();
        }
        
        // Validate non-copied string
        if (abs(course - info.their_course) > 0.01
                || abs(speed - info.their_speed) > 0.01) {
            std::cout << "WARNING: spoofing attempt ("
                      << course << ", " << info.their_course << "); ("
                      << speed << ", " << info.their_speed << ")"
                      << std::endl;
            pause();
            result = false;
        }
    }

    /* DEBUG */
    //std::cout << "NAMECHECK: '"
    //          << info.species
    //          << (result ? "' is family" : "' not family")
    //          << std::endl;

    return result;
}

// TODO: make this faster with std::to_string
String Inca::serialize() const {
    std::stringstream sstm;
    sstm << id << ";"
         << relative_position << ";"
         << exploration << ";"
         << get_course() << ";"
         << get_speed() << ";"
         << *gene;
    return sstm.str();
}

bool Inca::deserialize(String serial,
        double& id, Vector& rel_pos,
        Exploration& exp,
        double& course,
        double& speed,
        Gene& g) const {
    try {
        auto values = split(split(serial, ':')[1], ';');

        id = stoi(values[0]);
        rel_pos = Vector(values[1]);
        exp = Exploration(values[2]);
        course = stod(values[3]);
        speed = stod(values[4]);
        g = Gene(values[5]);

        return true;
    } catch (std::invalid_argument& error) {
        pause();
        return false;
    }
}
