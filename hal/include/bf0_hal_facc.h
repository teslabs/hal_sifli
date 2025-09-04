/*
 * SPDX-FileCopyrightText: 2019-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BF0_HAL_FACC_H
#define __BF0_HAL_FACC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "bf0_hal_def.h"

/** @addtogroup FACC
  * @ingroup BF0_HAL_Driver
  * @{
  */

#define FACC_MAX_FIFO_SIZE  256
#define FACC_FIR_STATE_SIZE 260     // 64*32/8+4
#define FACC_IIR_STATE_SIZE 388     // (64+32)*32/8+4

/**
 * @brief  HAL ACC State structures definition
 */
typedef enum
{
    HAL_FACC_STATE_RESET             = 0x00U,    /*!< FACC not yet initialized or disabled       */
    HAL_FACC_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use   */
    HAL_FACC_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing             */
    HAL_FACC_STATE_ERROR             = 0x03U,    /*!< FACC state error                           */
} HAL_FACC_StateTypeDef;

typedef struct
{
    uint32_t gain: 4;            /*!< facc gain*/
    uint32_t last_sel: 1;        /*!< 0:other data block, 1:last data block, only used in conv */
    uint32_t reserved2: 1;       /*!< Reserved*/
    uint32_t mod_sel: 1;         /*!< 0: fir, 1: iir*/
    uint32_t sym_sel: 1;         /*!< 0:normal, 1:symmetric(only support fir mod)*/
    uint32_t fp_sel: 1;          /*!< 0: 16bit, 1: 8bit*/
    uint32_t conv_sel: 1;        /*!< 0: filter, 1: convolution(only support fir mod) */
    uint32_t even_sel: 1;        /*!< when syn_sel=1, this bit is effective, 0: odd, 1: even */
    uint32_t buf_sel: 1;         /*!< when buf_sel=1, buffer mode is enabled, temporary state will save/restored by buffer. */
    uint32_t anti: 1;            /*!< when sym_sel=1, bit effective, 1: Andi-sym 0:normal,*/
    uint32_t reserved: 19;      /* 32bit alignment*/
} FACC_ConfigTypeDef;

/** FACC handle structure definition */
typedef struct __FACC_HandleTypeDef
{
    FACC_TypeDef              *Instance;                                  /*!< FACC register base address.          */
    void (*CpltCallback)(struct __FACC_HandleTypeDef *facc);              /*!< FACC processing complete callback.   */
    FACC_ConfigTypeDef config;                                            /*!< FACC parameter config.  */
    __IO HAL_FACC_StateTypeDef State;                                     /*!< FACC state.                          */
} FACC_HandleTypeDef;


/**
 * @brief  Init FACC Accelerator
 * @retval status, HAL_OK if successful, otherwise failed
 *
 */
HAL_StatusTypeDef HAL_FACC_Init(FACC_HandleTypeDef *facc);

/**
 * @brief  Init FACC Accelerator
 * @param  facc  FACC handle
 * @param  config FACC configuration
 * @retval status, HAL_OK if successful, otherwise failed
 *
 */
HAL_StatusTypeDef HAL_FACC_Config(FACC_HandleTypeDef *facc, FACC_ConfigTypeDef *config);

/**
 * @brief  Reset FACC Accelerator
 * @param  facc  FACC handle
 * @retval status, HAL_OK if successful, otherwise failed
 *
 */
HAL_StatusTypeDef HAL_FACC_Reset(FACC_HandleTypeDef *facc);


/**
 * @brief  Set IIR filter co-efficients.
 * @param  facc  FACC handle
 * @param  coff_b coefficent array b
 * @param  len_b  coefficent array b length in bytes
 * @param  coff_a coefficent array a
 * @param  len_a  coefficent array a length in bytes
 * @param  reverse  array in reverse order
 * @retval status, HAL_OK if successful, otherwise failed
 */
HAL_StatusTypeDef HAL_FACC_SetCoeff(FACC_HandleTypeDef *facc, uint8_t *coff_b, uint16_t len_b, uint8_t *coff_a, uint16_t len_a, uint8_t reverse);

#define HAL_FACC_SetCoeffFir(facc,coff,len) HAL_FACC_SetCoeff(facc,coff,len,NULL,0,0)
#define HAL_FACC_SetCoeffFirReverse(facc,coff,len) HAL_FACC_SetCoeff(facc,coff,len,NULL,0,1)
#define HAL_FACC_SetConvKernel(facc,kernel,len) HAL_FACC_SetCoeff(facc,kernel,len,NULL,0,0)


/**
 * @brief  Enable FACC buffer mode, used in FIR/IIR suspend/resume calculation.
 * @param  facc  FACC handle
 * @param  buf_addr  Buffer address
 * @retval status, HAL_OK if successful, otherwise failed
 */
HAL_StatusTypeDef HAL_FACC_Buffer_Enable(FACC_HandleTypeDef *facc, uint8_t *buf_addr);

/**
 * @brief  DeInit FACC Accelerator
 * @retval status, HAL_OK if successful, otherwise failed
 *
 */
HAL_StatusTypeDef HAL_FACC_DeInit(FACC_HandleTypeDef *facc);


/**
 * @brief  Start FACC in synchronous mode
 * @param[in] facc FACC handle
 * @param[in] input Input data
 * @param[out] output Output data,
 * @param[in] len length of input data in bytes
 * @retval status, HAL_OK if successful, otherwise failed
 * @note Ouput buffer length need to be 8 bytes aligned in Lite, 4 bytes aligned in PRO.
 */
HAL_StatusTypeDef HAL_FACC_Start(FACC_HandleTypeDef *facc, uint8_t *input, uint8_t *output, uint32_t len);


/**
 * @brief  Start FACC in asynchronous mode
 *
 *  CpltCallback is called after complete
 * @param[in] facc FACC handle
 * @param[in] input Input data
 * @param[out] output Output data
 * @param[in] len length of input data in bytes
 * @retval status, HAL_OK if successful, otherwise failed
 * @note Ouput buffer length need to be 8 bytes aligned in Lite, 4 bytes aligned in PRO.
 */
HAL_StatusTypeDef HAL_FACC_Start_IT(FACC_HandleTypeDef *facc, uint8_t *input, uint8_t *output, uint32_t len);


/**
 * @brief  FACC Interrupt process
 * @param[in] facc FACC handle
 * @retval status, HAL_OK if successful, otherwise failed
 */
HAL_StatusTypeDef HAL_FACC_IRQHandler(FACC_HandleTypeDef *facc);

#ifdef __cplusplus
}
#endif

///@}   FACC

#endif /* __BF0_HAL_FACC_H */