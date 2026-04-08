#include "user_sensored.h"

#include "parameters_conversion.h"
#include "user_config.h"
#include "user_platform.h"

typedef struct {
  SpeednPosFdbk_Handle_t _Super;
  uint8_t initialized;
  uint8_t online;
  uint8_t aligned;
  uint8_t hasSample;
  uint16_t rawAngle;
  uint16_t mechanicalAngleDeg10;
  uint16_t electricalAngleDeg10;
  uint16_t alignmentMechanicalAngleDeg10;
  int16_t mechanicalSpeedRpm;
  int16_t predictedElectricalAngleS16;
  uint32_t lastSampleTickMs;
  uint16_t lastMechanicalAngleDeg10;
} UserSensored_Handle_t;

static UserSensored_Handle_t s_userSensored;

static int32_t UserAbsI32(int32_t value) { return (value < 0) ? -value : value; }

static uint16_t NormalizeAngleDeg10(int32_t angleDeg10) {
  while (angleDeg10 < 0) {
    angleDeg10 += 3600;
  }
  while (angleDeg10 >= 3600) {
    angleDeg10 -= 3600;
  }
  return (uint16_t)angleDeg10;
}

static int16_t WrapDeltaAngleDeg10(uint16_t currentDeg10, uint16_t previousDeg10) {
  int32_t delta = (int32_t)currentDeg10 - (int32_t)previousDeg10;

  while (delta > 1800) {
    delta -= 3600;
  }
  while (delta < -1800) {
    delta += 3600;
  }
  return (int16_t)delta;
}

static int16_t Deg10ToS16(uint16_t angleDeg10) {
  uint32_t normalized = NormalizeAngleDeg10((int32_t)angleDeg10);
  return (int16_t)((normalized * 65536UL) / 3600UL);
}

static uint16_t RawToMechanicalAngleDeg10(uint16_t rawAngle) {
  return (uint16_t)((rawAngle * 3600UL) / 4096UL);
}

static int16_t MechanicalSpeedRpmToElSpeedDpp(int16_t mechanicalSpeedRpm) {
  int32_t electricalRpm =
      (int32_t)mechanicalSpeedRpm * (int32_t)USER_SENSORED_DIRECTION_SIGN * USER_MOTOR_PROFILE_POLE_PAIRS;
  return (int16_t)((electricalRpm * 65536L) / (60L * (int32_t)TF_REGULATION_RATE));
}

static uint16_t MechanicalToElectricalAngleDeg10(uint16_t mechanicalAngleDeg10) {
  int32_t mechanicalDeltaDeg10 =
      (int32_t)NormalizeAngleDeg10((int32_t)mechanicalAngleDeg10 -
                                   (int32_t)s_userSensored.alignmentMechanicalAngleDeg10);
  int32_t electricalAngleDeg10 =
      (mechanicalDeltaDeg10 * (int32_t)USER_MOTOR_PROFILE_POLE_PAIRS *
       (int32_t)USER_SENSORED_DIRECTION_SIGN) +
      (int32_t)USER_SENSORED_ALIGN_LOCK_ELEC_DEG10 +
      (int32_t)USER_SENSORED_ELEC_TRIM_DEG10;

  return NormalizeAngleDeg10(electricalAngleDeg10);
}

static void ClearFeedbackState(void) {
  s_userSensored._Super.hAvrMecSpeedUnit = 0;
  s_userSensored._Super.hElSpeedDpp = 0;
  s_userSensored._Super.InstantaneousElSpeedDpp = 0;
  s_userSensored.mechanicalSpeedRpm = 0;
}

void UserSensored_Init(void) {
  if (s_userSensored.initialized != 0U) {
    return;
  }

  s_userSensored._Super.bElToMecRatio = USER_MOTOR_PROFILE_POLE_PAIRS;
  s_userSensored._Super.SpeedUnit = SPEED_UNIT;
  s_userSensored._Super.bMaximumSpeedErrorsNumber = M1_SS_MEAS_ERRORS_BEFORE_FAULTS;
  s_userSensored._Super.hMaxReliableMecSpeedUnit = MAX_APPLICATION_SPEED_UNIT;
  s_userSensored._Super.hMinReliableMecSpeedUnit = 0U;
  s_userSensored._Super.hMaxReliableMecAccelUnitP = 65535U;
  s_userSensored._Super.hMeasurementFrequency = TF_REGULATION_RATE_SCALED;
  s_userSensored._Super.DPPConvFactor = DPP_CONV_FACTOR;
  s_userSensored.initialized = 1U;
}

void UserSensored_ResetRuntime(void) {
  UserSensored_Init();
  s_userSensored.online = 0U;
  s_userSensored.hasSample = 0U;
  s_userSensored.lastSampleTickMs = 0U;
  s_userSensored.predictedElectricalAngleS16 = Deg10ToS16(s_userSensored.electricalAngleDeg10);
  s_userSensored._Super.bSpeedErrorNumber = 0U;
  ClearFeedbackState();
}

void UserSensored_InvalidateAlignment(void) {
  UserSensored_Init();
  s_userSensored.aligned = 0U;
  s_userSensored.alignmentMechanicalAngleDeg10 = 0U;
  s_userSensored.electricalAngleDeg10 = 0U;
  s_userSensored._Super.hElAngle = 0;
  s_userSensored.predictedElectricalAngleS16 = 0;
  ClearFeedbackState();
}

