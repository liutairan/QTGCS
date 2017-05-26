#ifndef RXINDICATOREXPLICIT_H
#define RXINDICATOREXPLICIT_H
#include "digimeshpacket.h"
#include <QByteArray>


class RXIndicatorExplicit : public DigiMeshPacket
{

public:
    explicit RXIndicatorExplicit(QObject *parent = 0);
    void readPacket(QByteArray rx);
    
signals:
    
public slots:
    
};

#endif // RXINDICATOREXPLICIT_H
