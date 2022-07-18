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
#include "ztimer.h"
#include "fmt.h"

int nand_onfi_init(nand_onfi_t* const nand, nand_onfi_params_t* const params);
size_t nand_onfi_run_cmd(nand_onfi_t* const nand, const nand_onfi_cmd_t* const cmd, nand_onfi_cmd_params_t* const cmd_params, nand_onfi_rw_response_t* const err);
size_t nand_onfi_write_addr_column(const nand_onfi_t* const nand, const uint64_t* const addr_column, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_onfi_write_addr_row(const nand_onfi_t* const nand, const uint64_t* const addr_row, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_onfi_write_io(const nand_onfi_t* const nand, const uint16_t* const data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_onfi_read_io(const nand_onfi_t* const nand, uint16_t* const out_data, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns);
void nand_onfi_set_ctrl_pin(const nand_onfi_t* const nand);
void nand_onfi_set_io_pin_write(const nand_onfi_t* const nand);
void nand_onfi_set_io_pin_read(const nand_onfi_t* const nand);
bool nand_onfi_wait_until_ready(const nand_onfi_t* const nand, const uint8_t this_lun_no, const uint32_t ready_this_lun_timeout_ns, const uint32_t ready_other_luns_timeout_ns);
bool nand_onfi_wait_until_lun_ready(const nand_onfi_t* const nand, const uint8_t this_lun_no, const uint32_t timeout_ns);

static inline size_t nand_onfi_write_cycle(const nand_onfi_t* const nand, const uint16_t* const cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    return nand_onfi_write_io(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
}

static inline size_t nand_onfi_write_raw(const nand_onfi_t* const nand, const uint16_t* const data, const size_t data_size, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;

    for(size_t seq = 0; seq < data_size; ++seq) {
        ret_size += nand_onfi_write_cycle(nand, &(data[seq]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    }

    return ret_size;
}

static inline size_t nand_onfi_write_cmd(const nand_onfi_t* const nand, const uint8_t* const cmd, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    uint16_t cmd_aligned = (uint16_t)(*cmd);
    return nand_onfi_write_cycle(nand, &cmd_aligned, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns); 
}

static inline size_t nand_onfi_write_addr(const nand_onfi_t* const nand, const uint64_t addr[], const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    ret_size += nand_onfi_write_addr_column(nand, &(addr[NAND_ONFI_ADDR_INDEX_COLUMN]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    ret_size += nand_onfi_write_addr_row(nand, &(addr[NAND_ONFI_ADDR_INDEX_ROW]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    return ret_size;
}

static inline size_t nand_onfi_write_addr_column_single(const nand_onfi_t* const nand, const uint16_t* const addr_column_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    return nand_onfi_write_cycle(nand, addr_column_single_cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
}

static inline size_t nand_onfi_write_addr_row_single(const nand_onfi_t* const nand, const uint16_t* const addr_row_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    print_str("nand_onfi_write_addr_row_single: ");
    print_u32_hex((uint32_t)(uintptr_t)nand);
    print_str("\r\n");
    return nand_onfi_write_cycle(nand, addr_row_single_cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
}

static inline size_t nand_onfi_read_cycle(const nand_onfi_t* const nand, uint16_t* const out_cycle_data, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    print_str("nand_onfi_read_cycle: ");
    //print_u32_hex((uint32_t)(uintptr_t)nand);
    print_str("\r\n");
    return 0;
    return nand_onfi_read_io(nand, out_cycle_data, cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
}

static inline size_t nand_onfi_read_raw(const nand_onfi_t* const nand, uint16_t* const out_buffer, const size_t buffer_size, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    size_t ret_size = 0;
    print_str("nand_onfi_read_raw: ");
    //print_u32_hex((uint32_t)(uintptr_t)nand);
    print_str("\r\n");
    return 0;

    for(size_t seq = 0; seq < buffer_size; ++seq) {
        ret_size += nand_onfi_read_cycle(nand, &(out_buffer[seq]), cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
    }

    return ret_size;
}

static inline void nand_onfi_set_pin_default(const nand_onfi_t* const nand) {
    nand_onfi_set_ctrl_pin(nand);
    nand_onfi_set_io_pin_write(nand);
}

static inline void nand_onfi_set_latch_command(const nand_onfi_t* const nand) {
    gpio_write(nand->params.ale, 0);
    gpio_write(nand->params.cle, 1);
}

static inline void nand_onfi_set_latch_address(const nand_onfi_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 1);
}

static inline void nand_onfi_set_latch_raw(const nand_onfi_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 0);
}

static inline void nand_onfi_set_read_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.re, 0);
}

static inline void nand_onfi_set_read_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.re, 1);
}

static inline void nand_onfi_set_write_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.we, 0);
}

static inline void nand_onfi_set_write_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.we, 1);
}

static inline void nand_onfi_set_write_protect_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.wp, 0);
}

static inline void nand_onfi_set_write_protect_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.wp, 1);
}

