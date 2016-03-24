#if !(_Maya_h)
#define _Maya_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Exploration.hpp"
#include "SerialUtils.hpp"

class Maya : public LifeForm {
    private:
        enum Phylum {
            FAMILY = 1,
            ENEMY = 2,
            ALGAE = 3
        };
        Phylum get_phylum(std::string name);

        // TODO: parameterize
        const double SPEED_RESTING = 1;
        const double RADIUS_DEFAULT = 100;
        // PARAM
        const double MARGIN_WIDTH = grid_max / 16; // TODO: bound, by max_speed * 1 ?
 
        long id = lrand48();
        double speed;
        Angle direction;
        Vector relative_position;

        //int times_reproduced = 0; TODO: for sex

        SimTime last_update;
        void update_position(void);

        void set_mspeed(const double& speed);
        void set_direction(const Angle& course);
        void turn(const Angle& delta);

        Exploration exploration;

        bool locked_on;

        Event* action_event;
        void action(double radius);
        Vector potential_fields(ObjList area_info);

        ObjList sense(double radius);
        bool is_family(std::string name);
        std::string serialize(void) const;
        void deserialize(std::string serial,
                double& id, Vector& rel_pos, Exploration& exp) const;

        void avert_edge(void);
        void recurring(double timeout,
                const std::function <void (void)>& callback);
        void recurring(const std::function <double (void)>& timeout,
                const std::function <void (void)>& callback);
        // Nyquist sampling?
        const double UPDATE_INTERVAL = 0.5;
    protected:
        static void initialize(void);
        void spawn(void);
        void startup(void);
    public:
        Maya(void);
        ~Maya(void);
        Color my_color(void) const;
        static SmartPointer<LifeForm> create(void);
        virtual std::string species_name(void) const;
        virtual std::string player_name(void) const;
        virtual Action encounter(const ObjInfo&);
        friend class Initializer<Maya>;
};
#endif /* !(_Maya_h) */
