#ifndef USER_PLATFORM_H
#define USER_PLATFORM_H

#include "main.h"
#include "stm32g4xx_hal_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PB7/PB8 are intentionally managed in user-owned code so CubeMX / MC
 * Workbench regeneration does not overwrite the AS5600 path. Keep these pins
 * unassigned in generated peripheral configuration.
 */
#define USER_AS5600_SDA_Pin GPIO_PIN_7
#define USER_AS5600_SDA_GPIO_Port GPIOB
#define USER_AS5600_SCL_Pin GPIO_PIN_8
#define USER_AS5600_SCL_GPIO_Port GPIOB
#define USER_PWM_TARGET_Pin GPIO_PIN_15
#define USER_PWM_TARGET_GPIO_Port GPIOA

void UserPlatform_Init(void);
uint8_t UserPlatform_IsAs5600I2CReady(void);
I2C_HandleTypeDef *UserPlatform_GetAs5600I2C(void);
uint8_t UserPlatform_ReadAs5600RawAngle(uint16_t *rawAngle);
void UserPlatform_HandlePwmTargetEdgeInterrupt(void);
uint8_t UserPlatform_HasPwmTarget(void);
uint8_t UserPlatform_IsPwmTargetOnline(void);
uint8_t UserPlatform_GetPwmTargetAngleDeg10(uint16_t *angleDeg10);
uint16_t UserPlatform_GetPwmTargetPulseWidthUs(void);
uint32_t UserPlatform_GetPwmTargetFrameCount(void);
uint32_t UserPlatform_GetPwmTargetErrorCount(void);
void UserPlatform_PollRemoteTargetUart(void);
uint8_t UserPlatform_HasRemoteTarget(void);
uint8_t UserPlatform_IsRemoteTargetOnline(void);
uint8_t UserPlatform_GetRemoteTargetAngleDeg10(uint16_t *angleDeg10);
uint32_t UserPlatform_GetRemoteTargetFrameCount(void);
uint32_t UserPlatform_GetRemoteTargetErrorCount(void);

#ifdef __cplusplus
}
#endif

#endif /* USER_PLATFORM_H */
