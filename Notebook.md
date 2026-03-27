# ST_FOC 專案分析筆記

本文檔旨在分析此基於 ST Motor Control SDK (MCSDK) 的磁場導向控制 (FOC) 專案的軟體架構、執行流程與核心功能。

---

## 1. 專案高層架構 (High-Level Architecture)

本專案的軟體架構可以分為三個主要層次，由下至上分別是：硬體抽象層 (HAL)、馬達控制中介層 (MCSDK)，以及應用邏輯層 (Application)。

```
+------------------------------------------------+
|               應用邏輯層 (Application)          |
| - mc_app_hooks.c (旋鈕角度控制狀態機)         |
| - main.c (初始化與主迴圈)                    |
+------------------------------------------------+
| ▲            (MCI API, Hooks)            ▲ |
+------------------------------------------------+
|            馬達控制中介層 (ST MCSDK)            |
| - mc_tasks.c (高/中/低頻任務排程)            |
| - mc_interface.c (MCI API 實現)            |
| - FOC, STO, PWMC... (核心演算法模組)         |
+------------------------------------------------+
| ▲      (HAL, LL Drivers, Interrupts)     ▲ |
+------------------------------------------------+
|            硬體抽象層 (STM32 HAL/LL)          |
| - stm32g4xx_hal_...c (週邊驅動)              |
| - stm32g4xx_it.c (中斷服務)                |
+------------------------------------------------+
|               硬體 (STM32G431 MCU)             |
+------------------------------------------------+
```

### 1.1 硬體抽象層 (Hardware Abstraction Layer - HAL)

-   **功能**：由 STM32CubeMX 自動產生，提供一組標準化的 API 來操作 STM32G4 微控制器的底層硬體週邊，如 ADC, TIM, CORDIC, OPAMP, GPIO, UART 等。
-   **核心檔案**：`stm32g4xx_hal_*.c`, `stm32g4xx_ll_*.c`, `system_stm32g4xx.c`。
-   **與上層互動**：為 MCSDK 提供存取硬體資源的標準介面。例如，PWM 模組 (`PWMC`) 會透過 HAL/LL 函式來設定計時器 (TIM) 的暫存器。

### 1.2 馬達控制中介層 (ST Motor Control SDK - MCSDK)

-   **功能**：此為專案的核心，是一個高度模組化的馬達控制函式庫。它實現了 FOC 所需的複雜演算法，並將其封裝成易於使用的元件 (Component) 和 API。
-   **核心模組**:
    -   **FOC (Field-Oriented Control)**: 包含 Clarke/Park 變換、PI 控制器等，負責電流環的閉迴路控制。
    -   **STO (State Observer)**: 狀態觀測器，在此專案中使用 `STO-PLL` (鎖相迴路) 來估測無感測器馬達的轉子角度與速度。
    -   **PWMC (PWM Controller)**: PWM 產生與電流採樣模組，負責驅動逆變器並與 ADC 同步。
    -   **MCI (Motor Control Interface)**: 提供給上層應用程式的統一 API 介面，用以命令馬達啟動 (`MCI_StartMotor`)、停止 (`MCI_StopMotor`)、設定轉速或轉矩等。
    -   **Tasks**: 將控制邏輯劃分為高、中、低頻任務，透過 `SysTick` 中斷進行排程。
-   **核心檔案**：`mc_tasks.c`, `mc_interface.c`, `foc_*.c`, `sto_pll_*.c`, `pwm_curr_fdbk.c` 等。

### 1.3 應用邏輯層 (Application Layer)

-   **功能**：根據產品的具體需求，呼叫 MCSDK 提供的 API 來實現特定的馬達行為。
-   **核心檔案**:
    -   `main.c`: 負責系統初始化，包含時脈、GPIO、以及呼叫 `MX_MotorControl_Init()` 來初始化 MCSDK。
    -   `mc_app_hooks.c`: **本專案客製化邏輯的核心**。利用 MCSDK 提供的「鉤子函式 (Hooks)」，在 FOC 任務執行的特定時間點插入自訂程式碼。此專案的旋鈕角度控制狀態機便是在 `MC_APP_PostMediumFrequencyHook_M1()` 中實現的。
    -   `mc_tasks.c` (USER CODE 部分): 在高頻任務 `TSK_HighFrequencyTask` 中，將 `mc_app_hooks.c` 計算出的目標角度 `g_angSweep` 注入 FOC 演算法。

---

