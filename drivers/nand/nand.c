/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nand
 * @{
 *
 * @file
 * @brief       low level driver for accessing common NANDs
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include "nand.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

int nand_init(nand_t* const nand, nand_params_t* const params) {
    if(nand == NULL) {
        return NAND_INIT_ERROR;
    }

    if(params == NULL) {
        nand->init_done = false;
        return NAND_INIT_ERROR;
    }

    nand->params = *params;

    nand_set_pin_default(nand);

    nand->data_bus_width = 8;
    nand->addr_bus_width = 8;

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

    return NAND_OK;
}

size_t nand_write_addr_column(const nand_t* const nand, const uint64_t* const addr_column, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    const uint8_t column_addr_cycles = nand->column_addr_cycles;

    switch(nand->addr_bus_width) {
    case 8:
        {
            uint64_t mask = 0xFF;
            for(size_t seq = 0; seq < column_addr_cycles; ++seq) {
                uint8_t* const cycle_data = (uint8_t *)malloc(sizeof(uint8_t));
                *cycle_data = *addr_column & mask;
                ret_size += nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
                free(cycle_data);
                mask <<= 8;
            }
        }
        break;
    case 16:
        {
            uint64_t mask = 0xFF;
            for(size_t seq = 0; seq < column_addr_cycles; ++seq) {
                uint8_t* const cycle_data = (uint8_t *)malloc(sizeof(uint8_t) * 2);
                cycle_data[0] = *addr_column & mask;
                cycle_data[1] = *addr_column & (mask << 8);
                ret_size += nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
                free(cycle_data);
                mask <<= 16;
            }
        }
        break;
    }

    return ret_size;
}

size_t nand_write_addr_row(const nand_t* const nand, const uint64_t* const addr_row, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;
    const uint8_t row_addr_cycles = nand->row_addr_cycles;

    switch(nand->addr_bus_width) {
    case 8:
        {
            uint64_t mask = 0xFF;
            for(size_t seq = 0; seq < row_addr_cycles; ++seq) {
                uint8_t* const cycle_data = (uint8_t *)malloc(sizeof(uint8_t));
                *cycle_data = *addr_row & mask;
                ret_size += nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
                free(cycle_data);
                mask <<= 8;
            }
        }
        break;
    case 16:
        {
            uint64_t mask = 0xFF;
            for(size_t seq = 0; seq < row_addr_cycles; ++seq) {
                uint8_t* const cycle_data = (uint8_t *)malloc(sizeof(uint8_t) * 2);
                cycle_data[1] = *addr_row & (mask << 8);
                cycle_data[0] = *addr_row & mask;
                ret_size += nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
                free(cycle_data);
                mask <<= 16;
            }
        }
        break;
    }

    return ret_size;
}

size_t nand_write_addr_single(const nand_t* const nand, const uint16_t* const addr_single_cycle_data, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;

    switch(nand->addr_bus_width) {
    case 8:
        {
            ret_size = nand_write_cycle(nand, (uint8_t*)addr_single_cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
        }
        break;
    case 16:
        {
            uint8_t* const cycle_data = (uint8_t *)malloc(sizeof(uint8_t) * 2);
            cycle_data[1] = (*addr_single_cycle_data & 0xFF00) >> 8;
            cycle_data[0] = *addr_single_cycle_data & 0xFF;
            ret_size = nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
            free(cycle_data);
        }
        break;
    }

    return ret_size;
}

size_t nand_write_raw(const nand_t* const nand, const uint8_t* const data, const size_t data_size, const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_size = 0;

    size_t seq = 0;
    while(seq + 1 < data_size) {
        ret_size += nand_write_cycle(nand, &(data[seq]), cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);

        ++seq;
        if(nand->data_bus_width == 16) {
            ++seq;
        }
    }
    if(seq + 1 == data_size) {
        uint8_t* const cycle_data = (uint8_t*)malloc(sizeof(uint8_t) * 2);
        cycle_data[1] = 0x00;
        cycle_data[0] = data[seq];
        ret_size += nand_write_cycle(nand, cycle_data, cycle_write_enable_post_delay_ns, cycle_write_disable_post_delay_ns);
        free(cycle_data);
    }

    return ret_size;
}

size_t nand_write_io(const nand_t* const nand, const uint8_t data[2], const uint32_t cycle_write_enable_post_delay_ns, const uint32_t cycle_write_disable_post_delay_ns) {
    size_t ret_len = 0;

    nand_set_write_enable(nand);

    if(cycle_write_enable_post_delay_ns > 0) {
        nand_wait(cycle_write_enable_post_delay_ns);
    }

    if(nand->data_bus_width == 16) {
        gpio_write(nand->params.io15, (data[1] & NAND_MSB7) ? 1 : 0);
        gpio_write(nand->params.io14, (data[1] & NAND_MSB6) ? 1 : 0);
        gpio_write(nand->params.io13, (data[1] & NAND_MSB5) ? 1 : 0);
        gpio_write(nand->params.io12, (data[1] & NAND_MSB4) ? 1 : 0);
        gpio_write(nand->params.io11, (data[1] & NAND_MSB3) ? 1 : 0);
        gpio_write(nand->params.io10, (data[1] & NAND_MSB2) ? 1 : 0);
        gpio_write(nand->params.io9, (data[1] & NAND_MSB1) ? 1 : 0);
        gpio_write(nand->params.io8, (data[1] & NAND_MSB0) ? 1 : 0);
        ++ret_len;
    }

    gpio_write(nand->params.io7, (data[0] & NAND_MSB7) ? 1 : 0);
    gpio_write(nand->params.io6, (data[0] & NAND_MSB6) ? 1 : 0);
    gpio_write(nand->params.io5, (data[0] & NAND_MSB5) ? 1 : 0);
    gpio_write(nand->params.io4, (data[0] & NAND_MSB4) ? 1 : 0);
    gpio_write(nand->params.io3, (data[0] & NAND_MSB3) ? 1 : 0);
    gpio_write(nand->params.io2, (data[0] & NAND_MSB2) ? 1 : 0);
    gpio_write(nand->params.io1, (data[0] & NAND_MSB1) ? 1 : 0);
    gpio_write(nand->params.io0, (data[0] & NAND_MSB0) ? 1 : 0);
    ++ret_len;

    nand_set_write_disable(nand);

    if(cycle_write_disable_post_delay_ns > 0) {
        nand_wait(cycle_write_disable_post_delay_ns);
    }

    return ret_len;
}

size_t nand_read_raw(const nand_t* const nand, uint8_t* const out_buffer, const size_t buffer_size, const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    size_t ret_size = 0;

    size_t seq = 0;
    while(seq + 1 < buffer_size) {
        ret_size += nand_read_cycle(nand, &(out_buffer[seq]), cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);

        ++seq;
        if(nand->data_bus_width == 16) {
            ++seq;
        }
    }
    if(seq + 1 == buffer_size) {
        uint8_t* const cycle_data = (uint8_t*)malloc(sizeof(uint8_t) * 2);
        ret_size += nand_read_cycle(nand, cycle_data, cycle_read_enable_post_delay_ns, cycle_read_disable_post_delay_ns);
        out_buffer[seq] = cycle_data[0];
        free(cycle_data);
    }

    return ret_size;
}

size_t nand_read_io(const nand_t* const nand, uint8_t out_data[2], const uint32_t cycle_read_enable_post_delay_ns, const uint32_t cycle_read_disable_post_delay_ns) {
    size_t ret_len = 0;

    nand_set_read_enable(nand);

    if(cycle_read_enable_post_delay_ns > 0) {
        nand_wait(cycle_read_enable_post_delay_ns);
    }

    if(nand->data_bus_width == 16) {
        out_data[1] = 0;
        const bool io15 = gpio_read(nand->params.io15) != 0;
        const bool io14 = gpio_read(nand->params.io14) != 0;
        const bool io13 = gpio_read(nand->params.io13) != 0;
        const bool io12 = gpio_read(nand->params.io12) != 0;
        const bool io11 = gpio_read(nand->params.io11) != 0;
        const bool io10 = gpio_read(nand->params.io10) != 0;
        const bool io9 = gpio_read(nand->params.io9) != 0;
        const bool io8 = gpio_read(nand->params.io8) != 0;
        out_data[1] = (io15 << 7) | (io14 << 6) | (io13 << 5) | (io12 << 4) | (io11 << 3) | (io10 << 2) | (io9 << 1) | io8;
        ++ret_len;
    }

    out_data[0] = 0;
    const bool io7 = gpio_read(nand->params.io7) != 0;
    const bool io6 = gpio_read(nand->params.io6) != 0;
    const bool io5 = gpio_read(nand->params.io5) != 0;
    const bool io4 = gpio_read(nand->params.io4) != 0;
    const bool io3 = gpio_read(nand->params.io3) != 0;
    const bool io2 = gpio_read(nand->params.io2) != 0;
    const bool io1 = gpio_read(nand->params.io1) != 0;
    const bool io0 = gpio_read(nand->params.io0) != 0;
    out_data[0] = (io7 << 7) | (io6 << 6) | (io5 << 5) | (io4 << 4) | (io3 << 3) | (io2 << 2) | (io1 << 1) | io0;
    ++ret_len;

    nand_set_read_disable(nand);

    if(cycle_read_disable_post_delay_ns > 0) {
        nand_wait(cycle_read_disable_post_delay_ns);
    }

    return ret_len;
}

void nand_set_ctrl_pin(const nand_t* const nand) {
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

void nand_set_io_pin_write(const nand_t* const nand) {
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

void nand_set_io_pin_read(const nand_t* const nand) {
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

void nand_wait(const uint32_t delay_ns) {
    if(delay_ns != 0) {
        /* TODO: ztimer_sleep not working */
        //ztimer_sleep(ZTIMER_USEC, delay_ns / NAND_TIMING_MICROSEC(1));

        const uint32_t delay_deadline = nand_deadline_from_interval(delay_ns);
        uint32_t delay_left = delay_deadline;

        do {
            delay_left = nand_deadline_left(delay_deadline);
        } while(delay_left > 0);
    }
}

bool nand_wait_until_ready(const nand_t* const nand, const uint8_t this_lun_no, const uint32_t ready_this_lun_timeout_ns, const uint32_t ready_other_luns_timeout_ns) {
    const uint8_t lun_count = nand->lun_count;

    if(ready_other_luns_timeout_ns > 0) {
        for(uint8_t lun_pos = 0; lun_pos < lun_count; ++lun_pos) {
            if(lun_pos == this_lun_no) {
                continue;
            }

            if(! nand_wait_until_lun_ready(nand, lun_pos, ready_other_luns_timeout_ns)) {
                return false; /**< Other LUNs not ready but timeout */
            }
        }
    }

    if(ready_this_lun_timeout_ns > 0) {
        if(! nand_wait_until_lun_ready(nand, this_lun_no, ready_this_lun_timeout_ns)) {
            return false; /**< This LUN not ready but timeout */
        }
    }

    return true; /**< All LUNs ready */
}

bool nand_wait_until_lun_ready(const nand_t* const nand, const uint8_t this_lun_no, const uint32_t timeout_ns) {
    const uint32_t timeout_deadline = nand_deadline_from_interval(timeout_ns);
    uint32_t timeout_left = timeout_deadline;

    do {
        switch(this_lun_no) {
        case 0:
            {
                if(gpio_read(nand->params.rb0))
                {
                    return true;
                }
            }
            break;
        case 1:
            {
                if(gpio_read(nand->params.rb1))
                {
                    return true;
                }
            }
            break;
        case 2:
            {
                if(gpio_read(nand->params.rb2))
                {
                    return true;
                }
            }
            break;
        case 3:
            {
                if(gpio_read(nand->params.rb3))
                {
                    return true;
                }
            }
            break;
        }

        timeout_left = nand_deadline_left(timeout_deadline);
    } while(timeout_ns == 0 || timeout_left > 0);

    return false; /**< Not ready but timeout */
}

size_t nand_extract_id(uint8_t* const bytes_id, const size_t bytes_id_size) {
    const bool   is_DDR      = nand_check_DDR(bytes_id, bytes_id_size);
    const size_t folded_size = is_DDR ? nand_fold_DDR_repeat_bytes(bytes_id, bytes_id_size, 0x00) : bytes_id_size;
    const size_t id_size     = nand_extract_id_size(bytes_id, folded_size, NAND_MIN_ID_SIZE);

    for(size_t pos = id_size; pos < bytes_id_size; ++pos) {
        bytes_id[pos] = 0x00;
    }

    return id_size;
}

size_t nand_extract_id_size(const uint8_t * const bytes_id, const size_t bytes_id_size, const size_t min_pattern_size)
{
    if (bytes_id_size < 1)
    {
        return bytes_id_size;
    }

    uint8_t pattern[NAND_MAX_ID_SIZE] = { bytes_id[0] };
    size_t pattern_size = 1;

    for (size_t pos = 1; pos < min_pattern_size && pos < bytes_id_size; ++pos)
    {
        pattern[pos] = bytes_id[pos];
        ++pattern_size;
    }

    while(pattern_size < bytes_id_size)
    {
        bool isin_pattern = true;

        for(size_t start_pos = pattern_size; start_pos < bytes_id_size; start_pos += pattern_size)
        {
            size_t end_pos = (start_pos + pattern_size < bytes_id_size) ? start_pos + pattern_size : bytes_id_size;
            size_t cmp_pattern_pos = 0;
            size_t cmp_id_pos = start_pos;

            while(cmp_id_pos < end_pos)
            {
                if(pattern[cmp_pattern_pos] != bytes_id[cmp_id_pos])
                {
                    isin_pattern = false;
                    break;
                }

                ++cmp_pattern_pos;
                ++cmp_id_pos;
            }

            if(! isin_pattern)
            {
                break;
            }
        }

        if(! isin_pattern)
        {
            pattern[pattern_size] = bytes_id[pattern_size];
            ++pattern_size;
        }
        else
        {
            break;
        }
    }

    return pattern_size;
}

bool nand_check_DDR(const uint8_t * const bytes, const size_t bytes_size)
{
    if(bytes_size < 1)
    {
        return false;
    }

    size_t pos = 0;

    while(pos + 1 < bytes_size)
    {
        if(bytes[pos] != bytes[pos + 1])
        {
            return false;
        }
        pos += 2;
    }

    return true;
}

/** Fold the DDR repeat bytes to SDR bytes */
size_t nand_fold_DDR_repeat_bytes(uint8_t * const bytes, const size_t bytes_size, const uint8_t filling_empty_byte)
{
    /**
       ONFI 5.0
       4.4. NV-DDR / NV-DDR2 / NV-DDR3 / NV-LPDDR4 and Repeat Bytes
       The commands that repeat each data byte twice in the NV-DDR, NV-DDR2, NV-DDR3 and NV- LPDDR4 data interfaces are:
            Set Features, Read ID, Get Features, Read Status, Read Status Enhanced, and ODT Configure
    */
    size_t pos = 0;
    size_t size_folded = 0;

    while(pos < bytes_size / 2)
    {
        // Fold the DDR repeat bytes to SDR bytes
        bytes[pos] = bytes[pos * 2];
        ++pos;
    }

    if(pos * 2 + 1 == bytes_size)
    {
        // Copy last byte if `bytes_size` is odd
        bytes[pos] = bytes[pos * 2];
        ++pos;
    }

    size_folded = pos;

    while(pos < bytes_size)
    {
        // Fill the rest bytes with `empty_byte`
        bytes[pos] = filling_empty_byte;
        ++pos;
    }

    return size_folded;
}
