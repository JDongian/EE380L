#if !(_Aztec_h)
#define _Aztec_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"
#include "Angle.hpp"

class Aztec : public LifeForm {
    private:
        const double SPEED_RESTING = 1;
        const double RADIUS_DEFAULT = 100;

        long id;
        double speed;
        Angle direction;

        bool locked_on;

        Event* hunt_event;
        void hunt(double radius);

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
        Aztec(void);
        ~Aztec(void);
        Color my_color(void) const;
        static SmartPointer<LifeForm> create(void);
        virtual std::string species_name(void) const;
        virtual std::string player_name(void) const;
        virtual Action encounter(const ObjInfo&);
        friend class Initializer<Aztec>;
};

#endif /* !(_Aztec_h) */
