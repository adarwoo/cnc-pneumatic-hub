#include <asx/reactor.hpp>
#include "modbus.hpp"

extern "C" void board_init(void);

int main(void)
{
   board_init();
   hub::modbus_slave::init();

   // Off we go!
   asx::reactor::run();
}

