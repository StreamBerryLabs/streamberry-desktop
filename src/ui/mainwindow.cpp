#include "mainwindow.h"
#include "../video/decoder.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

#include "mainwindow.h"
#include "../video/decoder.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent, bool startMinimized)
    : QMainWindow(parent)
{
    setupUi();
    
    // Set window icon explicitly if needed (though QApplication::setWindowIcon should handle it)
    // Try to load from local path if resource fails (for development)
    if (windowIcon().isNull()) {
        setWindowIcon(QIcon("src/ui/icon.png"));
    }

    discovery = std::make_unique<DeviceDiscovery>(this);
    streamClient = std::make_unique<StreamClient>(this);
    virtualCam = std::make_unique<VirtualCam>();
    
    // Initialize decoder (we need a persistent decoder instance, but for now we create it in the callback or here)
    // Actually, the decoder should probably be a member of MainWindow or a separate controller.
    // For simplicity, let's handle decoding in the callback.
    
    connect(discovery.get(), &DeviceDiscovery::deviceFound, this, &MainWindow::onDeviceFound);
    
    connect(streamClient.get(), &StreamClient::connected, this, &MainWindow::onStreamConnected);
    connect(streamClient.get(), &StreamClient::disconnected, this, &MainWindow::onStreamDisconnected);
    connect(streamClient.get(), &StreamClient::errorOccurred, this, &MainWindow::onStreamError);

    // Setup frame callback
    // We need a persistent decoder
    static Decoder decoder;
    if (!decoder.init()) {
        QMessageBox::critical(this, "Error", "Failed to initialize video decoder");
    }

    streamClient->setFrameCallback([this](const VideoFrame& frame) {
        // Decode frame
        std::vector<uint8_t> decoded = decoder.decode(frame.data.data(), frame.data.size());
        
        if (!decoded.empty()) {
            // Initialize virtual cam if needed (assuming 1280x720 for now, or get from decoder)
            // Note: decoder.decode() returns YUYV data which is 2 bytes per pixel.
            // We need to know the dimensions. FFmpeg context has them.
            // For POC, let's assume 1280x720 or 1920x1080 based on the stream.
            // Ideally, we should get dimensions from the decoder.
            
            // Since we don't have easy access to decoder internals here without changing the API,
            // let's assume standard HD for now or try to infer.
            // 1280 * 720 * 2 = 1843200
            // 1920 * 1080 * 2 = 4147200
            
            int width = 1280;
            int height = 720;
            
            if (decoded.size() == 1920 * 1080 * 2) {
                width = 1920;
                height = 1080;
            }
            
            // Open virtual cam if not open
            // We need to find a device. Usually /dev/video0 or /dev/video2.
            // Let's try /dev/video0 first, or ask user.
            // For automation, we can try to open /dev/video20 (common for v4l2loopback)
            
            static bool camInitialized = false;
            if (!camInitialized) {
                 // Try a few devices
                 std::vector<std::string> devices = {"/dev/video0", "/dev/video1", "/dev/video2", "/dev/video20"};
                 for (const auto& dev : devices) {
                     if (virtualCam->open(dev, width, height)) {
                         qDebug() << "Opened virtual camera at" << QString::fromStdString(dev);
                         camInitialized = true;
                         break;
                     }
                 }
                 
                 if (!camInitialized) {
                     qDebug() << "Failed to open virtual camera";
                 }
            }
            
            if (camInitialized) {
                virtualCam->writeFrame(decoded.data(), decoded.size());
            }
        }
    });

    createTrayIcon();
    
    // Auto-connect setup
    isAutoConnecting = true;
    autoConnectTimer = new QTimer(this);
    connect(autoConnectTimer, &QTimer::timeout, this, &MainWindow::checkAutoConnect);
    autoConnectTimer->start(5000); // Check every 5 seconds if not connected
    
    // Start initial scan
    onScanClicked();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    scanButton = new QPushButton("Scan for Devices", this);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::onScanClicked);
    mainLayout->addWidget(scanButton);
    
    deviceList = new QListWidget(this);
    mainLayout->addWidget(deviceList);
    
    connectButton = new QPushButton("Connect", this);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connectButton->setEnabled(false);
    mainLayout->addWidget(connectButton);
    
    disconnectButton = new QPushButton("Disconnect", this);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    disconnectButton->setEnabled(false);
    mainLayout->addWidget(disconnectButton);
    
    statusLabel = new QLabel("Ready", this);
    mainLayout->addWidget(statusLabel);
    
    connect(deviceList, &QListWidget::itemSelectionChanged, [this]() {
        connectButton->setEnabled(!deviceList->selectedItems().isEmpty());
    });
    
    resize(400, 500);
}

