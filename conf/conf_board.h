#pragma once
/**
 * Defines all I/Os of the board using Pin
 * Compatible with C and C++ use (ioport.h or asx/ioport.hpp)
 * In C++, the namespace asx::ioport must be used
 */

/************************************************************************/
/* Debug pins                                                           */
/* Pin DEBUG_REACTOR_IDLE is available on the uPDI connector on Pin 3   */
/* UART0 TxD is available on the uPDI connector on Pin 5                */
/************************************************************************/
#define TRACE_INFO           IOPORT(A, 2)
#define TRACE_ERR            IOPORT(B, 0)

/************************************************************************/
/* Wire debug pin for alert and reactor                                 */
/************************************************************************/
#undef DEBUG_REACTOR_IDLE
#define DEBUG_REACTOR_BUSY    TRACE_INFO
#define ALERT_OUTPUT_PIN      TRACE_ERR

/************************************************************************/
/* Pneumatic control pins                                               */
/************************************************************************/
#define TOOL_SETTER_AIR_BLAST IOPORT(A, 3)
#define CHUCK_CLAMP           IOPORT(A, 5)
#define SPINDLE_CLEAN         IOPORT(A, 6)
#define DOOR_PUSH             IOPORT(A, 7)
#define DOOR_PULL             IOPORT(B, 3)

#define MAIN_PRESSURE_READOUT IOPORT(B, 1)
#define MIN_PRESSURE_READOUT  IOPORT(B, 2)