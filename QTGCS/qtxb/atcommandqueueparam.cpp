#include "atcommand.h"
#include "atcommandqueueparam.h"

ATCommandQueueParam::ATCommandQueueParam(QObject *parent) :
    ATCommand(parent)
{
    setFrameType(0x09);
    setFrameId(0x00);
}
