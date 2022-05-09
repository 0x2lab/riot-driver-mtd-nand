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

#if defined(ENABLE_DEBUG) && ((ENABLE_DEBUG) <= 1)
#  ifndef DEBUG_PRINT_NOOP
#    define DEBUG_PRINT_NOOP(...) do {} while (0)
#  endif
#  ifdef DEBUG
#    undef DEBUG
#  endif
#  define DEBUG(...) do { if (ENABLE_DEBUG) { DEBUG_PRINT_NOOP(__VA_ARGS__); } } while (0)
#endif

#include "nand_onfi.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

int nand_onfi_init(nand_onfi_t* const nand, nand_onfi_params_t* const params) {
    DEBUG_PUTS("nand_onfi_init: entry");
    DEBUG("nand_onfi_init: nand:%p params:%p\n", nand, params);

    if(nand == NULL) {
        DEBUG("nand_onfi_init: return:%d\n", NAND_ONFI_INIT_ERROR);
        DEBUG_PUTS("nand_onfi_init: end");
        return NAND_ONFI_INIT_ERROR;
    }

    if(params == NULL) {
        nand->init_done = false;
        DEBUG("nand_onfi_init: return:%d\n", NAND_ONFI_INIT_ERROR);
        DEBUG_PUTS("nand_onfi_init: end");
        return NAND_ONFI_INIT_ERROR;
    }

    nand->params = *params;
    DEBUG("nand_onfi_init: params.ce0:%lu\n",  params->ce0);
    DEBUG("nand_onfi_init: params.ce1:%lu\n",  params->ce1);
    DEBUG("nand_onfi_init: params.ce2:%lu\n",  params->ce2);
    DEBUG("nand_onfi_init: params.ce3:%lu\n",  params->ce3);
    DEBUG("nand_onfi_init: params.rb0:%lu\n",  params->rb0);
    DEBUG("nand_onfi_init: params.rb1:%lu\n",  params->rb1);
    DEBUG("nand_onfi_init: params.rb2:%lu\n",  params->rb2);
    DEBUG("nand_onfi_init: params.rb3:%lu\n",  params->rb3);
    DEBUG("nand_onfi_init: params.re:%lu\n",   params->re);
    DEBUG("nand_onfi_init: params.we:%lu\n",   params->we);
    DEBUG("nand_onfi_init: params.wp:%lu\n",   params->wp);
    DEBUG("nand_onfi_init: params.cle:%lu\n",  params->cle);
    DEBUG("nand_onfi_init: params.ale:%lu\n",  params->ale);
    DEBUG("nand_onfi_init: params.io0:%lu\n",  params->io0);
    DEBUG("nand_onfi_init: params.io1:%lu\n",  params->io1);
    DEBUG("nand_onfi_init: params.io2:%lu\n",  params->io2);
    DEBUG("nand_onfi_init: params.io3:%lu\n",  params->io3);
    DEBUG("nand_onfi_init: params.io4:%lu\n",  params->io4);
    DEBUG("nand_onfi_init: params.io5:%lu\n",  params->io5);
    DEBUG("nand_onfi_init: params.io6:%lu\n",  params->io6);
    DEBUG("nand_onfi_init: params.io7:%lu\n",  params->io7);
    DEBUG("nand_onfi_init: params.io8:%lu\n",  params->io8);
    DEBUG("nand_onfi_init: params.io9:%lu\n",  params->io9);
    DEBUG("nand_onfi_init: params.io10:%lu\n", params->io10);
    DEBUG("nand_onfi_init: params.io11:%lu\n", params->io11);
    DEBUG("nand_onfi_init: params.io12:%lu\n", params->io12);
    DEBUG("nand_onfi_init: params.io13:%lu\n", params->io13);
    DEBUG("nand_onfi_init: params.io14:%lu\n", params->io14);
    DEBUG("nand_onfi_init: params.io15:%lu\n", params->io15);

    nand_onfi_set_pin_default(nand);

    nand->data_bus_width = 8;
    nand->data_bytes_per_page = 2048;
    nand->spare_bytes_per_page = 64;
    nand->pages_per_block = 64;
    nand->blocks_per_lun = 8192;
    nand->lun_count = 1;
    nand->bb_per_lun = 0;
    nand->column_addr_cycles = 2;
    nand->row_addr_cycles = 3;
    nand->bits_per_cell = 2;
    nand->programs_per_page = 3;
    DEBUG("nand_onfi_init: nand.data_bus_width:%hu\n",      nand->data_bus_width);
    DEBUG("nand_onfi_init: nand.data_bytes_per_page:%"      PRIu32  "\n", nand->data_bytes_per_page);
    DEBUG("nand_onfi_init: nand.spare_bytes_per_page:%"     PRIu16  "\n", nand->spare_bytes_per_page);
    DEBUG("nand_onfi_init: nand.pages_per_block:%"          PRIu32  "\n", nand->pages_per_block);
    DEBUG("nand_onfi_init: nand.blocks_per_lun:%"           PRIu32  "\n", nand->blocks_per_lun);
    DEBUG("nand_onfi_init: nand.lun_count:%hu\n",           nand->lun_count);
    DEBUG("nand_onfi_init: nand.bb_per_lun:%"               PRIu16  "\n", nand->bb_per_lun);
    DEBUG("nand_onfi_init: nand.column_addr_cycles:%hu\n",  nand->column_addr_cycles);
    DEBUG("nand_onfi_init: nand.row_addr_cycles:%hu\n",     nand->row_addr_cycles);
    DEBUG("nand_onfi_init: nand.bits_per_cell:%hu\n",       nand->bits_per_cell);
    DEBUG("nand_onfi_init: nand.programs_per_page:%hu\n",   nand->programs_per_page);

    nand->init_done = true;
    DEBUG("nand_onfi_init: nand.init_done:%s\n", nand->init_done ? "true" : "false");

    DEBUG("nand_onfi_init: return:%d\n", NAND_ONFI_OK);
    DEBUG_PUTS("nand_onfi_init: end");
    return NAND_ONFI_OK;
}

