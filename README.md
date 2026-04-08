# ST_FOC

更新時間: 2026-04-08 14:32:34 +08:00

## 專案目的

這個專案以 `B-G431B-ESC1` 為主控制板，建立一套可在 `MC Workbench / CubeMX` 重新產碼後仍能保留的 user-owned 馬達控制架構。

目前主軸是:
- 本地板: `B-G431B-ESC1 + MKS 2804 + AS5600`
- 外部旋鈕節點: `ESP32S + 另一組 2804 + AS5600`
- 外部節點輸出 `PWM`
- 本地板接收 `PWM` 目標角，驅動本地 2804 跟隨

同時保留:
- `2216 + 電位器 + sensorless throttle`
- `2804 + AS5600 + 本地電位器位置模式`
- `UART 目標角模式`
- `低速起轉測試模式`

目前 active 模式在 [user_config.h](/d:/Work_202506/FOC/ST_FOC/ST_FOC/Inc/user_config.h#L41):

```c
#define USER_CONTROL_MODE USER_CONTROL_MODE_PWM_TARGET_AS5600
```

## 整體架構

```text
                Remote Knob Node
      +-----------------------------------+
      | ESP32S                            |
      |  + 2804 motor                     |
      |  + AS5600                         |
      |  + angle -> PWM output            |
      +----------------+------------------+
                       |
                       | PWM 400Hz
                       | 1000us ~ 2000us
                       v
      +-----------------------------------+
      | B-G431B-ESC1                      |
      |  PA15 / J3 PWM input              |
      |  PB7/PB8 -> local AS5600 I2C      |
      |  FOC + inverter + UVW             |
      +----------------+------------------+
                       |
                       v
                 Local 2804 motor
```

## 硬體架構

### 1. 主控制板

```text
B-G431B-ESC1
  MCU: STM32G431
  Power stage: onboard 3-phase inverter
  Sensor input:
    PB7 -> AS5600 SDA
    PB8 -> AS5600 SCL
  External target input:
    PA15 -> J3 PWM
  User button:
    PC10 -> zero calibration trigger
  Motor output:
    U / V / W -> local 2804
```

### 2. 外部旋鈕節點

```text
ESP32S
  reads remote 2804 + AS5600 angle
  converts angle to PWM duty
  outputs PWM to B-G431B-ESC1 J3 PWM
```

### 3. 建議接線

```text
Remote ESP32S               B-G431B-ESC1
------------------------------------------------
PWM output              ->  J3 PWM / PA15
GND                     ->  J3 GND

Local AS5600 VCC        ->  3.3V
Local AS5600 GND        ->  GND
Local AS5600 SDA        ->  PB7
Local AS5600 SCL        ->  PB8

Local 2804 U/V/W        ->  ESC U/V/W
```

### 4. 目前角度命令定義

```text
PWM pulse width
  1000us ->   0 deg
  1500us -> ~179.5 deg
  2000us -> 359 deg
```

對應設定在 [user_config.h](/d:/Work_202506/FOC/ST_FOC/ST_FOC/Inc/user_config.h#L120).

## 系統資料流

### 1. 命令端到跟隨端

```text
Remote AS5600 angle
  -> ESP32S
  -> PWM pulse width
  -> PA15 EXTI capture
  -> pulse width (us)
  -> target angle (deg10)
  -> position controller
  -> FOC current command
  -> local 2804 motion
```

### 2. 回授端

```text
Local 2804 shaft
  -> local magnet
  -> AS5600 raw angle
  -> mechanical angle
  -> electrical angle
  -> mechanical speed
  -> position loop / sensored FOC
```

## 軟體架構

### 模組分層

```text
Generated Layer
  main.c
  mc_app_hooks.c
  mc_tasks*.c
  motorcontrol / mc_* / parameters

User-Owned Layer
  user_config.h
  user_platform.[ch]
  user_sensored.[ch]
  user_sensored_foc.c
  user_app.[ch]
```

### 模組責任

```text
main.c
  -> MCU / HAL / MCSDK init
  -> UserPlatform_Init()

mc_app_hooks.c
  -> generated hook bridge
  -> UserApp_PostMediumFrequencyHook_M1(rawPot)

user_platform.c
  -> board-level user code
  -> AS5600 I2C access
  -> PWM pulse capture on PA15
  -> UART target parser (other mode)
  -> runtime utility functions

user_sensored.c
  -> local AS5600 feedback processing
  -> raw angle / mech angle / elec angle / speed

user_sensored_foc.c
  -> sensored FOC skeleton
  -> boot cap / alignment / enter RUN

user_app.c
  -> control-mode selection
  -> target source selection
  -> position controller
  -> current command generation
  -> button zero-calibration action

user_config.h
  -> compile-time mode selection
  -> motor profiles
  -> pulse width and position tuning
```

## 目前 Active 控制流程

### PWM 目標角模式

```text
PA15 edge interrupt
  -> UserPlatform_HandlePwmTargetEdgeInterrupt()
  -> compute pulse width
  -> map 1000~2000us to 0~3590 deg10

MC medium-frequency hook
  -> UserApp_PostMediumFrequencyHook_M1()
  -> read latest PWM target angle
  -> read local AS5600 actual angle
  -> RunSensoredPositionController()
  -> position error
  -> target rpm
  -> speed error
  -> q-axis current command
  -> MC_SetCurrentReferenceMotor1()
  -> FOC current loop
  -> inverter PWM
  -> local motor follows target
```

### 圖示

```text
PWM target angle
        |
        v
+----------------------------+
| RunSensoredPositionController |
+----------------------------+
        |
        +--> target angle
        +--> actual angle
        +--> angle error
        |
        v
  position -> speed
        |
        v
  speed -> current
        |
        v
   MCSDK current loop
        |
        v
      Motor
        |
        v
     AS5600 feedback
```

## PC10 按鍵功能

`PC10` 在目前這版不是 start/stop，而是零位校正按鍵。

作用:
- 把本地跟隨馬達「目前的 AS5600 角度」
- 記成新的本地 `0 deg` 對應點
- 只改本地跟隨端 offset
- 不改外部旋鈕節點的原始角度定義

### 校正流程

```text
User presses PC10
  -> EXTI15_10_IRQHandler()
  -> UserApp_HandleUserButton()
  -> set zero-calibration request flag

next control tick
  -> RunSensoredPositionController()
  -> current local angle read from AS5600
  -> compute new target offset
  -> store as s_posTargetOffsetDeg10
  -> future target angles use this new offset
```

### 用途

這個功能用來處理:
- 外部旋鈕那顆 2804 的機械 0 度
- 本地跟隨那顆 2804 的機械 0 度
- 安裝位置不一致

也就是兩顆馬達不用硬裝成完全同一個機械零位，只要把本地馬達轉到你想當成 `0 deg` 的位置，再按一下 `PC10` 即可。

## 控制模式總覽

```text
USER_CONTROL_MODE
|
+-- USER_CONTROL_MODE_THROTTLE
|    pot -> throttle -> sensorless speed control
|    for 2216
|
+-- USER_CONTROL_MODE_POSITION_AS5600
|    pot -> target angle
|    local AS5600 -> feedback
|
+-- USER_CONTROL_MODE_UART_TARGET_AS5600
|    external UART target angle -> local AS5600 follower
|
+-- USER_CONTROL_MODE_PWM_TARGET_AS5600
|    external PWM target angle -> local AS5600 follower
|    current active mode
|
+-- USER_CONTROL_MODE_FOLLOW_AS5600_KNOB
|    experimental follower mode
|
+-- USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST
     low-speed bring-up mode
```

## 重要檔案

```text
Inc/user_config.h
  compile-time configuration

Inc/user_platform.h
  user-owned hardware APIs

Inc/user_app.h
  app entry / button action

Src/user_platform.c
  PA15 PWM capture
  PB7/PB8 AS5600 I2C

Src/user_app.c
  PWM target branch
  position controller
  PC10 zero calibration

Src/user_sensored.c
  local AS5600 feedback processing

Src/user_sensored_foc.c
  sensored FOC override/skeleton

Src/stm32_mc_common_it.c
  PA15 EXTI
  PC10 button EXTI
```

## 建議 Live Watch

### PWM 模式輸入

- `dbg_pwmTargetOnline`
- `dbg_pwmTargetPulseWidthUs`
- `dbg_pwmTargetAngleDeg10`
- `dbg_pwmTargetFrameCount`
- `dbg_pwmTargetErrorCount`

### 跟隨控制

- `dbg_targetAngleDeg10`
- `dbg_actualAngleDeg10`
- `dbg_angleErrDeg10`
- `dbg_posTargetRpm`
- `dbg_posSpeedErrRpm`
- `dbg_posCurrentCmdMa`
- `dbg_sensoredSpeedRpm`

### 零位校正

- `dbg_posTargetOffsetDeg10`
- `dbg_zeroCalCount`

## Generate / 維護原則

這份專案的原則是:
- generated code 只保留 hook
- 真正的控制邏輯放在 user-owned 檔案
- 讓 `MC Workbench / CubeMX` regenerate 時，不會把主邏輯洗掉

目前特別 user-owned 的區域:
- `PB7 / PB8` 的 AS5600 I2C
- `PA15` 的 PWM target capture
- `PC10` 的零位校正動作

## 建置

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\build.ps1 -Preset Debug -BuildDir .\build\Debug
```

## 注意事項

- `devlog.md` 現在是本機專用，不再推送到遠端 Git。
- `USER_MCSDK_TUNE_PROFILE` 仍可能與 app-level profile 不同，bring-up 時請留意這件事。
- 若要把零位校正保留到重開機後，還需要再做 Flash/NVM 儲存，這版尚未加入。
