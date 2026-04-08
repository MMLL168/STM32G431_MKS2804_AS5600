#include "user_app.h"

#include "mc_api.h"
#include "mc_config.h"
#include "mc_interface.h"
#include "mc_type.h"
#include "parameters_conversion.h"
#include "user_config.h"
#include "user_platform.h"
#include "user_sensored.h"

#define USER_FULL_TURN_DEG10 3600U
#define USER_TARGET_MAX_DEG10 3590U
#define USER_AS5600_COUNTS_PER_TURN 4096U

typedef enum { CTRL_IDLE = 0, CTRL_RUNNING } ControlState_t;

static ControlState_t s_ctrlState = CTRL_IDLE;

#if USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
static int32_t s_followEstimatedAngleDeg10 = 0;
static uint32_t s_followLastTickMs = 0U;
static uint8_t s_followEstimateSeeded = 0U;
#endif

#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED)
static int32_t s_posCurrentCmdMilliAmp = 0;
static int32_t s_posTargetRpmCmd = 0;
static int32_t s_posFilteredSpeedRpm = 0;
static uint8_t s_posHoldActive = 1U;
static int16_t s_posTargetOffsetDeg10 = 0;
static uint8_t s_posTargetOffsetSeeded = 0U;
static uint8_t s_posZeroCalRequest = 0U;
static uint32_t s_userButtonLastTickMs = 0U;
#endif

int16_t dbg_throttlePct = 0;
uint16_t dbg_potRaw = 0;
uint16_t dbg_motorState = 0;
int16_t dbg_cmdRPM = 0;
int16_t dbg_actualRPM = 0;
uint16_t dbg_sourceAngleDeg10 = 0;
uint16_t dbg_targetAngleDeg10 = 0;
uint16_t dbg_actualAngleDeg10 = 0;
uint16_t dbg_virtualAngleDeg10 = 0;
int16_t dbg_angleErrDeg10 = 0;
uint16_t dbg_as5600Raw = 0;
uint8_t dbg_as5600ReadOk = 0;
uint8_t dbg_followEstimateSeeded = 0;
uint16_t dbg_currentFaults = 0;
uint16_t dbg_occurredFaults = 0;
uint16_t dbg_commandState = 0;
uint8_t dbg_motorProfile = USER_MOTOR_PROFILE;
uint8_t dbg_startupProfile = USER_STARTUP_PROFILE;
uint8_t dbg_mcsdkTuneProfile = USER_MCSDK_TUNE_PROFILE;
uint8_t dbg_motorProfileMismatch =
    (USER_MOTOR_PROFILE == USER_MCSDK_TUNE_PROFILE) ? 0U : 1U;
uint8_t dbg_focBackend = USER_FOC_BACKEND;
uint8_t dbg_sensoredOnline = 0U;
uint8_t dbg_sensoredAligned = 0U;
uint16_t dbg_sensoredElecAngleDeg10 = 0U;
int16_t dbg_sensoredSpeedRpm = 0;
uint16_t dbg_sensoredAlignMechDeg10 = 0U;
int16_t dbg_posCurrentCmdMa = 0;
uint8_t dbg_posHoldActive = 0U;
uint8_t dbg_posOutputMode = USER_POSITION_OUTPUT_MODE;
int16_t dbg_posTargetRpm = 0;
int16_t dbg_posSpeedErrRpm = 0;
uint16_t dbg_rawTargetAngleDeg10 = 0U;
int16_t dbg_posTargetOffsetDeg10 = 0;
int16_t dbg_targetSignedAngleDeg10 = 0;
int16_t dbg_actualSignedAngleDeg10 = 0;
uint8_t dbg_uartTargetOnline = 0U;
uint16_t dbg_uartTargetAngleDeg10 = 0U;
uint32_t dbg_uartTargetFrameCount = 0U;
uint32_t dbg_uartTargetErrorCount = 0U;
uint8_t dbg_pwmTargetOnline = 0U;
uint16_t dbg_pwmTargetAngleDeg10 = 0U;
uint16_t dbg_pwmTargetPulseWidthUs = 0U;
uint32_t dbg_pwmTargetFrameCount = 0U;
uint32_t dbg_pwmTargetErrorCount = 0U;
uint32_t dbg_zeroCalCount = 0U;

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_THROTTLE) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)
static int32_t UserAbsI32(int32_t value) { return (value < 0) ? -value : value; }
#endif

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB)
static int32_t UserClampI32(int32_t value, int32_t minValue, int32_t maxValue) {
  if (value < minValue) {
    return minValue;
  }
  if (value > maxValue) {
    return maxValue;
  }
  return value;
}
#endif

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_THROTTLE) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST)
static int16_t PotToSignedPercent(uint16_t rawValue) {
  int16_t throttlePct = 0;

  if (rawValue > (uint16_t)(USER_POT_CENTER + USER_POT_DEADBAND)) {
    uint32_t val = rawValue - (uint16_t)(USER_POT_CENTER + USER_POT_DEADBAND);
    uint32_t range = USER_POT_MAX - (uint16_t)(USER_POT_CENTER + USER_POT_DEADBAND);
    if (val > range) {
      val = range;
    }
    throttlePct = (int16_t)((val * 100U) / range);
  } else if (rawValue < (uint16_t)(USER_POT_CENTER - USER_POT_DEADBAND)) {
    uint32_t val = (uint16_t)(USER_POT_CENTER - USER_POT_DEADBAND) - rawValue;
    uint32_t range = (uint16_t)(USER_POT_CENTER - USER_POT_DEADBAND) - USER_POT_MIN;
    if (val > range) {
      val = range;
    }
    throttlePct = -(int16_t)((val * 100U) / range);
  }

  return throttlePct;
}
#endif

