#ifndef NAND_ONFI_TYPES_H
#define NAND_ONFI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "periph/gpio.h"

#include "nand/onfi/constants.h"
#include "nand/onfi/enums.h"

typedef struct _nand_onfi_cmd_timings_t         nand_onfi_cmd_timings_t;
typedef struct _nand_onfi_raw_t                 nand_onfi_raw_t;
typedef union  _nand_onfi_cmd_cycles_t          nand_onfi_cmd_cycles_t;
typedef struct _nand_onfi_cmd_chain_t           nand_onfi_cmd_chain_t;
typedef struct _nand_onfi_cmd_t                 nand_onfi_cmd_t;
typedef struct _nand_onfi_cmd_params_t          nand_onfi_cmd_params_t;
typedef struct _nand_onfi_params_t              nand_onfi_params_t;
typedef struct _nand_onfi_chip_t                nand_onfi_chip_t;
typedef struct _nand_onfi_prop_t                nand_onfi_prop_t;
typedef struct _nand_onfi_t                     nand_onfi_t;
typedef void (*nand_onfi_hook_cb_t)(nand_onfi_t* const nand, const nand_onfi_cmd_t* const cmd, nand_onfi_cmd_params_t* const cmd_params, const size_t current_chain_seq, nand_onfi_cmd_chain_t* current_chain);

struct _nand_onfi_cmd_timings_t {
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

struct _nand_onfi_raw_t {
    size_t                      raw_size;                           // Zero-able
    uint16_t*                   buffer;                             // Nullable
    size_t                      buffer_size;                        // Zero-able
    size_t                      current_buffer_seq;
    size_t                      current_raw_offset;
};

union _nand_onfi_cmd_cycles_t {
    uint8_t                     cmd;
    uint64_t                    addr[NAND_ONFI_ADDR_INDEX_ALL];
    uint64_t                    addr_column;
    uint64_t                    addr_row;
    uint16_t                    addr_single;
    nand_onfi_raw_t*            raw;                                // Nullable
};

struct _nand_onfi_cmd_chain_t {
    bool                        cycles_defined;
    nand_onfi_cmd_timings_t     timings;                            // Zero-able = NAND_ONFI_CMD_TIMING_IGNORE
    nand_onfi_cmd_type_t        cycles_type;
    nand_onfi_cmd_cycles_t      cycles;
};

struct _nand_onfi_cmd_t {
    nand_onfi_hook_cb_t         pre_hook_cb;                        // Nullable
    nand_onfi_hook_cb_t         post_hook_cb;                       // Nullable
    size_t                      chains_length;
    nand_onfi_cmd_chain_t       chains[NAND_ONFI_MAX_COMMAND_CYCLE_SIZE];
};

struct _nand_onfi_cmd_params_t {
    uint8_t                     lun_no;
    nand_onfi_cmd_t*            cmd_override;                       // Nullable
};

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

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_TYPES_H */