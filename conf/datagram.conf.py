#!/usr/bin/env python3
from modbus_rtu_rc import *  # Import everything from modbus_generator

Modbus({
    "namespace": "net",
    "slave": True,
    "buffer_size": 80, # Max reg read = 0x20 (32 x 2) + Frame (ID+CODE+NUM+CRC)

    "identification": {
        VENDOR_NAME:            "ARex",
        PRODUCT_CODE:           "PN-HUB",
        MAJOR_MINOR_REVISION:   "1.0",
        VENDOR_URL:             "github.com/adarwoo",
        MODEL_NAME:             "1.0",
    },

    "callbacks": {
        "on_get_pressure"          : [],
        "on_read_coils"            : [(u8, "addr"), (u8, "qty")],
        "on_write_coils"           : [(u8, "addr"), (u8, "qty"), (u8), (u8, "data")],
        "on_write_single_coil"     : [(u8, "index"), (u16, "value")],
        "on_custom"                : [(u8, "coils")],
    },

    f"device@49": [
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

        (CUSTOM,                u8(alias="outputs"), "on_custom"),
    ],
})
