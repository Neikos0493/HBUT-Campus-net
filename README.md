# HBUT 校园网自动认证

> **⚠️ 重要提醒：使用前请先查看压缩包中的教程获取密文**

基于 C++ & Win32 API 开发的 Windows 桌面应用，自动检测网络状态并完成校园网认证，无需手动打开浏览器登录。

## 功能

- **自动认证** — 首次配置账号与密码（密文）后，程序会在断网或检测到未认证状态时自动发起认证请求，无需任何人工干预。
- **开机自启** — 支持 Windows 开机自动启动，无需每次手动打开程序，确保网络始终在线。
- **状态反馈** — 通过托盘图标和日志实时显示认证状态（已连接 / 认证中 / 连接失败）。

## 环境要求

| 依赖 | 版本 |
|------|------|
| 操作系统 | Windows 10 / 11 |
| 编译器 | MSVC（Visual Studio 2022+）或 MinGW-w64 |

## 编译

```bash
# 使用 Visual Studio 开发者命令提示符
cl /EHsc src\main.cpp src\state_machine.cpp src\auth_client.cpp src\config_manager.cpp src\config_encryptor.cpp src\logger.cpp src\tray_manager.cpp src\startup_manager.cpp src\task_queue.cpp src\timer.cpp user32.lib ws2_32.lib
```

## 使用说明

### 首次配置

1. 运行程序，首次启动会弹出控制台窗口要求输入学号和密码（密文）。
2. 输入完成后配置自动保存到 `data/config.ini`。
3. 关闭控制台窗口，程序继续在后台运行（托盘可见）。

### 修改账号

右键点击系统托盘中的校园网图标 → 选择「修改账号」，按提示重新输入即可。

### 开机自启

程序首次启动时会自动注册到 Windows 启动项，无需额外操作。如需取消自启，可在任务管理器的「启动」选项卡中禁用。

## 注意事项

- 请勿将 `data/config.ini` 提交到公开仓库，其中包含加密后的密码信息。
- 认证地址（`172.16.54.18`）为 HBUT 校园网认证服务器，若学校网络环境变更，请相应修改 `src/main.cpp` 中的 `BUILTIN_URL`。

## License

GPLv3 — 详见 [LICENSE](./LICENSE) 文件。
