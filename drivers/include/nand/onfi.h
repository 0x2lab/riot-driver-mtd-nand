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

#include "nand.h"
#include "nand/onfi/timing.h"
#include "nand/onfi/cmd_timing.h"
#include "nand/onfi/cmd.h"

#define NAND_ONFI_MAX_UNIQUE_ID_SIZE             (512)
#define NAND_ONFI_PARAMETER_PAGE_SIZE            (768)          /**< ONFI states standard as 0-767 */

/**
 * @brief   version type of ONFI
 */
typedef enum {
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
} nand_onfi_version_t;

typedef struct __attribute__ ((packed)) {
    /* revision information and features block */
    /* 'O' 'N' 'F' 'I'  */
    uint8_t  sig[4];
    uint16_t revision;
    uint16_t features;
    uint16_t opt_cmd;
    uint8_t  jtg_cmd;                       /**< since ONFI (TODO) */
    uint8_t  training_cmd;                  /**< since ONFI (TODO) */
    uint16_t ext_param_page_length;         /**< since ONFI 2.1 */
    uint8_t  num_of_param_pages;            /**< since ONFI 2.1 */
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
    uint32_t data_bytes_per_ppage;          /**< reserved in ONFI 5.0 (TODO) */
    uint16_t spare_bytes_per_ppage;         /**< reserved in ONFI 5.0 (TODO) */
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
    uint8_t  ppage_attr;                    /**< reserved in ONFI 5.0 (TODO) */
    uint8_t  ecc_bits;
    uint8_t  interleaved_bits;
    uint8_t  interleaved_ops;
    uint8_t  reserved3;
    uint16_t nvddr3_timing_modes;           /**< since ONFI (TODO) */
    uint32_t nvlpddr4_timing_modes;         /**< since ONFI (TODO) */
    uint8_t  reserved4[6];

    /* electrical parameter block */
    uint8_t  io_pin_capacitance_max;        /**< reserved in ONFI 5.0 (TODO) */
    uint16_t sdr_timing_modes;
    uint16_t program_cache_timing_mode;     /**< reserved in ONFI 5.0 (TODO) */
    uint16_t t_prog;                        /**< reserved in ONFI 5.0 (TODO) */
    uint16_t t_bers;                        /**< reserved in ONFI 5.0 (TODO) */
    uint16_t t_r;                           /**< reserved in ONFI 5.0 (TODO) */
    uint16_t t_ccs;                         /**< reserved in ONFI 5.0 (TODO) */
    uint8_t  nvddr_timing_modes;
    uint8_t  nvddr2_timing_modes;
    uint8_t  nvddr_nvddr2_features;
    uint16_t clk_pin_capacitance_typ;       /**< reserved in ONFI 5.0 (TODO) */
    uint16_t io_pin_capacitance_typ;        /**< reserved in ONFI 5.0 (TODO) */
    uint16_t input_pin_capacitance_typ;     /**< reserved in ONFI 5.0 (TODO) */
    uint8_t  input_pin_capacitance_max;     /**< reserved in ONFI 5.0 (TODO) */
    uint8_t  driver_strength_support;
    uint16_t t_int_r;                       /**< reserved in ONFI 5.0 (TODO) */
    uint16_t t_adl;                         /**< reserved in ONFI 5.0 (TODO) */
    uint16_t reserved5;
    uint8_t  nvddr2_nvddr3_features;        /**< since ONFI (TODO) */
    uint8_t  nvddr2_nvddr3_warmup_cycles;   /**< since ONFI (TODO) */
    uint16_t nvddr3_timing_modes2;          /**< since ONFI (TODO) */
    uint8_t  nvddr2_timing_modes2;          /**< since ONFI (TODO) */
    uint8_t  reserved6;

    /* vendor */
    uint16_t vendor_revision;
    uint8_t  vendor[88];

    uint16_t crc;

    /* redundant */
    uint8_t  redundant1[256];
    uint8_t  redundant2[256];
} nand_onfi_chip_t;

typedef struct {
    nand_onfi_version_t version;            /**< ONFI version (BCD encoded), 0 if ONFI is not supported */
    uint16_t            t_prog;             /**< Page program time */
    uint16_t            t_bers;             /**< Block erase time */
    uint16_t            t_r;                /**< Page read time */
    uint16_t            t_ccs;              /**< Change column setup time */
    bool                fast_t_cad;         /**< Command/Address/Data slow or fast delay (NV-DDR only) */
    uint16_t            sdr_timing_modes;   /**< Supported asynchronous/SDR timing modes */
    uint16_t            nvddr_timing_modes; /**< Supported source synchronous/NV-DDR timing modes */
} nand_onfi_prop_t;

typedef struct {
    nand_t              nand;
    nand_onfi_chip_t    onfi_chip;
    nand_onfi_prop_t    onfi_prop;
} nand_onfi_t;

int nand_onfi_init(nand_onfi_t* const nand_onfi, nand_params_t* const params);
size_t nand_onfi_run_cmd(nand_onfi_t* const nand_onfi, const nand_cmd_t* const cmd, nand_cmd_params_t* const cmd_params, nand_rw_response_t* const err);
size_t nand_onfi_template_cmdw_addrsgw_rawsgr(nand_onfi_t* const nand_onfi, const uint8_t this_lun_no, const nand_cmd_t* const cmd, uint8_t* const buffer, const size_t buffer_size);
size_t nand_onfi_read_id(nand_onfi_t* const nand_onfi, const uint8_t this_lun_no, const nand_cmd_t* const id_cmd, uint8_t* const bytes_id, const size_t bytes_id_max_size);
size_t nand_onfi_read_parameter_page(nand_onfi_t* const nand_onfi, const uint8_t this_lun_no, const nand_cmd_t* const pp_cmd, uint8_t* const bytes_pp, const size_t bytes_pp_max_size);

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_H */
/** @} */
