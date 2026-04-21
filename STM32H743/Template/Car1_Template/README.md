## 改动说明

### 文件结构变动

- 增加了专门用于处理各种传入数据的库 `SensorProc.c` 位于`\Drivers\SensorProc`
- 将`kinematics.c` 与 `tracking.c`放在同一目录下

### 函数变动

- `void MEInit(Motor *L, Motor *R)` 变为 `void MECInit(void)` 即完全将motor处理封装在主程序之外
- 获取灰度传感器数据的函数封装到`void UpdateData`内，也就是无需再单独调用