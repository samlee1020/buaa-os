# ENDIAN is either EL (little endian) or EB (big endian)
ENDIAN         := EL

QEMU           := qemu-system-loongarch32
CROSS_COMPILE  ?= loongarch32r-linux-gnusf-
CC             := $(CROSS_COMPILE)gcc
AS             := $(CROSS_COMPILE)as -mabi=ilp32
CFLAGS         += --std=gnu99 -G 0 -nostdlib -fno-pic -ffreestanding -fno-stack-protector -fno-builtin -Wall
LD             := $(CROSS_COMPILE)ld
LDFLAGS        += -G 0 -static -n -nostdlib --fatal-warnings

HOST_CC        := cc
HOST_CFLAGS    += --std=gnu99 -O2 -Wall
HOST_ENDIAN    := $(shell lscpu | grep -iq 'little endian' && echo EL || echo EB)

ifneq ($(HOST_ENDIAN), $(ENDIAN))
# CONFIG_REVERSE_ENDIAN is checked in tools/fsformat.c (lab5)
HOST_CFLAGS    += -DCONFIG_REVERSE_ENDIAN
endif