size_t nand_onfi_run_cmd(nand_onfi_t* const nand, const nand_onfi_cmd_t cmd, nand_onfi_run_cmd_params_t cmd_params[], nand_onfi_rw_response_t* const err) {
    DEBUG_PUTS("nand_onfi_run_cmd: entry");
    DEBUG("nand_onfi_run_cmd: nand:%p cmd.length:%u cmd.cmd_data:%p cmd_params:%p err:%p\n", nand, cmd.length, cmd.cmd_data, cmd_params, err);

    size_t ret_size = 0;

    for(size_t seq = 0; seq < cmd.length; ++seq) {
        DEBUG("nand_onfi_run_cmd: seq:%u\n", seq);

        const nand_onfi_cmd_data_t*       const cmd_data                    = &(cmd.cmd_data[seq]);
        const nand_onfi_cmd_type_t*       const type                        = &(cmd_data->type);
        const nand_onfi_cmd_rw_t*         const rw                          = &(cmd_data->rw);
              nand_onfi_run_cmd_params_t* const cmd_param                   = &(cmd_params[seq]);
        const uint8_t*                    const lun                         = &(cmd_param->lun);
              nand_onfi_hook_cb_t         const pre_hook_cb                 =   cmd_param->pre_hook_cb;
              nand_onfi_cmd_cycle_t*      const cycles                      = &(cmd_param->cycles);
              nand_onfi_hook_cb_t         const post_hook_cb                =   cmd_param->post_hook_cb;

        DEBUG("nand_onfi_run_cmd: cmd_data:%p\n", cmd_data);
        DEBUG("nand_onfi_run_cmd: type:%p *type:%d\n", type, *type);
        DEBUG("nand_onfi_run_cmd: rw:%p *rw:%d\n", rw, *rw);
        DEBUG("nand_onfi_run_cmd: cmd_param:%p\n", cmd_param);
        DEBUG("nand_onfi_run_cmd: lun:%p *lun:%hu\n", lun, *lun);
        DEBUG("nand_onfi_run_cmd: pre_hook_cb:%p cycles:%p post_hook_cb:%p\n", pre_hook_cb, cycles, post_hook_cb);

        nand_onfi_wait_cmd_data(cmd_data);

        nand_onfi_set_chip_enable(nand, *lun);

        switch(*type) {
        case NAND_ONFI_CMD_TYPE_CMD:;
            DEBUG_PUTS("nand_onfi_run_cmd: cmd (command)");

            const uint8_t* const cmd = &(cmd_data->cycles.cmd);
            DEBUG("nand_onfi_run_cmd: cmd: cmd:%p *cmd:%02x\n", cmd, *cmd);

            if(*rw == NAND_ONFI_CMD_RW_WRITE) {
                DEBUG_PUTS("nand_onfi_run_cmd: cmd-w (command - write)");

                if(pre_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: pre_hook_cb: entry");
                    pre_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: pre_hook_cb: end");
                }

                nand_onfi_set_latch_command(nand);

                if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                    nand_onfi_set_latch_raw(nand);
                    *err = NAND_ONFI_RW_TIMEOUT;
                    DEBUG("nand_onfi_run_cmd: cmd-w: *err:%d\n", *err);
                    DEBUG("nand_onfi_run_cmd: cmd-w: return:%u\n", ret_size);
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: end");
                    return ret_size;
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_cmd(nand, cmd);

                nand_onfi_set_latch_raw(nand);

                if(post_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: post_hook_cb: entry");
                    post_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: post_hook_cb: end");
                }

                DEBUG_PUTS("nand_onfi_run_cmd: cmd-w: end");
            }

            DEBUG_PUTS("nand_onfi_run_cmd: cmd: end");
            break;

        case NAND_ONFI_CMD_TYPE_ADDR:;
            DEBUG_PUTS("nand_onfi_run_cmd: addr (address)");

            const uint32_t* const addr = cycles->addr;
            DEBUG("nand_onfi_run_cmd: addr: addr:%p *addr:%" PRIx32 "\n", addr, *addr);

            if(*rw == NAND_ONFI_CMD_RW_WRITE) {
                DEBUG_PUTS("nand_onfi_run_cmd: addr-w (address - write)");

                if(pre_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: addr-w: pre_hook_cb: entry");
                    pre_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: addr-w: pre_hook_cb: end");
                }

                nand_onfi_set_latch_address(nand);

                if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                    nand_onfi_set_latch_raw(nand);
                    *err = NAND_ONFI_RW_TIMEOUT;
                    DEBUG("nand_onfi_run_cmd: addr-w: *err:%d\n", *err);
                    DEBUG("nand_onfi_run_cmd: addr-w: return:%u\n", ret_size);
                    DEBUG_PUTS("nand_onfi_run_cmd: addr-w: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: addr: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: end");
                    return ret_size;
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr(nand, addr);

                nand_onfi_set_latch_raw(nand);

                if(post_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: addr-w: post_hook_cb: entry");
                    post_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: addr-w: post_hook_cb: end");
                }

                DEBUG_PUTS("nand_onfi_run_cmd: addr-w: end");
            }

            DEBUG_PUTS("nand_onfi_run_cmd: addr: end");
            break;

        case NAND_ONFI_CMD_TYPE_ADDR_COLUMN:;
            DEBUG_PUTS("nand_onfi_run_cmd: acol (column address)");

            const uint32_t* const addr_column = &(cycles->addr_column);
            DEBUG("nand_onfi_run_cmd: acol: addr_column:%p *addr_column:%" PRIx32 "\n", addr_column, *addr_column);

            if(*rw == NAND_ONFI_CMD_RW_WRITE) {
                DEBUG_PUTS("nand_onfi_run_cmd: acol-w (column address - write)");

                if(pre_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: acol-w: pre_hook_cb: entry");
                    pre_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: acol-w: pre_hook_cb: end");
                }

                nand_onfi_set_latch_address(nand);

                if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                    nand_onfi_set_latch_raw(nand);
                    *err = NAND_ONFI_RW_TIMEOUT;
                    DEBUG("nand_onfi_run_cmd: acol-w: *err:%d\n", *err);
                    DEBUG("nand_onfi_run_cmd: acol-w: return:%u\n", ret_size);
                    DEBUG_PUTS("nand_onfi_run_cmd: acol-w: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: acol: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: end");
                    return ret_size;
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr_column(nand, addr_column);

                nand_onfi_set_latch_raw(nand);

                if(post_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: acol-w: post_hook_cb: entry");
                    post_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: acol-w: post_hook_cb: end");
                }

                DEBUG_PUTS("nand_onfi_run_cmd: acol-w: end");
            }

            DEBUG_PUTS("nand_onfi_run_cmd: acol: end");
            break;

        case NAND_ONFI_CMD_TYPE_ADDR_ROW:;
            DEBUG_PUTS("nand_onfi_run_cmd: arow (row address)");

            const uint32_t* const addr_row = &(cycles->addr_row);
            DEBUG("nand_onfi_run_cmd: arow: addr_row:%p *addr_row:%" PRIx32 "\n", addr_row, *addr_row);

            if(*rw == NAND_ONFI_CMD_RW_WRITE) {
                DEBUG_PUTS("nand_onfi_run_cmd: arow-w (row address - write)");

                if(pre_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: arow-w: pre_hook_cb: entry");
                    pre_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: arow-w: pre_hook_cb: end");
                }

                nand_onfi_set_latch_address(nand);

                if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                    nand_onfi_set_latch_raw(nand);
                    *err = NAND_ONFI_RW_TIMEOUT;
                    DEBUG("nand_onfi_run_cmd: arow-w: *err:%d\n", *err);
                    DEBUG("nand_onfi_run_cmd: arow-w: return:%u\n", ret_size);
                    DEBUG_PUTS("nand_onfi_run_cmd: arow-w: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: arow: end");
                    DEBUG_PUTS("nand_onfi_run_cmd: end");
                    return ret_size;
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr_row(nand, addr_row);

                nand_onfi_set_latch_raw(nand);

                if(post_hook_cb != NULL) {
                    DEBUG_PUTS("nand_onfi_run_cmd: arow-w: post_hook_cb: entry");
                    post_hook_cb(nand, cmd_data, cmd_param);
                    DEBUG_PUTS("nand_onfi_run_cmd: arow-w: post_hook_cb: end");
                }

                DEBUG_PUTS("nand_onfi_run_cmd: arow-w: end");
            }

            DEBUG_PUTS("nand_onfi_run_cmd: arow: end");
            break;

        case NAND_ONFI_CMD_TYPE_RAW:;
            DEBUG_PUTS("nand_onfi_run_cmd: raw (raw)");

            const size_t                 intended_raw_size  =   cmd_data->raw_size;
                  nand_onfi_raw_t* const raw                = &(cycles->raw);
                  size_t*          const raw_size           = &(raw->raw_size);
                  size_t*          const raw_offset         = &(raw->raw_offset);
                  uint16_t*        const buffer             =   raw->buffer;
                  size_t                 buffer_size        =   raw->buffer_size;
                  size_t*          const buffer_seq         = &(raw->buffer_seq);
            DEBUG("nand_onfi_run_cmd: raw: intended_raw_size:%u\n",     intended_raw_size);
            DEBUG("nand_onfi_run_cmd: raw: raw:%p\n",                   raw);
            DEBUG("nand_onfi_run_cmd: raw: raw_size:%p *raw_size:%u\n", raw_size,   *raw_size);
            DEBUG("nand_onfi_run_cmd: raw: raw_offset:%p\n",            raw_offset);
            DEBUG("nand_onfi_run_cmd: raw: buffer:%p\n",                buffer);
            DEBUG("nand_onfi_run_cmd: raw: buffer_size:%u\n",           buffer_size);
            DEBUG("nand_onfi_run_cmd: raw: buffer_seq:%p\n",            buffer_seq);

            if(intended_raw_size != 0 && *raw_size == 0) {
                *raw_size = intended_raw_size;
                DEBUG("nand_onfi_run_cmd: raw: use *raw_size:%u as an intended raw size\n", *raw_size);
            }

            if(*raw_size == (size_t)NAND_ONFI_PAGE_SIZE_DEPENDANT_CYCLES) {
                *raw_size = nand_onfi_one_page_size(nand);
                DEBUG("nand_onfi_run_cmd: raw: use *raw_size:%u as a page size\n", *raw_size);
            }

            *raw_offset = 0;
            *buffer_seq = 0;
            DEBUG("nand_onfi_run_cmd: raw: init *raw_offset:%u and *buffer_seq:%u\n", *raw_offset, *buffer_seq);

            if(*rw == NAND_ONFI_CMD_RW_WRITE) {
                DEBUG_PUTS("nand_onfi_run_cmd: raw-w (raw - write)");

                while(buffer_size > 0 && *raw_offset < *raw_size) {
                    DEBUG("nand_onfi_run_cmd: raw-w[%u]: *raw_offset:%u (before)\n", *buffer_seq, *raw_offset);

                    if(pre_hook_cb != NULL) {
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-w[]: pre_hook_cb: entry");
                        pre_hook_cb(nand, cmd_data, cmd_param);
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-w[]: pre_hook_cb: end");

                        if(*raw_offset >= *raw_size) {
                            DEBUG_PUTS("nand_onfi_run_cmd: raw-w[]: write stop");
                            DEBUG("nand_onfi_run_cmd: raw-w[%u]: raw_offset:%u reached to *raw_size:%u\n", *buffer_seq, *raw_offset, *raw_size);
                            break;
                        }
                    }

                    if(buffer != NULL) {
                        const size_t raw_remaining_size = *raw_size - *raw_offset;
                        DEBUG("nand_onfi_run_cmd: raw-w[%u]: raw_remaining_size:%u\n", *buffer_seq, raw_remaining_size);

                        buffer_size = (raw_remaining_size > buffer_size) ? buffer_size : raw_remaining_size; /**< Only touch locally, instead of touch the passed param */
                        DEBUG("nand_onfi_run_cmd: raw-w[%u]: buffer_size:%u\n", *buffer_seq, buffer_size);

                        nand_onfi_set_latch_raw(nand);

                        if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                            *err = NAND_ONFI_RW_TIMEOUT;
                            DEBUG("nand_onfi_run_cmd: raw-w[%u]: *err:%d\n", *buffer_seq, *err);
                            DEBUG("nand_onfi_run_cmd: raw-w[%u]: return:%u\n", *buffer_seq, ret_size);
                            DEBUG_PUTS("nand_onfi_run_cmd: raw-w: end");
                            DEBUG_PUTS("nand_onfi_run_cmd: raw: end");
                            DEBUG_PUTS("nand_onfi_run_cmd: end");
                            return ret_size;
                        }

                        nand_onfi_set_io_pin_write(nand);
                        for(size_t buffer_offset = 0; buffer_offset < buffer_size; ++buffer_offset) {
                            DEBUG("nand_onfi_run_cmd: raw-w[%u]: buffer_offset:%u\n", *buffer_seq, buffer_offset);
                            ret_size += nand_onfi_write_cycle(nand, &buffer[buffer_offset]);
                        }
                    }

                    *raw_offset += buffer_size;
                    DEBUG("nand_onfi_run_cmd: raw-w[%u]: *raw_offset:%u (after)\n", *buffer_seq, *raw_offset);

                    if(post_hook_cb != NULL) {
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-w[]: post_hook_cb: entry");
                        post_hook_cb(nand, cmd_data, cmd_param);
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-w[]: post_hook_cb: end");
                    }

                    ++(*buffer_seq);
                }

                DEBUG_PUTS("nand_onfi_run_cmd: raw-w: end");
            } else {
                DEBUG_PUTS("nand_onfi_run_cmd: raw-r (raw - read)");

                while(buffer_size > 0 && *raw_offset < *raw_size) {
                    DEBUG("nand_onfi_run_cmd: raw-r[%u]: *raw_offset:%u (before)\n", *buffer_seq, *raw_offset);

                    if(pre_hook_cb != NULL) {
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-r[]: pre_hook_cb: entry");
                        pre_hook_cb(nand, cmd_data, cmd_param);
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-r[]: pre_hook_cb: end");

                        if(*raw_offset >= *raw_size) {
                            DEBUG_PUTS("nand_onfi_run_cmd: raw-r[]: read stop");
                            DEBUG("nand_onfi_run_cmd: raw-r[%u]: raw_offset:%u reached to *raw_size:%u\n", *buffer_seq, *raw_offset, *raw_size);
                            break;
                        }
                    }

                    if(buffer != NULL) {
                        const size_t raw_remaining_size = *raw_size - *raw_offset;
                        DEBUG("nand_onfi_run_cmd: raw-r[%u]: raw_remaining_size:%u\n", *buffer_seq, raw_remaining_size);

                        buffer_size = (raw_remaining_size > buffer_size) ? buffer_size : raw_remaining_size; /**< Only touch locally, instead of touch the passed param */
                        DEBUG("nand_onfi_run_cmd: raw-r[%u]: buffer_size:%u\n", *buffer_seq, buffer_size);

                        nand_onfi_set_latch_raw(nand);

                        if(! nand_onfi_wait_until_ready(nand, cmd_data, *lun)) {
                            *err = NAND_ONFI_RW_TIMEOUT;
                            DEBUG("nand_onfi_run_cmd: raw-r[%u]: *err:%d\n", *buffer_seq, *err);
                            DEBUG("nand_onfi_run_cmd: raw-r[%u]: return:%u\n", *buffer_seq, ret_size);
                            DEBUG_PUTS("nand_onfi_run_cmd: raw-r: end");
                            DEBUG_PUTS("nand_onfi_run_cmd: raw: end");
                            DEBUG_PUTS("nand_onfi_run_cmd: end");
                            return ret_size;
                        }

                        nand_onfi_set_io_pin_read(nand);
                        for(size_t buffer_offset = 0; buffer_offset < buffer_size; ++buffer_offset) {
                            DEBUG("nand_onfi_run_cmd: raw-r[%u]: buffer_offset:%u\n", *buffer_seq, buffer_offset);
                            ret_size += nand_onfi_read_cycle(nand, &buffer[buffer_offset]);
                        }

                        if(cycles->raw.buffer_size != buffer_size) {
                            cycles->raw.buffer_size = buffer_size; /**< Touch the passed param */
                        }

                    }

                    *raw_offset += buffer_size;
                    DEBUG("nand_onfi_run_cmd: raw-r[%u]: *raw_offset:%u (after)\n", *buffer_seq, *raw_offset);

                    if(post_hook_cb != NULL) {
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-r[]: post_hook_cb: entry");
                        post_hook_cb(nand, cmd_data, cmd_param);
                        DEBUG_PUTS("nand_onfi_run_cmd: raw-r[]: post_hook_cb: end");
                    }

                    ++(*buffer_seq);
                }

                DEBUG_PUTS("nand_onfi_run_cmd: raw-r: end");
            }

            DEBUG_PUTS("nand_onfi_run_cmd: raw: end");
            break;
        }

        nand_onfi_set_chip_disable(nand, *lun);
    }

    *err = NAND_ONFI_RW_OK;
    DEBUG("nand_onfi_run_cmd: *err:%d\n", *err);
    DEBUG("nand_onfi_run_cmd: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_run_cmd: end");
    return ret_size;
}

