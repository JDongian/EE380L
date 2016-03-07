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
    age_event = new Event(age_frequency, [=](void) { self->age(); });

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

/* When calculating the time until the next border crossing event (something
 * you need to do from inside set_course, set_speed, region_resize and
 * border_cross), fudge. Specifically add Point::tolerance time units to the
 * time. That way, when the time is up, the LifeForm will not only have
 * reached the edge of the boundary, but it is certain to have crossed the
 * edge. If you make the mistake of scheduling the event for when the object
 * is exactly on the edge, you will run the risk of the object not actually
 * leaving its region. Due to floating-point roundoff errors, the object could
 * be 10e-15 space units from the edge (or so). Effectively, your program
 * could go into an infinite loop, as the object keeps getting closer and
 * closer to the edge without ever really crossing it.
 */
void LifeForm::compute_next_move(void) {
    if (!is_alive) { return; }

    if (border_cross_event) {
        border_cross_event->cancel();
    }
 
    if (speed == 0) {
        border_cross_event = nullptr;
    } else {
       auto distance = space.distance_to_edge(pos, course);
       auto eta = distance / speed + Point::tolerance;
       SmartPointer<LifeForm> self{ this };

       border_cross_event = new Event(eta,
               [=](void) { self->border_cross(); });
    }
}

/* Collisions occur only between two LifeForms. When a LifeForm moves to a new
 * region, it collides with the closest LifeForm that is within 1.0 space
 * units. The test program will not check for collisions between three or more
 * LifeForms. However, your simulator must not crash when this happens.
 */
void LifeForm::check_encounter(void) {
    if (!is_alive) { return; }

    auto closest = space.closest(pos);
    closest->update_position();
    auto distance = pos.distance(closest->pos);

    if (distance < encounter_distance) {
        resolve_encounter(closest);
    }
}

/* When a collision occurs, simulator must invoke the encounter method on each
 * of the colliding life forms. The encounter method is pure virtual
 * (abstract) and must be implemented by every derived LifeForm class
 * (Craig, Algae). Be careful, the derived LifeForm can do arbitrarily
 * sophisticated things inside its encounter method. That includes calling
 * perceive, or set_course, or even reproduce. The encounter method returns
 * either EAT or IGNORE . Since there are two LifeForms colliding, there are
 * four cases; IGNORE/IGNORE, EAT/IGNORE, IGNORE/EAT and EAT/EAT. Provided at
 * least one LifeForm attempts to eat, you should generate a random number and
 * compare the result to eat_success_chance(eater->energy, eatee->energy).
 */
void LifeForm::resolve_encounter(SmartPointer<LifeForm> that) {
    if (!is_alive) { return; }
    if (!that->is_alive) { return; }

    lose_energy(encounter_penalty);
    that->lose_energy(encounter_penalty);

    if (is_alive && that->is_alive) {
        SmartPointer<LifeForm> self{ this };
        auto send_action = encounter(info_about_them(that));
        auto recv_action = that->encounter(that->info_about_them(self));
        bool try_send_eat = send_action == LIFEFORM_EAT &&
                            drand48() < eat_success_chance(energy, that->energy);
        bool try_recv_eat = recv_action == LIFEFORM_EAT &&
                            drand48() < eat_success_chance(that->energy, energy);
    
        if (try_send_eat && !try_recv_eat) {
            eat(that);
        } else if (!try_send_eat && try_recv_eat) {
            that->eat(self);
        } else if (try_send_eat && try_recv_eat) {
            // TODO: swap this out for https://piazza.com/class/ik5telvhcgio3?cid=63
            switch (encounter_strategy) {
                case EVEN_MONEY:
                    if (mrand48() < 0) {
                        eat(that);
                    } else {
                        that->eat(self);
                    }
                    break;
                case BIG_GUY_WINS:
                    if (energy > that->energy) {
                        eat(that);
                    } else {
                        that->eat(self);
                    }
                    break;
                case UNDERDOG_IS_HERE:
                    if (energy < that->energy) {
                        eat(that);
                    } else {
                        that->eat(self);
                    }
                    break;
                case FASTER_GUY_WINS:
                    if (speed > that->speed) {
                        eat(that);
                    } else {
                        that->eat(self);
                    }
                    break;
                case SLOWER_GUY_WINS:
                    if (speed < that->speed) {
                        eat(that);
                    } else {
                        that->eat(self);
                    }
                    break;
                default:
                    assert(0); // Unknown encounter strategy
            }
        }
    }
}

/* Objects eat each other according to their desire (indicated by the return
 * value of encounter) and their eat_success_chance (defined in Params.h). If
 * object A wants to eat object B, then the simulator should generate a random
 * number and compare it to the eat_success_chance(A, B). If the random number
 * is less than the eat success chance, then A gets to eat B.
 *
 * Watch out, B could also be trying to eat A at the same time. Your simulator
 * must allow stationary (or slow) objects to eat other objects (and
 * vice-versa). Your simulator must not allow two objects to both succefully
 * eat eath other. Params.h includes a variable called encounter_strategy
 * which explains how to break the tie. To make the implementations uniform,
 * we have set the tie-breaking rule so that the faster object gets to eat the
 * slower object in the tie-break situlation. Note: there's a tie only if both
 * A and B generate random numbers (they must each generate their own number)
 * that are less than their eat_success_chances. In this case only do you use
 * encounter_strategy to break the tie.
 *
 * Once you've figured out who got to eat whom, you have to award the victor
 * the spoils. The rules for this are for the eater to gain the energy of the
 * eatee, but with two caveats.
 *
 * 1. The energy is awarded to the eatee after exactly digestion_time time
 *    units have passed (create an event).
 * 2. The energy awarded is reduced by the eat_efficiency multiplier.
 *
 * Since eat_efficiency is less than 1, it is usually not a good idea to eat
 * your own young.
 */
void LifeForm::eat(SmartPointer<LifeForm> target) {
    //std::cout << "EATING: "
    //          << species_name()
    //          << " is eating "
    //          << target->species_name()
    //          << " in "
    //          << digestion_time
    //          << " frames."
    //          << std::endl;

    SmartPointer<LifeForm> self{ this };
    auto gain = eat_efficiency * target->energy;
    target->die_and_clean();

    lose_energy(eat_cost_function(0, 0));

    if (is_alive) {
        new Event(digestion_time, [=](void) { self->gain_energy(gain); });
    }
}

void LifeForm::energy_check() {
    if (energy < min_energy) {
        die_and_clean();
    }
}

void LifeForm::lose_energy(double loss) {
    assert(loss >= 0);

    if (!is_alive) { return; }

    //if (loss != 0) {
    //std::cout << "ENERGY: -"
    //          << loss
    //          << " ("
    //          << species_name()
    //          << ")"
    //          << std::endl;}
 
    energy -= loss;
    energy_check();
}

void LifeForm::gain_energy(double gain) {
    assert(gain >= 0);

    if (!is_alive) { return; }

    //std::cout << "ENERGY: +"
    //          << gain
    //          << " ("
    //          << species_name()
    //          << ")"
    //          << std::endl;
 
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