#if USER_FOC_BACKEND != USER_FOC_BACKEND_AS5600_SENSORED
static uint16_t AS5600_RawToAngleDeg10(uint16_t rawAngle) {
  return (uint16_t)((rawAngle * USER_FULL_TURN_DEG10) / USER_AS5600_COUNTS_PER_TURN);
}

static uint8_t AS5600_ReadRawAngle(uint16_t *rawAngle) {
  return UserPlatform_ReadAs5600RawAngle(rawAngle);
}
#endif

#if USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600
static uint16_t PotToAngleDeg10(uint16_t rawValue) {
  uint32_t clamped = (uint32_t)UserClampI32((int32_t)rawValue,
                                            USER_POS_POT_EFFECTIVE_MIN,
                                            USER_POS_POT_EFFECTIVE_MAX);
  uint32_t targetMaxDeg10 = USER_TARGET_MAX_DEG10;
  uint32_t effectiveRange = USER_POS_POT_EFFECTIVE_MAX - USER_POS_POT_EFFECTIVE_MIN;
  uint32_t normalizedRaw = clamped - USER_POS_POT_EFFECTIVE_MIN;
  uint32_t breakRaw = USER_POS_POT_BREAK_RAW;
  uint32_t breakAngleDeg10 = USER_POS_POT_BREAK_ANGLE_DEG10;

  /*
   * Full-range absolute mapping:
   * - effective min ->   0 deg
   * - effective max -> 359 deg
   * Values outside the effective slider travel are clamped.
   *
   * A piecewise correction is applied so the low-end slider travel expands
   * more aggressively; this compensates for the observed non-linear raw ADC
   * response of the slider potentiometer.
   */
  if (breakRaw <= USER_POS_POT_EFFECTIVE_MIN) {
    breakRaw = USER_POS_POT_EFFECTIVE_MIN + 1U;
  }
  if (breakRaw >= USER_POS_POT_EFFECTIVE_MAX) {
    breakRaw = USER_POS_POT_EFFECTIVE_MAX - 1U;
  }
  if (breakAngleDeg10 >= targetMaxDeg10) {
    breakAngleDeg10 = targetMaxDeg10 - 1U;
  }

  breakRaw -= USER_POS_POT_EFFECTIVE_MIN;

  if (normalizedRaw <= breakRaw) {
    return (uint16_t)((normalizedRaw * breakAngleDeg10) / breakRaw);
  }

  return (uint16_t)(breakAngleDeg10 +
                    (((normalizedRaw - breakRaw) * (targetMaxDeg10 - breakAngleDeg10)) /
                     (effectiveRange - breakRaw)));
}
#endif

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB)
static uint16_t NormalizeAngleDeg10(int32_t angleDeg10) {
  while (angleDeg10 < 0) {
    angleDeg10 += (int32_t)USER_FULL_TURN_DEG10;
  }
  while (angleDeg10 >= (int32_t)USER_FULL_TURN_DEG10) {
    angleDeg10 -= (int32_t)USER_FULL_TURN_DEG10;
  }
  return (uint16_t)angleDeg10;
}
#endif

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)
static int16_t AbsoluteToSignedAngleDeg10(uint16_t angleDeg10) {
  int32_t signedAngleDeg10 = angleDeg10;

  if (signedAngleDeg10 > ((int32_t)USER_FULL_TURN_DEG10 / 2)) {
    signedAngleDeg10 -= (int32_t)USER_FULL_TURN_DEG10;
  }

  return (int16_t)signedAngleDeg10;
}
#endif

#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED)
static uint16_t ApplyAngleOffsetDeg10(uint16_t angleDeg10, int16_t offsetDeg10) {
  return NormalizeAngleDeg10((int32_t)angleDeg10 + (int32_t)offsetDeg10);
}
#endif

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB)
static int16_t WrapAngleErrorDeg10(int16_t targetDeg10, int16_t actualDeg10) {
  int32_t error = (int32_t)targetDeg10 - (int32_t)actualDeg10;
  while (error > ((int32_t)USER_FULL_TURN_DEG10 / 2)) {
    error -= (int32_t)USER_FULL_TURN_DEG10;
  }
  while (error < -((int32_t)USER_FULL_TURN_DEG10 / 2)) {
    error += (int32_t)USER_FULL_TURN_DEG10;
  }
  return (int16_t)error;
}
#endif

#if USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
static void SeedFollowEstimate(uint16_t knobAngleDeg10) {
  s_followEstimatedAngleDeg10 = (int32_t)knobAngleDeg10;
  s_followLastTickMs = HAL_GetTick();
  s_followEstimateSeeded = 1U;
  dbg_followEstimateSeeded = 1U;
  dbg_virtualAngleDeg10 = knobAngleDeg10;
}

