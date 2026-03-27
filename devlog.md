# ST_FOC 開發日誌

---

## 2026-03-27 12:12:00 (Version Control)

### 初始化 Git 儲存庫並推送至 Github

**原因：** 依使用者要求將當前具備滑桿油門 (Slide Throttle) 控制、儀錶板刻度修正及即時監看變數狀態的完整專案，推送備份至遠端 Github 儲存庫。

**目的：**
建立版本控制，並與 https://github.com/MMLL168/FOC_G431_SlideThrottle.git 綁定，推播紀錄備存。

**修改方式：**
1. 於根目錄建立 `.gitignore` 排除編譯暫存檔以節省空間。
2. 執行 `git init` 初始化儲存庫並將所有源碼加入追蹤。
3. 提交 `Initial commit with Slide Throttle logic`。
4. 設定遠端 `origin` 為上述 Github 網址並推送至 `main` 分支。

**日期時間：** 2026-03-27 12:12:00

---

## 2026-03-27 12:08:00 (Motor Pilot Gauge Note)

### Motor Pilot 儀表板維持 5000 RPM 的原因與解決方案

**原因：** 
ST Motor Pilot 在點擊 Connect 連線時，會**即時透過 ASPEP 協定向馬達控制板 (Firmware) 內部讀取 `MaxAppPositiveMecSpeedUnit` 系統暫存器**。
由於我們剛才把程式碼改為 10000 RPM 限制且雖然「編譯(Build)成功」，但如果您還沒將新編譯好的韌體**燒錄(Download/Flash)**進板子裡，那目前板子上跑的依然是前一版的舊韌體 (上限還是被鎖在 4770)，所以 Motor Pilot 連線去讀它時，當然還是只會顯示 5000。

**解決方式：** 
1. 確保先在 VSCode 點擊 Debug (或用 ST-Link 工具) **完成燒錄** 最新編譯好的韌體。
2. 燒錄完成且板子重開機後，在 ST Motor Pilot 先點擊 **Disconnect** 再重新點擊 **Connect (Polling)** 進行連線。
3. 軟體向新韌體拿到新的 10000 上限暫存器後，儀表板就會瞬間自動擴大成正負 10000 轉的刻度了！

**日期時間：** 2026-03-27 12:08:00

---

## 2026-03-27 12:03:00 (Update ST Motor Pilot Dashboard Scale)

### 修正 Motor Pilot 儀表板最高轉速上限顯示 (4770 -> 10000)

**原因：** 使用者反應 ST Motor Pilot 的轉速儀表板刻度只顯示到正負 5000 RPM，即使韌體內的 `MAX_RPM` 已經拉高到 10000。
這是因為 Motor Pilot 在連線時，會讀取 `.stmcx` 或 `.ioc` 等專案設定檔來決定 GUI 的刻度（上次 Workbench Generate 寫死為 4770，軟體自動向上取整為 5000）。

**目的：**
修改專案設定檔內的速度上限變數，強制讓儀表板刻度能支援到正負 10000 RPM，讓視覺反饋與實際韌體參數一致。

**修改方式：**
修改專案根目錄下的生成檔：
*   **`ST_FOC.ioc`**: 將 `MotorControl.M1_MAX_APPLICATION_SPEED` 與 `MotorControl.M1_MOTOR_MAX_SPEED_RPM` 值由 `4770` 改為 `10000`。
*   **`ST_FOC.wbdef`**: 將對應的 `<define key="M1_MAX_APPLICATION_SPEED" value="10000" />` 改為 10000。

**日期時間：** 2026-03-27 12:03:00

---

## 2026-03-27 11:55:46 (Update Max RPM)

### 調整最高目標轉速為 10000 RPM

**原因：** 根據使用者需求，將 100% 油門的最高對應開環/閉環目標轉速調整為更高轉。

**目的：**
放寬 100% 油門時的最高速度上限至 10000 RPM。

**修改方式：**
修改 `mc_app_hooks.c` 檔案開頭 `#define MAX_RPM` 的定義值，從 4500 改為 10000。

**日期時間：** 2026-03-27 11:55:46

---

## 2026-03-27 11:47:00 (Add Debug Variables)

### 新增即時監看變數供搖桿控制狀態確認

**原因：** 使用者需要能即時監測馬達運作狀態，包含實際的油門大小、目標轉速、實際轉速，以及目前的馬達運作狀態，以便在使用 Live Watch 時進行 Debug 排錯與確認油門線性成果。

**目的：**
加入以下全域變數做為 Live Watch 的監看點：
*   `dbg_throttlePct`: 現在的油門大小 (-100% ~ 100%)
*   `dbg_cmdRPM`: 目標轉速命令 (RPM)
*   `dbg_actualRPM`: 實際測得轉速 (RPM)
*   `dbg_motorState`: 馬達狀態 (0=IDLE, 4=START, 6=RUN, 8=STOP, 10=FAULT_NOW, 11=FAULT_OVER)

**修改方式：**
1. **`mc_app_hooks.c`**:
   - 新增全域變數 `dbg_cmdRPM`, `dbg_actualRPM`。
   - 在 `MC_APP_PostMediumFrequencyHook_M1()` 結尾處，利用原本 Motor SDK 提供之 `MCI_GetMecSpeedRefUnit` 和 `MCI_GetAvrgMecSpeedUnit` 取得 `SPEED_UNIT` 變化量並分別換算回常見的 RPM 給予全域變數紀錄。

**日期時間：** 2026-03-27 11:47:00

---

## 2026-03-27 (Potentiometer Throttle Logic Restoration)

### 恢復使用滑桿電阻 (Potentiometer) 控制正反轉油門與參數復原

**原因：** 執行第三次 MC Workbench Generate 後，前次開發的控制程式碼被清除，`drive_parameters.h` 的自訂參數也被重置為預設值。根據使用者最新需求，改用滑桿電阻 (Potentiometer) 進行油門控制，中間為 0% 停轉，推至最頂 (100%) 為順時針最高速，推至最底 (-100%) 為逆時針最高速。

**目的：**
1. 建立滑桿電阻全程線性映射到 ±100% 油門，中心設為 0% 且具備死區。
2. 油門大於 0 或小於 0 即啟動馬達，初始目標轉速為最低啟動轉速 (`MIN_RPM`)。
3. 驅動速度跟隨油門大小「平滑且線性」地對應至 (`MAX_RPM` - `MIN_RPM`) 的區間內（0~100% 對應 1800~4500 RPM）。
4. 自動恢復 `drive_parameters.h` 被清除的自訂參數設定。

**修改方式：**
1. **`mc_app_hooks.c`**:
   - 停用 `SPDPOT_Run(&SpeedPotentiometer_M1, rawValue);` 以避免原版代碼搶走控制權。
   - 新增定義：`THR_CENTER = 32768`, `THR_DEADBAND = 2000`, `THR_MAX = 65520`, `THR_MIN = 0`。
   - 取得 `PotRegConv_M1` ADC 數值並線性轉換為 `throttlePct` (-100% ~ 100%)。
   - 實作 `THR_IDLE` 和 `THR_RUNNING` 的油門狀態機：脫離死區則立即啟動跳至 `MIN_RPM` (1800)；隨著油門繼續推大，使用 `MCI_ExecSpeedRamp` 令轉速以 500ms 絲滑爬升對應 `MIN_RPM` (1800) 到 `MAX_RPM` (4500) 轉之間。
