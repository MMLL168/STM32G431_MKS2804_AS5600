# ST FOC 馬達控制通訊架構說明 (MCP over UART)

## 1. MCP 是什麼？

**MCP** 全名為 **Motor Control Protocol** (馬達控制協定)。

這是 STMicroelectronics 官方提供的一套通訊協定，專門用於從外部裝置 (例如 PC、另一塊 MCU) 透過序列通訊 (如 UART) 來監控、除錯和控制馬達。它定義了一套標準的指令和資料格式，讓您可以：

*   讀寫馬達控制演算法中的參數 (例如 PID 參數、目標轉速)。
*   啟動或停止馬達。
*   接收來自馬達控制器的即時資料 (例如目前轉速、電流、溫度)。
*   執行即時的故障診斷。

簡單來說，MCP 就是您用來與 ST 馬達控制韌體「對話」的官方語言。

## 2. 通訊分層架構

此專案中的 UART 通訊採用了清晰的分層架構，將高階的馬達控制指令一步步轉換為底層的硬體訊號。

```
+--------------------------+
|   應用層 (e.g. mc_tasks.c)  |  <-- 使用 MCP Handle (MCP_Over_UartA)
+--------------------------+
             |
             ▼
+--------------------------+
|   MCP (馬達控制協定)        |  <-- 協定層，定義指令和資料格式
+--------------------------+
             |
             ▼
+--------------------------+
| ASPEP (異步執行者協定)     |  <-- 傳輸層，負責封包、CRC、狀態管理
| (in aspep.c)             |
+--------------------------+
             |
             ▼
+--------------------------+
| UART/DMA Driver          |  <-- 驅動層，直接控制硬體 (DMA, USART)
| (in usart_aspep_driver.c)|
+--------------------------+
```

## 3. 資料發送流程 (以應用層發送指令為例)

1.  **應用層觸發**: 在 `Src/mc_tasks.c` 這類任務檔案中，當需要發送資料時，會呼叫 MCP 相關函式。它會使用 `MCP_Over_UartA` 這個 Handle。

2.  **指向 ASPEP**: `MCP_Over_UartA` 在 `Src/mcp_config.c` 中被設定，其 `pTransportLayer` (傳輸層指標) 指向了 `aspepOverUartA` Handle。

3.  **ASPEP 封包**: 程式接著會呼叫 ASPEP 層的函式，例如 `ASPEP_sendPacket()`。這一層會執行以下操作：
    *   取得一個發送緩衝區 (`ASPEP_getBuffer`)。
    *   將 MCP 的指令資料打包成 ASPEP 格式的封包。
    *   計算並添加標頭 (Header) 和 CRC 校驗碼。

4.  **硬體驅動**: ASPEP 層會呼叫底層的 UART 驅動函式，例如 `UASPEP_CFG_TRANSMISSION()`。

5.  **DMA 傳輸**: `UASPEP_CFG_TRANSMISSION` 函式會設定 DMA 控制器，將準備好的資料封包從記憶體搬移到 USART 的傳輸資料暫存器 (TDR)，最終透過 TX 引腳逐位元發送出去。

接收流程則是這個過程的逆向操作，從硬體中斷開始，逐層向上解析。

## 4. 關鍵檔案總結

*   `Src/mc_tasks.c`: **應用層**。展示了如何使用 MCP 協定來執行高階任務。
*   `Src/mcp_config.c`: **組態與連結**。將 MCP、ASPEP 和 UART Driver 這三層連結在一起的地方。
*   `Inc/aspep.h`: **ASPEP 標頭檔**。定義了 `ASPEP_Handle_t` 結構以及 `ASPEP_sendPacket` 等核心函式的原型。
*   `Src/aspep.c`: **ASPEP 實作**。UART 通訊的**主要邏輯**所在地，負責處理封包的建立、解析和協定狀態機。
*   `Inc/usart_aspep_driver.h`: **UART 驅動標頭檔**。定義了底層硬體操作函式的原型。
*   `Src/usart_aspep_driver.c`: **UART 驅動實作**。直接控制 `USART` 和 `DMA` 的底層程式碼。

## 5. MCP 指令列表 (Commands)

所有指令的定義位於 `MCSDK_v6.4.1-Full/MotorControl/MCSDK/MCLib/Any/Inc/mcp.h`。

