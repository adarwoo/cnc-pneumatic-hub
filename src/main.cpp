#include <asx/reactor.hpp>
#include "modbus.hpp"

extern "C" void board_init(void);

int main(void)
{
   board_init();
   net::modbus_slave::init();

   // Off we go!
   asx::reactor::run();
}

