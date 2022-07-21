/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
_* This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nand NAND common driver
 * @ingroup     drivers_storage
 * @brief       Driver for reading and writing common NAND flash.
 * @anchor      drivers_nand
 * @{
 *
 * @file
 * @brief       Public interface for the nand driver.
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 */

#ifndef NAND_H
#define NAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "periph/gpio.h"
#include "ztimer.h"

#define NAND_MSB0                           (1)
#define NAND_MSB1                           (2)
#define NAND_MSB2                           (4)
#define NAND_MSB3                           (8)
#define NAND_MSB4                           (16)
#define NAND_MSB5                           (32)
#define NAND_MSB6                           (64)
#define NAND_MSB7                           (128)
#define NAND_MSB8                           (256)
#define NAND_MSB9                           (512)
#define NAND_MSB10                          (1024)
#define NAND_MSB11                          (2048)
#define NAND_MSB12                          (4096)
#define NAND_MSB13                          (8192)
#define NAND_MSB14                          (16384)
#define NAND_MSB15                          (32768)

#define NAND_MAX_CHIPS                      (8)

#define NAND_MAX_COMMAND_SIZE               (2)
#define NAND_MAX_COMMAND_CYCLE_SIZE         (10)
#define NAND_MIN_ID_SIZE                    (4)
#define NAND_MAX_ID_SIZE                    (20)
#define NAND_MAX_SIG_SIZE                   (20)

#define NAND_MAX_IO_BITS                    (16)

#define NAND_ADDR_INDEX_COLUMN              (0)
#define NAND_ADDR_INDEX_ROW                 (1)
#define NAND_ADDR_INDEX_ALL                 (2)
#define NAND_ADDR_IO_BITS                   (8)

#define NAND_MAX_ADDR_COLUMN_CYCLES         (10)
#define NAND_MAX_ADDR_ROW_CYCLES            (10)

#define NAND_INIT_ERROR                     (-1)    /**< returned on failed init */
#define NAND_INIT_OK                        (0)     /**< returned on successful init */
#define NAND_INIT_PARTIAL                   (1)     /**< returned on partial init */
#define NAND_INIT_ID_TOO_SHORT              (2)    /**< returned on failed init */
#define NAND_INIT_PARAMETER_PAGE_TOO_SHORT  (3)    /**< returned on failed init */

typedef enum {
    NAND_RW_OK = 0,             /**< no error */
    NAND_RW_TIMEOUT,            /**< cmd timed out (not-busy-state wasn't entered) */
    NAND_RW_WRITE_ERROR,        /**< data-packet response indicates error */
    NAND_RW_ECC_MISMATCH,       /**< CRC-mismatch of received data */
    NAND_RW_NOT_SUPPORTED,      /**< operation not supported on used card */
    NAND_RW_CMD_INVALID,
    NAND_RW_CMD_CHAIN_TOO_LONG
} nand_rw_response_t;

typedef struct _nand_params_t             nand_params_t;
typedef struct _nand_chip_t               nand_chip_t;
typedef struct _nand_prop_t               nand_prop_t;
typedef struct _nand_t                    nand_t;

/**
 * @brief   nand device params
 */
struct _nand_params_t {
    gpio_t ce0;             /**< pin connected to the I/O chip 0 enable */
    gpio_t ce1;             /**< pin that controls chip 1 enable */
    gpio_t ce2;             /**< pin that controls chip 2 enable */
    gpio_t ce3;             /**< pin that controls chip 3 enable */
    gpio_t rb0;             /**< pin connected to the chip 0 beady/busy */
    gpio_t rb1;             /**< pin connected to the chip 1 ready/busy */
    gpio_t rb2;             /**< pin connected to the chip 2 ready/busy */
    gpio_t rb3;             /**< pin connected to the chip 3 ready/busy */
    gpio_t re;              /**< pin that controls read enable */
    gpio_t we;              /**< pin that controls write enable*/
    gpio_t wp;              /**< pin that controls write protection */
    gpio_t cle;             /**< pin that controls command latch */
    gpio_t ale;             /**< pin that controls address latch */
    gpio_t io0;             /**< pin connected to the I/O 0 */
    gpio_t io1;             /**< pin connected to the I/O 1 */
    gpio_t io2;             /**< pin connected to the I/O 2 */
    gpio_t io3;             /**< pin connected to the I/O 3 */
    gpio_t io4;             /**< pin connected to the I/O 4 */
    gpio_t io5;             /**< pin connected to the I/O 5 */
    gpio_t io6;             /**< pin connected to the I/O 6 */
    gpio_t io7;             /**< pin connected to the I/O 7 */
    gpio_t io8;             /**< pin connected to the I/O 8 (only for 16-bit data access) */
    gpio_t io9;             /**< pin connected to the I/O 9 (only for 16-bit data access) */
    gpio_t io10;            /**< pin connected to the I/O 10 (only for 16-bit data access) */
    gpio_t io11;            /**< pin connected to the I/O 11 (only for 16-bit data access) */
    gpio_t io12;            /**< pin connected to the I/O 12 (only for 16-bit data access) */
    gpio_t io13;            /**< pin connected to the I/O 13 (only for 16-bit data access) */
    gpio_t io14;            /**< pin connected to the I/O 14 (only for 16-bit data access) */
    gpio_t io15;            /**< pin connected to the I/O 15 (only for 16-bit data access) */
};

struct _nand_t {
    bool                init_done;                 /**< set to true once the init procedure completed successfully */

    uint8_t             nand_id[NAND_MAX_ID_SIZE];
    uint8_t             nand_id_size;

    uint8_t             sig[NAND_MAX_SIG_SIZE];
    uint8_t             sig_size;

    uint8_t             data_bus_width;
    uint8_t             addr_bus_width;

    uint32_t            data_bytes_per_page;
    uint16_t            spare_bytes_per_page;
    uint32_t            pages_per_block;
    uint32_t            blocks_per_lun;
    uint8_t             lun_count;
    uint16_t            bb_per_lun;
    uint8_t             bits_per_cell;

    uint8_t             column_addr_cycles;
    uint8_t             row_addr_cycles;

    uint8_t             programs_per_page;

    nand_params_t       params;
};

int nand_init(nand_t* const nand, nand_params_t* const params);

size_t nand_write_addr_column(const nand_t* const nand, const uint64_t* const addr_column, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_write_addr_row(const nand_t* const nand, const uint64_t* const addr_row, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_write_addr_single(const nand_t* const nand, const uint16_t* const addr_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_write_raw(const nand_t* const nand, const uint8_t* const data, const size_t data_size, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);
size_t nand_write_io(const nand_t* const nand, const uint8_t data[2], const uint8_t bus_width, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns);

size_t nand_read_raw(const nand_t* const nand, uint8_t* const out_buffer, const size_t buffer_size, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns);
size_t nand_read_io(const nand_t* const nand, uint8_t out_data[2], const uint8_t bus_width, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns);

void nand_set_ctrl_pin(const nand_t* const nand);
void nand_set_io_pin_write(const nand_t* const nand);
void nand_set_io_pin_read(const nand_t* const nand);

void nand_wait(const uint32_t delay_ns);
bool nand_wait_until_ready(const nand_t* const nand, const uint8_t this_lun_no, const uint32_t ready_this_lun_timeout_ns, const uint32_t ready_other_luns_timeout_ns);
bool nand_wait_until_lun_ready(const nand_t* const nand, const uint8_t this_lun_no, const uint32_t timeout_ns);

bool nand_check_DDR(const uint8_t * const bytes, const size_t bytes_size);
size_t nand_fold_DDR_repeat_bytes(uint8_t * const bytes, const size_t bytes_size, const uint8_t filling_empty_byte);

static inline size_t nand_write_cycle(const nand_t* const nand, const uint8_t cycle_data[2], const uint8_t bus_width, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    return nand_write_io(nand, cycle_data, bus_width, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
}

static inline size_t nand_write_cmd(const nand_t* const nand, const uint8_t* const cmd, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    return nand_write_cycle(nand, cmd, 8, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns); 
}

static inline size_t nand_write_addr(const nand_t* const nand, const uint64_t addr[], const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    ret_size += nand_write_addr_column(nand, &(addr[NAND_ADDR_INDEX_COLUMN]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    ret_size += nand_write_addr_row(nand, &(addr[NAND_ADDR_INDEX_ROW]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    return ret_size;
}

static inline size_t nand_read_cycle(const nand_t* const nand, uint8_t out_cycle_data[2], const uint8_t bus_width, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    return nand_read_io(nand, out_cycle_data, bus_width, cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
}

static inline void nand_set_pin_default(const nand_t* const nand) {
    nand_set_ctrl_pin(nand);
    nand_set_io_pin_write(nand);
}

static inline void nand_set_latch_command(const nand_t* const nand) {
    gpio_write(nand->params.ale, 0);
    gpio_write(nand->params.cle, 1);
}

static inline void nand_set_latch_address(const nand_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 1);
}

static inline void nand_set_latch_raw(const nand_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 0);
}

static inline void nand_set_read_enable(const nand_t* const nand) {
    gpio_write(nand->params.re, 0);
}

static inline void nand_set_read_disable(const nand_t* const nand) {
    gpio_write(nand->params.re, 1);
}

static inline void nand_set_write_enable(const nand_t* const nand) {
    gpio_write(nand->params.we, 0);
}

static inline void nand_set_write_disable(const nand_t* const nand) {
    gpio_write(nand->params.we, 1);
}

static inline void nand_set_write_protect_enable(const nand_t* const nand) {
    gpio_write(nand->params.wp, 0);
}

static inline void nand_set_write_protect_disable(const nand_t* const nand) {
    gpio_write(nand->params.wp, 1);
}

static inline void nand_set_chip_enable(const nand_t* const nand, const uint8_t lun_no) {
    switch(lun_no) {
    case 0:
        {
            gpio_write(nand->params.ce0, 0);
        }
        break;
    case 1:
        {
            gpio_write(nand->params.ce1, 0);
        }
        break;
    case 2:
        {
            gpio_write(nand->params.ce2, 0);
        }
        break;
    case 3:
        {
            gpio_write(nand->params.ce3, 0);
        }
        break;
    }
}

static inline void nand_set_chip_disable(const nand_t* const nand, const uint8_t lun_no) {
    switch(lun_no) {
    case 0:
        {
            gpio_write(nand->params.ce0, 1);
        }
        break;
    case 1:
        {
            gpio_write(nand->params.ce1, 1);
        }
        break;
    case 2:
        {
            gpio_write(nand->params.ce2, 1);
        }
        break;
    case 3:
        {
            gpio_write(nand->params.ce3, 1);
        }
        break;
    }
}

static inline size_t nand_all_pages_count(const nand_t* const nand) {
    return nand->pages_per_block * nand->blocks_per_lun * nand->lun_count;
}

static inline size_t nand_one_lun_pages_count(const nand_t* const nand) {
    return nand_all_pages_count(nand) / nand->lun_count;
}

static inline size_t nand_one_page_size(const nand_t* const nand) {
    return nand->data_bytes_per_page + nand->spare_bytes_per_page;
}

static inline size_t nand_all_data_bytes_size(const nand_t* const nand) {
    return nand->data_bytes_per_page * nand_all_pages_count(nand);
}

static inline size_t nand_all_spare_bytes_size(const nand_t* const nand) {
    return nand->spare_bytes_per_page * nand_all_pages_count(nand);
}

static inline size_t nand_all_pages_size(const nand_t* const nand) {
    return nand_all_data_bytes_size(nand) + nand_all_spare_bytes_size(nand);
}

static inline size_t nand_one_lun_pages_size(const nand_t* const nand) {
    return nand_all_pages_size(nand) / nand->lun_count;
}

static inline uint64_t nand_offset_to_addr_column(const uint64_t offset) {
    return offset;
}

static inline uint64_t nand_page_no_to_addr_row(const uint64_t page_no) {
    return page_no;
}

static inline uint64_t nand_addr_flat_to_addr_column(const nand_t* const nand, const uint64_t addr_flat) {
    return addr_flat % nand_one_page_size(nand);
}

static inline uint64_t nand_addr_flat_to_addr_row(const nand_t* const nand, const uint64_t addr_flat) {
    return addr_flat / nand_one_page_size(nand);
}

static inline uint64_t nand_addr_to_addr_flat(const nand_t* const nand, const uint64_t addr_row, const uint64_t addr_column) {
    return addr_row * nand_one_page_size(nand) + addr_column;
}

static inline uint32_t nand_deadline_from_interval(const uint32_t interval_ns) {
    return ztimer_now(ZTIMER_USEC) + (interval_ns / 1000);
}

static inline uint32_t nand_deadline_left(const uint32_t deadline) {
    int32_t left = (int32_t)(deadline - ztimer_now(ZTIMER_USEC));
    return (left < 0) ? 0 : (uint32_t)left;
}

#ifdef __cplusplus
}
#endif

#endif /* NAND_H */
/** @} */
