#ifndef NAND_ONFI_ENUMS_H
#define NAND_ONFI_ENUMS_H

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_ONFI_INIT_ERROR                        (-1)   /**< returned on failed init */
#define NAND_ONFI_OK                                (0)    /**< returned on successful init */

typedef enum {
    NAND_ONFI_RW_OK = 0,           /**< no error */
    NAND_ONFI_RW_TIMEOUT,          /**< cmd timed out (not-busy-state wasn't entered) */
    NAND_ONFI_RW_WRITE_ERROR,      /**< data-packet response indicates error */
    NAND_ONFI_RW_ECC_MISMATCH,     /**< CRC-mismatch of received data */
    NAND_ONFI_RW_NOT_SUPPORTED,    /**< operation not supported on used card */
    NAND_ONFI_RW_CMD_INVALID,
    NAND_ONFI_RW_CMD_CHAIN_TOO_LONG
} nand_onfi_rw_response_t;

typedef enum {
    NAND_ONFI_CMD_TYPE_CMD_WRITE,
    NAND_ONFI_CMD_TYPE_ADDR_WRITE,
    NAND_ONFI_CMD_TYPE_ADDR_COLUMN_WRITE,
    NAND_ONFI_CMD_TYPE_ADDR_ROW_WRITE,
    NAND_ONFI_CMD_TYPE_RAW_WRITE,
    NAND_ONFI_CMD_TYPE_RAW_READ
//    NAND_ONFI_CMD_TYPE_USE_ORIGINAL,
//    NAND_ONFI_CMD_TYPE_USE_OVERRIDE,
//    NAND_ONFI_CMD_TYPE_DONT_USE
} nand_onfi_cmd_type_t;

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

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_ENUMS_H */