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

#include "nand/onfi.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"

#include "nand/onfi/msb.h"
#include "nand/onfi/enums.h"
#include "nand/onfi/constants.h"
#include "nand/onfi/constants_timing.h"
#include "nand/onfi/types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

int nand_onfi_init(nand_onfi_t* const nand, nand_onfi_params_t* const params) {
    if(nand == NULL) {
        return NAND_ONFI_INIT_ERROR;
    }

    if(params == NULL) {
        nand->init_done = false;
        return NAND_ONFI_INIT_ERROR;
    }

    nand->params = *params;

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

    nand->init_done = true;

    return NAND_ONFI_OK;
}

size_t nand_onfi_run_cmd(nand_onfi_t* const nand, const nand_onfi_cmd_t* const cmd, nand_onfi_cmd_params_t* const cmd_params, nand_onfi_rw_response_t* const err) {
    if(cmd == NULL) {
        *err = NAND_ONFI_RW_CMD_INVALID;
        return 0;
    }

    const uint8_t                                 lun_no            = cmd_params->lun_no;
          nand_onfi_cmd_t*                  const cmd_override      = cmd_params->cmd_override;
    const nand_onfi_hook_cb_t                     pre_hook_cb       = (cmd_override != NULL && cmd_override->pre_hook_cb   != NULL)               ? cmd_override->pre_hook_cb   : cmd->pre_hook_cb;
    const nand_onfi_hook_cb_t                     post_hook_cb      = (cmd_override != NULL && cmd_override->post_hook_cb  != NULL)               ? cmd_override->post_hook_cb  : cmd->post_hook_cb;
    const size_t                                  chains_length     = (cmd_override != NULL && cmd_override->chains_length >  cmd->chains_length) ? cmd_override->chains_length : cmd->chains_length;
          nand_onfi_cmd_chain_t*                  chains            = (nand_onfi_cmd_chain_t*)malloc(sizeof(nand_onfi_cmd_chain_t) * chains_length);

    if(chains_length > NAND_ONFI_MAX_COMMAND_CYCLE_SIZE) {
        *err = NAND_ONFI_RW_CMD_CHAIN_TOO_LONG;
        free(chains);
        return 0;
    } else {
        if(cmd_override == NULL) {
            memcpy(&chains, &(cmd->chains), sizeof(nand_onfi_cmd_chain_t) * cmd->chains_length);
        } else {
            for(size_t pos = 0; pos < chains_length; ++pos) {
                if(cmd_override->chains[pos].cycles_defined || pos >= cmd->chains_length) {
                    memcpy(&(chains[pos]), &(cmd_override->chains[pos]), sizeof(nand_onfi_cmd_chain_t));
                } else {
                    memcpy(&(chains[pos]), &(cmd->chains[pos]), sizeof(nand_onfi_cmd_chain_t));
                }
            }
        }
    }

    size_t ret_size = 0;

    nand_onfi_set_chip_enable(nand, lun_no);
    nand_onfi_set_write_protect_disable(nand);
    nand_onfi_set_read_disable(nand);
    nand_onfi_set_write_disable(nand);

    for(size_t seq = 0; seq < chains_length; ++seq) {
              nand_onfi_cmd_chain_t*    const current_chain     = &(chains[seq]);
        const bool                            cycles_defined    =   current_chain->cycles_defined;
        const nand_onfi_cmd_timings_t*  const timings           = &(current_chain->timings);
        const nand_onfi_cmd_type_t            cycles_type       =   current_chain->cycles_type;
        const nand_onfi_cmd_cycles_t*   const cycles            = &(current_chain->cycles);

        if(! cycles_defined) {
            continue;
        }

        nand_onfi_wait(timings->pre_delay_ns);
        puts("TEST6");
        
        switch(cycles_type) {
        case NAND_ONFI_CMD_TYPE_CMD_WRITE:;
            {
                const uint8_t command = cycles->cmd;

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_command(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                puts("TEST7");
                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                    nand_onfi_set_latch_raw(nand);
                    nand_onfi_wait(timings->latch_disable_post_delay_ns);

                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    puts("TEST8");
                    nand_onfi_wait(timings->ready_post_delay_ns);
                }

                if(pre_hook_cb != NULL) {
                    pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_cmd(nand, &command, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                if(post_hook_cb != NULL) {
                    post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_disable_post_delay_ns);
                puts("TEST7");
            }
            break;

        case NAND_ONFI_CMD_TYPE_ADDR_WRITE:;
            {
                const uint64_t* const addr = cycles->addr;

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_address(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                    nand_onfi_set_latch_raw(nand);
                    nand_onfi_wait(timings->latch_disable_post_delay_ns);

                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    nand_onfi_wait(timings->ready_post_delay_ns);
                }

                if(pre_hook_cb != NULL) {
                    pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr(nand, addr, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                if(post_hook_cb != NULL) {
                    post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_disable_post_delay_ns);
            }
            break;

        case NAND_ONFI_CMD_TYPE_ADDR_COLUMN_WRITE:;
            {
                const uint64_t* const addr_column = &(cycles->addr_column);

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_address(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                    nand_onfi_set_latch_raw(nand);
                    nand_onfi_wait(timings->latch_disable_post_delay_ns);

                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    nand_onfi_wait(timings->ready_post_delay_ns);
                }

                if(pre_hook_cb != NULL) {
                    pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr_column(nand, addr_column, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                if(post_hook_cb != NULL) {
                    post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_disable_post_delay_ns);
            }
            break;

        case NAND_ONFI_CMD_TYPE_ADDR_ROW_WRITE:;
            {
                const uint64_t* const addr_row = &(cycles->addr_row);

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_address(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                    nand_onfi_set_latch_raw(nand);
                    nand_onfi_wait(timings->latch_disable_post_delay_ns);

                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    nand_onfi_wait(timings->ready_post_delay_ns);
                }

                if(pre_hook_cb != NULL) {
                    pre_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_set_io_pin_write(nand);
                ret_size += nand_onfi_write_addr_row(nand, addr_row, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                if(post_hook_cb != NULL) {
                    post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                }

                nand_onfi_wait(timings->latch_disable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_disable_post_delay_ns);

            }
            break;

        case NAND_ONFI_CMD_TYPE_RAW_WRITE:;
            {
                nand_onfi_raw_t* const raw                =   cycles->raw;
                size_t*          const raw_size           = &(raw->raw_size);
                uint16_t*        const buffer             =   raw->buffer;
                size_t                 buffer_size        =   raw->buffer_size;
                size_t*          const current_buffer_seq = &(raw->current_buffer_seq);
                size_t*          const current_raw_offset = &(raw->current_raw_offset);

                if(*raw_size == 0) {
                    *err = NAND_ONFI_RW_OK;
                    free(chains);
                    return ret_size;
                }

                *current_raw_offset = 0;
                *current_buffer_seq = 0;

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    nand_onfi_wait(timings->ready_post_delay_ns);
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

                        nand_onfi_set_io_pin_write(nand);
                        ret_size += nand_onfi_write_raw(nand, buffer, buffer_size, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);
                    }

                    *current_raw_offset += buffer_size;

                    if(post_hook_cb != NULL) {
                        post_hook_cb(nand, cmd, cmd_params, seq, current_chain);
                    }

                    ++(*current_buffer_seq);
                }

            }
            break;

        case NAND_ONFI_CMD_TYPE_RAW_READ:;
            {
                nand_onfi_raw_t* const raw                =   cycles->raw;
                size_t*          const raw_size           = &(raw->raw_size);
                uint16_t*        const buffer             =   raw->buffer;
                size_t                 buffer_size        =   raw->buffer_size;
                size_t*          const current_buffer_seq = &(raw->current_buffer_seq);
                size_t*          const current_raw_offset = &(raw->current_raw_offset);

                if(*raw_size == 0) {
                    *err = NAND_ONFI_RW_OK;
                    free(chains);
                    return ret_size;
                }

                *current_raw_offset = 0;
                *current_buffer_seq = 0;

                nand_onfi_wait(timings->latch_enable_pre_delay_ns);
                nand_onfi_set_latch_raw(nand);
                nand_onfi_wait(timings->latch_enable_post_delay_ns);

                if(! nand_onfi_wait_until_ready(nand, lun_no, timings->ready_this_lun_timeout_ns, timings->ready_other_luns_timeout_ns)) {
                    *err = NAND_ONFI_RW_TIMEOUT;
                    free(chains);
                    return ret_size;
                } else {
                    nand_onfi_wait(timings->ready_post_delay_ns);
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

                        nand_onfi_set_io_pin_read(nand);
                        ret_size += nand_onfi_read_raw(nand, buffer, buffer_size, timings->cycle_rw_enable_post_delay_ns, timings->cycle_rw_disable_post_delay_ns);

                        if(raw->buffer_size != buffer_size) {
                            raw->buffer_size = buffer_size; /**< Touch the passed param */
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

        nand_onfi_wait(timings->post_delay_ns);
    }

    nand_onfi_set_chip_disable(nand, lun_no);

    *err = NAND_ONFI_RW_OK;
    free(chains);
    return ret_size;
}

size_t nand_onfi_write_cmd(const nand_onfi_t* const nand, const uint8_t* const cmd, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    uint16_t cmd_aligned = (uint16_t)(*cmd);
    const size_t ret_size = nand_onfi_write_cycle(nand, &cmd_aligned, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns); 
    return ret_size;
}

size_t nand_onfi_write_addr(const nand_onfi_t* const nand, const uint64_t addr[], const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    ret_size += nand_onfi_write_addr_column(nand, &(addr[NAND_ONFI_ADDR_INDEX_COLUMN]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    ret_size += nand_onfi_write_addr_row(nand, &(addr[NAND_ONFI_ADDR_INDEX_ROW]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    return ret_size;
}

size_t nand_onfi_write_addr_column(const nand_onfi_t* const nand, const uint64_t* const addr_column, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    const uint8_t column_addr_cycles = nand->column_addr_cycles;

    for(size_t seq = 0; seq < column_addr_cycles; ++seq) {
        const uint64_t mask = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1) << (NAND_ONFI_ADDR_IO_BITS * seq);
        const uint16_t cycle_data = *addr_column & mask;
        ret_size += nand_onfi_write_cycle(nand, &cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    }

    return ret_size;
}

size_t nand_onfi_write_addr_row(const nand_onfi_t* const nand, const uint64_t* const addr_row, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    const uint8_t row_addr_cycles = nand->row_addr_cycles;

    for(size_t seq = 0; seq < row_addr_cycles; ++seq) {
        const uint64_t mask = ((1 << NAND_ONFI_ADDR_IO_BITS) - 1) << (NAND_ONFI_ADDR_IO_BITS * seq);
        const uint16_t cycle_data = *addr_row & mask;
        ret_size += nand_onfi_write_cycle(nand, &cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    }

    return ret_size;
}

size_t nand_onfi_write_raw(const nand_onfi_t* const nand, const uint16_t* const data, const size_t data_size, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;

    for(size_t seq = 0; seq < data_size; ++seq) {
        ret_size += nand_onfi_write_cycle(nand, &(data[seq]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    }

    return ret_size;
}

size_t nand_onfi_write_cycle(const nand_onfi_t* const nand, const uint16_t* const cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    const size_t ret_size = nand_onfi_write_io(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
    return ret_size;
}

size_t nand_onfi_write_io(const nand_onfi_t* const nand, const uint16_t* const data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    nand_onfi_set_write_enable(nand);

    if(cycle_write_enable_post_delay_ns > 0) {
        nand_onfi_wait(cycle_write_enable_post_delay_ns);
    }

    if(nand->data_bus_width == 16) {
        const int io15 = (*data & NAND_ONFI_MSB15) ? 1 : 0;
        gpio_write(nand->params.io15, io15);

        const int io14 = (*data & NAND_ONFI_MSB14) ? 1 : 0;
        gpio_write(nand->params.io14, io14);

        const int io13 = (*data & NAND_ONFI_MSB13) ? 1 : 0;
        gpio_write(nand->params.io13, io13);

        const int io12 = (*data & NAND_ONFI_MSB12) ? 1 : 0;
        gpio_write(nand->params.io12, io12);

        const int io11 = (*data & NAND_ONFI_MSB11) ? 1 : 0;
        gpio_write(nand->params.io11, io11);

        const int io10 = (*data & NAND_ONFI_MSB10) ? 1 : 0;
        gpio_write(nand->params.io10, io10);

        const int io9 = (*data & NAND_ONFI_MSB9) ? 1 : 0;
        gpio_write(nand->params.io9, io9);

        const int io8 = (*data & NAND_ONFI_MSB8) ? 1 : 0;
        gpio_write(nand->params.io8, io8);
    }


    const int io7 = (*data & NAND_ONFI_MSB7) ? 1 : 0;
    gpio_write(nand->params.io7, io7);

    const int io6 = (*data & NAND_ONFI_MSB6) ? 1 : 0;
    gpio_write(nand->params.io6, io6);

    const int io5 = (*data & NAND_ONFI_MSB5) ? 1 : 0;
    gpio_write(nand->params.io5, io5);

    const int io4 = (*data & NAND_ONFI_MSB4) ? 1 : 0;
    gpio_write(nand->params.io4, io4);

    const int io3 = (*data & NAND_ONFI_MSB3) ? 1 : 0;
    gpio_write(nand->params.io3, io3);

    const int io2 = (*data & NAND_ONFI_MSB2) ? 1 : 0;
    gpio_write(nand->params.io2, io2);

    const int io1 = (*data & NAND_ONFI_MSB1) ? 1 : 0;
    gpio_write(nand->params.io1, io1);

    const int io0 = (*data & NAND_ONFI_MSB0) ? 1 : 0;
    gpio_write(nand->params.io0, io0);

    nand_onfi_set_write_disable(nand);

    if(cycle_write_disable_post_delay_ns > 0) {
        nand_onfi_wait(cycle_write_disable_post_delay_ns);
    }

    return 1;
}

size_t nand_onfi_read_raw(const nand_onfi_t* const nand, uint16_t* const out_buffer, const size_t buffer_size, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    size_t ret_size = 0;

    for(size_t seq = 0; seq < buffer_size; ++seq) {
        ret_size += nand_onfi_read_cycle(nand, &(out_buffer[seq]), cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
    }

    return ret_size;
}

size_t nand_onfi_read_cycle(const nand_onfi_t* const nand, uint16_t* const out_cycle_data, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    const size_t ret_size = nand_onfi_read_io(nand, out_cycle_data, cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
    return ret_size;
}

size_t nand_onfi_read_io(const nand_onfi_t* const nand, uint16_t* const out_data, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    *out_data = 0;
    nand_onfi_set_read_enable(nand);

    if(cycle_read_enable_post_delay_ns > 0) {
        nand_onfi_wait(cycle_read_enable_post_delay_ns);
    }

    if(nand->data_bus_width == 16) {
        const bool io15 = gpio_read(nand->params.io15) != 0;
        const bool io14 = gpio_read(nand->params.io14) != 0;
        const bool io13 = gpio_read(nand->params.io13) != 0;
        const bool io12 = gpio_read(nand->params.io12) != 0;
        const bool io11 = gpio_read(nand->params.io11) != 0;
        const bool io10 = gpio_read(nand->params.io10) != 0;
        const bool io9 = gpio_read(nand->params.io9) != 0;
        const bool io8 = gpio_read(nand->params.io8) != 0;

        *out_data = (io15 << 15) | (io14 << 14) | (io13 << 13) | (io12 << 12) | (io11 << 11) | (io10 << 10) | (io9 << 9) | (io8 << 8);
    }


    const bool io7 = gpio_read(nand->params.io7) != 0;
    const bool io6 = gpio_read(nand->params.io6) != 0;
    const bool io5 = gpio_read(nand->params.io5) != 0;
    const bool io4 = gpio_read(nand->params.io4) != 0;
    const bool io3 = gpio_read(nand->params.io3) != 0;
    const bool io2 = gpio_read(nand->params.io2) != 0;
    const bool io1 = gpio_read(nand->params.io1) != 0;
    const bool io0 = gpio_read(nand->params.io0) != 0;

    *out_data = *out_data | (io7 << 7) | (io6 << 6) | (io5 << 5) | (io4 << 4) | (io3 << 3) | (io2 << 2) | (io1 << 1) | io0;

    nand_onfi_set_read_disable(nand);

    if(cycle_read_disable_post_delay_ns > 0) {
        nand_onfi_wait(cycle_read_disable_post_delay_ns);
    }

    return 1;
}

void nand_onfi_set_pin_default(const nand_onfi_t* const nand) {
    nand_onfi_set_ctrl_pin(nand);
    nand_onfi_set_io_pin_write(nand);
}

void nand_onfi_set_ctrl_pin(const nand_onfi_t* const nand) {
    gpio_init(nand->params.ce0, GPIO_OUT);
    gpio_init(nand->params.rb0, GPIO_IN);

    if(nand->lun_count > 0) {
        gpio_init(nand->params.ce1, GPIO_OUT);
        gpio_init(nand->params.rb1, GPIO_IN);
    }

    if(nand->lun_count > 1) {
        gpio_init(nand->params.ce2, GPIO_OUT);
        gpio_init(nand->params.rb2, GPIO_IN);
    }

    if(nand->lun_count > 2) {
        gpio_init(nand->params.ce3, GPIO_OUT);
        gpio_init(nand->params.rb3, GPIO_IN);
    }

    gpio_init(nand->params.re, GPIO_OUT);
    gpio_init(nand->params.we, GPIO_OUT);
    gpio_init(nand->params.wp, GPIO_OUT);
    gpio_init(nand->params.cle, GPIO_OUT);
    gpio_init(nand->params.ale, GPIO_OUT);
}

void nand_onfi_set_io_pin_write(const nand_onfi_t* const nand) {
    if(nand->data_bus_width == 16) {
        gpio_init(nand->params.io15, GPIO_OUT);
        gpio_init(nand->params.io14, GPIO_OUT);
        gpio_init(nand->params.io13, GPIO_OUT);
        gpio_init(nand->params.io12, GPIO_OUT);
        gpio_init(nand->params.io11, GPIO_OUT);
        gpio_init(nand->params.io10, GPIO_OUT);
        gpio_init(nand->params.io9, GPIO_OUT);
        gpio_init(nand->params.io8, GPIO_OUT);
    }

    gpio_init(nand->params.io7, GPIO_OUT);
    gpio_init(nand->params.io6, GPIO_OUT);
    gpio_init(nand->params.io5, GPIO_OUT);
    gpio_init(nand->params.io4, GPIO_OUT);
    gpio_init(nand->params.io3, GPIO_OUT);
    gpio_init(nand->params.io2, GPIO_OUT);
    gpio_init(nand->params.io1, GPIO_OUT);
    gpio_init(nand->params.io0, GPIO_OUT);
}

void nand_onfi_set_io_pin_read(const nand_onfi_t* const nand) {
    if(nand->data_bus_width == 16) {
        gpio_init(nand->params.io15, GPIO_IN);
        gpio_init(nand->params.io14, GPIO_IN);
        gpio_init(nand->params.io13, GPIO_IN);
        gpio_init(nand->params.io12, GPIO_IN);
        gpio_init(nand->params.io11, GPIO_IN);
        gpio_init(nand->params.io10, GPIO_IN);
        gpio_init(nand->params.io9, GPIO_IN);
        gpio_init(nand->params.io8, GPIO_IN);
    }

    gpio_init(nand->params.io7, GPIO_IN);
    gpio_init(nand->params.io6, GPIO_IN);
    gpio_init(nand->params.io5, GPIO_IN);
    gpio_init(nand->params.io4, GPIO_IN);
    gpio_init(nand->params.io3, GPIO_IN);
    gpio_init(nand->params.io2, GPIO_IN);
    gpio_init(nand->params.io1, GPIO_IN);
    gpio_init(nand->params.io0, GPIO_IN);
}

void nand_onfi_set_latch_command(const nand_onfi_t* const nand) {
    gpio_write(nand->params.ale, 0);
    gpio_write(nand->params.cle, 1);
}

void nand_onfi_set_latch_address(const nand_onfi_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 1);
}

void nand_onfi_set_latch_raw(const nand_onfi_t* const nand) {
    gpio_write(nand->params.cle, 0);
    gpio_write(nand->params.ale, 0);
}

void nand_onfi_set_read_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.re, 0);
}

void nand_onfi_set_read_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.re, 1);
}

void nand_onfi_set_write_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.we, 0);
}

void nand_onfi_set_write_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.we, 1);
}

void nand_onfi_set_write_protect_enable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.wp, 0);
}

void nand_onfi_set_write_protect_disable(const nand_onfi_t* const nand) {
    gpio_write(nand->params.wp, 1);
}

void nand_onfi_set_chip_enable(const nand_onfi_t* const nand, const uint8_t lun_no) {
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

void nand_onfi_set_chip_disable(const nand_onfi_t* const nand, const uint8_t lun_no) {
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

void nand_onfi_wait(const uint32_t delay_ns) {
    if(delay_ns == 0) {
        return;
    }

    const uint32_t sleep_time = delay_ns / NAND_ONFI_TIMING_MICROSEC(1);

    ztimer_sleep(ZTIMER_USEC, sleep_time);

}

bool nand_onfi_wait_until_ready(const nand_onfi_t* const nand, const uint8_t this_lun_no, const uint32_t ready_this_lun_timeout_ns, const uint32_t ready_other_luns_timeout_ns) {
    const uint8_t lun_count = nand->lun_count;

    if(ready_other_luns_timeout_ns > 0) {
        puts("TEST9");
        while(1) {}
        for(uint8_t lun_pos = 0; lun_pos < lun_count; ++lun_pos) {
            if(lun_pos == this_lun_no) {
                continue;
            }

            if(! nand_onfi_wait_until_lun_ready(nand, lun_pos, ready_other_luns_timeout_ns)) {
                return false; /**< Other LUNs not ready but timeout */
            }
        }
    }

    if(ready_this_lun_timeout_ns > 0) {
        if(! nand_onfi_wait_until_lun_ready(nand, this_lun_no, ready_this_lun_timeout_ns)) {
            return false; /**< This LUN not ready but timeout */
        }
    }

    return true; /**< All LUNs ready */
}

bool nand_onfi_wait_until_lun_ready(const nand_onfi_t* const nand, const uint8_t this_lun_no, const uint32_t timeout_ns) {
    const uint32_t timeout_deadline = nand_onfi_deadline_from_interval(timeout_ns);
    uint32_t timeout_left = timeout_deadline;

    do {
        switch(this_lun_no) {
        case 0:
            if(gpio_read(nand->params.rb0))
            {
                return true;
            }
            break;
        case 1:
            if(gpio_read(nand->params.rb1))
            {
                return true;
            }
            break;
        case 2:
            if(gpio_read(nand->params.rb2))
            {
                return true;
            }
            break;
        case 3:
            if(gpio_read(nand->params.rb3))
            {
                return true;
            }
            break;
        }

        timeout_left = nand_onfi_deadline_left(timeout_deadline);
    } while(timeout_ns == 0 || timeout_left > 0);

    return false; /**< Not ready but timeout */
}

size_t nand_onfi_all_pages_count(const nand_onfi_t* const nand) {
    const size_t ret_size = nand->pages_per_block * nand->blocks_per_lun * nand->lun_count;
    return ret_size;
}

size_t nand_onfi_one_page_size(const nand_onfi_t* const nand) {
    const size_t ret_size = nand->data_bytes_per_page + nand->spare_bytes_per_page;

    return ret_size;
}

size_t nand_onfi_all_data_bytes_size(const nand_onfi_t* const nand) {
    const size_t ret_size = nand->data_bytes_per_page * nand_onfi_all_pages_count(nand);
    return ret_size;
}

size_t nand_onfi_all_spare_bytes_size(const nand_onfi_t* const nand) {
    const size_t ret_size = nand->spare_bytes_per_page * nand_onfi_all_pages_count(nand);
    return ret_size;
}

size_t nand_onfi_all_pages_size(const nand_onfi_t* const nand) {
    const size_t ret_size = nand_onfi_all_data_bytes_size(nand) + nand_onfi_all_spare_bytes_size(nand);
    return ret_size;
}

uint64_t nand_onfi_offset_to_addr_column(const uint64_t offset) {
    const uint64_t ret_addr = offset;
    return ret_addr;
}

uint64_t nand_onfi_page_num_to_addr_row(const uint64_t page_num) {
    const uint64_t ret_addr = page_num;
    return ret_addr;
}

uint64_t nand_onfi_addr_flat_to_addr_column(const nand_onfi_t* const nand, const uint64_t addr_flat) {
    const uint64_t ret_addr = addr_flat % nand_onfi_one_page_size(nand);
    return ret_addr;
}

uint64_t nand_onfi_addr_flat_to_addr_row(const nand_onfi_t* const nand, const uint64_t addr_flat) {
    const uint64_t ret_addr = addr_flat / nand_onfi_one_page_size(nand);
    return ret_addr;
}

uint64_t nand_onfi_addr_to_addr_flat(const nand_onfi_t* const nand, const uint64_t addr_row, const uint64_t addr_column) {
    const uint64_t ret_addr = addr_row * nand_onfi_one_page_size(nand) + addr_column;
    return ret_addr;
}

uint32_t nand_onfi_deadline_from_interval(const uint32_t interval_ns) {
    const uint32_t ret_time = ztimer_now(ZTIMER_USEC) + (interval_ns / NAND_ONFI_TIMING_MICROSEC(1));
    return ret_time;
}

uint32_t nand_onfi_deadline_left(const uint32_t deadline) {
    int32_t left = (int32_t)(deadline - ztimer_now(ZTIMER_USEC));
    const uint32_t ret_left = (left < 0) ? 0 : (uint32_t)left;
    return ret_left;
}
