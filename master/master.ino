#include <Common.h>
#include <packet.h>

const uint8_t MAX_MODULES = 12;
uint16_t ModuleSlots = 0;

uint8_t OpenedModule = 0;

uint16_t getResponseTable[GetRequest::MAX_GET_REQUESTS];

inline void setupBomb() {
    
    // temporary for testing the GET/RETURN
    getResponseTable[GetRequest::BATTERIES] = 60;
    getResponseTable[GetRequest::TIMER] = 62;
    
    //for (uint8_t module = 0; module < MAX_MODULES; module++)
    runRoutine(Routines::SETUP, 6);
}

inline void openModule(uint8_t moduleSlot)
{
    // to add - close the connection with the previous opened module
    // later change from hardcoded value to work with the shift register
    
    emptyStream(Serial1);
    digitalWrite(6, HIGH);
    delay(1);
}

inline void closeModule()
{
    // later change from hardcoded value to work with the shift register
    digitalWrite(6, LOW);
}

inline bool runRoutine(uint16_t routine, uint8_t moduleSlot)
{
    uint8_t packetID;
    uint16_t packetWord;
    
    openModule(moduleSlot);

    sendPacket(Serial1, PacketID::ROUTINE, routine);
    
    if (!readPacketWait(Serial1, packetID, packetWord))
    {
        closeModule();
        return false;
    }
    
    if (packetID != PacketID::COMMUNICATION || packetWord != Communication::ACK)
    {
        closeModule();
        return false;
    }
    
    while (!(packetID == PacketID::COMMUNICATION && packetWord == Communication::END))
    {
        if (!readPacketWait(Serial1, packetID, packetWord))
        {
            closeModule();
            return false;
        }
        
        processPacket(packetID, packetWord);
    }
    return true;
}

void throwException(uint8_t errorCode)
{
    closeModule();
    
    // to add - exception handling
    
    while(true) {}
}

void setup()
{
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);

    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE);
    
    delay(100);
    setupBomb();
}

void loop()
{
    runRoutine(Routines::UPDATE, 6);
}

// MASTER PACKET PROCESSING

inline void processPacket(uint8_t packetID, uint16_t packetWord)
{
    switch (packetID)
    {
        case PacketID::COMMUNICATION:
        {
            return;
        }
        case PacketID::GET:
        {
            processGetPacket(packetWord);
            return;
        }
        case PacketID::FLAG:
        {
            processFlagPacket(packetWord);
            return;
        }
        case PacketID::EXCEPTION:
        {
            throwException(packetWord);
            return;
        }
        default:
            return;
    }
}

inline void processGetPacket(uint16_t packetWord)
{
    if (packetWord >= GetRequest::MAX_GET_REQUESTS)
    {
        throwException(Exceptions::INVALID_GET);
        return;
    }
    sendPacket(Serial1, PacketID::RETURN, getResponseTable[packetWord]);
}

inline void processFlagPacket(uint16_t packetWord)
{
    sendPacket(Serial1, PacketID::RETURN, 0);
}