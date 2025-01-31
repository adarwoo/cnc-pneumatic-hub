/*
 * pressure_monitor.c
 *
 * Created: 07/05/2024 16:50:47
 *  Author: micro
 */
#include "digital_input.h"
#include "conf_board.h"

#include "pressure_mon.h"


/** Pointer to the input */
digital_input_handle_t _di;


/** Initialize the input as sampled over 50 ms */
void pressure_mon_init(void)
{
   _di = digital_input(
      IOPORT_PRESSURE_READOUT, // IOPort to sample
      REACTOR_NULL_HANDLE,     // No reactor required
      IOPORT_SENSE_DISABLE,    // No sensing - sampling
      TIMER_MILLISECONDS(50)   // Sample over 50ms
   );
}

/** Get the port filtered value */
bool pressure_mon_get(void)
{
   return digital_input_value(_di);
}

