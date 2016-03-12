#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "Aztec.h"
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
Initializer<Aztec> __Aztec_initializer;

void Aztec::initialize(void) {
    LifeForm::add_creator(Aztec::create, "Aztec");
}

SmartPointer<LifeForm> Aztec::create(void) {
    return new Aztec;
}

Aztec::Aztec() {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);
    new Event(0, [=](void) { self->startup(); });
}

Aztec::~Aztec() {}
/* end of boilerplate */

Color Aztec::my_color(void) const {
    return YELLOW;
}

String Aztec::player_name(void) const {
    return "jid295";
}

String Aztec::species_name(void) const {
    return "Aztec:" + serialize();
}

void Aztec::startup(void) {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);

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
    auto turn_timeout = [=](void) { return STAGE_MARGIN / get_speed(); };
    //auto turn_timeout = [=](void) { return grid_max / 4 / 1.414 / get_speed(); };
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

void Aztec::recurring(double timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);

    new Event(timeout, [=](void) {
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Aztec::recurring(const std::function <double (void)>& timeout,
        const std::function <void (void)>& callback) {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);

    new Event(timeout(), [=](void) {
        if (health() == 0.0) { return; } 
        callback();
        self->recurring(timeout, callback);
    });
}

void Aztec::set_direction(const Angle& course) {
    direction = course;
    set_course(course.rad());

    /* DEBUG */
    //std::cout << "TURN: +"
    //          << delta.deg()
    //          << "deg"
    //          << std::endl;
}

void Aztec::turn(const Angle& delta) {
    set_direction(direction + delta);
}

void Aztec::update_position(void) {
    if (health() == 0.0) { return; }
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);
}

ObjList Aztec::sense(double radius) {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);

    return perceive(radius);
}

Action Aztec::encounter(const ObjInfo& target) {
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);

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

void Aztec::hunt(double radius) {
    if (health() == 0.0) { return; }
    SmartPointer<Aztec> self = SmartPointer<Aztec>(this);
    // TODO: parameterize
    double timeout;

    ObjList area_info = sense(radius);

    ObjInfo best_target;
    best_target.distance = HUGE;
    bool target_exists = false;
    
    locked_on = false;

    for (auto target: area_info) {
        if (best_target.distance > target.distance) {
            target_exists = true;
            best_target = target;
        }
    }
    if (target_exists) {
        if (is_family(best_target.species)) {
            set_speed(SPEED_RESTING);
            set_direction(Angle(best_target.bearing + M_PI, Angle::RADIAN));
        
            // TODO: parameterize
            timeout = 4;
            //radius = ;

            /* DEBUG */
            //std::cout << "FAMILY IN HUNT RADIUS: "
            //    << species_name()
            //    << " senses "
            //    << best_target.species
            //    << ", my direction="
            //    << direction
            //    << std::endl;
         } else {
            // TODO: parameterize
            double speed_delta = 4;
            double new_speed = best_target.their_speed + speed_delta;
            bound(new_speed, 5.0, HUGE * 1.0);
            set_speed(new_speed);
            set_direction(Angle(best_target.bearing, Angle::RADIAN));
        
            locked_on = true;

            // PARAM
            timeout = best_target.distance / get_speed() / 4;
            // TODO: parameterize
            bound(timeout, 0.5, 20.0);
            radius = best_target.distance
                + Point::tolerance;
        }
    } else {
        set_speed(SPEED_RESTING);
        timeout = 10;
        radius = 100;
    }

    hunt_event = new Event(timeout, [=](void) { self->hunt(radius); });
}

void Aztec::spawn(void) {
    SmartPointer<Aztec> child = new Aztec;
    reproduce(child);
}

bool Aztec::is_family(String name) {
    String base = "Aztec";
    bool result = name.compare(0, base.length(), base) == 0;

    /* DEBUG */
    //std::cout << "NAMECHECK: '"
    //          << name
    //          << (result ? "' is family" : "' not family")
    //          << std::endl;

    return result;
}

// TODO: make this faster with std::to_string
String Aztec::serialize() const {
    std::stringstream sstm;
    sstm << id << ","
         << speed << ","
         << direction.deg();

    return sstm.str();
}
