#pragma once

#include <QObject>
#include <QWebSocket>
#include <functional>
#include <vector>
#include <cstdint>

struct VideoFrame {
    std::vector<uint8_t> data;
    bool isKeyframe;
    uint64_t timestamp;
};

class StreamClient : public QObject
{
    Q_OBJECT

public:
    explicit StreamClient(QObject *parent = nullptr);
    ~StreamClient();

    void connectToDevice(const QString& url);
    void disconnectFromDevice();
    
    using FrameCallback = std::function<void(const VideoFrame&)>;
    void setFrameCallback(FrameCallback callback);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString& error);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket m_webSocket;
    FrameCallback m_frameCallback;
    
    void handleVideoFrame(const QJsonObject& json);
    QByteArray decodeBase64(const QString& base64Str);
};
