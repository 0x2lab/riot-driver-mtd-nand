#ifndef NAND_ONFI_CMD_H
#define NAND_ONFI_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "nand/onfi/types.h"
#include "nand/onfi/enums.h"
#include "nand/onfi/cmd_timing.h"

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ = {
    .chains_length = 4,
    .chains = {
        {
            .cycles_defined             = true,
            .timings                    = NAND_ONFI_CMD_TIMING_CMD_WRITE_READY_THIS_LUN,
            .cycles_type                = NAND_ONFI_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x00 },
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_ONFI_CMD_TIMING_ADDR_WRITE,
            .cycles_type                = NAND_ONFI_CMD_TYPE_ADDR_WRITE,
        },
        {
            .cycles_defined             = true,
            .timings                    = NAND_ONFI_CMD_TIMING_CMD_WRITE_POST_DELAY,
            .cycles_type                = NAND_ONFI_CMD_TYPE_CMD_WRITE,
            .cycles                     = { .cmd = 0x30 }
        },
        {
            .cycles_defined             = false,
            .timings                    = NAND_ONFI_CMD_TIMING_RAW_READ,
            .cycles_type                = NAND_ONFI_CMD_TYPE_RAW_READ
        }
    }
};

static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_ID = {
        .chains_length = 3,
        .chains = {
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_CMD_WRITE,
                .cycles_type            = NAND_ONFI_CMD_TYPE_CMD_WRITE,
                .cycles                 = { .cmd = 0x90 }
            },
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_ADDR_WRITE_POST_DELAY,
                .cycles_type            = NAND_ONFI_CMD_TYPE_ADDR_ROW_SINGLE_WRITE,
                .cycles                 = { .addr_row = 0x00 }
            },
            {
                .cycles_defined         = false,
                .timings                = NAND_ONFI_CMD_TIMING_RAW_READ,
                .cycles_type            = NAND_ONFI_CMD_TYPE_RAW_READ
            }
        }
    };


static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_ID_ONFI_SIG = {
        .chains_length = 3,
        .chains = {
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_CMD_WRITE,
                .cycles_type            = NAND_ONFI_CMD_TYPE_CMD_WRITE,
                .cycles                 = { .cmd = 0x90 }
            },
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_ADDR_WRITE_POST_DELAY,
                .cycles_type            = NAND_ONFI_CMD_TYPE_ADDR_ROW_SINGLE_WRITE,
                .cycles                 = { .addr_row = 0x20 }
            },
            {
                .cycles_defined         = false,
                .timings                = NAND_ONFI_CMD_TIMING_RAW_READ,
                .cycles_type            = NAND_ONFI_CMD_TYPE_RAW_READ
            }
        }
    };


static const nand_onfi_cmd_t NAND_ONFI_CMD_READ_PARAMETER_PAGE = {
        .chains_length = 3,
        .chains = {
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_CMD_WRITE,
                .cycles_type            = NAND_ONFI_CMD_TYPE_CMD_WRITE,
                .cycles                 = { .cmd = 0xEC }
            },
            {
                .cycles_defined         = true,
                .timings                = NAND_ONFI_CMD_TIMING_ADDR_WRITE_POST_DELAY,
                .cycles_type            = NAND_ONFI_CMD_TYPE_ADDR_ROW_SINGLE_WRITE,
                .cycles                 = { .addr_row = 0x00 }
            },
            {
                .cycles_defined         = false,
                .timings                = NAND_ONFI_CMD_TIMING_RAW_READ,
                .cycles_type            = NAND_ONFI_CMD_TYPE_RAW_READ
            }
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


#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_CMD_H */