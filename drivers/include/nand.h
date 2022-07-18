/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nand NAND common driver
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

#ifndef NAND_H
#define NAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t nand_extract_id_size(const uint16_t * const bytes_id, const size_t bytes_id_size, const size_t min_pattern_size);
bool nand_check_DDR(const uint16_t * const bytes, const size_t bytes_size);
size_t nand_fold_DDR_repeat_bytes(uint16_t * const bytes, const size_t bytes_size, const uint8_t filling_empty_byte);

#ifdef __cplusplus
}
#endif

#endif /* NAND_H */
/** @} */
