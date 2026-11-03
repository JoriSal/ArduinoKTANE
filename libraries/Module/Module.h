#include <packet.h>
#include <Common.h>

class Module
{
public:
    Module(uint16_t moduleId) : ModuleID(moduleId) {};
    
    void recive();
    
    inline void init()
    {
        CommonSerialBegin();
        Serial.println();
        Serial.print("Initialized module ID: ");
        Serial.println(ModuleID);
    }
    
    inline void update()
    {
        uint8_t packetID;
        uint16_t packetWord;

        if (readPacket(packetID, packetWord))
        {
            Serial.write(packetID);
        }
    }
    
    uint16_t ModuleID;
    uint8_t Batteries;
    uint16_t timer;
};