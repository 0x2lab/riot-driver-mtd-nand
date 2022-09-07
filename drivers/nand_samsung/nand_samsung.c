/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nand_samsung
 * @{
 *
 * @file
 * @brief       low level driver for accessing Samsung NANDs
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"
#include "fmt.h"

#include "nand/samsung.h"
#include "nand_cmd.h"
#include "nand.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int nand_samsung_init(nand_samsung_t* const nand_samsung, nand_params_t* const params) {
    if(nand_samsung == NULL) {
        return NAND_INIT_ERROR;
    }

    nand_t* const nand = (nand_t*)nand_samsung;
    nand->init_done = false;

    if(params == NULL) {
        return NAND_INIT_ERROR;
    }

    int nand_init_status = nand_init(nand, params);
    if(nand_init_status != NAND_INIT_PARTIAL) {
        return nand_init_status;
    }

    nand->data_bus_width        = 8;
    nand->addr_bus_width        = 8;

    nand->nand_id_size          = nand_cmd_read_id(nand, 0, &NAND_SAMSUNG_CMD_READ_ID, nand->nand_id, NAND_MAX_ID_SIZE);
    if(nand->nand_id_size < NAND_MIN_ID_SIZE) {
        return NAND_INIT_ID_TOO_SHORT;
    }

    nand_samsung_read_chip(nand_samsung, &(nand_samsung->samsung_chip));

    nand->maker_code            = nand->nand_id[0];
    nand->device_code           = nand->nand_id[1];

    nand->data_bus_width        = nand_samsung->samsung_chip.data_bus_width;
    nand->addr_bus_width        = 8;

    nand->data_bytes_per_page   = nand_samsung->samsung_chip.data_bytes_per_page;
    nand->spare_bytes_per_page  = nand_samsung->samsung_chip.spare_bytes_per_512_bytes * nand->data_bytes_per_page / 512U;
    nand->pages_per_block       = nand_samsung->samsung_chip.data_bytes_per_block / nand->data_bytes_per_page;
    nand->blocks_per_lun        = nand_samsung->samsung_chip.data_blocks_per_plane * nand_samsung->samsung_chip.plane_count; /** TODO: multi-plane support */
    nand->lun_count             = 1; /** TODO: multi-LUN support */
    nand->bb_per_lun            = 0; /** TODO: BB support */

    nand->column_addr_cycles    = 2; /** TODO: variable-cycle support */
    nand->row_addr_cycles       = 3; /** TODO: variable-cycle support */

    nand->bits_per_cell         = 0; /** TODO: bits per cell support */
    nand->programs_per_page     = 0; /** TODO: programs_per_page support */

    nand->standard_type         = NAND_STD_SAMSUNG;

    nand->init_done             = true;

    return NAND_INIT_OK;
}

void nand_samsung_read_chip(nand_samsung_t* const nand_samsung, nand_samsung_chip_t* const chip) {
    nand_t* const nand = (nand_t*)nand_samsung;

    switch(nand->nand_id[2] & 0x03) {
        case 0x00: { chip->chip_no = 1; } break;
        case 0x01: { chip->chip_no = 2; } break;
        case 0x02: { chip->chip_no = 4; } break;
        case 0x03: { chip->chip_no = 8; } break;
    }

    switch(nand->nand_id[2] & 0x0C) {
        case 0x00: { chip->cell_level = 2; } break;
        case 0x04: { chip->cell_level = 4; } break;
        case 0x08: { chip->cell_level = 8; } break;
        case 0x0C: { chip->cell_level = 16; } break;
    }

    switch(nand->nand_id[2] & 0x30) {
        case 0x00: { chip->simul_prog_pages_no = 1; } break;
        case 0x10: { chip->simul_prog_pages_no = 2; } break;
        case 0x20: { chip->simul_prog_pages_no = 4; } break;
        case 0x30: { chip->simul_prog_pages_no = 8; } break;
    }

    switch(nand->nand_id[2] & 0x40) {
        case 0x00: { chip->interleaved_prog = false; } break;
        case 0x40: { chip->interleaved_prog = true; } break;
    }

    switch(nand->nand_id[2] & 0x80) {
        case 0x00: { chip->cache_prog = false; } break;
        case 0x80: { chip->cache_prog = true; } break;
    }

    switch(nand->nand_id[3] & 0x03) {
        case 0x00: { chip->data_bytes_per_page = 1024U; } break;
        case 0x01: { chip->data_bytes_per_page = 2048U; } break;
        case 0x02: { chip->data_bytes_per_page = 4096U; } break;
        case 0x03: { chip->data_bytes_per_page = 8192U; } break;
    }

    switch(nand->nand_id[3] & 0x04) {
        case 0x00: { chip->spare_bytes_per_512_bytes = 8; } break;
        case 0x04: { chip->spare_bytes_per_512_bytes = 16; } break;
    }

    switch(nand->nand_id[3] & 0x30) {
        case 0x00: { chip->data_bytes_per_block = 65536U; } break;
        case 0x10: { chip->data_bytes_per_block = 131072U; } break;
        case 0x20: { chip->data_bytes_per_block = 262144U; } break;
        case 0x30: { chip->data_bytes_per_block = 524288U; } break;
    }

    switch(nand->nand_id[3] & 0x40) {
        case 0x00: { chip->data_bus_width = 8; } break;
        case 0x40: { chip->data_bus_width = 16; } break;
    }

    switch(nand->nand_id[3] & 0x88) {
        case 0x00: { chip->serial_access_min_timings_ns = 50; } break; /** 50 ns or 30 ns */
        case 0x80: { chip->serial_access_min_timings_ns = 25; } break;
        default:   { chip->serial_access_min_timings_ns = 50; } break; /** reserved of 0x08 and 0x88 */
    }

    switch(nand->nand_id[4] & 0x0C) {
        case 0x00: { chip->plane_count = 1; } break;
        case 0x04: { chip->plane_count = 2; } break;
        case 0x08: { chip->plane_count = 4; } break;
        case 0x0C: { chip->plane_count = 8; } break;
    }

    switch(nand->nand_id[4] & 0x70) {
        case 0x00: { chip->data_blocks_per_plane = 8192U / chip->data_bytes_per_block * 1024U; } break; /** (64 / 8 * 1024 * 1024) */
        case 0x10: { chip->data_blocks_per_plane = 16384U / chip->data_bytes_per_block * 1024U; } break; /** (128 / 8 * 1024 * 1024) */
        case 0x20: { chip->data_blocks_per_plane = 32768U / chip->data_bytes_per_block * 1024U; } break; /** (256 / 8 * 1024 * 1024) */
        case 0x30: { chip->data_blocks_per_plane = 65536U / chip->data_bytes_per_block * 1024U; } break; /** (512 / 8 * 1024 * 1024) */
        case 0x40: { chip->data_blocks_per_plane = 131072U / chip->data_bytes_per_block * 1024U; } break; /** (1024 / 8 * 1024 * 1024) */
        case 0x50: { chip->data_blocks_per_plane = 262144U / chip->data_bytes_per_block * 1024U; } break; /** (2048 / 8 * 1024 * 1024) */
        case 0x60: { chip->data_blocks_per_plane = 524288U / chip->data_bytes_per_block * 1024U; } break; /** (4096 / 8 * 1024 * 1024) */
        case 0x70: { chip->data_blocks_per_plane = 1048576U / chip->data_bytes_per_block * 1024U; } break; /** (8192 / 8 * 1024 * 1024) */
    }

}