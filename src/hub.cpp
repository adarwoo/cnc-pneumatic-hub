#include <array>
#include <asx/ioport.hpp>
#include <asx/reactor.hpp>

#include "hub.hpp"

using namespace asx;
using namespace asx::ioport;

static constexpr auto IOPORT_TOOL_SETTER_AIR_BLAST = PinDef(A, 4);
static constexpr auto IOPORT_CHUCK_CLAMP           = PinDef(A, 5);
static constexpr auto IOPORT_SPINDLE_CLEAN         = PinDef(A, 6);
static constexpr auto IOPORT_DOOR_PUSH             = PinDef(A, 7);
static constexpr auto IOPORT_DOOR_PULL             = PinDef(B, 3);

static constexpr auto IOPORT_PRESSURE_READOUT      = PinDef(B, 2);

auto _init = Pin(IOPORT_PRESSURE_READOUT).init(dir_t::in, invert::inverted);

namespace coil
{
   struct Coil : public Pin
   {
      Coil(PinDef _pin) : Pin(_pin) {
         init(dir_t::out, value_t::low);
      }
   };

   std::array<Coil, COUNT> coils = {
      IOPORT_TOOL_SETTER_AIR_BLAST,
      IOPORT_CHUCK_CLAMP,
      IOPORT_SPINDLE_CLEAN,
      IOPORT_DOOR_PUSH,
      IOPORT_DOOR_PULL
   };

   // Buffer for the next change
   signed char next = -1;

   // Accept changes
   bool accept_change = true;

   void on_apply_change() {
      accept_change = true;

      if ( next >= 0 ) {
         for ( uint8_t i=0; i<coils.size(); ++i) {
            if ( next == i ) {
               coils[i].set(true);
            } else {
               coils[i].set(false);
            }
         }
      }
   }

   auto react_on_apply_change = reactor::bind(on_apply_change);

   auto constexpr pressure_read_pin = PinDef{IOPORT_PRESSURE_READOUT};

   auto pressure_pin = Pin(pressure_read_pin).init(pullup::enabled);

   uint8_t get_all()
   {
      uint8_t retval = 0;

      for (uint8_t index = 0; index < coils.size(); ++index)
      {
         retval = (retval << 1) | *coils[index];
      }

      return retval;
   }

   bool get(uint8_t index)
   {
      return *coils[index];
   }

   bool set(uint8_t index, bool on) {
      using namespace std::chrono;
      bool result = true;

      if (accept_change) {
         coils[index].set(on);

         accept_change = false; // Start a cool-off period
         react_on_apply_change.delay(250ms);
      } else {
         if (on) {
               if (next >= 0) {
                  // Pending operation already in progress!
                  result = false;
               } else {
                  if (not *coils[index]) { // OK if the switch is aleady on
                     next = index;
                  }
               }
         } else {
            // OK to turn off in all cases
            coils[index].set(false);
         }
      }

      return result;
   }

   void release() {
      for ( auto coil : coils ) {
         coil.set(false);
      }
   }

   bool read_pressure()
   {
      return *pressure_pin;
   }
}