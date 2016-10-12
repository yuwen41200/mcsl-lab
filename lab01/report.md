# 實驗一 實驗環境建立與 Debugger 操作 #

## 實驗目的 ##

* 測試實驗器材
* 熟悉開發環境
* 讓我們這門課不會被當

## 實驗步驟 ##

### 專案建立與程式編譯 ###

### 變數宣告與記憶體觀察 ###

1. 變數 X 與 str 的初始值是由誰在何處初始化的？  
   由誰：微控制器啟動（或 reset）時執行的程式。  
   在何處：`startup/startup_stm32.s` 中的 `LoopCopyDataInit` 與 `CopyDataInit` 段落。

2. 若將 X 宣告改在 text section 對其程式執行結果會有何改變？  
   `X` 的位址會改變（從 data section 移至 text section）。  
   因為 text section 是唯讀的，所以第 18 行（將值寫入 `X` 的位址）無效。

3. 程式執行完畢後 r2 內容與 str 字串在 memory 前 4 個 byte 呈現內容有何差異？  
   `r2` 的值是 `0x6c6c6548`，也就是 "lleH"。  
   `str` 的前四個位元組是 `0x48656c6c`，也就是 "Hell"。（所以這個微控制器是 little-endian）

4. 變數 str "Hello World!" 有無其他種宣告方式？若有請說明其中一種。  
   有，因為變數的記憶體位址是連續的，所以可以將第 6 行改為：  
   ```
   str: .word 0x6c6c6548  @ "lleH"
   str1: .word 0x6f57206f @ "oW o"
   str2: .word 0x21646c72 @ "!dlr"
   str3: .byte 0x00       @ "\0"
   ```

### 簡易算數與基本記憶體指令操作 ###
