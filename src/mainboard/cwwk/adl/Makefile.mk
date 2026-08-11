## SPDX-License-Identifier: GPL-2.0-or-later

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c

romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_SOC_INTEL_COMMON_BLOCK_HDA_VERB) += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR_ENABLED) += variants/$(VARIANT_DIR)/cfr.c
