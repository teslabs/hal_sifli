/*
 * SPDX-FileCopyrightText: 2019-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "bf0_hal.h"

#if defined(HAL_AUDCODEC_MODULE_ENABLED)||defined(_SIFLI_DOXYGEN_)
//#define DBG_LEVEL                DBG_LOG
//#define LOG_TAG              "hal.audcodec"
//#include "drv_log.h"


static void HAL_AUDCODEC_DMA_Init(DMA_HandleTypeDef          *hdma, uint8_t from_mem);

static void AUDCODEC_DMATxCplt(DMA_HandleTypeDef *hdma);
static void AUDCODEC_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void AUDCODEC_DMARxCplt(DMA_HandleTypeDef *hdma);
static void AUDCODEC_DMAHalfRxCplt(DMA_HandleTypeDef *hdma);
static void AUDCODEC_DMAError(DMA_HandleTypeDef *hdma);

uint8_t g_pll_state = 0;
uint8_t g_pll_freq_type = 0;
static uint16_t pll_cfg2_reset = 0;

void set_pll_state(uint8_t state)
{
    g_pll_state = state;
}
uint8_t get_pll_state()
{
    return g_pll_state;
}
void set_pll_freq_type(uint8_t type)
{
    g_pll_freq_type = type;
}
uint8_t get_pll_freq_type()
{
    return g_pll_freq_type;
}
// need updata drv_audcodec if modify this function
__weak int updata_pll_freq(uint8_t type) //type 0: 16k 1024 series  1:44.1k 1024 series 2:16k 1000 series 3: 44.1k 1000 series
{
    hwp_audcodec->PLL_CFG2 |= AUDCODEC_PLL_CFG2_RSTB;
    // wait 50us
    HAL_Delay_us(50);
    if (0 == type)// set pll to 49.152M   [(fcw+3)+sdin/2^20]*6M
    {
        hwp_audcodec->PLL_CFG3 = (201327 << AUDCODEC_PLL_CFG3_SDIN_Pos) |
                                 (5 << AUDCODEC_PLL_CFG3_FCW_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_UPDATE_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_SDMIN_BYPASS_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_MODE_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_EN_SDM_DITHER_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_DITHER_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_EN_SDM_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDMCLK_POL_Pos);
    }
    else if (1 == type)// set pll to 45.1584M
    {

        hwp_audcodec->PLL_CFG3 = (551970 << AUDCODEC_PLL_CFG3_SDIN_Pos) |
                                 (4 << AUDCODEC_PLL_CFG3_FCW_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_UPDATE_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_SDMIN_BYPASS_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_MODE_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_EN_SDM_DITHER_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_DITHER_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_EN_SDM_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDMCLK_POL_Pos);
    }
    else if (3 == type)// set pll to 44.1M
    {

        hwp_audcodec->PLL_CFG3 = (0x5999A << AUDCODEC_PLL_CFG3_SDIN_Pos) |
                                 (4 << AUDCODEC_PLL_CFG3_FCW_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_UPDATE_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_SDMIN_BYPASS_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_MODE_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_EN_SDM_DITHER_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDM_DITHER_Pos) |
                                 (1 << AUDCODEC_PLL_CFG3_EN_SDM_Pos) |
                                 (0 << AUDCODEC_PLL_CFG3_SDMCLK_POL_Pos);
    }
    else
    {
        //RT_ASSERT(0);
    }
    hwp_audcodec->PLL_CFG3 |= AUDCODEC_PLL_CFG3_SDM_UPDATE;
    hwp_audcodec->PLL_CFG3 &= ~AUDCODEC_PLL_CFG3_SDMIN_BYPASS;
    hwp_audcodec->PLL_CFG2 &= ~AUDCODEC_PLL_CFG2_RSTB;
    // wait 50us
    HAL_Delay_us(50);
    hwp_audcodec->PLL_CFG2 |= AUDCODEC_PLL_CFG2_RSTB;
    // check pll lock
    //wait(2500);
    HAL_Delay_us(50);

    hwp_audcodec->PLL_CFG1 |= AUDCODEC_PLL_CFG1_CSD_EN | AUDCODEC_PLL_CFG1_CSD_RST;
    HAL_Delay_us(50);
    hwp_audcodec->PLL_CFG1 &= ~AUDCODEC_PLL_CFG1_CSD_RST;
    if (hwp_audcodec->PLL_STAT & AUDCODEC_PLL_STAT_UNLOCK_Msk)
    {
        //rt_kprintf("pll lock fail! freq_type:%d\n", type);
        return -1;
    }
    else
    {
        //rt_kprintf("pll lock! freq_type:%d\n", type);
        hwp_audcodec->PLL_CFG1 &= ~AUDCODEC_PLL_CFG1_CSD_EN;
        set_pll_freq_type(type);
    }
    return 0;
}

/**
 * @brief  enable PLL function
 * @param freq - frequency
 * @param type - 0:1024 series, 1:1000 series
 * @return
 */
