#include "quadstates.h"

QuadStates::QuadStates(QByteArray frameid, QByteArray addr_long, QByteArray addr_short)
{
    frame_id = frameid;
    address_long = addr_long;
    address_short = addr_short;
}
