/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

#include <cmath>

using namespace std;

template <typename T>
T bound(T& x, const T& min, const T& max) {
    assert(min < max);
    if (x > max) { x = max; }
    if (x < min) { x = min; }
    return x;
}

ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
    ObjInfo info;

    info.species = neighbor->species_name();
    info.health = neighbor->health();
    info.distance = pos.distance(neighbor->position());
    info.bearing = pos.bearing(neighbor->position());
    info.their_speed = neighbor->speed;
    info.their_course = neighbor->course;
    return info;
}

void LifeForm::reproduce(SmartPointer<LifeForm> l) {
}

/* Percieve has three params in Params.h that must be used. max_perceive_range
 * and min_perceive_range define the maximum and minimum radii for the
 * perception circle. If a LifeForm tries to perceive with a larger or smaller
 * radius, you should set the radius to the maximum (or minimum), and then
 * invoke the perceive code as normally. Each time  an object attempts to
 * perceive, it should be charged the perceive_cost. Note that perceive_cost
 * is a function of the radius.
 */
ObjList LifeForm::perceive(double radius) {
    ObjList area_info{};

    bound(radius, min_perceive_range, max_perceive_range);

    for(auto&& lifeform: space.nearby(pos, radius)) {
        area_info.push_back(info_about_them(lifeform)); // TODO: understand memory
    }

    lose_energy(perceive_cost(radius));

    return area_info;
}

/* It costs energy to exist. Stationary, isolated objects eventually die. You
 * should schedule an event every age_frequency time units. The event should
 * subtract age_penalty units of energy from the LifeForm. If the energy drops
 * below min_energy, the LifeForm should die. Note that it is not acceptable
 * to apply the age_penalty to all LifeForms at the same time. Not all
 * LifeForms are born at the same time (some are children of other LifeForms)
 * and so they shouldn't be penalized at the same time.
 */
void LifeForm::age(void) {
    if (!is_alive) { return; }

    SmartPointer<LifeForm> self{ this };
    age_event = new Event(age_frequency, [self](void) { self->age(); });

    lose_energy(age_penalty);
}

/* Computes the new position, charge for energy consumed. To compute the new
 * position, you'll need to know how long it has been since the last time
 * you'd called update_position on this object (how much delta time has
 * passed). You'll need a data member called update_time. Each time
 * update_position is called, use Event::now() to see how much time has passed
 * since update_time. Calculate the new position by multiplying speed by delta
 * time. Set update_time equal to Event::now() when you're done.
 *
 * If less than Time::tolerance time units have passed since the last call to
 * update_position, then do nothing.
 */
void LifeForm::update_position(void) {
    if (!is_alive) { return; }

    SimTime delta_time = Event::now() - update_time;

    if (delta_time >= min_delta_time) {
        Point delta_position (cos(course), sin(course));
        delta_position *= speed * delta_time;
        Point new_position (pos + delta_position);

        //std::cout << "old position: " << pos
        //          //<< ", position change: " << delta_position
        //          << "\t new position: " << new_position
        //          << std::endl;

        update_time = Event::now();
        if (space.is_out_of_bounds(new_position)) {
            die_and_clean();
        } else {
            space.update_position(pos, new_position);
            pos = new_position;

            lose_energy(movement_cost(speed, delta_time));
        }
    }
}

/* This is your movement event handler function. Calls update_position and
 * then schedules the next movement event.
 */
void LifeForm::border_cross(void) {
    update_position();
    check_encounter(); // order probably doesn't matter here TODO: confirm
    compute_next_move();
}

/* This function will be a callback from the QuadTree. When another object is
 * created nearby, your object needs to determine the next possible time that
 * you could collide. The QuadTree knows when objects are inserted inside it,
 * so it can invoke this callback function on your objects. What you will want
 * to do is have region_resize cancel any pending border crossing events,
 * update your position, and schedule the next movement event (since the
 * region has changed size, you will encounter the boundary at a different
 * time than than before.)
 */
void LifeForm::region_resize(void) { 
    update_position();
    compute_next_move();
}

/* Cancel any pending border_cross event. Update the current position of the
 * object and then schedule new border_cross event based on the new course and
 * speed. Note that an object that is stationary will never cross a border.
 */
void LifeForm::set_course(double new_course) {
    update_position();
    course = new_course;
    compute_next_move();
}

/* Cancel any pending border_cross event. Update the current position of the
 * object and then schedule new border_cross event based on the new speed and
 * speed. Note that an object that is stationary will never cross a border.
 */
void LifeForm::set_speed(double new_speed) {
    update_position();
    speed = bound(new_speed, 0.0, max_speed);
    compute_next_move();
}

void LifeForm::compute_next_move(void) {
    if (!is_alive) { return; }

    if (border_cross_event) {
        border_cross_event->cancel();
    }
 
    if (speed == 0) {
        border_cross_event = nullptr;
    } else {
       double distance = space.distance_to_edge(pos, course);
       double eta = distance / speed + Point::tolerance;
       SmartPointer<LifeForm> self{ this };

       border_cross_event = new Event(eta,
               [self](void) { self->border_cross(); });
    }
}

void LifeForm::check_encounter(void) {
    if (!is_alive) { return; }

    SmartPointer<LifeForm> closest = space.closest(pos);
    closest->update_position();
    if (closest->is_alive) {
        double distance = pos.distance(closest->pos);
        if (distance < encounter_distance) {
            resolve_encounter(closest);
        }
    }
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm>other) {
    if (!is_alive) { return; }
    if (!other->is_alive) { return; }

    // TODO: implement
}

void LifeForm::energy_check() {
    if (energy < min_energy) {
        die_and_clean();
    }
}

void LifeForm::lose_energy(double loss) {
    assert(loss >= 0);
    energy -= loss;
    energy_check();
}

void LifeForm::gain_energy(double gain) {
    assert(gain >= 0);
    energy += gain;
}

void LifeForm::die_and_clean() {
    if (border_cross_event) {
        border_cross_event->cancel();
    }
    if (age_event) {
        age_event->cancel();
    }
    die();
}
