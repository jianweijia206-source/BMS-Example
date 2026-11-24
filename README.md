# BMS-Example (Keil uVision 5)

这是一个面向 Keil C51 / 8051 的简易电池管理系统示例工程，包含单体电压、总电流和温度数据结构、基于电压的 SOC 粗略估算，以及过压、欠压、过流和过温保护判断。

## 打开与编译

1. 使用 Keil uVision 5 打开 `BMS-Example.uvproj`。
2. 工程目标为 `BMS-8051`（Generic 8051，12 MHz）。
3. 选择 **Project → Build Target** 编译。

编译输出在 `Objects` 和 `Listings`。如需用于实际硬件，请在 `ReadSensors()` 中接入 ADC、电流采样和温度传感器，并按具体芯片型号调整工程目标和启动配置。当前 `main()` 只运行一次演示采样，保护阈值也仅用于示例，不可直接作为真实电池的安全控制参数。