size_t nand_onfi_write_cmd(const nand_onfi_t* const nand, const uint8_t* const cmd) {
    DEBUG_PUTS("nand_onfi_write_cmd: entry");
    DEBUG("nand_onfi_write_cmd: nand:%p cmd:%p\n", nand, cmd);
    DEBUG("nand_onfi_write_cmd: *cmd:%02x\n", *cmd);

    uint16_t cmd_aligned = (uint16_t)(*cmd);
    DEBUG("nand_onfi_write_cmd: cmd_aligned:%04" PRIx16 "\n", cmd_aligned);
    const size_t ret_size = nand_onfi_write_cycle(nand, &cmd_aligned); 

    DEBUG("nand_onfi_write_cmd: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_cmd: end");
    return ret_size;
}

size_t nand_onfi_write_addr(const nand_onfi_t* const nand, const uint32_t addr[]) {
    DEBUG_PUTS("nand_onfi_write_addr: entry");
    DEBUG("nand_onfi_write_addr: nand:%p addr:%p\n", nand, addr);

    size_t ret_size = 0;
    ret_size += nand_onfi_write_addr_column(nand, &(addr[NAND_ONFI_ADDR_INDEX_COLUMN]));
    ret_size += nand_onfi_write_addr_row(nand, &(addr[NAND_ONFI_ADDR_INDEX_ROW]));

    DEBUG("nand_onfi_write_addr: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_addr: end");
    return ret_size;
}