__weak int bf0_enable_pll(uint32_t freq, uint8_t type)// need updata drv_audcodec if modify this function
{
    uint8_t freq_type;
    uint8_t test_result = -1;
    uint32_t pll_cnt;
    uint32_t xtal_cnt;
    uint32_t fc_vco;
    uint32_t fc_vco_min;
    uint32_t fc_vco_max;
    uint32_t delta_cnt=0;
    uint32_t delta_cnt_min=0;
    uint32_t delta_cnt_max=0;
    uint32_t delta_fc_vco;
    uint32_t target_cnt = 1838;

    fc_vco = 16;
    delta_fc_vco = 8;
    test_result = 0;

    freq_type = type << 1;
    if ((freq == 44100) || (freq == 22050) || (freq == 11025))
    {
        freq_type += 1;
    }

    //rt_kprintf("PLL_ENABLE pll_state:%d, freq_type:%d\n", get_pll_state(), freq_type);

    if (0 == get_pll_state())
    {
        hwp_pmuc->HXT_CR1 |= PMUC_HXT_CR1_BUF_AUD_EN;
        hwp_hpsys_rcc->ENR2 |= HPSYS_RCC_ENR2_AUDCODEC;
        //hwp_lpsys_rcc->ENR1 |= LPSYS_RCC_ENR1_AUDCODEC;

        //-----------------step 2------------------//
// turn on bandgap
// turn on BG sample clock
        //hwp_audcodec->BG_CFG1 = 48000;
        //hwp_audcodec->BG_CFG2 = 48000000;
// turn on bandgap
        hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_RCFLT;
        hwp_audcodec->BG_CFG0 |= AUDCODEC_BG_CFG0_EN;
        HAL_Delay_us(100);
#if 1
        hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#else
        hwp_audcodec->BG_CFG0 |= AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#endif

//-----------------step 3------------------//
// pll calibration
// wait 100us
        //wait(5000);
        HAL_Delay_us(100);

        hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_IARY;
        hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_VCO;
        hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_ANA;
        hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_ICP_SEL_Msk;
        hwp_audcodec->PLL_CFG0 |= (8 << AUDCODEC_PLL_CFG0_ICP_SEL_Pos);
        hwp_audcodec->PLL_CFG2 |= AUDCODEC_PLL_CFG2_EN_DIG;
        hwp_audcodec->PLL_CFG3 |= AUDCODEC_PLL_CFG3_EN_SDM;
        hwp_audcodec->PLL_CFG4 |= AUDCODEC_PLL_CFG4_EN_CLK_DIG;
        hwp_audcodec->PLL_CFG1 = (3 << AUDCODEC_PLL_CFG1_R3_SEL_Pos) |
                                 (1 << AUDCODEC_PLL_CFG1_RZ_SEL_Pos) |
                                 (3 << AUDCODEC_PLL_CFG1_C2_SEL_Pos) |
                                 (6 << AUDCODEC_PLL_CFG1_CZ_SEL_Pos) |
                                 (0 << AUDCODEC_PLL_CFG1_CSD_RST_Pos) |
                                 (0 << AUDCODEC_PLL_CFG1_CSD_EN_Pos);
// wait 50us
        //wait(2500);
        HAL_Delay_us(50);
// VCO freq calibration
        hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_OPEN;
        hwp_audcodec->PLL_CFG2 |= AUDCODEC_PLL_CFG2_EN_LF_VCIN;
        hwp_audcodec->PLL_CAL_CFG = (0    << AUDCODEC_PLL_CAL_CFG_EN_Pos) |
                                    (2000 << AUDCODEC_PLL_CAL_CFG_LEN_Pos);
// setup calibration and run
// target pll_cnt = ceil(46MHz/48MHz*2000)+1 = 1918
// target difference between pll_cnt and xtal_cnt should be less than 1
        while (delta_fc_vco != 0)
        {
            hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_FC_VCO;
            hwp_audcodec->PLL_CFG0 |= (fc_vco << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
            hwp_audcodec->PLL_CAL_CFG |= AUDCODEC_PLL_CAL_CFG_EN;
            while (!(hwp_audcodec->PLL_CAL_CFG & AUDCODEC_PLL_CAL_CFG_DONE_Msk));
            pll_cnt = (hwp_audcodec->PLL_CAL_RESULT >> AUDCODEC_PLL_CAL_RESULT_PLL_CNT_Pos);
            xtal_cnt = (hwp_audcodec->PLL_CAL_RESULT & AUDCODEC_PLL_CAL_RESULT_XTAL_CNT_Msk);
            hwp_audcodec->PLL_CAL_CFG &= ~AUDCODEC_PLL_CAL_CFG_EN;
            if (pll_cnt < target_cnt)
            {
                fc_vco = fc_vco + delta_fc_vco;
                delta_cnt = target_cnt - pll_cnt;
            }
            else if (pll_cnt > target_cnt)
            {
                fc_vco = fc_vco - delta_fc_vco;
                delta_cnt = pll_cnt - target_cnt;
            }
            delta_fc_vco = delta_fc_vco >> 1;
        }
        //rt_kprintf("call par CFG1(%x)\r\n", hwp_audcodec->PLL_CFG1);
        fc_vco_min = fc_vco - 1;
        if (fc_vco == 31)
        {
            fc_vco_max = fc_vco;
        }
        else
        {
            fc_vco_max = fc_vco + 1;
        }

        hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_FC_VCO;
        hwp_audcodec->PLL_CFG0 |= (fc_vco_min << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
        hwp_audcodec->PLL_CAL_CFG |= AUDCODEC_PLL_CAL_CFG_EN;
        //rt_kprintf("fc %d, xtal %d, pll %d\r\n", fc_vco, xtal_cnt, pll_cnt);
        while (!(hwp_audcodec->PLL_CAL_CFG & AUDCODEC_PLL_CAL_CFG_DONE_Msk));
        pll_cnt = (hwp_audcodec->PLL_CAL_RESULT >> AUDCODEC_PLL_CAL_RESULT_PLL_CNT_Pos);
        hwp_audcodec->PLL_CAL_CFG &= ~AUDCODEC_PLL_CAL_CFG_EN;
        if (pll_cnt < target_cnt)
        {
            delta_cnt_min = target_cnt - pll_cnt;
        }
        else if (pll_cnt > target_cnt)
        {
            delta_cnt_min = pll_cnt - target_cnt;
        }

        hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_FC_VCO;
        hwp_audcodec->PLL_CFG0 |= (fc_vco_max << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
        hwp_audcodec->PLL_CAL_CFG |= AUDCODEC_PLL_CAL_CFG_EN;
        while (!(hwp_audcodec->PLL_CAL_CFG & AUDCODEC_PLL_CAL_CFG_DONE_Msk));
        pll_cnt = (hwp_audcodec->PLL_CAL_RESULT >> AUDCODEC_PLL_CAL_RESULT_PLL_CNT_Pos);
        hwp_audcodec->PLL_CAL_CFG &= ~AUDCODEC_PLL_CAL_CFG_EN;
        if (pll_cnt < target_cnt)
        {
            delta_cnt_max = target_cnt - pll_cnt;
        }
        else if (pll_cnt > target_cnt)
        {
            delta_cnt_max = pll_cnt - target_cnt;
        }

        hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_FC_VCO;

        if (delta_cnt_min <= delta_cnt && delta_cnt_min <= delta_cnt_max)
        {
            hwp_audcodec->PLL_CFG0 |= (fc_vco_min << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
        }
        else if (delta_cnt_max <= delta_cnt && delta_cnt_max <= delta_cnt_min)
        {
            hwp_audcodec->PLL_CFG0 |= (fc_vco_max << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
        }
        else
        {
            hwp_audcodec->PLL_CFG0 |= (fc_vco << AUDCODEC_PLL_CFG0_FC_VCO_Pos);
        }

        hwp_audcodec->PLL_CFG2 &= ~AUDCODEC_PLL_CFG2_EN_LF_VCIN;
        hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_OPEN;

//-----------------step 4------------------//
// set pll to 49.152M
// wait 50us
        //wait(2500);
        HAL_Delay_us(50);

        do
        {
            test_result = updata_pll_freq(freq_type);
        }
        while (test_result != 0);

        set_pll_state(1);
        set_pll_freq_type(freq_type);
    }
    else if (freq_type != get_pll_freq_type())
    {
        do
        {
            test_result = updata_pll_freq(freq_type);
        }
        while (test_result != 0);
        set_pll_freq_type(freq_type);
    }


    return test_result;
}
__weak void bf0_disable_pll()
{
    HAL_TURN_OFF_PLL();
    set_pll_state(0);
    //rt_kprintf("PLL disable\n");
}

__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_Analog_DACPath(AUDCODE_DAC_CLK_CONFIG_TYPE *cfg)
{
    HAL_DBG_printf("config Aanlog DACPath\n");
    //-----------------step 5------------------//
// turn on refgen
//    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_VREF_SEL;
//    hwp_audcodec->BG_CFG0 |= (7 << AUDCODEC_BG_CFG0_VREF_SEL_Pos);  // AVDD = 3.3V
    //hwp_audcodec->BG_CFG0 |= (2 << AUDCODEC_BG_CFG0_VREF_SEL_Pos);  // AVDD = 1.8V
//    hwp_audcodec->REFGEN_CFG &= ~AUDCODEC_REFGEN_CFG_EN_CHOP;
//    hwp_audcodec->REFGEN_CFG |= AUDCODEC_REFGEN_CFG_EN;
//    hwp_audcodec->PLL_CFG5 = AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG |
//                                AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN;
//    HAL_Delay(2);// wait 2ms

//-----------------step 6------------------//
// turn on hp dac1 and dac2 analog
// dac1 and dac2 clock
    hwp_audcodec->PLL_CFG4 &= ~AUDCODEC_PLL_CFG4_SEL_CLK_DAC;
    hwp_audcodec->PLL_CFG4 &= ~AUDCODEC_PLL_CFG4_SEL_CLK_DAC_SOURCE;
    hwp_audcodec->PLL_CFG4 |= (1  << AUDCODEC_PLL_CFG4_EN_CLK_CHOP_DAC_Pos) |
                              (1  << AUDCODEC_PLL_CFG4_EN_CLK_DAC_Pos) |
                              (cfg->sel_clk_dac_source  << AUDCODEC_PLL_CFG4_SEL_CLK_DAC_SOURCE_Pos) | // select xtal
                              (cfg->sel_clk_dac  << AUDCODEC_PLL_CFG4_SEL_CLK_DAC_Pos) |
                              (1  << AUDCODEC_PLL_CFG4_EN_CLK_DIG_Pos);

    hwp_audcodec->PLL_CFG5 |= (1  << AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG_Pos) |  //
                              (1  << AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN_Pos); //

    //if (pll_cfg2_reset == 0)
    {
        hwp_audcodec->PLL_CFG2  &= ~AUDCODEC_PLL_CFG2_RSTB;
        HAL_Delay_us(100);
        hwp_audcodec->PLL_CFG2  |= AUDCODEC_PLL_CFG2_RSTB;
    }

    // dac1 and dac2 power
#ifdef SF32LB58X
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_LP_MODE;
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_LP_MODE;
#else
#if AVDD_V18_ENABLE
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_LP_MODE;  //1.8v
#else
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_LP_MODE; //3.3v
#endif
#endif
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_EN_OS_DAC;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_OS_DAC;
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_EN_VCM;
#ifdef BSP_ENABLE_DAC2
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_EN_VCM;
#else
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_VCM;
#endif
    // wait 5us
    //wait(250);
    HAL_Delay_us(5);
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_EN_AMP;
#ifdef BSP_ENABLE_DAC2
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_EN_AMP;
#else
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_AMP;
#endif
    // wait 1us
    //wait(50);
    HAL_Delay_us(1);
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_EN_OS_DAC;
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_EN_OS_DAC;
    // wait 10us
    //wait(500);
    HAL_Delay_us(10);
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_EN_DAC;
#ifdef BSP_ENABLE_DAC2
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_EN_DAC;
#else
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_DAC;
#endif
    // wait 10us
    //wait(500);
    HAL_Delay_us(10);
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_SR;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_SR;
    pll_cfg2_reset |= (1 << HAL_AUDCODEC_DAC_CH0);
    return HAL_OK;
}

void HAL_TURN_OFF_PLL()
{
    pll_cfg2_reset = 0;
    // turn off pll
    hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_EN_IARY;
    hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_EN_VCO;
    hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_EN_ANA;
    hwp_audcodec->PLL_CFG2 &= ~AUDCODEC_PLL_CFG2_EN_DIG;
    hwp_audcodec->PLL_CFG3 &= ~AUDCODEC_PLL_CFG3_EN_SDM;
    hwp_audcodec->PLL_CFG4 &= ~AUDCODEC_PLL_CFG4_EN_CLK_DIG;

    // turn off refgen
    hwp_audcodec->REFGEN_CFG &= ~AUDCODEC_REFGEN_CFG_EN;
#if 1
    // turn off bandgap
    hwp_audcodec->BG_CFG1 = 0;
    hwp_audcodec->BG_CFG2 = 0;
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN;
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL;
#endif
    HAL_DBG_printf("turn off pll\n");

}
void HAL_TURN_ON_PLL()
{
    HAL_DBG_printf("turn on pll\n");
    //-----------------step 2------------------//
// turn on bandgap
    hwp_audcodec->BG_CFG0 = (1 << AUDCODEC_BG_CFG0_EN_Pos) |
                            (0 << AUDCODEC_BG_CFG0_LP_MODE_Pos) |
#if AVDD_V18_ENABLE
                            (2 << AUDCODEC_BG_CFG0_VREF_SEL_Pos) |   // 0xc: 3.3v  2:AVDD = 1.8V
#else
                            (0xc << AUDCODEC_BG_CFG0_VREF_SEL_Pos) |   // 0xc: 3.3v  2:AVDD = 1.8V
#endif
                            // (1 << AUDCODEC_BG_CFG0_EN_CHOP_Pos) |
                            (0 << AUDCODEC_BG_CFG0_EN_SMPL_Pos) |
                            (1 << AUDCODEC_BG_CFG0_EN_RCFLT_Pos) |
                            (4 << AUDCODEC_BG_CFG0_MIC_VREF_SEL_Pos) |
                            (1   << AUDCODEC_BG_CFG0_EN_AMP_Pos) |  //----
                            (0   << AUDCODEC_BG_CFG0_SET_VC_Pos);
// turn on BG sample clock
#if 1 //pop noise
    hwp_audcodec->BG_CFG1 = 0;
    hwp_audcodec->BG_CFG2 = 0;
#else
    hwp_audcodec->BG_CFG1 = 48000;
    hwp_audcodec->BG_CFG2 = 48000000;
#endif
// turn on bandgap
    // HAL_Delay_us(100);
    //hwp_audcodec->BG_CFG0 |= AUDCODEC_BG_CFG0_EN_SMPL;

//-----------------step 3------------------//
// pll calibration
// wait 100us
    //wait(5000);
    HAL_Delay_us(100);

    hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_IARY;
    hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_VCO;
    hwp_audcodec->PLL_CFG0 |= AUDCODEC_PLL_CFG0_EN_ANA;
    hwp_audcodec->PLL_CFG0 &= ~AUDCODEC_PLL_CFG0_ICP_SEL_Msk;
    hwp_audcodec->PLL_CFG0 |= (8 << AUDCODEC_PLL_CFG0_ICP_SEL_Pos);
    hwp_audcodec->PLL_CFG2 |= AUDCODEC_PLL_CFG2_EN_DIG;
    hwp_audcodec->PLL_CFG3 |= AUDCODEC_PLL_CFG3_EN_SDM;
    hwp_audcodec->PLL_CFG4 |= AUDCODEC_PLL_CFG4_EN_CLK_DIG;
    hwp_audcodec->PLL_CFG1 = (3 << AUDCODEC_PLL_CFG1_R3_SEL_Pos) |
                             (1 << AUDCODEC_PLL_CFG1_RZ_SEL_Pos) |
                             (3 << AUDCODEC_PLL_CFG1_C2_SEL_Pos) |
                             (6 << AUDCODEC_PLL_CFG1_CZ_SEL_Pos) |
                             (0 << AUDCODEC_PLL_CFG1_CSD_RST_Pos) |
                             (0 << AUDCODEC_PLL_CFG1_CSD_EN_Pos);
// wait 50us
    //wait(2500);
    HAL_Delay_us(50);

    HAL_AUCODEC_Refgen_Init();

}

__HAL_ROM_USED void HAL_AUDCODEC_Close_Analog_DACPath(void)
{
    hwp_audcodec->DAC1_CFG |= AUDCODEC_DAC1_CFG_SR;
    hwp_audcodec->DAC2_CFG |= AUDCODEC_DAC2_CFG_SR;
    // wait 10us
    //wait(500);
    HAL_Delay_us(10);
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_EN_DAC;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_DAC;
    // wait 10us
    //wait(500);
    HAL_Delay_us(10);
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_EN_VCM;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_VCM;
    // wait 10us
    //wait(500);
    HAL_Delay_us(10);
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_EN_AMP;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_AMP;
    hwp_audcodec->DAC1_CFG &= ~AUDCODEC_DAC1_CFG_EN_OS_DAC;
    hwp_audcodec->DAC2_CFG &= ~AUDCODEC_DAC2_CFG_EN_OS_DAC;
    HAL_DBG_printf("close Aanlog DACPath\n");

}

__HAL_ROM_USED void HAL_AUCODEC_Refgen_Init(void)
{
    HAL_DBG_printf("turn on refgen\n");
    // turn on refgen
    //hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL;
    //hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_VREF_SEL;
    //hwp_audcodec->BG_CFG0 |= (7 << AUDCODEC_BG_CFG0_VREF_SEL_Pos);  // AVDD = 3.3V
    //hwp_audcodec->BG_CFG0 |= (2 << AUDCODEC_BG_CFG0_VREF_SEL_Pos);  // AVDD = 1.8V
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL;
    hwp_audcodec->REFGEN_CFG &= ~AUDCODEC_REFGEN_CFG_EN_CHOP;
    hwp_audcodec->REFGEN_CFG |= AUDCODEC_REFGEN_CFG_EN;
#if AVDD_V18_ENABLE
    hwp_audcodec->REFGEN_CFG |= AUDCODEC_REFGEN_CFG_LV_MODE;
#else
    hwp_audcodec->REFGEN_CFG &= ~AUDCODEC_REFGEN_CFG_LV_MODE;
#endif
    //hwp_audcodec->PLL_CFG5 = AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG |
    //                            AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN;
    MODIFY_REG(hwp_audcodec->PLL_CFG5, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG_Msk, \
               MAKE_REG_VAL(1, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG_Msk, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_BG_Pos));
    MODIFY_REG(hwp_audcodec->PLL_CFG5, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN_Msk, \
               MAKE_REG_VAL(1, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN_Msk, AUDCODEC_PLL_CFG5_EN_CLK_CHOP_REFGEN_Pos));
    HAL_Delay(2);   //2ms
#if 1
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#else
    hwp_audcodec->BG_CFG0 |= AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#endif
}

__HAL_ROM_USED void HAL_AUDCODEC_Config_Analog_ADCPath(AUDCODE_ADC_CLK_CONFIG_TYPE *cfg)
{
    HAL_DBG_printf("config Aanlog ADCPath\n");
    // turn on lp adc1 and adc2 analog
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL;
    hwp_audcodec->ADC_ANA_CFG |= AUDCODEC_ADC_ANA_CFG_MICBIAS_EN;
    hwp_audcodec->ADC_ANA_CFG &= ~AUDCODEC_ADC_ANA_CFG_MICBIAS_CHOP_EN;
    HAL_Delay(2);   //2ms
#if 1
    hwp_audcodec->BG_CFG0 &= ~AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#else
    hwp_audcodec->BG_CFG0 |= AUDCODEC_BG_CFG0_EN_SMPL; //noise pop
#endif
    // adc1 and adc2 clock
    hwp_audcodec->PLL_CFG6 = (0 << AUDCODEC_PLL_CFG6_SEL_TST_CLK_Pos) |
                             (0 << AUDCODEC_PLL_CFG6_EN_TST_CLK_Pos) |
                             (0 << AUDCODEC_PLL_CFG6_EN_CLK_RCCAL_Pos) |
                             (3 << AUDCODEC_PLL_CFG6_SEL_CLK_CHOP_MICBIAS_Pos) |
                             (1 << AUDCODEC_PLL_CFG6_EN_CLK_CHOP_MICBIAS_Pos) |
                             (cfg->sel_clk_adc << AUDCODEC_PLL_CFG6_SEL_CLK_ADC2_Pos) |
                             (cfg->diva_clk_adc << AUDCODEC_PLL_CFG6_DIVA_CLK_ADC2_Pos) |
                             (1 << AUDCODEC_PLL_CFG6_EN_CLK_ADC2_Pos) |
                             (cfg->sel_clk_adc << AUDCODEC_PLL_CFG6_SEL_CLK_ADC1_Pos) |
                             (cfg->diva_clk_adc << AUDCODEC_PLL_CFG6_DIVA_CLK_ADC1_Pos) |
                             (1 << AUDCODEC_PLL_CFG6_EN_CLK_ADC1_Pos) |
                             (1 << AUDCODEC_PLL_CFG6_SEL_CLK_ADC0_Pos) |
                             (5 << AUDCODEC_PLL_CFG6_DIVA_CLK_ADC0_Pos) |
                             (1 << AUDCODEC_PLL_CFG6_EN_CLK_ADC0_Pos) |
                             (cfg->sel_clk_adc_source << AUDCODEC_PLL_CFG6_SEL_CLK_ADC_SOURCE_Pos);

    //if (pll_cfg2_reset == 0)
    {
        hwp_audcodec->PLL_CFG2  &= ~AUDCODEC_PLL_CFG2_RSTB;
        HAL_Delay(1);
        hwp_audcodec->PLL_CFG2  |= AUDCODEC_PLL_CFG2_RSTB;
    }

    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_DIFF_EN;
    //hwp_audcodec->ADC1_CFG1 |= AUDCODEC_ADC1_CFG1_DIFF_EN;
    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_DACN_EN;

    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_FSP;
    hwp_audcodec->ADC1_CFG1 |= (cfg->fsp << AUDCODEC_ADC1_CFG1_FSP_Pos);

    //this make long mic startup pulse
    hwp_audcodec->ADC1_CFG1 |= AUDCODEC_ADC1_CFG1_VCMST;
    hwp_audcodec->ADC1_CFG2 |= AUDCODEC_ADC1_CFG2_CLEAR;

    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_GC_Msk;
    hwp_audcodec->ADC1_CFG1 |= (0x4 << AUDCODEC_ADC1_CFG1_GC_Pos);

    hwp_audcodec->ADC1_CFG2 |= AUDCODEC_ADC1_CFG2_EN;
    hwp_audcodec->ADC1_CFG2 &= ~AUDCODEC_ADC1_CFG2_RSTB;

    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_VREF_SEL;
    hwp_audcodec->ADC1_CFG1 |= (2  << AUDCODEC_ADC1_CFG1_VREF_SEL_Pos) ;

    hwp_audcodec->ADC2_CFG1 &= ~AUDCODEC_ADC2_CFG1_FSP;
    hwp_audcodec->ADC2_CFG1 |= (cfg->fsp << AUDCODEC_ADC2_CFG1_FSP_Pos);

    hwp_audcodec->ADC2_CFG1 |= AUDCODEC_ADC2_CFG1_VCMST;
    hwp_audcodec->ADC2_CFG2 |= AUDCODEC_ADC2_CFG2_CLEAR;

    hwp_audcodec->ADC2_CFG1 &= ~AUDCODEC_ADC2_CFG1_GC_Msk;
    hwp_audcodec->ADC2_CFG1 |= (0x1E << AUDCODEC_ADC2_CFG1_GC_Pos);

    hwp_audcodec->ADC2_CFG2 |= AUDCODEC_ADC2_CFG2_EN;
    hwp_audcodec->ADC2_CFG2 &= ~AUDCODEC_ADC2_CFG2_RSTB;

    hwp_audcodec->ADC2_CFG2 &= ~AUDCODEC_ADC2_CFG1_VREF_SEL;
    hwp_audcodec->ADC2_CFG2 |= (2  << AUDCODEC_ADC2_CFG1_VREF_SEL_Pos) ;
    // wait 20ms
    HAL_Delay(20);
    //hwp_audcodec->BG_CFG0  |= AUDCODEC_BG_CFG0_EN_SMPL;
    hwp_audcodec->ADC1_CFG2 |= AUDCODEC_ADC1_CFG2_RSTB;
    hwp_audcodec->ADC1_CFG1 &= ~AUDCODEC_ADC1_CFG1_VCMST;
    hwp_audcodec->ADC1_CFG2 &= ~AUDCODEC_ADC1_CFG2_CLEAR;
    hwp_audcodec->ADC2_CFG2 |= AUDCODEC_ADC2_CFG2_RSTB;
    hwp_audcodec->ADC2_CFG1 &= ~AUDCODEC_ADC2_CFG1_VCMST;
    hwp_audcodec->ADC2_CFG2 &= ~AUDCODEC_ADC2_CFG2_CLEAR;
    pll_cfg2_reset |= (1 << HAL_AUDCODEC_ADC_CH0);

}

__HAL_ROM_USED void HAL_AUDCODEC_Close_Analog_ADCPath(void)
{
    // turn off adc
    hwp_audcodec->ADC1_CFG2 &= ~AUDCODEC_ADC1_CFG2_EN;
    hwp_audcodec->ADC2_CFG2 &= ~AUDCODEC_ADC2_CFG2_EN;
    hwp_audcodec->ADC_ANA_CFG &= ~AUDCODEC_ADC_ANA_CFG_MICBIAS_EN;
    HAL_DBG_printf("close Aanlog ADCPath\n");

}


/**
  * @brief Initializes the AUDCODEC according to the specified parameters
  *         in the AUDCODEC_InitTypeDef and create the associated handle.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Init(AUDCODEC_HandleTypeDef *hacodec)
{
    int i = 0;
    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    if (hacodec->State[0] == HAL_AUDCODEC_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        hacodec->Lock = HAL_UNLOCKED;

        /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
        HAL_AUDCODEC_MspInit(hacodec);
    }

    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
    {
        hacodec->State[i] = HAL_AUDCODEC_STATE_BUSY;
    }

    // disable and Set delay count
    //MODIFY_REG(hacodec->Instance->CFG, AUDCODEC_CFG_EN_DLY_SEL_Msk,
    //            MAKE_REG_VAL(hacodec->Init.en_dly_sel, AUDCODEC_CFG_EN_DLY_SEL_Msk, AUDCODEC_CFG_EN_DLY_SEL_Pos));
    //hacodec->Instance->CFG = MAKE_REG_VAL(hacodec->Init.en_dly_sel, AUDCODEC_CFG_EN_DLY_SEL_Msk, AUDCODEC_CFG_EN_DLY_SEL_Pos);
    //hacodec->Instance->CFG = MAKE_REG_VAL(hacodec->Init.en_dly_sel, AUDCODEC_CFG_EN_DLY_SEL_Msk, AUDCODEC_CFG_EN_DLY_SEL_Pos);


    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (hacodec->hdma[i] != NULL)
            hacodec->hdma[i]->Parent = (void *)hacodec;

    for (i = HAL_AUDCODEC_DAC_CH0; i < HAL_AUDCODEC_ADC_CH0; i++)
    {
        if (hacodec->hdma[i] != NULL)
            HAL_AUDCODEC_DMA_Init(hacodec->hdma[i], 1);
    }
    for (i = HAL_AUDCODEC_ADC_CH0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
    {
        if (hacodec->hdma[i] != NULL)
            HAL_AUDCODEC_DMA_Init(hacodec->hdma[i], 0);
    }

    //HAL_AUDCODEC_Config_DACPath(hacodec, &(hacodec->Init.dac_cfg));
    //HAL_AUDCODEC_Config_ADCPath(hacodec, &(hacodec->Init.adc_cfg));

    hacodec->ErrorCode = HAL_AUDCODEC_ERROR_NONE;
    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
    {
        hacodec->State[i] = HAL_AUDCODEC_STATE_READY;
    }

    return HAL_OK;
}

/**
  * @brief DeInitializes the AUDCODEC peripheral
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_DeInit(AUDCODEC_HandleTypeDef *hacodec)
{
    int i;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }


    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
    {
        hacodec->State[i] |= HAL_AUDCODEC_STATE_BUSY;
        if (hacodec->hdma[i] != NULL)
            HAL_DMA_DeInit(hacodec->hdma[i]);
    }


    /* Disable the AUDCODEC Peripheral Clock */
    __HAL_AUDCODEC_DAC_DISABLE(hacodec);
    __HAL_AUDCODEC_ADC_DISABLE(hacodec);

    /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
    HAL_AUDCODEC_MspDeInit(hacodec);

    hacodec->ErrorCode = HAL_AUDCODEC_ERROR_NONE;
    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
    {
        hacodec->State[i] |= HAL_AUDCODEC_STATE_RESET;
    }

    /* Release Lock */
    __HAL_UNLOCK(hacodec);

    return HAL_OK;
}

/**
  * @brief Initializes the AUDCODEC pin setting.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @retval None
  */
__HAL_ROM_USED void HAL_AUDCODEC_MspInit(AUDCODEC_HandleTypeDef *hacodec)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
}

/**
  * @brief deunitializes the AUDCODEC pin setting.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @retval None
  */
__HAL_ROM_USED void HAL_AUDCODEC_MspDeInit(AUDCODEC_HandleTypeDef *hacodec)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
}

/**
  * @brief Set AUDCODEC transmit channel configures.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param cfg: configure parameters
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_TChanel(AUDCODEC_HandleTypeDef *hacodec, int channel, AUDCODEC_DACCfgTypeDef *cfg)
{
    AUDCODE_DAC_CLK_CONFIG_TYPE *dac_clk = cfg->dac_clk;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    hacodec->Instance->CFG |= (3 << AUDCODEC_CFG_ADC_EN_DLY_SEL_Pos);
    hacodec->Instance->DAC_CFG = (dac_clk->osr_sel  << AUDCODEC_DAC_CFG_OSR_SEL_Pos) |
                                 (cfg->opmode  << AUDCODEC_DAC_CFG_OP_MODE_Pos) |
                                 (0  << AUDCODEC_DAC_CFG_PATH_RESET_Pos) |
                                 (dac_clk->clk_src_sel  << AUDCODEC_DAC_CFG_CLK_SRC_SEL_Pos) |
                                 (dac_clk->clk_div << AUDCODEC_DAC_CFG_CLK_DIV_Pos);
    switch (channel)
    {
    case 0:
        hacodec->Instance->DAC_CH0_CFG = (1    << AUDCODEC_DAC_CH0_CFG_ENABLE_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Pos) |
                                         (2    << AUDCODEC_DAC_CH0_CFG_DEM_MODE_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DMA_EN_Pos) |
                                         (6    << AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos) |
                                         (1    << AUDCODEC_DAC_CH0_CFG_DATA_FORMAT_Pos) |
                                         (dac_clk->sinc_gain << AUDCODEC_DAC_CH0_CFG_SINC_GAIN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DITHER_GAIN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DITHER_EN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_CLK_ANA_POL_Pos);

        hacodec->Instance->DAC_CH0_CFG_EXT = (1 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_EN_Pos) |
                                             (1 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_MODE_Pos) |
                                             (1 << AUDCODEC_DAC_CH0_CFG_EXT_ZERO_ADJUST_EN_Pos) |
                                             (2 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_INTERVAL_Pos) |
                                             (0 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_STAT_Pos);

        hacodec->Instance->DAC_CH0_DEBUG = (0    << AUDCODEC_DAC_CH0_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH0_DEBUG_DATA_OUT_Pos);
        //hacodec->Instance->DAC_CH0_DC = MAKE_REG_VAL(cfg->dc_offset, AUDCODEC_DAC_CH0_DC_OFFSET_Msk, AUDCODEC_DAC_CH0_DC_OFFSET_Pos);
        break;
    case 1:
        hacodec->Instance->DAC_CH1_CFG = (1    << AUDCODEC_DAC_CH0_CFG_ENABLE_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Pos) |
                                         (2    << AUDCODEC_DAC_CH0_CFG_DEM_MODE_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DMA_EN_Pos) |
                                         (6    << AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos) |
                                         (1    << AUDCODEC_DAC_CH0_CFG_DATA_FORMAT_Pos) |
                                         (dac_clk->sinc_gain << AUDCODEC_DAC_CH0_CFG_SINC_GAIN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DITHER_GAIN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_DITHER_EN_Pos) |
                                         (0    << AUDCODEC_DAC_CH0_CFG_CLK_ANA_POL_Pos);

        hacodec->Instance->DAC_CH1_CFG_EXT = (1 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_EN_Pos) |
                                             (1 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_MODE_Pos) |
                                             (1 << AUDCODEC_DAC_CH0_CFG_EXT_ZERO_ADJUST_EN_Pos) |
                                             (2 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_INTERVAL_Pos) |
                                             (0 << AUDCODEC_DAC_CH0_CFG_EXT_RAMP_STAT_Pos);

        hacodec->Instance->DAC_CH1_DEBUG = (0    << AUDCODEC_DAC_CH0_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH0_DEBUG_DATA_OUT_Pos);
        //hacodec->Instance->DAC_CH1_DC = MAKE_REG_VAL(cfg->dc_offset, AUDCODEC_DAC_CH1_DC_OFFSET_Msk, AUDCODEC_DAC_CH1_DC_OFFSET_Pos);
        break;
    default:
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
  * @brief Set AUDCODEC Receive configures.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param cfg: configure parameters
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_RChanel(AUDCODEC_HandleTypeDef *hacodec, int channel, AUDCODEC_ADCCfgTypeDef *cfg)
{
    AUDCODE_ADC_CLK_CONFIG_TYPE *adc_clk = cfg->adc_clk;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    hacodec->Instance->ADC_CFG = (adc_clk->osr_sel << AUDCODEC_ADC_CFG_OSR_SEL_Pos) |
                                 (cfg->opmode << AUDCODEC_ADC_CFG_OP_MODE_Pos) |      // 2 for test
                                 (0 << AUDCODEC_ADC_CFG_PATH_RESET_Pos) |
                                 (adc_clk->clk_src_sel << AUDCODEC_ADC_CFG_CLK_SRC_SEL_Pos) |
                                 (adc_clk->clk_div << AUDCODEC_ADC_CFG_CLK_DIV_Pos);

    //hacodec->Instance->ADC_CFG = 0x508;//0x508;  510:raw

    switch (channel)
    {
    case 0:

        hacodec->Instance->ADC_CH0_CFG = (1   << AUDCODEC_ADC_CH0_CFG_ENABLE_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_HPF_BYPASS_Pos) |
                                         (0x7 << AUDCODEC_ADC_CH0_CFG_HPF_COEF_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_STB_INV_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_DMA_EN_Pos) |
                                         (0xc << AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_FINE_VOL_Pos) |
                                         (1   << AUDCODEC_ADC_CH0_CFG_DATA_FORMAT_Pos); //16bit
        //hacodec->Instance->ADC_CH0_CFG = 0x10ABD;
        break;
    case 1:

        hacodec->Instance->ADC_CH1_CFG = (1   << AUDCODEC_ADC_CH0_CFG_ENABLE_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_HPF_BYPASS_Pos) |
                                         (0xf << AUDCODEC_ADC_CH0_CFG_HPF_COEF_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_STB_INV_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_DMA_EN_Pos) |
                                         (0xc << AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Pos) |
                                         (0   << AUDCODEC_ADC_CH0_CFG_FINE_VOL_Pos) |
                                         (1   << AUDCODEC_ADC_CH0_CFG_DATA_FORMAT_Pos);  //16bit
        break;
    default:
        return HAL_ERROR;
    }

    return HAL_OK;
}


/**
 * @brief  all channel disable
 * @param hacodec -pointer to a AUDCODEC_HandleTypeDef structure that contains
 *         the configuration information for AUDCODEC module
 * @param ch_type_bit : bit0 is dac, bit1 is adc
 * @return HAL status
 */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Clear_All_Channel(AUDCODEC_HandleTypeDef *hacodec, uint8_t ch_type_bit)
{
    if (ch_type_bit & 0x1)
    {
        __HAL_AUDCODEC_DAC_CH0_DISABLE(hacodec);
        __HAL_AUDCODEC_DAC_CH1_DISABLE(hacodec);
        hacodec->Instance->DAC_CFG |= AUDCODEC_DAC_CFG_PATH_RESET;
        hacodec->Instance->DAC_CFG &= ~AUDCODEC_DAC_CFG_PATH_RESET;
        pll_cfg2_reset &= ~(1 << HAL_AUDCODEC_DAC_CH0);
    }

    if (ch_type_bit & 0x2)
    {
        __HAL_AUDCODEC_ADC_CH0_DISABLE(hacodec);
        __HAL_AUDCODEC_ADC_CH1_DISABLE(hacodec);
        hacodec->Instance->ADC_CFG |= AUDCODEC_ADC_CFG_PATH_RESET;
        hacodec->Instance->ADC_CFG &= ~AUDCODEC_ADC_CFG_PATH_RESET;
        pll_cfg2_reset &= ~(1 << HAL_AUDCODEC_ADC_CH0);
    }

    return HAL_OK;
}

/**
  * @brief  Configure DAC path volume.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param channel: channel 0 or 1
  * @param volume: configure volume -36dB~54dB, unit is 0.5db, use fix point Q15.1 format
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_DACPath_Volume(AUDCODEC_HandleTypeDef *hacodec, int channel, int volume)
{
    uint32_t rough_vol, fine_vol;
    int volume2;
    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    if ((channel < 0) || (channel > 1))
    {
        return HAL_ERROR;
    }

    if (volume >= 0)
    {
        volume2 = volume;
    }
    else
    {
        volume2 = 0 - volume;
    }

    if (volume2 & 1)
    {
        fine_vol = 1;
    }
    else
    {
        fine_vol = 0;
    }

    volume2 = (volume2 >> 1);

    if (volume < 0)
    {
        volume = 0 - volume2;
        if (fine_vol)
            volume--;
    }
    else
        volume = volume2;

    //HAL_DBG_printf("codec vol=%d, 0.5 on/off=%d", volume, fine_vol);

    if ((volume < -36) || (volume > 54) || (volume == 54 && fine_vol))
    {
        return HAL_ERROR;
    }

    rough_vol = (volume + 36) / 6;
    fine_vol  = fine_vol + (((volume + 36) % 6) << 1);

    if (channel == 0)
    {
        MODIFY_REG(hacodec->Instance->DAC_CH0_CFG, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Msk | AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(rough_vol, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Msk, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Pos) |
                   MAKE_REG_VAL(fine_vol, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos));
    }
    else
    {
        MODIFY_REG(hacodec->Instance->DAC_CH1_CFG, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Msk | AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(rough_vol, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Msk, AUDCODEC_DAC_CH0_CFG_ROUGH_VOL_Pos) |
                   MAKE_REG_VAL(fine_vol, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos));
    }

    HAL_DBG_printf("set volume rough:%d, fine:%d, cfg0:0x%x", rough_vol, fine_vol, hacodec->Instance->DAC_CH0_CFG);

    return HAL_OK;
}

/**
  * @brief  Configure ADC path volume.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param channel: channel 0 or 1
  * @param volume: configure volume -36dB~54dB
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_ADCPath_Volume(AUDCODEC_HandleTypeDef *hacodec, int channel, int volume)
{
    uint32_t rough_vol, fine_vol;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    if ((channel < 0) || (channel > 1))
    {
        return HAL_ERROR;
    }

    if ((volume < -36) || (volume > 54))
    {
        return HAL_ERROR;
    }

    rough_vol = (volume + 36) / 6;
    fine_vol  = ((volume + 36) % 6) << 1;

    if (channel == 0)
    {
        MODIFY_REG(hacodec->Instance->ADC_CH0_CFG, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Msk | AUDCODEC_ADC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(rough_vol, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Msk, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Pos) |
                   MAKE_REG_VAL(fine_vol, AUDCODEC_ADC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_ADC_CH0_CFG_FINE_VOL_Pos));
    }
    else
    {
        MODIFY_REG(hacodec->Instance->ADC_CH1_CFG, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Msk | AUDCODEC_ADC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(rough_vol, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Msk, AUDCODEC_ADC_CH0_CFG_ROUGH_VOL_Pos) |
                   MAKE_REG_VAL(fine_vol, AUDCODEC_ADC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_ADC_CH0_CFG_FINE_VOL_Pos));
    }

    //LOG_E("set ADC volume rough:%d, fine:%d, cfg0:0x%x", rough_vol, fine_vol, hacodec->Instance->DAC_CH0_CFG);

    return HAL_OK;
}


/**
  * @brief  Config DAC path.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_DACPath(AUDCODEC_HandleTypeDef *hacodec, uint16_t bypass)
{
    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    if (bypass)
    {
        MODIFY_REG(hacodec->Instance->DAC_CH0_CFG, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Msk, \
                   MAKE_REG_VAL(1, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Msk, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Pos));

        hacodec->Instance->DAC_CH0_DEBUG = (1    << AUDCODEC_DAC_CH0_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH0_DEBUG_DATA_OUT_Pos);

        MODIFY_REG(hacodec->Instance->DAC_CH1_CFG, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Msk, \
                   MAKE_REG_VAL(1, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Msk, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Pos));

        hacodec->Instance->DAC_CH1_DEBUG = (1    << AUDCODEC_DAC_CH1_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH1_DEBUG_DATA_OUT_Pos);
    }
    else
    {
        MODIFY_REG(hacodec->Instance->DAC_CH0_CFG, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Msk, \
                   MAKE_REG_VAL(0, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Msk, AUDCODEC_DAC_CH0_CFG_DOUT_MUTE_Pos));
        hacodec->Instance->DAC_CH0_DEBUG = (0    << AUDCODEC_DAC_CH0_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH0_DEBUG_DATA_OUT_Pos);
        MODIFY_REG(hacodec->Instance->DAC_CH1_CFG, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Msk, \
                   MAKE_REG_VAL(0, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Msk, AUDCODEC_DAC_CH1_CFG_DOUT_MUTE_Pos));
        hacodec->Instance->DAC_CH1_DEBUG = (0    << AUDCODEC_DAC_CH1_DEBUG_BYPASS_Pos) |
                                           (0xFF    << AUDCODEC_DAC_CH1_DEBUG_DATA_OUT_Pos);
    }

    return HAL_OK;
}

/**
  * @brief  Configure DAC path mute.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param mute: is mute or not
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Mute_DACPath(AUDCODEC_HandleTypeDef *hacodec, int mute)
{
    static int fine_vol_0, fine_vol_1;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }

    if (mute)
    {
        fine_vol_0 = GET_REG_VAL(hacodec->Instance->DAC_CH0_CFG, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos);
        fine_vol_1 = GET_REG_VAL(hacodec->Instance->DAC_CH1_CFG, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Pos);
        MODIFY_REG(hacodec->Instance->DAC_CH0_CFG,  AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(0xF, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos));
        MODIFY_REG(hacodec->Instance->DAC_CH1_CFG,  AUDCODEC_DAC_CH1_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(0xF, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Pos));
    }
    else
    {
        MODIFY_REG(hacodec->Instance->DAC_CH0_CFG,  AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(fine_vol_0, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH0_CFG_FINE_VOL_Pos));
        MODIFY_REG(hacodec->Instance->DAC_CH1_CFG,  AUDCODEC_DAC_CH1_CFG_FINE_VOL_Msk, \
                   MAKE_REG_VAL(fine_vol_1, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Msk, AUDCODEC_DAC_CH1_CFG_FINE_VOL_Pos));
    }


    return HAL_OK;
}

#if 0

/**
  * @brief  Configure ADC path.
  * @param  hacodec: pointer to a AUDCODEC_HandleTypeDef structure that contains
  *         the configuration information for AUDCODEC module
  * @param cfg: configure parameters
  * @retval HAL status
  */
__HAL_ROM_USED HAL_StatusTypeDef HAL_AUDCODEC_Config_ADCPath(AUDCODEC_HandleTypeDef *hacodec, AUDCODEC_ADCCfgTypeDef *cfg)
{
    uint32_t value;

    /* Check the AUDCODEC handle allocation */
    if (hacodec == NULL)
    {
        return HAL_ERROR;
    }
    value = MAKE_REG_VAL(cfg->clk_div, AUDCODEC_ADC_CFG_CLK_DIV_Msk, AUDCODEC_ADC_CFG_CLK_DIV_Pos)
            | MAKE_REG_VAL(cfg->clk_src_sel, AUDCODEC_ADC_CFG_CLK_SRC_SEL_Msk, AUDCODEC_ADC_CFG_CLK_SRC_SEL_Pos)
            | MAKE_REG_VAL(cfg->op_mode, AUDCODEC_ADC_CFG_OP_MODE_Msk, AUDCODEC_ADC_CFG_OP_MODE_Pos)
            | MAKE_REG_VAL(cfg->osr_sel, AUDCODEC_ADC_CFG_OSR_SEL_Msk, AUDCODEC_ADC_CFG_OSR_SEL_Pos);

    hacodec->Instance->CFG = value;

    return HAL_OK;
}
#endif
static void HAL_AUDCODEC_DMA_Init(DMA_HandleTypeDef          *hdma, uint8_t from_mem)
{
    // configure TX DMA, instance and request should be initialized by caller
    if (hdma != NULL)
    {
        if (from_mem)
            hdma->Init.Direction           = DMA_MEMORY_TO_PERIPH;
        else
            hdma->Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma->Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma->Init.MemInc              = DMA_MINC_ENABLE;
        hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        /*Mode will be changed, based on destination select*/
        hdma->Init.Mode                = DMA_CIRCULAR; //DMA_CIRCULAR; //DMA_CIRCULAR; //DMA_NORMAL;
        hdma->Init.Priority            = DMA_PRIORITY_HIGH;
        hdma->Init.BurstSize = 0;
        //hdma->Parent = (void *)hacodec;

        HAL_DMA_Init(hdma);
    }

}

HAL_StatusTypeDef HAL_AUDCODEC_Transmit_DMA(AUDCODEC_HandleTypeDef *hacodec, uint8_t *pData, uint32_t Size, uint32_t did)
{
    uint32_t txentry;
    uint32_t *dmamask;

    if ((hacodec == NULL) || (hacodec->hdma[did] == NULL) || (pData == NULL) || (Size == 0U) || (did > HAL_AUDCODEC_DAC_CH1))
    {
        return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(hacodec);

    if (did == HAL_AUDCODEC_DAC_CH0)  // channel 0
    {
        txentry = (uint32_t)(&hacodec->Instance->DAC_CH0_ENTRY);
        dmamask = (uint32_t *)((uint32_t)&hacodec->Instance->DAC_CH0_CFG);
    }
    else
    {
        txentry = (uint32_t)(&hacodec->Instance->DAC_CH1_ENTRY);
        dmamask = (uint32_t *)((uint32_t)&hacodec->Instance->DAC_CH1_CFG);
    }

    if (!(hacodec->State[did] & HAL_AUDCODEC_STATE_BUSY_TX))
    {
        hacodec->buf[did] = pData;
        hacodec->State[did] |= HAL_AUDCODEC_STATE_BUSY_TX;
        hacodec->ErrorCode = HAL_AUDCODEC_ERROR_NONE;


        // DMA transmit as 32 bits
        hacodec->dataSize[did] = Size >> 2;

        /* Set the AUDCODEC Tx DMA Half transfert complete callback */
        hacodec->hdma[did]->XferHalfCpltCallback = AUDCODEC_DMATxHalfCplt;
        __HAL_DMA_SET_CIRCLUAR_MODE(hacodec->hdma[did], DMA_CIRCULAR);

        /* Set the AUDCODEC Tx DMA transfert complete callback */
        hacodec->hdma[did]->XferCpltCallback = AUDCODEC_DMATxCplt;

        /* Set the DMA error callback */
        hacodec->hdma[did]->XferErrorCallback = AUDCODEC_DMAError;

        /* Enable the Tx DMA Channel */
        HAL_DMA_Start_IT(hacodec->hdma[did], (uint32_t)hacodec->buf[did], txentry, hacodec->dataSize[did]);

        /* Check if the AUDCODEC Tx request is already enabled */
        *dmamask &= ~AUDCODEC_DAC_CH0_CFG_DMA_EN_Msk;
        *dmamask |= AUDCODEC_DAC_CH0_CFG_DMA_EN;
        /* Check if the AUDCODEC is already enabled */
        //__HAL_AUDCODEC_ENABLE(hacodec);

        /* Process Unlocked */
        __HAL_UNLOCK(hacodec);

        return HAL_OK;
    }
    else
    {
        /* Process Unlocked */
        __HAL_UNLOCK(hacodec);
        return HAL_BUSY;
    }

    return HAL_OK;

}

HAL_StatusTypeDef HAL_AUDCODEC_Receive_DMA(AUDCODEC_HandleTypeDef *hacodec, uint8_t *pData, uint32_t Size, uint32_t did)
{
    uint32_t rxentry;
    uint32_t *dmamask;

    /* check dma handle */
    if ((hacodec == NULL) || (hacodec->hdma[did] == NULL) || (pData == NULL) || (Size == 0U) || (did > HAL_AUDCODEC_INSTANC_CNT) || (did < HAL_AUDCODEC_ADC_CH0))
    {
        return  HAL_ERROR;
    }

    if (hacodec->State[did] & HAL_AUDCODEC_STATE_BUSY_RX)
    {
        return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(hacodec);

    if (did == HAL_AUDCODEC_ADC_CH0)
    {
        rxentry = (uint32_t)(&hacodec->Instance->ADC_CH0_ENTRY);
        dmamask = (uint32_t *)((uint32_t)&hacodec->Instance->ADC_CH0_CFG);
    }
    else
    {
        rxentry = (uint32_t)(&hacodec->Instance->ADC_CH1_ENTRY);
        dmamask = (uint32_t *)((uint32_t)&hacodec->Instance->ADC_CH1_CFG);
    }

    /* Set the transaction information */
    hacodec->State[did]  = HAL_AUDCODEC_STATE_BUSY_RX;
    hacodec->ErrorCode   = HAL_AUDCODEC_ERROR_NONE;

    hacodec->buf[did]    = pData;
    hacodec->dataSize[did] = Size >> 2;

    /* Set the AUDCODEC RxDMA Half transfer complete callback */
    hacodec->hdma[did]->XferHalfCpltCallback = AUDCODEC_DMAHalfRxCplt;
    __HAL_DMA_SET_CIRCLUAR_MODE(hacodec->hdma[did], DMA_CIRCULAR);

    /* Set the AUDCODEC Rx DMA transfer complete callback */
    hacodec->hdma[did]->XferCpltCallback = AUDCODEC_DMARxCplt;

    /* Set the AUDCODEC error callback */
    hacodec->hdma[did]->XferErrorCallback = AUDCODEC_DMAError;

    /* Set the AUDCODEC AbortCpltCallback */
    hacodec->hdma[did]->XferAbortCallback = NULL;


    /* Enable the Rx DMA Stream/Channel  */
    HAL_DMA_Start_IT(hacodec->hdma[did], (uint32_t)(rxentry), (uint32_t)hacodec->buf[did], hacodec->dataSize[did]);

    /* Check if the AUDCODEC Rx request is already enabled */
    *dmamask &= ~AUDCODEC_ADC_CH0_CFG_DMA_EN_Msk;
    *dmamask |= AUDCODEC_ADC_CH0_CFG_DMA_EN;
    /* Check if the AUDCODEC is already enabled */
    //__HAL_AUDCODEC_ENABLE(hacodec);

    /* Process Unlocked */
    __HAL_UNLOCK(hacodec);

    return HAL_OK;
}


HAL_StatusTypeDef HAL_AUDCODEC_DMAStop(AUDCODEC_HandleTypeDef *hacodec, uint32_t did)
{
    /* Abort the DMA rx/tx Stream/Channel  */
    if (hacodec->hdma[did] != NULL)
    {
        //__HAL_DMA_DISABLE(hacodec->hdma[did]);
        HAL_DMA_Abort(hacodec->hdma[did]);
    }

    switch (did)
    {
    case HAL_AUDCODEC_DAC_CH0:
        hacodec->Instance->DAC_CH0_CFG &= (~AUDCODEC_DAC_CH0_CFG_DMA_EN);
        break;
    case HAL_AUDCODEC_DAC_CH1:
        hacodec->Instance->DAC_CH1_CFG &= (~AUDCODEC_DAC_CH1_CFG_DMA_EN);
        break;
    case HAL_AUDCODEC_ADC_CH0:
        hacodec->Instance->ADC_CH0_CFG &= (~AUDCODEC_ADC_CH0_CFG_DMA_EN);
        break;
    case HAL_AUDCODEC_ADC_CH1:
        hacodec->Instance->ADC_CH1_CFG &= (~AUDCODEC_ADC_CH1_CFG_DMA_EN);
        break;
    }
    //__HAL_AUDCODEC_DISABLE(hacodec);

    //hacodec->State = HAL_AUDCODEC_STATE_READY;

    return HAL_OK;
}

__weak void HAL_AUDCODEC_TxCpltCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_AUDCODEC_TxHalfCpltCallback should be implemented in the user file
     */

}

__weak void HAL_AUDCODEC_TxHalfCpltCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_AUDCODEC_TxHalfCpltCallback should be implemented in the user file
     */

}

__weak void HAL_AUDCODEC_RxHalfCpltCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_AUDCODEC_RxHalfCpltCallback should be implemented in the user file
     */
}

__weak void HAL_AUDCODEC_RxCpltCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_AUDCODEC_RxCpltCallback should be implemented in the user file
     */
}

