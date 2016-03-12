#if !(_Maya_h)
#define _Maya_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"
#include "Angle.hpp"
#include "Vector.hpp"

class Maya : public LifeForm {
    private:
        enum Phylum {
            FAMILY = 1,
            ENEMY = 2,
            ALGAE = 3
        };
        Phylum get_phylum(std::string name);

        const double SPEED_RESTING = 1;
        const double RADIUS_DEFAULT = 100;

        long id;
        double speed;
        Angle direction;

        bool locked_on;

        Event* hunt_event;
        void hunt(double radius);
        Vector potential_fields(ObjList area_info);

        // update personal data
        void update_position(void);
        void age(void);

        ObjList sense(double radius);
        bool is_family(std::string name);
        std::string serialize(void) const;

        void set_direction(const Angle& course);
        void turn(const Angle& delta);
        void recurring(double timeout,
                const std::function <void (void)>& callback);
        void recurring(const std::function <double (void)>& timeout,
                const std::function <void (void)>& callback);
        // Nyquist sampling?
        double update_interval = 0.5;
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