## 2. 主要運行流程 (Main Execution Flow)

本專案並不像傳統的 C 語言程式在 `main()` 函式的 `while(1)` 迴圈中執行主要邏輯。它是一個完全由**中斷驅動 (Interrupt-Driven)** 的系統。

1.  **啟動 (Boot)**
    -   程式從 `startup_stm32g431xx.s` 的 `Reset_Handler` 開始。
    -   跳轉至 `system_stm32g4xx.c` 的 `SystemInit()`，設定基礎時脈。
    -   跳轉至 `main.c` 的 `main()` 函式。

2.  **初始化 (Initialization)**
    -   在 `main()` 函式中，依序呼叫 `HAL_Init()`、`SystemClock_Config()` 及各週邊的 `MX_*_Init()` 函式。
    -   最重要的呼叫是 `MX_MotorControl_Init()`，它會觸發 `mc_boot()` (`mc_tasks.c`)，對 MCSDK 內部所有模組 (FOC, STO, PI控制器, ...) 進行初始化。
    -   `main()` 函式的最後進入一個**空的 `while(1)` 迴圈**。此時系統的所有設定都已完成，程式開始等待中斷觸發。

3.  **SysTick 中斷與任務排程**
    -   `HAL_Init()` 會設定 `SysTick` 計時器每 1ms 觸發一次中斷。
    -   每次 `SysTick` 中斷發生時，會執行 `SysTick_Handler()` (位於 `stm32g4xx_it.c`)。
    -   `SysTick_Handler()` 會呼叫 `HAL_IncTick()` 和 `MC_RunMotorControlTasks()` (`mc_tasks.c`)。
    -   `MC_RunMotorControlTasks()` 是**中頻任務 (1kHz) 的排程器**。它會呼叫 `TSK_MediumFrequencyTaskM1()` 來執行速度迴圈、狀態機轉換等。
    -   `TSK_MediumFrequencyTaskM1()` 執行完畢後，會呼叫 `MC_APP_PostMediumFrequencyHook_M1()`，也就是您實作旋鈕控制邏輯的地方。

4.  **PWM 中斷與高頻任務**
    -   驅動馬達的 `TIM1` 計時器被設定為以 25kHz 頻率觸發更新事件 (Update Event)。
    -   每次更新事件會觸發 `TIM1_UP_TIM16_IRQHandler()` 中斷 (位於 `stm32g4xx_mc_it.c`)。
    -   此中斷處理函式會呼叫 `TSK_HighFrequencyTask()` (`mc_tasks.c`)。
    -   `TSK_HighFrequencyTask()` 是**高頻任務 (25kHz) 的核心**，它負責執行整個 FOC 電流環的計算。其內部流程大致為：
        1.  (您的客製化程式碼) 判斷是否為 RUN 狀態，若是則將 `g_angSweep` 覆寫至 `STO_PLL_M1._Super.hElAngle`。
        2.  呼叫 `FOC_HighFrequencyTask()`，其內部會執行：
            a. 讀取 ADC 電流採樣值。
            b. 執行 STO 觀測器，估算角度與速度。
            c. 執行 PI 控制器，計算 Vq, Vd 電壓。
            d. 執行反 Park、Clarke 變換與 SVM (空間向量調變)，計算出下一個 PWM 週期的三相責任週期。
            e. 更新計時器 `TIM1` 的比較暫存器。

**總結：**
整個系統就像兩個齒輪。大齒輪 (`SysTick`) 每 1ms 轉動一格，負責更新狀態、目標等宏觀任務。小齒輪 (PWM 中斷) 每 40µs (25kHz) 就轉動一格，負責執行最核心、最即時的 FOC 電流控制。您的應用程式，就是透過 `Hooks` 在這兩個齒輪轉動的間隙中，插入自己的邏輯。

---

## 3. 核心檔案分析 (Core File Analysis)

本節將深入分析幾個與系統運行最相關的核心檔案。

### 3.1 `main.c`

-   **檔案功能**：程式主進入點，負責完成所有硬體週邊與軟體模組的初始化，並在完成後進入一個空的無限迴圈，將系統的控制權完全交給中斷。

