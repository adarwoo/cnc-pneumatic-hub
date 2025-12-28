/**
 * @file main.cpp
 * @author software@arreckx.com
 * @brief Main application entry point
 */
#include <asx/reactor.hpp>
#include <ulog.h>

#include "modbus.hpp"
#include "pressure_mon.hpp"
#include "coils.hpp"


int main(void)
{
   ULOG_MILE("System startup");

   // Initialise the Modbus slave system
   net::modbus_slave::init();

   // Initialize the pressure monitoring system
   pressure_mon::init();

   // Initialise the coils
   coil::init();

   // Off we go!
   asx::reactor::run();
}