2. **`drive_parameters.h`**:
   - `MAX_APPLICATION_SPEED_RPM` 恢復為 10000。
   - `IQMAX_A` 恢復為 4，並還原 `PHASE1_DURATION` (100ms)、`PHASE2_DURATION` (1200ms)。
   - `OBS_MINIMUM_SPEED_RPM` 恢復 1717，`FW_VOLTAGE_REF` 恢復 970。

**日期：** 2026-03-27

---

## 2026-03-27 (Compile Error Fix)

### 移除 mc_tasks.c 中殘留的角度控制變數引用

**原因：** `mc_tasks.c` 中殘留了已被移除的旋鈕角度控制變數 `g_angCtrlState` 和 `g_angSweep` 的引用（這些變數原定義於 `mc_app_hooks.c`，但系統已改用搖桿控制且在 MC Workbench 第三次 Generate 後遭到清空），導致 `undefined reference` linker error。

**目的：** 修正編譯錯誤，允許系統順利編譯。

**修改方式：** 移除 `mc_tasks.c` 內 `USER CODE BEGIN Private Variables` 與 `USER CODE BEGIN HighFrequencyTask 0` 區塊中關於角度控制的宣告與覆寫 STO 電氣角代碼。

**日期：** 2026-03-27

---

## 2026-03-27（本對話 #3 — 最新狀態）

### 第三次 MC Workbench Generate — mc_tasks_foc.c 架構大改，Build 失敗

**原因：** 使用者執行第三次 MC Workbench Generate。本次 Generate 的 mc_tasks_foc.c 版本與前兩次完全不同：SCC/OTT 相關程式碼全部移除，改用標準 `FOC_CurrControllerM1` 靜態函式做電流控制，與第二次 Generate 的 SCC-based 架構不相容。

**mc_tasks_foc.c 主要變動：**
- `FOC_Init`：移除 SCC_Init、SCC.p* 設定、OTT_Init 等所有 SCC/OTT 初始化
- `TSK_MF_StopProcessing`：移除 SCC_Stop、OTT_Stop
- `CHARGE_BOOT_CAP`：直接進入 START，不再呼叫 SCC_Start
- `WAIT_STOP_MOTOR` case 完全移除，改為 `default: break`
- `SCC_MF` 無條件呼叫：已移除（不再存在）
- 新增 `static uint16_t FOC_CurrControllerM1(void)` 宣告
- START case：加入 `RUC_FirstAccelerationStageReached` 控制 STO 收斂判斷時機

**mc_app_hooks.c 狀態：**
- 所有使用者程式碼（油門狀態機、debug 變數、JoyToSpeed 等）已被 Generate 清空
- 只剩 BootHook（SPDPOT_Init）和 PostMediumFrequencyHook_M1（SPDPOT_Run）

**Build 狀態：** 失敗 — `collect2.exe: error: ld returned 1 exit status`（linker error）
目前 Debug session 使用的是舊版 ELF binary（非最新程式碼）。

**待辦：**
1. 找出 linker error 原因並修正
2. 恢復 mc_app_hooks.c 使用者程式碼（油門控制 + debug 變數）
3. 確認新版 mc_tasks_foc.c 架構下 STO 收斂是否正常

**日期：** 2026-03-27

---

## 2026-03-27（本對話 #3 — 當前狀態）

### ⚠ 未解決：啟動瞬間電流衝到 2A，緩升無效

**現象：** 一給油門（觸發 `MC_StartMotor1`），電流立即衝到接近 2A，而非從 0 緩升。
PHASE1_DURATION=2000ms、PHASE1_FINAL_CURRENT_A=1A 的 RUC ramp 設定無法防止此衝流。

**目前 drive_parameters.h 啟動參數（使用者手動調整）：**
```
PHASE1: 2000ms, 500 RPM, 1A
PHASE2: 2500ms, 1000 RPM, 2A
PHASE3: 1000ms, 1500 RPM, 2A
PHASE4: 1000ms, 2000 RPM, 2A
PHASE5: 0ms,   3000 RPM, 2A
```

**已確認事項：**
- SCC 修正有效：`dbg_sccState=0`（SCC_IDLE），正常 FOC 有在執行
- `dbg_Iqdref_q`、`dbg_Iqd_q`、`dbg_Vqd_q` 都有正常數值，電流控制迴路有在運作
- RUC 的電流 ramp（`STC_ExecRamp` 從 0 線性爬升到 `hFinalTorque`）理論上已有緩升
- 但實際量測電流仍在啟動瞬間衝高

**可能根本原因（待查）：**
1. 電源供應器瞬間過電流保護導致讀數失真，非真實馬達電流
2. 電流 PI 在 PHASE1 初期因角度誤差（500 RPM 開環）產生大誤差，PI 輸出飽和
3. 馬達電感充電瞬間電流（與 STARTING_ANGLE 對應的相電流初始值）
4. PHASE1_FINAL_SPEED_UNIT 從 0 改為 500 RPM 後，對齊階段即開始旋轉角度，轉子尚未跟上即產生大電流誤差

**下一步候選方向：**
- 將 PHASE1 恢復為 0 RPM（純對齊，不旋轉），確認是否是旋轉角度造成初始大電流
- 或確認電流量測是 Bus 側電流計還是相電流，排除量測問題
- 或降低電流 PI 的 Kp（`PID_TORQUE_KP_DEFAULT`）減少 PI 初始輸出

**日期：** 2026-03-27

---

## 2026-03-27 (本對話 #2)

### PHASE1_DURATION 太短 — 轉子對齊失敗 → SWITCH_OVER 永遠不成立

**原因：** `PHASE1_DURATION=100ms` 過短，轉子（特別是 2216 較重的轉子）無法在 100ms 內完成靜止對齊就進入 PHASE2 open-loop ramp。對齊角度偏差導致開環電流無法有效推動轉子，STO 偵測不到 BEMF，`hElSpeedDpp` 始終為 0，SWITCH_OVER 條件（速度在 1790~2025 RPM 區間內 2 次）永遠不成立，馬達永遠卡在 START。

**目的：** 給轉子足夠時間完成靜止對齊，讓 PHASE2 open-loop ramp 從正確角度出發，STO 得以偵測 BEMF 並收斂。

**修改方式：** `drive_parameters.h`：`PHASE1_DURATION` 100ms → **500ms**

**日期：** 2026-03-27

---

## 2026-03-26 (本對話)

### 第二次 MC Workbench Generate（Rev-up to FOC switch-over 關閉）— 還原所有自訂檔案

**原因：** 第一次 Generate 開啟 Rev-up to FOC switch-over 後，`mc_tasks_foc.c` 的啟動電流注入邏輯改變，馬達在 PHASE2 完全不轉（`dbg_instantRPM=0`）。確認兩者互斥後，決定關閉 Rev-up to FOC switch-over，重新 Generate（第二次），讓標準 open-loop ramp 啟動。

**目的：** 恢復正確啟動邏輯（open-loop PHASE1/PHASE2 ramp → SWITCH_OVER → RUN），同時對齊新的控制目標：
1. 油門 > 0 → 目標最低轉速固定 JOY_MIN_RPM=3500 RPM
2. 啟動後轉速以 500ms ramp 絲滑銜接油門命令

