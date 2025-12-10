#include <asx/reactor.hpp>
#include <asx/ioport.hpp>

#include "conf_board.h"
#include "modbus.hpp"
#include "pressure_mon.hpp"

using namespace asx::ioport;

int main(void)
{
   // Force a zero on output the set as output
   TOOL_SETTER_AIR_BLAST.init(dir_t::out, value_t::low);
   CHUCK_CLAMP.init(dir_t::out, value_t::low);
   SPINDLE_CLEAN.init(dir_t::out, value_t::low);
   DOOR_PUSH.init(dir_t::out, value_t::low);
   DOOR_PULL.init(dir_t::out, value_t::low);

   // Since the input is active when the switch closes, invert
   MIN_PRESSURE_READOUT.init(dir_t::in, invert::inverted);
   MAIN_PRESSURE_READOUT.init(dir_t::in, invert::inverted);

   // Activate trace pins for debug
   TRACE_INFO.init(dir_t::out, value_t::low);
   TRACE_WARN.init(dir_t::out, value_t::low);
   TRACE_ERR.init(dir_t::out, value_t::low);

   // Initialise the Modbus slave system
   net::modbus_slave::init();

   // Sample the pressure inputs every 10ms
   asx::reactor::bind([]{
      uint8_t raw_inputs = (MIN_PRESSURE_READOUT.get() << 0) |
                           (MAIN_PRESSURE_READOUT.get() << 1);

      pressure_readout.append(raw_inputs);
   }).repeat(std::chrono::milliseconds(10));

   // Off we go!
   asx::reactor::run();
}
