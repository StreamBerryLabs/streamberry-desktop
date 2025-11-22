#pragma once

#include <QObject>
#include <QString>
#include <vector>
#include <string>

struct StreamDevice {
    std::string name;
    std::string ip_address;
    std::string url;
    int port;
};

class DeviceDiscovery : public QObject
{
    Q_OBJECT

public:
    explicit DeviceDiscovery(QObject *parent = nullptr);
    void startScan();

signals:
    void deviceFound(const StreamDevice& device);
};
