#include "pressure_mon.hpp"
#include "datagram.hpp"
#include "coils.hpp"

using namespace asx;


namespace net {
   void on_read_coils(uint8_t addr, uint8_t qty) {
      if (qty > (coil::COUNT - addr)) {
         Datagram::reply_error(modbus::error_t::illegal_data_value);
      } else{
         uint8_t value = 0;

         // Build the value by reading the LEDs
         for (uint8_t i = 0; i < qty; ++i) {
            value |= (coil::get(addr + i) ? 1 : 0) << i;
         }

         Datagram::set_size(2);          // Include the byte count itself in the size
         Datagram::pack<uint8_t>(1);     // Byte count
         Datagram::pack<uint8_t>(value); // Single value
      }
   }

   void on_write_single_coil(uint8_t index, uint16_t operation) {
      coil::set(index, operation == 0xff00);
   }

   /// @brief Handler for the write coils modbus function
   /// @param from 0 to 4 (modbus 1 to 5)
   /// @param qty 1 to 4
   /// @param ignore The number of bytes
   /// @param values Binary form for the coils with the MSB matching the first coil
   void on_write_coils(uint8_t from, uint8_t qty, uint8_t ignore, uint8_t values) {
      // Values should only have 1 bit set as we only allow 1 coil on at once
      for (uint8_t i = 0; i < qty; ++i) {
         if ( not coil::set(from + i, values & 1) ) {
            Datagram::reply_error(modbus::error_t::negative_acknowledge);
            return;
         }

         values >>= 1;
      }

      // For the response, we need to shorten the frame
      // SlaveAddr[1]+FunctionCode[1]+Start[2]+Qty[2]
      Datagram::set_size(6);
   }

   void on_get_pressure(uint8_t addr, uint8_t qty) {
      if (addr + qty > 2) {
         Datagram::reply_error(modbus::error_t::illegal_data_value);
         return;
      }

      Datagram::pack<uint8_t>(1); // Number of bytes
      Datagram::pack<uint8_t>(pressure_mon::value());
   }

   void on_custom(uint8_t coils) {
      for (uint8_t i=0; i<coil::COUNT; ++i) {
         if ( not coil::set(i, (coils >> i) & 1) ) {
            Datagram::reply_error(modbus::error_t::negative_acknowledge);
            return;
         }
      }

      Datagram::pack(pressure_mon::value());
   }
} // namespace net
