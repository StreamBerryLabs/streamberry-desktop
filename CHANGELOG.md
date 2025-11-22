# Release Notes

## [1.0.0] - 2025-11-22

### 🎉 Initial Release
StreamBerry Desktop v1.0.0 is the first official release of the desktop client for StreamBerry-Cam. It brings seamless wireless webcam capabilities to Linux and Windows desktops.

### ✨ New Features
-   **Virtual Camera Support**:
    -   Native integration with `v4l2loopback` on Linux.
    -   Creates a virtual video device accessible by Zoom, Discord, OBS, etc.
-   **Auto-Connect**:
    -   Automatically scans the local network for StreamBerry devices.
    -   Connects to the first available device upon launch.
-   **System Tray Integration**:
    -   Minimizes to the system tray to save taskbar space.
    -   Background execution support.
-   **Command Line Options**:
    -   `--minimized`: Launch the application directly to the system tray.
-   **Low Latency Streaming**:
    -   Optimized C++ implementation using Qt 6 and FFmpeg.
    -   Hardware-accelerated H.264 decoding.

### 📦 Installation
-   **Linux (.deb)**:
    -   Debian/Ubuntu installer package included.
    -   Installs desktop entry and icons automatically.
-   **Source Build**:
    -   CMake build system for easy compilation on Linux and Windows.

### 🔧 Known Issues
-   Requires `v4l2loopback` to be installed manually on Linux (see README).
-   Audio streaming is not yet supported in this version.
