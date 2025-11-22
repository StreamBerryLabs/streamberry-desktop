# StreamBerry Desktop Client

![StreamBerry Logo](src/ui/icon.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-blue)](https://www.linux.org/)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-blue)](https://www.microsoft.com/windows)
[![Qt 6](https://img.shields.io/badge/Qt-6.0%2B-green)](https://www.qt.io/)

StreamBerry Desktop is a high-performance C++ application that transforms your mobile device into a premium wireless webcam for your desktop. Connect seamlessly via Wi-Fi and use your phone's camera in Zoom, Discord, OBS, Teams, and more.

## ✨ Features

-   **🚀 Zero Latency**: Built with C++ and Qt 6 for ultra-low latency streaming.
-   **🎥 Virtual Camera**: Native integration with Linux (`v4l2loopback`) and Windows (DirectShow).
-   **⚡ Auto-Connect**: Automatically finds and connects to your phone when it comes online.
-   **👻 Background Mode**: Runs silently in the system tray.
-   **🔋 Idle Mode**: Minimal resource usage when not streaming.
-   **🛠️ Hardware Decoding**: Efficient H.264 decoding using FFmpeg.

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
Download the latest `.deb` release from the [Releases](https://github.com/streamberry/streamberry-desktop/releases) page.
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

**2. Build from Source (AUR package coming soon)**
```bash
git clone https://github.com/streamberry/streamberry-desktop.git
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
git clone https://github.com/streamberry/streamberry-desktop.git
cd streamberry-desktop
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
*Note: A pre-built `.exe` installer will be available in the Releases section soon.*

---

## 🚀 Usage

1.  **Launch**: Open **StreamBerry Desktop** from your app menu.
2.  **Connect**:
    -   Ensure your phone is running the **StreamBerry App**.
    -   Connect both devices to the **same Wi-Fi network**.
    -   The app will **auto-connect**. If not, click **Scan** and select your device.
3.  **Stream**:
    -   Open your video app (Zoom, Discord, etc.).
    -   Select **"StreamBerry Camera"** (or "Dummy Video Device") as your webcam.

### Command Line Options

| Option | Description |
| :--- | :--- |
| `--minimized` | Start the application hidden in the system tray. |
| `--help` | Show help message. |

Example:
```bash
# Add this to your startup applications to have it ready in the background
streamberry-desktop --minimized
```

---

## 🔧 Troubleshooting

**"Failed to open virtual camera"**
-   Ensure `v4l2loopback` is loaded: `lsmod | grep v4l2loopback`
-   Try reloading it:
    ```bash
    sudo modprobe -r v4l2loopback
    sudo modprobe v4l2loopback exclusive_caps=1
    ```

**"No devices found"**
-   Check if your firewall is blocking port `8080` (WebSocket) or `5353` (mDNS).
-   Ensure both devices are on the same subnet.

**Video Lag / Artifacts**
-   Use a 5GHz Wi-Fi network for best results.
-   Lower the resolution in the mobile app if the network is congested.

---

## 👨‍💻 Development

### Build Requirements
-   **C++17** compliant compiler
-   **Qt 6.4+** (Core, Network, WebSockets, Widgets, Multimedia)
-   **FFmpeg 4.x+** (libavcodec, libavformat, libavutil, libswscale)
-   **CMake 3.16+**

### Building
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 📄 License

Copyright (c) 2025 StreamBerry Labs. Licensed under the [MIT License](LICENSE).