-   **核心函式**：
    -   `int main(void)`:
        -   呼叫 `HAL_Init()`: 初始化 Flash、設定 Systick 計時器 (1ms)。
        -   呼叫 `SystemClock_Config()`: 設定系統主時脈為 170MHz。
        -   呼叫 `MX_..._Init()`: 由 CubeMX 產生，初始化所有用到的硬體週邊 (ADC, TIM, CORDIC, COMP, OPAMP, ...)。
        -   呼叫 `MX_MotorControl_Init()`: **啟動 MCSDK 的關鍵**。此函式會進一步呼叫 `MCboot()` 來初始化所有馬達控制相關的軟體元件。
        -   進入 `while(1)`: 一個空的迴圈。程式執行到這裡，代表所有初始化都已完成，系統正在等待 `SysTick` 或其他中斷來驅動後續行為。

### 3.2 `stm32_mc_common_it.c` & `stm32g4xx_mc_it.c` (中斷服務常式)

-   **檔案功能**：系統的中樞神經，定義了各個硬體中斷發生時，CPU 應該執行的對應程式碼 (ISR, Interrupt Service Routine)。它們是整個中斷驅動架構的「引擎」。

-   **核心函式**：
    -   `SysTick_Handler()` (位於 `..._common_it.c`):
        -   **中頻任務的觸發點**，由 Systick 計時器每 1ms 固定呼叫。
        -   其核心任務是呼叫 `MC_RunMotorControlTasks()`，從而驅動 1kHz 的中頻任務排程器。
    -   `ADC1_2_IRQHandler()` (位於 `...g4xx_mc_it.c`):
        -   **高頻任務的觸發點**。在此專案中，PWM 計時器 (TIM1) 的更新事件會觸發 ADC 開始進行電流採樣，當 ADC 採樣完成後，就會觸發此中斷。
        -   其中斷服務常式非常簡潔：直接呼叫 `TSK_HighFrequencyTask()`，執行 FOC 核心運算。這種緊密的時序確保了電流控制的即時性。
    -   `TIMx_BRK_M1_IRQHandler()` (位於 `...g4xx_mc_it.c`):
        -   硬體故障處理函式，用於處理來自比較器 (Comparator) 的**過電流 (OCP)** 或**過電壓 (OVP)** 硬體保護信號。
        -   當偵測到 Brake 事件時，會立刻呼叫 `PWMC_SwitchOffPWM()` 強制關閉 PWM 輸出，保護硬體。
    -   `HardFault_Handler()` (位於 `..._common_it.c`):
        -   處理 CPU 內核的嚴重錯誤 (如記憶體存取錯誤)。
        -   它會呼叫 `TSK_HardwareFaultTask()` 來安全地關閉馬達，然後進入死迴圈，防止錯誤擴散。

### 3.3 `mc_tasks.c` (馬達控制任務)

-   **檔案功能**：定義並實現了 MCSDK 的高、中頻任務，並包含了任務排程的邏輯。

-   **核心函式**：
    -   `MCboot()`: 在 `main()` 中被呼叫，負責初始化所有 MCSDK 元件，如 FOC、STO、PID、VBUS 感測器等。
    -   `MC_RunMotorControlTasks()`: 中頻任務的**排程器**。它被 `SysTick_Handler` 每 1ms 呼叫一次，內部透過一個計數器來決定是否要執行 `TSK_MediumFrequencyTaskM1`。預設情況下，每執行一次 `MC_RunMotorControlTasks` 就會執行一次中頻任務。
    -   `TSK_MediumFrequencyTaskM1()`: **中頻任務本體 (1kHz)**。負責處理相對較慢的邏輯，例如：
        -   執行 MCSDK 的主狀態機 (`MCI_RunMotorControl`)，處理 `IDLE`, `START`, `RUN`, `STOP` 等狀態轉換。
        -   執行速度環 PI 控制器 (在此專案中因使用轉矩/角度控制而未直接使用)。
        -   檢查軟體錯誤 (如欠壓、過溫等)。
        -   在函式結尾，呼叫 `MC_APP_PostMediumFrequencyHook_M1()`，讓使用者可以掛接自己的應用程式碼。
    -   `TSK_HighFrequencyTask()`: **高頻任務本體 (25kHz)**。負責執行對即時性要求最高的 FOC 電流環。
        -   **`USER CODE` 區塊**：在此專案中，您加入了關鍵的客製化程式碼，判斷當馬達在 `RUN` 狀態時，將 `g_angSweep` (由中頻任務計算出的目標角度) **覆寫**到狀態觀測器 `STO_PLL_M1` 的內部角度變數 `hElAngle`。這是實現開迴路角度控制的**核心技巧**。
        -   呼叫 `FOC_HighFrequencyTask()`，執行 FOC 核心計算。

