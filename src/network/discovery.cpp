#include "discovery.h"
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QEventLoop>
#include <QTimer>
#include <iostream>

DeviceDiscovery::DeviceDiscovery(QObject *parent) : QObject(parent)
{
}

void DeviceDiscovery::startScan()
{
    // Simple subnet scan for now (similar to plugin)
    // In a real app, we'd use mDNS properly, but for this POC we'll scan common IPs
    // or just listen for broadcasts if the app supported it.
    // The plugin uses active probing.

    // We will run this in a separate thread or just use async Qt networking
    // For simplicity, let's just probe a few common local IPs and the localhost
    
    std::vector<std::string> ips_to_scan;
    
    // Add localhost
    ips_to_scan.push_back("127.0.0.1");
    
    // Get local IP and scan subnet
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : list) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && !address.isLoopback()) {
            QString ip = address.toString();
            QString subnet = ip.section('.', 0, 2); // Get 192.168.1
            
            // Scan .1 to .20
            for (int i = 1; i <= 20; i++) {
                ips_to_scan.push_back((subnet + "." + QString::number(i)).toStdString());
            }
        }
    }

    // Async probe
    for (const auto& ip : ips_to_scan) {
        QString qip = QString::fromStdString(ip);
        QTcpSocket* socket = new QTcpSocket(this);
        
        connect(socket, &QTcpSocket::connected, [this, socket, qip]() {
            StreamDevice device;
            device.ip_address = qip.toStdString();
            device.port = 8080;
            device.name = "StreamBerry Device (" + device.ip_address + ")";
            device.url = "ws://" + device.ip_address + ":8080/stream";
            
            emit deviceFound(device);
            socket->disconnectFromHost();
            socket->deleteLater();
        });
        
        connect(socket, &QTcpSocket::errorOccurred, [socket]() {
            socket->deleteLater();
        });

        socket->connectToHost(qip, 8080);
    }
}
