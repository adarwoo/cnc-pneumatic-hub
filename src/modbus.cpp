#include "datagram.hpp"
#include "hub.hpp"

using namespace asx;

namespace hub
{
   void on_read_coils(uint8_t addr, uint8_t qty)
   {
      if (qty > (coil::COUNT - addr))
      {
         Datagram::reply_error(modbus::error_t::illegal_data_value);
      }
      else
      {
         Datagram::pack((coil::get_all() >> addr) & (1 << qty));
      }
   }

   void on_write_single(uint8_t index, uint16_t operation)
   {
      switch (operation)
      {
      case 0x0000:
         coil::set(index, false);
         break;
      case 0xFF00:
         coil::set(index);
         break;
      case 0x5500:
         coil::set(index, !coil::get(index));
         break;
      default:
         break;
      }
   }

   void on_write_coils(uint8_t from, uint8_t qty, uint8_t ignore, uint8_t values)
   {
      // Values should only have 1 bit set
      if ( values && !(values & (values - 1)) ) {
         for (uint8_t i = 0; i < qty; ++i)
         {
            if ( not coil::set(from + i, values & 1) ) {
               Datagram::reply_error(modbus::error_t::negative_acknowledge);
               return;
            }

            values >>= 1;
         }
      } else if ( values == 0 ) {
         coil::release();
      } else {
         Datagram::reply_error(modbus::error_t::illegal_data_value);
         return;
      }

      // For the response, we need to shorten the frame
      // SlaveAddr[1]+FunctionCode[1]+Start[2]+Qty[2]
      Datagram::set_size(6);
   }

   void on_read_pressure()
   {
      Datagram::pack(coil::read_pressure());
   }

   void on_custom(uint8_t coils)
   {
      for (uint8_t i=0; i<coil::COUNT; ++i) {
         if ( not coil::set(i, (coils >> i) & 1) ) {
            Datagram::reply_error(modbus::error_t::negative_acknowledge);
            return;
         }
      }
      
      Datagram::pack(coil::read_pressure());
   }

}