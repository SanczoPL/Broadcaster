#include "../include/broadcaster.h"

constexpr auto IP{ "Ip" };
constexpr auto PORT{ "Port" };
constexpr auto PID{ "Pid" };
constexpr auto ID{ "Id" };
constexpr auto COMMAND{ "Command" };
constexpr auto PING{ "Ping" };
constexpr auto MESSAGE_TYPE{ "MessageType" };
constexpr auto TIME{ "Time" };

auto constexpr MY_ID{ 3 };

Broadcaster::Broadcaster(QJsonObject const& a_config) {
	spdlog::info("Broadcaster::Broadcaster()");
	configure(a_config);
}

Broadcaster::~Broadcaster() {}

void Broadcaster::configure(QJsonObject const& a_config) {
	spdlog::trace("Broadcaster::configure()");
	QObject::connect(this, &Broadcaster::subscribeRequest, &m_IO,
		&MQt::onSubscribe);
	QObject::connect(this, &Broadcaster::unsubscribeRequest, &m_IO,
		&MQt::onUnsubscribe);
	QObject::connect(&m_IO, &MQt::newMessage, this, &Broadcaster::onNewMessage);
	QObject::connect(this, &Broadcaster::sendMessageRequest, &m_IO,
		&MQt::onSendMessage);
	QObject::connect(&m_IO, &MQt::connected, this, &Broadcaster::onConnected);
	QObject::connect(&m_IO, &MQt::disconnected, this,
		&Broadcaster::onDisconnected);

	m_ip = (a_config[IP].toString());
	m_port = (static_cast<quint16>(a_config[PORT].toInt()));
	onConnect();
	spdlog::info("Broadcaster::configure() ip:{} port:{}", m_ip.toStdString(),
		QString::number(m_port).toStdString());
}

void Broadcaster::onConnect() { m_IO.startConnection(m_ip, m_port); }

void Broadcaster::onSubscribe(qint32 topic) {
	Logger->trace("Broadcaster::onSubscribe() topic:{}", topic);
	QVector<qint32> topics{};
	topics.push_back(topic);
	emit(subscribeRequest(topics));
}

void Broadcaster::onSubscribe(QVector<qint32> topics) {
	emit(subscribeRequest(topics));
}

void Broadcaster::onUnsubscribe(qint32 topic) {
	Logger->trace("Broadcaster::onUnsubscribe() topic:{}", topic);
	QVector<qint32> topics{};
	topics.push_back(topic);
	emit(unsubscribeRequest(topics));
}

void Broadcaster::onUnsubscribe(QVector<qint32> topics) {
	emit(unsubscribeRequest(topics));
}

void Broadcaster::onNewMessage(QByteArray const a_message) {
	Logger->warn("Broadcaster::onNewMessage(QByteArray const a_message)");
	Message message{};
	message.parse(a_message);
	QJsonObject jOut{ {"none", "none"} };
	bool ret = message.parse(a_message);
	if (ret <= 0) {
		Logger->warn("Broadcaster::onNewMessage() msg not correct");
	}
	else {
		Message::Header m_header = message.header();
		Logger->trace("Broadcaster::onNewMessage() a_message.size():{} m_header.size:{}", a_message.size() - 20, m_header.size);
		Logger->trace("Broadcaster::onNewMessage() message.type() :{} ", message.type());

		
		if (message.type() == Message::BINARY)
		{
			Logger->warn("Recived message that is UniMessage::BINARY or UniMessage::JSON");
			Logger->warn("else if (message.topic() == 3)");
			QByteArray data = message.content();
			emit(updateImage(data));
		}
		if (message.type() == Message::JSON)
		{
			Logger->warn("Recived msg that is  UniMessage::JSON");
			if (message.isValid()) {
				auto jDoc{ QJsonDocument::fromJson(message.content()) };
				if (!jDoc.isObject()) {
					Logger->trace("Broadcaster::onNewMessage() Recived invalid Message");
				}
				jOut = jDoc.object()[COMMAND].toObject();
				if(jOut[MESSAGE_TYPE].toString() == PING)
				{
					onPing(jOut);
				}
				emit(newMessage(jOut));
				QByteArray stateData{ QJsonDocument{jOut}.toJson(QJsonDocument::Compact) };
				Logger->trace("Broadcaster::onNewMessage from:{} \n{}", message.topic(), stateData.toStdString().c_str());
			}
		}
	}
}

void Broadcaster::onSendCommand(const qint32 topic, const QJsonObject json) {
	Message msg{};
	QByteArray stateData{ QJsonDocument{json}.toJson(QJsonDocument::Compact) };
	msg.fromData(stateData, Message::JSON, MY_ID, topic);
	Logger->trace("Broadcaster::onSendCommand to:{} \n{}", topic, stateData.toStdString().c_str());
	emit(sendMessageRequest(msg.rawData()));
}

void Broadcaster::onSendImage(const qint32 topic, QByteArray image) {
	Message msg{};
	msg.fromData(image, Message::BINARY, MY_ID, topic);
	Logger->trace("Broadcaster::onSendImage to:{}", topic);
	emit(sendMessageRequest(msg.rawData()));
}

void Broadcaster::onSendPing(const qint32 idSender, const qint32 topic) {
	quint64 _time = QDateTime::currentMSecsSinceEpoch();
	QJsonObject json = { {MESSAGE_TYPE, PING}, {TIME, QString::number(_time)} , {ID, idSender} };
	QJsonObject cmd = { {COMMAND, json} };
	Message msg{};
	QByteArray stateData{ QJsonDocument{cmd}.toJson(QJsonDocument::Compact) };
	msg.fromData(stateData, Message::JSON, MY_ID, topic);
	Logger->trace("Broadcaster::onSendPing() to:{}", topic);
	emit(sendMessageRequest(msg.rawData()));
}

void Broadcaster::onPing(QJsonObject ping) {
	qint32 topic = ping[ID].toInt();
	QJsonObject cmd = { {COMMAND, ping} };
	Message msg{};
	QByteArray stateData{ QJsonDocument{cmd}.toJson(QJsonDocument::Compact) };
	msg.fromData(stateData, Message::JSON, MY_ID, topic);
	Logger->trace("Broadcaster::onPing() to:{}", topic);
	emit(sendMessageRequest(msg.rawData()));
}

void Broadcaster::onConnected() { emit(connected()); }

void Broadcaster::onDisconnected() { emit(disconnected()); }