### 3.4 `mc_app_hooks.c` (應用程式鉤子 & 客製化邏輯)

-   **檔案功能**：**所有客製化應用邏輯的大本營**。此檔案利用 MCSDK 預留的「鉤子 (Hook)」函式，將自訂功能注入到 MCSDK 的執行流程中，而無需修改 SDK 原始碼。

-   **核心變數與函式**：
    -   `AngCtrlState_t`: 您自定義的旋鈕角度控制狀態機，包含 `ANG_IDLE`, `ANG_MOVING`, `ANG_HOLDING` 三個狀態。
    -   `g_angCtrlState`, `g_angSweep`: 與 `mc_tasks.c` 共用的全域變數。`g_angCtrlState` 用於在高頻任務中判斷是否覆寫角度，`g_angSweep` 則是傳遞命令角度的載體。
    -   `MC_APP_PostMediumFrequencyHook_M1()`: **客製化邏輯的主體**。此函式在 `TSK_MediumFrequencyTaskM1` 結尾被呼叫 (頻率 1kHz)，內部實作了完整的狀態機邏輯：
        -   讀取 ADC 旋鈕值並進行濾波。
        -   判斷旋鈕是否移動 (`potMoved`)。
        -   根據 `g_angCtrlState` 目前的狀態，執行對應的邏輯：
            -   `case ANG_IDLE`: 若旋鈕移動，則啟動馬達 (`MC_StartMotor1()`) 並切換到 `ANG_MOVING` 狀態。
            -   `case ANG_MOVING`: 處理 `RUN` 狀態的邏輯，包含**首次進入的同步**與後續的**角度推進與速率衰減**。若發生故障，則進行處理。若馬達停止，則嘗試重啟。
            -   `case ANG_HOLDING`: 判斷旋鈕是否再次移動，若移動則回到 `ANG_MOVING`。若靜止超過 `HOLD_TIMEOUT_MS`，則停止馬達 (`MC_StopMotor1()`) 並回到 `ANG_IDLE`。
    -   `PotToElecAngle32()`: 一個靜態輔助函式，負責將濾波後的 ADC 值，根據極對數 `POLE_PAIR_NUM`，線性映射到 32-bit 的絕對電氣角度。

---

## 4. FOC 運算流程概觀 (FOC Calculation Flow Overview)

磁場導向控制 (Field-Oriented Control) 的核心思想是，透過座標轉換，將三相交流的定子電流，解耦成類似直流馬達的兩個獨立分量：勵磁電流 (d軸) 和轉矩電流 (q軸)，從而實現對馬達轉矩的精確、快速控制。

以下是 MCSDK 在 `FOC_HighFrequencyTask()` 中執行的主要運算步驟：

1.  **電流採樣 (Current Sensing)**
    -   **目的**：取得馬達三相電流的瞬時值。
    -   **實作**：由 `PWMC` (PWM Controller) 模組負責。通常使用兩個或三個分流電阻 (Shunt Resistor) 進行採樣。`R3_2_GetPhaseCurrents()` (位於 `pwm_curr_fdbk.c`) 函式會讀取 ADC 轉換結果並回傳三相電流值 (Ia, Ib, Ic)。

2.  **Clarke 變換 (Stationary Frame Transformation)**
    -   **目的**：將三相靜止座標系 (Ia, Ib, Ic) 下的電流量，轉換成兩相靜止座標系 (α-β) 下的電流量 (Iα, Iβ)。
    -   **實作**：`FOC_Clarke()` 函式 (位於 `foc.c`)。

3.  **Park 變換 (Rotating Frame Transformation)**
    -   **目的**：將兩相靜止座標系 (α-β) 的電流量，根據目前估測到的轉子電氣角度 (θe)，轉換到與轉子同步旋轉的兩相旋轉座標系 (d-q) 下，得到直流分量 Id (勵磁電流) 和 Iq (轉矩電流)。
    -   **實作**：`FOC_Park()` 函式 (位於 `foc.c`)。它會使用 CORDIC 硬體加速器來進行 sin/cos 計算。

4.  **PI 控制器 (PI Regulators)**
    -   **目的**：將實際的 Id, Iq 電流與目標值 (Id_ref, Iq_ref) 進行比較，透過 PI (比例-積分) 控制器計算出需要施加的電壓 Vd, Vq，以消除誤差。
    -   **實作**：`PI_Controller()` 函式 (位於 `pi_regulator.c`)。FOC 模組內部會分別為 d 軸和 q 軸實例化兩個 PI 控制器。`Id_ref` 通常設為 0 (對於永磁同步馬達)，`Iq_ref` 則由上層的轉矩命令 (例如您的 `MOVE_IQ`) 決定。

