#include <Module.h>

uint16_t test;

void setup()
{
    Module.init(MODULE, 0, 1);
    Module.sync(test, GetRequest::BATTERIES);
}

void loop()
{
    Module.update();
}