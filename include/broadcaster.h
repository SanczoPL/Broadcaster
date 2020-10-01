#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QVector>
#include <stdio.h>

#include <mqt.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

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
  void onNewMessage(const QByteArray a_message);
  void onSendCommand(const qint32 topic, const QJsonObject json);
  void onConnected();
  void onDisconnected();

signals:
  void subscribeRequest(QVector<qint32> const a_topics);
  void unsubscribeRequest(QVector<qint32> const a_topics);
  void sendMessageRequest(QByteArray const a_message);
  void newMessage(QJsonObject const &a_json);
  void connected();
  void disconnected();

private:
  QString m_ip{};
  quint16 m_port{};
};
#endif // BROADCASTER_H
