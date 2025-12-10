#pragma once
/**
 * This file was generated to create a state machine for processing
 * uart data used for a modbus RTU. It should be included by
 * the modbus_rtu_slave.cpp file only which will create a full rtu slave device.
 */
#include <cstdint>
#include <ulog.h>
#include <asx/modbus_rtu_slave.hpp>

namespace net {
    // All callbacks registered
    void on_get_pressure();
    void on_read_coils(uint8_t addr, uint8_t qty);
    void on_write_coils(uint8_t addr, uint8_t qty, uint8_t, uint8_t data);
    void on_write_single_coil(uint8_t index, uint16_t value);
    void on_custom(uint8_t coils);
    void on_report_slave_id();
    void on_read_device_identification(uint8_t device_id, uint8_t object_id);
    void on_diagnostics();

    // All states to consider
    enum class state_t : uint8_t {
        IGNORE = 0,
        ERROR = 1,
        DEVICE_ADDRESS,
        DEVICE_49,
        DEVICE_49_READ_DISCRETE_INPUTS,
        DEVICE_49_READ_DISCRETE_INPUTS_from,
        DEVICE_49_READ_DISCRETE_INPUTS_from__ON_GET_PRESSURE__CRC,
        RDY_TO_CALL__ON_GET_PRESSURE,
        DEVICE_49_READ_COILS,
        DEVICE_49_READ_COILS_from,
        DEVICE_49_READ_COILS_from__ON_READ_COILS__CRC,
        RDY_TO_CALL__ON_READ_COILS,
        DEVICE_49_WRITE_SINGLE_COIL,
        DEVICE_49_WRITE_SINGLE_COIL_from,
        DEVICE_49_WRITE_SINGLE_COIL_from__ON_WRITE_SINGLE_COIL__CRC,
        RDY_TO_CALL__ON_WRITE_SINGLE_COIL,
        DEVICE_49_WRITE_MULTIPLE_COILS,
        DEVICE_49_WRITE_MULTIPLE_COILS_start,
        DEVICE_49_WRITE_MULTIPLE_COILS_start_qty,
        DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount,
        DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount__ON_WRITE_COILS__CRC,
        RDY_TO_CALL__ON_WRITE_COILS,
        DEVICE_49_CUSTOM,
        DEVICE_49_CUSTOM__ON_CUSTOM__CRC,
        RDY_TO_CALL__ON_CUSTOM,
        DEVICE_49__ON_REPORT_SLAVE_ID__CRC,
        RDY_TO_CALL__ON_REPORT_SLAVE_ID,
        DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT,
        DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION,
        DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE,
        DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE__ON_READ_DEVICE_IDENTIFICATION__CRC,
        RDY_TO_CALL__ON_READ_DEVICE_IDENTIFICATION,
        DEVICE_49_DIAGNOSTICS,
        DEVICE_49_DIAGNOSTICS_SUBFUNCTION,
        DEVICE_49_DIAGNOSTICS_SUBFUNCTION__ON_DIAGNOSTICS__CRC,
        RDY_TO_CALL__ON_DIAGNOSTICS
    };

    // Code 43 / 14 object category
    enum class object_code_t : uint8_t {
        BASIC_DEVICE_IDENTIFICATION = 0x01,
        REGULAR_DEVICE_IDENTIFICATION = 0x02,
        EXTENDED_DEVICE_IDENTIFICATION = 0x03,
        SPECIFIC_DEVICE_IDENTIFICATION = 0x04
    };


    class Datagram {
        using error_t = asx::modbus::error_t;

        ///< Device ID
        static constexpr auto device_address = uint8_t{49};
        ///< Adjusted buffer to only receive the largest amount of data possible
        inline static uint8_t buffer[80];
        ///< Number of characters in the buffer
        inline static uint8_t cnt;
        ///< Number of characters to send
        inline static uint8_t frame_size;
        ///< Error code
        inline static error_t error;
        ///< State
        inline static state_t state;
        ///< CRC for the datagram
        inline static asx::modbus::Crc crc{};

