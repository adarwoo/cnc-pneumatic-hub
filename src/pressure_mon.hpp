#pragma once
/**
 * pressure_mon.hpp
 *
 */
#include <asx/ioport.hpp>
#include <asx/reactor.hpp>
#include <asx/debouncer.hpp>

#include "conf_board.h"

namespace pressure_mon {
   namespace detail {
      inline auto pressure_readout = asx::Debouncer<2, 4>{};
   }

   /** Return a composite pressure status */
   static inline uint8_t value() {
      auto status = detail::pressure_readout.status();

      return (status.get(1) << 1) | status.get(0);
   }

   /** Return the pressure status */
   static inline auto status() {
      return detail::pressure_readout.status();
   }

   static inline void init() {
      using namespace asx::ioport;

      // Since the input is active when the switch closes, invert

      // These inputs are connected to microswitch which are closed when OK
      // The input is active high when the switch is open (alarm condition)
      LOW_PRESSURE_ALARM.init(dir_t::in);
      WATER_PUMP_ALARM.init(dir_t::in);

      // Sample the pressure inputs every 10ms
      asx::reactor::bind([]{
         uint8_t raw_inputs =
            *LOW_PRESSURE_ALARM | (*WATER_PUMP_ALARM << 1);

         detail::pressure_readout.append(raw_inputs);
      }).repeat(std::chrono::milliseconds(10));
   }
}
