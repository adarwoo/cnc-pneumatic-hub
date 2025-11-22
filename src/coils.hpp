#pragma once

#include <cstdint>

namespace coil {
    constexpr auto COUNT = 5;

    uint8_t get_all();
    bool get(uint8_t index);

    /// @brief Set the coil at the given index
    /// @param index Index 0-4 of the coil to set
    /// @param on If true, close the coil
    /// @return true to indicate the command was immediatly applied.
    bool set(uint8_t index, bool on=true);

    void release();
    bool read_pressure();
}
