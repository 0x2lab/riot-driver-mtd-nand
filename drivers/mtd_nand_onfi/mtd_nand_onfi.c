#define ENABLE_DEBUG 0
#include "debug.h"

#include "mtd_nand_onfi.h"
#include "nand.h"
#include "nand_cmd.h"
#include "nand/onfi.h"
#include "mtd.h"

#include <stdlib.h>
#include <errno.h>

static int mtd_nand_onfi_init(mtd_dev_t* const dev)
{
    if(dev == NULL) {
        return -ENODEV;
    }

    mtd_nand_onfi_t*    const mtd_nand  = (mtd_nand_onfi_t*)dev;
    if(mtd_nand->nand_onfi == NULL) {
        return -ENODEV;
    }

    nand_t*             const nand      = (nand_t*)&(mtd_nand->nand_onfi);
    if(mtd_nand->params != NULL && ! nand->init_done && nand_onfi_init(mtd_nand->nand_onfi, mtd_nand->params) != NAND_INIT_OK) {
        return -EIO;
    }

    if(! nand->init_done) {
        return -EIO;
    }

    dev->sector_count       = nand_onfi_all_block_count(nand);
    dev->page_size          = nand_onfi_one_page_size(mtd_nand->nand_onfi);
    dev->pages_per_sector   = nand->pages_per_block;    /**< NAND is intended to use one block per one access */

    return 0;
}

static int mtd_nand_onfi_read(mtd_dev_t* const dev, void* const read_buffer, const uint32_t addr_flat, const uint32_t size)
{
          mtd_nand_onfi_t *   const mtd_nand            = (mtd_nand_onfi_t*)dev;
          nand_t*             const nand                = (nand_t*)&(mtd_nand->nand_onfi);
    const uint64_t                  addr_column         = nand_addr_flat_to_addr_column(nand, addr_flat);
    const uint64_t                  addr_row            = nand_addr_flat_to_addr_row(nand, addr_flat);
    const uint8_t                   lun_no              = addr_flat / nand_one_lun_pages_size(nand); // TODO: lun_no looks invalid

          nand_rw_response_t* const err                 = (nand_rw_response_t *)malloc(sizeof(nand_rw_response_t));

          nand_raw_t*         const raw_store           = (nand_raw_t*)malloc(sizeof(nand_raw_t));
                raw_store->raw_size                     = size;
                raw_store->buffer                       = read_buffer;
                raw_store->buffer_size                  = size; // TODO: Should throw error if size is too large
                raw_store->current_buffer_seq           = 0;
                raw_store->current_raw_offset           = 0;

          nand_cmd_t*         const cmd_mutable         = (nand_cmd_t*)malloc(sizeof(nand_cmd_t));
                memcpy(cmd_mutable, &NAND_ONFI_CMD_READ, sizeof(nand_cmd_t));
                cmd_mutable->chains[1].cycles_defined   = true;
                cmd_mutable->chains[1].cycles.addr[0]   = addr_column;
                cmd_mutable->chains[1].cycles.addr[1]   = addr_row;
                cmd_mutable->chains[3].cycles_defined   = true;
                cmd_mutable->chains[3].cycles.raw       = raw_store;

          nand_cmd_params_t*  const cmd_params          = (nand_cmd_params_t*)malloc(sizeof(nand_cmd_params_t));
                cmd_params->lun_no                      = lun_no;
                cmd_params->cmd_override                = cmd_mutable;

    nand_run_cmd_chains(nand, &NAND_ONFI_CMD_READ, cmd_params, err);

    free(cmd_params);
    free(cmd_mutable);
    free(raw_store);

    if(*err != NAND_RW_OK) {
        free(err);
        return -EIO;
    }

    free(err);
    return size;
}

