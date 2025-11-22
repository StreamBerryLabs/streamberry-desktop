#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QCloseEvent>
#include <memory>
#include "network/discovery.h"
#include "network/stream_client.h"
#include "video/virtual_cam.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, bool startMinimized = false);
    ~MainWindow();

private slots:
    void onScanClicked();
    void onConnectClicked();
    void onDisconnectClicked();
    void onDeviceFound(const StreamDevice& device);
    void onStreamError(const QString& error);
    void onStreamConnected();
    void onStreamDisconnected();

private:
    void setupUi();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QPushButton *scanButton;
    QListWidget *deviceList;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QLabel *statusLabel;

    std::unique_ptr<DeviceDiscovery> discovery;
    std::unique_ptr<StreamClient> streamClient;
    std::unique_ptr<VirtualCam> virtualCam;
    
    std::vector<StreamDevice> foundDevices;

    // Tray and Auto-connect
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QTimer *autoConnectTimer;
    bool isAutoConnecting;

    void createTrayIcon();
    void checkAutoConnect();

protected:
    void closeEvent(QCloseEvent *event) override;
};
