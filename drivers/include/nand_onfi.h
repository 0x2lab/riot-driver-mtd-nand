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

#include "periph/gpio.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAND_ONFI_INIT_ERROR                        (-1)   /**< returned on failed init */
#define NAND_ONFI_OK                                (0)    /**< returned on successful init */

#define NAND_ONFI_MAX_CHIPS                         (8)
#define NAND_ONFI_MAX_COMMAND_SIZE                  (2)
#define NAND_ONFI_MAX_COMMAND_CYCLE_SIZE            (10)
#define NAND_ONFI_MAX_ID_SIZE                       (20)
#define NAND_ONFI_MAX_UNIQUE_ID_SIZE                (512)
#define NAND_ONFI_MAX_PARAMETER_PAGE_SIZE           (8192 + 256)      /**< ONFI states standard as 0-767 */
#define NAND_ONFI_MAX_IO_BITS                       (16)

#define NAND_ONFI_MSB0                              (1)
#define NAND_ONFI_MSB1                              (2)
#define NAND_ONFI_MSB2                              (4)
#define NAND_ONFI_MSB3                              (8)
#define NAND_ONFI_MSB4                              (16)
#define NAND_ONFI_MSB5                              (32)
#define NAND_ONFI_MSB6                              (64)
#define NAND_ONFI_MSB7                              (128)
#define NAND_ONFI_MSB8                              (256)
#define NAND_ONFI_MSB9                              (512)
#define NAND_ONFI_MSB10                             (1024)
#define NAND_ONFI_MSB11                             (2048)
#define NAND_ONFI_MSB12                             (4096)
#define NAND_ONFI_MSB13                             (8192)
#define NAND_ONFI_MSB14                             (16384)
#define NAND_ONFI_MSB15                             (32768)

#define NAND_ONFI_ADDR_INDEX_COLUMN                 (0)
#define NAND_ONFI_ADDR_INDEX_ROW                    (1)
#define NAND_ONFI_ADDR_INDEX_ALL                    (2)

#define NAND_ONFI_ADDR_IO_BITS                      (8)
#define NAND_ONFI_ADDR_COLUMN_CYCLES                (2)
#define NAND_ONFI_ADDR_ROW_CYCLES                   (3)

#define NAND_ONFI_INVALID_CYCLE                     (-1)
#define NAND_ONFI_PAGE_SIZE_DEPENDANT_CYCLES        (-2)

#define _NAND_ONFI_ADDR_CYCLES                      (NAND_ONFI_ADDR_COLUMN_CYCLES + NAND_ONFI_ADDR_ROW_CYCLES)
#define _NAND_ONFI_MAX_ADDR_COLUMN_SIZE             (1 << (NAND_ONFI_ADDR_IO_BITS * NAND_ONFI_ADDR_COLUMN_CYCLES))
#define _NAND_ONFI_MAX_ADDR_ROW_SIZE                (1 << (NAND_ONFI_ADDR_IO_BITS * NAND_ONFI_ADDR_ROW_CYCLES))

static const size_t   NAND_ONFI_ADDR_CYCLES          = _NAND_ONFI_ADDR_CYCLES;
static const uint32_t NAND_ONFI_MAX_ADDR_COLUMN_SIZE = _NAND_ONFI_MAX_ADDR_COLUMN_SIZE;
static const uint32_t NAND_ONFI_MAX_ADDR_ROW_SIZE    = _NAND_ONFI_MAX_ADDR_ROW_SIZE;

#define NAND_ONFI_TIMING_NANOSEC(x)                 (x)
#define NAND_ONFI_TIMING_MICROSEC(x)                (NAND_ONFI_TIMING_NANOSEC(1000 * (x)))
#define NAND_ONFI_TIMING_INFINITY                   (NAND_ONFI_TIMING_MICROSEC(10000))

#define NAND_ONFI_TIMING_ADL                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_BERS                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CCS                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CEH                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CH                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CS                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_DH                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_DS                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_FEAT                       (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_ITC                        (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_PROG                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_R                          (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RR                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RST                        (NAND_ONFI_TIMING_MICROSEC(5000))
#define NAND_ONFI_TIMING_WB                         (NAND_ONFI_TIMING_NANOSEC(200))
#define NAND_ONFI_TIMING_WHR                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WHRT                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WTRN                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WW                         (NAND_ONFI_TIMING_INFINITY)

