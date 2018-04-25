#ifndef DIGIMESHPACKET_H
#define DIGIMESHPACKET_H
#include <QByteArray>
#include <QObject>


class DigiMeshPacket : public QObject
{
    Q_OBJECT
    unsigned startDelimiter;
    unsigned length;
    unsigned frameType;
    unsigned frameId;
    unsigned checksum;
public:
    explicit DigiMeshPacket(QObject *parent = 0);
    QByteArray getPacket();
    void setStartDelimiter(unsigned sd);
    void setLength(unsigned l);
    void setFrameType(unsigned type);
    void setFrameId(unsigned id);
    void setChecksum(unsigned cs);
    unsigned getStartDelimiter();
    QByteArray getLength();
    unsigned getFrameType();
    unsigned getFrameId();
    unsigned getChecksum();
    void createChecksum(QByteArray array);
    void assemblePacket();
    QByteArray packet;



};

#endif // DIGIMESHPACKET_H