void MainWindow::createTrayIcon()
{
    trayMenu = new QMenu(this);
    QAction *showAction = trayMenu->addAction("Show");
    connect(showAction, &QAction::triggered, this, &QWidget::showNormal);
    
    QAction *quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);
    
    // Use application icon
    trayIcon->setIcon(windowIcon());
    if (trayIcon->icon().isNull()) {
        // Fallback
        trayIcon->setIcon(QIcon::fromTheme("camera-video"));
    }
    trayIcon->setToolTip("StreamBerry Desktop");
    
    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isVisible()) hide();
            else showNormal();
        }
    });
    
    trayIcon->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
        trayIcon->showMessage("StreamBerry", "Application minimized to tray", QSystemTrayIcon::Information, 1000);
    } else {
        event->accept();
    }
}

void MainWindow::checkAutoConnect()
{
    if (isAutoConnecting && !streamClient->findChild<QWebSocket*>() && foundDevices.empty()) {
        // If not connected and no devices found, scan again
        onScanClicked();
    }
}

void MainWindow::onScanClicked()
{
    deviceList->clear();
    foundDevices.clear();
    statusLabel->setText("Scanning...");
    discovery->startScan();
}

void MainWindow::onDeviceFound(const StreamDevice& device)
{
    foundDevices.push_back(device);
    deviceList->addItem(QString::fromStdString(device.name) + " (" + QString::fromStdString(device.url) + ")");
    statusLabel->setText("Device found: " + QString::fromStdString(device.name));
    
    // Auto-connect if enabled and not already connected
    if (isAutoConnecting) {
        // Select the item
        deviceList->setCurrentRow(foundDevices.size() - 1);
        onConnectClicked();
    }
}

void MainWindow::onConnectClicked()
{
    int row = deviceList->currentRow();
    if (row >= 0 && row < foundDevices.size()) {
        const auto& device = foundDevices[row];
        statusLabel->setText("Connecting to " + QString::fromStdString(device.name) + "...");
        streamClient->connectToDevice(QString::fromStdString(device.url));
        
        scanButton->setEnabled(false);
        connectButton->setEnabled(false);
        deviceList->setEnabled(false);
        
        // Stop auto-connect timer while connected
        autoConnectTimer->stop();
    }
}

void MainWindow::onDisconnectClicked()
{
    streamClient->disconnectFromDevice();
    // Disable auto-connect if user manually disconnects? 
    // Maybe not, let's keep it enabled but maybe pause it for a bit.
    // For now, let's say manual disconnect disables auto-connect until restart or manual connect.
    isAutoConnecting = false;
}

void MainWindow::onStreamConnected()
{
    statusLabel->setText("Connected! Streaming to virtual camera...");
    disconnectButton->setEnabled(true);
    trayIcon->setToolTip("StreamBerry: Connected");
    
    // Minimize to tray on successful auto-connect?
    // Let's just notify
    trayIcon->showMessage("StreamBerry", "Connected to camera", QSystemTrayIcon::Information, 2000);
}

void MainWindow::onStreamDisconnected()
{
    statusLabel->setText("Disconnected");
    disconnectButton->setEnabled(false);
    scanButton->setEnabled(true);
    connectButton->setEnabled(true);
    deviceList->setEnabled(true);
    trayIcon->setToolTip("StreamBerry: Disconnected");
    
    // Restart auto-connect timer
    if (isAutoConnecting) {
        autoConnectTimer->start(5000);
    }
}

void MainWindow::onStreamError(const QString& error)
{
    statusLabel->setText("Error: " + error);
    // Only show message box if window is visible, otherwise tray notification
    if (isVisible()) {
        QMessageBox::critical(this, "Stream Error", error);
    } else {
        trayIcon->showMessage("StreamBerry Error", error, QSystemTrayIcon::Critical);
    }
    onStreamDisconnected();
}