static void UpdateFollowEstimateFromMeasuredSpeed(void) {
  uint32_t nowTickMs;
  uint32_t deltaTickMs;
  int32_t measuredRpm;
  int32_t deltaAngleDeg10;

  if (s_followEstimateSeeded == 0U) {
    dbg_followEstimateSeeded = 0U;
    return;
  }

  nowTickMs = HAL_GetTick();
  deltaTickMs = nowTickMs - s_followLastTickMs;
  if (deltaTickMs == 0U) {
    dbg_followEstimateSeeded = 1U;
    dbg_virtualAngleDeg10 = (uint16_t)s_followEstimatedAngleDeg10;
    return;
  }
  s_followLastTickMs = nowTickMs;

  measuredRpm = (int32_t)MCI_GetAvrgMecSpeedUnit(pMCI[M1]) * U_RPM / SPEED_UNIT;
  deltaAngleDeg10 = (measuredRpm * (int32_t)USER_FULL_TURN_DEG10 * (int32_t)deltaTickMs) / 60000;
  s_followEstimatedAngleDeg10 += deltaAngleDeg10;
  s_followEstimatedAngleDeg10 = (int32_t)NormalizeAngleDeg10(s_followEstimatedAngleDeg10);

  dbg_followEstimateSeeded = 1U;
  dbg_virtualAngleDeg10 = (uint16_t)s_followEstimatedAngleDeg10;
}
#endif

static void UpdateAs5600Telemetry(void) {
#if USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED
  dbg_sensoredOnline = UserSensored_IsOnline();
  dbg_sensoredAligned = UserSensored_IsAligned();
  dbg_sensoredElecAngleDeg10 = UserSensored_GetElectricalAngleDeg10();
  dbg_sensoredSpeedRpm = UserSensored_GetMechanicalSpeedRpm();
  dbg_sensoredAlignMechDeg10 = UserSensored_GetAlignmentMechanicalAngleDeg10();

  if (dbg_sensoredOnline != 0U) {
    dbg_as5600ReadOk = 1U;
    dbg_as5600Raw = UserSensored_GetRawAngle();
    dbg_sourceAngleDeg10 = UserSensored_GetMechanicalAngleDeg10();
  } else {
    dbg_as5600ReadOk = 0U;
  }
#else
  static uint8_t s_pollDivider = 0U;
  uint16_t as5600Raw = 0U;

  s_pollDivider++;
  if (s_pollDivider < 50U) {
    return;
  }
  s_pollDivider = 0U;

  if (AS5600_ReadRawAngle(&as5600Raw) != 0U) {
    dbg_as5600ReadOk = 1U;
    dbg_as5600Raw = as5600Raw;
    dbg_sourceAngleDeg10 = AS5600_RawToAngleDeg10(as5600Raw);
  } else {
    dbg_as5600ReadOk = 0U;
  }
#endif
}

#if (USER_CONTROL_MODE == USER_CONTROL_MODE_THROTTLE) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST) || \
    (USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB) || \
    ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) && \
     (USER_FOC_BACKEND != USER_FOC_BACKEND_AS5600_SENSORED))
static void StartOrUpdateSpeedCommand(int32_t targetRpm, uint16_t rampMs) {
  int32_t targetSpeedUnit = targetRpm * SPEED_UNIT / U_RPM;
  MCI_State_t state = MCI_GetSTMState(pMCI[M1]);

  switch (s_ctrlState) {
  case CTRL_IDLE:
    if (state == IDLE) {
      MCI_ExecSpeedRamp(pMCI[M1], targetSpeedUnit, 0U);
      MC_StartMotor1();
      s_ctrlState = CTRL_RUNNING;
    } else if (state == FAULT_OVER) {
      MC_AcknowledgeFaultMotor1();
    }
    break;

  case CTRL_RUNNING:
    if (state == RUN) {
      MCI_ExecSpeedRamp(pMCI[M1], targetSpeedUnit, rampMs);
#if USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED
    } else if ((state == START) || (state == CHARGE_BOOT_CAP) ||
               (state == OFFSET_CALIB)) {
      MCI_ExecSpeedRamp(pMCI[M1], targetSpeedUnit, 0U);
#endif
    } else if ((state == FAULT_NOW) || (state == FAULT_OVER) || (state == IDLE)) {
      s_ctrlState = CTRL_IDLE;
    }
    break;
  }
}
#endif

#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED)
static void ResetPositionControllerState(void) {
  s_posCurrentCmdMilliAmp = 0;
  s_posTargetRpmCmd = 0;
  s_posFilteredSpeedRpm = 0;
  s_posHoldActive = 1U;
  s_posZeroCalRequest = 0U;
  dbg_posCurrentCmdMa = 0;
  dbg_posHoldActive = 1U;
  dbg_posTargetRpm = 0;
  dbg_posSpeedErrRpm = 0;
  dbg_rawTargetAngleDeg10 = 0U;
  dbg_posTargetOffsetDeg10 = s_posTargetOffsetDeg10;
  dbg_targetSignedAngleDeg10 = 0;
  dbg_actualSignedAngleDeg10 = 0;
}

static int16_t CurrentMilliAmpToDigit(int32_t currentMilliAmp) {
  return (int16_t)((currentMilliAmp * CURRENT_CONV_FACTOR) / 1000);
}

static void StartOrUpdateCurrentCommand(int32_t targetCurrentMilliAmp);
static void StopMotorControlIfRunning(void);