| 指令 (Command) | 十六進制值 | 功能說明 |
| --- | --- | --- |
| `GET_MCP_VERSION` | 0x00 | 取得 MCP 協定的版本號。 |
| `SET_DATA_ELEMENT` | 0x08 | 設定一個或多個參數 (暫存器) 的值。 |
| `GET_DATA_ELEMENT` | 0x10 | 讀取一個或多個參數 (暫存器) 的值。 |
| `START_MOTOR` | 0x18 | 啟動馬達。 |
| `STOP_MOTOR` | 0x20 | 停止馬達。 |
| `STOP_RAMP` | 0x28 | 停止目前的斜坡加減速，並維持在當前速度。 |
| `START_STOP` | 0x30 | 根據馬達目前狀態，切換啟動或停止 (Toggle)。 |
| `FAULT_ACK` | 0x38 | 清除故障 (Fault) 狀態。 |
| `CPULOAD_CLEAR` | 0x40 | 清除 CPU 負載的量測值。 |
| `IQDREF_CLEAR` | 0x48 | 清除電流環 (Iq, Id) 的參考值。 |
| `PFC_ENABLE` | 0x50 | 致能 PFC (功率因數校正) (在此專案中未實作)。 |
| `PFC_DISABLE` | 0x58 | 禁能 PFC (功率因數校正) (在此專案中未實作)。 |
| `PFC_FAULT_ACK` | 0x60 | 清除 PFC 的故障狀態 (在此專案中未實作)。 |
| `PROFILER_CMD` | 0x68 | 執行與性能分析器 (Profiler) 相關的指令。 |
| `SW_RESET` | 0x78 | 對微控制器執行軟體重置 (Reboot)。 |
| `SENSOR_SWITCH` | 0x80 | 切換主回授與輔助回授感測器。 |
| `MCP_USER_CMD` | 0x100 | 使用者自定義指令的基礎 ID。 |

## 6. 封包格式與相關文件

### 6.1 整體封包結構
一個完整的 UART 封包由 **ASPEP** 協定層包裹，它像一個「貨櫃」，裡面裝載著 MCP 的「貨物」。
```
[ 4-byte ASPEP Header |   MCP Packet (N bytes)   | 2-byte ASPEP CRC (可選) ]
```
- **ASPEP Header (4 bytes)**: 包含封包類型、Payload 長度、標頭 CRC 等資訊。
- **MCP Packet (N bytes)**: 實際的指令或回覆內容。
- **ASPEP CRC (2 bytes)**: 對 MCP Packet 內容的 CRC-16 校驗。

### 6.2 指令封包 (電腦 -> 馬達板)
內部的 MCP 封包格式:
```
[ 2-byte MCP Header |  N-byte MCP Payload (可選) ]
```
- **MCP Header (2 bytes)**: 包含 16 位元的指令 ID (例如 `START_MOTOR` 是 0x18) 和目標馬達 ID。
- **MCP Payload (N bytes)**: 指令所需的額外參數 (例如 `SET_DATA_ELEMENT` 時，這裡會放暫存器 ID 和數值)。

**範例: 一個 `START_MOTOR` 指令的完整 UART 封包為:**
`[ 4B ASPEP Header | 2B MCP Header (含 0x18) | 2B ASPEP CRC ]` = **總共 8 Bytes**。

### 6.3 回覆封包 (馬達板 -> 電腦)
內部的 MCP 封包格式:
```
[ M-byte MCP Reply Data (可選) | 1-byte MCP Status Code ]
```
- **MCP Reply Data (M bytes)**: 回覆的具體資料 (例如 `GET_DATA_ELEMENT` 時，這裡會放讀取到的數值)。
- **MCP Status Code (1 byte)**: 指令執行結果，例如 `MCP_CMD_OK` (0x00) 表示成功。

**範例: 一個 `START_MOTOR` 指令的成功回覆，其完整 UART 封包為:**
`[ 4B ASPEP Header | 1B MCP Status (0x00) | 2B ASPEP CRC ]` = **總共 7 Bytes**。

### 6.4 官方參考文件
關於 ST Motor Control Protocol 的最詳細定義，請參考官方使用者手冊：
- **文件編號**: **UM2380**
- **文件名稱**: User manual - STM32 motor control SDK v5.4 tools
- **下載連結**: [https://www.st.com/resource/en/user_manual/um2380-stm32-motor-control-sdk-v54-tools-stmicroelectronics.pdf](https://www.st.com/resource/en/user_manual/um2380-stm32-motor-control-sdk-v54-tools-stmicroelectronics.pdf)
