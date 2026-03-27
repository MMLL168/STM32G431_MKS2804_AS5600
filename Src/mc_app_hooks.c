
/**
 ******************************************************************************
 * @file    mc_app_hooks.c
 * @author  Motor Control SDK Team, ST Microelectronics
 * @brief   This file implements default motor control app hooks.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 * @ingroup MCAppHooks
 */

/* Includes ------------------------------------------------------------------*/
#include "mc_app_hooks.h"
#include "mc_config.h"
#include "mc_type.h"
#include "speed_potentiometer.h"

/* USER CODE BEGIN Includes */
#include "mc_api.h"
#include "mc_interface.h"

#define THR_CENTER 32768
#define THR_DEADBAND 2000
#define THR_MAX 65520
#define THR_MIN 0

#define MIN_RPM 1800
#define MAX_RPM 8000

typedef enum { THR_IDLE = 0, THR_RUNNING } ThrottleState_t;

static ThrottleState_t s_thrState = THR_IDLE;
int16_t dbg_throttlePct = 0;
uint16_t dbg_potRaw = 0;
uint16_t dbg_motorState = 0;
int16_t dbg_cmdRPM = 0;
int16_t dbg_actualRPM = 0;
/* USER CODE END Includes */

/** @addtogroup MCSDK
 * @{
 */

/** @addtogroup MCTasks
 * @{
 */

/**
 * @defgroup MCAppHooks Motor Control Applicative hooks
 * @brief User defined functions that are called in the Motor Control tasks.
 *
 *
 * @{
 */

/**
 * @brief Hook function called right before the end of the MCboot function.
 *
 *
 *
 */
__weak void MC_APP_BootHook(void) {
  /* RCM component initialization */
  (void)RCM_RegisterRegConv(&PotRegConv_M1);
  SPDPOT_Init(&SpeedPotentiometer_M1);
  /* USER CODE BEGIN BootHook */

  /* USER CODE END BootHook */
}

/**
 * @brief Hook function called right after the Medium Frequency Task for
 * Motor 1.
 *
 *
 *
 */
__weak void MC_APP_PostMediumFrequencyHook_M1(void) {
  uint16_t rawValue = RCM_GetRegularConv(&PotRegConv_M1);
  // (void)SPDPOT_Run(&SpeedPotentiometer_M1, rawValue); // SPDPOT disabled to
  // prevent conflicts

  /* USER SECTION BEGIN PostMediumFrequencyHookM1 */
  dbg_potRaw = rawValue;

  /* 1. Calculate throttle percentage */
  int16_t throttlePct = 0;
  if (rawValue > (uint16_t)(THR_CENTER + THR_DEADBAND)) {
    uint32_t val = rawValue - (uint16_t)(THR_CENTER + THR_DEADBAND);
    uint32_t range = THR_MAX - (uint16_t)(THR_CENTER + THR_DEADBAND);
    if (val > range)
      val = range;
    throttlePct = (int16_t)((val * 100) / range);
  } else if (rawValue < (uint16_t)(THR_CENTER - THR_DEADBAND)) {
    uint32_t val = (uint16_t)(THR_CENTER - THR_DEADBAND) - rawValue;
    uint32_t range = (uint16_t)(THR_CENTER - THR_DEADBAND) - THR_MIN;
    if (val > range)
      val = range;
    throttlePct = -(int16_t)((val * 100) / range);
  } else {
    throttlePct = 0;
  }

  dbg_throttlePct = throttlePct;

  MCI_State_t state = MCI_GetSTMState(pMCI[M1]);
  dbg_motorState = (uint16_t)state;

  /* 2. State Machine */
  switch (s_thrState) {
  case THR_IDLE:
    if (throttlePct != 0) {
      if (state == IDLE) {
        int16_t initSpeedRpm = (throttlePct > 0) ? MIN_RPM : -MIN_RPM;
        int32_t initSpeedUnit = (int32_t)initSpeedRpm * SPEED_UNIT / U_RPM;

        MCI_ExecSpeedRamp(pMCI[M1], initSpeedUnit, 0);
        MC_StartMotor1();
        s_thrState = THR_RUNNING;
      } else if (state == FAULT_OVER) {
        MC_AcknowledgeFaultMotor1();
      }
    } else {
      /* When idle and throttle is 0, auto clear fault */
      if (state == FAULT_OVER) {
        MC_AcknowledgeFaultMotor1();
      }
    }
    break;

  case THR_RUNNING:
    if (throttlePct == 0) {
      MC_StopMotor1();
      s_thrState = THR_IDLE;
    } else {
      if (state == RUN) {
        int32_t abspct = (throttlePct > 0) ? throttlePct : -throttlePct;
        int32_t targetRpm = MIN_RPM + ((MAX_RPM - MIN_RPM) * abspct) / 100;
        if (throttlePct < 0) {
          targetRpm = -targetRpm;
        }
        int32_t targetSpeedUnit = targetRpm * SPEED_UNIT / U_RPM;

        MCI_ExecSpeedRamp(
            pMCI[M1], targetSpeedUnit,
            3000); /* 3000 ms smooth transition for lower current load */
      } else if (state == FAULT_NOW || state == FAULT_OVER || state == IDLE) {
        s_thrState = THR_IDLE;
      }
    }
    break;
  }

  /* 3. Update debug variables */
  dbg_cmdRPM = (int16_t)(MCI_GetMecSpeedRefUnit(pMCI[M1]) * U_RPM / SPEED_UNIT);
  dbg_actualRPM =
      (int16_t)(MCI_GetAvrgMecSpeedUnit(pMCI[M1]) * U_RPM / SPEED_UNIT);
  /* USER SECTION END PostMediumFrequencyHookM1 */
}

/** @} */

/** @} */

/** @} */

/************************ (C) COPYRIGHT 2025 STMicroelectronics *****END OF
 * FILE****/