**修改方式：**

`mc_app_hooks.c`（完整還原 + 新邏輯）：
- `JOY_MIN_RPM` 3000 → **3500**
- THR_IDLE 啟動時改為固定命令 `RPM_2_SPEED_UNIT(JOY_MIN_RPM)`（不跟當前油門位置），確保 SWITCH_OVER 後速度 PID 從穩定點（3500 RPM）開始
- THR_RUNNING + RUN 改為 **500ms ramp**（舊 200ms），讓 1908→3500→油門目標過渡更絲滑
- 補回所有 debug 變數：dbg_joyRaw12、dbg_joyFiltered、dbg_deflection、dbg_targetSpeed、dbg_throttlePct、dbg_thrState、dbg_motorState、dbg_actualSpeedUnit、dbg_actualRPM、dbg_instantRPM、dbg_cmdRPM、dbg_fwVoltPct、dbg_fwIdRef

`drive_parameters.h`（還原 6 個參數）：
- `MAX_APPLICATION_SPEED_RPM`: 4770 → **10000**
- `IQMAX_A`: 3 → **4**
- `PHASE1_DURATION`: 1000ms → **100ms**
- `PHASE2_DURATION`: 3816ms → **1200ms**
- `OBS_MINIMUM_SPEED_RPM`: 1900 → **1717**
- `FW_VOLTAGE_REF`: 985 → **970**

**日期：** 2026-03-26

---

## 2026-03-27（本對話 #3 診斷）

### 緩升啟動電流 — 拉長 PHASE1/PHASE2 時間讓電流平滑爬升

**原因：** RevUp 控制器（RUC）本身有做電流 ramp（從 0 線性爬升到 PHASE_FINAL_CURRENT），但 PHASE1 只有 500ms，緩升太快，使用者感覺電流瞬間衝高（看到 1.2A）並抖動。

**目的：** 拉長 PHASE1 到 2000ms（電流從 0 到 1A 爬升，速率 0.5mA/ms），拉長 PHASE2 到 2500ms（1A 到 2A，同時轉速從 0 爬升到 1908 RPM）。讓電流緩升、降低啟動衝擊。

**修改方式：** `drive_parameters.h`：
- `PHASE1_DURATION`: 500ms → **2000ms**
- `PHASE1_FINAL_CURRENT_A`: 3A → **1A**
- `PHASE2_DURATION`: 1200ms → **2500ms**
- `PHASE2_FINAL_CURRENT_A`: 3A → **2A**
- PHASE3~5 同步改為 2A（備用）

**日期：** 2026-03-27

---

### ★ 根本原因確認 + 修正：SCC_MF 無條件呼叫讓 SCC 進入 KE 偵測模式

**原因（Root Cause）：**
`mc_tasks_foc.c` 第 520 行（第二次 Generate 後的版本）無條件在每個 MF Task 末尾呼叫 `SCC_MF(&SCC)`。這讓 SCC 狀態機從 `SCC_IDLE(0)` 依序推進到 `SCC_KE_DETECTING_PHASE(8)`。在 state 8，`SCC_SetPhaseVoltage` 執行的是 KE 量測演算法（非正常 FOC）：
- 馬達確實有轉（KE 偵測會帶動馬達），所以使用者看到「持續慢速轉」
- STO observer 被 SCC 當作暫存器，`wBemf_alfa_est` 發散（觀察值：±100M～526M）
- `hAvrMecSpeedUnit` 始終接近 0，`STO_PLL_IsObserverConverged` 永遠不成立
- 馬達永遠卡在 START（motorState=4）

診斷關鍵數據：`dbg_sccState=8`（SCC_KE_DETECTING_PHASE）、`dbg_mpOngoing=0`、`dbg_bemfEst` 數億級發散。舊版 STM32CubeIDE build 只在 FAULT_NOW/default case 呼叫 SCC_MF，正常啟動不推進 SCC 狀態機，故馬達正常運作。

**目的：** 確保正常啟動時 SCC 保持在 `SCC_IDLE(0)`（Motor Control mode），`SCC_SetPhaseVoltage` 執行正常 FOC。

**修改方式：** `mc_tasks_foc.c`（需在每次 MC Workbench Generate 後恢復）：
1. `SCC_MF` 改為有條件呼叫：只在 `SCC.bMPOngoing != 0`（明確執行 Motor Profiling）時呼叫
2. CHARGE_BOOT_CAP 和 WAIT_STOP_MOTOR 中的 `SCC_Start` 後加入：若 `bMPOngoing==0` 則強制 `SCC.sm_state = SCC_IDLE`

**日期：** 2026-03-27

---

### 擴大診斷變數 — 找出 START 卡住的根本層次

**原因：** `dbg_instantRPM=0`、`dbg_fwVoltPct=0`，馬達在 PHASE2 完全不轉，但 PHASE1 對齊有震動（正常）。需要確認問題在哪個層次：SCC 模式？電流命令？角度旋轉？

**目的：** 一次性加入完整診斷集，涵蓋 SCC 狀態機、電流命令/回授、電壓命令、電角度、BEMF 估測。

**修改方式：** `mc_app_hooks.c`：
- 加入 `#include "mp_self_com_ctrl.h"`
- 新增全域變數：`dbg_bemfEst`, `dbg_sccState`, `dbg_mpOngoing`, `dbg_Iqdref_q`, `dbg_Iqd_q`, `dbg_Vqd_q`, `dbg_elAngle`
- 在 `PostMediumFrequencyHook_M1` 末尾讀取 `SCC.sm_state`, `SCC.bMPOngoing`, `FOCVars[M1].Iqdref.q`, `FOCVars[M1].Iqd.q`, `FOCVars[M1].Vqd.q`, `FOCVars[M1].hElAngle`, `STO_PLL_M1.wBemf_alfa_est`

**判讀：**
- `dbg_sccState != 0` → SCC 在 Profiler 模式，不執行正常 FOC
- `dbg_Iqdref_q == 0` → RUC 電流命令未設定
- `dbg_elAngle` 固定 → 虛擬速度感測器沒有旋轉角度
- `dbg_Vqd_q == 0` → PI 控制器沒有輸出電壓

**日期：** 2026-03-27

---

## 2026-03-27 08:58

### OBS_MINIMUM_SPEED_RPM 過高 — STO 確認窗口只有 8 RPM 導致永遠卡在 START

**原因：** MC Workbench Generate 後，`OBS_MINIMUM_SPEED_RPM` 由 Motor Profiling 原值 1717 被改成 1900（Workbench UI Start-up speed threshold 設定）。`PHASE2_FINAL_SPEED=1908 RPM`，與 OBS_MIN=1900 只差 8 RPM。STO 平均 FIFO 深度 64 樣本 @ 1ms，但 8 RPM 速度區間只有 ~5ms，FIFO 根本無法積累足夠樣本 → SWITCH_OVER 條件永遠不滿足 → `dbg_motorState` 永遠停在 4（START）。同時 `dbg_instantRPM=0`，STO 無法輸出可靠估測。

**目的：** 恢復可靠的 SWITCH_OVER，讓 STO 有足夠窗口（1717～1908 RPM，191 RPM）積累估測。

