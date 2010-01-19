# 
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	$(CP) $(LINUX_KERNEL) $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.bin
endef

$(eval $(call BuildImage))
