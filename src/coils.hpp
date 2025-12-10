#pragma once

#include <cstdint>

namespace coil {
   /// @brief Number of coils managed
   constexpr auto COUNT = 5;

   /// @brief Get the status of all coils as a bitmask
   uint8_t get_all();

   /// @brief Get the coil at the given index
   bool get(uint8_t index);

   /// @brief Set the coil at the given index
   /// @param index Index 0-4 of the coil to set
   /// @param on If true, close the coil
   /// @return true to indicate the command was immediatly applied.
   bool set(uint8_t index, bool on=true);

   /// @brief  Initialize the coil GPIOs
   void init();
}
