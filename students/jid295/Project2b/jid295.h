#if !(_jid295_h)
#define _jid295_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Exploration.hpp"
#include "SerialUtils.h"
#include "Gene.hpp"

class jid295 : public LifeForm {
    private:
        enum Phylum {
            FAMILY = 1,
            ENEMY = 2,
            ALGAE = 3
        };
        Phylum get_phylum(const ObjInfo& info);

        Gene* gene;
        
        // Nyquist sampling?
        const double UPDATE_INTERVAL = 0.5;
        
        const double RESET_INTERVAL = 500;
 
        long id = lrand48();
        double speed;
        Angle direction;
        Vector relative_position;

        bool locked_on;

        Exploration exploration;

        //int times_reproduced = 0; TODO: for sex

        SimTime last_update;
        void update_position(void);
        void reset_position();

        void set_mspeed(const double& speed);
        void set_direction(const Angle& course);
        void turn(const Angle& delta);

        Event* action_event;
        void action(double radius);
        Vector potential_fields(ObjList area_info);

        ObjList sense(double radius);
        bool is_family(const ObjInfo& info);
        std::string serialize(void) const;
        bool deserialize(std::string serial,
                double& id,
                Vector& rel_pos,
                Exploration& exp,
                double& course,
                double& speed,
                Gene& params) const;

        void avert_edge(void);
        void recurring(double timeout,
                const std::function <void (void)>& callback);
        void recurring(const std::function <double (void)>& timeout,
                const std::function <void (void)>& callback);

        double score_enemy_health(double enemy_health, double my_health);
        double score_algae_health(double health);
        Vector gen_family_force(ObjInfo family);
        Vector gen_enemy_force(ObjInfo enemy, double my_health);
        Vector gen_algae_force(ObjInfo algae);
        Vector gen_edge_force(Vector norm_pos, double margin_width);
    protected:
        static void initialize(void);
        void spawn(void);
        void startup(void);
    public:
        jid295(void);
        jid295(Gene*);
        ~jid295(void);
        Color my_color(void) const;
        static SmartPointer<LifeForm> create(void);
        virtual std::string species_name(void) const;
        virtual std::string player_name(void) const;
        virtual Action encounter(const ObjInfo&);
        friend class Initializer<jid295>;
};
#endif /* !(_jid295_h) */
