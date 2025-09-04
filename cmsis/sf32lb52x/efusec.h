/*
 * SPDX-FileCopyrightText: 2019-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __EFUSEC_H
#define __EFUSEC_H

typedef struct
{
    __IO uint32_t CR;
    __IO uint32_t TIMR;
    __IO uint32_t SR;
    __IO uint32_t RSVDR;
    __IO uint32_t PGM_DATA0;
    __IO uint32_t PGM_DATA1;
    __IO uint32_t PGM_DATA2;
    __IO uint32_t PGM_DATA3;
    __IO uint32_t PGM_DATA4;
    __IO uint32_t PGM_DATA5;
    __IO uint32_t PGM_DATA6;
    __IO uint32_t PGM_DATA7;
    __IO uint32_t BANK0_DATA0;
    __IO uint32_t BANK0_DATA1;
    __IO uint32_t BANK0_DATA2;
    __IO uint32_t BANK0_DATA3;
    __IO uint32_t BANK0_DATA4;
    __IO uint32_t BANK0_DATA5;
    __IO uint32_t BANK0_DATA6;
    __IO uint32_t BANK0_DATA7;
    __IO uint32_t BANK1_DATA0;
    __IO uint32_t BANK1_DATA1;
    __IO uint32_t BANK1_DATA2;
    __IO uint32_t BANK1_DATA3;
    __IO uint32_t BANK1_DATA4;
    __IO uint32_t BANK1_DATA5;
    __IO uint32_t BANK1_DATA6;
    __IO uint32_t BANK1_DATA7;
    __IO uint32_t BANK2_DATA0;
    __IO uint32_t BANK2_DATA1;
    __IO uint32_t BANK2_DATA2;
    __IO uint32_t BANK2_DATA3;
    __IO uint32_t BANK2_DATA4;
    __IO uint32_t BANK2_DATA5;
    __IO uint32_t BANK2_DATA6;
    __IO uint32_t BANK2_DATA7;
    __IO uint32_t BANK3_DATA0;
    __IO uint32_t BANK3_DATA1;
    __IO uint32_t BANK3_DATA2;
    __IO uint32_t BANK3_DATA3;
    __IO uint32_t BANK3_DATA4;
    __IO uint32_t BANK3_DATA5;
    __IO uint32_t BANK3_DATA6;
    __IO uint32_t BANK3_DATA7;
    __IO uint32_t ANACR;
} EFUSEC_TypeDef;


/******************* Bit definition for EFUSEC_CR register ********************/
#define EFUSEC_CR_EN_Pos                (0U)
#define EFUSEC_CR_EN_Msk                (0x1UL << EFUSEC_CR_EN_Pos)
#define EFUSEC_CR_EN                    EFUSEC_CR_EN_Msk
#define EFUSEC_CR_MODE_Pos              (1U)
#define EFUSEC_CR_MODE_Msk              (0x1UL << EFUSEC_CR_MODE_Pos)
#define EFUSEC_CR_MODE                  EFUSEC_CR_MODE_Msk
#define EFUSEC_CR_BANKSEL_Pos           (2U)
#define EFUSEC_CR_BANKSEL_Msk           (0x3UL << EFUSEC_CR_BANKSEL_Pos)
#define EFUSEC_CR_BANKSEL               EFUSEC_CR_BANKSEL_Msk
#define EFUSEC_CR_IE_Pos                (4U)
#define EFUSEC_CR_IE_Msk                (0x1UL << EFUSEC_CR_IE_Pos)
#define EFUSEC_CR_IE                    EFUSEC_CR_IE_Msk

/****************** Bit definition for EFUSEC_TIMR register *******************/
#define EFUSEC_TIMR_THRCK_Pos           (0U)
#define EFUSEC_TIMR_THRCK_Msk           (0x7FUL << EFUSEC_TIMR_THRCK_Pos)
#define EFUSEC_TIMR_THRCK               EFUSEC_TIMR_THRCK_Msk
#define EFUSEC_TIMR_THPCK_Pos           (7U)
#define EFUSEC_TIMR_THPCK_Msk           (0x7UL << EFUSEC_TIMR_THPCK_Pos)
#define EFUSEC_TIMR_THPCK               EFUSEC_TIMR_THPCK_Msk
#define EFUSEC_TIMR_TCKHP_Pos           (10U)
#define EFUSEC_TIMR_TCKHP_Msk           (0x7FFUL << EFUSEC_TIMR_TCKHP_Pos)
#define EFUSEC_TIMR_TCKHP               EFUSEC_TIMR_TCKHP_Msk

