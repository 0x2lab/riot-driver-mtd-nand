/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards
 * @{
 *
 * @file
 * @brief       Generic board functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "kernel_defines.h"

#define LED_INIT(x) do {                                    \
                        gpio_init(LED##x##_PIN, GPIO_OUT);  \
                        LED##x##_OFF;                       \
                    } while (0)

__attribute__((__weak__))
void board_init(void)
{
    if (!IS_USED(MODULE_PERIPH_GPIO)) {
        return;
    }

#ifdef LED0_PIN
    LED_INIT(0);
#endif
#ifdef LED1_PIN
    LED_INIT(1);
#endif
#ifdef LED2_PIN
    LED_INIT(2);
#endif
#ifdef LED3_PIN
    LED_INIT(3);
#endif
#ifdef LED4_PIN
    LED_INIT(4);
#endif
#ifdef LED5_PIN
    LED_INIT(5);
#endif
#ifdef LED6_PIN
    LED_INIT(6);
#endif
#ifdef LED7_PIN
    LED_INIT(7);
#endif
}
