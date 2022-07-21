/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nand_onfi
 * @{
 *
 * @file
 * @brief       low level driver for accessing ONFI NANDs
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"
#include "fmt.h"

#include "nand/onfi.h"
#include "nand_cmd.h"
#include "nand.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int nand_onfi_init(nand_onfi_t* const nand_onfi, nand_params_t* const params) {
    if(nand_onfi == NULL) {
        return NAND_INIT_ERROR;
    }

    nand_t* nand = (nand_t*)nand_onfi;
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

    nand->nand_id_size          = nand_cmd_read_id(nand, 0, &NAND_ONFI_CMD_READ_ID, nand->nand_id, NAND_MAX_ID_SIZE);
    if(nand->nand_id_size < NAND_MIN_ID_SIZE) {
        return NAND_INIT_ID_TOO_SHORT;
    }

    nand->sig_size              = nand_cmd_read_id(nand, 0, &NAND_ONFI_CMD_READ_ID_ONFI_SIG, nand->sig, NAND_MAX_SIG_SIZE);

    const size_t pp_size        = nand_onfi_read_chip(nand_onfi, 0, &(nand_onfi->onfi_chip));
    if(pp_size < (NAND_ONFI_PARAMETER_PAGE_SIZE / 8)) {
        /** TODO: Parameter page too short */
        return NAND_INIT_PARAMETER_PAGE_TOO_SHORT;
    }

    nand->data_bus_width        = (nand_onfi->onfi_chip.features & 0x1) ? 16 : 8;
    nand->addr_bus_width        = 8;

    nand->data_bytes_per_page   = nand_onfi->onfi_chip.byte_per_page;
    nand->spare_bytes_per_page  = nand_onfi->onfi_chip.spare_bytes_per_page;
    nand->pages_per_block       = nand_onfi->onfi_chip.pages_per_block;
    nand->blocks_per_lun        = nand_onfi->onfi_chip.blocks_per_lun;
    nand->lun_count             = nand_onfi->onfi_chip.lun_count;
    nand->bb_per_lun            = nand_onfi->onfi_chip.bb_per_lun;

    nand->column_addr_cycles    = (nand_onfi->onfi_chip.addr_cycles & 0xF0) >> 4;
    nand->row_addr_cycles       = (nand_onfi->onfi_chip.addr_cycles & 0x0F);

    nand->bits_per_cell         = nand_onfi->onfi_chip.bits_per_cell;
    nand->programs_per_page     = nand_onfi->onfi_chip.programs_per_page;

    //TODO: nand_onfi->onfi_prop init

    nand->init_done             = true;

    return NAND_INIT_OK;
}

size_t nand_onfi_read_chip(nand_onfi_t* const nand_onfi, const uint8_t this_lun_no, nand_onfi_chip_t* const chip) {
    const size_t          buffer_size   = sizeof(nand_onfi_chip_t);
          uint8_t*  const buffer        = (uint8_t*)malloc(sizeof(uint8_t) * buffer_size);
    const size_t          pp_size       = nand_cmd_read_parameter_page((nand_t*)nand_onfi, this_lun_no, &NAND_ONFI_CMD_READ_PARAMETER_PAGE, buffer, buffer_size);
    if(pp_size == 0) {
        free(buffer);
        return 0;
    }

    memcpy(chip, buffer, sizeof(uint8_t) * pp_size);

    free(buffer);
    return pp_size;
}