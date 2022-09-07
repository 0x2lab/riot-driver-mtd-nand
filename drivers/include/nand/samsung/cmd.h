#ifndef NAND_SAMSUNG_CMD_H
#define NAND_SAMSUNG_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "nand_cmd.h"
#include "nand/samsung/cmd_timing.h"

static const nand_cmd_t NAND_SAMSUNG_CMD_READ = {
    .chains_length = 4,
    .chains = {
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_THIS_LUN,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x00 },
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE,
            .cycles_type                = NAND_CMD_TYPE_ADDR_WRITE,
        },
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x30 }
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_RAW_READ,
            .cycles_type                = NAND_CMD_TYPE_RAW_READ
        }
    }
};

static const nand_cmd_t NAND_SAMSUNG_CMD_READ_ID = {
        .chains_length = 3,
        .chains = {
            {
                .cycles_defined         = true,
                .timings                = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE,
                .cycles_type            = NAND_CMD_TYPE_CMD_WRITE,
                .cycles                 = { .cmd = 0x90 }
            },
            {
                .cycles_defined         = true,
                .timings                = NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE_POST_DELAY,
                .cycles_type            = NAND_CMD_TYPE_ADDR_SINGLE_WRITE,
                .cycles                 = { .addr_single = 0x00 }
            },
            {
                .cycles_defined         = false,
                .timings                = NAND_SAMSUNG_CMD_TIMING_RAW_READ,
                .cycles_type            = NAND_CMD_TYPE_RAW_READ
            }
        }
    };


static const nand_cmd_t NAND_SAMSUNG_CMD_PAGE_PROGRAM = {
    .chains_length = 4,
    .chains = {
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_THIS_LUN,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x80 },
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE,
            .cycles_type                = NAND_CMD_TYPE_ADDR_WRITE,
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_RAW_WRITE,
            .cycles_type                = NAND_CMD_TYPE_RAW_WRITE
        },
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x10 }
        }
    }
};

static const nand_cmd_t NAND_SAMSUNG_CMD_BLOCK_ERASE = {
    .chains_length = 3,
    .chains = {
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_THIS_LUN,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x60 },
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE,
            .cycles_type                = NAND_CMD_TYPE_ADDR_ROW_WRITE,
        },
        {
            .cycles_defined             = true,
            .timings                    = NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY,
            .cycles_type                = NAND_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0xD0 }
        }
    }
};

#ifdef __cplusplus
}
#endif

#endif /* NAND_SAMSUNG_CMD_H */