        static inline auto ntoh(const uint8_t offset) -> uint16_t {
            return (static_cast<uint16_t>(buffer[offset]) << 8) | static_cast<uint16_t>(buffer[offset + 1]);
        }

        static inline auto ntohl(const uint8_t offset) -> uint32_t {
            return
                (static_cast<uint32_t>(buffer[offset]) << 24) |
                (static_cast<uint32_t>(buffer[offset+1]) << 16) |
                (static_cast<uint32_t>(buffer[offset+2]) << 8) |
                static_cast<uint16_t>(buffer[offset+3]);
        }

    public:
        // Status of the datagram
        enum class status_t : uint8_t {
            GOOD_FRAME = 0,
            NOT_FOR_ME = 1,
            BAD_CRC = 2
        };

        
        static void reset() noexcept {
            cnt=0;
            crc.reset();
            error = error_t::ok;
            state = state_t::DEVICE_ADDRESS;
        }

        static status_t get_status() noexcept {
            if (state == state_t::IGNORE) {
                return status_t::NOT_FOR_ME;
            }

            return crc.check() ? status_t::GOOD_FRAME : status_t::BAD_CRC;
        }

        static void process_char(const uint8_t c) noexcept {
            ULOG_DEBUG0("Processing char: 0x{:2x} at position {}", c, cnt);

            if (state == state_t::IGNORE) {
                return;
            }

            crc(c);

            if (state != state_t::ERROR) {
                // Store the frame
                buffer[cnt++] = c; // Store the data
            }

            switch(state) {
            case state_t::ERROR:
                break;
            case state_t::DEVICE_ADDRESS:
                if ( c == 49 ) {
                    state = state_t::DEVICE_49;
                } else {
                    error = error_t::ignore_frame;
                    state = state_t::IGNORE;
                }
                break;
            case state_t::DEVICE_49:
                if ( c == 2 ) {
                    state = state_t::DEVICE_49_READ_DISCRETE_INPUTS;
                } else if ( c == 1 ) {
                    state = state_t::DEVICE_49_READ_COILS;
                } else if ( c == 5 ) {
                    state = state_t::DEVICE_49_WRITE_SINGLE_COIL;
                } else if ( c == 15 ) {
                    state = state_t::DEVICE_49_WRITE_MULTIPLE_COILS;
                } else if ( c == 101 ) {
                    state = state_t::DEVICE_49_CUSTOM;
                } else if ( c == 17 ) {
                    state = state_t::DEVICE_49__ON_REPORT_SLAVE_ID__CRC;
                } else if ( c == 43 ) {
                    state = state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT;
                } else if ( c == 8 ) {
                    state = state_t::DEVICE_49_DIAGNOSTICS;
                } else {
                    error = error_t::illegal_function_code;
                    state = state_t::ERROR;
                }
                break;
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS:
                if ( cnt == 4 ) {
                    auto c = ntoh(cnt-2);

                    if ( c == 0 ) {
                        state = state_t::DEVICE_49_READ_DISCRETE_INPUTS_from;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS_from:
                if ( cnt == 6 ) {
                    auto c = ntoh(cnt-2);

                    if ( c == 1 ) {
                        state = state_t::DEVICE_49_READ_DISCRETE_INPUTS_from__ON_GET_PRESSURE__CRC;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS_from__ON_GET_PRESSURE__CRC:
                if ( cnt == 8 ) {
                    state = state_t::RDY_TO_CALL__ON_GET_PRESSURE;
                }
                break;
            case state_t::DEVICE_49_READ_COILS:
                if ( cnt == 4 ) {
                    auto c = ntoh(cnt-2);

                    if ( c <= 11 ) {
                        state = state_t::DEVICE_49_READ_COILS_from;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_READ_COILS_from:
                if ( cnt == 6 ) {
                    auto c = ntoh(cnt-2);

                    if ( c >= 1 and c <= 12 ) {
                        state = state_t::DEVICE_49_READ_COILS_from__ON_READ_COILS__CRC;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_READ_COILS_from__ON_READ_COILS__CRC:
                if ( cnt == 8 ) {
                    state = state_t::RDY_TO_CALL__ON_READ_COILS;
                }
                break;
            case state_t::DEVICE_49_WRITE_SINGLE_COIL:
                if ( cnt == 4 ) {
                    auto c = ntoh(cnt-2);

                    if ( c <= 11 ) {
                        state = state_t::DEVICE_49_WRITE_SINGLE_COIL_from;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_WRITE_SINGLE_COIL_from:
                if ( cnt == 6 ) {
                    auto c = ntoh(cnt-2);

                    if ( c == 0xff00 || c == 0x0 ) {
                        state = state_t::DEVICE_49_WRITE_SINGLE_COIL_from__ON_WRITE_SINGLE_COIL__CRC;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_WRITE_SINGLE_COIL_from__ON_WRITE_SINGLE_COIL__CRC:
                if ( cnt == 8 ) {
                    state = state_t::RDY_TO_CALL__ON_WRITE_SINGLE_COIL;
                }
                break;
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS:
                if ( cnt == 4 ) {
                    auto c = ntoh(cnt-2);

                    if ( c <= 11 ) {
                        state = state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start:
                if ( cnt == 6 ) {
                    auto c = ntoh(cnt-2);

                    if ( c >= 1 and c <= 8 ) {
                        state = state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty;
                    } else {
                        error = error_t::illegal_data_value;
                        state = state_t::ERROR;
                    }
                }
                break;
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty:
                if ( c == 1 ) {
                    state = state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount;
                } else {
                    error = error_t::illegal_data_value;
                    state = state_t::ERROR;
                }
                break;
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount:
                state = state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount__ON_WRITE_COILS__CRC;
                break;
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount__ON_WRITE_COILS__CRC:
                if ( cnt == 10 ) {
                    state = state_t::RDY_TO_CALL__ON_WRITE_COILS;
                }
                break;
            case state_t::DEVICE_49_CUSTOM:
                state = state_t::DEVICE_49_CUSTOM__ON_CUSTOM__CRC;
                break;
            case state_t::DEVICE_49_CUSTOM__ON_CUSTOM__CRC:
                if ( cnt == 5 ) {
                    state = state_t::RDY_TO_CALL__ON_CUSTOM;
                }
                break;
            case state_t::DEVICE_49__ON_REPORT_SLAVE_ID__CRC:
                if ( cnt == 4 ) {
                    state = state_t::RDY_TO_CALL__ON_REPORT_SLAVE_ID;
                }
                break;
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT:
                if ( c == 14 ) {
                    state = state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION;
                } else {
                    error = error_t::illegal_data_value;
                    state = state_t::ERROR;
                }
                break;
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION:
                if ( c >= 1 and c <= 3 ) {
                    state = state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE;
                } else {
                    error = error_t::illegal_data_value;
                    state = state_t::ERROR;
                }
                break;
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE:
                state = state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE__ON_READ_DEVICE_IDENTIFICATION__CRC;
                break;
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE__ON_READ_DEVICE_IDENTIFICATION__CRC:
                if ( cnt == 7 ) {
                    state = state_t::RDY_TO_CALL__ON_READ_DEVICE_IDENTIFICATION;
                }
                break;
            case state_t::DEVICE_49_DIAGNOSTICS:
                if ( cnt == 4 ) {
                    state = state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION;
                }
                break;
            case state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION:
                if ( cnt == 6 ) {
                    state = state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION__ON_DIAGNOSTICS__CRC;
                }
                break;
            case state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION__ON_DIAGNOSTICS__CRC:
                if ( cnt == 8 ) {
                    state = state_t::RDY_TO_CALL__ON_DIAGNOSTICS;
                }
                break;
            case state_t::RDY_TO_CALL__ON_GET_PRESSURE:
            case state_t::RDY_TO_CALL__ON_READ_COILS:
            case state_t::RDY_TO_CALL__ON_WRITE_SINGLE_COIL:
            case state_t::RDY_TO_CALL__ON_WRITE_COILS:
            case state_t::RDY_TO_CALL__ON_CUSTOM:
            case state_t::RDY_TO_CALL__ON_REPORT_SLAVE_ID:
            case state_t::RDY_TO_CALL__ON_READ_DEVICE_IDENTIFICATION:
            case state_t::RDY_TO_CALL__ON_DIAGNOSTICS:
            default:
                error = error_t::illegal_data_value;
                state = state_t::ERROR;
                break;
            }
        }

        static void reply_error( error_t err ) noexcept {
            buffer[1] |= 0x80;
            buffer[2] = (uint8_t)err;
            cnt = 3;
        }

        template<typename T>
        static void pack(const T& value) noexcept {
            if constexpr ( sizeof(T) == 1 ) {
                buffer[cnt++] = value;
            } else if constexpr ( sizeof(T) == 2 ) {
                buffer[cnt++] = value >> 8;
                buffer[cnt++] = value & 0xff;
            } else if constexpr ( sizeof(T) == 4 ) {
                buffer[cnt++] = value >> 24;
                buffer[cnt++] = value >> 16 & 0xff;
                buffer[cnt++] = value >> 8 & 0xff;
                buffer[cnt++] = value & 0xff;
            }
        }

        static void pack(const char *v) noexcept {
            auto length = strlen(v);
            memcpy(&buffer[cnt], v, length);
            cnt += length;
        }

        static inline void set_size(uint8_t size) {
            cnt = size;
        }

        /** Called when a T3.5 has been detected, in a good sequence */
        static void ready_reply() noexcept {
            frame_size = cnt; // Store the frame size
            cnt = 2; // Points to the function code
            

            switch(state) {
            case state_t::IGNORE:
                break;
            case state_t::DEVICE_ADDRESS:
            case state_t::DEVICE_49:
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS:
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS_from:
            case state_t::DEVICE_49_READ_DISCRETE_INPUTS_from__ON_GET_PRESSURE__CRC:
            case state_t::DEVICE_49_READ_COILS:
            case state_t::DEVICE_49_READ_COILS_from:
            case state_t::DEVICE_49_READ_COILS_from__ON_READ_COILS__CRC:
            case state_t::DEVICE_49_WRITE_SINGLE_COIL:
            case state_t::DEVICE_49_WRITE_SINGLE_COIL_from:
            case state_t::DEVICE_49_WRITE_SINGLE_COIL_from__ON_WRITE_SINGLE_COIL__CRC:
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS:
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start:
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty:
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount:
            case state_t::DEVICE_49_WRITE_MULTIPLE_COILS_start_qty_bytecount__ON_WRITE_COILS__CRC:
            case state_t::DEVICE_49_CUSTOM:
            case state_t::DEVICE_49_CUSTOM__ON_CUSTOM__CRC:
            case state_t::DEVICE_49__ON_REPORT_SLAVE_ID__CRC:
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT:
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION:
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE:
            case state_t::DEVICE_49_ENCAPSULATED_INTERFACE_TRANSPORT_READ_DEVICE_IDENTIFICATION_READ_DEVICE_ID_CODE__ON_READ_DEVICE_IDENTIFICATION__CRC:
            case state_t::DEVICE_49_DIAGNOSTICS:
            case state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION:
            case state_t::DEVICE_49_DIAGNOSTICS_SUBFUNCTION__ON_DIAGNOSTICS__CRC:
                error = error_t::illegal_data_value;
            case state_t::ERROR:
                buffer[1] |= 0x80; // Mark the error
                buffer[2] = (uint8_t)error; // Add the error code
                cnt = 3;
                break;
            case state_t::RDY_TO_CALL__ON_GET_PRESSURE:
                on_get_pressure();
                break;
            case state_t::RDY_TO_CALL__ON_READ_COILS:
                on_read_coils(buffer[3], buffer[5]);
                break;
            case state_t::RDY_TO_CALL__ON_WRITE_SINGLE_COIL:
                on_write_single_coil(buffer[3], ntoh(4));
                break;
            case state_t::RDY_TO_CALL__ON_WRITE_COILS:
                on_write_coils(buffer[3], buffer[5], buffer[6], buffer[7]);
                break;
            case state_t::RDY_TO_CALL__ON_CUSTOM:
                on_custom(buffer[2]);
                break;
            case state_t::RDY_TO_CALL__ON_REPORT_SLAVE_ID:
                on_report_slave_id();
                break;
            case state_t::RDY_TO_CALL__ON_READ_DEVICE_IDENTIFICATION:
                on_read_device_identification(buffer[3], buffer[4]);
                break;
            case state_t::RDY_TO_CALL__ON_DIAGNOSTICS:
                on_diagnostics();
                break;
            default:
                break;
            }

            // If the cnt is 2 - nothing was changed in the buffer - return it as is
            if ( cnt == 2 ) {
                // Framesize includes the previous CRC which still holds valid
                cnt = frame_size;
            } else {
                // Add the CRC
                crc.reset();
                auto _crc = crc.update(std::string_view{(char *)buffer, cnt});
                buffer[cnt++] = _crc & 0xff;
                buffer[cnt++] = _crc >> 8;
            }
        }

        static std::string_view get_buffer() noexcept {
            // Return the buffer ready to send
            return std::string_view{(char *)buffer, cnt};
        }
    }; // struct Processor

    /** Answer command 17 - Report slave id */
    inline void on_report_slave_id() {
        Datagram::set_size(2); // Reset the count to 2 (ID + code)
        Datagram::pack<uint8_t>(12); // Byte count
        Datagram::pack<uint8_t>(255); // slave ID
        Datagram::pack<uint8_t>(0xFF); // Status OK
        Datagram::pack("PN-HUB_1.0"); // Function code
    }

    /** Answer command 43/14 */
     inline void on_read_device_identification(uint8_t device_id, uint8_t object_id) {
        Datagram::set_size(4); // Reset the count to 4 (addr/func/mei_type/DevId)
        Datagram::pack<uint8_t>(2); // Conformity level
        Datagram::pack<uint8_t>(0); // No more to follow

    Datagram::pack<uint8_t>(0); // Next object ID

        if (device_id == 1) { // Device ID 1 has a fixed number of objects
           Datagram::pack<uint8_t>(0x03); // 3 objects
        } else {
           Datagram::pack<uint8_t>(5); // 5 objects
        }

        if (device_id == 1) {
                    Datagram::pack<uint8_t>(0x00); // Object code
            Datagram::pack<uint8_t>(4); // Length of the object
            Datagram::pack("ARex");
                    Datagram::pack<uint8_t>(0x01); // Object code
            Datagram::pack<uint8_t>(6); // Length of the object
            Datagram::pack("PN-HUB");
                    Datagram::pack<uint8_t>(0x02); // Object code
            Datagram::pack<uint8_t>(3); // Length of the object
            Datagram::pack("1.0");
        } else {
                    Datagram::pack<uint8_t>(0x03); // Object code
            Datagram::pack<uint8_t>(18); // Length of the object
            Datagram::pack("github.com/adarwoo");
                    Datagram::pack<uint8_t>(0x05); // Object code
            Datagram::pack<uint8_t>(3); // Length of the object
            Datagram::pack("1.0");
        }
    }

    inline void on_diagnostics() {}
} // namespace net