size_t nand_onfi_write_addr_column(const nand_onfi_t* const nand, const uint32_t* const addr_column) {
    DEBUG_PUTS("nand_onfi_write_addr_column: entry");
    DEBUG("nand_onfi_write_addr_column: nand:%p addr_column:%p\n", nand, addr_column);
    DEBUG("nand_onfi_write_addr_column: *addr_column:%" PRIx32 "\n", *addr_column);

    size_t ret_size = 0;

    /*** TODO: Flexible address size */
    static const uint32_t mask_column_1 = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1);
    static const uint32_t mask_column_2 = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1) << NAND_ONFI_ADDR_IO_BITS;

    uint16_t cycles[NAND_ONFI_ADDR_COLUMN_CYCLES] = {};
    cycles[0] = (*addr_column & mask_column_1);
    cycles[1] = (*addr_column & mask_column_2) >> NAND_ONFI_ADDR_IO_BITS;

    ret_size += nand_onfi_write_cycle(nand, &(cycles[0]));
    ret_size += nand_onfi_write_cycle(nand, &(cycles[1]));

    DEBUG("nand_onfi_write_addr_column: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_addr_column: end");
    return ret_size;
}

size_t nand_onfi_write_addr_row(const nand_onfi_t* const nand, const uint32_t* const addr_row) {
    DEBUG_PUTS("nand_onfi_write_addr_row: entry");
    DEBUG("nand_onfi_write_addr_row: nand:%p addr_row:%p\n", nand, addr_row);
    DEBUG("nand_onfi_write_addr_row: *addr_row:%" PRIx32 "\n", *addr_row);

    size_t ret_size = 0;

    /*** TODO: Flexible address size */
    static const uint32_t mask_row_1    = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1);
    static const uint32_t mask_row_2    = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1) <<  NAND_ONFI_ADDR_IO_BITS;
    static const uint32_t mask_row_3    = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1) << (NAND_ONFI_ADDR_IO_BITS << 1);

    uint16_t cycles[NAND_ONFI_ADDR_ROW_CYCLES] = {};
    cycles[0] = (*addr_row & mask_row_1);
    cycles[1] = (*addr_row & mask_row_2) >>  8;
    cycles[2] = (*addr_row & mask_row_3) >> (NAND_ONFI_ADDR_IO_BITS << 1);

    ret_size += nand_onfi_write_cycle(nand, &(cycles[0]));
    ret_size += nand_onfi_write_cycle(nand, &(cycles[1]));
    ret_size += nand_onfi_write_cycle(nand, &(cycles[2]));

    DEBUG("nand_onfi_write_addr_row: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_addr_row: end");
    return ret_size;
}

size_t nand_onfi_write_raw(const nand_onfi_t* const nand, const uint16_t* const raw_data, const size_t raw_size) {
    DEBUG_PUTS("nand_onfi_write_raw: entry");
    DEBUG("nand_onfi_write_raw: nand:%p raw_data:%p raw_size:%u\n", nand, raw_data, raw_size);

    size_t ret_size = 0;

    for(size_t seq = 0; seq < raw_size; ++seq) {
        DEBUG("nand_onfi_write_raw: &(raw_data[%u]):%p\n", seq, &(raw_data[seq]));
        ret_size += nand_onfi_write_cycle(nand, &(raw_data[seq]));
    }

    DEBUG("nand_onfi_write_raw: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_raw: end");
    return ret_size;
}

size_t nand_onfi_write_cycle(const nand_onfi_t* const nand, const uint16_t* const cycle_data) {
    DEBUG_PUTS("nand_onfi_write_cycle: entry");
    DEBUG("nand_onfi_write_cycle: nand:%p cycle_data:%p\n", nand, cycle_data);

    const size_t ret_size = nand_onfi_write_io(nand, cycle_data);

    DEBUG("nand_onfi_write_cycle: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_write_cycle: end");
    return ret_size;
}

