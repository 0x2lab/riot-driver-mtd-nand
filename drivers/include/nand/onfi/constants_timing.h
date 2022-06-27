#ifndef NAND_ONFI_TIMING_H
#define NAND_ONFI_TIMING_H

#ifdef __cplusplus
extern "C" {
#endif

#define NAND_ONFI_TIMING_NANOSEC(x)                 (x)
#define NAND_ONFI_TIMING_MICROSEC(x)                (NAND_ONFI_TIMING_NANOSEC(1000 * (x)))

#define NAND_ONFI_TIMING_IGNORE                     (0)
#define NAND_ONFI_TIMING_INFINITY                   (NAND_ONFI_TIMING_MICROSEC(10000))

#define NAND_ONFI_TIMING_ADL                        (NAND_ONFI_TIMING_NANOSEC(400))
#define NAND_ONFI_TIMING_BERS                       (NAND_ONFI_TIMING_NANOSEC(500))
#define NAND_ONFI_TIMING_CCS                        (NAND_ONFI_TIMING_NANOSEC(500))
#define NAND_ONFI_TIMING_CEH                        (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_CH                         (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_CS                         (NAND_ONFI_TIMING_NANOSEC(70))
#define NAND_ONFI_TIMING_DH                         (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_DS                         (NAND_ONFI_TIMING_NANOSEC(40))
#define NAND_ONFI_TIMING_FEAT                       (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_ITC                        (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_PROG                       (NAND_ONFI_TIMING_MICROSEC(3))
#define NAND_ONFI_TIMING_R                          (NAND_ONFI_TIMING_MICROSEC(200))
#define NAND_ONFI_TIMING_RR                         (NAND_ONFI_TIMING_NANOSEC(40))
#define NAND_ONFI_TIMING_RST                        (NAND_ONFI_TIMING_MICROSEC(5000))
#define NAND_ONFI_TIMING_WB                         (NAND_ONFI_TIMING_NANOSEC(200))
#define NAND_ONFI_TIMING_WHR                        (NAND_ONFI_TIMING_NANOSEC(120))
#define NAND_ONFI_TIMING_WW                         (NAND_ONFI_TIMING_NANOSEC(100))

#define NAND_ONFI_TIMING_PLEBSY                     (NAND_ONFI_TIMING_BERS)
#define NAND_ONFI_TIMING_PLPBSY                     (NAND_ONFI_TIMING_PROG)
#define NAND_ONFI_TIMING_PLRBSY                     (NAND_ONFI_TIMING_R)
#define NAND_ONFI_TIMING_PCBSY                      (NAND_ONFI_TIMING_PROG)
#define NAND_ONFI_TIMING_RCBSY                      (NAND_ONFI_TIMING_R)

#define NAND_ONFI_TIMING_VDLY                       (NAND_ONFI_TIMING_NANOSEC(50))
#define NAND_ONFI_TIMING_CEVDLY                     (NAND_ONFI_TIMING_NANOSEC(50))
#define NAND_ONFI_TIMING_ENI                        (NAND_ONFI_TIMING_NANOSEC(15))
#define NAND_ONFI_TIMING_ENO                        (NAND_ONFI_TIMING_NANOSEC(50))

#define NAND_ONFI_TIMING_ZQCL                       (NAND_ONFI_TIMING_MICROSEC(1))
#define NAND_ONFI_TIMING_ZQCS                       (NAND_ONFI_TIMING_NANOSEC(400))

#define NAND_ONFI_TIMING_ALH                        (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_ALS                        (NAND_ONFI_TIMING_NANOSEC(50))
#define NAND_ONFI_TIMING_AR                         (NAND_ONFI_TIMING_NANOSEC(25))
#define NAND_ONFI_TIMING_CEA                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_CHZ                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_CLH                        (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_CLR                        (NAND_ONFI_TIMING_NANOSEC(20))
#define NAND_ONFI_TIMING_CLS                        (NAND_ONFI_TIMING_NANOSEC(50))
#define NAND_ONFI_TIMING_COH                        (NAND_ONFI_TIMING_NANOSEC(0))
#define NAND_ONFI_TIMING_CR                         (NAND_ONFI_TIMING_NANOSEC(10))
#define NAND_ONFI_TIMING_CR2                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_CR2_READ_ID                (NAND_ONFI_TIMING_NANOSEC(150))
#define NAND_ONFI_TIMING_CS3                        (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_IR                         (NAND_ONFI_TIMING_NANOSEC(10))
#define NAND_ONFI_TIMING_RC                         (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_REA                        (NAND_ONFI_TIMING_NANOSEC(40))
#define NAND_ONFI_TIMING_REH                        (NAND_ONFI_TIMING_NANOSEC(30))
#define NAND_ONFI_TIMING_RHOH                       (NAND_ONFI_TIMING_NANOSEC(0))
#define NAND_ONFI_TIMING_RHW                        (NAND_ONFI_TIMING_NANOSEC(200))
#define NAND_ONFI_TIMING_RHZ                        (NAND_ONFI_TIMING_NANOSEC(200))
#define NAND_ONFI_TIMING_RLOH                       (NAND_ONFI_TIMING_NANOSEC(0))
#define NAND_ONFI_TIMING_RP                         (NAND_ONFI_TIMING_NANOSEC(50))
#define NAND_ONFI_TIMING_WC                         (NAND_ONFI_TIMING_NANOSEC(100))
#define NAND_ONFI_TIMING_WH                         (NAND_ONFI_TIMING_NANOSEC(30))
#define NAND_ONFI_TIMING_WP                         (NAND_ONFI_TIMING_NANOSEC(50))

#ifdef __cplusplus
}
#endif

#endif /* NAND_ONFI_TIMING_H */