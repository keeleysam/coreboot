/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

/*
 * Realtek ALC897. Pin configuration read back from the vendor firmware.
 * The board brings out a single combo jack on the front panel: headphone
 * out on node 0x14 and microphone in on node 0x18. Nodes 0x19 and 0x1b
 * report an internal microphone and speaker, neither of which exists on
 * this chassis, but they are reproduced as the vendor programs them.
 */
static const u32 alc897_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10ec0000),
	AZALIA_PIN_CFG(0, 0x11, 0x40000000),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x02214010),	/* HP Out, front, green */
	AZALIA_PIN_CFG(0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x02a19030),	/* Mic In, front, pink */
	AZALIA_PIN_CFG(0, 0x19, 0x90a7013f),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x90170120),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4026b505),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),
};

/* Alder Lake-N display audio, feeding the two HDMI outputs. */
static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC897",
		.vendor_id    = 0x10ec0897,
		.subsystem_id = 0x10ec0000,
		.address      = 0,
		.verbs        = alc897_verbs,
		.verb_count   = ARRAY_SIZE(alc897_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI)",
		.vendor_id    = 0x8086281c,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