size_t nand_onfi_write_io(const nand_onfi_t* const nand, const uint16_t* const data) {
    DEBUG_PUTS("nand_onfi_write_io: entry");
    DEBUG("nand_onfi_write_io: nand:%p data:%p\n", nand, data);
    DEBUG("nand_onfi_write_io: nand.data_bus_width:%hu\n", nand->data_bus_width);
    DEBUG("nand_onfi_write_io: *data:%04" PRIx16 "\n", *data);

    nand_onfi_set_write_enable(nand);

    if(nand->data_bus_width == 16) {
        DEBUG_PUTS("nand_onfi_write_io: write to [io15-io8]");

        const int io15 = (*data & NAND_ONFI_MSB15) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io15:%d\n", io15);
        gpio_write(nand->params.io15, io15);

        const int io14 = (*data & NAND_ONFI_MSB14) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io14:%d\n", io14);
        gpio_write(nand->params.io14, io14);

        const int io13 = (*data & NAND_ONFI_MSB13) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io13:%d\n", io13);
        gpio_write(nand->params.io13, io13);

        const int io12 = (*data & NAND_ONFI_MSB12) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io12:%d\n", io12);
        gpio_write(nand->params.io12, io12);

        const int io11 = (*data & NAND_ONFI_MSB11) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io11:%d\n", io11);
        gpio_write(nand->params.io11, io11);

        const int io10 = (*data & NAND_ONFI_MSB10) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io10:%d\n", io10);
        gpio_write(nand->params.io10, io10);

        const int io9 = (*data & NAND_ONFI_MSB9) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io9:%d\n", io9);
        gpio_write(nand->params.io9, io9);

        const int io8 = (*data & NAND_ONFI_MSB8) ? 1 : 0;
        DEBUG("nand_onfi_write_io: io8:%d\n", io8);
        gpio_write(nand->params.io8, io8);
    }

    DEBUG_PUTS("nand_onfi_write_io: write to [io7-io0]");

    const int io7 = (*data & NAND_ONFI_MSB7) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io7:%d\n", io7);
    gpio_write(nand->params.io7, io7);

    const int io6 = (*data & NAND_ONFI_MSB6) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io6:%d\n", io6);
    gpio_write(nand->params.io6, io6);

    const int io5 = (*data & NAND_ONFI_MSB5) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io5:%d\n", io5);
    gpio_write(nand->params.io5, io5);

    const int io4 = (*data & NAND_ONFI_MSB4) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io4:%d\n", io4);
    gpio_write(nand->params.io4, io4);

    const int io3 = (*data & NAND_ONFI_MSB3) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io3:%d\n", io3);
    gpio_write(nand->params.io3, io3);

    const int io2 = (*data & NAND_ONFI_MSB2) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io2:%d\n", io2);
    gpio_write(nand->params.io2, io2);

    const int io1 = (*data & NAND_ONFI_MSB1) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io1:%d\n", io1);
    gpio_write(nand->params.io1, io1);

    const int io0 = (*data & NAND_ONFI_MSB0) ? 1 : 0;
    DEBUG("nand_onfi_write_io: io0:%d\n", io0);
    gpio_write(nand->params.io0, io0);

    nand_onfi_set_write_disable(nand);

    DEBUG_PUTS("nand_onfi_write_io: return:1");
    DEBUG_PUTS("nand_onfi_write_io: end");
    return 1;
}

size_t nand_onfi_read_cycle(const nand_onfi_t* const nand, uint16_t* const out_cycle_data) {
    DEBUG_PUTS("nand_onfi_read_cycle: entry");
    DEBUG("nand_onfi_read_cycle: nand:%p out_cycle_data:%p\n", nand, out_cycle_data);

    const size_t ret_size = nand_onfi_read_io(nand, out_cycle_data);

    DEBUG("nand_onfi_read_cycle: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_read_cycle: end");
    return ret_size;
}

size_t nand_onfi_read_io(const nand_onfi_t* const nand, uint16_t* const out_data) {
    DEBUG_PUTS("nand_onfi_read_io: entry");
    DEBUG("nand_onfi_read_io: nand:%p out_data:%p\n", nand, out_data);
    DEBUG("nand_onfi_read_io: nand.data_bus_width:%hu\n", nand->data_bus_width);
    *out_data = 0;

    nand_onfi_set_read_enable(nand);

    nand_onfi_wait(NAND_ONFI_TIMING_R);

    if(nand->data_bus_width == 16) {
        DEBUG_PUTS("nand_onfi_read_io: read from [io15-io8]");

        const bool io15 = gpio_read(nand->params.io15) != 0;
        DEBUG("nand_onfi_read_io: io15:%d\n", io15 ? 1 : 0);

        const bool io14 = gpio_read(nand->params.io14) != 0;
        DEBUG("nand_onfi_read_io: io14:%d\n", io14 ? 1 : 0);

        const bool io13 = gpio_read(nand->params.io13) != 0;
        DEBUG("nand_onfi_read_io: io13:%d\n", io13 ? 1 : 0);

        const bool io12 = gpio_read(nand->params.io12) != 0;
        DEBUG("nand_onfi_read_io: io12:%d\n", io12 ? 1 : 0);

        const bool io11 = gpio_read(nand->params.io11) != 0;
        DEBUG("nand_onfi_read_io: io11:%d\n", io11 ? 1 : 0);

        const bool io10 = gpio_read(nand->params.io10) != 0;
        DEBUG("nand_onfi_read_io: io10:%d\n", io10 ? 1 : 0);

        const bool io9 = gpio_read(nand->params.io9) != 0;
        DEBUG("nand_onfi_read_io: io9:%d\n", io9 ? 1 : 0);

        const bool io8 = gpio_read(nand->params.io8) != 0;
        DEBUG("nand_onfi_read_io: io8:%d\n", io8 ? 1 : 0);

        *out_data = (io15 << 15) | (io14 << 14) | (io13 << 13) | (io12 << 12) | (io11 << 11) | (io10 << 10) | (io9 << 9) | (io8 << 8);
    }

    DEBUG_PUTS("nand_onfi_read_io: read from [io7-io0]");

    const bool io7 = gpio_read(nand->params.io7) != 0;
    DEBUG("nand_onfi_read_io: io7:%d\n", io7 ? 1 : 0);

    const bool io6 = gpio_read(nand->params.io6) != 0;
    DEBUG("nand_onfi_read_io: io6:%d\n", io6 ? 1 : 0);

    const bool io5 = gpio_read(nand->params.io5) != 0;
    DEBUG("nand_onfi_read_io: io5:%d\n", io5 ? 1 : 0);

    const bool io4 = gpio_read(nand->params.io4) != 0;
    DEBUG("nand_onfi_read_io: io4:%d\n", io4 ? 1 : 0);

    const bool io3 = gpio_read(nand->params.io3) != 0;
    DEBUG("nand_onfi_read_io: io3:%d\n", io3 ? 1 : 0);

    const bool io2 = gpio_read(nand->params.io2) != 0;
    DEBUG("nand_onfi_read_io: io2:%d\n", io2 ? 1 : 0);

    const bool io1 = gpio_read(nand->params.io1) != 0;
    DEBUG("nand_onfi_read_io: io1:%d\n", io1 ? 1 : 0);

    const bool io0 = gpio_read(nand->params.io0) != 0;
    DEBUG("nand_onfi_read_io: io0:%d\n", io0 ? 1 : 0);

    *out_data = *out_data | (io7 << 7) | (io6 << 6) | (io5 << 5) | (io4 << 4) | (io3 << 3) | (io2 << 2) | (io1 << 1) | io0;

    nand_onfi_set_read_disable(nand);

    DEBUG("nand_onfi_read_io: *out_data:%04" PRIx16 "\n", *out_data);
    DEBUG_PUTS("nand_onfi_read_io: return:1");
    DEBUG_PUTS("nand_onfi_read_io: end");
    return 1;
}

