#include "main.h"
#include "mc_config.h"
#include "mc_config_common.h"
#include "mc_interface.h"
#include "mc_math.h"
#include "mc_tasks.h"
#include "parameters_conversion.h"
#include "pwm_common.h"
#include "regular_conversion_manager.h"
#include "user_config.h"
#include "user_sensored.h"

#if USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED

#define M1_CHARGE_BOOT_CAP_TICKS \
  (((uint16_t)SYS_TICK_FREQUENCY * (uint16_t)10) / 1000U)
#define M1_CHARGE_BOOT_CAP_DUTY_CYCLES \
  (uint32_t)(0.000 * ((uint32_t)PWM_PERIOD_CYCLES / 2U))

static uint32_t s_alignmentStartTickMs = 0U;

void FOC_InitAdditionalMethods(uint8_t bMotor);
void FOC_CalcCurrRef(uint8_t bMotor);

static int16_t UserSensored_GetLockElectricalAngleS16(void) {
  return (int16_t)((USER_SENSORED_ALIGN_LOCK_ELEC_DEG10 * 65536UL) / 3600UL);
}

static qd_t UserSensored_GetAlignmentCurrentReference(void) {
  qd_t iqdRef = {0};

  iqdRef.q = 0;
  iqdRef.d = (int16_t)(USER_SENSORED_ALIGN_CURRENT_A * CURRENT_CONV_FACTOR);
  return iqdRef;
}

static void UserSensored_StopProcessing(uint8_t motor) {
  R3_2_SwitchOffPWM(pwmcHandle[motor]);
  FOC_Clear(motor);
  STC_Clear(pSTC[motor]);
  TSK_SetStopPermanencyTimeM1(STOPPERMANENCY_TICKS);
  Mci[motor].State = STOP;
}

static void UserSensored_EnterRunState(void) {
  FOCVars[M1].Iqdref = STC_GetDefaultIqdref(pSTC[M1]);
  FOCVars[M1].UserIdref = FOCVars[M1].Iqdref.d;
  STC_SetSpeedSensor(pSTC[M1], UserSensored_GetSpeedSensor());
  FOC_InitAdditionalMethods(M1);
  FOC_CalcCurrRef(M1);
  STC_ForceSpeedReferenceToCurrentSpeed(pSTC[M1]);
  MCI_ExecBufferedCommands(&Mci[M1]);
  Mci[M1].State = RUN;
}

static void UserSensored_BeginAlignment(void) {
  qd_t iqdRef = UserSensored_GetAlignmentCurrentReference();

  VSS_Clear(&VirtualSpeedSensorM1);
  VSS_SetElAngle(&VirtualSpeedSensorM1, UserSensored_GetLockElectricalAngleS16());
  STC_SetSpeedSensor(pSTC[M1], &VirtualSpeedSensorM1._Super);
  FOCVars[M1].bDriveInput = EXTERNAL;
  FOCVars[M1].Iqdref = iqdRef;
  FOCVars[M1].UserIdref = iqdRef.d;
  s_alignmentStartTickMs = HAL_GetTick();
  PWMC_SwitchOnPWM(pwmcHandle[M1]);
  Mci[M1].State = START;
}

static uint16_t UserSensored_FOC_CurrControllerM1(void) {
  qd_t Iqd, Vqd;
  ab_t Iab;
  alphabeta_t Ialphabeta, Valphabeta;
  int16_t hElAngle;
  uint16_t hCodeError = MC_NO_FAULTS;
  SpeednPosFdbk_Handle_t *speedHandle;

  speedHandle = STC_GetSpeedSensor(pSTC[M1]);
  hElAngle = SPD_GetElAngle(speedHandle);
  hElAngle +=
      SPD_GetInstElSpeedDpp(speedHandle) * PARK_ANGLE_COMPENSATION_FACTOR;
  PWMC_GetPhaseCurrents(pwmcHandle[M1], &Iab);
  Ialphabeta = MCM_Clarke(Iab);
  Iqd = MCM_Park(Ialphabeta, hElAngle);

  if (PWMC_GetPWMState(pwmcHandle[M1]) == true) {
    Vqd.q = PI_Controller(pPIDIq[M1], (int32_t)(FOCVars[M1].Iqdref.q) - Iqd.q);
    Vqd.d = PI_Controller(pPIDId[M1], (int32_t)(FOCVars[M1].Iqdref.d) - Iqd.d);
  } else {
    Vqd.q = 0;
    Vqd.d = 0;
  }

  Vqd = Circle_Limitation(&CircleLimitationM1, Vqd);
  hElAngle +=
      SPD_GetInstElSpeedDpp(speedHandle) * REV_PARK_ANGLE_COMPENSATION_FACTOR;
  Valphabeta = MCM_Rev_Park(Vqd, hElAngle);

  if (PWMC_GetPWMState(pwmcHandle[M1]) == true) {
    hCodeError = PWMC_SetPhaseVoltage(pwmcHandle[M1], Valphabeta);
  }

  FOCVars[M1].Vqd = Vqd;
  FOCVars[M1].Iab = Iab;
  FOCVars[M1].Ialphabeta = Ialphabeta;
  FOCVars[M1].Iqd = Iqd;
  FOCVars[M1].Valphabeta = Valphabeta;
  FOCVars[M1].hElAngle = hElAngle;

  FW_DataProcess(pFW[M1], Vqd);
  return hCodeError;
}

