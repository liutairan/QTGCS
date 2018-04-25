#ifndef TXREQUESTEXPLICIT_H
#define TXREQUESTEXPLICIT_H

#include "txrequest.h"
#include <QByteArray>

class TXRequestExplicit : public TXRequest
{

public:
    explicit TXRequestExplicit(QObject *parent = 0);
    QByteArray sourceEndpoint;
    QByteArray destinationEndpoint;
    QByteArray clusterID;
    QByteArray profileID;


    
};

#endif // TXREQUESTEXPLICIT_H
