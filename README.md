# cnc-pneumatic-hub

Part of my CNC project - this is a modbus electro-pneumatic valve remote hub controller.

Basically, the Masso G3 controller uses TTL output which are incapable of driver electro-pneumatic solenoids.
The CNC patch panel converts the Masso TTL output into relay coils command.
This board then drives the coils.

Additionally, a state machine has been added to add some filtering to avoid aggressive driving ON-OFF.

This project is a good example of the Modbus ASX framework for slave device.

The device does not support recovery mode since it is not required.

It has a fixed Modbus device address 49 and 115200 8E1 serial.

It does support the Modbus MEI standard for discovery.