5.  **反 Park 變換 (Inverse Park Transformation)**
    -   **目的**：將 PI 控制器輸出的 d-q 座標系電壓 (Vd, Vq)，再根據轉子角度 (θe)，轉換回兩相靜止座標系 (α-β)，得到電壓指令 (Vα, Vβ)。
    -   **實作**：`FOC_RevPark()` 函式 (位於 `foc.c`)。

6.  **空間向量調變 (Space Vector Modulation - SVM)**
    -   **目的**：根據兩相靜止電壓指令 (Vα, Vβ)，計算出三相逆變器 (Inverter) 中六個功率管 (MOSFET/IGBT) 在下一個 PWM 週期中的開關時間 (責任週期)。SVM 是一種能最大化利用母線電壓的 PWM 生成演算法。
    -   **實作**：`PWMC_SetPhaseVoltage()` 函式 (位於 `pwm_curr_fdbk.c`)。此函式會計算出三個 PWM 通道的比較值，並更新 `TIM1` 計時器的 `CCR` 暫存器，從而在下一個 PWM 週期生成對應的電壓。

---

## 5. 馬達參數詳解 (Motor Parameters Explanation)

更換馬達是馬達控制開發中的常見需求。由於每顆馬達的機械結構、電氣特性和物理慣性都不同，因此必須在軟體中對應調整參數，FOC 演算法才能正確驅動馬達。

**最佳實踐**: 最安全、最推薦的方式是使用 ST Motor Control Workbench 圖形化工具來輸入新馬達的規格書 (Datasheet) 參數，然後重新產生專案。Workbench 會自動計算並配置好大部分底層參數。

如果您需要手動修改，以下是關鍵參數的詳細說明：

### 5.1 馬達固有參數 (Motor-Specific Electrical/Mechanical Parameters)

這些參數直接描述馬達本身的物理和電氣特性，是 FOC 控制的基礎。它們通常可以在馬達的規格書中找到。

**檔案**: `Inc/pmsm_motor_parameters.h`

-   `POLE_PAIR_NUM`
    -   **意義**: 馬達的**極對數**。它決定了機械角度與電氣角度的轉換關係 (電氣角度 = 機械角度 × 極對數)。這是 FOC 中最關鍵的參數之一，如果設定錯誤，馬達將完全無法正常轉動。
    -   **如何取得**: 通常是馬達規格書中最先標示的參數之一。可以透過計算馬達上的磁鐵數量來判斷 (磁鐵數 / 2 = 極對數)。

-   `RS`
    -   **意義**: 定子相電阻 (Stator Resistance)，單位是歐姆 (ohm)。此參數用於 FOC 的電壓補償，尤其是在低速時，電阻上的壓降佔比不可忽略。
    -   **如何取得**: 規格書會提供線電阻 (Line-to-Line Resistance)，相電阻約為線電阻的一半。也可以使用 LCR 電橋直接測量。

-   `LS`
    -   **意義**: 定子相電感 (Stator Inductance)，單位是亨利 (H)。此參數是 FOC 電流環 PI 控制器增益計算的基礎，直接影響電流環的動態響應。對於 I-PMSM (內藏式永磁馬達)，通常需要分別設定 d 軸和 q 軸電感 (Ld, Lq)。在此專案中，被簡化為單一 `LS` (即 Lq)。
    -   **如何取得**: 規格書提供，或使用 LCR 電橋在特定頻率下 (如 1kHz) 測量。

-   `MOTOR_VOLTAGE_CONSTANT` (即 `Ke`)
    -   **意義**: 反電動勢常數 (Back-EMF Constant)，單位是 `Vrms (ph-ph) / kRPM`。它描述了馬達在作為發電機時，轉速與反電動勢之間的關係。此參數對於無感測器演算法 (如此專案中的狀態觀測器 STO) 至關重要，觀測器透過比較估測的反電動勢與實際電壓來修正角度和速度的估測值。
    -   **如何取得**: 規格書會提供。也可以透過示波器測量馬達在被外部動力帶動旋轉時的線電壓來估算。

-   `NOMINAL_CURRENT_A`
    -   **意義**: 馬達的額定電流，單位是安培 (A)。此值主要用於設定 PI 控制器的輸出飽和限制，確保給定的目標轉矩不會超出馬達的承受範圍。