**修改方式：** `drive_parameters.h`：`OBS_MINIMUM_SPEED_RPM` 1900 → **1717**（Motor Profiling 原值）

**日期：** 2026-03-27 08:58

---

## 2026-03-26（MC Workbench Generate 後還原 + Rev-up to FOC switch-over）

### MC Workbench 重新生成專案 — 自訂程式碼全部被清空

**原因：** 使用者在 MC Workbench 選擇 Rev-up to FOC switch-over（On-the-fly 關閉），點選 Generate the project，導致 `mc_app_hooks.c` 恢復為空模板（USER CODE 內容全部消失），`drive_parameters.h` 所有自訂值全部重設為 Workbench 預設。

**目的：** 還原所有自訂程式碼，同時套用 Rev-up to FOC switch-over 帶來的新參數結構。

**修改方式（還原 mc_app_hooks.c）：**
- `/* USER CODE BEGIN Includes */`：補回 parameters_conversion.h / mc_interface.h / mc_api.h
- `/* USER CODE BEGIN 0 */`：補回 ThrottleState_t、全域變數、debug 變數、JoyToSpeed()
- `/* USER SECTION BEGIN PostMediumFrequencyHookM1 */`：補回完整油門邏輯

**修改方式（還原 drive_parameters.h）：**
| 參數 | Workbench 重置值 | 還原後 |
|------|----------------|--------|
| `MAX_APPLICATION_SPEED_RPM` | 4770 | **10000** |
| `IQMAX_A` | 3 | **4** |
| `PHASE1_DURATION` | 1000ms | **100ms** |
| `PHASE2_DURATION` | 3816ms | **1200ms** |
| `FW_VOLTAGE_REF` | 985 | **970** |

**Workbench 新增/變更（保留）：**
- `OBS_MINIMUM_SPEED_RPM`：1717 → **1900**（Workbench 依 Start-up threshold UI 設定）
- `TRANSITION_DURATION = 25ms`（Rev-up to FOC switch-over 新增）
- Rev-up to FOC switch-over：**ON**（啟動→RUN 切換更平滑）
- On-the-fly start-up：**OFF**（馬達每次從靜止啟動，不需要）

**日期：** 2026-03-26

---

## 2026-03-26（速度範圍擴展 + dbg_instantRPM）

### 擴展最高轉速至 10000 RPM + 新增即時轉速 debug 變數

**原因：**
1. `JOY_MAX_RPM=10000` 但 `MAX_APPLICATION_SPEED_RPM=4770`，MCSDK SpeednTorqCtrl 把所有 >4770 RPM 的命令截斷，馬達卡在 4770 無法提速。
2. START 階段 `dbg_actualRPM=0`（STO 平均 FIFO 尚未填滿），無法觀察爬升過程。

**目的：** 解除轉速上限，並新增可在啟動階段讀到即時轉速的 debug 變數。

**修改方式：**
- `drive_parameters.h`：`MAX_APPLICATION_SPEED_RPM` 4770 → **10000**
- `mc_app_hooks.c`：新增 `dbg_instantRPM`（用 `SPD_GetElSpeedDpp` × 25000/65536 × 60/POLE_PAIR_NUM 換算 RPM，START 階段可見）
- `mc_app_hooks.c`：`JOY_MIN_RPM` 1800 → **3000**（使用者手動設定）；`JOY_MAX_RPM` 4500 → **10000**（使用者手動設定）

**已知限制：** 實際最高轉速受電壓與負載限制，FW 介入時 `dbg_fwVoltPct` 接近 970。

**日期：** 2026-03-26

---

## 2026-03-26（PHASE2 多次嘗試 → 確認恢復原值）

### PHASE2_FINAL_SPEED 升高導致 STO SWITCH_OVER 失敗 — 恢復 Motor Profiling 原值

**原因：** 嘗試將 PHASE2_FINAL_SPEED 從 1908 升至 2800 再至 3000 RPM（目標與 JOY_MIN_RPM=3000 對齊），但 STO 在超出 Motor Profiling 驗證範圍的速度下無法完成 SWITCH_OVER 確認（`dbg_motorState` 永遠停在 4=START，`dbg_actualRPM` 始終為 0）。

**目的：** 恢復可靠啟動，START → RUN 的 1092 RPM 落差（1908→3000）由速度 PID 的 200ms ramp 自然補平。

**修改方式：**
- `drive_parameters.h`：`PHASE2_FINAL_SPEED_UNIT` 恢復 **(1908×SPEED_UNIT/U_RPM)**
- `drive_parameters.h`：`PHASE2_DURATION` 恢復 **1200ms**（Motor Profiling 原值）

**教訓：** PHASE2_FINAL_SPEED 不應超過 Motor Profiling 驗證的範圍，STO SWITCH_OVER 條件嚴格（速度帶 15/16~17/16 × PHASE2_FINAL，且需 NB_CONSECUTIVE_TESTS=2 次確認）。

**日期：** 2026-03-26

---

## 2026-03-26（Flux Weakening 導入說明）

### dbg_motorState 值對應表 — MCI_State_t 完整說明

`dbg_motorState` 直接反映 `MCI_GetSTMState(pMCI[M1])` 的回傳值（`MCI_State_t`），定義於 `Inc/mc_interface.h`。

| 值 | 枚舉名稱 | 說明 |
|----|----------|------|
| **0** | `IDLE` | 待機：馬達停止、無故障。等待 `MC_StartMotor1()` 命令。搖桿置中時應維持此狀態。 |
| **4** | `START` | 啟動中：Rev-Up 序列執行中（PHASE1 對齊 → PHASE2 加速 → SWITCH_OVER 切換）。此狀態持續 PHASE1+PHASE2 約 1300ms。 |
| **6** | `RUN` | 正常運行：STO 閉環速度控制中。`MCI_ExecSpeedRamp()` 在此狀態才有效。 |
| **8** | `STOP` | 停止程序執行中：`MC_StopMotor1()` 後短暫停留，PWM 關閉，等待馬達自然減速。 |
| **10** | `FAULT_NOW` | 故障剛發生：偵測到過電流/過壓/STO失鎖等保護，即時進入此狀態，PWM 立刻關閉。 |
| **11** | `FAULT_OVER` | 故障已穩定：從 `FAULT_NOW` 轉移，馬達已停。需呼叫 `MC_AcknowledgeFaultMotor1()` 清除後才能回 `IDLE`。狀態機自動處理。 |
| **20** | `WAIT_STOP_MOTOR` | 等待馬達停妥：停止後的緩衝保護期，確保馬達真正靜止再允許重新啟動。 |

**正常操作流程：**
```
搖桿推出死區 → IDLE(0) → START(4) ≈ 1.3s → RUN(6) → 搖桿回中 → STOP(8) → IDLE(0)
```

**故障流程：**
```
RUN(6) → FAULT_NOW(10) → FAULT_OVER(11) → AcknowledgeFault → IDLE(0)
```

---

### Flux Weakening 導入說明

**結論：FW 已啟用，無需額外啟動代碼。**

- `mc_tasks_foc.c:131-132`：`FW_Init()` 初始化
- `mc_tasks_foc.c:627`：`FW_CalcCurrRef()` 每個 FOC 週期（25kHz）自動執行
- `drive_parameters.h`：`FW_VOLTAGE_REF = 985`（98.5% Vs）

