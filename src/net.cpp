// Optimize for size for all types of builds to avoid serious bloating
#pragma GCC optimize("Os")

#include <ulog.h>
#include <asx/reactor.hpp>
#include "state.hpp"
#include "net.hpp"


namespace net {
   using namespace asx::modbus;

   // Create an alias for the static method
   using dg = Datagram;

   //
   // Constants
   //

   // ID to use in recovery
   auto constexpr RECOVERY_DEVICE_ID = uint8_t{248};

   constexpr auto DEVICE_ID = uint16_t{0x4901};
   constexpr auto HW_VERSION = uint16_t{0x0001};
   constexpr auto FW_VERSION = uint16_t{0x0001};

    // -------------------------------------------------------------------------
   // Write holding
   // -------------------------------------------------------------------------
   void on_write_comms_settings(uint8_t addr, uint8_t baud, uint8_t parity, uint8_t stopbits) {
      ULOG_INFO("Writing Communication Settings");

      // Note : All ranges have already been enforced
      if ( state::is_in_recovery_mode() ) {
         config::set_comm(
            addr,
            static_cast<baud_t>(baud),
            static_cast<asx::uart::parity>(parity),
            static_cast<asx::uart::stop>(stopbits)
         );

         // Drop out of recovery mode. This will reset the UART
         state::set_recovery_mode(false);

         // For the response, we need to shorten the frame
         // SlaveAddr[1]+FunctionCode[1]+Start[2]+Qty[2]
         dg::set_size(6);
      } else {
         ULOG_WARN("Cannot change communication settings outside of recovery mode");
         dg::reply_error(error_t::negative_acknowledge);
      }
   }

   void on_read_holdings(uint8_t index, uint8_t qty) {
      ULOG_TRACE("Read Holdings Index: {} - Quantity: {}", index, qty);

      // Number of bytes returned
      dg::pack<uint8_t>(qty*2);
      auto &cfg = config::get_config();

      while ( qty-- ) {
         switch(index++) {
         // Communication settings
         case 0x00: dg::pack<uint16_t>( cfg.address ); break;
         case 0x01: dg::pack<uint16_t>( static_cast<uint16_t>(cfg.baud) ); break;
         case 0x02: dg::pack<uint16_t>( static_cast<uint16_t>(cfg.parity) ); break;
         case 0x03: dg::pack<uint16_t>( static_cast<uint16_t>(cfg.stopbits) ); break;

         default:
            dg::pack(uint16_t{0});
         }
      }
   }
}