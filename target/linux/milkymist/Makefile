#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

#Add target specific (nommu) flags
TARGET_CFLAGS+= -DNO_FORK

ARCH:=lm32
BOARD:=milkymist
BOARDNAME:=Milkymist (Lattice Mico32)
FEATURES:=nommu ramdisk

LINUX_VERSION:=2.6.23

include $(INCLUDE_DIR)/target.mk

CFLAGS:=-O2 -pipe  -funit-at-a-time -mmultiply-enabled -mdivide-enabled -mbarrel-shift-enabled -msign-extend-enabled

DEFAULT_PACKAGES:= \
	base-files \
	busybox \
	dnsmasq \
	dropbear \
	opkg \

KERNEL_MAKEOPTS += LDFLAGS_BUILD_ID=""

define Target/Description
	Build firmware images for the Milkymist open hardware system-on-chip platform.
endef

$(eval $(call BuildTarget))
