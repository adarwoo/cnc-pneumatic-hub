#!/usr/bin/env python3
from modbus_rtu_slave_rc import *  # Import everything from modbus_generator

DEVICE_ADDRESS = 49

Modbus({
    "buffer_size": 128,
    "namespace": "hub",

    "callbacks": {
        "on_get_pressure"          : [],
        "on_read_coils"            : [(u8, "addr"), (u8, "qty")],
        "on_write_coils"           : [(u8, "addr"), (u8, "qty"), (u8), (u8, "data")],
        "on_write_single_coil"     : [(u8, "index"), (u16, "value")],
        "on_custom"                : [(u8, "coils")],
        "on_read_holding_registers": [(u8, "addr"), (u8, "qty")],
    },

    f"device@{DEVICE_ADDRESS}": [
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
    ],
})
