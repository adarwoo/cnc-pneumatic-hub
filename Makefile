TOP:=.
ARCH:=attiny1627
BIN:=cnc_pneumatic_hub
INCLUDE_DIRS:=conf src

ASX_USE:=modbus_rtu eeprom

# Project own files
SRCS = \
   src/main.cpp \
   src/modbus.cpp \
   src/net.cpp \
	src/board.c \
   src/config.cpp \
   src/coils.cpp \

# Inlude the actual build rules
include asx/make/rules.mak

# Add dependency to generate the datagram from the config
src/main.cpp : conf/datagram.hpp