#define NAND_ONFI_TIMING_PLEBSY                     (NAND_ONFI_TIMING_BERS)
#define NAND_ONFI_TIMING_PLPBSY                     (NAND_ONFI_TIMING_PROG)
#define NAND_ONFI_TIMING_PLRBSY                     (NAND_ONFI_TIMING_R)
#define NAND_ONFI_TIMING_PCBSY                      (NAND_ONFI_TIMING_PROG)
#define NAND_ONFI_TIMING_RCBSY                      (NAND_ONFI_TIMING_R)

#define NAND_ONFI_TIMING_VDLY                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CEVDLY                     (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_ENI                        (NAND_ONFI_TIMING_NANOSEC(15))
#define NAND_ONFI_TIMING_ENO                        (NAND_ONFI_TIMING_NANOSEC(50))

#define NAND_ONFI_TIMING_ZQCL                       (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_ZQCS                       (NAND_ONFI_TIMING_NANOSEC(400))

#define NAND_ONFI_TIMING_ALH                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_ALS                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_AR                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CEA                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_CHZ                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_CLH                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CLR                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CLS                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_COH                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CR                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CR2                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CR2_READ_ID                (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_CS3                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_IR                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RC                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_REA                        (NAND_ONFI_TIMING_NANOSEC(40))
#define NAND_ONFI_TIMING_REH                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RHOH                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RHW                        (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RHZ                        (NAND_ONFI_TIMING_NANOSEC(200))
#define NAND_ONFI_TIMING_RLOH                       (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_RP                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WC                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WH                         (NAND_ONFI_TIMING_INFINITY)
#define NAND_ONFI_TIMING_WP                         (NAND_ONFI_TIMING_INFINITY)


typedef enum   _nand_onfi_rw_response_t     nand_onfi_rw_response_t;
typedef enum   _nand_onfi_cmd_type_t        nand_onfi_cmd_type_t;
typedef enum   _nand_onfi_cmd_rw_t          nand_onfi_cmd_rw_t;
typedef struct _nand_onfi_raw_t             nand_onfi_raw_t;
typedef union  _nand_onfi_cmd_cycle_t       nand_onfi_cmd_cycle_t;
typedef struct _nand_onfi_cmd_data_t        nand_onfi_cmd_data_t;
typedef struct _nand_onfi_cmd_t             nand_onfi_cmd_t;
typedef struct _nand_onfi_run_cmd_params_t  nand_onfi_run_cmd_params_t;
typedef struct _nand_onfi_t                 nand_onfi_t;
typedef struct _nand_onfi_params_t          nand_onfi_params_t;
typedef enum   _nand_onfi_version_t         nand_onfi_version_t;
typedef struct _nand_onfi_chip_t            nand_onfi_chip_t;
typedef struct _nand_onfi_prop_t            nand_onfi_prop_t;
typedef void (*nand_onfi_hook_cb_t)(nand_onfi_t* const nand, const nand_onfi_cmd_data_t* const cmd_data, nand_onfi_run_cmd_params_t* const cmd_param);

enum _nand_onfi_rw_response_t {
    NAND_ONFI_RW_OK = 0,           /**< no error */
    NAND_ONFI_RW_TIMEOUT,          /**< cmd timed out (not-busy-state wasn't entered) */
    NAND_ONFI_RW_WRITE_ERROR,      /**< data-packet response indicates error */
    NAND_ONFI_RW_ECC_MISMATCH,     /**< CRC-mismatch of received data */
    NAND_ONFI_RW_NOT_SUPPORTED     /**< operation not supported on used card */
};

enum _nand_onfi_cmd_type_t {
    NAND_ONFI_CMD_TYPE_CMD,
    NAND_ONFI_CMD_TYPE_ADDR,
    NAND_ONFI_CMD_TYPE_ADDR_COLUMN,
    NAND_ONFI_CMD_TYPE_ADDR_ROW,
    NAND_ONFI_CMD_TYPE_RAW
};

