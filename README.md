# ST_FOC

更新時間: 2026-04-08 07:47:16 +08:00

## 專案摘要

這個專案以 `B-G431B-ESC1` 為硬體基底，保留原本 `MC Workbench / CubeMX` 生成的馬達控制骨架，並把自訂控制邏輯集中在 user-owned 檔案。

目前專案同時支援：

- `2216 + 電位器 + sensorless throttle`
- `2804 + AS5600(I2C) + 位置控制`
- `2804 + AS5600 -> 2216 跟隨` 的預留模式
- 低速正反轉 bring-up 測試模式

目前 active setup 來自 [Inc/user_config.h](Inc/user_config.h)：

- `USER_CONTROL_MODE = USER_CONTROL_MODE_POSITION_AS5600`
- `USER_MOTOR_PROFILE = USER_MOTOR_PROFILE_2804_220KV`
- `USER_STARTUP_PROFILE = USER_STARTUP_PROFILE_2804_GENTLE_SENSORLESS`
- `USER_POSITION_OUTPUT_MODE = USER_POS_OUTPUT_MODE_SPEED`
  目前這條 speed 模式是 user-owned 的級聯控制：
  `位置誤差 -> 目標 RPM -> 速度誤差 -> q-axis current`

## 整體系統架構

```text
                    +----------------------+
                    |   Potentiometer      |
                    |   PB12 / ADC         |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |   UserApp            |
                    |   mode select        |
                    |   target compute     |
                    +----+------------+----+
                         |            |
                         |            |
     sensorless path     |            |      sensored path
     (2216 throttle)     |            |      (2804 position)
                         v            v
                +----------------+  +----------------------+
                | speed command  |  | user-owned cascade   |
                | to MCSDK       |  | speed -> current     |
                +--------+-------+  +----------+-----------+
                         |                     ^
                         |                     |
                         v                     |
                 +-------------------------------+
                 | MCSDK / MCI / STC / FOC       |
                 | speed loop / current loop     |
                 | PWM generation                |
                 +---------------+---------------+
                                 |
                                 v
                         +---------------+
                         | Inverter UVW  |
                         +-------+-------+
                                 |
                                 v
                               Motor
                                 ^
                                 |
                         +-------+-------+
                         | AS5600        |
                         | PB7/PB8 I2C   |
                         +---------------+
```

## 軟體架構圖

```text
main.c
  -> HAL / MCU / MCSDK init
  -> UserPlatform_Init()

mc_app_hooks.c
  -> MC_APP_BootHook()
  -> MC_APP_PostMediumFrequencyHook_M1()
  -> UserApp_PostMediumFrequencyHook_M1(rawPot)

user_platform.c / user_platform.h
  -> user-owned board layer
  -> I2C1 for AS5600
  -> PB7/PB8 pin ownership

user_sensored.c / user_sensored.h
  -> AS5600 raw angle
  -> mechanical angle
  -> electrical angle
  -> mechanical speed

user_sensored_foc.c
  -> sensored FOC skeleton
  -> boot cap / alignment / enter RUN
  -> connect AS5600 feedback into FOC path

user_app.c / user_app.h
  -> outer application logic
  -> control mode switch
  -> target generation
  -> position -> speed/current conversion
  -> speed mode uses user-owned speed-to-current cascade

user_config.h
  -> compile-time switches
  -> motor profiles
  -> startup profiles
  -> mode selection
```

## 生成碼與自訂碼邊界

```text
CubeMX / MC Workbench generated
  main.c
  main.h
  mc_app_hooks.c
  mc_* / motorcontrol / startup / parameters
  stm32g4xx_hal_*.c

User-owned
  Inc/user_config.h
  Inc/user_platform.h
  Inc/user_app.h
  Inc/user_sensored.h
  Src/user_platform.c
  Src/user_app.c
  Src/user_sensored.c
  Src/user_sensored_foc.c
```

Generate-safe 原則：

- `main.c` 只保留 user hook 呼叫
- `mc_app_hooks.c` 只做 generated hook -> user app 的轉接
- `PB7/PB8` 的 AS5600 I2C 路徑由 user-owned code 管理
- 重新 Generate 後，優先檢查 `main.c`、`mc_app_hooks.c`、`main.h` 是否仍保留 user block 呼叫

## 硬體接線圖

### 1. 主控制板

```text
B-G431B-ESC1
  PB12  -> 電位器 ADC 輸入
  PB7   -> AS5600 SDA
  PB8   -> AS5600 SCL
  U/V/W -> 馬達三相
  3.3V  -> AS5600 VCC
  GND   -> AS5600 GND
```

### 2. 目前使用中的感測與命令來源

```text
Potentiometer
  one side -> 3.3V
  one side -> GND
  wiper    -> PB12

AS5600
  VCC -> 3.3V
  GND -> GND
  SDA -> PB7
  SCL -> PB8
```

### 3. 實體訊號流

```text
Potentiometer rotation
  -> ADC raw value on PB12
  -> target angle / target speed

Motor shaft rotation
  -> magnet
  -> AS5600 absolute angle
  -> PB7/PB8 I2C
  -> shaft feedback

MCU output
  -> PWM UH/UL, VH/VL, WH/WL
  -> ESC power stage
  -> UVW
  -> motor
```

