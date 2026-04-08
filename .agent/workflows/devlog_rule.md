---
description: 所有對 ST_FOC 專案的修改都必須記錄到 devlog.md
---

# ST_FOC 修改記錄準則

每次對專案任何檔案（`.c` / `.h` / `.md` / `CMakeLists.txt` / `.json` 等）做修改後，**必須**同步更新 `d:\Work_202506\FOC\ST_FOC\ST_FOC\devlog.md`。

## 執行方式

優先使用自動化腳本：

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\update-devlog.ps1 `
  -Title "本次修改標題" `
  -Reason "為什麼要改" `
  -Purpose "這次修改想達成什麼" `
  -Methods @(
    "`檔案路徑` — 修改摘要 1",
    "`檔案路徑` — 修改摘要 2"
  )
```

如果只想重建 `devlog.md` 頂部索引行號：

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\update-devlog.ps1 -RebuildIndexOnly
```

VS Code 也提供 task：
- `Rebuild Devlog Index`

## 記錄格式

新條目會插入 `devlog.md` 第一條 `---` 之後，並自動帶入當下日期時間，格式如下：

```markdown
## YYYY-MM-DD HH:MM:SS (修改標題)

**原因：** 說明為什麼要做這個修改。

**目的：** 說明這個修改要達成什麼效果。

**修改方式：**
- `檔案路徑` — 修改內容摘要

**日期時間：** YYYY-MM-DD HH:MM:SS

---
```

## 索引規則

1. `devlog.md` 最上方必須保留 `## 索引`
2. 索引需列出每個 `##` 條目的實際行號
3. 每次新增條目或調整內容後，都要同步重建索引

## 注意事項

1. 每次修改**至少一條**記錄，不可省略
2. 若同一時間有多個相關修改，可合併成一條記錄
3. 使用繁體中文撰寫
4. 「原因」說明觸發原因，「目的」說明預期效果，「修改方式」列具體檔案與變動
5. 若修改是為了避免 CubeMX / MC Workbench 覆蓋，請在內容中明確標出「生成隔離」或「user-owned」
