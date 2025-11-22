#include <asx/eeprom.hpp>
#include <ulog.h>
#include <state.hpp>
#include <config.hpp>
#include "datagram.hpp"


using namespace asx;

namespace config {
   static const auto default_config = EepromConfig {
      .address                   = 44,
      .baud                      = baud_t::_9600,
      .stopbits                  = uart::stop::_1,
      .parity                    = uart::parity::none,
   };

   constexpr auto baudrates = std::array<uint32_t, 10>{
      300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
   };

   constexpr bool is_valid_baudrate(uint8_t check) {
      return check < baudrates .size();
   }

   static auto eeprom_config = asx::eeprom::Storage<EepromConfig, 2>(
      default_config);

   /*
    * Getters for the runtime configuration
    */
   uart::parity UartRunTimeConfig::get_parity() {
      return state::is_in_recovery_mode()
         ? uart::parity::none
         : eeprom_config.parity;
   }

   uart::stop UartRunTimeConfig::get_stop() {
      return state::is_in_recovery_mode()
         ? uart::stop::_1
         : eeprom_config.stopbits;
   }

   uint32_t UartRunTimeConfig::get_baud() {
      return state::is_in_recovery_mode()
         ? 9600
         : baudrates[static_cast<uint8_t>(eeprom_config.baud)];
   }

   void init() {
      ULOG_MILE("Checking EEProm configuration");

      // Check if the eeprom was recovered
      if ( eeprom_config.is_formatted() ) {
         ULOG_WARN("EEprom content was invalid - reset to defaults");
      }
   }

   void reset_config() {
      ULOG_INFO("Resetting configuration to default");

      eeprom_config = default_config;
      eeprom_config.update();
   }

   const EepromConfig& get_config() {
      return eeprom_config;
   }

   /*
    * Modbus callbacks
    */
   void set_comm(uint8_t device_id, baud_t baud, uart::parity parity, uart::stop stopbits) {
      ULOG_INFO("Setting communication parameters ID:{} Baud:{} Parity:{} StopBits:{}",
         device_id,
         static_cast<uint8_t>(baud),
         static_cast<uint8_t>(parity),
         static_cast<uint8_t>(stopbits)
      );

      eeprom_config.address = device_id;
      eeprom_config.baud = baud;
      eeprom_config.parity = parity;
      eeprom_config.stopbits = stopbits;
      eeprom_config.update();
   }
} // End of relay namespace