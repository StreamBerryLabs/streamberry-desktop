#pragma once

#include <vector>
#include <cstdint>
#include <string>

class VirtualCam {
public:
    VirtualCam();
    ~VirtualCam();

    bool open(const std::string& devicePath, int width, int height);
    void close();
    bool writeFrame(const uint8_t* data, size_t size);

private:
    int m_fd;
    int m_width;
    int m_height;
    bool m_isOpen;
};