void nand_onfi_set_pin_default(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_pin_default: entry");
    DEBUG("nand_onfi_set_pin_default: nand:%p\n", nand);

    nand_onfi_set_ctrl_pin(nand);
    nand_onfi_set_io_pin_write(nand);

    DEBUG_PUTS("nand_onfi_set_pin_default: end");
}

void nand_onfi_set_ctrl_pin(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_pin_ctrl_pin: entry");
    DEBUG("nand_onfi_set_ctrl_pin: nand:%p\n", nand);
    DEBUG("nand_onfi_set_ctrl_pin: nand.lun_count: %hu\n", nand->lun_count);

    DEBUG_PUTS("nand_onfi_set_ctrl_pin: set [ce0 rb0] to [out in]");
    gpio_init(nand->params.ce0, GPIO_OUT);
    gpio_init(nand->params.rb0, GPIO_IN);

    if(nand->lun_count > 0) {
        DEBUG_PUTS("nand_onfi_set_ctrl_pin: set [ce1 rb1] to [out in]");
        gpio_init(nand->params.ce1, GPIO_OUT);
        gpio_init(nand->params.rb1, GPIO_IN);
    }

    if(nand->lun_count > 1) {
        DEBUG_PUTS("nand_onfi_set_ctrl_pin: set [ce2 rb2] to [out in]");
        gpio_init(nand->params.ce2, GPIO_OUT);
        gpio_init(nand->params.rb2, GPIO_IN);
    }

    if(nand->lun_count > 2) {
        DEBUG_PUTS("nand_onfi_set_ctrl_pin: set [ce3 rb3] to [out in]");
        gpio_init(nand->params.ce3, GPIO_OUT);
        gpio_init(nand->params.rb3, GPIO_IN);
    }

    DEBUG_PUTS("nand_onfi_set_ctrl_pin: set [re we wp cle ale] to out");
    gpio_init(nand->params.re, GPIO_OUT);
    gpio_init(nand->params.we, GPIO_OUT);
    gpio_init(nand->params.wp, GPIO_OUT);
    gpio_init(nand->params.cle, GPIO_OUT);
    gpio_init(nand->params.ale, GPIO_OUT);

    DEBUG_PUTS("nand_onfi_set_ctrl_pin: end");
}

void nand_onfi_set_io_pin_write(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_io_pin_write: entry");
    DEBUG("nand_onfi_set_io_pin_write: nand:%p\n", nand);
    DEBUG("nand_onfi_set_io_pin_write: nand.data_bus_width:%hu\n", nand->data_bus_width);

    if(nand->data_bus_width == 16) {
        DEBUG_PUTS("nand_onfi_set_io_pin_write: set [io15-io8] to out");
        gpio_init(nand->params.io15, GPIO_OUT);
        gpio_init(nand->params.io14, GPIO_OUT);
        gpio_init(nand->params.io13, GPIO_OUT);
        gpio_init(nand->params.io12, GPIO_OUT);
        gpio_init(nand->params.io11, GPIO_OUT);
        gpio_init(nand->params.io10, GPIO_OUT);
        gpio_init(nand->params.io9, GPIO_OUT);
        gpio_init(nand->params.io8, GPIO_OUT);
    }

    DEBUG_PUTS("nand_onfi_set_io_pin_write: set [io7-io0] to out");
    gpio_init(nand->params.io7, GPIO_OUT);
    gpio_init(nand->params.io6, GPIO_OUT);
    gpio_init(nand->params.io5, GPIO_OUT);
    gpio_init(nand->params.io4, GPIO_OUT);
    gpio_init(nand->params.io3, GPIO_OUT);
    gpio_init(nand->params.io2, GPIO_OUT);
    gpio_init(nand->params.io1, GPIO_OUT);
    gpio_init(nand->params.io0, GPIO_OUT);

    DEBUG_PUTS("nand_onfi_set_io_pin_write: end");
}

void nand_onfi_set_io_pin_read(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_io_pin_read: entry");
    DEBUG("nand_onfi_set_io_pin_read: nand:%p\n", nand);
    DEBUG("nand_onfi_set_io_pin_read: nand.data_bus_width:%hu\n", nand->data_bus_width);

    if(nand->data_bus_width == 16) {
        DEBUG_PUTS("nand_onfi_set_io_pin_read: set [io15-io8] to in");
        gpio_init(nand->params.io15, GPIO_IN);
        gpio_init(nand->params.io14, GPIO_IN);
        gpio_init(nand->params.io13, GPIO_IN);
        gpio_init(nand->params.io12, GPIO_IN);
        gpio_init(nand->params.io11, GPIO_IN);
        gpio_init(nand->params.io10, GPIO_IN);
        gpio_init(nand->params.io9, GPIO_IN);
        gpio_init(nand->params.io8, GPIO_IN);
    }

    DEBUG_PUTS("nand_onfi_set_io_pin_read: set [io7-io0] to in");
    gpio_init(nand->params.io7, GPIO_IN);
    gpio_init(nand->params.io6, GPIO_IN);
    gpio_init(nand->params.io5, GPIO_IN);
    gpio_init(nand->params.io4, GPIO_IN);
    gpio_init(nand->params.io3, GPIO_IN);
    gpio_init(nand->params.io2, GPIO_IN);
    gpio_init(nand->params.io1, GPIO_IN);
    gpio_init(nand->params.io0, GPIO_IN);

    DEBUG_PUTS("nand_onfi_set_io_pin_read: end");
}

void nand_onfi_set_latch_command(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_latch_command: entry");
    DEBUG("nand_onfi_set_latch_command: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_latch_command: set [ale cle] to [0 1]");
    gpio_write(nand->params.ale, 0);
    gpio_write(nand->params.cle, 1);

    DEBUG_PUTS("nand_onfi_set_latch_command: end");
}

void nand_onfi_set_latch_address(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_latch_address: entry");
    DEBUG("nand_onfi_set_latch_address: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_latch_address: set [cle ale] to [0 1]");
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 1);

    DEBUG_PUTS("nand_onfi_set_latch_address: end");
}

void nand_onfi_set_latch_raw(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_latch_raw: entry");
    DEBUG("nand_onfi_set_latch_raw: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_latch_raw: set [cle ale] to [0 0]");
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 0);

    DEBUG_PUTS("nand_onfi_set_latch_raw: end");
}

void nand_onfi_set_read_enable(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_read_enable: entry");
    DEBUG("nand_onfi_set_read_enable: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_read_enable: set re to 0");
    gpio_write(nand->params.re, 0);

    DEBUG_PUTS("nand_onfi_set_read_enable: end");
}

void nand_onfi_set_read_disable(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_read_disable: entry");
    DEBUG("nand_onfi_set_read_disable: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_read_disable: set re to 1");
    gpio_write(nand->params.re, 1);

    DEBUG_PUTS("nand_onfi_set_read_disable: end");
}

void nand_onfi_set_write_enable(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_write_enable: entry");
    DEBUG("nand_onfi_set_write_enable: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_write_enable: set we to 0");
    gpio_write(nand->params.we, 0);

    DEBUG_PUTS("nand_onfi_set_write_enable: end");
}

void nand_onfi_set_write_disable(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_set_write_disable: entry");
    DEBUG("nand_onfi_set_write_disable: nand:%p\n", nand);

    DEBUG_PUTS("nand_onfi_set_write_disable: set we to 1");
    gpio_write(nand->params.we, 1);

    DEBUG_PUTS("nand_onfi_set_write_disable: end");
}