## 控制模式圖

```text
USER_CONTROL_MODE
|
+-- USER_CONTROL_MODE_THROTTLE
|    電位器 -> 油門百分比 -> speed command -> sensorless FOC
|    主要給 2216 使用
|
+-- USER_CONTROL_MODE_POSITION_AS5600
|    電位器 -> target angle
|    AS5600 -> actual angle
|    再依 USER_POSITION_OUTPUT_MODE:
|      - CURRENT: 位置誤差 -> q-axis current
|      - SPEED:   位置誤差 -> target RPM -> speed error -> q-axis current
|
+-- USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
|    外部角度 -> follower target
|
+-- USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST
     固定低速正反轉 bring-up
```

## 目前位置模式控制路徑

目前 active 是 `position -> speed -> current`：

```text
PB12 pot raw
  -> PotToAngleDeg10()
  -> targetAngleDeg10

AS5600 raw angle
  -> UserSensored_MediumFrequencyUpdate()
  -> actualAngleDeg10 / actualRPM

UserApp_PostMediumFrequencyHook_M1()
  -> angle error
  -> hold / deadband decision
  -> target RPM
  -> speed error
  -> q-axis current command
  -> StartOrUpdateCurrentCommand()

MCSDK current loop
  -> FOC
  -> PWM
  -> motor
```

如果切回 `position -> current`：

```text
angle error
  -> position P + speed damping
  -> q-axis current command
  -> StartOrUpdateCurrentCommand()
  -> FOC torque response
```

## 時序 / 迴圈流程圖

### 開機流程

```text
Power on
  -> HAL init
  -> MCSDK init
  -> UserPlatform_Init()
  -> MC_APP_BootHook()
  -> register potentiometer regular conversion
  -> system enters periodic motor-control loop
```

### 中頻控制流程

```text
MC medium-frequency tick
  -> read pot raw via RCM_GetRegularConv()
  -> UserApp_PostMediumFrequencyHook_M1(rawPot)
  -> update AS5600 telemetry
  -> compute target / error
  -> send speed or current command
```

### 有感 FOC 流程

```text
Boot cap
  -> current offset calibration
  -> alignment
  -> capture AS5600 mechanical angle
  -> derive electrical angle
  -> enter RUN
  -> periodic medium-frequency update
  -> periodic high-frequency current control
```

### 閉迴路概念

```text
target angle
  - actual angle
  = angle error

angle error
  -> outer loop
  -> target speed or target current

MCSDK inner loop
  -> voltage / current / PWM

motor response
  -> AS5600 feedback
  -> next loop
```

## 主要檔案對照

```text
Src/main.c
  system init entry

Src/mc_app_hooks.c
  generated hook bridge

Src/user_app.c
  application control logic

Src/user_sensored.c
  AS5600 feedback processing

Src/user_sensored_foc.c
  sensored FOC task override

Src/user_platform.c
  user-owned platform / I2C glue

Inc/user_config.h
  all compile-time switches
```

## 常用切換點

### 1. 切控制模式

修改 [Inc/user_config.h](Inc/user_config.h)：

```c
#define USER_CONTROL_MODE USER_CONTROL_MODE_POSITION_AS5600
```

可選：

- `USER_CONTROL_MODE_THROTTLE`
- `USER_CONTROL_MODE_POSITION_AS5600`
- `USER_CONTROL_MODE_FOLLOW_AS5600_KNOB`
- `USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST`

### 2. 切位置輸出型態

修改 [Inc/user_config.h](Inc/user_config.h)：

```c
#define USER_POSITION_OUTPUT_MODE USER_POS_OUTPUT_MODE_SPEED
```

可選：

- `USER_POS_OUTPUT_MODE_SPEED`
- `USER_POS_OUTPUT_MODE_CURRENT`

### 3. 切馬達 profile

修改 [Inc/user_config.h](Inc/user_config.h)：

```c
#define USER_MOTOR_PROFILE USER_MOTOR_PROFILE_2804_220KV
```

## 建置與除錯

### 建置

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build.ps1 -Preset Debug -BuildDir .\build\Debug
```

### 建議 Live Watch

位置模式常用：

- `dbg_targetAngleDeg10`
- `dbg_actualAngleDeg10`
- `dbg_angleErrDeg10`
- `dbg_cmdRPM`
- `dbg_actualRPM`
- `dbg_posCurrentCmdMa`
- `dbg_posHoldActive`
- `dbg_posOutputMode`
- `dbg_as5600Raw`
- `dbg_as5600ReadOk`

## 備註

- `PB7/PB8` 的 AS5600 I2C 腳位由 user-owned code 管理，重新 Generate 時不要在生成工具裡重新指派別的功能。
- `USER_MCSDK_TUNE_PROFILE` 目前仍標示為 `2216` 生成參數，代表 app-level profile 與 MCSDK 低層 tune 可能暫時不一致，這是 bring-up 階段的暫時做法。
- `devlog.md` 會持續記錄每次調整的原因、目的、方法與時間。
