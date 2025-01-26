#include <array>
#include <boost/sml.hpp>

#include <asx/reactor.hpp>

#include "hub.hpp"

#include "conf_board.h"
#include <ioport.h>

using namespace asx;

// Reactor for the timer(s) of each coil
reactor::Handle react_on_update[5];

// Auto assign the reactor
uint8_t next_avail_reactor = 0;


namespace coil
{
   struct open {};
   struct close {};
   struct update {};

   // Prototype for the reactor handler
   void on_update_lock(uint8_t index);

   class Coil {
   protected:
      ioport_pin_t pin;

      // Operation is not allowed
      bool lock;

      // Which reactor to use
      uint8_t reactor_index;

   public:
      Coil(ioport_pin_t _pin) : pin(_pin), lock{false}, reactor_index{next_avail_reactor++} {
      }

      bool is_locked() {
         return lock;
      }

      uint8_t get_reactor_index() {
         return reactor_index;
      }

      void set_lock(bool state) {
         lock = state;
      }

      void set(bool level) {
         ioport_set_pin_level(pin, level);
      }
   };

   struct PinStateMachineGenerator {
      auto operator()() {
         using namespace boost::sml;

         auto do_open  = [] (Coil *coil) { coil->set(false); };
         auto do_close = [] (Coil *coil) {
            using namespace std::chrono;

            uint8_t index = coil->get_reactor_index();

            react_on_update[index].delay(250ms, index);
            coil->set(true);
            coil->set_lock(true);
         };

         auto locked = [] (Coil *coil) { return coil->is_locked(); };

         return make_transition_table(
         * "opened"_s       + event<close> [locked]            = "should_close"_s
         , "opened"_s       + event<close>          / do_close = "closed"_s
         , "closed"_s       + event<open>           / do_open  = "opened"_s
         , "should_close"_s + event<open>           / do_open  = "opened"_s
         , "should_close"_s + event<update>         / do_close = "closed"_s
         );
      }
   };

   class ManagedCoil : public Coil
   {
      boost::sml::sm<PinStateMachineGenerator> sm;
   public:
      ManagedCoil(ioport_pin_t _pin) : Coil(_pin), sm(static_cast<Coil*>(this)) {
         react_on_update[reactor_index] = reactor::bind(on_update_lock);
      }

      // Called by the reactor on expiry
      void do_update() {
         lock = false;
         sm.process_event(update{});
      }

      bool open_close(bool on_off = true) {
         bool retval = true;

         if ( on_off == false ) {
            sm.process_event(open{});
         } else {
            using namespace boost::sml;

            sm.process_event(close{});
            retval = sm.is("closed"_s);
         }

         return retval;
      }

      bool get() {
         using namespace boost::sml;
         return sm.is("opened"_s) == false;
      }
   };

   std::array<ManagedCoil, COUNT> coils = {
      IOPORT_TOOL_SETTER_AIR_BLAST,
      IOPORT_CHUCK_CLAMP,
      IOPORT_SPINDLE_CLEAN,
      IOPORT_DOOR_PUSH,
      IOPORT_DOOR_PULL
   };

   void on_update_lock(uint8_t index) {
      coils[index].do_update();
   }

   bool get(uint8_t index) {
      return coils[index].get();
   }

   uint8_t get_all() {
      uint8_t retval = 0;

      for (uint8_t index = 0; index < coils.size(); ++index) {
         retval = (retval << 1) | coils[index].get();
      }

      return retval;
   }

   bool set(uint8_t index, bool on) {
      return coils[index].open_close(on);
   }

   bool read_pressure() {
      return ioport_get_pin_level(IOPORT_PRESSURE_READOUT);
   }
}