void nand_onfi_set_chip_enable(const nand_onfi_t* const nand, const uint8_t lun) {
    DEBUG_PUTS("nand_onfi_set_chip_enable: entry");
    DEBUG("nand_onfi_set_chip_enable: nand:%p lun:%hu\n", nand, lun);

    switch(lun) {
    case 0:
        DEBUG_PUTS("nand_onfi_set_chip_enable: set ce0 to 0");
        gpio_write(nand->params.ce0, 0);
        break;
    case 1:
        DEBUG_PUTS("nand_onfi_set_chip_enable: set ce1 to 0");
        gpio_write(nand->params.ce1, 0);
        break;
    case 2:
        DEBUG_PUTS("nand_onfi_set_chip_enable: set ce2 to 0");
        gpio_write(nand->params.ce2, 0);
        break;
    case 3:
        DEBUG_PUTS("nand_onfi_set_chip_enable: set ce3 to 0");
        gpio_write(nand->params.ce3, 0);
        break;
    }

    DEBUG_PUTS("nand_onfi_set_chip_enable: end");
}

void nand_onfi_set_chip_disable(const nand_onfi_t* const nand, const uint8_t lun) {
    DEBUG_PUTS("nand_onfi_set_chip_disable: entry");
    DEBUG("nand_onfi_set_chip_disable: nand:%p lun:%hu\n", nand, lun);

    switch(lun) {
    case 0:
        DEBUG_PUTS("nand_onfi_set_chip_disable: set ce0 to 1");
        gpio_write(nand->params.ce0, 1);
        break;
    case 1:
        DEBUG_PUTS("nand_onfi_set_chip_disable: set ce1 to 1");
        gpio_write(nand->params.ce1, 1);
        break;
    case 2:
        DEBUG_PUTS("nand_onfi_set_chip_disable: set ce2 to 1");
        gpio_write(nand->params.ce2, 1);
        break;
    case 3:
        DEBUG_PUTS("nand_onfi_set_chip_disable: set ce3 to 1");
        gpio_write(nand->params.ce3, 1);
        break;
    }

    DEBUG_PUTS("nand_onfi_set_chip_disable: end");
}

void nand_onfi_wait(const uint32_t delay_ns) {
    DEBUG_PUTS("nand_onfi_wait: entry");
    DEBUG("nand_onfi_wait: delay_ns:%" PRIu32 "\n", delay_ns);

    ztimer_sleep(ZTIMER_USEC, delay_ns / NAND_ONFI_TIMING_MICROSEC(1));

    DEBUG_PUTS("nand_onfi_wait: end");
}

void nand_onfi_wait_cmd_data(const nand_onfi_cmd_data_t* const cmd_data) {
    DEBUG_PUTS("nand_onfi_wait_cmd_data: entry");
    DEBUG("nand_onfi_wait_cmd_data: cmd_data:%p\n", cmd_data);
    DEBUG("nand_onfi_wait_cmd_data: cmd_data.delay_ns:%" PRIu32 "\n", cmd_data->delay_ns);

    nand_onfi_wait(cmd_data->delay_ns);

    DEBUG_PUTS("nand_onfi_wait_cmd_data: end");
}

bool nand_onfi_wait_until_ready(const nand_onfi_t* const nand, const nand_onfi_cmd_data_t* const cmd_data, const uint8_t this_lun) {
    DEBUG_PUTS("nand_onfi_wait_until_ready: entry");
    DEBUG("nand_onfi_wait_until_ready: nand:%p cmd_data:%p this_lun:%hu\n", nand, cmd_data, this_lun);

    const bool      wait_until_ready              = cmd_data->wait_until_ready;
    const bool      wait_until_other_luns_ready   = cmd_data->wait_until_other_luns_ready;
    const uint32_t  timeout_ns                    = cmd_data->timeout_ns;
    const uint8_t   lun_count                     = nand->lun_count;
    DEBUG("nand_onfi_wait_until_ready: cmd_data.wait_until_ready:%s\n", cmd_data->wait_until_ready ? "true" : "false");
    DEBUG("nand_onfi_wait_until_ready: cmd_data.wait_until_other_luns_ready:%s\n", cmd_data->wait_until_other_luns_ready ? "true" : "false");
    DEBUG("nand_onfi_wait_until_ready: cmd_data.timeout_ns:%" PRIu32 "\n", cmd_data->timeout_ns);
    DEBUG("nand_onfi_wait_until_ready: nand.lun_count:%hu\n", nand->lun_count);

    for(uint8_t lun_pos = 0; lun_pos < lun_count; ++lun_pos) {
        DEBUG("nand_onfi_wait_until_ready: lun_pos:%hu\n", lun_pos);

        if(wait_until_other_luns_ready && lun_pos != this_lun) {
            if(! nand_onfi_wait_until_lun_ready(nand, lun_pos, timeout_ns)) {
                DEBUG("nand_onfi_wait_until_ready: lun %hu (other lun) not ready\n", lun_pos);
                DEBUG_PUTS("nand_onfi_wait_until_ready: return:false");
                DEBUG_PUTS("nand_onfi_wait_until_ready: end");
                return false; /**< Other LUNs not ready but timeout */
            }
        }

        if(wait_until_ready && lun_pos == this_lun) {
            if(! nand_onfi_wait_until_lun_ready(nand, lun_pos, timeout_ns)) {
                DEBUG("nand_onfi_wait_until_ready: lun %hu (this lun) not ready\n", lun_pos);
                DEBUG_PUTS("nand_onfi_wait_until_ready: return:false");
                DEBUG_PUTS("nand_onfi_wait_until_ready: end");
                return false; /**< This LUN not ready but timeout */
            }
        }
    }

    DEBUG_PUTS("nand_onfi_wait_until_ready: return:true");
    DEBUG_PUTS("nand_onfi_wait_until_ready: end");
    return true; /**< All LUNs ready */
}

bool nand_onfi_wait_until_lun_ready(const nand_onfi_t* const nand, const uint8_t this_lun, const uint32_t timeout_ns) {
    DEBUG_PUTS("nand_onfi_wait_until_lun_ready: entry");
    DEBUG("nand_onfi_wait_until_lun_ready: nand:%p this_lun:%hu timeout_ns:%" PRIu32 "\n", nand, this_lun, timeout_ns);

    const uint32_t timeout_deadline = nand_onfi_deadline_from_interval(timeout_ns);
    uint32_t timeout_left = timeout_deadline;
    DEBUG("nand_onfi_wait_until_lun_ready: timeout_deadline:%" PRIu32 "\n", timeout_deadline);

    do {
        switch(this_lun) {
        case 0:
            if(gpio_read(nand->params.rb0))
            {
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: rb0 became ready");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: return:true");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: end");
                return true;
            }
            break;
        case 1:
            if(gpio_read(nand->params.rb1))
            {
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: rb1 became ready");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: return:true");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: end");
                return true;
            }
            break;
        case 2:
            if(gpio_read(nand->params.rb2))
            {
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: rb2 became ready");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: return:true");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: end");
                return true;
            }
            break;
        case 3:
            if(gpio_read(nand->params.rb3))
            {
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: rb3 became ready");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: return:true");
                DEBUG_PUTS("nand_onfi_wait_until_lun_ready: end");
                return true;
            }
            break;
        }

        timeout_left = nand_onfi_deadline_left(timeout_deadline);
        DEBUG("nand_onfi_wait_until_lun_ready: timeout_left:%" PRIu32 "\n", timeout_left);
    } while(timeout_ns == 0 || timeout_left > 0);

    DEBUG_PUTS("nand_onfi_wait_until_lun_ready: not ready but timeout");
    DEBUG_PUTS("nand_onfi_wait_until_lun_ready: return:false");
    DEBUG_PUTS("nand_onfi_wait_until_lun_ready: end");
    return false; /**< Not ready but timeout */
}

