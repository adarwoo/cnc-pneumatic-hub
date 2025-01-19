#pragma once

#include <cstdint>

namespace coil {
    constexpr auto COUNT = 5;

    uint8_t get_all();
    bool get(uint8_t index);
    bool set(uint8_t index, bool on=true);
    void release();
    bool read_pressure();
}
