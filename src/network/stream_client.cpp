#include "stream_client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

StreamClient::StreamClient(QObject *parent) : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &StreamClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &StreamClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &StreamClient::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::errorOccurred, this, &StreamClient::onError);
}

StreamClient::~StreamClient()
{
    m_webSocket.close();
}

void StreamClient::connectToDevice(const QString& url)
{
    m_webSocket.open(QUrl(url));
}

void StreamClient::disconnectFromDevice()
{
    m_webSocket.close();
}

bool StreamClient::isConnected() const
{
    return m_webSocket.state() == QAbstractSocket::ConnectedState;
}

void StreamClient::setFrameCallback(FrameCallback callback)
{
    m_frameCallback = callback;
}

void StreamClient::onConnected()
{
    qDebug() << "WebSocket connected";
    emit connected();
}

void StreamClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
    emit disconnected();
}

void StreamClient::onTextMessageReceived(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject json = doc.object();
    QString type = json["type"].toString();

    if (type == "video_frame") {
        handleVideoFrame(json);
    }
}

void StreamClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket error:" << error << m_webSocket.errorString();
    emit errorOccurred(m_webSocket.errorString());
}

void StreamClient::handleVideoFrame(const QJsonObject& json)
{
    if (!m_frameCallback) return;

    QString base64Data = json["data"].toString();
    QByteArray decoded = decodeBase64(base64Data);
    
    if (decoded.isEmpty()) return;

    VideoFrame frame;
    frame.data.resize(decoded.size());
    memcpy(frame.data.data(), decoded.data(), decoded.size());
    
    frame.isKeyframe = json["keyframe"].toBool();
    frame.timestamp = json["timestamp"].toVariant().toULongLong();

    m_frameCallback(frame);
}

QByteArray StreamClient::decodeBase64(const QString& base64Str)
{
    return QByteArray::fromBase64(base64Str.toUtf8());
}
