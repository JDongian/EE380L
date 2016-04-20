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

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
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

ObjList LifeForm::perceive(double dist){
	ObjList nearby_obj;
	this->update_position();
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!is_alive) {
		return nearby_obj;
	}
	if(dist>max_perceive_range) dist=max_perceive_range;
	if(dist<min_perceive_range) dist=min_perceive_range;
	std::vector<SmartPointer<LifeForm>> nearby_lifeforms=space.nearby(pos,dist);
//	auto nearby_lifeforms=space.nearby(pos,dist);
	for(auto v: nearby_lifeforms){
		nearby_obj.push_back(info_about_them(v));
	}
	energy-=perceive_cost(dist);
	if(energy<min_energy){
		this->die();
	}
	return nearby_obj;
}

void LifeForm::border_cross(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!is_alive) {
		return;
	}
	update_position();
	check_encounter();
	compute_next_move();
}

void LifeForm::update_position(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}

	if(!is_alive ) {
		return;
	}

	double elapsed_time=Event::now()-update_time;
	assert(elapsed_time>=0);

	if(elapsed_time<=0.001) {
		return;
	}

	energy=energy-movement_cost(speed,elapsed_time);
	if(energy<min_energy){
		this->die();
		return;
	}
	double distance_traveled=Point::tolerance+speed*elapsed_time;
	Point old_pos=pos;
	Point new_pos;
	new_pos.xpos=pos.xpos+distance_traveled*cos(course);
	new_pos.ypos=pos.ypos+distance_traveled*sin(course);
	if(space.is_out_of_bounds(new_pos)){
		this->die();
		return;
	}

	pos=new_pos;
	update_time=Event::now();
	/*This must come before space.update_position(), since space.update_position() can
	 * trigger callback to "this" LifeForm, and the callback may depend on this object's
	 * update_time, e.g., LifeForm::update_position() (iterative call), crash may happen.
	 */
	space.update_position(old_pos,pos);

//	space.update_position(pos,new_pos);
//	pos=new_pos;
}


void LifeForm::compute_next_move(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!is_alive) {
		return;
	}
	if(speed<=0){
		return;
	}
	double cross_time=Point::tolerance+space.distance_to_edge(pos,course)/speed;
	SmartPointer<LifeForm> obj(this);
	border_cross_event=new Event(cross_time,[obj](void){obj->border_cross();});
}

void LifeForm::set_course(double c){

	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!is_alive) {
		return;
	}
	border_cross_event->cancel();
	update_position();
	course=c;
	compute_next_move();
}

void LifeForm::set_speed(double s){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!is_alive) {
		return;
	}
	border_cross_event->cancel();
	update_position();
	speed=(s>max_speed)?max_speed:s;
	compute_next_move();
}

void LifeForm::reproduce(SmartPointer<LifeForm> child){
	update_position();
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!is_alive) {
		child=nullptr;
		return;
	}
	if(Event::now()<reproduce_time+min_reproduce_time) {
		child=nullptr;
		return;
	}
	if(!child) {
		return;
	}

	SmartPointer<LifeForm> nearest;
	int count=0;
	do{
		double dist=drand48()*(reproduce_dist-encounter_distance)+encounter_distance+0.001;
		double theta=drand48()*2*M_PI;
		child->pos.xpos=pos.xpos+dist*cos(theta);
		child->pos.ypos=pos.ypos+dist*sin(theta);
//		if(space.is_out_of_bounds(child->position())) break;//////////////////
		while(space.is_out_of_bounds(child->position())){
			double dist=drand48()*(reproduce_dist-encounter_distance)+encounter_distance+0.001;
			double theta=drand48()*2*M_PI;
			child->pos.xpos=pos.xpos+dist*cos(theta);
			child->pos.ypos=pos.ypos+dist*sin(theta);
		}
		nearest=space.closest(child->position());
		if(++count>5){
			child=nullptr;
			return;
		}
	}
	while(nearest && nearest->position().distance(child->pos)<=encounter_distance);

	if(space.is_out_of_bounds(child->position())) {
		child=nullptr;
		return;
	}
	child->start_point=child->pos;

	energy*=0.5*(1-reproduce_cost);
	if(energy<min_energy){
		this->die();
		child=nullptr;
		return;
	}
	child->energy=energy;
	reproduce_time=Event::now();
	space.insert(child,child->pos,[child](void){child->region_resize();});
	new Event(age_frequency, [child](void){child->age();});
	child->is_alive=true;
}

