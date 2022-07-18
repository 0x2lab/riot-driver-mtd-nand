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

    if(params == NULL) {
        nand->init_done = false;
        return NAND_INIT_ERROR;
    }

    int nand_init_status = nand_init(nand, params);
    if(nand_init_status != NAND_OK) {
        return nand_init_status;
    }

    //TODO: nand_onfi->onfi_chip init
    //TODO: nand_onfi->onfi_prop init

    return NAND_OK;
}

size_t nand_onfi_run_cmd(nand_onfi_t* const nand_onfi, const nand_cmd_t* const cmd, nand_cmd_params_t* const cmd_params, nand_rw_response_t* const err) {
    if(nand_onfi == NULL || cmd == NULL) {
        if(err != NULL) {
            *err = NAND_RW_CMD_INVALID;
        }
        return 0;
    }

          nand_t*                nand           = (nand_t*)nand_onfi;
    const uint8_t                lun_no         = cmd_params->lun_no;
          nand_cmd_t*      const cmd_override   = cmd_params->cmd_override;
    const nand_hook_cb_t         pre_hook_cb    = (cmd_override != NULL && cmd_override->pre_hook_cb   != NULL)               ? cmd_override->pre_hook_cb   : cmd->pre_hook_cb;
    const nand_hook_cb_t         post_hook_cb   = (cmd_override != NULL && cmd_override->post_hook_cb  != NULL)               ? cmd_override->post_hook_cb  : cmd->post_hook_cb;
    const size_t                 chains_length  = (cmd_override != NULL && cmd_override->chains_length >  cmd->chains_length) ? cmd_override->chains_length : cmd->chains_length;
          nand_cmd_chain_t*      chains         = (nand_cmd_chain_t*)malloc(sizeof(nand_cmd_chain_t) * chains_length);

    if(chains_length > NAND_MAX_COMMAND_CYCLE_SIZE) {
        if(err != NULL) {
            *err = NAND_RW_CMD_CHAIN_TOO_LONG;
        }
        free(chains);
        return 0;
    } else {
        if(cmd_override == NULL) {
            memcpy(&chains, &(cmd->chains), sizeof(nand_cmd_chain_t) * cmd->chains_length);
        } else {
            for(size_t pos = 0; pos < chains_length; ++pos) {
                if(cmd_override->chains[pos].cycles_defined || pos >= cmd->chains_length) {
                    memcpy(&(chains[pos]), &(cmd_override->chains[pos]), sizeof(nand_cmd_chain_t));
                } else {
                    memcpy(&(chains[pos]), &(cmd->chains[pos]), sizeof(nand_cmd_chain_t));
                }
            }
        }
    }

    size_t rw_size = 0;

    nand_set_chip_enable(nand, lun_no);
    nand_set_write_protect_disable(nand);
    nand_set_read_disable(nand);
    nand_set_write_disable(nand);

    for(size_t seq = 0; seq < chains_length; ++seq) {
              nand_cmd_chain_t*    const current_chain  = &(chains[seq]);
        const bool                       cycles_defined =   current_chain->cycles_defined;
        const nand_cmd_timings_t*  const timings        = &(current_chain->timings);
        const nand_cmd_type_t            cycles_type    =   current_chain->cycles_type;
        const nand_cmd_cycles_t*   const cycles         = &(current_chain->cycles);

        if(! cycles_defined) {
            continue;
        }

        nand_wait(timings->pre_delay_ns);
        
        switch(cycles_type) {
        case NAND_CMD_TYPE_CMD_WRITE:
        case NAND_CMD_TYPE_ADDR_WRITE:
        case NAND_CMD_TYPE_ADDR_COLUMN_WRITE:
        case NAND_CMD_TYPE_ADDR_ROW_WRITE:
        case NAND_CMD_TYPE_ADDR_SINGLE_WRITE:
            {
                nand_wait(timings->latch_enable_pre_delay_ns);

                switch(cycles_type) {
                case NAND_CMD_TYPE_CMD_WRITE:
                    nand_set_latch_command(nand);
                    break;
                default:
                    nand_set_latch_address(nand);
                    break;
                }

                nand_wait(timings->latch_enable_post_delay_ns);

                if(! nand_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    nand_wait(timings->latch_disable_pre_delay_ns);
                    nand_set_latch_raw(nand);
                    nand_wait(timings->latch_disable_post_delay_ns);

                    if(err != NULL) {
                        *err = NAND_RW_TIMEOUT;
                    }
                    free(chains);
                    return rw_size;
                } else {
                    nand_wait(timings->ready_post_delay_ns);
                }

                if(pre_hook_cb != NULL) {
                    pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_set_io_pin_write(nand);

                switch(cycles_type) {
                case NAND_CMD_TYPE_CMD_WRITE:
                    rw_size += nand_write_cmd(nand, &(cycles->cmd), timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    break;

                case NAND_CMD_TYPE_ADDR_WRITE:
                    rw_size += nand_write_addr(nand, cycles->addr, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    break;

                case NAND_CMD_TYPE_ADDR_COLUMN_WRITE:
                    rw_size += nand_write_addr_column(nand, &(cycles->addr_column), timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    break;

                case NAND_CMD_TYPE_ADDR_ROW_WRITE:
                    rw_size += nand_write_addr_row(nand, &(cycles->addr_row), timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    break;

                case NAND_CMD_TYPE_ADDR_SINGLE_WRITE:
                    rw_size += nand_write_addr_single(nand, &(cycles->addr_single), timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    break;

                default:
                    break;
                }

                if(post_hook_cb != NULL) {
                    post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_wait(timings->latch_disable_pre_delay_ns);
                nand_set_latch_raw(nand);
                nand_wait(timings->latch_disable_post_delay_ns);
            }
            break;

        case NAND_CMD_TYPE_RAW_WRITE:
        case NAND_CMD_TYPE_RAW_READ:
            {
                nand_raw_t* const raw                   =   cycles->raw;
                size_t*     const raw_size              = &(raw->raw_size);
                uint16_t*   const buffer                =   raw->buffer;
                size_t            buffer_size           =   raw->buffer_size;
                size_t*     const current_buffer_seq    = &(raw->current_buffer_seq);
                size_t*     const current_raw_offset    = &(raw->current_raw_offset);

                if(*raw_size == 0) {
                    if(err != NULL) {
                        *err = NAND_RW_OK;
                    }
                    free(chains);
                    return rw_size;
                }

                *current_raw_offset = 0;
                *current_buffer_seq = 0;

                nand_wait(timings->latch_enable_pre_delay_ns);
                nand_set_latch_raw(nand);
                nand_wait(timings->latch_enable_post_delay_ns);

                if(! nand_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    if(err != NULL) {
                        *err = NAND_RW_TIMEOUT;
                    }
                    free(chains);
                    return rw_size;
                } else {
                    nand_wait(timings->ready_post_delay_ns);
                }

                while(buffer_size > 0 && *current_raw_offset < *raw_size) {
                    if(pre_hook_cb != NULL) {
                        pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);

                        if(*current_raw_offset >= *raw_size) {
                            break;
                        }
                    }

                    if(buffer != NULL) {
                        const size_t raw_remaining_size = *raw_size - *current_raw_offset;
                        buffer_size = (raw_remaining_size > buffer_size) ? buffer_size : raw_remaining_size; /**< Only touch locally, instead of touch the passed param */

                        switch(cycles_type) {
                        case NAND_CMD_TYPE_RAW_WRITE:
                            print_str("TEST1\r\n");
                            fflush(stdout);
                            return 0;
                            {
                                nand_set_io_pin_write(nand);
                                rw_size += nand_write_raw(nand, buffer, buffer_size, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                            }
                            break;

                        case NAND_CMD_TYPE_RAW_READ:
                            print_str("TEST2\r\n");
                            fflush(stdout);
                            nand_set_io_pin_read(nand);
                            print_u32_hex((uint32_t)(uintptr_t)buffer);
                            print_str("\r\n");
                            fflush(stdout);
                            {
                                rw_size += nand_read_raw(nand, buffer, buffer_size, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                                if(raw->buffer_size != buffer_size) {
                                    raw->buffer_size = buffer_size; /**< Touch the passed param */
                                }
                            }
                            break;

                        default:
                            print_str("TEST3\r\n");
                            fflush(stdout);
                            return 0;
                            break;
                        }
                    }

                    *current_raw_offset += buffer_size;

                    if(post_hook_cb != NULL) {
                        post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                    }

                    ++(*current_buffer_seq);
                }

            }
            break;
        }

        nand_wait(timings->post_delay_ns);
    }

    nand_set_chip_disable(nand, lun_no);

    if(err != NULL) {
        *err = NAND_RW_OK;
    }
    free(chains);
    return rw_size;
}
