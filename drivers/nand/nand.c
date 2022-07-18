#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t nand_extract_id_size(const uint16_t * const bytes_id, const size_t bytes_id_size, const size_t min_pattern_size)
{
    if (bytes_id_size < 1)
    {
        return bytes_id_size;
    }

    uint16_t pattern[20] = { bytes_id[0] };
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

bool nand_check_DDR(const uint16_t * const bytes, const size_t bytes_size)
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
size_t nand_fold_DDR_repeat_bytes(uint16_t * const bytes, const size_t bytes_size, const uint8_t filling_empty_byte)
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
        bytes[pos] = (uint16_t)filling_empty_byte;
        ++pos;
    }

    return size_folded;
}