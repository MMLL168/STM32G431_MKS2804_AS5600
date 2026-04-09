#include "user_platform.h"
#include <stdio.h>

#include "user_config.h"
#include "mc_config.h"
#include "parameters_conversion.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_usart.h"

#define USER_AS5600_RAW_ANGLE_REG 0x0EU
#define USER_UART_TARGET_LINE_MAX_LEN 24U
#define USER_UART_TARGET_PREFIX 'T'
#define USER_UART_TARGET_FULL_TURN_DEG10 3600U
#define USER_UART_TARGET_MAX_DEG10 3590U
#define USER_UART_TARGET_PI_X10000 31416UL
#define USER_UART_TARGET_TAU_X10000 62832UL

static I2C_HandleTypeDef s_hi2c1;
static TIM_HandleTypeDef s_htim2;
static uint8_t s_as5600I2cReady = 0U;
static uint8_t s_pwmTargetHasValue = 0U;
static uint8_t s_pwmTargetAwaitingFalling = 0U;
static uint16_t s_pwmTargetAngleDeg10 = 0U;
static uint16_t s_pwmTargetPulseWidthUs = 0U;
static uint32_t s_pwmTargetFrameCount = 0U;
static uint32_t s_pwmTargetErrorCount = 0U;
static uint32_t s_pwmTargetLastTickMs = 0U;
static uint32_t s_pwmTargetRiseCaptureUs = 0U;
static uint16_t s_remoteTargetAngleDeg10 = 0U;
static uint32_t s_remoteTargetFrameCount = 0U;
static uint32_t s_remoteTargetErrorCount = 0U;
static uint32_t s_remoteTargetLastFrameTickMs = 0U;
static uint8_t s_remoteTargetHasValue = 0U;
static uint8_t s_remoteTargetLineLength = 0U;
static char s_remoteTargetLine[USER_UART_TARGET_LINE_MAX_LEN];
static uint32_t s_angleTelemetryLastTickMs = 0U;
static uint32_t s_angleTelemetryTxCount = 0U;
static uint32_t s_angleTelemetryErrorCount = 0U;

extern UART_HandleTypeDef huart2;

static int16_t UserCurrentMilliAmpToDigit(int32_t currentMilliAmp) {
  return (int16_t)((currentMilliAmp * (int32_t)CURRENT_CONV_FACTOR) / 1000);
}

static int16_t UserRpmToSpeedUnit(int32_t rpm) {
  return (int16_t)((rpm * SPEED_UNIT) / U_RPM);
}

static uint32_t UserPlatform_GetApb1TimerClockHz(void) {
  uint32_t pclk1Hz = HAL_RCC_GetPCLK1Freq();

  if ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_CFGR_PPRE1_DIV1) {
    return pclk1Hz;
  }

  return pclk1Hz * 2U;
}

static uint16_t UserPlatform_MapPulseWidthUsToAngleDeg10(uint16_t pulseWidthUs) {
  uint32_t clamped = pulseWidthUs;
  uint32_t inputSpanUs = USER_PWM_TARGET_MAX_PULSE_US - USER_PWM_TARGET_MIN_PULSE_US;

  if (clamped <= USER_PWM_TARGET_MIN_PULSE_US) {
    return 0U;
  }
  if (clamped >= USER_PWM_TARGET_MAX_PULSE_US) {
    return USER_UART_TARGET_MAX_DEG10;
  }

  clamped -= USER_PWM_TARGET_MIN_PULSE_US;
  return (uint16_t)((clamped * USER_UART_TARGET_MAX_DEG10 + (inputSpanUs / 2U)) /
                    inputSpanUs);
}

