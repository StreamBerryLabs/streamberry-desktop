#include "virtual_cam.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring>
#include <iostream>

VirtualCam::VirtualCam()
    : m_fd(-1)
    , m_width(0)
    , m_height(0)
    , m_isOpen(false)
{
}

VirtualCam::~VirtualCam()
{
    close();
}

bool VirtualCam::open(const std::string& devicePath, int width, int height)
{
    m_fd = ::open(devicePath.c_str(), O_RDWR);
    if (m_fd < 0) {
        perror("Opening video device");
        return false;
    }

    struct v4l2_format vid_format;
    memset(&vid_format, 0, sizeof(vid_format));
    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    if (ioctl(m_fd, VIDIOC_G_FMT, &vid_format) < 0) {
        perror("VIDIOC_G_FMT");
        // Continue anyway, we will try to set it
    }

    vid_format.fmt.pix.width = width;
    vid_format.fmt.pix.height = height;
    vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    vid_format.fmt.pix.sizeimage = width * height * 2;
    vid_format.fmt.pix.field = V4L2_FIELD_NONE;
    vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    if (ioctl(m_fd, VIDIOC_S_FMT, &vid_format) < 0) {
        perror("VIDIOC_S_FMT");
        ::close(m_fd);
        return false;
    }

    m_width = width;
    m_height = height;
    m_isOpen = true;
    return true;
}

void VirtualCam::close()
{
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
    m_isOpen = false;
}

bool VirtualCam::writeFrame(const uint8_t* data, size_t size)
{
    if (!m_isOpen || m_fd < 0) return false;

    ssize_t written = ::write(m_fd, data, size);
    if (written < 0) {
        perror("Writing frame");
        return false;
    }
    return true;
}