### 5.2 控制迴路與調校參數 (Control Loop & Tuning Parameters)

這些參數用於設定 FOC 控制演算法的內部運作方式和 PI 控制器的增益，需要根據馬達的特性和應用的動態需求進行調校 (Tuning)。

**檔案**: `Inc/drive_parameters.h`

-   `PWM_FREQUENCY`
    -   **意義**: PWM 開關頻率，單位是赫茲 (Hz)。較高的頻率可以帶來更平滑的電流、更低的轉矩漣波和噪音，但同時會增加功率管的開關損耗。此專案設定為 25kHz。
    -   **如何選擇**: 需要在性能和效率之間取捨，通常取決於功率管的性能和馬達的電感。電感越小的馬達通常需要越高的 PWM 頻率。

-   `PID_TORQUE_KP_DEFAULT`, `PID_TORQUE_KI_DEFAULT`
    -   **意義**: 電流環 (d-q 軸) PI 控制器的**比例 (Kp)** 和**積分 (Ki)** 增益。這兩個參數是 FOC 控制的靈魂，直接決定了電流環的響應速度和穩定性。
    -   **如何調校**: 這是一個專業的調校過程。通常先設定一個較小的 Kp，然後逐漸增大直到電流響應變快且不過衝；接著再加入 Ki 來消除靜態誤差。ST Motor Workbench 提供了自動調校的工具，手動調整需要豐富的經驗和示波器 (或 ST Motor Profiler) 輔助。

-   `PID_SPEED_KP_DEFAULT`, `PID_SPEED_KI_DEFAULT`
    -   **意義**: 速度環 PI 控制器的增益。它們負責根據目標速度和實際速度的誤差來產生轉矩命令 (Iq_ref)。
    -   **如何調校**: 類似電流環，但速度環的響應通常比電流環慢得多 (頻率 1kHz vs 25kHz)。

-   `PLL_KP_GAIN`, `PLL_KI_GAIN`
    -   **意義**: 狀態觀測器中鎖相迴路 (PLL) 的 PI 控制器增益。它們決定了觀測器追蹤轉子角度的速度和精度。
    -   **如何調校**: 如果馬達在啟動或變速時容易失步 (lose track)，通常需要調整這兩個參數。這也需要專業的調校。

### 5.3 應用層相關參數 (Application-Specific Parameters)

這些參數與特定的應用邏輯緊密相關。

**檔案**: `Src/mc_app_hooks.c`

-   `MOVE_IQ`, `HOLD_IQ`
    -   **意義**: 您在應用中定義的「移動電流」和「保持電流」。它們是 q 軸電流的目標值，直接對應到馬達輸出的轉矩 (轉矩 ≈ Iq × Kt，Kt 為轉矩常數)。
    -   **如何選擇**: 更換馬達後，如果新馬達的轉矩常數 (Torque Constant, Kt) 不同，或者您希望輸出更大/更小的力，就需要調整這兩個值。它們需要被轉換成 FOC 演算法使用的內部格式，即乘以 `CURRENT_CONV_FACTOR`。

-   `STEP_PER_TICK`
    -   **意義**: 在您的開迴路角度控制中，馬達每 1ms (一個中頻任務週期) 推進的電氣角度步長。
    -   **如何選擇**: 這個值決定了馬達的「掃描」速度。如果新馬達的慣性 (Inertia) 較大，或者負載較重，可能需要減小這個值以避免失步。反之，如果希望馬達轉得更快，可以適當增大此值。

-   `ARRIVAL_THRESH`, `HOLD_TIMEOUT_MS`
    -   **意義**: 「到達門檻」和「保持逾時」。這些都屬於應用層的行為定義，與馬達本身特性關係不大，但可能需要根據新馬達的響應特性進行微調。

---

## 6. 應用延伸：三軸避震雲台研究計畫

從現有的單軸 FOC 驅動基礎，發展為一個完整的三軸避震雲台 (3-Axis Gimbal)，是一個典型的 FOC 進階應用。其核心是利用 FOC 提供的高精度、高動態轉矩控制能力，來即時抵銷外部的震動與擾動。

以下是一份實現此目標的詳細研究計畫，分為三個主要階段。

### 階段一：硬體選型與整合 (Hardware Selection & Integration)

此階段的目標是備齊並整合雲台系統所需的關鍵硬體。