**FW 運作原理：**
| 條件 | 行為 |
|------|------|
| `|Vqd|` < 98.5% 最大電壓 | Id=0，正常 MTPA 運行 |
| `|Vqd|` ≥ 98.5% 最大電壓 | FW PI 輸出負 Id，降低磁通，騰出 Vq 給 Iq |

**本次調整：**
- `FW_VOLTAGE_REF`：985 → **970**（97%），FW 提早 1.5% 介入，高速段過渡更柔和
- `mc_app_hooks.c` 新增 debug 變數：`dbg_fwVoltPct`（當前電壓使用率‰）、`dbg_fwIdRef`（FW 注入的 -Id 量）

**觀察方法（Live Watch）：**
- `dbg_fwVoltPct`：低速時 ~500-800，接近 4500 RPM 時若 > 970 則 FW 介入
- `dbg_fwIdRef`：= 0 為未介入；< 0（負值）表示 FW 正在注入去磁電流

**Live Watch 建議清單（現階段有效變數）：**

| 優先 | 變數名 | 說明 |
|------|--------|------|
| ★必要 | `dbg_joyRaw12` | ADC 12-bit 原始值（置中≈2000）|
| ★必要 | `dbg_throttlePct` | 油門百分比 -100~+100% |
| ★必要 | `dbg_motorState` | 馬達狀態（0/4/6/8/10/11）|
| ★必要 | `dbg_thrState` | 油門狀態機（0=IDLE 1=RUNNING）|
| ★必要 | `dbg_actualRPM` | STO 估測轉速 RPM |
| ★必要 | `dbg_cmdRPM` | 命令轉速 RPM |
| ★必要 | `dbg_fwVoltPct` | FW 電壓使用率（‰，>970 時 FW 介入）|
| ★必要 | `dbg_fwIdRef` | FW 去磁電流（< 0 表示介入）|
| 選用 | `dbg_deflection` | 搖桿偏移量（排查死區）|
| 選用 | `STO_PLL_M1._Super.hElAngle` | 電氣角（排查 STO 用）|

**需移除的舊變數（not available，已不存在）：**
`s_targetAngle32`, `mciState`, `dbg_potRaw`, `dbg_potFiltered`, `dbg_state`, `dbg_targetAngle`, `dbg_sweepAngle`, `s_potFiltered`, `dbg_potDiff`, `dbg_potRaw12`

**日期：** 2026-03-26

---

## 2026-03-26 16:29

### 縮短 PHASE2 啟動時間 — Motor Profiling 後由 3816ms 改回合理值

**原因：** Motor Profiling 後 Workbench 將 `PHASE2_DURATION` 設為 3816ms，導致每次推搖桿要等約 4 秒馬達才進入 RUN（`dbg_motorState=4=START` 長時間停留，看起來像無反應）。

**目的：** 縮短啟動等待時間到 ~1.3 秒（PHASE1=100ms + PHASE2=1200ms）。

**修改方式：** 修改 `Inc/drive_parameters.h`：`PHASE2_DURATION` 3816ms → **1200ms**（OBS_MIN=1717 RPM，目標速 1908 RPM，加速需求更短）。

**日期：** 2026-03-26 16:29:02

---

## 2026-03-26 16:23

### 自訂程式碼包入 USER CODE 區塊 — 防止 MC Workbench 再次覆寫

**原因：** MC Workbench Motor Profiling 重新生成專案時完整覆寫 `mc_app_hooks.c`，自訂搖桿程式碼全部消失。

**目的：** 將自訂程式碼包入 `/* USER CODE BEGIN/END */` 保護區塊，使 MC Workbench 重新生成時能保留區塊內容。同時處理 `SPDPOT_Run` 與搖桿油門狀態機的衝突。

**修改方式：** 重新整理 `Src/mc_app_hooks.c`：
- `/* USER CODE BEGIN Includes */`：自訂 include（parameters_conversion.h / mc_interface.h / mc_api.h）
- `/* USER CODE BEGIN 0 */`：狀態機定義、全域變數、靜態變數、`JoyToSpeed()` 函式
- `/* USER CODE BEGIN BootHook */`：Boot 初始化（目前空，RCM/SPDPOT 已由 Workbench 生成）
- `/* USER SECTION BEGIN PostMediumFrequencyHookM1 */`：完整油門邏輯（ADC 濾波、油門映射、THR 狀態機），並加入 `SPDPOT_Run` 衝突處理（若搖桿在死區但馬達被啟動則停止）

**日期：** 2026-03-26 16:23:00

---

## 2026-03-26 16:13

### 恢復 mc_app_hooks.c — MC Workbench Motor Profiling 後被覆寫

**原因：** 重跑 MC Workbench Motor Profile 後，重新生成專案將 `mc_app_hooks.c` 全部覆寫（恢復為 2125 bytes 預設模板），搖桿油門程式碼全部消失，導致無法 debug（程式碼不完整）。

**Motor Profiling 新參數：**
- `OBS_MINIMUM_SPEED_RPM`：3551 → **1717 RPM**（STO 低速追蹤能力提升）
- `MAX_APPLICATION_SPEED_RPM`：9864 → **4770 RPM**（實際量測最高速）
- `MOTOR_VOLTAGE_CONSTANT`：0.822 → **0.9 Vrms/kRPM**
- `PHASE2_DURATION`：1000ms → **3816ms**，`PHASE2_FINAL_SPEED` = 1908 RPM（因 OBS_MIN 降低，rev-up 目標可更低）
- `IQMAX_A`：4 → 3（Workbench 覆寫，需手動補回）

**修改方式（恢復）：**
- `Src/mc_app_hooks.c`：完整重寫，恢復搖桿油門狀態機，更新 `JOY_MIN_RPM = 1800`（≥ 新 OBS_MIN 1717）、`JOY_MAX_RPM = 4500`（≤ 新 MAX_APP 4770）
- `Inc/drive_parameters.h`：`IQMAX_A` 3 → 4；`PHASE1_DURATION` 1000ms → 100ms

**日期：** 2026-03-26 16:13:39

---

## 2026-03-26 15:15

### 降低穩態電流 — IQMAX 調降 + 加速斜坡加長

**原因：** IQMAX_A=5A 後確認突破 5300 RPM 轉速限制，但加速期間電流衝到 2A，需降低電流使用。

**目的：** 在確保可達目標轉速前提下，減少加速電流峰值。

**修改方式：**
- `Inc/drive_parameters.h`：`IQMAX_A` 5 → **4**（夠用但避免過大）
- `Src/mc_app_hooks.c`：`MCI_ExecSpeedRamp` 斜坡時間 50ms → **200ms**（加速更柔和，降低瞬時電流）

**日期：** 2026-03-26 15:15:20

---

## 2026-03-26 15:10

### 增大 IQMAX_A — 突破轉速 5300 RPM 瓶頸

**原因：** 13V 供電，Holybro 2216 920KV 理論最高 ~11960 RPM，但實際轉速卡在 5300 RPM（命令 6200 RPM），確認並非 Back-EMF 電壓上限，而是速度 PID 受 `IQMAX_A=3A` 截斷，無法輸出足夠轉矩加速。

**目的：** 允許速度 PID 使用更高電流，突破 5300 RPM 瓶頸，使馬達可達 6000+ RPM。

