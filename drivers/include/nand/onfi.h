/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nand_onfi ONFI NAND driver
 * @ingroup     drivers_storage
 * @brief       Driver for reading and writing ONFI NANDs.
 * @anchor      drivers_nand_onfi
 * @{
 *
 * @file
 * @brief       Public interface for the nand_onfi driver.
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 */

#ifndef NAND_ONFI_H
#define NAND_ONFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "nand/onfi/msb.h"
#include "nand/onfi/enums.h"
#include "nand/onfi/constants.h"
#include "nand/onfi/constants_timing.h"
#include "nand/onfi/types.h"
#include "nand/onfi/cmd.h"
#include "nand/onfi/cmd_timing.h"

int         nand_onfi_init(nand_onfi_t * const, nand_onfi_params_t * const);
size_t      nand_onfi_run_cmd(nand_onfi_t * const, const nand_onfi_cmd_t * const, nand_onfi_cmd_params_t * const, nand_onfi_rw_response_t * const);
size_t      nand_onfi_write_cmd(const nand_onfi_t * const, const uint8_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_write_addr(const nand_onfi_t * const, const uint64_t [], const uint32_t, const uint32_t);
size_t      nand_onfi_write_addr_column(const nand_onfi_t * const, const uint64_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_write_addr_column_single(const nand_onfi_t* const nand, const uint16_t* const addr_column_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t      nand_onfi_write_addr_row(const nand_onfi_t * const, const uint64_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_write_addr_row_single(const nand_onfi_t* const nand, const uint16_t* const addr_row_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t      nand_onfi_write_raw(const nand_onfi_t * const, const uint16_t * const, const size_t, const uint32_t, const uint32_t);
size_t      nand_onfi_write_cycle(const nand_onfi_t * const, const uint16_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_write_io(const nand_onfi_t * const, const uint16_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_read_raw(const nand_onfi_t * const, uint16_t * const, const size_t, const uint32_t, const uint32_t);
size_t      nand_onfi_read_cycle(const nand_onfi_t * const, uint16_t * const, const uint32_t, const uint32_t);
size_t      nand_onfi_read_io(const nand_onfi_t * const, uint16_t * const, const uint32_t, const uint32_t);
void        nand_onfi_set_pin_default(const nand_onfi_t * const);
void        nand_onfi_set_ctrl_pin(const nand_onfi_t * const);
void        nand_onfi_set_io_pin_write(const nand_onfi_t * const);
void        nand_onfi_set_io_pin_read(const nand_onfi_t * const);
void        nand_onfi_set_latch_command(const nand_onfi_t * const);
void        nand_onfi_set_latch_address(const nand_onfi_t * const);
void        nand_onfi_set_latch_raw(const nand_onfi_t * const);
void        nand_onfi_set_read_enable(const nand_onfi_t * const);
void        nand_onfi_set_read_disable(const nand_onfi_t * const);
void        nand_onfi_set_write_enable(const nand_onfi_t * const);
void        nand_onfi_set_write_disable(const nand_onfi_t * const);
void        nand_onfi_set_write_protect_enable(const nand_onfi_t * const);
void        nand_onfi_set_write_protect_disable(const nand_onfi_t * const);
void        nand_onfi_set_chip_enable(const nand_onfi_t * const, const uint8_t);
void        nand_onfi_set_chip_disable(const nand_onfi_t * const, const uint8_t);
void        nand_onfi_wait(const uint32_t);
bool        nand_onfi_wait_until_ready(const nand_onfi_t * const, const uint8_t, const uint32_t, const uint32_t);
bool        nand_onfi_wait_until_lun_ready(const nand_onfi_t * const, const uint8_t, const uint32_t);
size_t      nand_onfi_all_pages_count(const nand_onfi_t * const);
size_t      nand_onfi_one_page_size(const nand_onfi_t * const);
size_t      nand_onfi_all_data_bytes_size(const nand_onfi_t * const);
size_t      nand_onfi_all_spare_bytes_size(const nand_onfi_t * const);
size_t      nand_onfi_all_pages_size(const nand_onfi_t * const);
uint64_t    nand_onfi_offset_to_addr_column(const uint64_t);
uint64_t    nand_onfi_page_num_to_addr_row(const uint64_t);
uint64_t    nand_onfi_addr_flat_to_addr_column(const nand_onfi_t * const, const uint64_t);
uint64_t    nand_onfi_addr_flat_to_addr_row(const nand_onfi_t * const, const uint64_t);
uint64_t    nand_onfi_addr_to_addr_flat(const nand_onfi_t * const, const uint64_t, const uint64_t);
uint32_t    nand_onfi_deadline_from_interval(const uint32_t);
uint32_t    nand_onfi_deadline_left(const uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_H */
/** @} */
