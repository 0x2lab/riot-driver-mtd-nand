/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nand_samsung Samsung NAND driver
 * @ingroup     drivers_storage
 * @brief       Driver for reading and writing Samsung NANDs.
 * @anchor      drivers_nand_samsung
 * @{
 *
 * @file
 * @brief       Public interface for the nand_samsung driver.
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 */

#ifndef NAND_SAMSUNG_H
#define NAND_SAMSUNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "nand.h"
#include "nand/samsung/timing.h"
#include "nand/samsung/cmd_timing.h"
#include "nand/samsung/cmd.h"

typedef struct {
    uint8_t  chip_no;
    uint8_t  cell_level;
    uint8_t  simul_prog_pages_no;
    bool     interleaved_prog;
    bool     cache_prog;
    uint32_t data_bytes_per_page;
    uint32_t data_bytes_per_block;
    uint32_t data_blocks_per_plane;
    uint8_t  plane_count;
    uint16_t spare_bytes_per_512_bytes;
    uint8_t  data_bus_width;
    uint16_t serial_access_min_timings_ns;
} nand_samsung_chip_t;

typedef struct {
    nand_t                  nand;
    nand_samsung_chip_t     samsung_chip;
} nand_samsung_t;

int nand_samsung_init(nand_samsung_t* const nand_samsung, nand_params_t* const params);
void nand_samsung_read_chip(nand_samsung_t* const nand_samsung, nand_samsung_chip_t* const chip);

#ifdef __cplusplus
}
#endif

#endif /* NAND_SAMSUNG_H */
/** @} */