/******************* Bit definition for EFUSEC_SR register ********************/
#define EFUSEC_SR_DONE_Pos              (0U)
#define EFUSEC_SR_DONE_Msk              (0x1UL << EFUSEC_SR_DONE_Pos)
#define EFUSEC_SR_DONE                  EFUSEC_SR_DONE_Msk

/****************** Bit definition for EFUSEC_RSVDR register ******************/

/**************** Bit definition for EFUSEC_PGM_DATA0 register ****************/
#define EFUSEC_PGM_DATA0_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA0_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA0_DATA_Pos)
#define EFUSEC_PGM_DATA0_DATA           EFUSEC_PGM_DATA0_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA1 register ****************/
#define EFUSEC_PGM_DATA1_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA1_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA1_DATA_Pos)
#define EFUSEC_PGM_DATA1_DATA           EFUSEC_PGM_DATA1_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA2 register ****************/
#define EFUSEC_PGM_DATA2_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA2_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA2_DATA_Pos)
#define EFUSEC_PGM_DATA2_DATA           EFUSEC_PGM_DATA2_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA3 register ****************/
#define EFUSEC_PGM_DATA3_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA3_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA3_DATA_Pos)
#define EFUSEC_PGM_DATA3_DATA           EFUSEC_PGM_DATA3_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA4 register ****************/
#define EFUSEC_PGM_DATA4_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA4_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA4_DATA_Pos)
#define EFUSEC_PGM_DATA4_DATA           EFUSEC_PGM_DATA4_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA5 register ****************/
#define EFUSEC_PGM_DATA5_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA5_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA5_DATA_Pos)
#define EFUSEC_PGM_DATA5_DATA           EFUSEC_PGM_DATA5_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA6 register ****************/
#define EFUSEC_PGM_DATA6_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA6_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA6_DATA_Pos)
#define EFUSEC_PGM_DATA6_DATA           EFUSEC_PGM_DATA6_DATA_Msk

/**************** Bit definition for EFUSEC_PGM_DATA7 register ****************/
#define EFUSEC_PGM_DATA7_DATA_Pos       (0U)
#define EFUSEC_PGM_DATA7_DATA_Msk       (0xFFFFFFFFUL << EFUSEC_PGM_DATA7_DATA_Pos)
#define EFUSEC_PGM_DATA7_DATA           EFUSEC_PGM_DATA7_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA0 register ***************/
#define EFUSEC_BANK0_DATA0_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA0_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA0_DATA_Pos)
#define EFUSEC_BANK0_DATA0_DATA         EFUSEC_BANK0_DATA0_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA1 register ***************/
#define EFUSEC_BANK0_DATA1_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA1_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA1_DATA_Pos)
#define EFUSEC_BANK0_DATA1_DATA         EFUSEC_BANK0_DATA1_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA2 register ***************/
#define EFUSEC_BANK0_DATA2_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA2_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA2_DATA_Pos)
#define EFUSEC_BANK0_DATA2_DATA         EFUSEC_BANK0_DATA2_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA3 register ***************/
#define EFUSEC_BANK0_DATA3_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA3_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA3_DATA_Pos)
#define EFUSEC_BANK0_DATA3_DATA         EFUSEC_BANK0_DATA3_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA4 register ***************/
#define EFUSEC_BANK0_DATA4_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA4_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA4_DATA_Pos)
#define EFUSEC_BANK0_DATA4_DATA         EFUSEC_BANK0_DATA4_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA5 register ***************/
#define EFUSEC_BANK0_DATA5_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA5_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA5_DATA_Pos)
#define EFUSEC_BANK0_DATA5_DATA         EFUSEC_BANK0_DATA5_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA6 register ***************/
#define EFUSEC_BANK0_DATA6_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA6_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA6_DATA_Pos)
#define EFUSEC_BANK0_DATA6_DATA         EFUSEC_BANK0_DATA6_DATA_Msk

