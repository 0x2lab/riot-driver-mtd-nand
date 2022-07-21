/*
 * Copyright (C) 2022 Jongmin Kim <jmkim@pukyong.ac.kr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_nand_onfi mtd wrapper for nand_onfi
 * @ingroup     drivers_storage
 * @brief       Driver for ONFI NANDs using mtd interface
 *
 * @{
 *
 * @file
 * @brief       Interface definition for mtd_nand driver
 *
 * @author      Jongmin Kim <jmkim@pukyong.ac.kr>
 */

#ifndef MTD_NAND_ONFI_H
#define MTD_NAND_ONFI_H

#include "nand.h"
#include "nand_cmd.h"
#include "nand/onfi.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd_nand_onfi device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                 /**< inherit from mtd_dev_t object */
    nand_onfi_t* nand_onfi;         /**< nand_onfi dev descriptor */
    const nand_params_t* params;    /**< params for nand_onfi init */
} mtd_nand_onfi_t;

/**
 * @brief   nand device operations table for mtd
 */
extern const mtd_desc_t mtd_nand_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_NAND_ONFI_H */
/** @} */
