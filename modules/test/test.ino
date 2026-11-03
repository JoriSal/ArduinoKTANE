#include <Module.h>

Module module(10);

void setup()
{
    module.init();
}

void loop()
{
    module.update();
}