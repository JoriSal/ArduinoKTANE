#include <packet.h>
#include <Module.h>

void setup()
{
    Serial.begin(9600);
    Serial.println("");
    Module module(10);
    //module.recive();
}

void loop()
{
    if (Serial.available() >= 3) {
        byte packetID = Serial.read();
        byte lowByte = Serial.read();
        byte highByte = Serial.read();
    
        uint16_t packetWord = (highByte << 8) | lowByte;
    
        Serial.print("ID: ");
        Serial.print(packetID);
        Serial.print(" | Word: ");
        Serial.println(packetWord);
    }
    //sendPacket(0xAB, 0x0FF0);
}

inline void openSocket(uint8_t moduleSlot)
{
}