static inline void nand_onfi_set_chip_enable(const nand_onfi_t* const nand, const uint8_t lun_no) {
    switch(lun_no) {
    case 0:
        gpio_write(nand->params.ce0, 0);
        break;
    case 1:
        gpio_write(nand->params.ce1, 0);
        break;
    case 2:
        gpio_write(nand->params.ce2, 0);
        break;
    case 3:
        gpio_write(nand->params.ce3, 0);
        break;
    }

}

static inline void nand_onfi_set_chip_disable(const nand_onfi_t* const nand, const uint8_t lun_no) {
    switch(lun_no) {
    case 0:
        gpio_write(nand->params.ce0, 1);
        break;
    case 1:
        gpio_write(nand->params.ce1, 1);
        break;
    case 2:
        gpio_write(nand->params.ce2, 1);
        break;
    case 3:
        gpio_write(nand->params.ce3, 1);
        break;
    }

}

static inline void nand_onfi_wait(const uint32_t delay_ns) {
    if(delay_ns != 0) {
        ztimer_sleep(ZTIMER_USEC, delay_ns / NAND_ONFI_TIMING_MICROSEC(1));
    }
}

static inline size_t nand_onfi_all_pages_count(const nand_onfi_t* const nand) {
    return nand->pages_per_block * nand->blocks_per_lun * nand->lun_count;
}

static inline size_t nand_onfi_one_page_size(const nand_onfi_t* const nand) {
    return nand->data_bytes_per_page + nand->spare_bytes_per_page;
}

static inline size_t nand_onfi_all_data_bytes_size(const nand_onfi_t* const nand) {
    return nand->data_bytes_per_page * nand_onfi_all_pages_count(nand);
}

static inline size_t nand_onfi_all_spare_bytes_size(const nand_onfi_t* const nand) {
    return nand->spare_bytes_per_page * nand_onfi_all_pages_count(nand);
}

static inline size_t nand_onfi_all_pages_size(const nand_onfi_t* const nand) {
    return nand_onfi_all_data_bytes_size(nand) + nand_onfi_all_spare_bytes_size(nand);
}

static inline uint64_t nand_onfi_offset_to_addr_column(const uint64_t offset) {
    return offset;
}

static inline uint64_t nand_onfi_page_num_to_addr_row(const uint64_t page_num) {
    return page_num;
}

static inline uint64_t nand_onfi_addr_flat_to_addr_column(const nand_onfi_t* const nand, const uint64_t addr_flat) {
    return addr_flat % nand_onfi_one_page_size(nand);
}

static inline uint64_t nand_onfi_addr_flat_to_addr_row(const nand_onfi_t* const nand, const uint64_t addr_flat) {
    return addr_flat / nand_onfi_one_page_size(nand);
}

static inline uint64_t nand_onfi_addr_to_addr_flat(const nand_onfi_t* const nand, const uint64_t addr_row, const uint64_t addr_column) {
    return addr_row * nand_onfi_one_page_size(nand) + addr_column;
}

static inline uint32_t nand_onfi_deadline_from_interval(const uint32_t interval_ns) {
    return ztimer_now(ZTIMER_USEC) + (interval_ns / NAND_ONFI_TIMING_MICROSEC(1));
}

static inline uint32_t nand_onfi_deadline_left(const uint32_t deadline) {
    int32_t left = (int32_t)(deadline - ztimer_now(ZTIMER_USEC));
    return (left < 0) ? 0 : (uint32_t)left;
}

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_H */
/** @} */