/*************** Bit definition for EFUSEC_BANK0_DATA7 register ***************/
#define EFUSEC_BANK0_DATA7_DATA_Pos     (0U)
#define EFUSEC_BANK0_DATA7_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK0_DATA7_DATA_Pos)
#define EFUSEC_BANK0_DATA7_DATA         EFUSEC_BANK0_DATA7_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA0 register ***************/
#define EFUSEC_BANK1_DATA0_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA0_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA0_DATA_Pos)
#define EFUSEC_BANK1_DATA0_DATA         EFUSEC_BANK1_DATA0_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA1 register ***************/
#define EFUSEC_BANK1_DATA1_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA1_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA1_DATA_Pos)
#define EFUSEC_BANK1_DATA1_DATA         EFUSEC_BANK1_DATA1_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA2 register ***************/
#define EFUSEC_BANK1_DATA2_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA2_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA2_DATA_Pos)
#define EFUSEC_BANK1_DATA2_DATA         EFUSEC_BANK1_DATA2_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA3 register ***************/
#define EFUSEC_BANK1_DATA3_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA3_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA3_DATA_Pos)
#define EFUSEC_BANK1_DATA3_DATA         EFUSEC_BANK1_DATA3_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA4 register ***************/
#define EFUSEC_BANK1_DATA4_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA4_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA4_DATA_Pos)
#define EFUSEC_BANK1_DATA4_DATA         EFUSEC_BANK1_DATA4_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA5 register ***************/
#define EFUSEC_BANK1_DATA5_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA5_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA5_DATA_Pos)
#define EFUSEC_BANK1_DATA5_DATA         EFUSEC_BANK1_DATA5_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA6 register ***************/
#define EFUSEC_BANK1_DATA6_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA6_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA6_DATA_Pos)
#define EFUSEC_BANK1_DATA6_DATA         EFUSEC_BANK1_DATA6_DATA_Msk

/*************** Bit definition for EFUSEC_BANK1_DATA7 register ***************/
#define EFUSEC_BANK1_DATA7_DATA_Pos     (0U)
#define EFUSEC_BANK1_DATA7_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK1_DATA7_DATA_Pos)
#define EFUSEC_BANK1_DATA7_DATA         EFUSEC_BANK1_DATA7_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA0 register ***************/
#define EFUSEC_BANK2_DATA0_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA0_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA0_DATA_Pos)
#define EFUSEC_BANK2_DATA0_DATA         EFUSEC_BANK2_DATA0_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA1 register ***************/
#define EFUSEC_BANK2_DATA1_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA1_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA1_DATA_Pos)
#define EFUSEC_BANK2_DATA1_DATA         EFUSEC_BANK2_DATA1_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA2 register ***************/
#define EFUSEC_BANK2_DATA2_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA2_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA2_DATA_Pos)
#define EFUSEC_BANK2_DATA2_DATA         EFUSEC_BANK2_DATA2_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA3 register ***************/
#define EFUSEC_BANK2_DATA3_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA3_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA3_DATA_Pos)
#define EFUSEC_BANK2_DATA3_DATA         EFUSEC_BANK2_DATA3_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA4 register ***************/
#define EFUSEC_BANK2_DATA4_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA4_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA4_DATA_Pos)
#define EFUSEC_BANK2_DATA4_DATA         EFUSEC_BANK2_DATA4_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA5 register ***************/
#define EFUSEC_BANK2_DATA5_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA5_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA5_DATA_Pos)
#define EFUSEC_BANK2_DATA5_DATA         EFUSEC_BANK2_DATA5_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA6 register ***************/
#define EFUSEC_BANK2_DATA6_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA6_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA6_DATA_Pos)
#define EFUSEC_BANK2_DATA6_DATA         EFUSEC_BANK2_DATA6_DATA_Msk

