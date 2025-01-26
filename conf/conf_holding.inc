const unsigned char holding_0[] =
   // 0(1) Manufacturer ID
   "\x49\x37"
   // 1(2) Device model
   "\x0\x01"
   // 2/3(3/4) Serial number
   "\x0\x0\x0\x1"
   // 4(5) Firmware version major
   "\x0\x1"
   // 5(6) Firmware version minor
   "\x0\x1"
   // 6(7) Hardware revision
   "\x0\x1"
   // 7(8) Device address - 49
   "\x0\x31"
   // 8-16(9-17/8) Device name - 2 bytes alignes
   //.x.x.x.x.x.x.x.x
   "CNCPneumatic HUB"
   // 17-22 (18-23/8) Manufacturer name - 2 bytes alignes
   //.x.x.x.x.x.x.x.x
    "ARex 2025       "
   // Device type code
   "\x0\x31"
   // Manufacturing date
   "20250126"
;
