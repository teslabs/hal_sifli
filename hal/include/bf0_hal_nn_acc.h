/*
 * SPDX-FileCopyrightText: 2019-2025 SiFli Technologies(Nanjing) Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BF0_NN_ACC_H_
#define BF0_NN_ACC_H_



/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>

/** @addtogroup BF0_HAL_Driver
  * @{
  */

/** @defgroup NNACC CNN accelerator
  * @brief Neural network CNN accelerator.
  * @{
  */

/**
  * @brief  HAL NNACC State structures definition
  */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HAL_NNACC_STATE_RESET             = 0x00U,    /*!< NN_ACC not yet initialized or disabled       */
    HAL_NNACC_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use    */
    HAL_NNACC_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing              */
    HAL_NNACC_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                               */
    HAL_NNACC_STATE_ERROR             = 0x04U,    /*!< NN_ACC state error                           */
    HAL_NNACC_STATE_SUSPEND           = 0x05U     /*!< NN_ACC process is suspended                  */
} HAL_NNACC_StateTypeDef;

typedef enum
{
    HAL_NNACC_MODE_CONV2D,
    HAL_NNACC_MODE_CONV2D_UNSIGNED,
    HAL_NNACC_MODE_DEPTHWISE_CONV2D,
    HAL_NNACC_MODE_DEPTHWISE_CONV2D_UNSIGNED,
} HAL_NNACC_ModeTypeDef;

/// Neural nextwork calulation configuration
typedef struct
{
    const int8_t *input;    ///< Input vector
    const int8_t *wt;       ///< Weights
    const int8_t *bias;     ///< Bias
    int8_t *output;         ///< output
    uint16_t in_dim_x;      ///< Input x dimension
    uint16_t in_dim_y;      ///< Input y dimension
    uint16_t in_ch_num;     ///< Input channel number
    uint16_t kernel_dim_x;  ///< CNN kernel x dimension
    uint16_t kernel_dim_y;  ///< CNN kernel y dimension
    uint16_t padding_x;     ///< x padding
    uint16_t padding_y;     ///< y padding
    uint16_t stride_x;      ///< x stride
    uint16_t stride_y;      ///< y stride
    uint16_t out_dim_x;     ///< Output x dimension
    uint16_t out_dim_y;     ///< Output y dimension
    uint16_t out_ch_num;    ///< Output channel number
    uint16_t bias_shift;    ///< Output bias shift
    uint16_t out_shift;     ///< Output shift
    HAL_NNACC_ModeTypeDef mode;
} NNACC_ConfigTypeDef;

typedef struct __NNACC_HandleTypeDef
{
    NN_ACC_TypeDef *instance;
    __IO HAL_NNACC_StateTypeDef state;
    void (* CpltCallback)(struct __NNACC_HandleTypeDef *nn_acc);   /*!< NNACC complete callback.          */
    void (* ErrorCallback)(struct __NNACC_HandleTypeDef *nn_acc);   /*!< NNACC Error callback.          */
} NNACC_HandleTypeDef;


/**
  * @brief initialize the Neural network accelerator
  * @param[in] nn_acc The Neural network accelerator instance
  * @retval HAL_OK if success, otherwise failed.
*/
HAL_StatusTypeDef HAL_NNACC_Init(NNACC_HandleTypeDef *nn_acc);

/**
  * @brief Start Neural network CNN calulation
  * @param[in] nn_acc The Neural network accelerator instance
  * @param[in] config CNN configuration
  * @retval HAL_OK if success, otherwise failed.
*/
HAL_StatusTypeDef HAL_NNACC_Start(NNACC_HandleTypeDef *nn_acc, NNACC_ConfigTypeDef *config);

/**
  * @brief Start Neural network CNN calulation with interrupt mode
  * @param[in] nn_acc The Neural network accelerator instance
  * @param[in] config CNN configuration
  * @retval HAL_OK if success, otherwise failed.
*/
HAL_StatusTypeDef HAL_NNACC_Start_IT(NNACC_HandleTypeDef *nn_acc, NNACC_ConfigTypeDef *config);

/**
  * @brief CNN calulation interrupt handler
  * @param[in] nn_acc The Neural network accelerator instance
  * @retval None.
*/
void HAL_NNACC_IRQHandler(NNACC_HandleTypeDef *nn_acc);

///@} NNACC

///@} BF0_HAL_Driver

#ifdef __cplusplus
}
#endif
#endif // BF0_NN_ACC_H_