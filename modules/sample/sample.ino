#include <Module.h>

uint16_t test;

void setup()
{
    Module.init(MODULE, 0, 1);
    Module.sync(test, GetMaster::BATTERIES);
}

void loop()
{
    Module.update();
    if (test == 60)
    {
        digitalWrite(3, 1);
        test = 0;
        delay(1000);
    }
    digitalWrite(3, 0);
}