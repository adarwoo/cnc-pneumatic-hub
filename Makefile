TOP:=.
ARCH:=attiny3224
BIN:=cnc_pneumatic_hub
INCLUDE_DIRS:=conf src

ASX_USE:=modbus_rtu reactor timer ulog

# Project own files
SRCS = \
   src/main.cpp \
   src/modbus.cpp \
   src/coils.cpp \

# Inlude the actual build rules
include asx/make/rules.mak

# Add dependency to generate the datagram from the config
src/main.cpp : conf/datagram.hpp