**修改方式：** 修改 `Inc/drive_parameters.h`：
- `IQMAX_A`：`3` → `5`（5A，短時峰值，Holybro 2216 可承受）

**注意：** 長時間大油門請確認馬達溫度不超標。

**日期：** 2026-03-26 15:10:23

---

## 2026-03-26 14:45

### 修正啟動方向 — 負油門應逆時針啟動

**原因：** `THR_IDLE` 直接呼叫 `MC_StartMotor1()`，但 MCSDK VSS rev-up 方向由啟動前的速度命令決定。原本沒有預設方向，一律以 CW rev-up，導致負油門啟動也是順時針。

**目的：** 正油門 → 順時針啟動；負油門 → 逆時針啟動。

**修改方式：** 在 `Src/mc_app_hooks.c` 的 `THR_IDLE` 啟動分支中，在 `MC_StartMotor1()` 之前先呼叫 `MCI_ExecSpeedRamp(pMCI[M1], targetSpeed, 0)`，讓 MCSDK VSS 以目標速度的符號決定 rev-up 方向。

**注意：** 若 CCW rev-up 成功但 SWITCH_OVER 後 fault，代表此版 MCSDK STO 對逆時針方向有限制，需進一步排查。

**日期：** 2026-03-26 14:45:23

---

## 2026-03-26 14:22

### 速度映射改為最低轉速起跳（1% → JOY_MIN_RPM）

**原因：** 原本速度映射從 0 RPM 線性到 JOY_MAX_RPM，但 STO 最低追蹤速 = OBS_MINIMUM_SPEED_RPM ≈ 3551 RPM，低於此速度 STO 失鎖。油門小於某個值等於無效。

**目的：** 油門 1%（剛出死區）直接跳到 JOY_MIN_RPM（3600 RPM），之後線性到 JOY_MAX_RPM（8000 RPM），確保整個油門行程都在 STO 可靠運作範圍內。

**修改方式：** 修改 `Src/mc_app_hooks.c`：
- 新增 `#define JOY_MIN_RPM 3600`
- 修改 `JoyToSpeed()` 函式：以 `minSpeed = RPM_2_SPEED_UNIT(JOY_MIN_RPM)` 為起點，線性映射到 `maxSpeed = RPM_2_SPEED_UNIT(JOY_MAX_RPM)`；回傳值帶上方向符號（正=CW，負=CCW）

**日期：** 2026-03-26 14:22:28

---

## 2026-03-26 14:16

### 縮短 Rev-Up 時間 — 最小化啟動延遲

**原因：** MCSDK 預設 rev-up 需 ~9 秒（PHASE1=1000ms + PHASE2=7892ms），使用者每次推搖桿都要等到馬達衝到 3946 RPM 再掉回油門速度，體驗差。


**目的：** 盡量縮短 rev-up 時間，讓馬達更快進入可控 RUN 狀態。

**修改方式：** 修改 `Inc/drive_parameters.h`：
- `PHASE1_DURATION`：1000ms → **100ms**
- `PHASE2_DURATION`：7892ms → **1000ms**
- `PHASE2_FINAL_SPEED_UNIT`：3946 RPM → **3600 RPM**（貼近 OBS_MINIMUM_SPEED_RPM=3551，降低進 RUN 後的超速落差）

**已知限制：** STO 需要至少達到 OBS_MINIMUM_SPEED_RPM（3551 RPM）才能鎖定，此衝速現象無法完全消除，只能盡量縮小落差。

**日期：** 2026-03-26 14:16:40

---

## 2026-03-26 13:43

### 改用搖桿 Y 軸油門控制取代旋鈕角度定位

**原因：** 需求變更，改用 JoyStick X2 搖桿模組的 Y 軸作為油門輸入。

**目的：** 搖桿置中 → 停止；往上推 → 順時針（CW）；往下推 → 逆時針（CCW）。速度大小正比於搖桿偏移量。

**硬體量測：** 中心 1.61V，上限 3.28V，下限 0V（3.3V ADC）
- `JOY_CENTER_LA = 32000`（1.61V → 12-bit ≈ 2000 → la = 32000）
- `JOY_UP_RANGE_LA = 33104`（33104 la units = 3.28V 行程）
- `JOY_DOWN_RANGE_LA = 32000`（32000 la units = 0V 行程）

**修改方式：** 完整重寫 `Src/mc_app_hooks.c`：
- 移除 `AngCtrlState_t` 角度定位狀態機（SYNCING/MOVING/HOLDING 等）
- 新增 `ThrottleState_t`：`THR_IDLE(0)` / `THR_RUNNING(1)`
- `JoyToSpeed()` 將偏移量線性映射至 ±`RPM_2_SPEED_UNIT(2000)` 速度單位
- 保留 `g_angCtrlState = 0`、`g_angSweep = 0` 供 mc_tasks.c 相容
- 新增 Debug 變數：`dbg_joyRaw12`、`dbg_joyFiltered`、`dbg_deflection`、`dbg_targetSpeed`、`dbg_thrState`、`dbg_motorState`

**注意：** CCW（負速度）可能因 STO 低速失鎖而故障，故障後自動清除回 IDLE 重試。

**日期：** 2026-03-26 13:43:38

---

## 2026-03-26

### 上電後自動啟動同步旋轉

**原因：** 原本設計為旋鈕移動才觸發進入 `ANG_SYNCING`，導致每次上電後需要手動轉旋鈕才能啟動同步，操作較不直覺。

**目的：** 上電後立即自動進入 `ANG_SYNCING` 階段，馬達自動以 `SYNC_SPEED_RPM`（1000 RPM）旋轉 `SYNC_DURATION_MS`（20 秒），讓 STO 建立可靠的電氣角估測，完成後停止等待旋鈕輸入。

**修改方式：** 修改 `Src/mc_app_hooks.c` 的 `s_firstRun` 初始化區塊：
- 在預載旋鈕初值後，立即呼叫 `MC_StartMotor1()`
- 設定 `g_angCtrlState = ANG_SYNCING`，`s_syncMs = 0`
- 上電後即自動啟動同步，不再需要旋鈕觸發

**日期：** 2026-03-26 11:01:18

---

### 修正 HF Task — ANG_SYNCING 時誤覆寫 STO 角度導致過電流循環

**原因：** `mc_tasks.c` 的 `HighFrequencyTask 0` 條件為 `g_angCtrlState != 0u`，原本對應舊版三狀態機（IDLE=0, MOVING=1, HOLDING=2）是正確的，但加入 `ANG_SYNCING(1)` 與 `ANG_POST_SYNC_STOP(2)` 後條件未同步更新，導致同步階段進入 RUN 時，STO 估測的真實角度被強制蓋回 `g_angSweep`（≈ 0），產生角度突跳 → 過電流故障 → 重啟的無限循環。

**目的：** 讓 `ANG_SYNCING` / `ANG_POST_SYNC_STOP` 階段 STO 能自由估測真實電氣角，僅在 `ANG_MOVING(3)` 和 `ANG_HOLDING(4)` 時才對 STO 角度進行開迴路注入。

**修改方式：** 修改 `Src/mc_tasks.c` 的 `HighFrequencyTask 0`：
- 條件 `g_angCtrlState != 0u` 改為 `g_angCtrlState >= 3u`
- 同步更新 `extern` 宣告的狀態值注釋

