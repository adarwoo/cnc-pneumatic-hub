#include <asx/reactor.hpp>
#include <asx/ioport.hpp>

#include "conf_board.h"
#include "modbus.hpp"
#include "pressure_mon.hpp"
#include "coils.hpp"

using namespace asx::ioport;

int main(void)
{
   // Activate trace pins for debug
   TRACE_INFO.init(dir_t::out, value_t::low);
   TRACE_WARN.init(dir_t::out, value_t::low);
   TRACE_ERR.init(dir_t::out, value_t::low);

   // Initialise the Modbus slave system
   net::modbus_slave::init();

   // Initialize the pressure monitoring system
   pressure_mon::init();

   // Initialise the coils
   coil::init();

   // Off we go!
   asx::reactor::run();
}