void TSK_MediumFrequencyTaskM1(void) {
  PQD_CalcElMotorPower(pMPM[M1]);
  UserSensored_MediumFrequencyUpdate();

  if (MCI_GetCurrentFaults(&Mci[M1]) == MC_NO_FAULTS) {
    if (MCI_GetOccurredFaults(&Mci[M1]) == MC_NO_FAULTS) {
      switch (Mci[M1].State) {
      case IDLE:
        if ((MCI_START == Mci[M1].DirectCommand) ||
            (MCI_MEASURE_OFFSETS == Mci[M1].DirectCommand)) {
          if (pwmcHandle[M1]->offsetCalibStatus == false) {
            (void)PWMC_CurrentReadingCalibr(pwmcHandle[M1], CRC_START);
            Mci[M1].State = OFFSET_CALIB;
          } else {
            pwmcHandle[M1]->OffCalibrWaitTimeCounter = 1u;
            (void)PWMC_CurrentReadingCalibr(pwmcHandle[M1], CRC_EXEC);
            R3_2_TurnOnLowSides(pwmcHandle[M1], M1_CHARGE_BOOT_CAP_DUTY_CYCLES);
            TSK_SetChargeBootCapDelayM1(M1_CHARGE_BOOT_CAP_TICKS);
            Mci[M1].State = CHARGE_BOOT_CAP;
          }
        }
        break;

      case OFFSET_CALIB:
        if (MCI_STOP == Mci[M1].DirectCommand) {
          UserSensored_StopProcessing(M1);
        } else if (PWMC_CurrentReadingCalibr(pwmcHandle[M1], CRC_EXEC)) {
          if (MCI_MEASURE_OFFSETS == Mci[M1].DirectCommand) {
            FOC_Clear(M1);
            STC_Clear(pSTC[M1]);
            Mci[M1].DirectCommand = MCI_NO_COMMAND;
            Mci[M1].State = IDLE;
          } else {
            R3_2_TurnOnLowSides(pwmcHandle[M1], M1_CHARGE_BOOT_CAP_DUTY_CYCLES);
            TSK_SetChargeBootCapDelayM1(M1_CHARGE_BOOT_CAP_TICKS);
            Mci[M1].State = CHARGE_BOOT_CAP;
          }
        }
        break;

      case CHARGE_BOOT_CAP:
        if (MCI_STOP == Mci[M1].DirectCommand) {
          UserSensored_StopProcessing(M1);
        } else if (TSK_ChargeBootCapDelayHasElapsedM1()) {
          R3_2_SwitchOffPWM(pwmcHandle[M1]);
          FOCVars[M1].bDriveInput = EXTERNAL;
          FOC_Clear(M1);
          UserSensored_ResetRuntime();
          UserSensored_MediumFrequencyUpdate();

          if ((UserSensored_IsAligned() != 0U) && (UserSensored_IsOnline() != 0U)) {
            STC_SetSpeedSensor(pSTC[M1], UserSensored_GetSpeedSensor());
            PWMC_SwitchOnPWM(pwmcHandle[M1]);
            UserSensored_EnterRunState();
          } else {
            UserSensored_BeginAlignment();
          }
        }
        break;

      case START:
        if (MCI_STOP == Mci[M1].DirectCommand) {
          UserSensored_StopProcessing(M1);
        } else {
          qd_t iqdRef = UserSensored_GetAlignmentCurrentReference();

          FOCVars[M1].bDriveInput = EXTERNAL;
          FOCVars[M1].Iqdref = iqdRef;
          FOCVars[M1].UserIdref = iqdRef.d;

          if ((HAL_GetTick() - s_alignmentStartTickMs) >= USER_SENSORED_ALIGN_DURATION_MS) {
            if (UserSensored_CaptureAlignment() == 0U) {
              MCI_FaultProcessing(&Mci[M1], MC_SPEED_FDBK, 0);
            } else {
              UserSensored_EnterRunState();
            }
          }
        }
        break;

      case RUN:
        if (MCI_STOP == Mci[M1].DirectCommand) {
          UserSensored_StopProcessing(M1);
        } else {
          MCI_ExecBufferedCommands(&Mci[M1]);
          FOC_CalcCurrRef(M1);
          if (!SPD_Check(UserSensored_GetSpeedSensor())) {
            MCI_FaultProcessing(&Mci[M1], MC_SPEED_FDBK, 0);
          }
        }
        break;

      case STOP:
        if (TSK_StopPermanencyTimeHasElapsedM1()) {
          STC_SetSpeedSensor(pSTC[M1], &VirtualSpeedSensorM1._Super);
          VSS_Clear(&VirtualSpeedSensorM1);
          UserSensored_ResetRuntime();
          Mci[M1].DirectCommand = MCI_NO_COMMAND;
          Mci[M1].State = IDLE;
        }
        break;

      case FAULT_OVER:
        if (MCI_ACK_FAULTS == Mci[M1].DirectCommand) {
          Mci[M1].DirectCommand = MCI_NO_COMMAND;
          Mci[M1].State = IDLE;
        }
        break;

      case FAULT_NOW:
        Mci[M1].State = FAULT_OVER;
        break;

      default:
        break;
      }
    } else {
      Mci[M1].State = FAULT_OVER;
    }
  } else {
    Mci[M1].State = FAULT_NOW;
  }
}

uint8_t FOC_HighFrequencyTask(uint8_t bMotorNbr) {
  uint16_t hFOCreturn;

  RCM_ReadOngoingConv();
  RCM_ExecNextConv();

  if ((Mci[M1].State != IDLE) && (Mci[M1].State != FAULT_NOW) &&
      (Mci[M1].State != FAULT_OVER)) {
    UserSensored_HighFrequencyUpdate();
  }

  hFOCreturn = UserSensored_FOC_CurrControllerM1();
  if (hFOCreturn == MC_DURATION) {
    MCI_FaultProcessing(&Mci[M1], MC_DURATION, 0);
  }

  return bMotorNbr;
}

#endif /* USER_FOC_BACKEND == USER_FOC_BACKEND_AS5600_SENSORED */