static void RunSensoredPositionController(uint16_t rawTargetAngleDeg10) {
  if (UserSensored_IsOnline() != 0U) {
    int16_t actualAbsAngleDeg10 = (int16_t)UserSensored_GetMechanicalAngleDeg10();
    int16_t actualSignedAngleDeg10 =
        AbsoluteToSignedAngleDeg10((uint16_t)actualAbsAngleDeg10);
    int32_t targetAngleDeg10 = (int32_t)rawTargetAngleDeg10;
    int16_t targetPhysicalAngleDeg10;
    int32_t angleErrDeg10;
    int32_t absErrDeg10;
    int16_t speedRpm = UserSensored_GetMechanicalSpeedRpm();

    dbg_rawTargetAngleDeg10 = rawTargetAngleDeg10;

    if (s_posTargetOffsetSeeded == 0U) {
#if USER_POS_BOOT_ALIGN_TO_SHAFT != 0U
      s_posTargetOffsetDeg10 =
          WrapAngleErrorDeg10(actualAbsAngleDeg10, (int16_t)rawTargetAngleDeg10);
#else
      s_posTargetOffsetDeg10 = 0;
#endif
      s_posTargetOffsetSeeded = 1U;
    }

    if (s_posZeroCalRequest != 0U) {
      s_posTargetOffsetDeg10 =
          WrapAngleErrorDeg10(actualAbsAngleDeg10, (int16_t)rawTargetAngleDeg10);
      s_posTargetOffsetSeeded = 1U;
      s_posZeroCalRequest = 0U;
      s_posCurrentCmdMilliAmp = 0;
      s_posTargetRpmCmd = 0;
      s_posFilteredSpeedRpm = 0;
      s_posHoldActive = 1U;
      dbg_zeroCalCount++;
    }

    targetAngleDeg10 =
        (int32_t)ApplyAngleOffsetDeg10(rawTargetAngleDeg10, s_posTargetOffsetDeg10);
    targetPhysicalAngleDeg10 = (int16_t)NormalizeAngleDeg10(targetAngleDeg10);
    dbg_targetAngleDeg10 = (uint16_t)targetPhysicalAngleDeg10;
    dbg_posTargetOffsetDeg10 = s_posTargetOffsetDeg10;
    dbg_targetSignedAngleDeg10 =
        AbsoluteToSignedAngleDeg10((uint16_t)targetPhysicalAngleDeg10);

    angleErrDeg10 = WrapAngleErrorDeg10(targetPhysicalAngleDeg10, actualAbsAngleDeg10);
    absErrDeg10 = UserAbsI32(angleErrDeg10);

    dbg_actualAngleDeg10 = (uint16_t)actualAbsAngleDeg10;
    dbg_actualSignedAngleDeg10 = actualSignedAngleDeg10;
    dbg_virtualAngleDeg10 = 0U;
    dbg_angleErrDeg10 =
        (int16_t)UserClampI32(angleErrDeg10, INT16_MIN, INT16_MAX);
    dbg_sensoredSpeedRpm = speedRpm;

    if (s_posHoldActive != 0U) {
      if (absErrDeg10 >= USER_POS_HOLD_EXIT_DEG10) {
        s_posHoldActive = 0U;
      }
    } else if ((absErrDeg10 <= USER_POS_HOLD_ENTER_DEG10) &&
               (UserAbsI32((int32_t)speedRpm) <= USER_POS_SPEED_ZERO_WINDOW_RPM)) {
      s_posHoldActive = 1U;
    }
    dbg_posHoldActive = s_posHoldActive;

#if USER_POSITION_OUTPUT_MODE == USER_POS_OUTPUT_MODE_CURRENT
    {
      int32_t effectiveErrDeg10 = angleErrDeg10;
      int32_t targetCurrentMilliAmp;
      int32_t dampingCurrentMilliAmp;
      int32_t absSpeedRpm;

      dbg_posTargetRpm = 0;

      if (s_posHoldActive != 0U) {
        effectiveErrDeg10 = 0;
        targetCurrentMilliAmp = 0;
      } else {
        targetCurrentMilliAmp =
            (effectiveErrDeg10 * USER_POS_CURRENT_KP_MA_PER_DEG) / 10;
        dampingCurrentMilliAmp =
            (int32_t)speedRpm * (int32_t)USER_POS_CURRENT_KD_MA_PER_RPM;
        targetCurrentMilliAmp -= dampingCurrentMilliAmp;
        targetCurrentMilliAmp *= USER_POS_CURRENT_DIRECTION_SIGN;

        targetCurrentMilliAmp = UserClampI32(
            targetCurrentMilliAmp,
            -(int32_t)USER_POS_MAX_CURRENT_MA,
            (int32_t)USER_POS_MAX_CURRENT_MA);
      }

      absSpeedRpm = UserAbsI32(speedRpm);
      if ((effectiveErrDeg10 == 0) && (absSpeedRpm <= USER_POS_SPEED_ZERO_WINDOW_RPM)) {
        targetCurrentMilliAmp = 0;
      }

      if (targetCurrentMilliAmp > s_posCurrentCmdMilliAmp) {
        s_posCurrentCmdMilliAmp += USER_POS_CURRENT_SLEW_MA_PER_TICK;
        if (s_posCurrentCmdMilliAmp > targetCurrentMilliAmp) {
          s_posCurrentCmdMilliAmp = targetCurrentMilliAmp;
        }
      } else if (targetCurrentMilliAmp < s_posCurrentCmdMilliAmp) {
        s_posCurrentCmdMilliAmp -= USER_POS_CURRENT_SLEW_MA_PER_TICK;
        if (s_posCurrentCmdMilliAmp < targetCurrentMilliAmp) {
          s_posCurrentCmdMilliAmp = targetCurrentMilliAmp;
        }
      }

      if ((UserAbsI32(s_posCurrentCmdMilliAmp) <= USER_POS_CURRENT_ZERO_WINDOW_MA) &&
          (s_posHoldActive != 0U)) {
        s_posCurrentCmdMilliAmp = 0;
      }

      dbg_posCurrentCmdMa = (int16_t)s_posCurrentCmdMilliAmp;
      if ((s_posCurrentCmdMilliAmp == 0) && (s_ctrlState == CTRL_IDLE) && (dbg_motorState == IDLE)) {
        /* Keep the drive off until there is a real position demand. */
      } else {
        StartOrUpdateCurrentCommand(s_posCurrentCmdMilliAmp);
      }
    }
#else
    {
      int32_t rawTargetRpm = 0;
      int32_t targetRpm = 0;
      int32_t speedErrRpm = 0;
      int32_t targetCurrentMilliAmp = 0;
      int32_t absTargetRpm;
      int32_t filteredSpeedRpm = (int32_t)speedRpm;

      if (s_posHoldActive != 0U) {
        s_posTargetRpmCmd = 0;
        dbg_posTargetRpm = 0;
        dbg_posSpeedErrRpm = 0;
        targetCurrentMilliAmp = 0;
      } else {
        if (USER_POS_SPEED_LPF_SHIFT > 0) {
          s_posFilteredSpeedRpm +=
              (filteredSpeedRpm - s_posFilteredSpeedRpm) >> USER_POS_SPEED_LPF_SHIFT;
          filteredSpeedRpm = s_posFilteredSpeedRpm;
        }

        rawTargetRpm =
            (angleErrDeg10 * (int32_t)USER_POS_SPEED_KP_RPM_PER_DEG) / 10;
        rawTargetRpm -= filteredSpeedRpm * (int32_t)USER_POS_SPEED_KD_RPM_PER_RPM;
        rawTargetRpm *= USER_POS_SPEED_DIRECTION_SIGN;
        rawTargetRpm = UserClampI32(rawTargetRpm,
                                    -(int32_t)USER_POS_SPEED_OUT_MAX_RPM,
                                    (int32_t)USER_POS_SPEED_OUT_MAX_RPM);

        absTargetRpm = UserAbsI32(rawTargetRpm);
        if ((absTargetRpm > 0) &&
            (absTargetRpm < (int32_t)USER_POS_SPEED_OUT_MIN_RPM)) {
          rawTargetRpm = (rawTargetRpm > 0) ? USER_POS_SPEED_OUT_MIN_RPM
                                            : -(int32_t)USER_POS_SPEED_OUT_MIN_RPM;
        }

        if (rawTargetRpm > s_posTargetRpmCmd) {
          s_posTargetRpmCmd += USER_POS_TARGET_RPM_SLEW_RPM_PER_TICK;
          if (s_posTargetRpmCmd > rawTargetRpm) {
            s_posTargetRpmCmd = rawTargetRpm;
          }
        } else if (rawTargetRpm < s_posTargetRpmCmd) {
          s_posTargetRpmCmd -= USER_POS_TARGET_RPM_SLEW_RPM_PER_TICK;
          if (s_posTargetRpmCmd < rawTargetRpm) {
            s_posTargetRpmCmd = rawTargetRpm;
          }
        }

        targetRpm = s_posTargetRpmCmd;
        speedErrRpm = targetRpm - filteredSpeedRpm;
        targetCurrentMilliAmp =
            speedErrRpm * (int32_t)USER_POS_SPEED_TO_CURRENT_KP_MA_PER_RPM;
        targetCurrentMilliAmp *= USER_POS_SPEED_TO_CURRENT_DIRECTION_SIGN;
        targetCurrentMilliAmp = UserClampI32(
            targetCurrentMilliAmp,
            -(int32_t)USER_POS_MAX_CURRENT_MA,
            (int32_t)USER_POS_MAX_CURRENT_MA);

        dbg_posTargetRpm = (int16_t)targetRpm;
        dbg_posSpeedErrRpm = (int16_t)UserClampI32(speedErrRpm, INT16_MIN, INT16_MAX);
      }

      if (targetCurrentMilliAmp > s_posCurrentCmdMilliAmp) {
        s_posCurrentCmdMilliAmp += USER_POS_CURRENT_SLEW_MA_PER_TICK;
        if (s_posCurrentCmdMilliAmp > targetCurrentMilliAmp) {
          s_posCurrentCmdMilliAmp = targetCurrentMilliAmp;
        }
      } else if (targetCurrentMilliAmp < s_posCurrentCmdMilliAmp) {
        s_posCurrentCmdMilliAmp -= USER_POS_CURRENT_SLEW_MA_PER_TICK;
        if (s_posCurrentCmdMilliAmp < targetCurrentMilliAmp) {
          s_posCurrentCmdMilliAmp = targetCurrentMilliAmp;
        }
      }

      if ((UserAbsI32(s_posCurrentCmdMilliAmp) <= USER_POS_CURRENT_ZERO_WINDOW_MA) &&
          (s_posHoldActive != 0U)) {
        s_posCurrentCmdMilliAmp = 0;
      }

      dbg_posCurrentCmdMa = (int16_t)s_posCurrentCmdMilliAmp;

      if ((s_posCurrentCmdMilliAmp == 0) && (s_ctrlState == CTRL_IDLE) &&
          (dbg_motorState == IDLE)) {
        /* Keep the drive off until there is a real position demand. */
      } else {
        StartOrUpdateCurrentCommand(s_posCurrentCmdMilliAmp);
      }
    }
#endif
  } else {
    dbg_actualAngleDeg10 = 0U;
    dbg_actualSignedAngleDeg10 = 0;
    dbg_virtualAngleDeg10 = 0U;
    dbg_angleErrDeg10 = 0;
    dbg_posSpeedErrRpm = 0;
    ResetPositionControllerState();
    StopMotorControlIfRunning();
  }
}