void UserSensored_MediumFrequencyUpdate(void) {
  uint16_t rawAngle = 0U;
  uint16_t mechanicalAngleDeg10;
  uint32_t nowTickMs;
  int16_t instantMechanicalSpeedRpm = 0;

  UserSensored_Init();

  if (UserPlatform_ReadAs5600RawAngle(&rawAngle) == 0U) {
    s_userSensored.online = 0U;
    s_userSensored._Super.bSpeedErrorNumber =
        s_userSensored._Super.bMaximumSpeedErrorsNumber;
    ClearFeedbackState();
    return;
  }

  nowTickMs = HAL_GetTick();
  mechanicalAngleDeg10 = RawToMechanicalAngleDeg10(rawAngle);

  s_userSensored.online = 1U;
  s_userSensored.rawAngle = rawAngle;
  s_userSensored.mechanicalAngleDeg10 = mechanicalAngleDeg10;
  s_userSensored._Super.hMecAngle = Deg10ToS16(mechanicalAngleDeg10);
  s_userSensored._Super.wMecAngle =
      ((int32_t)mechanicalAngleDeg10 * 65536L) / 3600L;

  if ((s_userSensored.hasSample != 0U) && (nowTickMs != s_userSensored.lastSampleTickMs)) {
    uint32_t deltaTickMs = nowTickMs - s_userSensored.lastSampleTickMs;
    int32_t deltaAngleDeg10 =
        (int32_t)WrapDeltaAngleDeg10(mechanicalAngleDeg10, s_userSensored.lastMechanicalAngleDeg10);

    instantMechanicalSpeedRpm =
        (int16_t)((deltaAngleDeg10 * 60000L) / (3600L * (int32_t)deltaTickMs));
  }

  s_userSensored.mechanicalSpeedRpm +=
      (int16_t)(((int32_t)instantMechanicalSpeedRpm -
                 (int32_t)s_userSensored.mechanicalSpeedRpm) /
                (1 << USER_POS_SPEED_LPF_SHIFT));

  if (UserAbsI32((int32_t)s_userSensored.mechanicalSpeedRpm) <=
      USER_SENSORED_SPEED_ZERO_WINDOW_RPM) {
    s_userSensored.mechanicalSpeedRpm = 0;
  }

  s_userSensored.lastMechanicalAngleDeg10 = mechanicalAngleDeg10;
  s_userSensored.lastSampleTickMs = nowTickMs;
  s_userSensored.hasSample = 1U;

  s_userSensored._Super.hAvrMecSpeedUnit =
      (int16_t)((s_userSensored.mechanicalSpeedRpm * SPEED_UNIT) / U_RPM);
  s_userSensored._Super.hElSpeedDpp =
      MechanicalSpeedRpmToElSpeedDpp(s_userSensored.mechanicalSpeedRpm);
  s_userSensored._Super.InstantaneousElSpeedDpp = s_userSensored._Super.hElSpeedDpp;

  if (s_userSensored.aligned != 0U) {
    s_userSensored.electricalAngleDeg10 =
        MechanicalToElectricalAngleDeg10(mechanicalAngleDeg10);
    s_userSensored.predictedElectricalAngleS16 =
        Deg10ToS16(s_userSensored.electricalAngleDeg10);
    s_userSensored._Super.hElAngle = s_userSensored.predictedElectricalAngleS16;
  }

  s_userSensored._Super.bSpeedErrorNumber = 0U;
}

void UserSensored_HighFrequencyUpdate(void) {
  UserSensored_Init();

  if ((s_userSensored.online == 0U) || (s_userSensored.aligned == 0U)) {
    return;
  }

  s_userSensored.predictedElectricalAngleS16 =
      (int16_t)(s_userSensored.predictedElectricalAngleS16 +
                s_userSensored._Super.InstantaneousElSpeedDpp);
  s_userSensored._Super.hElAngle = s_userSensored.predictedElectricalAngleS16;
}

uint8_t UserSensored_CaptureAlignment(void) {
  UserSensored_MediumFrequencyUpdate();

  if (s_userSensored.online == 0U) {
    return 0U;
  }

  s_userSensored.alignmentMechanicalAngleDeg10 = s_userSensored.mechanicalAngleDeg10;
  s_userSensored.aligned = 1U;
  s_userSensored.electricalAngleDeg10 =
      MechanicalToElectricalAngleDeg10(s_userSensored.mechanicalAngleDeg10);
  s_userSensored.predictedElectricalAngleS16 =
      Deg10ToS16(s_userSensored.electricalAngleDeg10);
  s_userSensored._Super.hElAngle = s_userSensored.predictedElectricalAngleS16;
  return 1U;
}

SpeednPosFdbk_Handle_t *UserSensored_GetSpeedSensor(void) {
  UserSensored_Init();
  return &s_userSensored._Super;
}

uint8_t UserSensored_IsOnline(void) { return s_userSensored.online; }

uint8_t UserSensored_IsAligned(void) { return s_userSensored.aligned; }

uint16_t UserSensored_GetRawAngle(void) { return s_userSensored.rawAngle; }

uint16_t UserSensored_GetMechanicalAngleDeg10(void) {
  return s_userSensored.mechanicalAngleDeg10;
}

uint16_t UserSensored_GetElectricalAngleDeg10(void) {
  return s_userSensored.electricalAngleDeg10;
}

uint16_t UserSensored_GetAlignmentMechanicalAngleDeg10(void) {
  return s_userSensored.alignmentMechanicalAngleDeg10;
}

int16_t UserSensored_GetMechanicalSpeedRpm(void) {
  return s_userSensored.mechanicalSpeedRpm;
}
