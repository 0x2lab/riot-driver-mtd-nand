#ifndef NAND_ONFI_CONSTANTS_H
#define NAND_ONFI_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_ONFI_MAX_CHIPS                         (8)

#define NAND_ONFI_MAX_COMMAND_SIZE                  (2)
#define NAND_ONFI_MAX_COMMAND_CYCLE_SIZE            (10)

#define NAND_ONFI_MAX_ID_SIZE                       (20)

#define NAND_ONFI_MAX_UNIQUE_ID_SIZE                (512)
#define NAND_ONFI_MAX_PARAMETER_PAGE_SIZE           (8192 + 256)      /**< ONFI states standard as 0-767 */

#define NAND_ONFI_MAX_IO_BITS                       (16)

#define NAND_ONFI_MAX_ADDR_COLUMN_CYCLES            (10)
#define NAND_ONFI_MAX_ADDR_ROW_CYCLES               (10)

#define NAND_ONFI_ADDR_INDEX_COLUMN                 (0)
#define NAND_ONFI_ADDR_INDEX_ROW                    (1)
#define NAND_ONFI_ADDR_INDEX_ALL                    (2)
#define NAND_ONFI_ADDR_IO_BITS                      (8)



#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_CONSTANTS_H */