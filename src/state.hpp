#pragma once

namespace state {
   namespace detail {
       inline auto recovery = false; 
   }

   // Is the device in recovery mode
   inline bool is_in_recovery_mode() {
      return detail::recovery;
   }

   inline void set_recovery_mode(bool on) {
      detail::recovery = on;
   }
} // End of namespace state