/*************** Bit definition for EFUSEC_BANK2_DATA7 register ***************/
#define EFUSEC_BANK2_DATA7_DATA_Pos     (0U)
#define EFUSEC_BANK2_DATA7_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK2_DATA7_DATA_Pos)
#define EFUSEC_BANK2_DATA7_DATA         EFUSEC_BANK2_DATA7_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA0 register ***************/
#define EFUSEC_BANK3_DATA0_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA0_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA0_DATA_Pos)
#define EFUSEC_BANK3_DATA0_DATA         EFUSEC_BANK3_DATA0_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA1 register ***************/
#define EFUSEC_BANK3_DATA1_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA1_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA1_DATA_Pos)
#define EFUSEC_BANK3_DATA1_DATA         EFUSEC_BANK3_DATA1_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA2 register ***************/
#define EFUSEC_BANK3_DATA2_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA2_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA2_DATA_Pos)
#define EFUSEC_BANK3_DATA2_DATA         EFUSEC_BANK3_DATA2_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA3 register ***************/
#define EFUSEC_BANK3_DATA3_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA3_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA3_DATA_Pos)
#define EFUSEC_BANK3_DATA3_DATA         EFUSEC_BANK3_DATA3_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA4 register ***************/
#define EFUSEC_BANK3_DATA4_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA4_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA4_DATA_Pos)
#define EFUSEC_BANK3_DATA4_DATA         EFUSEC_BANK3_DATA4_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA5 register ***************/
#define EFUSEC_BANK3_DATA5_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA5_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA5_DATA_Pos)
#define EFUSEC_BANK3_DATA5_DATA         EFUSEC_BANK3_DATA5_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA6 register ***************/
#define EFUSEC_BANK3_DATA6_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA6_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA6_DATA_Pos)
#define EFUSEC_BANK3_DATA6_DATA         EFUSEC_BANK3_DATA6_DATA_Msk

/*************** Bit definition for EFUSEC_BANK3_DATA7 register ***************/
#define EFUSEC_BANK3_DATA7_DATA_Pos     (0U)
#define EFUSEC_BANK3_DATA7_DATA_Msk     (0xFFFFFFFFUL << EFUSEC_BANK3_DATA7_DATA_Pos)
#define EFUSEC_BANK3_DATA7_DATA         EFUSEC_BANK3_DATA7_DATA_Msk

/****************** Bit definition for EFUSEC_ANACR register ******************/
#define EFUSEC_ANACR_LDO_EN_Pos         (0U)
#define EFUSEC_ANACR_LDO_EN_Msk         (0x1UL << EFUSEC_ANACR_LDO_EN_Pos)
#define EFUSEC_ANACR_LDO_EN             EFUSEC_ANACR_LDO_EN_Msk
#define EFUSEC_ANACR_LDO_VREF_SEL_Pos   (1U)
#define EFUSEC_ANACR_LDO_VREF_SEL_Msk   (0x7UL << EFUSEC_ANACR_LDO_VREF_SEL_Pos)
#define EFUSEC_ANACR_LDO_VREF_SEL       EFUSEC_ANACR_LDO_VREF_SEL_Msk
#define EFUSEC_ANACR_LDO_MODE_Pos       (4U)
#define EFUSEC_ANACR_LDO_MODE_Msk       (0x1UL << EFUSEC_ANACR_LDO_MODE_Pos)
#define EFUSEC_ANACR_LDO_MODE           EFUSEC_ANACR_LDO_MODE_Msk
#define EFUSEC_ANACR_LDO_DC_TR_Pos      (8U)
#define EFUSEC_ANACR_LDO_DC_TR_Msk      (0x7UL << EFUSEC_ANACR_LDO_DC_TR_Pos)
#define EFUSEC_ANACR_LDO_DC_TR          EFUSEC_ANACR_LDO_DC_TR_Msk
#define EFUSEC_ANACR_RESERVE0_Pos       (16U)
#define EFUSEC_ANACR_RESERVE0_Msk       (0xFFUL << EFUSEC_ANACR_RESERVE0_Pos)
#define EFUSEC_ANACR_RESERVE0           EFUSEC_ANACR_RESERVE0_Msk
#define EFUSEC_ANACR_RESERVE1_Pos       (24U)
#define EFUSEC_ANACR_RESERVE1_Msk       (0xFFUL << EFUSEC_ANACR_RESERVE1_Pos)
#define EFUSEC_ANACR_RESERVE1           EFUSEC_ANACR_RESERVE1_Msk

#endif