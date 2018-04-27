#include "alertnotificationservice.h"

const char* AlertNotificationService::UUID_SERVICE_ALERT_NOTIFICATION  = "{00001811-0000-1000-8000-00805f9b34fb}";
const char* AlertNotificationService::UUID_CHARACTERISTIC_ALERT_NOTIFICATION_NEW_ALERT = "{00002a46-0000-1000-8000-00805f9b34fb}";
const char* AlertNotificationService::UUID_CHARACTERISTIC_ALERT_NOTIFICATION_CONTROL = "{00002a44-0000-1000-8000-00805f9b34fb}";

AlertNotificationService::AlertNotificationService(QObject *parent) : BipService(UUID_SERVICE_ALERT_NOTIFICATION, parent)
{

}

void AlertNotificationService::sendAlert(const QString &sender, const QString &subject, const QString &message)
{
    qDebug() << "Alert:" << sender << subject << message;

    if (message.isEmpty()) {
        return;
    }

    int category = 0xfa; //Custom Huami icon
    int icon = mapSenderToIcon(sender);

    if (sender == "Messages") { //SMS must use category, not icon
        category = AlertCategory::SMS;
    }

    if (icon == HuamiIcon::EMAIL) { //Email icon doesnt work, so use category
        category = AlertCategory::Email;
    }

    QByteArray send = QByteArray(1, category) + QByteArray(1, 1); //1 alert

    send += QByteArray(1, mapSenderToIcon(sender));
    send += sender.left(32).toUtf8() + QByteArray(1, 0x00); //Null char indicates end of first line

    if (!subject.isEmpty()) {
        send += subject.left(128).toUtf8() + QByteArray(2, 0x0a);
    }

    if (!message.isEmpty()) {
        send += message.left(128).toUtf8();
    }

    send.truncate(230); //!TODO is 230 is the max?
    service()->writeCharacteristic(service()->characteristic(QBluetoothUuid(QString(UUID_CHARACTERISTIC_ALERT_NOTIFICATION_NEW_ALERT))), send, QLowEnergyService::WriteWithResponse);
}

void AlertNotificationService::incomingCall(const QString &caller)
{
    QByteArray send = QByteArray::fromHex("0301");
    send += caller.toUtf8();
    service()->writeCharacteristic(service()->characteristic(QBluetoothUuid(QString(UUID_CHARACTERISTIC_ALERT_NOTIFICATION_NEW_ALERT))), send, QLowEnergyService::WriteWithResponse);
}

int AlertNotificationService::mapSenderToIcon(const QString &sender)
{
    QString s = sender.toLower();

    QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailREX.setCaseSensitivity(Qt::CaseInsensitive);
    mailREX.setPatternSyntax(QRegExp::RegExp);

    if (mailREX.exactMatch(s)) {
            return HuamiIcon::EMAIL;
    }
    if (s == "facebook") {
        return HuamiIcon::FACEBOOK;
    }
    if (s == "twitter" || s == "tweetian") {
        return HuamiIcon::TWITTER;
    }
    if (s == "messenger") {
        return HuamiIcon::FACEBOOK_MESSENGER;
    }
    if (s == "snapchat") {
        return HuamiIcon::SNAPCHAT;
    }
    if (s == "whatsapp") {
        return HuamiIcon::WHATSAPP;
    }
    if (s == "instagram") {
        return HuamiIcon::INSTAGRAM;
    }
    if (s == "telegram" || s == "sailorgram") {
        return HuamiIcon::TELEGRAM;
    }
    if (s == "skype") {
        return HuamiIcon::SKYPE;
    }

    return HuamiIcon::APP_11;
}