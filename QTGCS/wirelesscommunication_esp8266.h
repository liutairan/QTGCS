#ifndef WIRELESSCOMMUNICATION_ESP8266_H
#define WIRELESSCOMMUNICATION_ESP8266_H

#include <QObject>

class WirelessCommunication_ESP8266 : public QObject
{
    Q_OBJECT
public:
    explicit WirelessCommunication_ESP8266(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WIRELESSCOMMUNICATION_ESP8266_H