#define ENABLE_DEBUG 0
#include "debug.h"
#include "mtd.h"
#include "mtd_nand.h"

#include <errno.h>

static int mtd_nand_init(mtd_dev_t *dev)
{
    DEBUG("mtd_nand_init\n");
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

    if((mtd_nand->nand->init_done == true) ||
        (nand_onfi_init(mtd_nand->nand, mtd_nand->params) == 0))
    {
        dev->sector_count = nand_onfi_all_pages_count(mtd_nand->nand);
        dev->page_size = nand_onfi_one_page_size(mtd_nand->nand);
        dev->pages_per_sector = mtd_nand->nand->pages_per_block; /**< NAND is intended to use one block per one access */

        return 0;
    }

    return -EIO;
}

static int mtd_nand_read_page(mtd_dev_t *dev, void *read_buffer, uint32_t page_num, uint32_t offset, uint32_t size)
{
    nand_onfi_rw_response_t err;
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

    DEBUG("mtd_nand_read_page: page:%" PRIu32 " offset:%" PRIu32 " size:%" PRIu32 "\n", page_num, offset, size);

    const uint32_t column_addr = nand_onfi_offset_to_column_addr(offset);
    const uint32_t row_addr = nand_onfi_page_num_to_row_addr(page_num);
    const nand_onfi_run_cmd_params_t cmd_params[] = {
        {},
        { .cycles = { .addr = { column_addr, row_addr } } },
        {},
        { .cycles = { .raw = { .buffer = read_buffer, .buffer_size = size } } }
    };

    size_t read_size = nand_onfi_run_cmd(mtd_nand->nand, NAND_ONFI_CMD_READ, cmd_params, &err);

    if(err != NAND_ONFI_RW_OK) {
        return -EIO;
    }

    return read_size;
}

static int mtd_nand_write_page(mtd_dev_t *dev, const void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

    return 0;
}

static int mtd_nand_erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

}

static int mtd_nand_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

    switch(power) {
    case MTD_POWER_UP:
        for(uint8_t lun = 0; lun < mtd_nand->nand->lun_count; ++lun) {
            nand_onfi_set_chip_enable(mtd_nand->nand, lun);
        }
        break;

    case MTD_POWER_DOWN:
        for(uint8_t lun = 0; lun < mtd_nand->nand->lun_count; ++lun) {
            nand_onfi_set_chip_disable(mtd_nand->nand, lun);
        }
        break;
    }

    return 0;
}

static int mtd_nand_read(mtd_dev_t *dev, void *read_buffer, uint32_t addr_flat, uint32_t size)
{
    nand_onfi_rw_response_t err;
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

    DEBUG("mtd_nand_read: addr_flat:%" PRIu32 " size:%" PRIu32 "\n", addr_flat, size);

    const uint32_t column_addr = nand_onfi_addr_flat_to_addr_column(mtd_nand->nand, addr_flat);
    const uint32_t row_addr = nand_onfi_addr_flat_to_addr_row(mtd_nand->nand, addr_flat);
    const nand_onfi_run_cmd_params_t cmd_params[] = {
        {},
        { .cycles = { .addr = { column_addr, row_addr } } },
        {},
        { .cycles = { .raw = { .buffer = read_buffer, .buffer_size = size, .raw_size = size } } }
    };

    size_t read_size = nand_onfi_run_cmd(mtd_nand->nand, NAND_ONFI_CMD_READ, cmd_params, &err);

    if(err != NAND_ONFI_RW_OK) {
        return -EIO;
    }

    return read_size;
}

static int mtd_nand_write(mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    mtd_nand_t *mtd_nand = (mtd_nand_t*)dev;

}

const mtd_desc_t mtd_nand_driver = {
    .init = mtd_nand_init,
    .read = mtd_nand_read,
    .read_page = mtd_nand_read_page,
    .write = mtd_nand_write,
    .write_page = mtd_nand_write_page,
    .erase_sector = mtd_nand_erase_sector,
    .power = mtd_nand_power,
};