#!/usr/bin/env python3
from modbus_rtu_rc import *  # Import everything from modbus_generator

Modbus({
    "namespace": "net",
    "slave": True,
    "buffer_size": 80, # Max reg read = 0x20 (32 x 2) + Frame (ID+CODE+NUM+CRC)
    "slave_id": 49, # For code 17

    "identification": {
        VENDOR_NAME:            "ARex",
        PRODUCT_CODE:           "PN-HUB",
        MAJOR_MINOR_REVISION:   "1.0",
        VENDOR_URL:             "github.com/adarwoo",
        MODEL_NAME:             "1.0",
        PRIVATE_OBJECTS_0:      "ReCoVeRy;1;0x0000", # Device supports recovery mode
    },

    "callbacks": {
        "on_get_pressure"          : [],
        "on_read_coils"            : [(u8, "addr"), (u8, "qty")],
        "on_write_coils"           : [(u8, "addr"), (u8, "qty"), (u8), (u8, "data")],
        "on_write_single_coil"     : [(u8, "index"), (u16, "value")],
        "on_custom"                : [(u8, "coils")],
        "on_read_holding_registers": [(u8, "addr"), (u8, "qty")],
        "on_read_holdings"         : [(u8, "addr"), (u8, "count")],
        "on_write_comms_settings"   : [
            (u8, "addr"), (u8, "baud"), (u8, "parity"), (u8, "stopbits")
        ],
    },

    f"device": [
        # Read the push button and switches state
        (READ_DISCRETE_INPUTS,  u16(0, alias="from"),
                                u16(1, alias="qty"), # Byte count
                                "on_get_pressure"),

        (READ_COILS,            u16(0, 11, alias="from"),
                                u16(1, 12, alias="qty"),
                                "on_read_coils"),

        (WRITE_SINGLE_COIL,     u16(0, 11, alias="from"),
                                u16([0xff00,0], alias="qty"),
                                "on_write_single_coil"),

        (WRITE_MULTIPLE_COILS,  u16(0, 11, alias="start"),
                                u16(1, 8, alias="qty"),
                                u8(1, alias="bytecount"),
                                u8(alias="data"),
                                "on_write_coils"),

        (READ_HOLDING_REGISTERS,u16(0, 32, alias="from"),
                                u16(1, 32, alias="qty"),
                                "on_read_holding_registers"),

        (CUSTOM,                u8(alias="outputs"), "on_custom"),

        # Generic read of holding registers
        (READ_HOLDING_REGISTERS, u16(0, 0x3), u16(1,0x4),
                                "on_read_holdings"),

        # Communication settings
        (WRITE_MULTIPLE_REGISTERS,
            u16(0), u16(4), u8(8),
                u16(1,247), # Device address
                u16(0,9),   # Baud rate
                u16(0,2),   # Parity
                u16(1,2),   # Stop bits
            "on_write_comms_settings"
        ),
    ],
})
