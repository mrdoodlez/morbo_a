#include "adc.h"

static struct
{
    ADC_HandleTypeDef hadc1;
    uint32_t currChannel;
    char convInProgress;

    struct
    {
        ADC_ConvertCb cb;
    } conversions[ADC_Chan_Tot];
} _adcContext;

static const uint32_t _channels[] =
    {
        ADC_CHANNEL_4,
        ADC_CHANNEL_2};

static void MX_ADC1_Init(void);
static void SelectChannel(ADC_Chan_t ch);

/******************************************************************************/

void ADC_Init(int dev)
{
    if (dev == 1)
    {
        MX_ADC1_Init();
    }
}

int ADC_Read(int dev, ADC_Chan_t chan, ADC_ConvertCb cb)
{
    if (dev != 1)
        return -1;

    if (_adcContext.convInProgress)
        return -2;

    SelectChannel(chan);

    _adcContext.conversions[chan].cb = cb;

    HAL_ADC_Start_IT(&_adcContext.hadc1);

    _adcContext.convInProgress = 1;

    return 0;
}

/******************************************************************************/

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{
    /** Common config
     */
    _adcContext.hadc1.Instance = ADC1;
    _adcContext.hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    _adcContext.hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    _adcContext.hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
    _adcContext.hadc1.Init.GainCompensation = 0;
    _adcContext.hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    _adcContext.hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    _adcContext.hadc1.Init.LowPowerAutoWait = DISABLE;
    _adcContext.hadc1.Init.ContinuousConvMode = DISABLE;
    _adcContext.hadc1.Init.NbrOfConversion = 1;
    _adcContext.hadc1.Init.DiscontinuousConvMode = DISABLE;
    _adcContext.hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_HRTIM_TRG1;
    _adcContext.hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    _adcContext.hadc1.Init.DMAContinuousRequests = ENABLE;
    _adcContext.hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    _adcContext.hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&_adcContext.hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}

static void SelectChannel(ADC_Chan_t ch)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    /** Configure Regular Channel
     */
    sConfig.Channel = _channels[ch]; // ADC_CHANNEL_4;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    if (HAL_ADC_ConfigChannel(&_adcContext.hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    _adcContext.currChannel = ch;
}

/**
 * @brief ADC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    if (hadc->Instance == ADC1)
    {
        /** Initializes the peripherals clocks
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
        PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_ADC12_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PA1     ------> ADC1_IN2
        PA2     ------> ADC1_IN3
        PA3     ------> ADC1_IN4
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    }
}

/**
 * @brief ADC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC12_CLK_DISABLE();

        /**ADC1 GPIO Configuration
        PA1     ------> ADC1_IN2
        PA2     ------> ADC1_IN3
        PA3     ------> ADC1_IN4
        PB11     ------> ADC1_IN14
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
    }
}

/******************************************************************************/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        uint32_t val = HAL_ADC_GetValue(hadc);

        if (_adcContext.conversions[_adcContext.currChannel].cb != 0)
            _adcContext.conversions[_adcContext.currChannel].cb(val);

        _adcContext.convInProgress = 0;
    }
}

// **Interrupt Handler for ADC**
void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&_adcContext.hadc1);
}