static void StartOrUpdateCurrentCommand(int32_t targetCurrentMilliAmp) {
  qd_t iqdRef = {0};
  MCI_State_t state = MCI_GetSTMState(pMCI[M1]);

  iqdRef.q = CurrentMilliAmpToDigit(targetCurrentMilliAmp);
  iqdRef.d = 0;

  switch (s_ctrlState) {
  case CTRL_IDLE:
    if (state == IDLE) {
      MC_SetCurrentReferenceMotor1(iqdRef);
      MC_StartMotor1();
      s_ctrlState = CTRL_RUNNING;
    } else if (state == FAULT_OVER) {
      MC_AcknowledgeFaultMotor1();
    }
    break;

  case CTRL_RUNNING:
    if ((state == RUN) || (state == START) || (state == CHARGE_BOOT_CAP) ||
        (state == OFFSET_CALIB)) {
      MC_SetCurrentReferenceMotor1(iqdRef);
    } else if ((state == FAULT_NOW) || (state == FAULT_OVER) || (state == IDLE)) {
      s_ctrlState = CTRL_IDLE;
    }
    break;
  }
}
#endif

static void StopMotorDriveIfRunning(void) {
  if (s_ctrlState != CTRL_IDLE) {
    MC_StopMotor1();
    s_ctrlState = CTRL_IDLE;
  }
}