enum _nand_onfi_cmd_rw_t {
    NAND_ONFI_CMD_RW_WRITE = 0,
    NAND_ONFI_CMD_RW_READ = 1
};

struct _nand_onfi_raw_t {
    uint16_t*               buffer;
    size_t                  buffer_size;
    size_t                  buffer_seq;
    size_t                  raw_offset;
    size_t                  raw_size;
};

union _nand_onfi_cmd_cycle_t {
    uint8_t                 cmd;
    uint32_t                addr[NAND_ONFI_ADDR_INDEX_ALL];
    uint32_t                addr_column;
    uint32_t                addr_row;
    nand_onfi_raw_t         raw;
};

struct _nand_onfi_cmd_data_t {
    nand_onfi_cmd_type_t    type;
    nand_onfi_cmd_rw_t      rw;
    bool                    wait_until_ready;
    bool                    wait_until_other_luns_ready;
    nand_onfi_cmd_cycle_t   cycles;
    size_t                  raw_size;
    uint32_t                delay_ns;
    uint32_t                timeout_ns;
};

struct _nand_onfi_cmd_t {
    size_t                  length;
    nand_onfi_cmd_data_t    cmd_data[NAND_ONFI_MAX_COMMAND_CYCLE_SIZE];
};

struct _nand_onfi_run_cmd_params_t {
    uint8_t                 lun;
    nand_onfi_hook_cb_t     pre_hook_cb;
    nand_onfi_cmd_cycle_t   cycles;
    nand_onfi_hook_cb_t     post_hook_cb;
};

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ                           = {
        .length = 4,
        .cmd_data = {
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = true, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x00 } },
                { .type = NAND_ONFI_CMD_TYPE_ADDR, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = true,.wait_until_other_luns_ready = false },
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = true, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x30 } },
                { .type = NAND_ONFI_CMD_TYPE_RAW, .rw = NAND_ONFI_CMD_RW_READ, .wait_until_ready = true, .wait_until_other_luns_ready = false, .raw_size = NAND_ONFI_PAGE_SIZE_DEPENDANT_CYCLES }
            }
    };

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_ID                        = {
        .length = 3,
        .cmd_data = {
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = false, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x90 } },
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = false, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x00 } },
                { .type = NAND_ONFI_CMD_TYPE_RAW, .rw = NAND_ONFI_CMD_RW_READ, .wait_until_ready = false, .wait_until_other_luns_ready = false, .raw_size = NAND_ONFI_MAX_ID_SIZE }
            }
    };

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_ID_ONFI_SIG               = {
        .length = 3,
        .cmd_data = {
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = false, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x90 } },
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = false, .wait_until_other_luns_ready = false, .cycles = { .cmd = 0x20 } },
                { .type = NAND_ONFI_CMD_TYPE_RAW, .rw = NAND_ONFI_CMD_RW_READ, .wait_until_ready = false, .wait_until_other_luns_ready = false, .raw_size = NAND_ONFI_MAX_ID_SIZE }
            }
    };

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_PARAMETER_PAGE            = {
        .length = 3,
        .cmd_data = {
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = true, .wait_until_other_luns_ready = true, .cycles = { .cmd = 0xEC } },
                { .type = NAND_ONFI_CMD_TYPE_CMD, .rw = NAND_ONFI_CMD_RW_WRITE, .wait_until_ready = true, .wait_until_other_luns_ready = true, .cycles = { .cmd = 0x00 } },
                { .type = NAND_ONFI_CMD_TYPE_RAW, .rw = NAND_ONFI_CMD_RW_READ, .wait_until_ready = true, .wait_until_other_luns_ready = true, .raw_size = NAND_ONFI_MAX_PARAMETER_PAGE_SIZE }
            }
    };
