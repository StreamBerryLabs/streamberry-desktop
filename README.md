<div align="center">

<img src="src/ui/icon.png" alt="StreamBerry Desktop Logo" width="200"/>

# 🖥️ StreamBerry Desktop

### The Official Desktop Client for StreamBerry-Cam

[![Version](https://img.shields.io/badge/version-1.0.0-blue?style=for-the-badge&logo=linux&logoColor=white)](https://github.com/StreamBerryLabs/streamberry-desktop)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-blue?style=for-the-badge&logo=linux)](https://www.linux.org/)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-blue?style=for-the-badge&logo=windows)](https://www.microsoft.com/windows)
[![Qt 6](https://img.shields.io/badge/Qt-6.0%2B-green?style=for-the-badge&logo=qt&logoColor=white)](https://www.qt.io/)

**StreamBerry Desktop** is the perfect companion for the [StreamBerry-Cam Android App](https://github.com/StreamBerryLabs/StreamBerry-Cam). It seamlessly connects to your mobile device and creates a virtual camera on your desktop, allowing you to use your phone's high-quality camera in Zoom, Discord, OBS, and more.

[🚀 Quick Start](#-quick-start) •
[📦 Download](#-installation) •
[✨ Features](#-features) •
[🐛 Report Bug](https://github.com/StreamBerryLabs/streamberry-desktop/issues)

</div>

---

## ✨ Features

<div align="center">

| 🎯 Feature | 📝 Description |
|:-----------|:---------------|
| 🚀 **Zero Latency** | Native C++ implementation for ultra-low latency streaming |
| 🎥 **Virtual Camera** | Creates a native `/dev/video` device (Linux) or DirectShow filter (Windows) |
| ⚡ **Auto-Connect** | Automatically discovers and connects to your phone on the network |
| 👻 **Background Mode** | Runs silently in the system tray, ready when you are |
| 🔋 **Idle Mode** | Minimal resource usage when not streaming |
| 🛠️ **Hardware Decoding** | Efficient H.264 decoding using FFmpeg and hardware acceleration |

</div>

---

## 📥 Installation

### 🐧 Linux (Ubuntu / Debian / Mint)

**1. Install Dependencies**
You need `v4l2loopback` to create the virtual camera device.
```bash
sudo apt update
sudo apt install v4l2loopback-dkms
sudo modprobe v4l2loopback exclusive_caps=1 video_nr=0 card_label="StreamBerry Camera"
```

**2. Install StreamBerry**
Download the latest `.deb` release from the [Releases](https://github.com/StreamBerryLabs/streamberry-desktop/releases) page.
```bash
sudo dpkg -i streamberry-desktop-1.0.0-Linux.deb
sudo apt-get install -f  # Fix any missing dependencies
```

### 🏹 Linux (Arch / Manjaro)

**1. Install Dependencies**
```bash
sudo pacman -S v4l2loopback-dkms linux-headers qt6-base qt6-websockets qt6-multimedia ffmpeg
sudo modprobe v4l2loopback exclusive_caps=1 video_nr=0 card_label="StreamBerry Camera"
```

**2. Build from Source**
```bash
git clone https://github.com/StreamBerryLabs/streamberry-desktop.git
cd streamberry-desktop
mkdir build && cd build
cmake ..
make
sudo make install
```

### 🪟 Windows (10/11)

**1. Prerequisites**
-   Install [Qt 6](https://www.qt.io/download) (Open Source).
-   Install [CMake](https://cmake.org/download/).
-   Install a C++ Compiler (MSVC 2019+ or MinGW).

**2. Build and Install**
```powershell
git clone https://github.com/StreamBerryLabs/streamberry-desktop.git
cd streamberry-desktop
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## 🚀 Quick Start

1.  **📱 Prepare Android App**: Open **StreamBerry-Cam** on your phone and connect to WiFi.
2.  **🖥️ Launch Desktop App**: Open **StreamBerry Desktop** on your computer.
3.  **🔗 Auto-Connect**: The app will automatically find your phone and connect.
    -   *Manual Connect*: If needed, click "Scan" and select your device.
4.  **🎥 Start Using**: Open Zoom/Discord/OBS and select **"StreamBerry Camera"** as your video source.

### Command Line Options

| Option | Description |
| :--- | :--- |
| `--minimized` | Start the application hidden in the system tray. |
| `--help` | Show help message. |

Example:
```bash
# Add this to your startup applications
streamberry-desktop --minimized
```

---

## 🔧 Troubleshooting

<details>
<summary>❌ <b>"Failed to open virtual camera"</b></summary>

-   Ensure `v4l2loopback` is loaded: `lsmod | grep v4l2loopback`
-   Try reloading it with correct options:
    ```bash
    sudo modprobe -r v4l2loopback
    sudo modprobe v4l2loopback exclusive_caps=1
    ```
</details>

<details>
<summary>🔍 <b>"No devices found"</b></summary>

-   Check if your firewall is blocking port `8080` (WebSocket) or `5353` (mDNS).
-   Ensure both devices are on the same subnet.
</details>

---

## 📄 License

Copyright (c) 2025 StreamBerry Labs. Licensed under the [MIT License](LICENSE).

<div align="center">

<sub>Built with 🖥️ C++ • 🎨 Qt 6 • 🎥 FFmpeg • ⚡ Real-time Streaming</sub>

</div>