static void StopMotorControlIfRunning(void) {
  StopMotorDriveIfRunning();
#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED)
  ResetPositionControllerState();
#endif
#if USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
  s_followLastTickMs = HAL_GetTick();
#endif
}

void UserApp_HandleUserButton(void) {
#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED)
  uint32_t nowTickMs = HAL_GetTick();

  if ((nowTickMs - s_userButtonLastTickMs) < USER_BUTTON_DEBOUNCE_MS) {
    return;
  }
  s_userButtonLastTickMs = nowTickMs;
  s_posZeroCalRequest = 1U;
#else
  /* No user-owned button action in the current control mode. */
#endif
}

void UserApp_PostMediumFrequencyHook_M1(uint16_t rawValue) {
  MCI_State_t state = MCI_GetSTMState(pMCI[M1]);

  dbg_potRaw = rawValue;
  dbg_motorState = (uint16_t)state;
  UpdateAs5600Telemetry();

#if USER_CONTROL_MODE == USER_CONTROL_MODE_THROTTLE
  {
    int16_t throttlePct = PotToSignedPercent(rawValue);

    dbg_throttlePct = throttlePct;
    dbg_targetAngleDeg10 = 0U;
    dbg_targetSignedAngleDeg10 = 0;
    dbg_angleErrDeg10 = 0;
    dbg_actualSignedAngleDeg10 = 0;
    dbg_posCurrentCmdMa = 0;
    dbg_posTargetRpm = 0;
    dbg_posSpeedErrRpm = 0;

    if (throttlePct == 0) {
      StopMotorControlIfRunning();
      if (state == FAULT_OVER) {
        MC_AcknowledgeFaultMotor1();
      }
    } else {
      int32_t absPct = UserAbsI32(throttlePct);
      int32_t targetRpm =
          USER_THR_MIN_RPM + ((USER_THR_MAX_RPM - USER_THR_MIN_RPM) * absPct) / 100;
      if (throttlePct < 0) {
        targetRpm = -targetRpm;
      }
      StartOrUpdateSpeedCommand(targetRpm, USER_THR_RAMP_MS);
    }
  }
#elif USER_CONTROL_MODE == USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST
  {
    int16_t throttlePct = PotToSignedPercent(rawValue);
    int32_t targetRpm = USER_TEST_SPIN_RPM;

    dbg_throttlePct = throttlePct;
    dbg_targetAngleDeg10 = 0U;
    dbg_actualAngleDeg10 = dbg_sourceAngleDeg10;
    dbg_targetSignedAngleDeg10 = 0;
    dbg_actualSignedAngleDeg10 = 0;
    dbg_virtualAngleDeg10 = 0U;
    dbg_angleErrDeg10 = 0;
    dbg_posCurrentCmdMa = 0;
    dbg_posTargetRpm = 0;
    dbg_posSpeedErrRpm = 0;

    if (throttlePct == 0) {
      StopMotorControlIfRunning();
      if (state == FAULT_OVER) {
        MC_AcknowledgeFaultMotor1();
      }
    } else {
      if (throttlePct < 0) {
        targetRpm = -targetRpm;
      }
      targetRpm *= USER_TEST_SPIN_DIRECTION_SIGN;
      StartOrUpdateSpeedCommand(targetRpm, USER_TEST_SPIN_RAMP_MS);
    }
  }
#elif USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600
  {
    uint16_t rawTargetAngleDeg10 = PotToAngleDeg10(rawValue);

    dbg_throttlePct = 0;
    dbg_uartTargetOnline = 0U;
    dbg_uartTargetAngleDeg10 = 0U;
    dbg_uartTargetFrameCount = 0U;
    dbg_uartTargetErrorCount = 0U;

#if USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED
    RunSensoredPositionController(rawTargetAngleDeg10);
#else
    {
      uint16_t as5600Raw = 0U;

      if (AS5600_ReadRawAngle(&as5600Raw) != 0U) {
        int16_t actualAngleDeg10 = (int16_t)AS5600_RawToAngleDeg10(as5600Raw);
        int16_t angleErrDeg10 = WrapAngleErrorDeg10((int16_t)targetAngleDeg10, actualAngleDeg10);
        int32_t absErrDeg10 = UserAbsI32(angleErrDeg10);

        dbg_as5600ReadOk = 1U;
        dbg_as5600Raw = as5600Raw;
        dbg_sourceAngleDeg10 = (uint16_t)actualAngleDeg10;
        dbg_actualAngleDeg10 = (uint16_t)actualAngleDeg10;
        dbg_targetSignedAngleDeg10 = rawTargetSignedAngleDeg10;
        dbg_actualSignedAngleDeg10 = actualAngleDeg10;
        dbg_virtualAngleDeg10 = 0U;
        dbg_angleErrDeg10 = angleErrDeg10;
        dbg_posSpeedErrRpm = 0;

        if (absErrDeg10 <= (USER_POS_ANGLE_DEADBAND_DEG * 10)) {
          StopMotorControlIfRunning();
        } else {
          int32_t targetRpm = (absErrDeg10 * USER_POS_SPEED_KP_RPM_PER_DEG) / 10;
          targetRpm = UserClampI32(targetRpm, USER_POS_MIN_RPM, USER_POS_MAX_RPM);
          if (angleErrDeg10 < 0) {
            targetRpm = -targetRpm;
          }
          targetRpm *= USER_POS_SPEED_DIRECTION_SIGN;
          StartOrUpdateSpeedCommand(targetRpm, USER_POS_RAMP_MS);
        }
      } else {
        dbg_as5600ReadOk = 0U;
        dbg_as5600Raw = 0U;
        dbg_sourceAngleDeg10 = 0U;
        dbg_actualAngleDeg10 = 0U;
        dbg_targetSignedAngleDeg10 = 0;
        dbg_actualSignedAngleDeg10 = 0;
        dbg_virtualAngleDeg10 = 0U;
        dbg_angleErrDeg10 = 0;
        dbg_posSpeedErrRpm = 0;
        StopMotorControlIfRunning();
      }
    }
#endif
  }
#elif USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600
  {
    uint16_t rawTargetAngleDeg10 = 0U;
    uint8_t hasRemoteTarget;

    dbg_throttlePct = 0;
    UserPlatform_PollRemoteTargetUart();
    dbg_uartTargetOnline = UserPlatform_IsRemoteTargetOnline();
    dbg_uartTargetFrameCount = UserPlatform_GetRemoteTargetFrameCount();
    dbg_uartTargetErrorCount = UserPlatform_GetRemoteTargetErrorCount();
    hasRemoteTarget = UserPlatform_GetRemoteTargetAngleDeg10(&rawTargetAngleDeg10);

    if (hasRemoteTarget != 0U) {
      dbg_uartTargetAngleDeg10 = rawTargetAngleDeg10;
      RunSensoredPositionController(rawTargetAngleDeg10);
    } else if (UserSensored_IsOnline() != 0U) {
      rawTargetAngleDeg10 = UserSensored_GetMechanicalAngleDeg10();
      dbg_uartTargetAngleDeg10 = rawTargetAngleDeg10;
      RunSensoredPositionController(rawTargetAngleDeg10);
    } else {
      dbg_uartTargetAngleDeg10 = 0U;
      dbg_targetAngleDeg10 = 0U;
      dbg_rawTargetAngleDeg10 = 0U;
      dbg_targetSignedAngleDeg10 = 0;
      dbg_actualAngleDeg10 = 0U;
      dbg_actualSignedAngleDeg10 = 0;
      dbg_virtualAngleDeg10 = 0U;
      dbg_angleErrDeg10 = 0;
      dbg_posCurrentCmdMa = 0;
      dbg_posTargetRpm = 0;
      dbg_posSpeedErrRpm = 0;
      ResetPositionControllerState();
      StopMotorControlIfRunning();
    }
  }
#elif USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600
  {
    uint16_t rawTargetAngleDeg10 = 0U;
    uint8_t hasPwmTarget;

    dbg_throttlePct = 0;
    dbg_uartTargetOnline = 0U;
    dbg_uartTargetAngleDeg10 = 0U;
    dbg_uartTargetFrameCount = 0U;
    dbg_uartTargetErrorCount = 0U;

    dbg_pwmTargetOnline = UserPlatform_IsPwmTargetOnline();
    dbg_pwmTargetPulseWidthUs = UserPlatform_GetPwmTargetPulseWidthUs();
    dbg_pwmTargetFrameCount = UserPlatform_GetPwmTargetFrameCount();
    dbg_pwmTargetErrorCount = UserPlatform_GetPwmTargetErrorCount();
    hasPwmTarget = UserPlatform_GetPwmTargetAngleDeg10(&rawTargetAngleDeg10);

    if (hasPwmTarget != 0U) {
      dbg_pwmTargetAngleDeg10 = rawTargetAngleDeg10;
      RunSensoredPositionController(rawTargetAngleDeg10);
    } else if (UserSensored_IsOnline() != 0U) {
      rawTargetAngleDeg10 = UserSensored_GetMechanicalAngleDeg10();
      dbg_pwmTargetAngleDeg10 = rawTargetAngleDeg10;
      RunSensoredPositionController(rawTargetAngleDeg10);
    } else {
      dbg_pwmTargetAngleDeg10 = 0U;
      dbg_pwmTargetPulseWidthUs = 0U;
      dbg_targetAngleDeg10 = 0U;
      dbg_rawTargetAngleDeg10 = 0U;
      dbg_targetSignedAngleDeg10 = 0;
      dbg_actualAngleDeg10 = 0U;
      dbg_actualSignedAngleDeg10 = 0;
      dbg_virtualAngleDeg10 = 0U;
      dbg_angleErrDeg10 = 0;
      dbg_posCurrentCmdMa = 0;
      dbg_posTargetRpm = 0;
      dbg_posSpeedErrRpm = 0;
      ResetPositionControllerState();
      StopMotorControlIfRunning();
    }
  }
#elif USER_CONTROL_MODE == USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
  {
    uint16_t knobRaw = 0U;

    dbg_throttlePct = 0;
    dbg_posCurrentCmdMa = 0;
    dbg_posTargetRpm = 0;
    dbg_posSpeedErrRpm = 0;

    if (AS5600_ReadRawAngle(&knobRaw) != 0U) {
      uint16_t knobAngleDeg10 = AS5600_RawToAngleDeg10(knobRaw);
      int16_t angleErrDeg10;
      int32_t absErrDeg10;

      dbg_as5600ReadOk = 1U;
      dbg_as5600Raw = knobRaw;
      dbg_sourceAngleDeg10 = knobAngleDeg10;
      dbg_targetAngleDeg10 = knobAngleDeg10;
      dbg_targetSignedAngleDeg10 = 0;
      dbg_actualSignedAngleDeg10 = 0;

      if ((s_followEstimateSeeded == 0U) && (USER_FOLLOW_BOOT_ASSUME_ALIGNED != 0U)) {
        SeedFollowEstimate(knobAngleDeg10);
      }

      UpdateFollowEstimateFromMeasuredSpeed();
      dbg_actualAngleDeg10 = dbg_virtualAngleDeg10;

      if (s_followEstimateSeeded == 0U) {
        dbg_angleErrDeg10 = 0;
        StopMotorControlIfRunning();
      } else {
        /*
         * This is an approximate follower mode. Without a second shaft sensor on
         * the 2216, follower angle is estimated from measured speed only.
         */
        angleErrDeg10 =
            WrapAngleErrorDeg10((int16_t)knobAngleDeg10, (int16_t)s_followEstimatedAngleDeg10);
        absErrDeg10 = UserAbsI32(angleErrDeg10);
        dbg_angleErrDeg10 = angleErrDeg10;

        if (absErrDeg10 <= (USER_FOLLOW_ANGLE_DEADBAND_DEG * 10)) {
          StopMotorControlIfRunning();
        } else {
          int32_t targetRpm = (absErrDeg10 * USER_FOLLOW_SPEED_KP_RPM_PER_DEG) / 10;
          targetRpm = UserClampI32(targetRpm, USER_FOLLOW_MIN_RPM, USER_FOLLOW_MAX_RPM);
          if (angleErrDeg10 < 0) {
            targetRpm = -targetRpm;
          }
          targetRpm *= USER_FOLLOW_DIRECTION_SIGN;
          StartOrUpdateSpeedCommand(targetRpm, USER_FOLLOW_RAMP_MS);
        }
      }
    } else {
      dbg_as5600ReadOk = 0U;
      dbg_as5600Raw = 0U;
      dbg_sourceAngleDeg10 = 0U;
      dbg_targetAngleDeg10 = 0U;
      dbg_targetSignedAngleDeg10 = 0;
      dbg_actualSignedAngleDeg10 = 0;
      dbg_angleErrDeg10 = 0;
      StopMotorControlIfRunning();
    }
  }
#else
#error Unsupported USER_CONTROL_MODE
#endif

  dbg_cmdRPM = (int16_t)(MCI_GetMecSpeedRefUnit(pMCI[M1]) * U_RPM / SPEED_UNIT);
#if ((USER_CONTROL_MODE == USER_CONTROL_MODE_POSITION_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_UART_TARGET_AS5600) || \
     (USER_CONTROL_MODE == USER_CONTROL_MODE_PWM_TARGET_AS5600)) && \
    (USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED) && \
    (USER_POSITION_OUTPUT_MODE == USER_POS_OUTPUT_MODE_SPEED)
  dbg_cmdRPM = dbg_posTargetRpm;
#endif
  dbg_actualRPM = (int16_t)(MCI_GetAvrgMecSpeedUnit(pMCI[M1]) * U_RPM / SPEED_UNIT);
  dbg_currentFaults = MC_GetCurrentFaultsMotor1();
  dbg_occurredFaults = MC_GetOccurredFaultsMotor1();
  dbg_commandState = (uint16_t)MC_GetCommandStateMotor1();
}