**日期：** 2026-03-26 11:07:05

---

## 2026-03-25

### 建立 VSCode Debug 設定

**原因：** 專案透過 MC Workbench 產生後，缺少 VSCode 偵錯環境設定，無法在 VSCode 中直接編譯與燒錄除錯。

**目的：** 讓開發者可以在 VSCode 中一鍵編譯並透過 ST-Link 或 J-Link 進行 GDB 除錯，停在 main() 進入點。

**修改方式：** 新增 `.vscode/` 資料夾與以下四個設定檔：
- `.vscode/tasks.json` — 定義 CMake 設定（`--preset Debug`）與建置任務
- `.vscode/launch.json` — 定義三種除錯組態：
  1. `Debug (OpenOCD + ST-Link)` — 主要使用，自動觸發建置後啟動 GDB
  2. `Debug (J-Link)` — 備用，使用 J-Link 連接
  3. `附加至目標 (OpenOCD)` — 附加模式，不重新燒錄
- `.vscode/c_cpp_properties.json` — IntelliSense 設定，指定 arm-none-eabi-gcc 與 STM32G431xx 巨集
- `.vscode/extensions.json` — 推薦安裝的 VSCode 擴充套件

**日期：** 2026-03-25

---

### 修正工具路徑未加入 PATH 問題

**原因：** 執行 Debug 時出現「建置失敗：無法設定專案」，cmake、ninja、arm-none-eabi-gcc 三個工具皆未加入系統 PATH，VSCode 任務找不到可執行檔。

**目的：** 不修改系統 PATH，改由 VSCode 任務層級注入 STM32CubeIDE 內建工具的路徑，讓建置任務可以找到所有必要工具。

**修改方式：**
- `.vscode/tasks.json` — 在兩個任務的 `options.env.PATH` 中注入以下三個 STM32CubeIDE 內建工具目錄：
  - cmake：`plugins/com.st.stm32cube.ide.mcu.externaltools.cmake.../tools/bin`
  - ninja：`plugins/com.st.stm32cube.ide.mcu.externaltools.ninja.../tools/bin`
  - arm-none-eabi-gcc：`plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.../tools/bin`
- `.vscode/launch.json` — 在 `Debug (ST-Link GDB Server)` 組態加入 `serverpath`（ST-LINK_gdbserver.exe）與 `stm32cubeprogrammer`（STM32_Programmer_CLI.exe）完整路徑
- `.vscode/c_cpp_properties.json` — 更新 `compilerPath` 為 arm-none-eabi-gcc.exe 完整路徑

**日期：** 2026-03-25

---

### 修正 tasks.json — cmake 在 PowerShell 下找不到問題

**原因：** tasks.json 的 `options.env.PATH` 使用 Unix 格式路徑（`/c/ST/...`，分隔符為 `:`），但 VSCode 在 Windows 上預設以 PowerShell 執行任務，PowerShell 需要 Windows 格式路徑（`C:\ST\...`，分隔符為 `;`），導致 PATH 注入無效，cmake 執行時仍找不到。

**目的：** 讓 cmake、ninja、arm-none-eabi-gcc 的路徑注入在 PowerShell 環境下正常生效。

**修改方式：** 修改 `.vscode/tasks.json` 兩個任務的 `options.env.PATH`：
- 路徑分隔符由 `/` 改為 `\`
- 目錄清單分隔符由 `:` 改為 `;`
- 磁碟路徑由 `/c/ST/` 改為 `C:\ST\`

**日期：** 2026-03-25

---

### 新增旋鈕角度控制功能（開迴路，類步進馬達）

**原因：** 需求為旋轉 B-G431B-ESC1 上的可變電阻，馬達轉到對應角度後斷電節能；無編碼器，使用開迴路電氣角注入方式實現。

**目的：** 旋鈕靜止 → 馬達斷電。旋鈕轉動 → 馬達通電，從上次位置掃角至新目標，到位後保持 1.5 秒再斷電。

**修改方式：**
- `Src/mc_app_hooks.c` — 實作 `AngCtrlState_t` 狀態機（IDLE/MOVING/HOLDING），在 `MC_APP_PostMediumFrequencyHook_M1()`（1kHz）進行旋鈕讀值、角度映射、掃角推進、電流設定
- `Src/mc_tasks.c` — 在 `HighFrequencyTask 0`（25kHz，FOC 計算前）覆寫 `STO_PLL_M1._Super.hElAngle = g_angSweep`，使 FOC Park 轉換使用命令角度

**已知限制：** 啟動時 STO rev-up 序列會有短暫旋轉（STO 必須先建立鎖定才能接管）；無外力負載下定位精度可接受。

**日期：** 2026-03-25

---

### 手動修正 pmsm_motor_parameters.h — MOTOR_VOLTAGE_CONSTANT 未隨馬達更新

**原因：** MC Workbench 的 `pmsm_motor_parameters.h` 屬於「只產生一次」的檔案，後續 Generate 不會自動覆寫，導致馬達切換到 Holybro 2216 920KV 後，`MOTOR_VOLTAGE_CONSTANT` 仍停留在舊馬達（A2212）的值 `0.8`，正確值應為 `0.822 Vrms/kRPM`。

**目的：** 讓 FOC 電壓常數符合 Holybro 2216 920KV 的實際 B-Emf 參數。

**修改方式：** 手動編輯 `Inc/pmsm_motor_parameters.h`：
- `MOTOR_VOLTAGE_CONSTANT`：`0.8` → `0.822`

**日期：** 2026-03-25

---

### 修正 launch.json — ST-LINK GDB Server 找不到 STM32CubeProgrammer

**原因：** `stm32cubeprogrammer` 欄位設定為 exe 完整路徑（`...bin/STM32_Programmer_CLI.exe`），但 ST-LINK_gdbserver.exe 的 `-cp` 參數需要的是**目錄路徑**，gdbserver 將其視為目錄搜尋時找不到而退出。錯誤訊息為：`ERROR: Couldn't locate STM32CubeProgrammer in '...STM32_Programmer_CLI.exe'`。

**目的：** 讓 ST-LINK GDB Server 正確找到 STM32CubeProgrammer。

**修改方式：** 修改 `.vscode/launch.json` 的 `stm32cubeprogrammer` 欄位，由完整 exe 路徑改為目錄路徑：
```
"stm32cubeprogrammer": ".../cubeprogrammer.win32_.../tools/bin"
```

**日期：** 2026-03-25

---

### 修正 launch.json — arm-none-eabi-gdb.exe 找不到問題

**原因：** launch.json 各組態缺少 `armToolchainPath` 欄位，Cortex-Debug 擴充套件無法自行找到 `arm-none-eabi-gdb.exe`，啟動 GDB 偵錯時報錯。

**目的：** 讓 Cortex-Debug 正確找到 STM32CubeIDE 內建的 arm-none-eabi-gdb.exe。

**修改方式：** 在 `.vscode/launch.json` 三個組態（ST-Link、J-Link、OpenOCD）中各自加入：
```
"armToolchainPath": "C:/ST/STM32CubeIDE_1.18.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.win32_1.0.100.202602081740/tools/bin"
```

**日期：** 2026-03-25

