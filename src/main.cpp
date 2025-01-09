#include <asx/reactor.hpp>
#include "modbus.hpp"

int main(void)
{
   hub::modbus_slave::init();

   // Off we go!
   asx::reactor::run();
}

