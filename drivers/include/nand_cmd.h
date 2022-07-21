/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
_* This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nand NAND common cmd
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
#ifndef NAND_CMD_H
#define NAND_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "nand.h"

typedef enum {
    NAND_CMD_TYPE_CMD_WRITE,
    NAND_CMD_TYPE_ADDR_WRITE,
    NAND_CMD_TYPE_ADDR_COLUMN_WRITE,
    NAND_CMD_TYPE_ADDR_ROW_WRITE,
    NAND_CMD_TYPE_ADDR_SINGLE_WRITE,
    NAND_CMD_TYPE_RAW_WRITE,
    NAND_CMD_TYPE_RAW_READ
//    NAND_CMD_TYPE_USE_ORIGINAL,
//    NAND_CMD_TYPE_USE_OVERRIDE,
//    NAND_CMD_TYPE_DONT_USE
} nand_cmd_type_t;

typedef struct _nand_cmd_timings_t        nand_cmd_timings_t;
typedef struct _nand_raw_t                nand_raw_t;
typedef union  _nand_cmd_cycles_t         nand_cmd_cycles_t;
typedef struct _nand_cmd_chain_t          nand_cmd_chain_t;
typedef struct _nand_cmd_t                nand_cmd_t;
typedef struct _nand_cmd_params_t         nand_cmd_params_t;
typedef void (*nand_hook_cb_t)(nand_t* const nand, const nand_cmd_t* const cmd, nand_cmd_params_t* const cmd_params, const size_t current_chain_seq, nand_cmd_chain_t* current_chain);

struct _nand_cmd_timings_t {
    uint32_t                    pre_delay_ns;
    uint32_t                    latch_enable_pre_delay_ns;          // command: tCLH, address: tALH
    uint32_t                    latch_enable_post_delay_ns;         // command: tCLS, address: tALS
    uint32_t                    ready_this_lun_timeout_ns;
    uint32_t                    ready_other_luns_timeout_ns;
    uint32_t                    ready_post_delay_ns;                // tRR
    uint32_t                    cycle_rw_enable_post_delay_ns;      // read: tREA
    uint32_t                    cycle_rw_disable_post_delay_ns;     // read: tREH, write: tWH
    uint32_t                    latch_disable_pre_delay_ns;         // command: tCLH, address: tALH
    uint32_t                    latch_disable_post_delay_ns;        // command: tCLS, address: tALS
    uint32_t                    post_delay_ns;                      // command (with or without address) finished: tWB
};

struct _nand_raw_t {
    size_t                      raw_size;                           // Zero-able
    uint8_t*                    buffer;                             // Nullable
    size_t                      buffer_size;                        // Zero-able
    size_t                      current_buffer_seq;
    size_t                      current_raw_offset;
};

union _nand_cmd_cycles_t {
    uint8_t                     cmd;
    uint64_t                    addr[NAND_ADDR_INDEX_ALL];
    uint64_t                    addr_column;
    uint64_t                    addr_row;
    uint16_t                    addr_single;
    nand_raw_t*                 raw;                                // Nullable
};

struct _nand_cmd_chain_t {
    bool                        cycles_defined;
    nand_cmd_timings_t          timings;                            // Zero-able = NAND_CMD_TIMING_IGNORE
    nand_cmd_type_t             cycles_type;
    nand_cmd_cycles_t           cycles;
};

struct _nand_cmd_t {
    nand_hook_cb_t              pre_hook_cb;                        // Nullable
    nand_hook_cb_t              post_hook_cb;                       // Nullable
    size_t                      chains_length;
    nand_cmd_chain_t            chains[NAND_MAX_COMMAND_CYCLE_SIZE];
};

struct _nand_cmd_params_t {
    uint8_t                     lun_no;
    nand_cmd_t*                 cmd_override;                       // Nullable
};

size_t nand_run_cmd_chains(nand_t* const nand, const nand_cmd_t* const cmd, nand_cmd_params_t* const cmd_params, nand_rw_response_t* const err);

size_t nand_cmd_base_cmdw_addrsgw_rawsgr(nand_t* const nand, const uint8_t this_lun_no, const nand_cmd_t* const cmd, uint8_t* const buffer, const size_t buffer_size);
size_t nand_cmd_read_id(nand_t* const nand, const uint8_t this_lun_no, const nand_cmd_t* const id_cmd, uint8_t* const bytes_id, const size_t bytes_id_max_size);
size_t nand_cmd_read_parameter_page(nand_t* const nand, const uint8_t this_lun_no, const nand_cmd_t* const pp_cmd, uint8_t* const bytes_pp, const size_t bytes_pp_max_size);

size_t nand_extract_id(uint8_t* const bytes_id, const size_t bytes_id_size);
size_t nand_extract_id_size(const uint8_t * const bytes_id, const size_t bytes_id_size, const size_t min_pattern_size);

#ifdef __cplusplus
}
#endif

#endif /* NAND_CMD_H */
/** @} */