---

### 修正 launch.json — servertype 不支援問題

**原因：** 執行 Debug 時出現錯誤「Invalid servertype parameters. The following values are supported: 'jli...'」，Cortex-Debug 擴充套件版本不支援 `openocd` servertype。

**目的：** 改用 `stlink` servertype，直接呼叫隨 STM32CubeIDE 安裝的 ST-Link GDB Server，不依賴 OpenOCD。

**修改方式：** 調整 `.vscode/launch.json`，將主要組態改為 `Debug (ST-Link GDB Server)`（servertype: stlink），OpenOCD 組態移至第三順位保留備用。

**日期：** 2026-03-25

---

### 修正角度控制 — 馬達一直自動轉停循環

**原因：** 兩個 bug 疊加：
1. `potDiff = newTarget - s_targetAngle32` 單位是 s16 電氣角度，但 `POT_DEADBAND=80` 原本設計為 ADC counts。80 個 s16 units ≈ 0.04° 電氣角，旋鈕任何微小抖動（甚至 ADC 雜訊）都會觸發 `potMoved`，導致 HOLDING 狀態立即回到 MOVING，形成無限循環。
2. `RCM_GetRegularConv` 實際回傳 **left-aligned 12-bit ADC**（呼叫 `LL_ADC_REG_ReadConversionData12L`），範圍 0~65520，而非 0~4095；`PotToElecAngle32` 除以 4095 導致計算結果偏大 16 倍。

**目的：** 讓旋鈕靜止時馬達穩定 HOLDING，不誤觸發重新掃角。

**修改方式：** 修改 `Src/mc_app_hooks.c`：
- 新增 `static uint16_t s_lastTrigPot` 記住上次觸發目標更新時的旋鈕 ADC 值
- `potDiff` 改為 `s_potFiltered - s_lastTrigPot`（ADC counts 比較，不再用 s16 角度）
- `POT_DEADBAND` 從 80 改為 1280（left-aligned ADC units = 80 × 12-bit counts）
- 所有更新 `s_targetAngle32` 的地方同步更新 `s_lastTrigPot = s_potFiltered`
- `PotToElecAngle32` 除數從 4095 改為 65520（= 4095 << 4，left-aligned 滿量程）

**日期：** 2026-03-25

---

### 修正旋鈕 ADC 範圍與 deadband — 馬達轉旋鈕沒反應

**原因：** 確認 B-G431B-ESC1 電路圖後發現旋鈕實際範圍：
- 電路：+3V3(ST-LINK) → R2(10kΩ) → PB12 → Pot(0~10kΩ) → GND
- Pot 滿量程電壓：0V（底）～1.65V（頂）
- ADC 12-bit 對應：0 ～ 2047；left-aligned：0 ～ 32752
- 之前 `PotToElecAngle32` 除以 65520（ADC 理論滿量程），但實際旋鈕只覆蓋一半，映射範圍錯誤
- `POT_DEADBAND=1280`（上一次修正時設定）佔旋鈕全行程 1280/32752 = 3.9%，需要較大幅度轉動才觸發；加上之前 deadband 過大問題共同導致無反應

**目的：** 讓旋鈕從底轉到頂能完整對應 0 ~ POLE_PAIR_NUM 圈電氣角，並降低 deadband 使觸發更靈敏。

**修改方式：** 修改 `Src/mc_app_hooks.c`：
- `PotToElecAngle32` 除數從 65520 改為 **32752**（= 2047 × 16，旋鈕實際滿量程 left-aligned）
- `POT_DEADBAND` 從 1280 改為 **80**（left-aligned = 5 個 12-bit counts = 0.24% 全行程）
- 新增 `dbg_potRaw12`（rawPot >> 4，顯示 0~4095 實際值）與 `dbg_potDiff`（距上次觸發的差值）

**日期：** 2026-03-25

---

### 修正馬達自動循環 — rev-up 結束後角度突跳導致過電流故障

**原因：** 馬達 rev-up 期間（START/SWITCH_OVER），HF task 不注入角度，STO 追蹤實際旋轉角度至幾百 RPM。進入 RUN 後，HF task 開始注入 `g_angSweep`（仍停留在上次停止的靜止位置），造成 FOC Park 轉換角度與馬達實際電氣角之間瞬間突跳數百 s16°。Back-EMF 與命令電流方向嚴重偏離 → 相電流過大 → `FAULT_NOW` → `FAULT_OVER` → 重新啟動 → 無限循環。

**目的：** 消除進入 RUN 時的角度突跳，讓 FOC 保持連續，馬達在自然減速過程中不觸發過電流故障。

**修改方式：** 修改 `Src/mc_app_hooks.c`：
- 新增 `static bool s_syncedOnRun`（每次進入 RUN 時重置）與 `static int32_t s_sweepRate`（掃角速率）
- 在 ANG_MOVING / RUN 分支：首次進入 RUN 時（`!s_syncedOnRun`）執行同步：
  - `s_sweepAngle32 = STO_PLL_M1._Super.hElAngle`（對齊至 STO 目前估測角）
  - `s_sweepRate = SPD_GetElSpeedDpp(&STO_PLL_M1._Super) × 25`（取 STO 電氣速度換算為 s16°/ms，@25kHz）
- 同步後：若 `|s_sweepRate| > STEP_PER_TICK`，每 tick 以目前速率推進角度並乘以 31/32 指數衰減（使馬達自然減速）；速率降至 STEP_PER_TICK 以下後改回固定步進向目標推進
- 在 FAULT_OVER 與 IDLE/STOP 分支中重置 `s_syncedOnRun = false`，確保故障重啟後重新同步
- 新增 `dbg_sweepRate` Live Watch 變數，便於觀察速率衰減過程

**日期：** 2026-03-25

---

### 修正馬達自動循環 — 解決 RUN 狀態時序競態問題

**原因：** 在 `mc_app_hooks.c` 的 1kHz 中頻任務中，首次進入 RUN 狀態時，程式會在同一個 1ms 週期內，同時「同步」STO 估測角，並「推進」到下一個角度。這個被提前計算的角度寫入 `g_angSweep` 後，會被 25kHz 高頻任務讀取並強制設定給 FOC，但此時馬達的實際角度尚未到達該位置，產生了微小的角度突跳，引發過電流故障並導致系統不斷重啟。

**目的：** 將「同步」和「推進」兩個動作分離，確保首次進入 RUN 時，FOC 使用的是當下真實的馬達角度，消除角度突跳，讓狀態轉換平順。

**修改方式：** 修改 `Src/mc_app_hooks.c` 中 `case ANG_MOVING:` 的 `mciState == RUN` 邏輯：
-   將原有邏輯拆分為 `if (!s_syncedOnRun)` 和 `else` 兩個區塊。
-   `if (!s_syncedOnRun)` 區塊 (首次進入 RUN 的 tick) **只執行**角度與速度的同步，然後設定旗標 `s_syncedOnRun = true`。
-   `else` 區塊 (後續的 tick) 才執行角度推進、速率衰減、目標更新等邏輯。
-   將 `MCI_ExecTorqueRamp()` 呼叫移至 `if/else` 結構之外，確保在 `RUN` 期間 (非 `HOLDING`) 持續施加轉矩。

**日期：** 2026-03-25 18:14:40
