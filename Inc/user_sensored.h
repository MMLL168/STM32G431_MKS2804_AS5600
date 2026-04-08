#ifndef USER_SENSORED_H
#define USER_SENSORED_H

#include <stdint.h>

#include "mc_type.h"
#include "speed_pos_fdbk.h"

#ifdef __cplusplus
extern "C" {
#endif

void UserSensored_Init(void);
void UserSensored_ResetRuntime(void);
void UserSensored_InvalidateAlignment(void);
void UserSensored_MediumFrequencyUpdate(void);
void UserSensored_HighFrequencyUpdate(void);
uint8_t UserSensored_CaptureAlignment(void);

SpeednPosFdbk_Handle_t *UserSensored_GetSpeedSensor(void);
uint8_t UserSensored_IsOnline(void);
uint8_t UserSensored_IsAligned(void);
uint16_t UserSensored_GetRawAngle(void);
uint16_t UserSensored_GetMechanicalAngleDeg10(void);
uint16_t UserSensored_GetElectricalAngleDeg10(void);
uint16_t UserSensored_GetAlignmentMechanicalAngleDeg10(void);
int16_t UserSensored_GetMechanicalSpeedRpm(void);

#ifdef __cplusplus
}
#endif

#endif /* USER_SENSORED_H */