-   **1.1. IMU (慣性測量單元) 感測器**
    -   **研究**:
        -   **目的**: 即時感測雲台(相機)的姿態 (俯仰 Pitch, 滾轉 Roll, 偏航 Yaw)，作為穩定控制的目標。
        -   **方案比較**:
            -   **MPU-6050**: 經典、便宜的六軸 (三軸加速度+三軸陀螺儀) 感測器，但需要 MCU 自行進行姿態融合演算法。
            -   **BNO055**: 內建 32-bit Cortex-M0 核心，可直接在晶片內完成九軸數據融合 (加速度+陀螺儀+磁力計)，並透過 I2C 輸出穩定的四元數 (Quaternion) 或歐拉角 (Euler Angle)。**對於快速開發原型，此為推薦方案**，可大幅簡化軟體複雜度。
            -   **ICM-20948**: 較新的九軸感測器，性能更佳，但同樣需要外部進行姿態融合。
    -   **任務**:
        1.  選定 IMU 型號 (建議 BNO055)。
        2.  設計 I2C/SPI 硬體連接電路。
        3.  在 `main.c` 中新增 I2C 週邊初始化。
        4.  撰寫或移植 IMU 的驅動程式，目標是能以至少 100Hz 的頻率讀取到姿態角度。

-   **1.2. 雲台專用無刷馬達**
    -   **研究**:
        -   **目的**: 雲台需要的是在低速、甚至靜止時提供穩定、平順、高精度的力矩。
        -   **特性**: 雲台馬達通常是**外轉式 (Outrunner)**、**高極對數** (如 7-14對)、**高電阻**、**低 KV 值**的設計，以實現低速下的大扭力。
    -   **任務**:
        1.  評估目前使用的馬達是否符合雲台需求。若不符合，需採購雲台專用馬達。
        2.  將新馬達的參數 (極對數、電阻、電感、BEMF常數等) 填入 Motor Control Workbench 並重新產生參數檔 (`pmsm_motor_parameters.h`)。
        3.  為每個軸 (Pitch, Roll, Yaw) 準備一個 B-G431B-ESC1 驅動板。

-   **1.3. 馬達位置感測器：研究與決策 (關鍵抉擇)**
    -   **問題**: 雲台的三個軸是否需要位置感測器 (如磁編碼器)？
    -   **結論**: **不是絕對必要，但強烈建議使用**。這是決定雲台最終性能的關鍵。

    -   **方案一：無感測器控制 (Sensorless)**
        -   **原理**: 依賴 STO (狀態觀測器) 偵測馬達旋轉時的**反電動勢 (Back-EMF)** 來估算位置。
        -   **優點**: 成本極低，接線簡單。
        -   **致命缺點**: 雲台的核心工作場景是**低速**與**靜止**。此時反電動勢極度微弱，STO 會失去精度，導致：
            -   **保持力矩不足**: 雲台感覺「軟」，無法有力抵抗擾動。
            -   **位置漂移**: 無法精確鎖定一點。
            -   **震動或異音**: 控制器在不穩定的估測下，可能導致馬達抖動。

    -   **方案二：有感測器控制 (使用磁編碼器)**
        -   **原理**: 透過 AS5600 等磁編碼器**直接測量**馬達軸的絕對角度。
        -   **優點**:
            -   **全速域精確定位**: 即使在**完全靜止**時，也能提供極度精確的角度回饋。
            -   **強大的保持力矩**: 控制器能獲得精確的位置誤差，從而命令 FOC 輸出精準的反向力矩來對抗擾動，使雲台能「硬朗」地鎖定姿態。
            -   **無漂移、高精度**：控制穩定性的根本保證。
        -   **缺點**: 增加額外的硬體成本和結構設計複雜度。

    -   **決策建議**:
        打個比方：無感測器雲台像閉眼站立的人，能大致站穩但易被推倒；有感測器雲台則像睜眼站立的人，能瞬間感知失衡並立即校正。為了達到消費級產品的穩定效果，**強烈建議為每個軸都配備磁編碼器**。

    -   **任務**:
        1.  選定磁編碼器型號 (如 AS5600)。
        2.  設計磁鐵與感測器晶片的安裝結構。
        3.  透過 ST Motor Workbench，將馬達的主感測器由「無感測器」改為「編碼器」，並設定對應參數，重新產生專案。

### 階段二：單軸姿態閉迴路控制 (Single-Axis Closed-Loop Attitude Control)

