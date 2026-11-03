#include <packet.h>
#include <Common.h>

void setup()
{
    CommonSerialBegin();
}

void loop()
{
    sendPacket(0xB3, 0xABCD);
}