#ifndef NODEIDENTIFICATIONINDICATOR_H
#define NODEIDENTIFICATIONINDICATOR_H
#include "digimeshpacket.h"
#include <QByteArray>

class NodeIdentificationIndicator : public DigiMeshPacket
{

public:
    explicit NodeIdentificationIndicator(QObject *parent = 0);
    void readPacket(QByteArray rx);
    
signals:
    
public slots:
    
};

#endif // NODEIDENTIFICATIONINDICATOR_H
