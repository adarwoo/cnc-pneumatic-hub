#include <array>
#include <boost/sml.hpp>
#include <ulog.h>

#include <asx/reactor.hpp>
#include <asx/ioport.hpp>

#include "conf_board.h"
#include "coils.hpp"

namespace coil
{
   using namespace asx;
   using namespace asx::ioport;

   struct open {};
   struct close {};
   struct update {};

   // Prototype for the reactor handler
   void on_update_lock(uint8_t index);

   // Reactor for the timer(s) of each coil
   reactor::Handle react_on_update[COUNT];

   // Auto assign the reactor
   uint8_t next_avail_reactor = 0;

   class Coil {
   protected:
      Pin pin;

      // Operation is not allowed
      bool lock;

      // Which reactor to use
      uint8_t reactor_index;

   public:
      Coil(Pin _pin) :
        pin(_pin),
        lock{false},
        reactor_index{next_avail_reactor++} {
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
         pin.set(level);
      }
   };

   struct PinStateMachineGenerator {
      auto operator()() {
         using namespace boost::sml;

         auto do_open  = [] (Coil *coil) { coil->set(false); };
         auto do_close = [] (Coil *coil) {
            uint8_t index = coil->get_reactor_index();

            react_on_update[index].delay(std::chrono::milliseconds(250), index);
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
      ManagedCoil(Pin _pin) : Coil(_pin), sm(static_cast<Coil*>(this)) {
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
      ManagedCoil{TOOL_SETTER_AIR_BLAST},
      ManagedCoil{CHUCK_CLAMP},
      ManagedCoil{SPINDLE_CLEAN},
      ManagedCoil{DOOR_PUSH},
      ManagedCoil{DOOR_PULL}
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

   void init() {
      // Force a zero on output the set as output
      TOOL_SETTER_AIR_BLAST.init(dir_t::out, value_t::low);
      CHUCK_CLAMP.init(dir_t::out, value_t::low);
      SPINDLE_CLEAN.init(dir_t::out, value_t::low);
      DOOR_PUSH.init(dir_t::out, value_t::low);
      DOOR_PULL.init(dir_t::out, value_t::low);
   }
} // namespace coil