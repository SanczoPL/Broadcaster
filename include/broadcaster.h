#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QVector>
#include <stdio.h>

#include "../../QtCommunicatorPlugin/include/mqt.h"
#include "../../src/structures.h"

class Broadcaster : public QObject {
  Q_OBJECT

public:
  Broadcaster(QJsonObject const &a_config);
  ~Broadcaster();

private:
  MQt m_IO{};

public slots:
  void configure(const QJsonObject &a_config);
  void onConnect();
  void onSubscribe(const qint32 topic);
  void onUnsubscribe(const qint32 topic);
  void onSubscribe(QVector<qint32> topics);
  void onUnsubscribe(QVector<qint32> topics);
  void onNewMessage(const QByteArray a_message);
  void onSendCommand(const qint32 topic, const QJsonObject json);
  void onSendImage(const qint32 topic, QByteArray image);
  void onSendPing(const qint32 idSender, const qint32 topic);
  void onConnected();
  void onDisconnected();

signals:
  void subscribeRequest(QVector<qint32> const a_topics);
  void unsubscribeRequest(QVector<qint32> const a_topics);
  void sendMessageRequest(QByteArray const a_message);
  void newMessage(QJsonObject const &a_json);
  void connected();
  void disconnected();
  void updateImage(QByteArray image);

private:
  QString m_ip{};
  quint16 m_port{};
};
#endif // BROADCASTER_H