#if 0
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_CACHE_RANDOM              = { .cmd_data = { 0x00, 0x31 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_MULTI_PLANE               = { .cmd_data = { 0x00, 0x32 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_COPYBACK_READ                  = { .cmd_data = { 0x00, 0x35 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_CHANGE_READ_COLUMN             = { .cmd_data = { 0x05, 0xe0 }, .params = NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN }; /**< Copyback read with data output */
static const nand_onfi_cmd_t NAND_ONFI_CMD_CHANGE_READ_COLUMN_ENHANCED    = { .cmd_data = { 0x06, 0xe0 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_ODT_DISABLE                    = { .cmd_data = { 0x1b       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_ODT_ENABLE                     = { .cmd_data = { 0x1c       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_CACHE_SEQUENTIAL_CONTINUE = { .cmd_data = { 0x31       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN }; /**< Non-standard but ONFI-compliant */
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_CACHE_SEQUENTIAL          = { .cmd_data = { 0x31       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_CACHE_END                 = { .cmd_data = { 0x3f       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_BLOCK_ERASE                    = { .cmd_data = { 0x60, 0xd0 }, .params = NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_BLOCK_ERASE_MULTI_PLANE        = { .cmd_data = { 0x60, 0xd1 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_STATUS                    = { .cmd_data = { 0x70       }, .params = NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_STATUS_ENHANCED           = { .cmd_data = { 0x78       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_PAGE_PROGRAM                   = { .cmd_data = { 0x80, 0x10 }, .params = NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_PAGE_PROGRAM_MULTI_PLANE       = { .cmd_data = { 0x80, 0x11 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_PAGE_CACHE_PROGRAM             = { .cmd_data = { 0x80, 0x15 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_COPYBACK_PROGRAM               = { .cmd_data = { 0x85, 0x10 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_COPYBACK_PROGRAM_DATA_MODIFY   = { .cmd_data = { 0x85, 0x10 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN }; /**< Non-standard but ONFI-compliant */
static const nand_onfi_cmd_t NAND_ONFI_CMD_COPYBACK_PROGRAM_MULTI_PLANE   = { .cmd_data = { 0x85, 0x11 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_SMALL_DATA_MOVE                = { .cmd_data = { 0x85, 0x15 }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_CHANGE_WRITE_COLUMN            = { .cmd_data = { 0x85       }, .params = NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_CHANGE_ROW_ADDRESSS            = { .cmd_data = { 0x85       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_ID                        = { .cmd_data = { 0x90       } };
static const nand_onfi_cmd_t NAND_ONFI_CMD_VOLUME_SELECT                  = { .cmd_data = { 0xe1       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_ODT_CONFIGURE                  = { .cmd_data = { 0xe2       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_PARAMETER_PAGE            = { .cmd_data = { 0xec       } };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_UNIQUE_ID                 = { .cmd_data = { 0xed       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_GET_FEATURES                   = { .cmd_data = { 0xee       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_SET_FEATURES                   = { .cmd_data = { 0xef       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_COMMAND_BASED_DCC_TRAINING     = { .cmd_data = { 0x18       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_DQ_TRAINING               = { .cmd_data = { 0x62       } };
static const nand_onfi_cmd_t NAND_ONFI_CMD_WRITE_TX_DQ_TRAINING_PATTERN   = { .cmd_data = { 0x63       } };
static const nand_onfi_cmd_t NAND_ONFI_CMD_WRITE_TX_DQ_TRAINING_READBACK  = { .cmd_data = { 0x64       } };
static const nand_onfi_cmd_t NAND_ONFI_CMD_WRITE_RX_DQ_TRAINING           = { .cmd_data = { 0x76       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL };
static const nand_onfi_cmd_t NAND_ONFI_CMD_LUN_GET_FEATURES               = { .cmd_data = { 0xd4       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_LUN_SET_FEATURES               = { .cmd_data = { 0xd5       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_ZQ_CALIBRATION_SHORT           = { .cmd_data = { 0xd9       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_ZQ_CALIBRATION_LONG            = { .cmd_data = { 0xf9       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_OTHER_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_RESET_LUN                      = { .cmd_data = { 0xfa       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_SYNCHRONOUS_RESET              = { .cmd_data = { 0xfc       }, .params = NAND_ONFI_CMD_PARAM_OPTIONAL | NAND_ONFI_CMD_PARAM_READY_BUSY | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
static const nand_onfi_cmd_t NAND_ONFI_CMD_RESET                          = { .cmd_data = { 0xff       }, .params = NAND_ONFI_CMD_PARAM_READY_BUSY | NAND_ONFI_CMD_PARAM_ALLOW_ALL_BUSY_LUN };
#endif

/**
 * @brief   nand_onfi device params
 */
struct _nand_onfi_params_t {
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

/**
 * @brief   version type of ONFI
 */
enum _nand_onfi_version_t {
    NAND_ONFI_V10 = 10,         /**< ONFI version 1.0 */
    NAND_ONFI_V20 = 20,         /**< ONFI version 2.0 */
    NAND_ONFI_V21 = 21,         /**< ONFI version 2.1 */
    NAND_ONFI_V22 = 22,         /**< ONFI version 2.2 */
    NAND_ONFI_V23 = 23,         /**< ONFI version 2.3 */
    NAND_ONFI_V30 = 30,         /**< ONFI version 3.0 */
    NAND_ONFI_V31 = 31,         /**< ONFI version 3.1 */
    NAND_ONFI_V32 = 32,         /**< ONFI version 3.2 */
    NAND_ONFI_V40 = 40,         /**< ONFI version 4.0 */
    NAND_ONFI_V50 = 50          /**< ONFI version 5.0 */
};

struct _nand_onfi_chip_t {
    /* revision information and features block */
    /* 'O' 'N' 'F' 'I'  */
    uint8_t  sig[4];
    uint16_t revision;
    uint16_t features;
    uint16_t opt_cmd;
    uint8_t  reserved0[2];
    uint16_t ext_param_page_length; /**< since ONFI 2.1 */
    uint8_t  num_of_param_pages;    /**< since ONFI 2.1 */
    uint8_t  reserved1[17];

    /* manufacturer information block */
    char     manufacturer[12];
    char     model[20];
    uint8_t  jedec_id;
    uint16_t date_code;
    uint8_t  reserved2[13];

    /* memory organization block */
    uint32_t byte_per_page;
    uint16_t spare_bytes_per_page;
    uint32_t data_bytes_per_ppage;
    uint16_t spare_bytes_per_ppage;
    uint32_t pages_per_block;
    uint32_t blocks_per_lun;
    uint8_t  lun_count;
    uint8_t  addr_cycles;
    uint8_t  bits_per_cell;
    uint16_t bb_per_lun;
    uint16_t block_endurance;
    uint8_t  guaranteed_good_blocks;
    uint16_t guaranteed_block_endurance;
    uint8_t  programs_per_page;
    uint8_t  ppage_attr;
    uint8_t  ecc_bits;
    uint8_t  interleaved_bits;
    uint8_t  interleaved_ops;
    uint8_t  reserved3[13];

    /* electrical parameter block */
    uint8_t  io_pin_capacitance_max;
    uint16_t sdr_timing_modes;
    uint16_t program_cache_timing_mode;
    uint16_t t_prog;
    uint16_t t_bers;
    uint16_t t_r;
    uint16_t t_ccs;
    uint8_t  nvddr_timing_modes;
    uint8_t  nvddr2_timing_modes;
    uint8_t  nvddr_nvddr2_features;
    uint16_t clk_pin_capacitance_typ;
    uint16_t io_pin_capacitance_typ;
    uint16_t input_pin_capacitance_typ;
    uint8_t  input_pin_capacitance_max;
    uint8_t  driver_strength_support;
    uint16_t t_int_r;
    uint16_t t_adl;
    uint8_t  reserved4[8];

    /* vendor */
    uint16_t vendor_revision;
    uint8_t  vendor[88];

    uint16_t crc;
};

struct _nand_onfi_prop_t {
    nand_onfi_version_t version;            /**< ONFI version (BCD encoded), 0 if ONFI is not supported */
    uint16_t            t_prog;             /**< Page program time */
    uint16_t            t_bers;             /**< Block erase time */
    uint16_t            t_r;                /**< Page read time */
    uint16_t            t_ccs;              /**< Change column setup time */
    bool                fast_t_cad;         /**< Command/Address/Data slow or fast delay (NV-DDR only) */
    uint16_t            sdr_timing_modes;   /**< Supported asynchronous/SDR timing modes */
    uint16_t            nvddr_timing_modes; /**< Supported source synchronous/NV-DDR timing modes */
};

struct _nand_onfi_t {
    nand_onfi_chip_t    chip;
    nand_onfi_prop_t    onfi;

    uint8_t             data_bus_width;

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

    bool                init_done;                 /**< set to true once the init procedure completed successfully */

    nand_onfi_params_t  params;
};

int nand_onfi_init(nand_onfi_t* const nand, nand_onfi_params_t* const params);

size_t nand_onfi_run_cmd(nand_onfi_t* const nand, const nand_onfi_cmd_t cmd, nand_onfi_run_cmd_params_t cmd_params[], nand_onfi_rw_response_t* const err);
size_t nand_onfi_write_cmd(const nand_onfi_t* const nand, const uint8_t* const cmd);
size_t nand_onfi_write_addr(const nand_onfi_t* const nand, const uint32_t addr[]);
size_t nand_onfi_write_addr_column(const nand_onfi_t* const nand, const uint32_t* const addr_column);
size_t nand_onfi_write_addr_row(const nand_onfi_t* const nand, const uint32_t* const addr_row);
size_t nand_onfi_write_raw(const nand_onfi_t* const nand, const uint16_t* const raw_data, const size_t raw_size);
size_t nand_onfi_write_cycle(const nand_onfi_t* const nand, const uint16_t* const cycle_data);
size_t nand_onfi_write_io(const nand_onfi_t* const nand, const uint16_t* const data);

size_t nand_onfi_read_cycle(const nand_onfi_t* const nand, uint16_t* const out_cycle_data);
size_t nand_onfi_read_io(const nand_onfi_t* const nand, uint16_t* const out_data);

void nand_onfi_set_pin_default(const nand_onfi_t* const nand);
void nand_onfi_set_ctrl_pin(const nand_onfi_t* const nand);
void nand_onfi_set_io_pin_write(const nand_onfi_t* const nand);
void nand_onfi_set_io_pin_read(const nand_onfi_t* const nand);

void nand_onfi_set_latch_command(const nand_onfi_t* const nand);
void nand_onfi_set_latch_address(const nand_onfi_t* const nand);
void nand_onfi_set_latch_raw(const nand_onfi_t* const nand);

void nand_onfi_set_read_enable(const nand_onfi_t* const nand);
void nand_onfi_set_read_disable(const nand_onfi_t* const nand);
void nand_onfi_set_write_enable(const nand_onfi_t* const nand);
void nand_onfi_set_write_disable(const nand_onfi_t* const nand);
void nand_onfi_set_chip_enable(const nand_onfi_t* const nand, const uint8_t lun);
void nand_onfi_set_chip_disable(const nand_onfi_t* const nand, const uint8_t lun);

void nand_onfi_wait(const uint32_t delay_ns);
void nand_onfi_wait_cmd_data(const nand_onfi_cmd_data_t* const cmd_data);
bool nand_onfi_wait_until_ready(const nand_onfi_t* const nand, const nand_onfi_cmd_data_t* const cmd_data, const uint8_t this_lun);
bool nand_onfi_wait_until_lun_ready(const nand_onfi_t* const nand, const uint8_t this_lun, const uint32_t timeout_ns);

size_t nand_onfi_all_pages_count(const nand_onfi_t* const nand);
size_t nand_onfi_one_page_size(const nand_onfi_t* const nand);
size_t nand_onfi_all_data_bytes_size(const nand_onfi_t* const nand);
size_t nand_onfi_all_spare_bytes_size(const nand_onfi_t* const nand);
size_t nand_onfi_all_pages_size(const nand_onfi_t* const nand);

uint32_t nand_onfi_offset_to_addr_column(const uint32_t offset);
uint32_t nand_onfi_page_num_to_addr_row(const uint32_t page_num);
uint32_t nand_onfi_addr_flat_to_addr_column(const nand_onfi_t* const nand, const uint32_t addr_flat);
uint32_t nand_onfi_addr_flat_to_addr_row(const nand_onfi_t* const nand, const uint32_t addr_flat);
uint32_t nand_onfi_addr_to_addr_flat(const nand_onfi_t* const nand, const uint32_t addr_row, const uint32_t addr_column);

uint32_t nand_onfi_deadline_from_interval(const uint32_t interval_ns);
uint32_t nand_onfi_deadline_left(const uint32_t deadline);

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_H */
/** @} */