__weak void HAL_AUDCODEC_ErrorCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_PDM_ErrorCallback should be implemented in the user file
     */
}

__weak void HAL_AUDCODEC_AbortCpltCallback(AUDCODEC_HandleTypeDef *hacodec, int cid)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hacodec);
    UNUSED(cid);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_PDM_AbortCpltCallback should be implemented in the user file
     */
}


static void AUDCODEC_DMATxCplt(DMA_HandleTypeDef *hdma)
{
    AUDCODEC_HandleTypeDef *aprc = (AUDCODEC_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

    int i;
    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (aprc->hdma[i] == hdma)
            break;
    if (i >= HAL_AUDCODEC_INSTANC_CNT) // something wrong
        return;

    HAL_AUDCODEC_TxCpltCallback(aprc, i);
}

/**
  * @brief DMA I2S transmit process half complete callback
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void AUDCODEC_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
    AUDCODEC_HandleTypeDef *aprc = (AUDCODEC_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
    int i;
    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (aprc->hdma[i] == hdma)
            break;
    if (i >= HAL_AUDCODEC_INSTANC_CNT) // something wrong
        return;

    HAL_AUDCODEC_TxHalfCpltCallback(aprc, i);
}

static void AUDCODEC_DMARxCplt(DMA_HandleTypeDef *hdma)
{
    AUDCODEC_HandleTypeDef *aprc = (AUDCODEC_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
    int i;

    for (i = HAL_AUDCODEC_DAC_CH0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (aprc->hdma[i] == hdma)
            break;
    if (i >= HAL_AUDCODEC_INSTANC_CNT)
        return;

    HAL_AUDCODEC_RxCpltCallback(aprc, i);

}

/**
  * @brief  DMA PDM half receive process complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void AUDCODEC_DMAHalfRxCplt(DMA_HandleTypeDef *hdma)
{
    AUDCODEC_HandleTypeDef *aprc = (AUDCODEC_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
    int i;
    for (i = HAL_AUDCODEC_DAC_CH0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (aprc->hdma[i] == hdma)
            break;
    if (i >= HAL_AUDCODEC_INSTANC_CNT)
        return;
    HAL_AUDCODEC_RxHalfCpltCallback(aprc, i);
}


/**
  * @brief  DMA PDM communication error callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void AUDCODEC_DMAError(DMA_HandleTypeDef *hdma)
{
    AUDCODEC_HandleTypeDef *aprc = (AUDCODEC_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
    int i;
    for (i = 0; i < HAL_AUDCODEC_INSTANC_CNT; i++)
        if (aprc->hdma[i] == hdma)
            break;
    if (i >= HAL_AUDCODEC_INSTANC_CNT)
        return;

    aprc->State[i] = HAL_AUDCODEC_STATE_READY;
    HAL_AUDCODEC_ErrorCallback(aprc, i);
}

#endif /* HAL_AUDCODEC_MODULE_ENABLED */