static int mtd_nand_onfi_read_page(mtd_dev_t* const dev, void* const read_buffer, const uint32_t page_no, const uint32_t offset, const uint32_t size)
{
          mtd_nand_onfi_t *   const mtd_nand            = (mtd_nand_onfi_t*)dev;
          nand_t*             const nand                = (nand_t*)&(mtd_nand->nand_onfi);
    const uint64_t                  addr_column         = nand_offset_to_addr_column(offset);
    const uint64_t                  addr_row            = nand_page_no_to_addr_row(page_no);
    const uint8_t                   lun_no              = page_no / nand_one_lun_pages_count(nand); // TODO: lun_no looks invalid
    const size_t                    page_size           = nand_one_page_size(nand);
    const size_t                    raw_size            = (size < page_size) ? size : page_size; // TODO: Should throw error if size > page_size

          nand_rw_response_t* const err                 = (nand_rw_response_t *)malloc(sizeof(nand_rw_response_t));

          nand_raw_t*         const raw_store           = (nand_raw_t*)malloc(sizeof(nand_raw_t));
                raw_store->raw_size                     = raw_size;
                raw_store->buffer                       = read_buffer;
                raw_store->buffer_size                  = size;
                raw_store->current_buffer_seq           = 0;
                raw_store->current_raw_offset           = 0;

          nand_cmd_t*         const cmd_mutable         = (nand_cmd_t*)malloc(sizeof(nand_cmd_t));
                memcpy(cmd_mutable, &NAND_ONFI_CMD_READ, sizeof(nand_cmd_t));
                cmd_mutable->chains[1].cycles_defined   = true;
                cmd_mutable->chains[1].cycles.addr[0]   = addr_column;
                cmd_mutable->chains[1].cycles.addr[1]   = addr_row;
                cmd_mutable->chains[3].cycles_defined   = true;
                cmd_mutable->chains[3].cycles.raw       = raw_store;

          nand_cmd_params_t*  const cmd_params          = (nand_cmd_params_t*)malloc(sizeof(nand_cmd_params_t));
                cmd_params->lun_no                      = lun_no;
                cmd_params->cmd_override                = cmd_mutable;

    nand_run_cmd_chains(nand, &NAND_ONFI_CMD_READ, cmd_params, err);

    free(cmd_params);
    free(cmd_mutable);
    free(raw_store);

    if(*err != NAND_RW_OK) {
        free(err);
        return -EIO;
    }

    free(err);
    return raw_size;
}

static int mtd_nand_onfi_write(mtd_dev_t* const dev, const void* const write_buffer, const uint32_t addr_flat, const uint32_t size)
{
          mtd_nand_onfi_t *   const mtd_nand            = (mtd_nand_onfi_t*)dev;
          nand_t*             const nand                = (nand_t*)&(mtd_nand->nand_onfi);
    const uint64_t                  addr_column         = nand_addr_flat_to_addr_column(nand, addr_flat);
    const uint64_t                  addr_row            = nand_addr_flat_to_addr_row(nand, addr_flat);
    const uint8_t                   lun_no              = addr_flat / nand_one_lun_pages_size(nand); // TODO: lun_no looks invalid

          nand_rw_response_t* const err                 = (nand_rw_response_t *)malloc(sizeof(nand_rw_response_t));

          nand_raw_t*         const raw_store           = (nand_raw_t*)malloc(sizeof(nand_raw_t));
                raw_store->raw_size                     = size;
                raw_store->buffer                       = write_buffer;
                raw_store->buffer_size                  = size;
                raw_store->current_buffer_seq           = 0;
                raw_store->current_raw_offset           = 0;

          nand_cmd_t*         const cmd_mutable         = (nand_cmd_t*)malloc(sizeof(nand_cmd_t));
                memcpy(cmd_mutable, &NAND_ONFI_CMD_PAGE_PROGRAM, sizeof(nand_cmd_t));
                cmd_mutable->chains[1].cycles_defined   = true;
                cmd_mutable->chains[1].cycles.addr[0]   = addr_column;
                cmd_mutable->chains[1].cycles.addr[1]   = addr_row;
                cmd_mutable->chains[3].cycles_defined   = true;
                cmd_mutable->chains[3].cycles.raw       = raw_store;

          nand_cmd_params_t*  const cmd_params          = (nand_cmd_params_t*)malloc(sizeof(nand_cmd_params_t));
                cmd_params->lun_no                      = lun_no;
                cmd_params->cmd_override                = cmd_mutable;

    nand_run_cmd_chains(nand, &NAND_ONFI_CMD_PAGE_PROGRAM, cmd_params, err);

    free(cmd_params);
    free(cmd_mutable);
    free(raw_store);

    if(*err != NAND_RW_OK) {
        free(err);
        return -EIO;
    }

    free(err);
    return size;

}

