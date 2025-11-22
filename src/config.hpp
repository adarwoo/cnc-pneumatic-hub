#pragma once
/**
 * @file config.hpp
 * @brief Configuration management.
 * @details This file provides functions to manage the configuration,
 *          including reading and writing configuration parameters to EEPROM.
 *          It includes settings for Modbus
 */

#include <cstdint>
#include <asx/uart.hpp>


namespace config {
   /**
    * Specific type for storing the baud rate as 1 bytes
    */
   enum class baud_t : uint8_t {
      _300    = 0,
      _600    = 1,
      _1200   = 2,
      _2400   = 3,
      _4800   = 4,
      _9600   = 5,
      _19200  = 6,
      _38400  = 7,
      _57600  = 8,
      _115200 = 9
   };

   /**
    * @brief Configuration structure stored in EEPROM.
    * This structure contains all the configuration parameters for the relay module.
    */
   struct EepromConfig {
      /// @brief Modbus config
      uint8_t address;
      /// @brief UART setup
      baud_t baud; // Baud rate selection
      asx::uart::stop stopbits;
      asx::uart::parity parity;
   };

   // Run-time configuration for the UART
   struct UartRunTimeConfig {
      static constexpr void init() {}

      static constexpr asx::uart::width get_width() {
         return asx::uart::width::_8;
      }

      static asx::uart::parity get_parity();
      static asx::uart::stop get_stop();
      static uint32_t get_baud();

      static constexpr bool has(int options) {
         return (asx::uart::rs485 | asx::uart::onewire) & options;
      }
   };


   // -------------------------------------------------------------------------
   // API
   // -------------------------------------------------------------------------

   // Global function
   void init();
   void reset_config();
   const EepromConfig& get_config();

   // Dedicated storage update functions
   // These match the modbus capabilities and groups
   void set_comm(uint8_t device_id, baud_t baud, asx::uart::parity parity, asx::uart::stop stops);
} // End of config namespace
