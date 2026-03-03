## 使用说明

本文件夹工程用于电赛培训，适配开发板 Penguins-G474，主控为 STM32G474VET6。

本文件夹工程均配置为超频模式，运行主频 200 MHz，由`mclk_modifier.bat`实现。

### 开发环境

|工具／组件|版本号|说明|下载链接|
|-|-|-|-|
|MDK-ARM (Keil)|v5.43a|基于 Arm MCU 的软件开发套件|[HomePage](https://www.keil.com/demo/eval/arm.htm) or [v5.43a directly](https://www.keil.com/files/eval/MDK543a.exe)|
|STM32CubeMX|v6.17.0|STM32Cube 初始化代码生成器|[HomePage](https://www.st.com.cn/zh/development-tools/stm32cubemx.html)|
|STM32CubeG4|v1.6.2|STM32G4 系列 MCU 固件包|in *STM32CubeMX* or [ST](https://www.st.com/en/embedded-software/stm32cubeg4.html) or [GitHub](https://github.com/STMicroelectronics/STM32CubeG4)|
|STM32G4xx\_DFP|v2.2.0|STM32G4 系列设备支持包|in *Pack Installer* or [Arm](https://www.keil.arm.com/packs/stm32g4xx_dfp-keil/overview/) or [GitHub](https://github.com/Open-CMSIS-Pack/STM32G4xx_DFP)|
|CH341SER|v3.9.1|USB 转串口芯片驱动|[HomePage](https://www.wch.cn/downloads/CH341SER_EXE.html)|
|VOFA+|v1.3.10|插件驱动的高自由度上位机|[HomePage](https://www.vofa.plus/downloads)|

MDK-ARM 需使用注册机 [keygen.exe](https://github.com/Penguins-lwz/NUEDC-2026/blob/main/keygen.exe) 获取许可证破解，操作办法自行查阅网络资源。

MDK-ARM v5.43a 附带组件 Pack Installer v1.4.1.1 无法在安装 DFP 包时生成 SFR 文件，进而无法在调试时使用 System Viewer 访问外设寄存器，应参考 [知识库文章 KA006065](https://developer.arm.com/documentation/ka006065/latest) 手动覆盖安装 [Pack Installer v1.4.1.3](https://documentation-service.arm.com/static/68c97736cccf2a5517018393) 解决。

MDK-ARM v5.43a 附带组件 AC6 (ARMCLANG)，而不再附带组件 AC5 (ARMCC)，如需使用可参考 [知识库文章 KA005073](https://developer.arm.com/documentation/ka005073/latest)、[CSDN 博文](https://blog.csdn.net/qcmyqcmy/article/details/125814461) 手动安装 Arm Compiler v5.06u7。
