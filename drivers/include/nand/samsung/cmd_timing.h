#ifndef NAND_SAMSUNG_CMD_TIMING_H
#define NAND_SAMSUNG_CMD_TIMING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nand/samsung/timing.h"

#define NAND_SAMSUNG_CMD_TIMING_IGNORE {                                 \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE {                              \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_THIS_LUN {               \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_OTHER_LUNS {             \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_READY_ALL_LUNS {               \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY {                   \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB         \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY_READY_THIS_LUN {    \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB         \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY_READY_OTHER_LUNS {  \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB         \
}

#define NAND_SAMSUNG_CMD_TIMING_CMD_WRITE_POST_DELAY_READY_ALL_LUNS {    \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_CLS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_INFINITY , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_CLH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_CLS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB - (NAND_SAMSUNG_TIMING_CLH + NAND_SAMSUNG_TIMING_CLS) \
}

#define NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE {                             \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_ALH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_ALS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_WH       , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_ALH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_ALS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_ADDR_WRITE_POST_DELAY {                  \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_ALH      , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_ALS      , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_WH       , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_ALH      , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_ALS      , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB - (NAND_SAMSUNG_TIMING_ALH + NAND_SAMSUNG_TIMING_ALS) \
}

#define NAND_SAMSUNG_CMD_TIMING_RAW_WRITE {                              \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_WH       , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#define NAND_SAMSUNG_CMD_TIMING_RAW_WRITE_POST_DELAY {                   \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_IGNORE   , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_WH       , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_WB         \
}

#define NAND_SAMSUNG_CMD_TIMING_RAW_READ {                               \
    .pre_delay_ns                       = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_pre_delay_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_enable_post_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_this_lun_timeout_ns          = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_other_luns_timeout_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .ready_post_delay_ns                = NAND_SAMSUNG_TIMING_RR       , \
    .cycle_rw_enable_post_delay_ns      = NAND_SAMSUNG_TIMING_REA      , \
    .cycle_rw_disable_post_delay_ns     = NAND_SAMSUNG_TIMING_REH      , \
    .latch_disable_pre_delay_ns         = NAND_SAMSUNG_TIMING_IGNORE   , \
    .latch_disable_post_delay_ns        = NAND_SAMSUNG_TIMING_IGNORE   , \
    .post_delay_ns                      = NAND_SAMSUNG_TIMING_IGNORE     \
}

#ifdef __cplusplus
}
#endif

#endif /* NAND_SAMSUNG_CMD_TIMING_H */