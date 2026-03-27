# JoyStick Y 軸油門控制計畫

---

## 需求說明

使用 **JoyStick X2 搖桿模組** Y 軸取代旋鈕：

| 搖桿位置 | 馬達行為 |
|----------|----------|
| 置中（死區內） | 停止 |
| 往上推 | 順時針（CW），速度正比偏移量 |
| 往下推 | 逆時針（CCW），速度正比偏移量 |

---

## 硬體量測結果

| 位置 | 電壓 | 12-bit ADC | Left-Aligned ADC |
|------|------|-----------|-----------------|
| 置中 | 1.61 V | ≈ 2000 | 32000 |
| 最大（上） | 3.28 V | ≈ 4069 | 65104 |
| 最小（下） | 0 V | 0 | 0 |

- **上行程**：32000 → 65104，區間 = 33104 la units  
- **下行程**：32000 → 0，區間 = 32000 la units

---

## 軟體設計

### 狀態機（`ThrottleState_t`）

```
THR_IDLE (0)  ──────────────────────────────►  THR_RUNNING (1)
  等待搖桿離開死區                               馬達加速中或運行中
  └─ !inDeadband → MC_StartMotor1()             ├─ RUN：MCI_ExecSpeedRamp(targetSpeed)
                                                 ├─ inDeadband → MC_StopMotor1() → IDLE
                                                 ├─ FAULT_OVER → AcknowledgeFault → IDLE
                                                 └─ IDLE/STOP → IDLE
```

### 關鍵參數（`mc_app_hooks.c`）

```c
#define JOY_CENTER_LA      32000u  // 中心 ADC 值（實測 1.61V）
#define JOY_DEADBAND_LA    2400u   // ±死區（±0.12V，150 個 12-bit counts）
#define JOY_UP_RANGE_LA    33104u  // 上推最大行程（la units）
#define JOY_DOWN_RANGE_LA  32000u  // 下推最大行程（la units）
#define JOY_MAX_RPM        2000    // 滿偏目標轉速
```

### 速度映射 `JoyToSpeed(deflection)`

- 移除死區偏移量後線性映射到 `±RPM_2_SPEED_UNIT(JOY_MAX_RPM)`
- 上推 → 正速度（CW）；下推 → 負速度（CCW 嘗試）

### 相容性

| 變數 | 說明 |
|------|------|
| `g_angCtrlState = 0` | 固定為 0，使 mc_tasks.c HF Task 不觸發角度注入 |
| `g_angSweep = 0` | 保留定義，供連結器使用 |

---

## Debug 監看變數

```
dbg_thrState      ← 0=IDLE 1=RUNNING
dbg_motorState    ← MCI 底層狀態
dbg_joyRaw12      ← ADC 12-bit 原始值（置中 ≈ 2000）
dbg_deflection    ← 相對中心偏移（正=上推，負=下推）
dbg_targetSpeed   ← 目標速度 speed_unit
```

---

## 已知限制

> **CCW（逆時針）**：MCSDK STO 在低速（< OBS_MINIMUM_SPEED_RPM ≈ 3551 RPM）會失去 Back-EMF 追蹤。  
> 下推搖桿時，馬達先以速度迴路試圖跑負速，若 STO 失鎖則 `FAULT_OVER`，狀態機自動清除後回到 `IDLE`，等待再次觸發。  
> 若 CCW 無法穩定運行，可考慮：
> - 交換任意兩條馬達相線（物理反向），搖桿下推改命令正速
> - 或使用開迴路電氣角注入方式直接指定方向

---

## 測試驗證流程

1. 上電，搖桿置中 → `dbg_thrState=0`，馬達靜止 ✓
2. 搖桿往上推 → 馬達啟動，`dbg_thrState=1`，轉速增加 ✓
3. 搖桿回中 → 馬達停止，`dbg_thrState=0` ✓
4. 搖桿往下推 → 馬達嘗試 CCW（視 STO 能力）
5. 調整 `JOY_DEADBAND_LA` / `JOY_MAX_RPM` 至預期手感

---

*最後更新：2026-03-26*
