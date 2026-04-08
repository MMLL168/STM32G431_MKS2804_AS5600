# ST_FOC 開發日誌

## 索引
<!-- devlog-index:start -->
- L91: 2026-04-08 13:54:10 (新增 PC10 零位校正按鈕)
- L106: 2026-04-08 12:40:00 (新增 PWM 角度目標模式)
- L121: 2026-04-08 11:25:27 (新增 ESP32 UART 旋鈕跟隨模式)
- L136: 2026-04-08 11:03:21 (修正 active 2804 220KV 的阻尼參數套用位置)
- L149: 2026-04-08 11:02:09 (分離感測速度零區與 hold 判斷並增加 active 2804 阻尼)
- L162: 2026-04-08 10:55:38 (放寬 active 2804 目標附近靜止容忍區)
- L175: 2026-04-08 10:34:12 (回彈更準但保留滑順感的 active 2804 參數調整)
- L188: 2026-04-08 10:02:48 (位置模式加入目標速度斜率限制與速度阻尼)
- L201: 2026-04-08 09:58:32 (移除 active 2804 220KV 小角度最小轉速地板)
- L214: 2026-04-08 09:54:52 (位置模式加入滑桿低端分段校正)
- L227: 2026-04-08 09:50:56 (修正小角度最小轉速套用到 2804 220KV active profile)
- L240: 2026-04-08 09:49:49 (微調位置模式前端死區與小角度啟動力)
- L253: 2026-04-08 09:42:49 (位置模式加入電位器有效行程校正)
- L266: 2026-04-08 09:38:48 (位置映射改為整個電位器行程對應 0 到 359 度)
- L279: 2026-04-08 09:29:10 (位置模式改為單圈絕對角控制)
- L292: 2026-04-08 09:15:06 (位置映射改為中心 0 與雙向 ±359 度)
- L305: 2026-04-08 08:56:07 (修正 speed 級聯零電流誤停車)
- L318: 2026-04-08 08:50:52 (位置模式改為中心 0 與雙向 360 度相對角)
- L331: 2026-04-08 08:13:36 (拆分 speed 級聯扭矩方向)
- L344: 2026-04-08 08:08:57 (2804 級聯位置模式收小電流權限)
- L357: 2026-04-08 07:49:16 (2804 有感位置模式改為 user-owned 級聯控制)
- L370: 2026-04-08 07:34:22 (speed 位置模式加入速度阻尼)
- L383: 2026-04-08 07:26:22 (拆分位置模式 speed/current 方向)
- L396: 2026-04-08 07:19:56 (speed 位置模式加入啟動目標對齊)
- L409: 2026-04-07 18:38:28 (新增 README 系統架構文件)
- L422: 2026-04-07 17:07:51 (位置模式改為可切換 speed 輸出)
- L435: 2026-04-07 16:01:40 (翻轉 2804 位置控制方向)
- L448: 2026-04-07 15:59:22 (移除位置模式濾波保留保守增益)
- L461: 2026-04-07 15:56:49 (位置模式加入輕量濾波抑制亂跳)
- L474: 2026-04-07 15:52:50 (還原到早期會抖動的有感位置版)
- L487: 2026-04-07 15:47:13 (修正 speed 位置模式 hold 重置問題)
- L500: 2026-04-07 15:42:22 (位置模式改為 speed 外迴路)
- L513: 2026-04-07 15:28:22 (位置模式上電座標對齊)
- L526: 2026-04-07 15:18:32 (位置模式加入目標鎖存避免自轉)
- L543: 2026-04-07 15:11:19 (位置模式加入持位平滑與去抖)
- L560: 2026-04-07 15:06:17 (位置模式改成正反轉鏡像對稱映射)
- L575: 2026-04-07 14:56:10 (位置模式改成左右完全對稱映射)
- L590: 2026-04-07 14:54:13 (位置模式電位器端點重疊)
- L605: 2026-04-07 14:40:02 (新增 AS5600 有感 FOC 骨架與模式選後端)
- L623: 2026-04-07 14:12:42 (依照 MKS 2804 220KV 規格新增 motor profile)
- L636: 2026-04-07 14:00:28 (新增低速轉動測試模式)
- L649: 2026-04-07 13:52:07 (新增可選 sensorless startup profile)
- L662: 2026-04-07 13:44:26 (補上馬達 fault 即時監控變數)
- L675: 2026-04-07 12:32:12 (切回 2804 單機角度模式並保留跟隨模式)
- L688: 2026-04-07 12:30:26 (新增外部 AS5600 旋鈕跟隨模式)
- L701: 2026-04-07 12:23:07 (切換至 2804 位置模式並加入馬達 profile)
- L717: 2026-04-07 12:15:19 (AS5600 常駐診斷變數)
- L732: 2026-04-07 11:22:18 (生成隔離與 devlog 自動化)
- L749: 2026-03-27 14:41:00 (Code Protection from Workbench)
- L769: 2026-03-27 14:38:00 (PI Revert & Troubleshooting)
- L785: 2026-03-27 14:32:00 (PI Optimization for Measured Ls Anomaly)
- L804: 2026-03-27 13:39:00 (Manual Parameter Tuning)
- L821: 2026-03-27 12:28:00 (Voltage & Current Expansion)
- L840: 2026-03-27 12:21:00 (Current Optimization Re-tuning)
- L858: 2026-03-27 12:15:00 (Current Optimization)
- L880: 2026-03-27 12:12:00 (Version Control)
- L899: 2026-03-27 12:08:00 (Motor Pilot Gauge Note)
- L916: 2026-03-27 12:03:00 (Update ST Motor Pilot Dashboard Scale)
- L935: 2026-03-27 11:55:46 (Update Max RPM)
- L951: 2026-03-27 11:47:00 (Add Debug Variables)
- L973: 2026-03-27 (Potentiometer Throttle Logic Restoration)
- L1000: 2026-03-27 (Compile Error Fix)
- L1014: 2026-03-27（本對話 #3 — 最新狀態）
- L1045: 2026-03-27（本對話 #3 — 當前狀態）
- L1082: 2026-03-27 (本對話 #2)
- L1096: 2026-03-26 (本對話)
- L1126: 2026-03-27（本對話 #3 診斷）
- L1187: 2026-03-27 08:58
- L1201: 2026-03-26（MC Workbench Generate 後還原 + Rev-up to FOC switch-over）
- L1233: 2026-03-26（速度範圍擴展 + dbg_instantRPM）
- L1254: 2026-03-26（PHASE2 多次嘗試 → 確認恢復原值）
- L1272: 2026-03-26（Flux Weakening 導入說明）
- L1344: 2026-03-26 16:29
- L1358: 2026-03-26 16:23
- L1376: 2026-03-26 16:13
- L1397: 2026-03-26 15:15
- L1413: 2026-03-26 15:10
- L1430: 2026-03-26 14:45
- L1446: 2026-03-26 14:22
- L1462: 2026-03-26 14:16
- L1482: 2026-03-26 13:43
- L1508: 2026-03-26
- L1539: 2026-03-25
<!-- devlog-index:end -->

---
## 2026-04-08 13:54:10 (新增 PC10 零位校正按鈕)

**原因：** 本地跟隨馬達與外部旋鈕馬達的機械 0 度不在同一個物理位置，需要在現場用板載按鈕快速把本地當前角度校正成新的 0 度參考。

**目的：** 讓使用者在不改外部旋鈕輸出與不重編譯的情況下，直接按 B-G431B-ESC1 的 PC10 按鈕完成本地跟隨馬達的零位對齊。

**修改方式：**
- 在 Inc/user_app.h 與 Src/stm32_mc_common_it.c 新增 user-owned 的 PC10 按鈕校正入口，於 POSITION/UART/PWM 三種 AS5600 sensored 目標模式下改由 UserApp_HandleUserButton() 接手，不再走原本的 Start/Stop callback。
- 在 Src/user_app.c 新增按鈕防彈跳、零位校正請求與 dbg_zeroCalCount，按下 PC10 後會用當前本地 AS5600 機械角與當前目標角計算新的 target offset，將本地跟隨馬達當前位置記成新的 0 度對應點。
- 調整 ResetPositionControllerState() 與目標角應用流程，讓 s_posTargetOffsetDeg10 在停車、暫停或目標中斷時仍保留，避免剛校正好的 0 度 offset 被重設。

**日期時間：** 2026-04-08 13:54:10

---

## 2026-04-08 12:40:00 (新增 PWM 角度目標模式)

**原因：** J3 的 USART2_TX/RX 與 ST-LINK 共線，外部 ESP32S 的 UART 角度串流容易互相干擾，因此改走 J3 PWM 腳位承載單圈目標角。

**目的：** 保留既有無感與 AS5600 模式，另外新增一個 user-owned 的外部 PWM 角度命令模式，讓遠端 2804+AS5600 旋鈕可經 PWM 輸入驅動本地 2804+AS5600 跟隨。

**修改方式：**
- 在 Inc/user_config.h、Inc/user_platform.h 新增 USER_CONTROL_MODE_PWM_TARGET_AS5600、J3 PWM/PA15 腳位與 1000~2000us 對應 0~359 度設定，並把 active 模式切到 PWM 角度目標模式。
- 在 Src/user_platform.c 新增 PA15 EXTI 雙邊緣脈寬量測、DWT cycle counter 換算、pulse width -> angleDeg10 映射與 online/frame/error 統計 API。
- 在 Src/stm32_mc_common_it.c 與 Src/user_app.c 補上 PWM 中斷入口、PWM 目標 debug 變數與 USER_CONTROL_MODE_PWM_TARGET_AS5600 控制分支，重用既有 AS5600 sensored position follower 追蹤外部 PWM 角度命令。

**日期時間：** 2026-04-08 12:40:00

---

## 2026-04-08 11:25:27 (新增 ESP32 UART 旋鈕跟隨模式)

**原因：** 需要讓另一組 2804+AS5600 經由 ESP32S UART 送出單圈角度，取代本地電位器，並讓目前的 2804+AS5600 當跟隨馬達；同時避免 USART2 被 MCSDK 的 ASPEP/Motor Pilot 佔用。

**目的：** 保留既有 2216 無感油門模式與本地位置模式，新增可切換的遠端 UART 角度目標模式，並讓 regenerate 後仍維持 user-owned 邏輯。

**修改方式：**
- 在 Inc/user_platform.h 與 Src/user_platform.c 新增 USART2 輪詢式 UART parser，解析 ESP32 發送的 T4.7615\r\n 格式，轉成 0~3590 deg10 單圈目標，並提供 online/frame/error debug 介面。
- 在 Src/user_app.c 新增 USER_CONTROL_MODE_UART_TARGET_AS5600 分支，將 UART 角度目標接到既有 AS5600 sensored 位置跟隨控制，並保留舊模式；同時把 position follower 主體抽成共用 helper，讓 pot 與 UART 模式共用同一套控制。
- 在 Src/mc_tasks.c 與 Src/stm32_mc_common_it.c 依 USER_PLATFORM_OWNS_USART2 加上 guard，讓新模式下停用 USART2 的 ASPEP start、packet process 與 UART/DMA interrupt 處理，避免與遠端 UART 角度串流衝突。

**日期時間：** 2026-04-08 11:25:27

---

## 2026-04-08 11:03:21 (修正 active 2804 220KV 的阻尼參數套用位置)

**原因：** 前一輪將 USER_POS_SPEED_KD_RPM_PER_RPM 提高到 2 時，實際上套到了 2804_100KV profile，active 的 2804_220KV 仍維持舊值 1，導致針對 fixed target hunting 的阻尼修正沒有真正作用在當前測試設定。

**目的：** 確保速度阻尼增加這項修正真正套用到目前 active 的 2804 220KV 位置模式。

**修改方式：**
- 將 Inc/user_config.h 的 2804_100KV profile 的 USER_POS_SPEED_KD_RPM_PER_RPM 還原為 1。,將 active 2804_220KV profile 的 USER_POS_SPEED_KD_RPM_PER_RPM 調整為 2，讓 fixed target 附近的速度阻尼真正作用在當前模式。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修正 profile 後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 11:03:21

---

## 2026-04-08 11:02:09 (分離感測速度零區與 hold 判斷並增加 active 2804 阻尼)

**原因：** 使用者回報在固定目標角附近仍會來回動，感覺像在門檻邊界。程式檢查後發現 USER_POS_SPEED_ZERO_WINDOW_RPM 同時被用於 hold 判斷與 UserSensored_MediumFrequencyUpdate() 的速度歸零，會讓低速附近的阻尼資訊太早消失。

**目的：** 保留目標附近的速度回授阻尼，降低 fixed target 時的小幅 hunting，同時維持 hold 邏輯的靜止判斷。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_SENSORED_SPEED_ZERO_WINDOW_RPM，讓感測速度歸零窗與 USER_POS_SPEED_ZERO_WINDOW_RPM 的 hold 判斷解耦；active 2804 220KV 先設為 5 rpm。,在 Src/user_sensored.c 將 mechanicalSpeedRpm 的歸零判斷改用 USER_SENSORED_SPEED_ZERO_WINDOW_RPM，而不再直接共用 USER_POS_SPEED_ZERO_WINDOW_RPM。,將 active 2804 220KV 的 USER_POS_SPEED_KD_RPM_PER_RPM 由 1 提高到 2，增加目標附近的速度阻尼；使用 tools/build.ps1 重新建置 Debug 韌體，確認 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 11:02:09

---

## 2026-04-08 10:55:38 (放寬 active 2804 目標附近靜止容忍區)

**原因：** 使用者回報在目標角附近仍會來回動。Live Watch 顯示誤差只剩幾度內時，dbg_cmdRPM 仍會輸出 10~30 rpm 的小命令，但 actualRPM 幾乎為 0，屬於目標附近的小幅 hunting。

**目的：** 讓 active 2804 220KV 在接近目標角時更容易安靜停住，不再持續做小幅試探修正。

**修改方式：**
- 將 active 2804 220KV 的 USER_POS_HOLD_ENTER_DEG10 / USER_POS_HOLD_EXIT_DEG10 由 8/25 放寬為 15/30。,將 USER_POS_SPEED_ZERO_WINDOW_RPM / USER_POS_CURRENT_ZERO_WINDOW_MA 由 12/12 放寬為 20/20，讓低速低力矩區更容易視為可接受的靜止狀態。,使用 tools/build.ps1 重新建置 Debug 韌體，確認放寬目標附近容忍區後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 10:55:38

---

## 2026-04-08 10:34:12 (回彈更準但保留滑順感的 active 2804 參數調整)

**原因：** 使用者希望用手故意扳離目標角後，回彈更準，但不要失去目前較滑順的手感。現有 active 2804 參數在 hold 門檻、零電流窗與速度到電流增益上偏保守，容易在接近目標時提早放掉修正。

**目的：** 提高回彈準確度，讓外力釋放後更容易回到命令角附近，同時保留速度斜率限制帶來的平順感。

**修改方式：**
- 將 active 2804 220KV 的 USER_POS_SPEED_KP_RPM_PER_DEG 由 3 提高到 4，USER_POS_SPEED_TO_CURRENT_KP_MA_PER_RPM 由 1 提高到 2，增加小誤差下的修正力。,將 active 2804 220KV 的 USER_POS_HOLD_ENTER_DEG10 / USER_POS_HOLD_EXIT_DEG10 由 20/50 收到 8/25，並把 USER_POS_SPEED_ZERO_WINDOW_RPM / USER_POS_CURRENT_ZERO_WINDOW_MA 由 25/40 收到 12/12，減少提早放掉修正的情況。,保留 USER_POS_TARGET_RPM_SLEW_RPM_PER_TICK 與 USER_POS_SPEED_KD_RPM_PER_RPM 的滑順化設計不變，並使用 tools/build.ps1 重新建置 Debug 韌體，確認 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 10:34:12

---

## 2026-04-08 10:02:48 (位置模式加入目標速度斜率限制與速度阻尼)

**原因：** 使用者希望位置模式手感更絲滑。目前 speed 分支會根據角度誤差直接跳出 targetRpm，且 USER_POS_SPEED_KD_RPM_PER_RPM 雖有定義但未實際用在 active 2804 220KV 的速度外環，導致命令變化偏硬。

**目的：** 讓 2804 位置模式在滑動電位器時更平順、不突兀，降低目標速度突變帶來的生硬感。

**修改方式：**
- 在 Inc/user_config.h 為各 profile 新增 USER_POS_TARGET_RPM_SLEW_RPM_PER_TICK，並將 active 2804 220KV 的 USER_POS_SPEED_KD_RPM_PER_RPM 設為 1。,在 Src/user_app.c 的 USER_POS_OUTPUT_MODE_SPEED 分支加入 s_posTargetRpmCmd 與 s_posFilteredSpeedRpm 狀態，對 rawTargetRpm 做斜率限制，並把 filteredSpeedRpm 的阻尼項真正套進外環計算。,使用 tools/build.ps1 重新建置 Debug 韌體，確認滑順化修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 10:02:48

---

## 2026-04-08 09:58:32 (移除 active 2804 220KV 小角度最小轉速地板)

**原因：** 加入低端分段校正後，active 2804 220KV 仍保留 USER_POS_SPEED_OUT_MIN_RPM = 60，導致目標附近小誤差時也被強制以固定最小速度正反追，形成來回擺動。

**目的：** 先消除目標附近的 bang-bang 震盪，保留分段校正帶來的低端角度手感改善。

**修改方式：**
- 將 active 2804 220KV profile 的 USER_POS_SPEED_OUT_MIN_RPM 由 60 調回 0，移除目標附近的固定最小速度地板。,保留 USER_POS_POT_BREAK_RAW / USER_POS_POT_BREAK_ANGLE_DEG10 的低端分段校正不變，先只排除 speed floor 對穩定性的影響。,使用 tools/build.ps1 重新建置 Debug 韌體，確認移除最小速度地板後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:58:32

---

## 2026-04-08 09:54:52 (位置模式加入滑桿低端分段校正)

**原因：** 根據 Live Watch，滑桿在 raw 6300 左右時使用者手感上已經滑了約四分之一，但線性映射下 target angle 仍只有十幾度，顯示滑桿輸出的 raw 值在低端分佈非常擠，單純裁端點不足以補償。

**目的：** 讓滑桿低端的手感角度變化更接近實際位移，避免低端大段移動只換到很小的角度。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_POT_BREAK_RAW 與 USER_POS_POT_BREAK_ANGLE_DEG10，定義位置模式專用的分段校正點，預設將 raw 約 6300 對應到 90 度。,在 Src/user_app.c 的 PotToAngleDeg10() 加入分段線性映射：低端 0~break 區間用較大的斜率展開，其餘區間再映射到剩餘角度。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:54:52

---

## 2026-04-08 09:50:56 (修正小角度最小轉速套用到 2804 220KV active profile)

**原因：** 前一輪調整 USER_POS_SPEED_OUT_MIN_RPM 時，誤將 60 rpm 套到 2804_100KV profile，而目前 active 的 2804_220KV profile 仍維持 0 rpm，導致建議的小角度啟動力沒有真正作用在當前測試設定。

**目的：** 確保小角度時的最小目標轉速確實作用在目前 active 的 2804 220KV 位置模式。

**修改方式：**
- 將 Inc/user_config.h 的 2804_100KV profile 的 USER_POS_SPEED_OUT_MIN_RPM 還原為 0。,將 Inc/user_config.h 的 active 2804_220KV profile 的 USER_POS_SPEED_OUT_MIN_RPM 改為 60，使小角度誤差時也有基本轉速命令。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修正 profile 後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:50:56

---

## 2026-04-08 09:49:49 (微調位置模式前端死區與小角度啟動力)

**原因：** 依據實測 Live Watch，電位器在 raw 0~3000 左右時 target angle 仍有明顯偏移，代表前端有效行程還需再收；同時小角度誤差下 dbg_cmdRPM 只有數十 rpm，actualRPM 仍為 0，顯示小角度命令太軟。

**目的：** 讓電位器低端更貼近 0 度起點，並讓小角度命令能夠推動 2804 更準確地靠近目標角。

**修改方式：**
- 將 Inc/user_config.h 的 USER_POS_POT_EFFECTIVE_MIN 由 2000 調整為 3300，進一步裁掉滑桿低端死區。,將 2804 220KV 位置模式的 USER_POS_SPEED_OUT_MIN_RPM 由 0 調整為 60，避免小角度誤差時目標轉速過小而推不動馬達。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:49:49

---

## 2026-04-08 09:42:49 (位置模式加入電位器有效行程校正)

**原因：** 使用者回報電位器在 0~2000 raw 之間角度看起來不動，超過後才有反應。這代表滑桿前端存在實際機械或 ADC 死區，但位置模式仍用理想 0~65520 全範圍做映射。

**目的：** 讓位置模式只使用滑桿真正有效的行程去對應 AS5600 的 0~359 度，避免端點死區吃掉一部分角度解析度。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_POT_EFFECTIVE_MIN 與 USER_POS_POT_EFFECTIVE_MAX，作為位置模式專用的有效行程校正參數，預設先收掉前後各 2000 raw。,在 Src/user_app.c 的 PotToAngleDeg10() 中改用 USER_POS_POT_EFFECTIVE_MIN/MAX 做 clamped 映射，超出有效行程的值直接夾到 0 度或 359 度。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:42:49

---

## 2026-04-08 09:38:48 (位置映射改為整個電位器行程對應 0 到 359 度)

**原因：** 使用者澄清需求為：電位器最小到最大對應順轉 0 到 359 度，最大回到最小則是逆轉 359 到 0 度；也就是整個電位器行程只對應 AS5600 的單圈絕對角，不要再做中心分半或正負角語意。

**目的：** 讓電位器與 AS5600 的單圈 0~359 度直接一一對應，滑動方向只影響目標角增減，不再使用中心 0 與上下半區不同語意。

**修改方式：**
- 在 Src/user_app.c 將位置模式的 PotToSignedAngleDeg10() 改為 PotToAngleDeg10()，映射改成整個電位器行程由 0 度到 359 度。,在 Src/user_app.c 的位置模式主流程中，將 rawTargetAngleDeg10 改為直接使用單圈絕對角，dbg_targetAngleDeg10 與實際控制都以這個單圈角為主，dbg_targetSignedAngleDeg10 僅保留為 signed 觀察值。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:38:48

---

## 2026-04-08 09:29:10 (位置模式改為單圈絕對角控制)

**原因：** 使用者回報滑桿電位器滑太快時，馬達會多轉一圈才到目標角度。根因是位置模式仍以相對多圈角度累加實際位置，再用 signed 目標直接相減，跨圈時會挑到錯的 turn branch。

**目的：** 讓電位器範圍真正對應 AS5600 的單圈 0~359 度，一圈內選對實際角度，不再因 fast move 或跨圈造成多轉一圈。

**修改方式：**
- 在 Src/user_app.c 取消位置模式對 AS5600 實際角度的多圈累加使用，改為直接用 AS5600 單圈絕對角與 NormalizeAngleDeg10 後的單圈目標角做控制。,在 Src/user_app.c 的 sensored position 分支，將 angleErrDeg10 改為 WrapAngleErrorDeg10(targetPhysicalAngleDeg10, actualAbsAngleDeg10)，保留 dbg_targetSignedAngleDeg10 只作命令語意顯示。,移除不再需要的多圈相對角 seed/reset 使用點，並重新建置 Debug 韌體，確認 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:29:10

---

## 2026-04-08 09:15:06 (位置映射改為中心 0 與雙向 ±359 度)

**原因：** 使用者明確要求電位器中心對應 AS5600 的 0 度，上半行程到 +359 度、下半行程到 -359 度；前一版仍是 ±360 度語意，且實際角度座標在首次上電時以 boot 位置為 0，會讓中心 0 與 AS5600 真正零點不一致。

**目的：** 讓電位器整個行程明確對應到 AS5600 的單圈角度語意，中心固定是 0 度，並讓 signed 位置追蹤以 AS5600 零點為基準而不是以開機位置為基準。

**修改方式：**
- 在 Src/user_app.c 新增 USER_SIGNED_TARGET_MAX_DEG10 = 3590，將 PotToSignedAngleDeg10() 的中心映射由 ±360 度改成 ±359 度。,在 Src/user_app.c 新增 AbsoluteToSignedAngleDeg10()，並將 UpdatePositionActualRelativeAngleDeg10() 的初次 seed 改為 AS5600 絕對角的 signed 表示，讓 pot 中心真正對應 AS5600 0 度，而不是對應開機當下的位置。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修改後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 09:15:06

---

## 2026-04-08 08:56:07 (修正 speed 級聯零電流誤停車)

**原因：** 位置模式在 speed->current 級聯中，電流命令穿越 0 mA 的那一拍會直接送 Stop，讓馬達狀態掉進 STOP=8；之後明明角度誤差還在，也只能等狀態機回到 IDLE，造成固定電位器時停在目標外、角度不準。

**目的：** 讓 2804 + AS5600 的位置模式在接近目標時不會因零電流交越而提早停車，保留 FOC 執行狀態以便立即修正殘餘角度誤差。

**修改方式：**
- 在 Src/user_app.c 的 USER_POS_OUTPUT_MODE_SPEED 分支，將 s_posCurrentCmdMilliAmp == 0 的處理改成比照 current 模式：只有在控制器本來就 idle 且狀態已經是 IDLE 時才保持關閉。,移除 speed 級聯每次零電流都呼叫 StopMotorDriveIfRunning() 的行為，改為持續呼叫 StartOrUpdateCurrentCommand(0) 保持可即時恢復扭矩。,使用 tools/build.ps1 重新建置 Debug 韌體，確認修正後 build/Debug/ST_FOC.elf 成功產生。

**日期時間：** 2026-04-08 08:56:07

---

## 2026-04-08 08:50:52 (位置模式改為中心 0 與雙向 360 度相對角)

**原因：** 使用者希望電位器在位置模式下改成中間 = 角度 0，往上代表正轉到 360 度，往下代表逆轉到 360 度。原本的單圈包角映射只適合 0~360 的絕對角或 ±180 的鏡像角，無法正確表達中心起算的 +360 / -360 相對角語意。

**目的：** 把 2804 + AS5600 的 active 位置模式改成以中心為零點的相對角控制，讓電位器上半圈對應 +0~360 度、下半圈對應 -0~360 度，並保留多圈相對角追蹤所需的實際角累積。

**修改方式：**
- 在 Src/user_app.c 將位置模式的電位器映射改成 PotToSignedAngleDeg10()，令電位器中心 = 0、上半圈 = +3600、下半圈 = -3600（單位 0.1 度）。,在 Src/user_app.c 新增位置模式專用的相對角累積狀態，利用 AS5600 每次量到的單圈角差累積成 s_posActualRelativeDeg10，讓控制器能分辨 +360 與 -360，而不是再次落回單圈最短路徑。,在 Src/user_app.c 的有感位置模式主分支中，改用 signed target 與 relative actual 計算 angle error，不再把 active 位置命令當成單圈包角；同時保留 dbg_targetAngleDeg10 / dbg_actualAngleDeg10 的單圈顯示，另外新增 dbg_targetSignedAngleDeg10 / dbg_actualSignedAngleDeg10 方便看真正的相對角。,在 ResetPositionControllerState() 一併重置相對角追蹤狀態與新的 signed debug 變數，避免切模式或停車後殘留舊的多圈角資訊。,使用 tools/build.ps1 重新建置 Debug 韌體，確認新的中心 0 / 雙向 360 度相對角位置模式可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-08 08:50:52

---

## 2026-04-08 08:13:36 (拆分 speed 級聯扭矩方向)

**原因：** 使用者回報新版本一啟動後會連續絲滑旋轉；對照 Live Watch 可見 dbg_cmdRPM 只有 ±120，但 dbg_actualRPM 持續約 1500 rpm，判斷不是目標角還在跳，而是 speed-to-current 級聯在應該煞車時仍沿用 current 模式的方向號誌，導致同方向持續推扭矩。

**目的：** 讓 2804 的 position -> speed -> current 級聯使用獨立的速度誤差到扭矩方向，避免 current 模式為了角度誤差翻轉的符號，誤套到 speed 級聯上造成連續旋轉。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_SPEED_TO_CURRENT_DIRECTION_SIGN，讓 speed 級聯的扭矩方向與 current 直推模式分開定義。,將 2804 220KV 的 USER_POS_SPEED_TO_CURRENT_DIRECTION_SIGN 設為 1，保留 current 模式的 USER_POS_CURRENT_DIRECTION_SIGN = -1，不再共用同一方向。,在 Src/user_app.c 的 speed-to-current 級聯分支，將 targetCurrentMilliAmp 的方向改用 USER_POS_SPEED_TO_CURRENT_DIRECTION_SIGN，而不是 USER_POS_CURRENT_DIRECTION_SIGN。,使用 tools/build.ps1 重新建置 Debug 韌體，確認拆分 speed 級聯扭矩方向後 build/Debug/ST_FOC.elf 可成功產生。

**日期時間：** 2026-04-08 08:13:36

---

## 2026-04-08 08:08:57 (2804 級聯位置模式收小電流權限)

**原因：** 燒入 user-owned 級聯控制後，使用者回報固定 target angle 時仍會一段一段震動前進；Live Watch 顯示 dbg_cmdRPM 只有數百 rpm，但 dbg_actualRPM 仍衝到 1000~1500 rpm，判斷目前 2804 位置模式不是方向再次反了，而是 speed-to-current 級聯仍然太容易打到 1A 飽和，造成分段式過衝。

**目的：** 把 2804 有感位置模式的輸出權限收斂到更接近伺服用途的量級，讓 target 改變時不再每次都以接近飽和電流猛推，降低區段式抖動與過衝。

**修改方式：**
- 在 Inc/user_config.h 為各 motor profile 新增 USER_POS_MAX_CURRENT_MA 與 USER_RUNTIME_MAX_CURRENT_MA，讓 user-owned 控制與 runtime tuning 都能以 mA 等級設定最大電流，不再受限於整數安培粒度。,在 Inc/user_config.h 將 2804 220KV 的 USER_POS_SPEED_KP_RPM_PER_DEG 降到 3、USER_POS_SPEED_OUT_MAX_RPM 降到 120、USER_POS_SPEED_TO_CURRENT_KP_MA_PER_RPM 降到 1，並把 USER_POS_CURRENT_SLEW_MA_PER_TICK 收到 8，避免 target 角一改就大幅推流。,在 Inc/user_config.h 將 2804 220KV 的 USER_POS_MAX_CURRENT_MA / USER_RUNTIME_MAX_CURRENT_MA 設為 350 mA，並同步降低 runtime Iq/Id PI 到較保守的 384/64，減少 current loop 對 2804 gimbal motor 的過激反應。,在 Src/user_platform.c 將 runtime tuning 的電流限制改為使用 mA 轉 digit，不再以 USER_RUNTIME_MAX_CURRENT_A 直接乘 CURRENT_CONV_FACTOR。,在 Src/user_app.c 將有感位置模式 current clamp 改為 USER_POS_MAX_CURRENT_MA，讓 2804 位置控制真正以 350 mA 等級限制 q-axis command。,使用 tools/build.ps1 重新建置 Debug 韌體，確認調整後 build/Debug/ST_FOC.elf 可成功產生。

**日期時間：** 2026-04-08 08:08:57

---

## 2026-04-08 07:49:16 (2804 有感位置模式改為 user-owned 級聯控制)

**原因：** 使用者回報 2804 + AS5600 位置模式只要快速移動電位器就會劇烈震動、角度語意也不正確；重新從頭檢查後，確認目前 active 路徑同時存在兩個結構性問題：位置模式的 boot target offset 讓電位器不再代表固定絕對角，且 speed 型位置模式仍間接依賴 2216 生成的 MCSDK speed PI，導致快動命令時內外迴路耦合失衡。

**目的：** 在不破壞 2216 無感油門模式的前提下，把 2804 + AS5600 位置模式整理成語意正確、模式邊界清楚的 user-owned 級聯控制，降低快動命令時的震動與角度錯位。

**修改方式：**
- 在 Inc/user_config.h 關閉 USER_POS_BOOT_ALIGN_TO_SHAFT，恢復 2804 位置模式的絕對角命令語意，避免 target 在開機後被靜默平移。,在 Inc/user_config.h 為 2804 profile 重整 position speed/current 參數，移除 speed 模式最小速度門檻、降低最大位置速度、關閉外層速度微分項，並新增 USER_POS_SPEED_TO_CURRENT_KP_MA_PER_RPM 供 user-owned 級聯控制使用。,在 Src/user_app.c 將 USER_POSITION_OUTPUT_MODE = SPEED 的有感位置模式改成 位置誤差 -> 目標 RPM -> 速度誤差 -> q-axis current，不再經過 MCSDK 生成的 speed PI；同時沿用 current command 的方向定義與電流 slew/hold 邏輯。,在 Src/user_platform.c 新增 runtime motor tuning override，依 USER_MOTOR_PROFILE 對 2804 路徑套用較保守的 speed PI / current PI / torque limit / speed limit，而不改寫 2216 的生成參數檔。,在 README.md 更新 active architecture，明確標示目前 2804 active path 為 position -> speed -> current 的 user-owned cascade，避免文件與實作脫節。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 build/Debug/ST_FOC.elf 可成功產生。

**日期時間：** 2026-04-08 07:49:16

---

## 2026-04-08 07:34:22 (speed 位置模式加入速度阻尼)

**原因：** 使用者回報 speed 型位置模式雖然方向較正確，但仍會先劇烈震動，再固定慢速朝單一方向旋轉，代表單純以角度誤差成比例輸出目標 rpm 仍過於激進且缺乏速度阻尼。

**目的：** 把目前 active 的 position -> speed 模式調成更接近伺服控制：角度誤差決定目標速度，實際轉速提供阻尼，接近目標時先緩降到 0 rpm 而不是立刻 Stop，以降低過衝與來回震盪。

**修改方式：**
- 在 Inc/user_config.h 為各 motor profile 新增 USER_POS_SPEED_OUT_MIN_RPM、USER_POS_SPEED_OUT_MAX_RPM、USER_POS_SPEED_KD_RPM_PER_RPM 與 USER_POS_SPEED_KEEP_RUN_AT_ZERO，讓 speed 型位置控制能獨立調整最小速度、最大速度、速度阻尼與接近目標時是否維持 RUN。,在 Inc/user_config.h 針對 2804 220KV 降低 USER_POS_SPEED_KP_RPM_PER_DEG 到 6、拉長 USER_POS_RAMP_MS 到 350 ms、放寬 hold enter/exit 視窗，並限制 speed output max rpm 為 420，以降低接近目標前的過激反應。,在 Src/user_app.c 的 speed 型位置分支加入 	argetRpm = Kp * angleError - Kd * actualSpeed，並增加 dbg_posTargetRpm 方便直接觀察外層位置環算出的目標速度。,在 Src/user_app.c 的 speed 型 hold 分支中，若控制器原本就在運轉，改為下 0 rpm speed command 而不是直接 Stop，減少接近目標時反覆停機 / 啟動帶來的抖動。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 speed 型位置伺服化與新 debug 變數加入後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-08 07:34:22

---

## 2026-04-08 07:26:22 (拆分位置模式 speed/current 方向)

**原因：** 使用者回報在 speed 型位置模式下，電位器改變後會先劇烈震動，接著固定以逆時針慢速轉動；Live Watch 顯示 cmdRPM=-900 但目標角相對當前角的最短路徑實際應是正向，判斷 speed 版與 current 版共用同一個 USER_POS_DIRECTION_SIGN 已互相干擾。

**目的：** 讓 position -> speed 與 position -> current 各自使用正確方向定義，避免前面為 current 模式翻過的方向設定把 speed 模式也一起帶反。

**修改方式：**
- 在 Inc/user_config.h 將 USER_POS_DIRECTION_SIGN 拆成 USER_POS_CURRENT_DIRECTION_SIGN 與 USER_POS_SPEED_DIRECTION_SIGN，讓兩種位置輸出路徑可獨立設定方向。,在 Inc/user_config.h 對 2804 220KV 設定 USER_POS_CURRENT_DIRECTION_SIGN = -1、USER_POS_SPEED_DIRECTION_SIGN = 1，保留 current 版先前調出的方向，同時把目前 active 的 speed 版改回正向。,在 Src/user_app.c 讓 current 分支改用 USER_POS_CURRENT_DIRECTION_SIGN，speed 分支與 sensorless-fallback 的 speed 路徑改用 USER_POS_SPEED_DIRECTION_SIGN，避免共用同一方向巨集。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 speed/current 方向拆分後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-08 07:26:22

---

## 2026-04-08 07:19:56 (speed 位置模式加入啟動目標對齊)

**原因：** 使用者回報切到 position -> speed 後，Live Watch 顯示 target angle 固定在遠離當前軸角的位置，導致控制器持續以固定方向慢速轉動去追遠方目標；需要讓 speed 模式在進入時先對齊當前軸角。

**目的：** 避免 2804 + AS5600 在 speed 型位置模式下，因為電位器固定位置對應到遠方絕對角度而持續自動旋轉；同時保留 speed 模式較易收斂的優點。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_BOOT_ALIGN_TO_SHAFT 開關，讓位置模式可在進入控制時把電位器角度座標對齊到當前軸角。,在 Src/user_app.c 為 sensored 位置模式加入 s_posTargetOffsetDeg10 與 s_posTargetOffsetSeeded，第一次取得有效軸角時自動計算 raw target 與實際角度之間的 offset。,在 Src/user_app.c 的 speed 型位置分支中，將 raw target 先套用 offset 後再作為最終 target angle，並新增 dbg_rawTargetAngleDeg10 與 dbg_posTargetOffsetDeg10 供 Live Watch 觀察。,在 Src/user_app.c 重新引入 StopMotorDriveIfRunning()，讓 speed 模式 hold 時只停馬達輸出、不重置 target offset，避免每個週期都重新 seed 而永遠無法動作。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 speed 型位置模式加入啟動目標對齊後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-08 07:19:56

---

## 2026-04-07 18:38:28 (新增 README 系統架構文件)

**原因：** 使用者要求把目前專案的完整系統架構整理成文件，包含軟體架構、硬體接線圖、控制流程與時序，並寫入 README.md 且標記日期時間。

**目的：** 讓後續維護、除錯、重新 Generate 以及模式切換時，都能直接從 README.md 了解整體架構、接線、控制路徑與主要設定入口，降低口頭說明遺失的風險。

**修改方式：**
- 新增 README.md，整理目前 active setup、控制模式、馬達 profile 與位置輸出模式，並標記文件更新時間。,在 README.md 以 ASCII 圖方式加入整體系統架構圖、軟體架構圖、硬體接線圖、控制模式圖以及開機 / 中頻 / 有感 FOC 的時序流程圖。,在 README.md 補充 generated code 與 user-owned code 的邊界、常用切換點、主要檔案對照與建置 / Live Watch 建議，讓文件同時可作為維護手冊。

**日期時間：** 2026-04-07 18:38:28

---

## 2026-04-07 17:07:51 (位置模式改為可切換 speed 輸出)

**原因：** 依照使用者同意的建議，將 2804 + AS5600 的位置控制從直接輸出 q 軸電流改成較容易收斂的 position -> speed 版本，同時保留原本 current 型位置環作為可切換選項。

**目的：** 讓目前 active 的 2804 位置模式先走較穩定的 speed loop 路徑，減少直接扭力控制造成的鐘擺震盪，同時保留 current 版供之後比較或回切。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_OUTPUT_MODE_CURRENT 與 USER_POS_OUTPUT_MODE_SPEED，並加入 USER_POSITION_OUTPUT_MODE 作為位置模式輸出選擇開關。,在 Inc/user_config.h 將目前 active setup 的 USER_POSITION_OUTPUT_MODE 設為 USER_POS_OUTPUT_MODE_SPEED，讓 2804 + AS5600 先走較穩的 position -> speed 路徑。,在 Src/user_app.c 擴充 StartOrUpdateSpeedCommand 的編譯條件，讓 AS5600 sensored position mode 於 speed output 模式下可直接使用 MCSDK 的 speed ramp。,在 Src/user_app.c 將 sensored position mode 拆成 current / speed 兩條分支：current 分支保留原本 q 軸電流控制，speed 分支則依角度誤差產生目標 rpm 並交給 StartOrUpdateSpeedCommand。,在 Src/user_app.c 新增 dbg_posOutputMode 監看變數，方便在 Live Watch 直接確認目前 position mode 走的是 speed 還是 current 輸出。,使用 tools/build.ps1 重新建置 Debug 韌體，確認切換成可選的 speed / current position output 後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 17:07:51

---

## 2026-04-07 16:01:40 (翻轉 2804 位置控制方向)

**原因：** 使用者回報即使電位器固定不動，馬達仍在目標點附近大幅來回甩動。從 Live Watch 可見 target 幾乎固定，但 actual angle 與 actual RPM 仍持續大幅變化，較像位置控制方向相反造成的正回授，而不是單純濾波或目標映射問題。

**目的：** 先用最小變更驗證 2804 + AS5600 目前的外層位置控制方向是否反了，避免在方向未對的前提下持續誤調增益。

**修改方式：**
- 在 Inc/user_config.h 將 USER_MOTOR_PROFILE_2804_220KV 的 USER_POS_DIRECTION_SIGN 由 1 改為 -1，只翻轉 2804 位置控制外環方向，不影響 2216 無感油門模式與其它 profile。,使用 tools/build.ps1 重新建置 Debug 韌體，確認翻轉 2804 位置控制方向後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 16:01:40

---

## 2026-04-07 15:59:22 (移除位置模式濾波保留保守增益)

**原因：** 使用者回報加入輕量濾波後雖然亂跳改善，但來回擺動仍可達約 30 度，懷疑濾波延遲反而放大來回修正，因此要求先不要濾波。

**目的：** 回到無濾波的有感位置控制版本，同時保留目前較保守的 2804 位置控制增益，讓後續能直接判斷剩餘的來回擺動是否主要來自控制參數而不是濾波延遲。

**修改方式：**
- 在 Inc/user_config.h 移除 USER_POS_POT_LPF_SHIFT 與 USER_POS_ANGLE_LPF_SHIFT，讓位置模式不再宣告電位器與機械角低通參數。,在 Src/user_app.c 移除 s_posFilteredPotRaw、dbg_potFiltered 與 FilterPotRaw()，讓位置模式重新直接用原始電位器 ADC 值換算目標角度。,在 Src/user_sensored.c 移除 filteredMechanicalAngleDeg10 與機械角環狀低通，讓 AS5600 讀值重新直接作為機械角來源。,保留前一輪調整過的 2804 220KV 保守 position gains、hold window 與 damping 參數，不一起回退，方便把『濾波影響』和『控制參數影響』分開驗證。,使用 tools/build.ps1 重新建置 Debug 韌體，確認移除濾波後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:59:22

---

## 2026-04-07 15:56:49 (位置模式加入輕量濾波抑制亂跳)

**原因：** 使用者回報即使回到較早的 current 型有感位置基線，2804 + AS5600 在靜止附近仍會一直亂跳，判斷主因是電位器 ADC 微抖與 AS5600 機械角在低速時的微小噪聲共同喚醒位置電流環。

**目的：** 在不再次大改架構的前提下，先用輕量濾波和更保守的 2804 位置參數，把靜止附近的抖動壓下來，保留目前已知可動作的 current 型位置控制基線。

**修改方式：**
- 在 Inc/user_config.h 為各 motor profile 新增 USER_POS_POT_LPF_SHIFT 與 USER_POS_ANGLE_LPF_SHIFT，讓位置模式可對電位器輸入與 AS5600 機械角各自做輕量低通濾波。,在 Inc/user_config.h 將 2804 220KV 的 position tuning 調得更保守，降低 USER_POS_CURRENT_KP_MA_PER_DEG、提高 USER_POS_CURRENT_KD_MA_PER_RPM，並放寬 hold / zero window、降低 current slew，減少靜止附近反覆喚醒與過衝。,在 Src/user_app.c 新增 s_posFilteredPotRaw 與 dbg_potFiltered，讓 sensored position mode 先對電位器 ADC 做簡單 IIR 低通，再轉成目標角度。,在 Src/user_sensored.c 對 AS5600 機械角加入環狀低通濾波，利用 WrapDeltaAngleDeg10 處理跨 0/360 度，再將濾波後角度餵給位置誤差計算，直接降低角度噪聲造成的誤差跳動。,使用 tools/build.ps1 重新建置 Debug 韌體，確認加入輕量濾波與保守 2804 參數後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:56:49

---

## 2026-04-07 15:52:50 (還原到早期會抖動的有感位置版)

**原因：** 使用者確認最近幾輪為了抑制自轉而加入的 target 鎖存、上電座標對齊與 speed 外迴路效果不理想，希望先回到較早那版『會動、但靜止時會發抖』的 2804 + AS5600 有感位置控制狀態。

**目的：** 把目前 2804 + AS5600 的位置模式還原到加入持位平滑與去抖之後、但尚未加入目標鎖存 / 啟動對齊 / speed 型輸出的版本，讓後續調整重新從已知可動作的 current 型位置控制基線開始。

**修改方式：**
- 在 Inc/user_config.h 移除 USER_POS_OUTPUT_MODE 相關設定，讓位置模式回到原本固定使用 current 型輸出，不再切到 speed 外迴路。,在 Inc/user_config.h 移除 USER_POS_BOOT_ALIGN_TO_SHAFT 與 USER_POS_POT_UPDATE_DEADBAND_RAW / USER_POS_TARGET_SLEW_DEG10_PER_TICK 等後續追加的 anti-self-rotation 參數，回到較早的 position tuning 介面。,在 Src/user_app.c 移除 target 鎖存、pot offset、target slew、speed output 切換與其對應 debug 變數，讓 sensored 位置模式重新回到電位器角度直接驅動 current 型 position loop 的版本。,在 Src/user_app.c 將 StopMotorControlIfRunning() 還原成原本的完整停止與重置位置狀態行為，避免保留只為後期 speed 版加入的分拆 helper。,使用 tools/build.ps1 重新建置 Debug 韌體，確認還原後的基線版本仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:52:50

---

## 2026-04-07 15:47:13 (修正 speed 位置模式 hold 重置問題)

**原因：** 使用者回報切換成 speed 型位置外迴路後馬達完全不轉，檢查控制流程後確認 hold 狀態下呼叫 StopMotorControlIfRunning() 會連同位置控制狀態一起重置，導致每個週期都重新 seed target 到當前角度，永遠無法形成有效角度誤差。

**目的：** 讓 2804 + AS5600 的 speed 型位置模式在停車保持時只關閉馬達輸出，不重置 pot offset 與 target seed 狀態，避免位置命令每個週期被洗掉而無法啟動。

**修改方式：**
- 在 Src/user_app.c 拆出 StopMotorDriveIfRunning()，專門負責停止馬達輸出並保留位置控制內部狀態。,保留原本 StopMotorControlIfRunning() 作為需要完整清除控制狀態的路徑，例如感測器離線或其它真正需要 reset position state 的情況。,在 Src/user_app.c 的 speed 型 sensored 位置模式 hold 分支改用 StopMotorDriveIfRunning()，讓 hold 時只停 PWM，不會把 s_posTargetSeeded、pot offset 與 target command 一起清掉。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 hold 狀態修正後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:47:13

---

## 2026-04-07 15:42:22 (位置模式改為 speed 外迴路)

**原因：** 使用者回報雖然上電自轉已改善，但只要一調電位器 2804 就會亂轉，代表目前直接輸出 q 軸電流的 position loop 對這顆馬達過於敏感，出現過衝與不穩定。

**目的：** 把 2804 + AS5600 的位置模式改成較保守的外層位置對速度控制，讓使用者調整電位器時先用角度誤差產生目標轉速，再交給 MCSDK 速度環處理，降低一動就暴衝的機率，同時保留 current 型輸出作為可選模式。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_OUTPUT_MODE_SPEED 與 USER_POS_OUTPUT_MODE_CURRENT，讓位置模式可明確選擇輸出到 speed loop 或 current loop，避免之後誤用。,在 Inc/user_config.h 將目前各 motor profile 的 USER_POS_OUTPUT_MODE 預設設為 USER_POS_OUTPUT_MODE_SPEED，先讓 2804 位置模式走較穩定的 speed 型控制。,在 Src/user_app.c 擴充 StartOrUpdateSpeedCommand 的編譯條件，讓 AS5600 sensored 位置模式在 speed output 模式下也能直接使用 MCSDK speed ramp。,在 Src/user_app.c 保留原本 current 型位置控制分支，但新增 USER_POS_OUTPUT_MODE 條件切換；當選擇 speed output 時，位置模式會依角度誤差產生目標 rpm 並呼叫 StartOrUpdateSpeedCommand，而不是直接下 q 軸電流。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 speed 型位置外迴路切換後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:42:22

---

## 2026-04-07 15:28:22 (位置模式上電座標對齊)

**原因：** 使用者回報位置模式下電位器沒有操作時馬達仍會自己轉動，檢查 Live Watch 後確認是上電時電位器映射角度與當前軸角度不一致，控制器持續追逐未對齊的目標。

**目的：** 讓 2804 + AS5600 的位置模式在進入控制時先把命令座標對齊目前軸角度，避免剛上電或剛切模式就因座標不一致而自發旋轉，同時保留原本 2216 無感油門模式不受影響。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_POS_BOOT_ALIGN_TO_SHAFT 開關，集中控制位置模式是否在啟動時將電位器命令座標對齊到當前軸角度。,在 Src/user_app.c 的 sensored 位置模式加入 s_posPotAngleOffsetDeg10，第一次取得有效 AS5600 角度時自動計算 raw pot angle 與實際軸角度之間的 offset。,在 Src/user_app.c 將後續接受到的電位器角度命令都先套用 offset 再作為 position target，避免電位器停在某個固定位置時，因為座標系不同而讓馬達自己追到另一個角度。,在 Src/user_app.c 新增 dbg_rawTargetAngleDeg10 與 dbg_posPotOffsetDeg10，方便在 Live Watch 同時比對原始 pot 角度和啟動對齊 offset。,使用 tools/build.ps1 重新建置 Debug 韌體，確認上電座標對齊機制已成功編譯並產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 15:28:22

---

## 2026-04-07 15:18:32 (位置模式加入目標鎖存避免自轉)

**原因：** 使用者回報在沒有主動操作時馬達會自己亂轉，需要先抑制命令端的自發動作。

**目的：** 讓有感位置模式上電後先穩定停在當前位置，只有在電位器真的被操作時才開始改變目標角。

**修改方式：**
- 在 Inc/user_config.h 為位置模式加入電位器更新死區與目標角斜率限制參數，讓 target 不會因 ADC 小波動或剛上電時的目標差異立刻大幅跳動。
- 調整 Src/user_app.c，讓有感位置模式在第一次取得 AS5600 角度時先把目標鎖到當前實際角度，只有電位器明顯移動超過門檻後才接受新的 pot 目標。
- 在 Src/user_app.c 對 pot 目標加入 SlewAngleToward() 角度斜率限制，降低目標突變造成的自發旋轉與突兀追角。
- 保留原本的持位遲滯與電流平滑，讓這次修正主要聚焦在 command 端防自轉，而不影響 sensorless 2216 模式。
- 用 tools/build.ps1 驗證 Debug build 成功，確認防自轉修正已正常連結。

**日期時間：** 2026-04-07 15:18:32

---

## 2026-04-07 15:11:19 (位置模式加入持位平滑與去抖)

**原因：** 使用者回報馬達在靜止時會持續發抖，希望位置模式停住時更平穩、更絲滑。

**目的：** 降低 AS5600 角度與速度微小波動造成的電流來回修正，讓有感位置模式更接近穩定的定位伺服手感。

**修改方式：**
- 在 Inc/user_config.h 為位置模式加入持位遲滯、速度零區、電流零區、電流斜率限制與速度低通參數，讓不同馬達 profile 可分別調整靜止穩定度。
- 調整 Src/user_sensored.c，把 AS5600 推導出的機械速度加入低通與零速窗，降低站立時速度量測抖動對阻尼項的影響。
- 調整 Src/user_app.c 的有感位置模式控制，加入持位進出遲滯、電流命令平滑器與持位狀態，讓靜止時不在死區邊緣反覆來回補償。
- 新增 dbg_posHoldActive 方便在 Live Watch 觀察目前是否進入平穩持位區。
- 用 tools/build.ps1 驗證 Debug build 成功，確認這次平滑化控制修改可正常連結。

**日期時間：** 2026-04-07 15:11:19

---

## 2026-04-07 15:06:17 (位置模式改成正反轉鏡像對稱映射)

**原因：** 使用者澄清希望的是以電位器中心為 0，左右兩側分別代表正轉與逆轉的鏡像對稱，而不是單純把兩端折到同一個角度。

**目的：** 讓位置模式的目標角具備正負方向意義，操作手感符合鏡像對稱需求，同時維持兩端在物理上重疊於 180 度。

**修改方式：**
- 調整 Src/user_app.c 的 PotToAngleDeg10() 為真正的鏡像正反轉映射，讓位置模式下電位器中心 = 0 度、右側 = 正角 0~+180、左側 = 負角 0~-180（以 360-角度編碼）。
- 保留最小值與最大值在物理上重疊到 180 度，但左右兩邊的目標角現在會以中心為對稱軸，分別代表正轉與逆轉。
- 用 tools/build.ps1 驗證 Debug build 成功，確認映射修正沒有引入編譯問題。

**日期時間：** 2026-04-07 15:06:17

---

## 2026-04-07 14:56:10 (位置模式改成左右完全對稱映射)

**原因：** 使用者希望電位器在位置模式下不只端點重疊，左右兩側也要完全對稱。

**目的：** 讓電位器最小值與最大值對應同一個目標角，並讓中心點成為對稱軸，提升操作直覺。

**修改方式：**
- 調整 Src/user_app.c 的 PotToAngleDeg10() 為對稱三角映射，讓位置模式下電位器最小值 = 0 度、中心 = 180 度、最大值 = 0 度。
- 保留 AS5600 位置模式的最短路徑誤差計算，只修改目標角產生方式，讓左右兩邊的定位手感完全對稱。
- 用 tools/build.ps1 驗證 Debug build 成功，確認新映射沒有引入編譯問題。

**日期時間：** 2026-04-07 14:56:10

---

## 2026-04-07 14:54:13 (位置模式電位器端點重疊)

**原因：** 使用者希望電位器最小值與最大值在位置模式下對應到同一個最終目標點，避免兩端看起來是不同角度。

**目的：** 讓 0 度與 360 度在電位器兩端明確重疊，使用旋鈕繞一圈時邏輯更直覺。

**修改方式：**
- 調整 Src/user_app.c 的 PotToAngleDeg10()，讓位置模式下電位器最大值映射到 0 度，與最小值重疊成同一個目標點。
- 保留 0~360 度整圈映射與最短路徑誤差計算，只修正端點行為，不影響原本 2216 無感油門模式。
- 用 tools/build.ps1 驗證 Debug build 成功，確認這次端點折返沒有引入編譯問題。

**日期時間：** 2026-04-07 14:54:13

---

## 2026-04-07 14:40:02 (新增 AS5600 有感 FOC 骨架與模式選後端)

**原因：** 依 USER_CONTROL_MODE 保留原本 2216 無感油門模式，同時把 2804 + AS5600(I2C) 位置控制改成 user-owned 的有感 FOC 骨架，避免後續 CubeMX / MC Workbench 生成覆蓋核心邏輯。

**目的：** 讓模式切換時能自動選擇 sensorless 或 AS5600 sensored backend，並把 AS5600 絕對角度真正接進 FOC 控制角度來源。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_FOC_BACKEND 推導與 sensored 對位參數，並把 active setup 切到 2804 + AS5600 的位置模式。
- 在 Inc/Src/user_platform.* 抽出 UserPlatform_ReadAs5600RawAngle()，讓 AS5600 低層 I2C 讀取可由 app 與 sensored 控制共用。
- 新增 Inc/user_sensored.h、Src/user_sensored.c，建立 user-owned 的 AS5600 速度/角度回授 handle、對位基準、角度預測與 telemetry getter。
- 新增 Src/user_sensored_foc.c，在 USER_CONTROL_MODE_POSITION_AS5600 時以強符號覆寫 TSK_MediumFrequencyTaskM1() 與 FOC_HighFrequencyTask()，改走 boot-cap -> 對位 -> sensored RUN 的 state flow。
- 調整 Src/user_app.c，讓 position mode 在有感 backend 下使用 UserSensored 的機械角回授，deadband 內改送 0 速保持而不是每次停機重啟，並補充 backend / sensored debug 變數。
- 更新 CMakeLists.txt 納入新的 user-owned sensored 檔案，並用 tools/build.ps1 驗證 Debug build 成功。

**日期時間：** 2026-04-07 14:40:02

---

## 2026-04-07 14:12:42 (依照 MKS 2804 220KV 規格新增 motor profile)

**原因：** 你補了 MKS 2804 的實際規格照片，顯示這顆馬達是 220KV、12V、7 pole pairs、最大轉速 3840RPM，而不是之前暫時假設的 100~110KV；需要把使用者可選的 motor profile 與 startup 參數改成對應這顆實際馬達。

**目的：** 保留原本的 2804 100~110KV 假設 profile 供比較或回退，同時新增真正對應照片規格的 2804 220KV profile，並把 active setup 切到這顆馬達的 gentle sensorless 啟動配置。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_MOTOR_PROFILE_2804_220KV，保留原本 USER_MOTOR_PROFILE_2804_100KV，不直接覆蓋舊假設設定。,在 Inc/user_config.h 將 active setup 切到 USER_MOTOR_PROFILE_2804_220KV，並把 startup profile 改成 USER_STARTUP_PROFILE_2804_GENTLE_SENSORLESS。,在 Inc/user_config.h 依照片規格為 MKS YT2804 220KV 定義新的 app-level throttle、position、follow 與 fixed low-speed spin 參數，讓測試轉速與定位範圍更符合 220KV / 3840RPM 的實際馬達。,在 Inc/user_config.h 依 MKS 2804 220KV 重新定義 gentle sensorless 與 low-speed test 的 startup 參數，包括 default target speed、phase2 final speed、observer minimum speed 與 transition duration。,使用 tools/build.ps1 重新建置 Debug 韌體，確認新增 2804 220KV profile 後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 14:12:42

---

## 2026-04-07 14:00:28 (新增低速轉動測試模式)

**原因：** 目前想先單純驗證 2804 的 sensorless 啟動、UVW 輸出、開環到閉環切換是否正常，不想再被電位器角度誤差 deadband 影響，也不希望為了測試去覆寫另一顆馬達之後要用的設定。

**目的：** 提供一個使用者可選的固定低速轉動模式，讓電位器只負責停車 / 正轉 / 反轉，使啟動測試與角度控制邏輯分離，方便先確認底層馬達控制鏈路。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_CONTROL_MODE_LOW_SPEED_SPIN_TEST，並把 active setup 切到 2804 的低速轉動測試模式。,在 Inc/user_config.h 讓電位器在此模式下只作為 enable / direction selector，並為 2216 與 2804 各自定義 USER_TEST_SPIN_RPM、USER_TEST_SPIN_RAMP_MS、USER_TEST_SPIN_DIRECTION_SIGN，避免覆寫另一顆馬達的測試設定。,在 Inc/user_config.h 將 2804 low-speed startup profile 的 DEFAULT_TARGET_SPEED_RPM 調整到 650rpm，讓固定低速測試與 startup observer 門檻更一致。,在 Src/user_app.c 將 PotToSignedPercent 的條件編譯擴充到低速測試模式，並新增固定低速轉動控制分支：電位器偏離中心就以固定 rpm 啟動，回到中心就停車。,在 Src/user_app.c 依 active mode 條件編譯 UserAbsI32，移除切到低速測試模式後的 unused warning。,使用 tools/build.ps1 重新建置 Debug 韌體，確認新增低速測試模式後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 14:00:28

---

## 2026-04-07 13:52:07 (新增可選 sensorless startup profile)

**原因：** 目前 2804 定位測試已確認電位器與 AS5600 鏈路正常，但馬達卡在 START 狀態，尚未進入 RUN；需要先試一版保守的開環啟動到閉環切換參數，同時不能覆寫掉未來還要用到的 2216 啟動設定。

**目的：** 把 rev-up / observer 啟動參數做成 user_config 可切換的 startup profile，讓使用者能在不破壞另一顆馬達設定的前提下，試不同的 sensorless 啟動策略。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_STARTUP_PROFILE 選項，提供 MCSDK default、2804 gentle sensorless、2804 low-speed test 三種啟動策略，並把目前 active setup 設成 USER_STARTUP_PROFILE_2804_LOW_SPEED_TEST。,在 Inc/user_config.h 為 2804 startup profile 定義獨立的 default target speed、phase1/phase2 duration、phase current、phase2 final speed、observer minimum speed 與 transition duration，避免直接覆蓋 2216 的啟動設定。,在 Inc/user_config.h 加入 2216 profile 的 safety gate，防止誤把 2804 的 startup 測試選項套到 2216 上。,在 Inc/drive_parameters.h 的 user-protected overrides 區塊引入 user_config.h，並依 USER_STARTUP_OVERRIDE_ENABLE 條件式覆寫 DEFAULT_TARGET_SPEED_RPM、PHASE1/PHASE2、ENABLE_SL_ALGO_FROM_PHASE、OBS_MINIMUM_SPEED_RPM 與 TRANSITION_DURATION，讓 Generate 後仍保留此切換機制。,在 Src/user_app.c 增加 dbg_startupProfile 監控變數，方便在 Live Watch 直接確認目前跑的是哪一套啟動策略。,使用 tools/build.ps1 重新建置 Debug 韌體，確認 startup profile 選擇機制已成功編譯並產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 13:52:07

---

## 2026-04-07 13:44:26 (補上馬達 fault 即時監控變數)

**原因：** 在目前 2804 定位測試中，電位器與 AS5600 數值都有變化，但馬達不會動，Live Watch 顯示 motorState=10；需要直接看到 MCSDK fault bitmask，才能分辨是啟動失敗、欠壓、過流或其他保護介入。

**目的：** 讓除錯資訊直接出現在 debugger / Live Watch，不必每次再回頭查 state number 對應或猜 fault 類型，加快現場 bring-up 判斷。

**修改方式：**
- 在 Src/user_app.c 增加 dbg_currentFaults、dbg_occurredFaults、dbg_commandState 三個即時監控變數。,在 UserApp_PostMediumFrequencyHook_M1() 內更新 MC_GetCurrentFaultsMotor1()、MC_GetOccurredFaultsMotor1() 與 MC_GetCommandStateMotor1() 的回傳值，讓 fault 與命令狀態可直接從 Live Watch 觀察。,使用 tools/build.ps1 重新建置 Debug 韌體，確認新增監控變數後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 13:44:26

---

## 2026-04-07 12:32:12 (切回 2804 單機角度模式並保留跟隨模式)

**原因：** 目前 2216 還沒有裝 AS5600，因此先回到電位器對應 2804 馬達角度 0~360 的單機定位模式，比較符合現階段硬體狀態；同時保留剛新增的跟隨模式，方便後續 2216 補上角度感測器後直接再切回測試。

**目的：** 讓現在的 active setup 與手上硬體一致，避免誤進入沒有第二顆感測器的跟隨模式；同時保留後續雙感測器擴充的程式路徑。

**修改方式：**
- 在 Inc/user_config.h 將 active setup 改回 USER_CONTROL_MODE_POSITION_AS5600，並把 USER_MOTOR_PROFILE 切回 USER_MOTOR_PROFILE_2804_100KV，讓電位器繼續當 2804 的目標角度來源。,在 Inc/user_config.h 保留 USER_CONTROL_MODE_FOLLOW_AS5600_KNOB 與對應參數定義，不刪除跟隨模式，讓之後 2216 裝上 AS5600 後可以直接切換回去。,由於目前 MC Workbench 低層參數仍是 2216，將 USER_ALLOW_PROFILE_MISMATCH 改回 1，允許現階段以 2804 app profile 進行 bring-up 與定位測試。,在 Src/user_app.c 調整條件編譯範圍，移除 position 模式下不再使用的 NormalizeAngleDeg10 編譯警告，保持 build 輸出乾淨。,使用 tools/build.ps1 重新建置 Debug 韌體，確認切回單機角度模式後仍可成功產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 12:32:12

---

## 2026-04-07 12:30:26 (新增外部 AS5600 旋鈕跟隨模式)

**原因：** 希望把 2804 加上 AS5600 當成外部旋鈕，並讓另一顆 2216 依照旋鈕角度跟隨；同時保留原本油門與單機角度模式，避免之後切換功能時混淆。

**目的：** 把控制模式切分成更清楚的 user_config 選項，先提供可實測的原型跟隨模式，並把目前沒有 2216 第二顆角度感測器的限制明確寫進設定與程式。

**修改方式：**
- 在 Inc/user_config.h 新增 USER_CONTROL_MODE_FOLLOW_AS5600_KNOB，並把目前 active setup 切到 2216 跟隨 2804+AS5600 旋鈕的模式。,在 Inc/user_config.h 依 2216 與 2804 profile 補上 knob-follow 專用速度上下限、deadband、比例增益、ramp 與 bootstrap 假設，讓不同馬達配置不會共用同一組跟隨參數。,在 Src/user_app.c 新增外部 AS5600 旋鈕跟隨控制邏輯，利用 AS5600 角度當目標角度，並用 2216 的平均轉速積分出虛擬跟隨角度做近似閉迴路控制。,在 Src/user_app.c 增加 dbg_sourceAngleDeg10、dbg_virtualAngleDeg10、dbg_followEstimateSeeded 等 debug 變數，方便後續在 debugger 觀察旋鈕角度、虛擬跟隨角度與估測器狀態。,使用 tools/build.ps1 重新建置 Debug 韌體，確認新模式可成功編譯並產出 build/Debug/ST_FOC.elf。

**日期時間：** 2026-04-07 12:30:26

---

## 2026-04-07 12:23:07 (切換至 2804 位置模式並加入馬達 profile)

**原因：** 使用者目前已接上 AS5600，並將馬達換成 2804 低 KV 電機，同時還要保留原本 2216 高 KV 馬達的油門模式，避免日後切換模式或馬達時混用錯誤參數。

**目的：** 在 user_config.h 內把控制模式與馬達 app profile 分開管理，讓 2216 / 2804 的外層控制參數可明確切換，並加入 MCSDK tune mismatch 的安全閘，降低誤用風險。

**修改方式：**
- `Inc/user_config.h` — 改成「控制模式 + 馬達 profile + MCSDK tune profile」結構，新增 2216 與 2804 兩組 app-level 參數，並把目前設定切到 `USER_CONTROL_MODE_POSITION_AS5600` + `USER_MOTOR_PROFILE_2804_100KV`。
- `Inc/user_config.h` — 新增 `USER_MCSDK_TUNE_PROFILE` 與 `USER_ALLOW_PROFILE_MISMATCH`，明確標示目前底層 MCSDK 仍是 2216 生成參數，避免之後切換時忘記 Workbench 尚未重生。
- `Src/user_app.c` — 補上 `dbg_motorProfile`、`dbg_mcsdkTuneProfile`、`dbg_motorProfileMismatch`，並依目前控制模式整理條件編譯，清掉未使用函式 warning。
- `build/Debug/ST_FOC.elf` — 重新建置確認新設定可正常編譯連結。

**日期時間：** 2026-04-07 12:23:07

---

## 2026-04-07 12:15:19 (AS5600 常駐診斷變數)

**原因：** 使用者已完成 AS5600 接線，需要快速確認 I2C 是否真的讀得到資料，不希望每次驗證都臨時改控制模式或手動下 GDB 函式呼叫。

**目的：** 讓韌體在油門模式與角度模式下都會定期更新 AS5600 的讀取狀態與角度 debug 變數，之後可直接用 debugger / memory read 驗證感測器連線。

**修改方式：**
- `Src/user_app.c` — 新增 `dbg_as5600ReadOk`，並把 AS5600 讀取函式改成兩種控制模式都可用。
- `Src/user_app.c` — 在中頻 hook 內加入低頻率 `UpdateAs5600Telemetry()` 輪詢，定期更新 `dbg_as5600Raw` 與 `dbg_actualAngleDeg10`，不影響原本油門控制邏輯。
- `build/Debug/ST_FOC.elf` — 重新建置並燒錄到 B-G431B-ESC1，之後以 SWD 直接讀 RAM 驗證感測器回應。

**日期時間：** 2026-04-07 12:15:19

---

## 2026-04-07 11:22:18 (生成隔離與 devlog 自動化)

**原因：** 專案會持續使用 CubeMX 與 MC Workbench 重新生成，先前 AS5600/I2C 與建置修補直接寫在生成檔內，後續 Generate 容易被覆蓋；同時需要建立固定的 devlog 更新流程與頂部索引。

**目的：** 把自訂功能收斂到 user-owned 檔案與頂層 CMake，讓生成檔只保留最小 user hook，並讓每次修改都能自動留下帶時間戳與行號索引的紀錄。

**修改方式：**
- `CMakeLists.txt` — 將 `-mthumb`、I2C HAL driver 與 `HAL_I2C_MODULE_ENABLED` 移到頂層 user-owned CMake，避免依賴生成 CMake/hal_conf 手改。
- `Src/main.c`、`Src/mc_app_hooks.c`、`Inc/main.h`、`Src/stm32g4xx_hal_msp.c` — 把 AS5600/I2C 與控制實作移出生成檔，生成檔只保留 `UserPlatform_Init()` 與 `UserApp_PostMediumFrequencyHook_M1()` 這類最小 hook。
- `Src/user_platform.c`、`Inc/user_platform.h`、`Src/user_app.c`、`Inc/user_app.h` — 新增 user-owned 平台與控制模組，集中管理 PB7/PB8 的 I2C1、AS5600 讀值與模式控制邏輯。
- `tools/update-devlog.ps1`、`.vscode/tasks.json`、`.agent/workflows/devlog_rule.md` — 新增 devlog 自動寫入/重建索引腳本、VS Code task 與紀錄準則。

**日期時間：** 2026-04-07 11:22:18

---


## 2026-03-27 14:41:00 (Code Protection from Workbench)

### 包裝自訂參數與常式，避免被 MC Workbench 重新覆蓋

**原因：** 使用者準備使用 ST MC Workbench 重新生成專案以修復 $0.02mH$ 量測失誤問題。但這會導致 `drive_parameters.h` 遭到重寫（洗掉我們微調的啟動電流與 `IQMAX`），且 `mc_app_hooks.c` 會自動解除註解恢復原廠的 `SPDPOT_Run()`，導致兩套油門程式互相衝突打架。

**目的：**
透過將我們的微調移入專屬的 `USER CODE` 區塊，完美保全客製化參數，讓使用者可以安心且無數次地 Generate 原廠韌體。

**修改內容：**
1. **`drive_parameters.h`**:
   - 利用檔案最底部的 `/* ##@@_USER_CODE_START_##@@ */` 標籤，放入 `#undef` 來覆寫上半段原廠生成的常數。
   - 當前受保護的心血結晶包含：啟動柔和參數 (`PHASE1/2_CURRENT` 與 `DURATION`)、`IQMAX_A = 4` 以及 `OV_VOLTAGE = 17V`。
2. **`mc_app_hooks.c`**:
   - 在置頂的 `/* USER CODE BEGIN Includes */` 中，硬性宣告 `#define SPDPOT_Run(a, b) false`，廢除掉原廠滑桿的干涉。這樣就算未來 Generate 把我們之前註解掉的程式碼自動解開，我們自創的滑桿油門也不會被干擾。

**日期時間：** 2026-03-27 14:41:00

---

## 2026-03-27 14:38:00 (PI Revert & Troubleshooting)

### 撤銷手動 x10 倍電流環增益 (出現高頻振盪)

**原因：** 實測證明 `PID_TORQUE_KP` 放大 10 倍過於激進，導致 PI 控制迴路直接進入高頻不穩定狀態 (高頻噪音伴隨漏電流暴增)。在欠缺精確馬達參數 ($0.02mH$ 測量錯誤) 的情況下，僅單方面手動修改單一電流環 $K_p$，而無法同步去修改定點數的 STO 觀測器矩陣，造成了兩邊數學模型相位打架、崩潰振盪。

**修改內容 (`drive_parameters.h`)：**
1. **`PID_TORQUE_KP_DEFAULT`**: 緊急退回原始安全的 **`2081`**。
2. **`PID_FLUX_KP_DEFAULT`**: 緊急退回原始安全的 **`2081`**。

**後續計畫：** 實驗證明無法直接手動修改 C Code 修復電感錯誤。必須回到原廠 ST MC Workbench 軟體，進入 Motor 頁面將 $0.02mH$ 手動覆寫為正常的 $0.2mH \sim 0.3mH$ 之間，並重新點擊 Generate，以全面性且安全地由系統更新所有觀測器與電流環的底層數學常數！

**日期時間：** 2026-03-27 14:38:00

---

## 2026-03-27 14:32:00 (PI Optimization for Measured Ls Anomaly)

### 手動修正 Torque PI 以挽救相電感量測失誤造成的功耗過高

**原因：** 使用者提供的 Motor Profiler 截圖顯示，2216 920KV 馬達的被測電感量 ($L_s$) 僅為 `0.02 mH` (20 μH)。這對該級別馬達而言是極為嚴重的縮水測量誤差（真實物理值應約為 `0.2 mH`）。因為硬體電流環的 $K_p \propto L_s$，這導致 Workbench 自動算出來的 `PID_TORQUE_KP` (2081) 比正常值弱了整整 10 倍！
如此孱弱的電流環 (Torque PI) 在高轉速 (高電氣頻率) 下會產生巨大的「**相位落後 (Phase Lag)**」。送進去的電流錯過了最佳磁場排斥角度，淪為純發熱的無效電流。為了維持轉速，速度 PID 只能請求遠大於正常 `0.3A` 極限的爆量 $I_q$，因此導致空載電流異常居高不下。

**目的：**
不透過 Workbench 重新生成，直接手動將 C Code 裡面的「扭力與磁通 KP 增益」乘上 10 倍，強制還原應有的控制頻寬與相位反饋準確度，逼出馬達真實的極致效率。

**修改內容 (`drive_parameters.h`)：**
1. **`PID_TORQUE_KP_DEFAULT`**: 由極弱的 `2081` 強制放大 10 倍為 **`20810`**。
2. **`PID_FLUX_KP_DEFAULT`**: 同樣放大 10 倍為 **`20810`**。
3. **`PID_TORQUE_KI_DEFAULT`** 等維持不變 (3330)，因為積分 $K_i$ 是依賴電阻 $R_s$ 計算的，其量測值 $R_s=0.1\Omega$ 是準確的。

**日期時間：** 2026-03-27 14:32:00

---

## 2026-03-27 13:39:00 (Manual Parameter Tuning)

### 使用者自訂參數微調 (MAX_RPM, OV, IQMAX)

**原因：** 使用者根據實測情況，手動將前一次放寬的安全防護與極限參數往回微調，以求取更平衡且安全的運作表現。

**修改內容：**
1. **`mc_app_hooks.c`**:
   - `MAX_RPM` (目標滿油門轉速): 從 `10000` RPM 調降為 **`8000`** RPM。
2. **`drive_parameters.h`**:
   - `OV_VOLTAGE_THRESHOLD_V` (硬體過壓保護閾值): 由 `20V` 回調至原本適中且較為安全的軟體極限 **`17V`**。
   - `IQMAX_A` (閉環全域電流極限): 由大額度弱磁使用的 `8A` 降回至原本實測不錯的 **`4A`**。

**日期時間：** 2026-03-27 13:39:00

---

## 2026-03-27 12:28:00 (Voltage & Current Expansion)

### 提升供電電壓與放寬電流限制以突破轉速天花板

**原因：** 為了突破 5000 RPM 的物理反電動勢 (BEMF) 電壓極限，並且順利讓轉速衝升至 10000 RPM，使用者將電壓由原本的 14V 提升至 4S 鋰電池額定電壓 (14.8V，滿電約 16.8V)。同時我們必須解除 FOC 較為保守的閉環穩態電流限制 (`IQMAX_A`)，以提供弱磁控制所需的負 $I_d$ 電流空間，對抗高轉速下的發電機定子磁場。

**目的：**
放行最高轉速極限，讓馬達在高速域有足夠的「電壓與電流籌碼」能飆上 10000 RPM，並避免滿電時觸發保護。

**修改方式 (`drive_parameters.h`)：**
1. **`IQMAX_A`**: 由原本保守的 `3A` 放寬至 **`8A`**。給予足夠的總電流預算供系統調配給弱磁 (FW) 使用，補足高轉扭力。
2. **`OV_VOLTAGE_THRESHOLD_V`**: 由預設的 `17V` 提升到 **`20V`**。因為 4S 電池充飽電壓高達 16.8V，在高速煞車時電能回灌極易飆破 17V 造成 `MC_OVER_VOLT` 錯誤導致斷電摔機，20V 更能確保 4S 系統的安全裕度。

**注意：** 此修改僅放行「極限高轉速段」的封印，至於低速起跑時（`PHASE1`/`PHASE2`）的電流依然維持在 **`2A`** 的省電溫和起步，兩者完美兼顧，並不衝突。

**日期時間：** 2026-03-27 12:28:00

---

## 2026-03-27 12:21:00 (Current Optimization Re-tuning)

### 修正馬達抖動無法轉起問題 (提升啟動電流至平衡點)

**原因：** 前次為了追求極低電流，將開環啟動（Phase 1 與 Phase 2）電流降至 `1A`。但實測發現 1A 產生的扭力不足以克服馬達轉子的慣性，導致強迫對齊不確實、以及加速期發生失步（Rotor Slip），表現為「馬達只有抖動，無法起轉」。無感測器 FOC 必須在開環階段擁有足夠轉速與反電動勢 (BEMF) 才能準確讓 STO 觀測器鎖定接手。

**目的：**
在「確保穩定起轉」與「較低電流衝擊」之間尋找一個平衡點。

**修改方式 (`drive_parameters.h`)：**
1. `PHASE1_FINAL_CURRENT_A`: 提升回 `2A`，時間縮短微調至 `400ms`（給予足夠剛性電流以確保轉子強制歸零對齊）。
2. `PHASE2_FINAL_CURRENT_A`: 提升回 `2A`，時間縮短改為 `1500ms`（確保能在 1.5 秒內產生足夠力矩將轉子穩定帶動到 1908 轉，而不至於失步抖動）。
3. `IQMAX_A`: 稍微上調至 `3A`，保留閉環時合理的加速極限扭力空間。此配置電流尖峰將會低於最一開始暴力拉升的 3A / 4A 舊版配置，同時又具備足夠的可靠啟動力矩。

**日期時間：** 2026-03-27 12:21:00

---

## 2026-03-27 12:15:00 (Current Optimization)

### 降低整體控制電流與拉長啟動斜坡 (以換取極低電流)

**原因：** 使用者反應運作電流偏高，希望能盡可能降低電流消耗。為了降低 FOC 無感測器控制的啟動衝流以及加速時的瞬間電流（Iq 用於產生扭矩，加速度越快則需求電流越大），我們需要對所有相關電流與斜率做全面封頂與放緩。

**目的：**
最小化馬達開環對齊、加速以及閉環動態響應過程中所造成的電流尖峰，確保維持在極度柔和與省電的狀態。

**修改方式：**
1. **`drive_parameters.h` (開環與最大電流修改)**:
   - `IQMAX_A`: 從 `4A` 大幅降至 `2A`，直接從全域限制閉環最大容許電流。
   - `PHASE1_FINAL_CURRENT_A` 與 `PHASE2_FINAL_CURRENT_A`: 皆從 `3A` 降為 `1A`（開環注入的強迫電流降至最低）。
   - `PHASE1_DURATION`: 從 `100ms` 延長為 `500ms`，給 1A 的微弱電流多點時間完成對齊。
   - `PHASE2_DURATION`: 從 `1200ms` 延長為 `2000ms`，減緩盲轉爬升期的機械加速度。
2. **`mc_app_hooks.c` (閉環油門跟隨修改)**:
   - 將穩定運行後的推油門目標追隨斜率 `MCI_ExecSpeedRamp` 的爬升間隔，由原本的 `500 ms` 拉長至 `3000 ms`。三秒鐘的油門緩升能大幅降低系統因突增轉速產生的 Iq 加速尖峰電流。

**日期時間：** 2026-03-27 12:15:00

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
