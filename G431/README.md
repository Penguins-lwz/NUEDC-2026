# 使用说明

本文件夹工程用于蓝桥培训，适配开发板 [CT117E-M4](https://item.taobao.com/item.htm?id=634196587834&skuId=4927684543021)，主控为 STM32G431RBT6。

本文件夹工程对标电赛培训 [G474](https://github.com/Penguins-lwz/NUEDC-2026/tree/main/G474)，已接受此培训的读者可以对照学习。

## 开发环境

|工具／组件|版本号|说明|下载链接|
|-|-|-|-|
|MDK-ARM (Keil)|v5.43a|基于 Arm MCU 的软件开发套件|[HomePage](https://www.keil.com/demo/eval/arm.htm) or [v5.43a directly](https://www.keil.com/files/eval/MDK543a.exe)|
|STM32CubeMX|v6.17.0|STM32Cube 初始化代码生成器|[HomePage](https://www.st.com.cn/zh/development-tools/stm32cubemx.html)|
|STM32CubeG4|v1.6.2|STM32G4 系列 MCU 固件包|in *STM32CubeMX* or [ST](https://www.st.com/en/embedded-software/stm32cubeg4.html) or [GitHub](https://github.com/STMicroelectronics/STM32CubeG4)|
|STM32G4xx\_DFP|v2.2.0|STM32G4 系列设备支持包|in *Pack Installer* or [Arm](https://www.keil.arm.com/packs/stm32g4xx_dfp-keil) or [GitHub](https://github.com/Open-CMSIS-Pack/STM32G4xx_DFP)|
|CH341SER|v3.9.1|USB 转串口芯片驱动|[HomePage](https://www.wch.cn/downloads/CH341SER_EXE.html)|
|VOFA+|v1.3.10|插件驱动的高自由度上位机|[HomePage](https://www.vofa.plus/downloads)|

## 注意事项

### Notice I

MDK-ARM 需使用注册机 [keygen.exe](https://github.com/Penguins-lwz/NUEDC-2026/blob/main/keygen.exe) 获取增强版 (Plus) 或专业版 (Professional) 许可证，以解除代码容量限制。

### Notice II

点击链接跳转至大赛官网获取 [电子赛资源数据包](https://dasai.lanqiao.cn/notices/1096)。

## 已知问题

### Problem I

MDK-ARM v5.43a 附带组件 Pack Installer v1.4.1.1 存在已知问题，无法在安装 DFP 时由 \*.svd 文件生成有赖于 MDK-ARM 描述设备外设寄存器的 \*.sfr 文件，导致无法在调试时使用 System Viewer 访问外设寄存器，应参考 [知识库文章 KA006065](https://developer.arm.com/documentation/ka006065/latest) 手动覆盖安装 [Pack Installer v1.4.1.3](https://documentation-service.arm.com/static/68c97736cccf2a5517018393) 解决。

### Problem II

第 17 届电子赛资源数据包的 I2C 驱动存在已知问题，停止位稳定时间过短，导致 E2PROM 连续读写偶发失败，应修改以下源文件解决（即将 `I2C_Stop` 函数末尾等待时间加长至原有 1000 倍）：

- `i2c_hal.c`
```C
void I2CStop(void)
{
    SCL_Output(0);
    delay1(DELAY_TIME);
    SDA_Output(0);
    delay1(DELAY_TIME);
    SCL_Output(1);
    delay1(DELAY_TIME);
    SDA_Output(1);
    delay1(DELAY_TIME * 1000);
}
```