void LifeForm::age(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!is_alive) {
		return;
	}
	energy-=age_penalty;
	if(energy<min_energy){
		this->die();
		return;
	}
	SmartPointer<LifeForm> obj(this);
	new Event(age_frequency,[obj](void){obj->age();});
}

void LifeForm::region_resize(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!is_alive) {
		return;
	}
	border_cross_event->cancel();
	update_position();
	compute_next_move();
}

void LifeForm::check_encounter(){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!is_alive) {
		return;
	}
	SmartPointer<LifeForm> obj(this);
	SmartPointer<LifeForm> nearest=space.closest(obj->position());
	if(!nearest) {
		return;
	}
	nearest->update_position();
	if(space.is_out_of_bounds(nearest->position()) || nearest->energy<min_energy){
		nearest->die();
		return;
	}
	if(nearest->is_alive && nearest->position().distance(obj->position())<=encounter_distance){
		obj->energy-=encounter_penalty;
		if(obj->energy<min_energy) {
			obj->die();
		}
		nearest->energy-=encounter_penalty;
		if(nearest->energy<min_energy) {
			nearest->die();
		}
		obj->resolve_encounter(nearest);
	}
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> that){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
	}
	if(!that) {
		return;
	}
	if(space.is_out_of_bounds(that->position()) || that->energy<min_energy){
		that->die();
	}
	if(!(is_alive || that->is_alive)) {
		return;  //if both are dead, then return
	}
	SmartPointer<LifeForm> obj(this);
	if(is_alive && that->is_alive){ //if both alive, do encounter on both and eat may be called
		Action action1=obj->encounter(obj->info_about_them(that));
		Action action2=that->encounter(that->info_about_them(obj));
		if(action1==Action::LIFEFORM_EAT && action2==Action::LIFEFORM_EAT){
			double roll1=drand48();
			double roll2=drand48();
			double chance1=eat_success_chance(energy,that->energy);
			double chance2=eat_success_chance(that->energy,energy);
			if(roll1<chance1 && roll2<chance2){
				switch(encounter_strategy){
				case EncounterResolver::BIG_GUY_WINS:{
					if(energy>that->energy) obj->eat(that);
					else that->eat(obj);
					break;
				}
				case EncounterResolver::FASTER_GUY_WINS:{
					if(speed>that->speed) obj->eat(that);
					else that->eat(obj);
					break;
				}
				case EncounterResolver::SLOWER_GUY_WINS:{
					if(speed<=that->speed) obj->eat(that);
					else that->eat(obj);
					break;
				}
				case EncounterResolver::EVEN_MONEY:{
					double roll = drand48();
					if(roll<=0.5) obj->eat(that);
					else that->eat(obj);
					break;
				}
				default:{
					double roll = drand48();
					if(roll<=0.5) obj->eat(that);
					else that->eat(obj);
				}
				}
			}
			else if(roll1<chance1){
				obj->eat(that);
			}
			else if(roll2<chance2){
				that->eat(obj);
			}
		}
		else if(action1==Action::LIFEFORM_EAT){
			double roll=drand48();
			if(roll<eat_success_chance(energy,that->energy)){
				obj->eat(that);
			}
		}
		else if(action2==Action::LIFEFORM_EAT){
			double roll=drand48();
			if(roll<eat_success_chance(that->energy,energy)){
				that->eat(obj);
			}
		}
	}
	/*if only one alive, then do encounter on the alive one, since encounter may
	 *  modify the behavior of that lifeform, but no eat will be called
	 */
	else if(is_alive){
		obj->encounter(obj->info_about_them(that));
	}
	else if(that->is_alive){
		that->encounter(that->info_about_them(obj));
	}
}

void LifeForm::eat(SmartPointer<LifeForm> that){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!that) {
		return;
	}
	if(space.is_out_of_bounds(that->position()) || that->energy<min_energy) {
		that->die();
		return;
	}
	if(!(is_alive && that->is_alive)) {
		return;
	}
	double gain=that->energy;
	that->die();
	energy-=eat_cost_function(0,0);
	if(energy<min_energy){
		this->die();
		return;
	}
	SmartPointer<LifeForm> obj(this);
	new Event(digestion_time,[obj,gain](void){obj->gain_energy(gain);});
}

void LifeForm::gain_energy(double e){
	if(space.is_out_of_bounds(pos) || energy<min_energy) {
		this->die();
		return;
	}
	if(!is_alive) {
		return;
	}
	energy+=e*eat_efficiency;
}
