/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/gpio.h>
#include <gpio.h>

static const struct pad_config gpio_table[] = {
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
