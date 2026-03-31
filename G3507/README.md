# 使用说明

本文件夹工程用于电赛培训，适配开发板 [LP-MSPM0G3507](https://www.ti.com.cn/tool/cn/LP-MSPM0G3507)，主控为 MSPM0G3507SMPR。

## 开发环境

|工具／组件|版本号|说明|下载链接|
|-|-|-|-|
|MDK-ARM (Keil)|v5.43a|基于 Arm MCU 的软件开发套件|[HomePage](https://www.keil.com/demo/eval/arm.htm) or [v5.43a directly](https://www.keil.com/files/eval/MDK543a.exe)|
|MSPM0-SDK|v2.10.00.04|MSPM0 软件开发套件|[HomePage](https://www.ti.com.cn/tool/zh-cn/download/MSPM0-SDK)|
|SysConfig|v1.27.0|TI 全系列 MCU 系统配置工具|[HomePage](https://www.ti.com.cn/tool/zh-cn/download/SYSCONFIG)|
|MSPM0G1X0X\_G3X0X\_DFP|v1.3.1|MSPM0G1X0X\_G3X0X 系列设备支持包|in *Pack Installer* or [Arm](https://www.keil.arm.com/packs/mspm0g1x0x_g3x0x_dfp-texasinstruments)|
|CH341SER|v3.9.1|USB 转串口芯片驱动|[HomePage](https://www.wch.cn/downloads/CH341SER_EXE.html)|
|VOFA+|v1.3.10|插件驱动的高自由度上位机|[HomePage](https://www.vofa.plus/downloads)|

## 注意事项

### Notice I

MDK-ARM 需使用注册机 [keygen.exe](https://github.com/Penguins-lwz/NUEDC-2026/blob/main/keygen.exe) 获取增强版 (Plus) 或专业版 (Professional) 许可证，以解除代码容量限制。

### Notice II

MDK-ARM v5.37+ 全面使用 AC6 (ARMCLANG)，如需使用 AC5 (ARMCC) 可参考 [知识库文章 KA005073](https://developer.arm.com/documentation/ka005073/latest)、[CSDN 博文](https://blog.csdn.net/qcmyqcmy/article/details/125814461) 手动安装 Arm Compiler v5.06u7。

### Notice III

MSPM0-SDK 与 SysConfig 存在依赖默认安装路径的绝对路径，为便于培训教学与使用官方例程，请保留默认安装路径：
- `C:\ti\mspm0_sdk_2_10_00_04\`
- `C:\ti\sysconfig_1.27.0\`

### Notice IV

SysConfig v1.27.0 高于 MSPM0-SDK v2.10.00.04 要求的最低版本，应修改以下配置文件（即将 `v1.26.2` 修改为 `v1.27.0`）：

- `<SDK_PATH>\tools\keil\MSPM0_SDK_syscfg_menu_import.cfg`
```
[Sysconfig v1.27.0 - MSPM0 SDK v2_10_00_04]
Command=C:\ti\sysconfig_1.27.0\nw\nw.exe C:\ti\sysconfig_1.27.0\app
```

注意，`Command` 指令路径参数追加尾缀 `\app` 的原因见 [已知问题 II](#problem-ii)。
 
- `<SDK_PATH>\tools\keil\syscfg.bat`
```
set SYSCFG_PATH="C:\ti\sysconfig_1.27.0\sysconfig_cli.bat"
```

## 已知问题

### Problem I

MDK-ARM v5.43a 附带组件 Pack Installer v1.4.1.1 存在已知问题，无法在安装 DFP 时由 \*.svd 文件生成有赖于 MDK-ARM 描述设备外设寄存器的 \*.sfr 文件，导致无法在调试时使用 System Viewer 访问外设寄存器，应参考 [知识库文章 KA006065](https://developer.arm.com/documentation/ka006065/latest) 手动覆盖安装 [Pack Installer v1.4.1.3](https://documentation-service.arm.com/static/68c97736cccf2a5517018393) 解决。

### Problem II

MSPM0-SDK v2.09.00.01+ 存在已知问题，未就 SysConfig v1.26.0+ 对 `package.json` 文件存放路径调整作对应更新，导致无法在 MDK-ARM 导入 SysConfig，应修改以下配置文件解决（即将 `Command` 指令路径参数追加尾缀 `\app`）：

- `<SDK_PATH>\tools\keil\MSPM0_SDK_syscfg_menu_import.cfg`
```
Command=C:\ti\sysconfig_1.27.0\nw\nw.exe C:\ti\sysconfig_1.27.0\app
```

此问题已提交至 TI 客户支持中心并要求发布至 [E2E 论坛](https://e2e.ti.com/support/microcontrollers/msp-low-power-microcontrollers-group/msp430/f/msp-low-power-microcontroller-forum/1628616/mspm0c1103-bug-report-mspm0-sdk-v2-09-sysconfig-import-path-issue-with-mdk-arm-keil-fix-attached)，但被错误添加 MSPM0C1103 标签。

### Problem III

MSPM0-SDK v1.00.00.04+ 存在已知问题，开发板 [LP-MSPM0G3507](https://www.ti.com.cn/tool/cn/LP-MSPM0G3507) 的 BSL 按键 PA18 在已有外部下拉电阻的条件下，SysConfig 应用 Launch Pad Shortcut 时被错误配置使能内部上拉电阻，导致不能正确读取数字电平，应主动调整为不使能内部上下拉电阻解决。