size_t nand_onfi_all_pages_count(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_all_pages_count: entry");
    DEBUG("nand_onfi_all_pages_count: nand:%p\n", nand);

    const size_t ret_size = nand->pages_per_block * nand->blocks_per_lun * nand->lun_count;

    DEBUG("nand_onfi_all_pages_count: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_all_pages_count: end");
    return ret_size;
}

size_t nand_onfi_one_page_size(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_one_page_size: entry");
    DEBUG("nand_onfi_one_page_size: nand:%p\n", nand);

    const size_t ret_size = nand->data_bytes_per_page + nand->spare_bytes_per_page;

    DEBUG("nand_onfi_one_page_size: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_one_page_size: end");
    return ret_size;
}

size_t nand_onfi_all_data_bytes_size(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_all_data_bytes_size: entry");
    DEBUG("nand_onfi_all_data_bytes_size: nand:%p\n", nand);

    const size_t ret_size = nand->data_bytes_per_page * nand_onfi_all_pages_count(nand);

    DEBUG("nand_onfi_all_data_bytes_size: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_all_data_bytes_size: end");
    return ret_size;
}

size_t nand_onfi_all_spare_bytes_size(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_all_spare_bytes_size: entry");
    DEBUG("nand_onfi_all_spare_bytes_size: nand:%p\n", nand);

    const size_t ret_size = nand->spare_bytes_per_page * nand_onfi_all_pages_count(nand);

    DEBUG("nand_onfi_all_spare_bytes_size: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_all_spare_bytes_size: end");
    return ret_size;
}

size_t nand_onfi_all_pages_size(const nand_onfi_t* const nand) {
    DEBUG_PUTS("nand_onfi_all_pages_size: entry");
    DEBUG("nand_onfi_all_pages_size: nand:%p\n", nand);

    const size_t ret_size = nand_onfi_all_data_bytes_size(nand) + nand_onfi_all_spare_bytes_size(nand);

    DEBUG("nand_onfi_all_pages_size: return:%u\n", ret_size);
    DEBUG_PUTS("nand_onfi_all_pages_size: end");
    return ret_size;
}

uint32_t nand_onfi_offset_to_addr_column(const uint32_t offset) {
    DEBUG_PUTS("nand_onfi_offset_to_addr_column: entry");
    DEBUG("nand_onfi_offset_to_addr_column: offset:%" PRIu32 "\n", offset);

    const uint32_t ret_addr = offset;

    DEBUG("nand_onfi_offset_to_addr_column: return:%" PRIx32 "\n", ret_addr); 
    DEBUG_PUTS("nand_onfi_offset_to_addr_column: end");
    return ret_addr;
}

uint32_t nand_onfi_page_num_to_addr_row(const uint32_t page_num) {
    DEBUG_PUTS("nand_onfi_offset_to_addr_row: entry");
    DEBUG("nand_onfi_page_num_to_addr_row: page_num:%" PRIu32 "\n", page_num);

    const uint32_t ret_addr = page_num;

    DEBUG("nand_onfi_page_num_to_addr_row: return:%" PRIx32 "\n", ret_addr);
    DEBUG_PUTS("nand_onfi_offset_to_addr_row: end");
    return ret_addr;
}

uint32_t nand_onfi_addr_flat_to_addr_column(const nand_onfi_t* const nand, const uint32_t addr_flat) {
    DEBUG_PUTS("nand_onfi_addr_flat_to_addr_column: entry");
    DEBUG("nand_onfi_addr_flat_to_addr_column: nand:%p addr_flat:%" PRIu32 "\n", nand, addr_flat);

    const uint32_t ret_addr = addr_flat % nand_onfi_one_page_size(nand);

    DEBUG("nand_onfi_addr_flat_to_addr_column: return:%" PRIx32 "\n", ret_addr);
    DEBUG_PUTS("nand_onfi_addr_flat_to_addr_column: end");
    return ret_addr;
}

uint32_t nand_onfi_addr_flat_to_addr_row(const nand_onfi_t* const nand, const uint32_t addr_flat) {
    DEBUG_PUTS("nand_onfi_addr_flat_to_addr_row: entry");
    DEBUG("nand_onfi_addr_flat_to_addr_row: nand:%p addr_flat:%" PRIu32 "\n", nand, addr_flat);

    const uint32_t ret_addr = addr_flat / nand_onfi_one_page_size(nand);

    DEBUG("nand_onfi_addr_flat_to_addr_row: return:%" PRIx32 "\n", ret_addr);
    DEBUG_PUTS("nand_onfi_addr_flat_to_addr_row: end");
    return ret_addr;
}

uint32_t nand_onfi_addr_to_addr_flat(const nand_onfi_t* const nand, const uint32_t addr_row, const uint32_t addr_column) {
    DEBUG_PUTS("nand_onfi_addr_to_addr_flat: entry");
    DEBUG("nand_onfi_addr_to_addr_flat: nand:%p addr_row:%" PRIx32 " addr_column:%" PRIx32 "\n", nand, addr_row, addr_column);

    const uint32_t ret_addr = addr_row * nand_onfi_one_page_size(nand) + addr_column;

    DEBUG("nand_onfi_addr_to_addr_flat: return:%" PRIu32 "\n", ret_addr);
    DEBUG_PUTS("nand_onfi_addr_to_addr_flat: end");
    return ret_addr;
}

uint32_t nand_onfi_deadline_from_interval(const uint32_t interval_ns) {
    DEBUG_PUTS("nand_onfi_deadline_from_interval: entry");
    DEBUG("nand_onfi_deadline_from_interval: interval_ns:%" PRIu32 "\n", interval_ns);

    const uint32_t ret_time = ztimer_now(ZTIMER_USEC) + (interval_ns / NAND_ONFI_TIMING_MICROSEC(1));

    DEBUG("nand_onfi_deadline_from_interval: return:%" PRIu32 "\n", ret_time);
    DEBUG_PUTS("nand_onfi_deadline_from_interval: end");
    return ret_time;
}

uint32_t nand_onfi_deadline_left(const uint32_t deadline) {
    DEBUG_PUTS("nand_onfi_deadline_left: entry");
    DEBUG("nand_onfi_deadline_left: deadline:%" PRIu32 "\n", deadline);

    int32_t left = (int32_t)(deadline - ztimer_now(ZTIMER_USEC));
    const uint32_t ret_left = (left < 0) ? 0 : (uint32_t)left;

    DEBUG("nand_onfi_deadline_left: return:%" PRIu32 "\n", ret_left);
    DEBUG_PUTS("nand_onfi_deadline_left: end");
    return ret_left;
}

#ifdef DEBUG_PRINT_NOOP
#  ifdef DEBUG
#    undef DEBUG
#    define DEBUG(...) do { if (ENABLE_DEBUG) { DEBUG_PRINT(__VA_ARGS__); } } while (0)
#  endif
#  undef DEBUG_PRINT_NOOP
#endif