此階段先專注於單一個軸 (例如 Roll 軸)，實現最核心的穩定功能。

-   **2.1. 姿態數據讀取與濾波**
    -   **研究**: 如果未使用 BNO055 這類內建融合的 IMU，則需研究姿態融合演算法。
        -   **方案**: **Mahony** 或 **Madgwick** 濾波器是針對低成本 MCU 的高效開源演算法。它們輸入原始的加速度和陀螺儀數據，輸出場時間穩定的姿態角。
    -   **任務**:
        1.  建立一個新的 `gimbal_control.c` 檔案來存放雲台相關邏輯。
        2.  在中頻任務 `MC_APP_PostMediumFrequencyHook_M1` 中，以 1kHz 的頻率執行以下操作：
        3.  呼叫 IMU 驅動函式，讀取姿態數據。
        4.  (若需要) 執行姿態融合演算法。
        5.  得到一個代表「平台目前姿態」的精確角度 (例如 `current_platform_angle`)。

-   **2.2. 位置 PID 串級控制**
    -   **研究**: 雲台控制是一個典型的**位置閉迴路**。我們需要一個位置控制器，它的目標是讓 `current_platform_angle` 永遠保持在一個設定值 (例如 0 度)。
    -   **架構**: 可以在原有的 FOC 控制基礎上，再往外套一層「位置環」。這構成了一個串級 PID 系統：
        -   **外環 (位置環)**: `Input = (目標角度 - 平台目前角度)`, `Output = 目標轉矩 (Iq_ref)`。此環路在中頻 (1kHz) 任務中執行。
        -   **內環 (電流環)**: FOC 本身就是電流環，負責忠實地執行外環給定的轉矩命令。此環路在高頻 (25kHz) 任務中執行。
    -   **任務**:
        1.  在 `gimbal_control.c` 中，實作一個 PID 控制器 (`gimbal_pid`)。
        2.  修改 `mc_app_hooks.c`，完全取代現有的「開迴路旋鈕控制」邏輯。
        3.  在 `MC_APP_PostMediumFrequencyHook_M1` 中：
            a. 讀取平台姿態角 `current_platform_angle`。
            b. 設定目標角度 `target_angle` (初始可設為 0)。
            c. 計算誤差 `error = target_angle - current_platform_angle`。
            d. 將誤差輸入 `gimbal_pid`，得到輸出 `torque_command`。
            e. 呼叫 MCSDK 的 API `MCI_ExecTorqueRamp(pMCI[M1], torque_command, 0)`，將 PID 計算出的轉矩命令傳給 FOC 底層去執行。

### 階段三：三軸協調與功能擴展 (3-Axis Coordination & Feature Expansion)

-   **3.1. 多軸控制架構**
    -   **研究**: 如何在一個 MCU 中同時管理三個獨立的 FOC 控制實例。
        -   **方案**: 最穩定可靠的方式是引入一個**即時操作系統 (RTOS)**，如 FreeRTOS。為每個軸建立一個獨立的任務，各自管理自己的 IMU 數據、PID 計算和馬達命令。
        -   若不用 RTOS，則需要擴展現有的 `mc_tasks.c`，將其從單馬達架構改為能依序輪詢處理三個馬達實例的架構，這會相對複雜。
    -   **任務**: 移植 FreeRTOS 到專案中，並建立多任務管理架構。

-   **3.2. 使用者控制模式**
    -   **研究**: 典型消費級雲台的使用者控制模式。
        -   **鎖定模式 (Lock Mode)**: 三個軸的目標角度都固定不變，相機永遠指向空間中的一個固定方向。
        -   **跟隨模式 (Follow Mode)**: Yaw 軸和 Pitch 軸的目標角度會平滑地跟隨雲台手柄的轉動，而 Roll 軸保持水平。
    -   **任務**: 設計狀態機來管理不同的操作模式，並透過 UART 或藍芽等通訊方式接收使用者輸入，以切換模式或微調目標角度。

### 階段四：調校與優化 (Tuning & Optimization)

-   **任務**:
    -   **PID 調校**: 這是雲台性能的關鍵。需要為每個軸、不同模式下的位置環 PID 控制器，精心調整 Kp, Ki, Kd 參數，以達到最佳的穩定效果 (快速響應且無震盪)。
    -   **濾波器調校**: 調整 IMU 姿態融合演算法的參數，在靈敏度和穩定性之間取得平衡。
    -   **功耗優化**: 在馬達靜止時，適當降低保持電流以節省電力。