static void UserPlatform_InitPwmTargetInput(void) {
#if USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  uint32_t timerClockHz = UserPlatform_GetApb1TimerClockHz();
  uint32_t prescaler = 0U;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();

  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_15);
  LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_15);
  LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_15);
  LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);

  GPIO_InitStruct.Pin = USER_PWM_TARGET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(USER_PWM_TARGET_GPIO_Port, &GPIO_InitStruct);

  if (timerClockHz >= 1000000U) {
    prescaler = (timerClockHz / 1000000U) - 1U;
  }

  s_htim2.Instance = TIM2;
  s_htim2.Init.Prescaler = prescaler;
  s_htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  s_htim2.Init.Period = 0xFFFFFFFFU;
  s_htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  s_htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&s_htim2) != HAL_OK) {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0U;
  if (HAL_TIM_IC_ConfigChannel(&s_htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(TIM2_IRQn, 3, 2);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  if (HAL_TIM_IC_Start_IT(&s_htim2, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  s_pwmTargetHasValue = 0U;
  s_pwmTargetAwaitingFalling = 0U;
  s_pwmTargetAngleDeg10 = 0U;
  s_pwmTargetPulseWidthUs = 0U;
  s_pwmTargetFrameCount = 0U;
  s_pwmTargetErrorCount = 0U;
  s_pwmTargetLastTickMs = 0U;
  s_pwmTargetRiseCaptureUs = 0U;
#endif
}

static void UserPlatform_InitUserButtonInput(void) {
#if (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*
   * Re-own PC10 in user code so press and release are both visible.
   * Generated code configures only falling-edge start/stop behavior.
   */
  GPIO_InitStruct.Pin = Start_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Start_Stop_GPIO_Port, &GPIO_InitStruct);
#endif
}

static void UserPlatform_ClearRemoteTargetLine(void) {
  s_remoteTargetLineLength = 0U;
  s_remoteTargetLine[0] = '\0';
}

static uint8_t UserPlatform_ParseRemoteTargetLine(const char *line,
                                                  uint16_t *angleDeg10) {
  uint32_t wholePart = 0U;
  uint32_t fracPart = 0U;
  uint32_t radiansX10000;
  uint8_t fracDigits = 0U;
  uint8_t hasWholeDigits = 0U;
  const char *cursor = line;

  if ((line == NULL) || (angleDeg10 == NULL) || (*cursor != USER_UART_TARGET_PREFIX)) {
    return 0U;
  }

  cursor++;
  if ((*cursor < '0') || (*cursor > '9')) {
    return 0U;
  }

  while ((*cursor >= '0') && (*cursor <= '9')) {
    hasWholeDigits = 1U;
    wholePart = (wholePart * 10U) + (uint32_t)(*cursor - '0');
    cursor++;
  }

  if (*cursor == '.') {
    cursor++;
    while ((*cursor >= '0') && (*cursor <= '9')) {
      if (fracDigits >= 4U) {
        return 0U;
      }
      fracPart = (fracPart * 10U) + (uint32_t)(*cursor - '0');
      fracDigits++;
      cursor++;
    }
  }

  if ((hasWholeDigits == 0U) || (*cursor != '\0')) {
    return 0U;
  }

  while (fracDigits < 4U) {
    fracPart *= 10U;
    fracDigits++;
  }

  radiansX10000 = (wholePart * 10000U) + fracPart;
  if (radiansX10000 > USER_UART_TARGET_TAU_X10000) {
    radiansX10000 = USER_UART_TARGET_TAU_X10000;
  }

  *angleDeg10 = (uint16_t)((radiansX10000 * 1800UL + (USER_UART_TARGET_PI_X10000 / 2UL)) /
                           USER_UART_TARGET_PI_X10000);
  if (*angleDeg10 >= USER_UART_TARGET_FULL_TURN_DEG10) {
    *angleDeg10 = USER_UART_TARGET_MAX_DEG10;
  }

  return 1U;
}

static void UserPlatform_FinalizeRemoteTargetLine(void) {
  uint16_t targetAngleDeg10 = 0U;

  if (s_remoteTargetLineLength == 0U) {
    return;
  }

  s_remoteTargetLine[s_remoteTargetLineLength] = '\0';
  if (UserPlatform_ParseRemoteTargetLine(s_remoteTargetLine, &targetAngleDeg10) != 0U) {
    s_remoteTargetAngleDeg10 = targetAngleDeg10;
    s_remoteTargetLastFrameTickMs = HAL_GetTick();
    s_remoteTargetHasValue = 1U;
    s_remoteTargetFrameCount++;
  } else {
    s_remoteTargetErrorCount++;
  }

  UserPlatform_ClearRemoteTargetLine();
}

static void UserPlatform_PrepareUartTargetPort(void) {
#if USER_PLATFORM_OWNS_USART2 != 0U
  LL_USART_DisableIT_IDLE(USART2);
  LL_USART_DisableIT_ERROR(USART2);
  LL_USART_DisableIT_TC(USART2);
  LL_USART_DisableDMAReq_RX(USART2);
  LL_USART_DisableDMAReq_TX(USART2);
  LL_USART_ClearFlag_IDLE(USART2);
  LL_USART_ClearFlag_ORE(USART2);
  LL_USART_ClearFlag_NE(USART2);
  LL_USART_ClearFlag_FE(USART2);

  while (LL_USART_IsActiveFlag_RXNE_RXFNE(USART2) != 0U) {
    (void)LL_USART_ReceiveData8(USART2);
  }

  UserPlatform_ClearRemoteTargetLine();
#endif
}

static void UserPlatform_ApplyRuntimeMotorTuning(void) {
#if USER_RUNTIME_TUNE_ENABLE != 0
  int16_t speedLimitDigit =
      UserCurrentMilliAmpToDigit(USER_RUNTIME_MAX_CURRENT_MA);
  int16_t maxSpeedUnit = UserRpmToSpeedUnit(USER_RUNTIME_MAX_SPEED_RPM);
  int32_t speedIntegralLimit;

  PIDSpeedHandle_M1.hDefKpGain = USER_RUNTIME_SPEED_PID_KP;
  PIDSpeedHandle_M1.hDefKiGain = USER_RUNTIME_SPEED_PID_KI;
  PID_SetKP(&PIDSpeedHandle_M1, USER_RUNTIME_SPEED_PID_KP);
  PID_SetKI(&PIDSpeedHandle_M1, USER_RUNTIME_SPEED_PID_KI);
  PID_SetIntegralTerm(&PIDSpeedHandle_M1, 0);

  speedIntegralLimit =
      (int32_t)speedLimitDigit * (int32_t)PID_GetKIDivisor(&PIDSpeedHandle_M1);
  PIDSpeedHandle_M1.wUpperIntegralLimit = speedIntegralLimit;
  PIDSpeedHandle_M1.wLowerIntegralLimit = -speedIntegralLimit;
  PIDSpeedHandle_M1.hUpperOutputLimit = speedLimitDigit;
  PIDSpeedHandle_M1.hLowerOutputLimit = -speedLimitDigit;

  PIDIqHandle_M1.hDefKpGain = USER_RUNTIME_IQ_PID_KP;
  PIDIqHandle_M1.hDefKiGain = USER_RUNTIME_IQ_PID_KI;
  PID_SetKP(&PIDIqHandle_M1, USER_RUNTIME_IQ_PID_KP);
  PID_SetKI(&PIDIqHandle_M1, USER_RUNTIME_IQ_PID_KI);
  PID_SetIntegralTerm(&PIDIqHandle_M1, 0);

  PIDIdHandle_M1.hDefKpGain = USER_RUNTIME_ID_PID_KP;
  PIDIdHandle_M1.hDefKiGain = USER_RUNTIME_ID_PID_KI;
  PID_SetKP(&PIDIdHandle_M1, USER_RUNTIME_ID_PID_KP);
  PID_SetKI(&PIDIdHandle_M1, USER_RUNTIME_ID_PID_KI);
  PID_SetIntegralTerm(&PIDIdHandle_M1, 0);

  SpeednTorqCtrlM1.MaxAppPositiveMecSpeedUnit = (uint16_t)maxSpeedUnit;
  SpeednTorqCtrlM1.MinAppPositiveMecSpeedUnit = 0U;
  SpeednTorqCtrlM1.MaxAppNegativeMecSpeedUnit = 0;
  SpeednTorqCtrlM1.MinAppNegativeMecSpeedUnit = (int16_t)(-maxSpeedUnit);
  SpeednTorqCtrlM1.MaxPositiveTorque = (uint16_t)speedLimitDigit;
  SpeednTorqCtrlM1.MinNegativeTorque = (int16_t)(-speedLimitDigit);
  SpeednTorqCtrlM1.MecSpeedRefUnitDefault = 0;
  SpeednTorqCtrlM1.TorqueRefDefault = 0;
  SpeednTorqCtrlM1.IdrefDefault = 0;
  STC_Clear(&SpeednTorqCtrlM1);
#endif
}

void UserPlatform_Init(void) {
  if (s_as5600I2cReady != 0U) {
    return;
  }

  s_hi2c1.Instance = I2C1;
  s_hi2c1.Init.Timing = 0x10C0ECFF;
  s_hi2c1.Init.OwnAddress1 = 0U;
  s_hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  s_hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  s_hi2c1.Init.OwnAddress2 = 0U;
  s_hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  s_hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  s_hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&s_hi2c1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigAnalogFilter(&s_hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigDigitalFilter(&s_hi2c1, 0U) != HAL_OK) {
    Error_Handler();
  }

  UserPlatform_InitUserButtonInput();
  UserPlatform_InitPwmTargetInput();
  UserPlatform_PrepareUartTargetPort();
  UserPlatform_ApplyRuntimeMotorTuning();
  s_as5600I2cReady = 1U;
}

uint8_t UserPlatform_IsAs5600I2CReady(void) { return s_as5600I2cReady; }

I2C_HandleTypeDef *UserPlatform_GetAs5600I2C(void) { return &s_hi2c1; }

uint8_t UserPlatform_ReadAs5600RawAngle(uint16_t *rawAngle) {
  uint8_t reg = USER_AS5600_RAW_ANGLE_REG;
  uint8_t rxData[2] = {0};

  if ((rawAngle == NULL) || (s_as5600I2cReady == 0U)) {
    return 0U;
  }

  if (HAL_I2C_Master_Transmit(&s_hi2c1, USER_AS5600_I2C_ADDR, &reg, 1U, 10U) != HAL_OK) {
    return 0U;
  }

  if (HAL_I2C_Master_Receive(&s_hi2c1, USER_AS5600_I2C_ADDR, rxData, 2U, 10U) != HAL_OK) {
    return 0U;
  }

  *rawAngle = (uint16_t)((((uint16_t)rxData[0]) << 8) | rxData[1]) & 0x0FFFU;
  return 1U;
}

void UserPlatform_HandlePwmTargetEdgeInterrupt(void) {
  /* PWM capture now uses TIM2 input capture hardware, not EXTI timing. */
  (void)0;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
#if USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600
  if ((htim == NULL) || (htim->Instance != TIM2) ||
      (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)) {
    return;
  }

  {
    GPIO_PinState pinState = HAL_GPIO_ReadPin(USER_PWM_TARGET_GPIO_Port, USER_PWM_TARGET_Pin);
    uint32_t captureUs = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    if (pinState == GPIO_PIN_SET) {
      s_pwmTargetRiseCaptureUs = captureUs;
      s_pwmTargetAwaitingFalling = 1U;
      return;
    }

    if (s_pwmTargetAwaitingFalling == 0U) {
      s_pwmTargetErrorCount++;
      return;
    }

    s_pwmTargetAwaitingFalling = 0U;

    {
      uint32_t pulseWidthUs = captureUs - s_pwmTargetRiseCaptureUs;

      if ((pulseWidthUs < 800U) || (pulseWidthUs > 2200U)) {
        s_pwmTargetErrorCount++;
        return;
      }

      s_pwmTargetPulseWidthUs = (uint16_t)pulseWidthUs;
      s_pwmTargetAngleDeg10 = UserPlatform_MapPulseWidthUsToAngleDeg10(s_pwmTargetPulseWidthUs);
      s_pwmTargetLastTickMs = HAL_GetTick();
      s_pwmTargetHasValue = 1U;
      s_pwmTargetFrameCount++;
    }
  }
#else
  (void)htim;
#endif
}

void TIM2_IRQHandler(void) {
#if USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600
  HAL_TIM_IRQHandler(&s_htim2);
#endif
}

uint8_t UserPlatform_HasPwmTarget(void) { return s_pwmTargetHasValue; }

uint8_t UserPlatform_IsPwmTargetOnline(void) {
  if (s_pwmTargetHasValue == 0U) {
    return 0U;
  }

  return ((HAL_GetTick() - s_pwmTargetLastTickMs) <= USER_PWM_TARGET_TIMEOUT_MS) ? 1U : 0U;
}

uint8_t UserPlatform_GetPwmTargetAngleDeg10(uint16_t *angleDeg10) {
  if ((angleDeg10 == NULL) || (s_pwmTargetHasValue == 0U)) {
    return 0U;
  }

  *angleDeg10 = s_pwmTargetAngleDeg10;
  return 1U;
}

uint16_t UserPlatform_GetPwmTargetPulseWidthUs(void) { return s_pwmTargetPulseWidthUs; }

uint32_t UserPlatform_GetPwmTargetFrameCount(void) { return s_pwmTargetFrameCount; }

uint32_t UserPlatform_GetPwmTargetErrorCount(void) { return s_pwmTargetErrorCount; }

void UserPlatform_PollRemoteTargetUart(void) {
#if USER_PLATFORM_OWNS_USART2 != 0U
  uint32_t errorFlags =
      USART2->ISR & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE);

  if (errorFlags != 0U) {
    USART2->ICR = USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NECF | USART_ICR_PECF;
    s_remoteTargetErrorCount++;
  }

  while (LL_USART_IsActiveFlag_RXNE_RXFNE(USART2) != 0U) {
    uint8_t rxByte = LL_USART_ReceiveData8(USART2);

    if (rxByte == '\r') {
      continue;
    }

    if (rxByte == '\n') {
      UserPlatform_FinalizeRemoteTargetLine();
      continue;
    }

    if ((rxByte < 0x20U) || (rxByte > 0x7EU)) {
      s_remoteTargetErrorCount++;
      UserPlatform_ClearRemoteTargetLine();
      continue;
    }

    if (s_remoteTargetLineLength >= (USER_UART_TARGET_LINE_MAX_LEN - 1U)) {
      s_remoteTargetErrorCount++;
      UserPlatform_ClearRemoteTargetLine();
      continue;
    }

    s_remoteTargetLine[s_remoteTargetLineLength] = (char)rxByte;
    s_remoteTargetLineLength++;
  }
#endif
}

uint8_t UserPlatform_HasRemoteTarget(void) { return s_remoteTargetHasValue; }

uint8_t UserPlatform_IsRemoteTargetOnline(void) {
  if (s_remoteTargetHasValue == 0U) {
    return 0U;
  }

  return ((HAL_GetTick() - s_remoteTargetLastFrameTickMs) <= USER_UART_TARGET_TIMEOUT_MS) ? 1U
                                                                                            : 0U;
}

uint8_t UserPlatform_GetRemoteTargetAngleDeg10(uint16_t *angleDeg10) {
  if ((angleDeg10 == NULL) || (s_remoteTargetHasValue == 0U)) {
    return 0U;
  }

  *angleDeg10 = s_remoteTargetAngleDeg10;
  return 1U;
}

uint32_t UserPlatform_GetRemoteTargetFrameCount(void) { return s_remoteTargetFrameCount; }

uint32_t UserPlatform_GetRemoteTargetErrorCount(void) { return s_remoteTargetErrorCount; }

void UserPlatform_SendAngleTelemetry(uint16_t knobAngleDeg10,
                                     uint16_t followerAngleDeg10,
                                     int16_t angleErrDeg10,
                                     uint16_t pulseWidthUs) {
#if USER_PLATFORM_OWNS_USART2 != 0U
  uint32_t nowTickMs = HAL_GetTick();
  char line[48];
  int length;

  if ((nowTickMs - s_angleTelemetryLastTickMs) < USER_ANGLE_TELEMETRY_INTERVAL_MS) {
    return;
  }

  if (huart2.gState != HAL_UART_STATE_READY) {
    s_angleTelemetryErrorCount++;
    return;
  }

  length = snprintf(line, sizeof(line), "ANG,%u,%u,%d,%u\r\n", knobAngleDeg10,
                    followerAngleDeg10, (int)angleErrDeg10, pulseWidthUs);
  if ((length <= 0) || ((size_t)length >= sizeof(line))) {
    s_angleTelemetryErrorCount++;
    return;
  }

  if (HAL_UART_Transmit(&huart2, (uint8_t *)line, (uint16_t)length, 2U) == HAL_OK) {
    s_angleTelemetryLastTickMs = nowTickMs;
    s_angleTelemetryTxCount++;
  } else {
    s_angleTelemetryErrorCount++;
  }
#else
  (void)knobAngleDeg10;
  (void)followerAngleDeg10;
  (void)angleErrDeg10;
  (void)pulseWidthUs;
#endif
}

uint32_t UserPlatform_GetAngleTelemetryTxCount(void) { return s_angleTelemetryTxCount; }

uint32_t UserPlatform_GetAngleTelemetryErrorCount(void) {
  return s_angleTelemetryErrorCount;
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  if (hi2c->Instance != I2C1) {
    return;
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = USER_AS5600_SDA_Pin | USER_AS5600_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  __HAL_RCC_I2C1_CLK_ENABLE();
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
  if (hi2c->Instance != I2C1) {
    return;
  }

  __HAL_RCC_I2C1_CLK_DISABLE();
  HAL_GPIO_DeInit(GPIOB, USER_AS5600_SDA_Pin | USER_AS5600_SCL_Pin);
  s_as5600I2cReady = 0U;
}
