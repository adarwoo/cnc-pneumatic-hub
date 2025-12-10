#include <asx/reactor.hpp>
#include <asx/ioport.hpp>

#include "conf_board.h"
#include "modbus.hpp"

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
   PRESSURE_READOUT.init(dir_t::in, invert::inverted);

   // Activate trace pins for debug
   TRACE_INFO.init(dir_t::out, value_t::low);
   TRACE_WARN.init(dir_t::out, value_t::low);
   TRACE_ERR.init(dir_t::out, value_t::low);

   net::modbus_slave::init();

   // Off we go!
   asx::reactor::run();
}