static int mtd_nand_onfi_write_page(mtd_dev_t * const dev, const void * const write_buffer, const uint32_t page_no, const uint32_t offset, const uint32_t size)
{
          mtd_nand_onfi_t *   const mtd_nand            = (mtd_nand_onfi_t*)dev;
          nand_t*             const nand                = (nand_t*)&(mtd_nand->nand_onfi);
    const uint64_t                  addr_column         = nand_offset_to_addr_column(offset);
    const uint64_t                  addr_row            = nand_page_no_to_addr_row(page_no);
    const uint8_t                   lun_no              = page_no / nand_one_lun_pages_count(nand); // TODO: lun_no looks invalid
    const size_t                    page_size           = nand_one_page_size(nand);
    const size_t                    raw_size            = (size < page_size) ? size : page_size; // TODO: Should throw error if size > page_size

          nand_rw_response_t* const err                 = (nand_rw_response_t *)malloc(sizeof(nand_rw_response_t));

          nand_raw_t*         const raw_store           = (nand_raw_t*)malloc(sizeof(nand_raw_t));
                raw_store->raw_size                     = raw_size;
                raw_store->buffer                       = write_buffer;
                raw_store->buffer_size                  = raw_size;
                raw_store->current_buffer_seq           = 0;
                raw_store->current_raw_offset           = 0;

          nand_cmd_t*         const cmd_mutable         = (nand_cmd_t*)malloc(sizeof(nand_cmd_t));
                memcpy(cmd_mutable, &NAND_ONFI_CMD_PAGE_PROGRAM, sizeof(nand_cmd_t));
                cmd_mutable->chains[1].cycles_defined   = true;
                cmd_mutable->chains[1].cycles.addr[0]   = addr_column;
                cmd_mutable->chains[1].cycles.addr[1]   = addr_row;
                cmd_mutable->chains[3].cycles_defined   = true;
                cmd_mutable->chains[3].cycles.raw       = raw_store;

          nand_cmd_params_t*  const cmd_params          = (nand_cmd_params_t*)malloc(sizeof(nand_cmd_params_t));
                cmd_params->lun_no                      = lun_no;
                cmd_params->cmd_override                = cmd_mutable;

    nand_run_cmd_chains(nand, &NAND_ONFI_CMD_PAGE_PROGRAM, cmd_params, err);

    free(cmd_params);
    free(cmd_mutable);
    free(raw_store);

    if(*err != NAND_RW_OK) {
        free(err);
        return -EIO;
    }

    free(err);
    return raw_size;
}

static int mtd_nand_onfi_erase(mtd_dev_t* const dev, const uint32_t addr_flat, const uint32_t count) {
    return -ENOTSUP;
}

static int mtd_nand_onfi_erase_block(mtd_dev_t* const dev, const uint32_t block_no, const uint32_t count)
{
    mtd_nand_onfi_t *   const mtd_nand  = (mtd_nand_onfi_t*)dev;
    nand_t*             const nand      = (nand_t*)&(mtd_nand->nand_onfi);
    nand_rw_response_t* const err       = (nand_rw_response_t *)malloc(sizeof(nand_rw_response_t));

    for(uint32_t erasure_pos = block_no; erasure_pos < block_no + count; ++erasure_pos) {
        const uint64_t                  addr_row            = nand_page_no_to_addr_row(erasure_pos);
        const uint8_t                   lun_no              = erasure_pos / nand->blocks_per_lun; // TODO: lun_no looks invalid

              nand_cmd_t*         const cmd_mutable         = (nand_cmd_t*)malloc(sizeof(nand_cmd_t));
                    memcpy(cmd_mutable, &NAND_ONFI_CMD_BLOCK_ERASE, sizeof(nand_cmd_t));
                    cmd_mutable->chains[1].cycles_defined   = true;
                    cmd_mutable->chains[1].cycles.addr_row  = addr_row;

              nand_cmd_params_t*  const cmd_params          = (nand_cmd_params_t*)malloc(sizeof(nand_cmd_params_t));
                    cmd_params->lun_no                      = lun_no;
                    cmd_params->cmd_override                = cmd_mutable;

        nand_run_cmd_chains(nand, &NAND_ONFI_CMD_BLOCK_ERASE, cmd_params, err);

        free(cmd_params);
        free(cmd_mutable);

        if(*err != NAND_RW_OK) {
            free(err);
            return -EIO;
        }
    }

    free(err);
    return count;
}

static int mtd_nand_onfi_power(mtd_dev_t *dev, enum mtd_power_state power)
{
    mtd_nand_onfi_t *   const mtd_nand  = (mtd_nand_onfi_t*)dev;
    nand_t*             const nand      = (nand_t*)&(mtd_nand->nand_onfi);

    switch(power) {
    case MTD_POWER_UP:
        for(uint8_t this_lun_no = 0; this_lun_no < nand->lun_count; ++this_lun_no) {
            nand_set_chip_enable(nand, this_lun_no);
        }
        break;

    case MTD_POWER_DOWN:
        for(uint8_t this_lun_no = 0; this_lun_no < nand->lun_count; ++this_lun_no) {
            nand_onfi_set_chip_disable(nand, this_lun_no);
        }
        break;
    }

    return 0;
}

const mtd_desc_t mtd_nand_driver = {
    .init           = mtd_nand_onfi_init,
    .read           = mtd_nand_onfi_read,
    .read_page      = mtd_nand_onfi_read_page,
    .write          = mtd_nand_onfi_write,
    .write_page     = mtd_nand_onfi_write_page,
    .erase          = mtd_nand_onfi_erase,
    .erase_sector   = mtd_nand_onfi_erase_block,
    .power          = mtd_nand_onfi_power,
};